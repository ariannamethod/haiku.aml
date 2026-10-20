#!/usr/bin/env bash
set -euo pipefail
haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-living-pressure.XXXXXX")
trap 'haiku_exit=$?; if (( haiku_exit == 0 )); then rm -rf "$haiku_work"; else printf "Pressure test files retained at %s\n" "$haiku_work" >&2; fi' EXIT
mkdir -p "$haiku_work/prefix/lib"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
export AML_PREFIX="$haiku_work/prefix"
"$haiku_aml" "$haiku_root/tests/living_pressure.aml" > "$haiku_work/interpreted.out"
"$haiku_amlc" "$haiku_root/tests/living_pressure.aml" --scalar -o "$haiku_work/pressure" > "$haiku_work/build.out" 2> "$haiku_work/build.err"
(cd "$haiku_work" && ./pressure) > "$haiku_work/compiled.out"
printf 'HAIKU_LIVING_PRESSURE_OK\n' > "$haiku_work/expected.out"
cmp "$haiku_work/expected.out" "$haiku_work/interpreted.out"
cmp "$haiku_work/expected.out" "$haiku_work/compiled.out"
haiku_rejections=0
haiku_bad_case() {
    local haiku_expected=$1 haiku_call=$2
    cat > "$haiku_work/invalid.aml" <<AML
IMPORT "$haiku_root/src/living_features.aml"
parameters = zeros(57)
candidates = haiku_words("celebration celebration")
user = list_new()
pressure = map_new()
map_set(pressure, "rain", 0.125)
$haiku_call
PRINT "HAIKU_INVALID_ACCEPTED"
AML
    if "$haiku_aml" "$haiku_work/invalid.aml" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Interpreter accepted invalid pressure operation: %s\n' "$haiku_call" >&2; exit 1
    fi
    grep -Fq "$haiku_expected" "$haiku_work/invalid.err" || { cat "$haiku_work/invalid.err" >&2; exit 1; }
    ! grep -q HAIKU_INVALID_ACCEPTED "$haiku_work/invalid.out"
    "$haiku_amlc" "$haiku_work/invalid.aml" --scalar -o "$haiku_work/invalid" > "$haiku_work/build.out" 2> "$haiku_work/build.err"
    if (cd "$haiku_work" && ./invalid) > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Compiled program accepted invalid pressure operation: %s\n' "$haiku_call" >&2; exit 1
    fi
    grep -Fq "$haiku_expected" "$haiku_work/invalid.err" || { cat "$haiku_work/invalid.err" >&2; exit 1; }
    ! grep -q HAIKU_INVALID_ACCEPTED "$haiku_work/invalid.out"
    haiku_rejections=$((haiku_rejections + 1))
}
haiku_bad_case 'pressure exceeds its bound' $'map_set(pressure, "rain", 0.188)\nhaiku_living_word_pressure("rain", pressure)'
haiku_bad_case 'pressure exceeds its bound' $'map_set(pressure, "rain", -0.188)\nhaiku_living_word_pressure("rain", pressure)'
haiku_bad_case 'map value must be a finite scalar' $'map_set(pressure, "rain", 1e39)\nhaiku_living_word_pressure("rain", pressure)'
haiku_bad_case 'depth must be a finite integer' 'haiku_living_rae_pressure(parameters, candidates, user, 1.5, [4], pressure)'
haiku_bad_case 'depth must be in [0, 10]' 'haiku_living_rae_pressure(parameters, candidates, user, 11, [4], pressure)'
haiku_bad_case 'syllables differ' 'haiku_living_rae_pressure(parameters, candidates, user, 3, [3], pressure)'
printf 'PASS: bounded unique-word pressure, independent final scores, changed selection, empty-map parity and all four foreground draw/pending equivalence\n'
printf 'PASS: %s invalid pressure/depth/form operations rejected interpreted and compiled\n' "$haiku_rejections"
