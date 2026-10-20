#!/usr/bin/env bash
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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-living-features.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
export AML_PREFIX="$haiku_work/prefix"
"$haiku_aml" "$haiku_root/tests/living_features.aml" > "$haiku_work/interpreted.out"
"$haiku_amlc" "$haiku_root/tests/living_features.aml" --scalar -o "$haiku_work/features" > "$haiku_work/build.out" 2> "$haiku_work/build.err"
(cd "$haiku_work" && ./features) > "$haiku_work/compiled.out"
printf 'HAIKU_LIVING_FEATURES_OK\n' > "$haiku_work/expected.out"
diff -u "$haiku_work/expected.out" "$haiku_work/interpreted.out"
diff -u "$haiku_work/expected.out" "$haiku_work/compiled.out"
"$haiku_cc" -O2 -Wall -Wextra -Werror -I"$haiku_include" "$haiku_root/tests/mathbrain_unchanged.c" \
    "$haiku_bridge" "$haiku_lib" "$haiku_notorch" -lm -lpthread \
    "${haiku_link_flags[@]}" -o "$haiku_work/unchanged"
haiku_rejections=0
haiku_bad_case() {
    local haiku_expected=$1 haiku_kind=$2 haiku_call=$3
    cat > "$haiku_work/invalid.aml" <<AML
IMPORT "$haiku_root/src/living_features.aml"
parameters = zeros(57)
i = 0
while i < 57:
    parameters[i] = (i - 28) * 0.005
    i = i + 1
state = haiku_${haiku_kind}_state()
map_set(state, "observations", 9)
rows = list_new()
counts = map_new()
user = list_new()
rng = rng_new(42)
candidates = list_new()
list_push(candidates, "rain")
# INVALID_CALL
$haiku_call
PRINT "HAIKU_INVALID_ACCEPTED"
AML
    if "$haiku_aml" "$haiku_work/invalid.aml" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Interpreter accepted invalid living feature operation: %s\n' "$haiku_call" >&2; exit 1
    fi
    if ! grep -Fq "$haiku_expected" "$haiku_work/invalid.err"; then cat "$haiku_work/invalid.err" >&2; exit 1; fi
    if grep -q HAIKU_INVALID_ACCEPTED "$haiku_work/invalid.out"; then exit 1; fi
    "$haiku_amlc" "$haiku_work/invalid.aml" --scalar -o "$haiku_work/invalid" > "$haiku_work/build.out" 2> "$haiku_work/build.err"
    if (cd "$haiku_work" && ./invalid) > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Compiled program accepted invalid living feature operation: %s\n' "$haiku_call" >&2; exit 1
    fi
    if ! grep -Fq "$haiku_expected" "$haiku_work/invalid.err"; then cat "$haiku_work/invalid.err" >&2; exit 1; fi
    if grep -q HAIKU_INVALID_ACCEPTED "$haiku_work/invalid.out"; then exit 1; fi
    "$haiku_work/unchanged" "$haiku_work/invalid.aml" "$haiku_expected"
    haiku_rejections=$((haiku_rejections + 1))
}
haiku_bad_case 'one to three target lines' mathbrain 'haiku_living_mathbrain_observe(parameters, state, "celebration", rows, counts, user, 0.75, 0)'
haiku_bad_case 'one to three target lines' mathbrain 'haiku_living_mathbrain_observe(parameters, state, "celebration", rows, counts, user, 0.75, [1, 1, 1, 1])'
haiku_bad_case 'integers in [1, 32]' mathbrain 'haiku_living_mathbrain_observe(parameters, state, "celebration", rows, counts, user, 0.75, [0])'
haiku_bad_case 'integers in [1, 32]' mathbrain 'haiku_living_mathbrain_observe(parameters, state, "celebration", rows, counts, user, 0.75, [-1])'
haiku_bad_case 'integers in [1, 32]' mathbrain 'haiku_living_mathbrain_observe(parameters, state, "celebration", rows, counts, user, 0.75, [4.5])'
haiku_bad_case 'integers in [1, 32]' mathbrain 'haiku_living_mathbrain_observe(parameters, state, "celebration", rows, counts, user, 0.75, [33])'
haiku_bad_case 'targets must be finite' mathbrain 'haiku_living_mathbrain_observe(parameters, state, "celebration", rows, counts, user, 0.75, [1e39])'
haiku_bad_case 'line count differs' mathbrain 'haiku_living_mathbrain_observe(parameters, state, "water\nwater", rows, counts, user, 0.75, [4])'
haiku_bad_case 'syllables differ' mathbrain 'haiku_living_mathbrain_observe(parameters, state, "rain", rows, counts, user, 0.75, [4])'
haiku_bad_case 'syllables differ' mathbrain 'haiku_living_mathbrain_observe(parameters, state, "", rows, counts, user, 0.75, [4])'
haiku_bad_case 'syllables differ' mathbrain 'haiku_living_mathbrain_observe(parameters, state, "water\n", rows, counts, user, 0.75, [2, 2])'
haiku_bad_case 'syllables differ' rae 'haiku_living_rae_observe(parameters, state, "rain", user, 0.75, [4])'
haiku_bad_case 'line count differs' rae 'haiku_living_rae_observe(parameters, state, "water\nwater", user, 0.75, [4])'
haiku_bad_case 'syllables differ' rae 'haiku_living_rae_select(parameters, candidates, user, 3, [4])'
printf 'PASS: short-form independent values, one observation, exact 5/7/5 learner and recursive-selection parity in interpreter and compiled execution\n'
printf 'PASS: %s invalid target/line/syllable operations rejected in both paths; learner parameters and owner maps/lists unchanged\n' "$haiku_rejections"
