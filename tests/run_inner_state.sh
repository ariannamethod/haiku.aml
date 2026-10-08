#!/usr/bin/env bash
# Both a new inner life and an explicit v1 migration survive a fresh process.
set -euo pipefail
haiku_root=$(cd "$(dirname "$0")/.." && pwd)
bash "$haiku_root/scripts/setup-tokenizer.sh" --check
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_include=${HAIKU_AML_INCLUDE:-"$haiku_root/../ariannamethod.ai/core"}
haiku_cc=${CC:-cc}
read -r -a haiku_link_flags <<< "${HAIKU_NOTORCH_LDFLAGS:-}"
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-inner-state.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"

haiku_run() {
    local phase=$1 path=$2 output=$3
    shift 3
    if ! (cd "$haiku_work/foreign" && printf '%s\n' "$phase" | "$@" "$path") > "$output" 2> "$haiku_work/run.err"; then
        cat "$output" "$haiku_work/run.err" >&2; exit 1
    fi
    if [ -s "$haiku_work/run.err" ]; then cat "$haiku_work/run.err" >&2; exit 1; fi
}

for haiku_migrate in 0 1; do
    for haiku_variant in regex0 regex1 sp0 sp1 scripted; do
        haiku_mode=regex
        haiku_train=0
        haiku_scripted=0
        case "$haiku_variant" in
            regex1) haiku_train=1;;
            sp0) haiku_mode=sentencepiece;;
            sp1) haiku_mode=sentencepiece; haiku_train=1;;
            scripted) haiku_train=1; haiku_scripted=1;;
        esac
        haiku_case="$haiku_work/$haiku_migrate-$haiku_variant"
        mkdir -p "$haiku_case"
        cat > "$haiku_case/run.aml" <<EOF
IMPORT "$haiku_root/tests/fixtures/state_turns.aml"
phase = list_get(read_line(), 0)
model = 0
if text_equal("$haiku_mode", "sentencepiece"):
    model = tokenizer_load("$haiku_root/models/haiku_sp.model")
if text_equal(phase, "b"):
    state = haiku_state_load("$haiku_case/mid.state", model)
else:
    state = haiku_state_new("$haiku_mode", model, $haiku_train, 575, 57)
    if $haiku_scripted:
        tape = zeros(4097)
        cursor = 1
        while cursor < len(tape):
            draw = cursor * 5 + 3
            tape[cursor] = (draw - floor(draw / 16) * 16) / 16
            cursor = cursor + 1
        record_set(state, "draws", tape)
        map_set(record_get(state, "syllables"), "the", 2)
    if $haiku_migrate == 0:
        state = haiku_state_enable_inner(state, model)
    state_part_a(state, model)
if text_equal(phase, "a"):
    assert(haiku_state_save(state, "$haiku_case/mid.state", model) == 1, "inner midpoint save was not durable")
else:
    if $haiku_migrate:
        state = haiku_state_enable_inner(state, model)
    state_part_b(state, model)
    assert(record_get(state, "version") == 2 and record_get(state, "turn") == 7, "inner continuation identity differs")
    assert(record_get(state, "inner_started_turn") == 3 * $haiku_migrate, "inner migration boundary differs")
    assert(list_len(record_get(state, "meta_history")) == 3 * (7 - 3 * $haiku_migrate), "inner history count differs")
    assert(map_get(record_get(state, "mathbrain_state"), "observations") == 7, "inner MathBrain count differs")
    assert(map_get(record_get(state, "rae_state"), "observations") == 7 * $haiku_train, "inner RAE count differs")
    if text_equal(phase, "b"):
        haiku_state_save(state, "$haiku_case/split.state", model)
    else:
        haiku_state_save(state, "$haiku_case/full.state", model)
EOF
        haiku_run full "$haiku_case/run.aml" "$haiku_case/full.out" "$haiku_aml"
        haiku_run a "$haiku_case/run.aml" "$haiku_case/a.out" "$haiku_aml"
        test ! -s "$haiku_case/a.out"
        cp "$haiku_case/mid.state" "$haiku_case/mid-before.state"
        haiku_run b "$haiku_case/run.aml" "$haiku_case/split.out" "$haiku_aml"
        cmp "$haiku_case/full.out" "$haiku_case/split.out"
        cmp "$haiku_case/full.state" "$haiku_case/split.state"
        cmp "$haiku_case/mid.state" "$haiku_case/mid-before.state"
        if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_case/run.aml" --scalar \
            -o "$haiku_case/run" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
            cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2; exit 1
        fi
        haiku_run a '' "$haiku_case/compiled-a.out" "$haiku_case/run"
        cmp "$haiku_case/mid.state" "$haiku_case/mid-before.state"
        haiku_run b '' "$haiku_case/compiled-b.out" "$haiku_case/run"
        cmp "$haiku_case/full.out" "$haiku_case/compiled-b.out"
        cmp "$haiku_case/full.state" "$haiku_case/split.state"
    done
done

# A valid custom foreground may have fewer than three cloud words. Reflection
# still runs; rings begin only when the third exact word arrives.
haiku_case="$haiku_work/small-cloud"
mkdir -p "$haiku_case"
cat > "$haiku_case/run.aml" <<EOF
IMPORT "$haiku_root/src/session.aml"
phase = list_get(read_line(), 0)
if text_equal(phase, "b"):
    state = haiku_state_load("$haiku_case/mid.state", 0)
else:
    state = haiku_state_new("regex", 0, 1, 575, 57)
    record_set(state, "weights", map_new())
    record_set(state, "frequencies", map_new())
    record_set(state, "last_used", map_new())
    record_set(state, "origins", list_new())
    words = haiku_words("rain")
    haiku_cloud_seed(record_get(state, "weights"), record_get(state, "frequencies"), record_get(state, "last_used"), record_get(state, "origins"), words, 0)
    record_set(state, "rows", list_new())
    record_set(state, "counts", map_new())
    record_set(state, "vocab", list_new())
    haiku_chain_seed(record_get(state, "rows"), record_get(state, "counts"), record_get(state, "vocab"), words)
    record_set(state, "syllables", haiku_syllable_cache(words))
    state = haiku_state_enable_inner(state, 0)
    haiku_session_turn(state, "rain", 0)
    assert(list_len(record_get(state, "ring_trigrams")) == 0, "one-word cloud unexpectedly ran rings")
    haiku_session_turn(state, "moon", 0)
    assert(map_len(record_get(state, "weights")) == 2, "small cloud did not grow")
    assert(list_len(record_get(state, "ring_trigrams")) == 0, "two-word cloud unexpectedly ran rings")
if text_equal(phase, "a"):
    haiku_state_save(state, "$haiku_case/mid.state", 0)
else:
    PRINT haiku_session_turn(state, "wind", 0)
    assert(map_len(record_get(state, "weights")) == 3, "third word did not enter cloud")
    assert(list_len(record_get(state, "ring_trigrams")) == 45, "three-word cloud did not start rings")
    assert(list_len(record_get(state, "meta_history")) == 9, "small cloud lost reflections")
    if text_equal(phase, "b"):
        haiku_state_save(state, "$haiku_case/split.state", 0)
    else:
        haiku_state_save(state, "$haiku_case/full.state", 0)
EOF
haiku_run full "$haiku_case/run.aml" "$haiku_case/full.out" "$haiku_aml"
haiku_run a "$haiku_case/run.aml" "$haiku_case/a.out" "$haiku_aml"
test ! -s "$haiku_case/a.out"
cp "$haiku_case/mid.state" "$haiku_case/mid-before.state"
haiku_run b "$haiku_case/run.aml" "$haiku_case/split.out" "$haiku_aml"
cmp "$haiku_case/full.out" "$haiku_case/split.out"
cmp "$haiku_case/full.state" "$haiku_case/split.state"
cmp "$haiku_case/mid.state" "$haiku_case/mid-before.state"
if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_case/run.aml" --scalar \
    -o "$haiku_case/run" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
    cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2; exit 1
fi
haiku_run a '' "$haiku_case/compiled-a.out" "$haiku_case/run"
cmp "$haiku_case/mid.state" "$haiku_case/mid-before.state"
haiku_run b '' "$haiku_case/compiled-b.out" "$haiku_case/run"
cmp "$haiku_case/full.out" "$haiku_case/compiled-b.out"
cmp "$haiku_case/full.state" "$haiku_case/split.state"

"$haiku_cc" -O2 -Wall -Wextra -Werror -I"$haiku_include" "$haiku_root/tests/inner_state_validation.c" \
    "$haiku_bridge" "$haiku_lib" "$haiku_notorch" -lm -lpthread \
    "${haiku_link_flags[@]}" -o "$haiku_work/validate-inner"
"$haiku_work/validate-inner" "$haiku_root" "$haiku_work" \
    "$haiku_work/1-regex1/mid.state" "$haiku_work/0-regex1/mid.state"
printf 'PASS: ten fresh-v2/migrated-v1 restart variants, regex/SP, fixed/learning RAE, native/scripted draws\n'
printf 'PASS: spoken text and all 36 checkpoint fields agree across interpreter/compiled process boundaries\n'
printf 'PASS: one/two-word cloud reflection survives restart and enables rings at the third word\n'
