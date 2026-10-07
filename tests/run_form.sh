#!/usr/bin/env bash
# English syllables and form through AML, with no Python runtime dependency.
set -euo pipefail

haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-form.XXXXXX")
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
mkdir -p "$haiku_work/prefix/lib"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
printf '[AML] HAIKU_FORM_PARITY_OK\n' > "$haiku_work/expected.out"

haiku_values=0
haiku_bounds=0
for haiku_fixture in "$haiku_root"/tests/fixtures/form_words_*.aml \
    "$haiku_root/tests/fixtures/form_lines.aml" \
    "$haiku_root/tests/fixtures/form_seeds.aml" \
    "$haiku_root/tests/fixtures/form_shapes.aml" \
    "$haiku_root/tests/fixtures/form_limits.aml"; do
    if ! LC_ALL=C awk 'length($0) >= 256 {bad=1} END {exit bad || NR > 500}' "$haiku_fixture"; then
        printf 'Form fixture exceeds the AML source budget: %s\n' "$haiku_fixture" >&2
        exit 1
    fi
    if ! "$haiku_aml" "$haiku_fixture" > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"; then
        cat "$haiku_work/interpreted.out" "$haiku_work/interpreted.err" >&2
        exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/interpreted.out"
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/form" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
        exit 1
    fi
    if ! (cd "$haiku_work" && ./form) > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"; then
        cat "$haiku_work/compiled.out" "$haiku_work/compiled.err" >&2
        exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/compiled.out"
    haiku_count=$(awk '/^# Numeric reference values:/ {print $5}' "$haiku_fixture")
    haiku_values=$((haiku_values + ${haiku_count:-0}))
    haiku_count=$(awk '/^# Boundary values:/ {print $4}' "$haiku_fixture")
    haiku_bounds=$((haiku_bounds + ${haiku_count:-0}))
done

haiku_rejections=0
for haiku_fixture in "$haiku_root"/tests/fixtures/form_invalid/*.aml; do
    haiku_error=$(sed -n 's/^# Expected error: //p' "$haiku_fixture")
    if [ -z "$haiku_error" ]; then exit 1; fi
    if "$haiku_aml" "$haiku_fixture" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Interpreter accepted invalid form input: %s\n' "$haiku_fixture" >&2
        exit 1
    fi
    if ! grep -Fq "$haiku_error" "$haiku_work/invalid.err"; then
        cat "$haiku_work/invalid.err" >&2
        exit 1
    fi
    if grep -q 'INVALID_FORM_ACCEPTED' "$haiku_work/invalid.out"; then exit 1; fi
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/invalid" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
        exit 1
    fi
    if (cd "$haiku_work" && ./invalid) > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Compiled program accepted invalid form input: %s\n' "$haiku_fixture" >&2
        exit 1
    fi
    if ! grep -Fq "$haiku_error" "$haiku_work/invalid.err"; then
        cat "$haiku_work/invalid.err" >&2
        exit 1
    fi
    if grep -q 'INVALID_FORM_ACCEPTED' "$haiku_work/invalid.out"; then exit 1; fi
    haiku_rejections=$((haiku_rejections + 1))
done
printf 'PASS: %s English form reference results and %s boundary checks in AML interpreter and compiled --scalar\n' "$haiku_values" "$haiku_bounds"
printf 'PASS: %s invalid form inputs rejected in both execution paths\n' "$haiku_rejections"
