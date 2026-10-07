#!/usr/bin/env bash
# Original Python numerical receipts; the product/test execution remains AML+C.
set -euo pipefail

haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_include=${HAIKU_AML_INCLUDE:-"$haiku_root/../ariannamethod.ai/core"}
haiku_cc=${CC:-cc}
read -r -a haiku_link_flags <<< "${HAIKU_NOTORCH_LDFLAGS:-}"
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-mathbrain.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
for haiku_tool in "$haiku_aml" "$haiku_amlc"; do
    if [ ! -x "$haiku_tool" ]; then printf 'Missing AML executable: %s\n' "$haiku_tool" >&2; exit 1; fi
done
for haiku_archive in "$haiku_lib" "$haiku_bridge" "$haiku_notorch"; do
    if [ ! -f "$haiku_archive" ]; then printf 'Missing numerical archive: %s\n' "$haiku_archive" >&2; exit 1; fi
done
mkdir -p "$haiku_work/prefix/lib"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
printf '[AML] HAIKU_MATHBRAIN_OK\n' > "$haiku_work/expected.out"

haiku_values=0
for haiku_fixture in "$haiku_root"/tests/fixtures/mathbrain_steps_*.aml \
    "$haiku_root"/tests/fixtures/mathbrain_boundary_*.aml \
    "$haiku_root/tests/fixtures/mathbrain_features.aml" \
    "$haiku_root/tests/fixtures/mathbrain_generic.aml" \
    "$haiku_root/tests/fixtures/mathbrain_native.aml"; do
    if ! LC_ALL=C awk 'length($0) >= 256 {bad=1} END {exit bad || NR > 1000}' "$haiku_fixture"; then
        printf 'MathBrain fixture exceeds the AML source budget: %s\n' "$haiku_fixture" >&2; exit 1
    fi
    if ! "$haiku_aml" "$haiku_fixture" > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"; then
        cat "$haiku_work/interpreted.out" "$haiku_work/interpreted.err" >&2; exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/interpreted.out"
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/mathbrain" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2; exit 1
    fi
    if ! (cd "$haiku_work" && ./mathbrain) > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"; then
        cat "$haiku_work/compiled.out" "$haiku_work/compiled.err" >&2; exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/compiled.out"
    haiku_count=$(awk '/^# Numeric reference values:/ {print $5}' "$haiku_fixture")
    haiku_values=$((haiku_values + ${haiku_count:-0}))
done

"$haiku_cc" -O2 -Wall -Wextra -Werror -I"$haiku_include" "$haiku_root/tests/mathbrain_unchanged.c" \
    "$haiku_bridge" "$haiku_lib" "$haiku_notorch" -lm -lpthread \
    "${haiku_link_flags[@]}" -o "$haiku_work/unchanged"
haiku_rejections=0
for haiku_fixture in "$haiku_root"/tests/fixtures/mathbrain_invalid/*.aml; do
    haiku_expected=$(sed -n 's/^# EXPECT: //p' "$haiku_fixture")
    if [ -z "$haiku_expected" ]; then exit 1; fi
    if "$haiku_aml" "$haiku_fixture" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Interpreter accepted invalid MathBrain input: %s\n' "$haiku_fixture" >&2; exit 1
    fi
    if ! grep -Fq "$haiku_expected" "$haiku_work/invalid.err"; then cat "$haiku_work/invalid.err" >&2; exit 1; fi
    if grep -q HAIKU_MATHBRAIN_INVALID_ACCEPTED "$haiku_work/invalid.out"; then exit 1; fi
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/invalid" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2; exit 1
    fi
    if (cd "$haiku_work" && ./invalid) > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Compiled program accepted invalid MathBrain input: %s\n' "$haiku_fixture" >&2; exit 1
    fi
    if ! grep -Fq "$haiku_expected" "$haiku_work/invalid.err"; then cat "$haiku_work/invalid.err" >&2; exit 1; fi
    if grep -q HAIKU_MATHBRAIN_INVALID_ACCEPTED "$haiku_work/invalid.out"; then exit 1; fi
    if ! "$haiku_work/unchanged" "$haiku_fixture" "$haiku_expected" \
        > "$haiku_work/preserved.out" 2> "$haiku_work/preserved.err"; then
        cat "$haiku_work/preserved.out" "$haiku_work/preserved.err" >&2; exit 1
    fi
    haiku_rejections=$((haiku_rejections + 1))
done
printf 'PASS: %s MathBrain/learner reference values, including 24 full updates, in interpreter and compiled --scalar\n' "$haiku_values"
printf 'PASS: owned initialization, staged updates, isolated models, skip semantics and exact observation boundary\n'
printf 'PASS: %s invalid learner operations rejected in both paths; all owner state checked by host\n' "$haiku_rejections"
