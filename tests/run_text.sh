#!/usr/bin/env bash
# Run the Unicode word/line organ against direct Python reference values.
set -euo pipefail

haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-text.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT

for haiku_tool in "$haiku_aml" "$haiku_amlc"; do
    if [ ! -x "$haiku_tool" ]; then
        printf 'Missing AML executable: %s\n' "$haiku_tool" >&2
        exit 1
    fi
done
if [ ! -f "$haiku_lib" ]; then
    printf 'Missing scalar AML library: %s\n' "$haiku_lib" >&2
    exit 1
fi

haiku_fixture="$haiku_root/tests/fixtures/text.aml"
if ! LC_ALL=C awk 'length($0) >= 256 {bad=1} END {exit bad || NR > 500}' "$haiku_fixture"; then
    printf 'Text fixture exceeds the initial AML source budget.\n' >&2
    exit 1
fi
printf '[AML] HAIKU_TEXT_PARITY_OK\n' > "$haiku_work/expected.out"

if ! "$haiku_aml" "$haiku_fixture" > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"; then
    cat "$haiku_work/interpreted.out" "$haiku_work/interpreted.err" >&2
    exit 1
fi
diff -u "$haiku_work/expected.out" "$haiku_work/interpreted.out"

mkdir -p "$haiku_work/prefix/lib"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
    -o "$haiku_work/text" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
    cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
    exit 1
fi
if ! (cd "$haiku_work" && ./text) > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"; then
    cat "$haiku_work/compiled.out" "$haiku_work/compiled.err" >&2
    exit 1
fi
diff -u "$haiku_work/expected.out" "$haiku_work/compiled.out"
diff -u "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
haiku_checks=$(awk '/^# Reference values:/ {print $4}' "$haiku_fixture")
printf 'PASS: %s Python text reference values in AML interpreter and compiled --scalar\n' "$haiku_checks"
