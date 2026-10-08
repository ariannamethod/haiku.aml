#!/usr/bin/env bash
# Exercise acquisition in isolated directories; the real installed asset stays put.
set -euo pipefail
haiku_root=$(cd "$(dirname "$0")/.." && pwd)
bash "$haiku_root/scripts/setup-tokenizer.sh" --check >/dev/null
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-tokenizer-setup.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
haiku_bash=$BASH
haiku_tool="$haiku_work/repo/scripts/setup-tokenizer.sh"
haiku_model="$haiku_work/repo/models/haiku_sp.model"
mkdir -p "$haiku_work/repo/scripts" "$haiku_work/repo/models" "$haiku_work/repo/tests" "$haiku_work/bin"
cp "$haiku_root/scripts/setup-tokenizer.sh" "$haiku_tool"
cp "$haiku_root/tests/run_tokenizer.sh" "$haiku_work/repo/tests/run_tokenizer.sh"
cp "$haiku_root/models/haiku_sp.model" "$haiku_work/source model"
cp "$haiku_work/source model" "$haiku_work/corrupt model"
printf x | dd of="$haiku_work/corrupt model" bs=1 count=1 conv=notrunc 2>/dev/null

reject() {
    local haiku_expected=$1
    shift
    if "$@" > "$haiku_work/stdout" 2> "$haiku_work/stderr"; then
        printf 'Unexpected setup success: %s\n' "$*" >&2
        exit 1
    fi
    if ! grep -Fq "$haiku_expected" "$haiku_work/stderr"; then
        cat "$haiku_work/stderr" >&2
        exit 1
    fi
}
no_temp() {
    if compgen -G "$haiku_work/repo/models/.haiku_sp.model.*" >/dev/null; then
        printf 'Unpublished tokenizer temporary file remained.\n' >&2
        exit 1
    fi
}

reject 'Set up the optional tokenizer:' "$haiku_bash" "$haiku_tool" --check
reject 'Set up the optional tokenizer:' "$haiku_bash" "$haiku_work/repo/tests/run_tokenizer.sh"
test ! -e "$haiku_model"
mkdir "$haiku_model"
reject 'destination must be a regular file' "$haiku_bash" "$haiku_tool" --from "$haiku_work/source model"
rmdir "$haiku_model"
(cd / && "$haiku_bash" "$haiku_tool" --from "$haiku_work/source model") >/dev/null
cmp "$haiku_work/source model" "$haiku_model"
printf truncated > "$haiku_work/short model"
reject 'size mismatch' "$haiku_bash" "$haiku_tool" --from "$haiku_work/short model"
cmp "$haiku_work/source model" "$haiku_model"
reject 'SHA-256 mismatch' "$haiku_bash" "$haiku_tool" --from "$haiku_work/corrupt model"
cmp "$haiku_work/source model" "$haiku_model"
reject 'Set up the optional tokenizer:' "$haiku_bash" "$haiku_tool" --from "$haiku_work/absent"
cmp "$haiku_work/source model" "$haiku_model"
no_temp

# Limit PATH to test each supported digest utility without platform assumptions.
for haiku_name in dirname wc mkdir mktemp cp rm chmod mv grep cat; do
    ln -s "$(command -v "$haiku_name")" "$haiku_work/bin/$haiku_name"
done
haiku_digest_count=0
for haiku_name in sha256sum shasum openssl; do
    if command -v "$haiku_name" >/dev/null 2>&1; then
        ln -s "$(command -v "$haiku_name")" "$haiku_work/bin/$haiku_name"
        PATH="$haiku_work/bin" "$haiku_bash" "$haiku_tool" --check >/dev/null
        rm "$haiku_work/bin/$haiku_name"
        haiku_digest_count=$((haiku_digest_count + 1))
    fi
done
for haiku_name in sha256sum shasum openssl; do
    if command -v "$haiku_name" >/dev/null 2>&1; then
        ln -s "$(command -v "$haiku_name")" "$haiku_work/bin/$haiku_name"
        break
    fi
done

# Fake downloaders check the pinned URL and publish only the selected fixture.
cat > "$haiku_work/bin/curl" <<'SH'
#!/bin/bash
set -euo pipefail
haiku_output=
haiku_url=
while [ "$#" -gt 0 ]; do
    case $1 in
        --output) shift; haiku_output=$1 ;;
        --output-document=*) haiku_output=${1#*=} ;;
        https://*) haiku_url=$1 ;;
    esac
    shift
done
test "$haiku_url" = https://raw.githubusercontent.com/ariannamethod/harmonix/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku/models/haiku_sp.model
test -n "$haiku_output"
printf 'download\n' >> "$HAIKU_TEST_FETCH_MARKER"
if [ "${HAIKU_TEST_FETCH_FAIL:-0}" = 1 ]; then
    printf partial > "$haiku_output"
    exit 23
fi
cp "$HAIKU_TEST_SOURCE" "$haiku_output"
SH
chmod +x "$haiku_work/bin/curl"
export HAIKU_TEST_SOURCE="$haiku_work/source model"
export HAIKU_TEST_FETCH_MARKER="$haiku_work/downloads"

# A valid install never invokes a downloader.
PATH="$haiku_work/bin" "$haiku_bash" "$haiku_tool" >/dev/null
test ! -e "$HAIKU_TEST_FETCH_MARKER"
cp "$haiku_work/corrupt model" "$haiku_model"
reject 'SHA-256 mismatch' "$haiku_bash" "$haiku_tool" --check
HAIKU_TEST_FETCH_FAIL=1 PATH="$haiku_work/bin" \
    reject 'download failed' "$haiku_bash" "$haiku_tool"
cmp "$haiku_work/corrupt model" "$haiku_model"
no_temp
HAIKU_TEST_SOURCE="$haiku_work/corrupt model" PATH="$haiku_work/bin" \
    reject 'SHA-256 mismatch' "$haiku_bash" "$haiku_tool"
cmp "$haiku_work/corrupt model" "$haiku_model"
no_temp
PATH="$haiku_work/bin" "$haiku_bash" "$haiku_tool" >/dev/null
cmp "$haiku_work/source model" "$haiku_model"

# Wget fallback and failed acquisition with no existing model.
mv "$haiku_work/bin/curl" "$haiku_work/bin/wget"
rm "$haiku_model"
HAIKU_TEST_FETCH_FAIL=1 PATH="$haiku_work/bin" \
    reject 'download failed' "$haiku_bash" "$haiku_tool"
test ! -e "$haiku_model"
no_temp
PATH="$haiku_work/bin" "$haiku_bash" "$haiku_tool" >/dev/null
cmp "$haiku_work/source model" "$haiku_model"
no_temp
printf 'PASS: tokenizer setup copy/download/check, %s digest backends, failure preservation and cleanup\n' \
    "$haiku_digest_count"
