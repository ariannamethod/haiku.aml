#!/usr/bin/env bash
# Acquire the exact optional Python HAiKU tokenizer, then publish it atomically.
set -euo pipefail

haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_model="$haiku_root/models/haiku_sp.model"
haiku_revision=abb878c52d763b73e5ad7a4d6a68a9ea7a248a39
haiku_url="https://raw.githubusercontent.com/ariannamethod/harmonix/$haiku_revision/haiku/models/haiku_sp.model"
haiku_expected_sha=b8fb56e049977498be0f59586534e67b43b23fd1d7a2efd06cc6c62d9213b942
haiku_expected_size=256534
haiku_mode=fetch
haiku_source=

usage() {
    printf 'Usage: bash %s [--check | --from MODEL_PATH]\n' "$0"
    printf 'Default: fetch the pinned optional tokenizer; --check never downloads.\n'
}
case ${1:-} in
    '') [ "$#" -eq 0 ] || { usage >&2; exit 2; } ;;
    --check) [ "$#" -eq 1 ] || { usage >&2; exit 2; }; haiku_mode=check ;;
    --from) [ "$#" -eq 2 ] || { usage >&2; exit 2; }; haiku_mode=copy; haiku_source=$2 ;;
    --help|-h) usage; exit 0 ;;
    *) usage >&2; exit 2 ;;
esac

if command -v sha256sum >/dev/null 2>&1; then
    haiku_hash=(sha256sum)
elif command -v shasum >/dev/null 2>&1; then
    haiku_hash=(shasum -a 256)
elif command -v openssl >/dev/null 2>&1; then
    haiku_hash=(openssl dgst -sha256)
else
    printf 'Tokenizer verification needs sha256sum, shasum, or openssl.\n' >&2
    exit 1
fi

verify_model() {
    local haiku_file=$1 haiku_size haiku_hash_output haiku_actual_sha
    if [ ! -f "$haiku_file" ]; then
        printf 'Missing tokenizer model: %s\n' "$haiku_file" >&2
        return 1
    fi
    haiku_size=$(wc -c < "$haiku_file") || return 1
    if [ "$haiku_size" -ne "$haiku_expected_size" ]; then
        printf 'Tokenizer size mismatch: expected %s bytes, got %s.\n' \
            "$haiku_expected_size" "$haiku_size" >&2
        return 1
    fi
    haiku_hash_output=$("${haiku_hash[@]}" < "$haiku_file") || return 1
    if [ "${haiku_hash[0]}" = openssl ]; then
        haiku_actual_sha=${haiku_hash_output##* }
    else
        haiku_actual_sha=${haiku_hash_output%% *}
    fi
    if [ "$haiku_actual_sha" != "$haiku_expected_sha" ]; then
        printf 'Tokenizer SHA-256 mismatch: expected %s, got %s.\n' \
            "$haiku_expected_sha" "$haiku_actual_sha" >&2
        return 1
    fi
}

setup_hint() {
    printf 'Set up the optional tokenizer: bash %q\n' "$haiku_root/scripts/setup-tokenizer.sh" >&2
    printf 'Or copy the pinned source asset: bash %q --from /path/to/haiku_sp.model\n' \
        "$haiku_root/scripts/setup-tokenizer.sh" >&2
}

if [ "$haiku_mode" = check ]; then
    if ! verify_model "$haiku_model"; then setup_hint; exit 1; fi
    printf 'Verified tokenizer: %s\n' "$haiku_model"
    exit 0
fi
if [ "$haiku_mode" = fetch ] && verify_model "$haiku_model" >/dev/null 2>&1; then
    printf 'Verified tokenizer: %s\n' "$haiku_model"
    exit 0
fi
if [ -e "$haiku_model" ] && [ ! -f "$haiku_model" ]; then
    printf 'Tokenizer destination must be a regular file: %s\n' "$haiku_model" >&2
    exit 1
fi

mkdir -p "$haiku_root/models"
haiku_temp=$(mktemp "$haiku_root/models/.haiku_sp.model.XXXXXX")
trap 'rm -f "$haiku_temp"' EXIT
trap 'exit 130' INT
trap 'exit 143' TERM
if [ "$haiku_mode" = copy ]; then
    if ! cp -- "$haiku_source" "$haiku_temp"; then setup_hint; exit 1; fi
elif command -v curl >/dev/null 2>&1; then
    if ! curl --fail --location --silent --show-error --proto '=https' \
        --output "$haiku_temp" "$haiku_url"; then
        printf 'Pinned tokenizer download failed.\n' >&2
        setup_hint
        exit 1
    fi
elif command -v wget >/dev/null 2>&1; then
    if ! wget --quiet --https-only --output-document="$haiku_temp" "$haiku_url"; then
        printf 'Pinned tokenizer download failed.\n' >&2
        setup_hint
        exit 1
    fi
else
    printf 'Tokenizer download needs curl or wget.\n' >&2
    setup_hint
    exit 1
fi
if ! verify_model "$haiku_temp"; then setup_hint; exit 1; fi
chmod 644 "$haiku_temp"
mv -f -- "$haiku_temp" "$haiku_model"
printf 'Installed verified tokenizer: %s\n' "$haiku_model"
