#!/usr/bin/env bash
# Exact original token strings/triples in both explicit modes.
set -euo pipefail

haiku_root=$(cd "$(dirname "$0")/.." && pwd)
bash "$haiku_root/scripts/setup-tokenizer.sh" --check
bash "$haiku_root/tests/tokenizer_setup.sh"
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-tokenizer.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT

for haiku_tool in "$haiku_aml" "$haiku_amlc"; do
    if [ ! -x "$haiku_tool" ]; then
        printf 'Missing AML executable: %s\n' "$haiku_tool" >&2
        exit 1
    fi
done
for haiku_archive in "$haiku_lib" "$haiku_bridge" "$haiku_notorch"; do
    if [ ! -f "$haiku_archive" ]; then
        printf 'Missing native archive: %s\n' "$haiku_archive" >&2
        exit 1
    fi
done
mkdir -p "$haiku_work/prefix/lib"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
printf 'HAIKU_TOKENIZER_OK\n' > "$haiku_work/expected.out"

haiku_references=0
for haiku_fixture in "$haiku_root"/tests/fixtures/tokenizer_parity_*.aml \
    "$haiku_root/tests/fixtures/tokenizer_boundaries.aml"; do
    # Both paths run from a foreign CWD: the model belongs to the AML source.
    if ! (cd "$haiku_work" && "$haiku_aml" "$haiku_fixture") \
        > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"; then
        cat "$haiku_work/interpreted.out" "$haiku_work/interpreted.err" >&2
        exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/interpreted.out"
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/tokenizer" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
        exit 1
    fi
    if ! (cd "$haiku_work" && ./tokenizer) \
        > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"; then
        cat "$haiku_work/compiled.out" "$haiku_work/compiled.err" >&2
        exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/compiled.out"
    haiku_count=$(awk '/^# Tokenizer references:/ {print $4}' "$haiku_fixture")
    haiku_references=$((haiku_references + ${haiku_count:-0}))
done

haiku_rejections=0
for haiku_fixture in "$haiku_root"/tests/fixtures/tokenizer_invalid/*.aml; do
    haiku_expected=$(sed -n 's/^# EXPECT: //p' "$haiku_fixture")
    test -n "$haiku_expected"
    if (cd "$haiku_work" && "$haiku_aml" "$haiku_fixture") \
        > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Interpreter accepted invalid tokenizer input: %s\n' "$haiku_fixture" >&2
        exit 1
    fi
    if ! grep -Fq "$haiku_expected" "$haiku_work/invalid.err"; then
        cat "$haiku_work/invalid.err" >&2
        exit 1
    fi
    if grep -q HAIKU_INVALID_ACCEPTED "$haiku_work/invalid.out"; then exit 1; fi
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/invalid" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
        exit 1
    fi
    if (cd "$haiku_work" && ./invalid) \
        > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Compiled AML accepted invalid tokenizer input: %s\n' "$haiku_fixture" >&2
        exit 1
    fi
    if ! grep -Fq "$haiku_expected" "$haiku_work/invalid.err"; then
        cat "$haiku_work/invalid.err" >&2
        exit 1
    fi
    if grep -q HAIKU_INVALID_ACCEPTED "$haiku_work/invalid.out"; then exit 1; fi
    haiku_rejections=$((haiku_rejections + 1))
done
printf 'PASS: %s tokenizer reference fields in interpreter and compiled --scalar\n' "$haiku_references"
printf 'PASS: source-relative original model, exact marker cleanup and Unicode length boundaries\n'
printf 'PASS: %s rejected tokenizer operations, including native whitespace at the generator boundary\n' "$haiku_rejections"
