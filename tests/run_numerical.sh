#!/usr/bin/env bash
# Assemble one AML program while INCLUDE still has separate execution scope.
set -euo pipefail

haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-numerical.XXXXXX")
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

cat "$haiku_root/src/harmonix.aml" "$haiku_root/src/bridges.aml" \
    "$haiku_root/tests/fixtures/numerical.aml" > "$haiku_work/numerical.aml"
if ! awk 'length($0) >= 510 {bad=1} END {exit bad || NR > 500}' "$haiku_work/numerical.aml"; then
    printf 'Assembled fixture exceeds the initial AML source budget.\n' >&2
    exit 1
fi
printf '[AML] HAIKU_NUMERICAL_PARITY_OK\n' > "$haiku_work/expected.out"

if ! "$haiku_aml" "$haiku_work/numerical.aml" > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"; then
    cat "$haiku_work/interpreted.out" "$haiku_work/interpreted.err" >&2
    exit 1
fi
diff -u "$haiku_work/expected.out" "$haiku_work/interpreted.out"

# A temporary installation prefix lets amlc --scalar find the chosen libaml.
mkdir -p "$haiku_work/prefix/lib"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/numerical.aml" --scalar \
    -o "$haiku_work/numerical" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
    cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
    exit 1
fi
if ! "$haiku_work/numerical" > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"; then
    cat "$haiku_work/compiled.out" "$haiku_work/compiled.err" >&2
    exit 1
fi
diff -u "$haiku_work/expected.out" "$haiku_work/compiled.out"
diff -u "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
haiku_checks=$(awk '/case_failures = .*haiku_check\(/ {n++} END {print n+0}' "$haiku_root/tests/fixtures/numerical.aml")
printf 'PASS: %s Python reference values in AML interpreter and compiled --scalar\n' "$haiku_checks"
