#!/usr/bin/env bash
# Exact words, rolling triples and Harmonix observations through native lists.
set -euo pipefail

haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-lexical.XXXXXX")
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
printf '[AML] HAIKU_LEXICAL_PARITY_OK\n' > "$haiku_work/expected.out"

haiku_numeric=0
haiku_lists=0
for haiku_fixture in "$haiku_root"/tests/fixtures/lexical_observe_*.aml \
    "$haiku_root"/tests/fixtures/lexical_words_*.aml \
    "$haiku_root/tests/fixtures/lexical_rolling.aml" \
    "$haiku_root/tests/fixtures/lexical_unique.aml"; do
    if ! LC_ALL=C awk 'length($0) >= 256 {bad=1} END {exit bad || NR > 500}' "$haiku_fixture"; then
        printf 'Lexical fixture exceeds the AML source budget: %s\n' "$haiku_fixture" >&2
        exit 1
    fi
    if ! "$haiku_aml" "$haiku_fixture" > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"; then
        cat "$haiku_work/interpreted.out" "$haiku_work/interpreted.err" >&2
        exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/interpreted.out"
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/lexical" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
        exit 1
    fi
    if ! (cd "$haiku_work" && ./lexical) > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"; then
        cat "$haiku_work/compiled.out" "$haiku_work/compiled.err" >&2
        exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/compiled.out"
    haiku_count=$(awk '/^# Numeric reference values:/ {print $5}' "$haiku_fixture")
    haiku_numeric=$((haiku_numeric + haiku_count))
    haiku_count=$(awk '/^# List reference results:/ {print $5}' "$haiku_fixture")
    haiku_lists=$((haiku_lists + haiku_count))
done

# A partial group is rejected even when the other participant has no triples.
haiku_rejections=0
for haiku_side in user system; do
    for haiku_size in 1 2 4; do
        haiku_fixture="$haiku_work/malformed.aml"
        {
            printf 'IMPORT "%s/src/lexicon.aml"\n' "$haiku_root"
            printf '%s\n' 'user = list_new()' 'system = list_new()'
            for ((haiku_i=0; haiku_i<haiku_size; haiku_i++)); do
                printf 'list_push(%s, "piece")\n' "$haiku_side"
            done
            printf '%s\n' 'observation = haiku_observe_trigrams(user, system)' 'ECHO MALFORMED_ACCEPTED'
        } > "$haiku_fixture"
        if "$haiku_aml" "$haiku_fixture" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
            printf 'Interpreter accepted %s partial group of %s items.\n' "$haiku_side" "$haiku_size" >&2
            exit 1
        fi
        if ! grep -q 'list index out of range' "$haiku_work/invalid.err"; then
            cat "$haiku_work/invalid.err" >&2
            exit 1
        fi
        if grep -q 'MALFORMED_ACCEPTED' "$haiku_work/invalid.out"; then exit 1; fi
        if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
            -o "$haiku_work/invalid" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
            cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
            exit 1
        fi
        if (cd "$haiku_work" && ./invalid) > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
            printf 'Compiled program accepted %s partial group of %s items.\n' "$haiku_side" "$haiku_size" >&2
            exit 1
        fi
        if ! grep -q 'list index out of range' "$haiku_work/invalid.err"; then
            cat "$haiku_work/invalid.err" >&2
            exit 1
        fi
        if grep -q 'MALFORMED_ACCEPTED' "$haiku_work/invalid.out"; then exit 1; fi
        haiku_rejections=$((haiku_rejections + 1))
    done
done
printf 'PASS: %s numerical and %s list reference results in AML interpreter and compiled --scalar\n' "$haiku_numeric" "$haiku_lists"
printf 'PASS: %s malformed triple lists rejected in both execution paths\n' "$haiku_rejections"
