#!/usr/bin/env bash
# New dreams and explicitly upgraded foreground/inner lives cross a restart.
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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-dream-state.XXXXXX")
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

# profile 0 starts v3; profiles 1/2 migrate their existing v1/v2 after turn 3.
# The shorter declared cooldown makes multiple dream events fit seven turns.
for haiku_profile in 0 1 2; do
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
        haiku_case="$haiku_work/$haiku_profile-$haiku_variant"
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
        tape = zeros(8193)
        cursor = 1
        while cursor < len(tape):
            draw = cursor * 5 + 3
            tape[cursor] = (draw - floor(draw / 16) * 16) / 16
            cursor = cursor + 1
        record_set(state, "draws", tape)
        map_set(record_get(state, "syllables"), "the", 2)
    if $haiku_profile == 2:
        state = haiku_state_enable_inner(state, model)
    if $haiku_profile == 0:
        state = haiku_state_enable_dreams(state, model)
        record_set(state, "dream_config", [2, 1, 4])
    state_part_a(state, model)
if text_equal(phase, "a"):
    assert(haiku_state_save(state, "$haiku_case/mid.state", model) == 1, "dream midpoint save was not durable")
else:
    if $haiku_profile > 0:
        state = haiku_state_enable_dreams(state, model)
        record_set(state, "dream_config", [2, 1, 4])
    state_part_b(state, model)
    assert(record_get(state, "version") == 3 and record_get(state, "turn") == 7, "dream continuation identity differs")
    boundary = 0
    if $haiku_profile > 0:
        boundary = 3
    assert(record_get(state, "phase4_started_turn") == boundary, "dream migration boundary differs")
    assert(haiku_phase4_check(state) == 7 - boundary, "Phase4 retained a different event count")
    assert(haiku_journal_check(state) == 7 - boundary, "journal retained a different event count")
    assert(haiku_dream_check(state) > 0, "continuation did not exercise a dream")
    assert(record_get(state, "dream_last_turn") > boundary, "dream turn did not advance")
    if $haiku_profile == 0 and text_equal("$haiku_variant", "sp1"):
        assert(map_len(record_get(state, "dream_vocab")) > 0, "SentencePiece dreams did not exercise generator-only vocabulary")
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

"$haiku_cc" -O2 -Wall -Wextra -Werror -I"$haiku_include" "$haiku_root/tests/dream_state_validation.c" \
    "$haiku_bridge" "$haiku_lib" "$haiku_notorch" -lm -lpthread \
    "${haiku_link_flags[@]}" -o "$haiku_work/validate-dream"
"$haiku_work/validate-dream" "$haiku_root" "$haiku_work" \
    "$haiku_work/1-regex1/mid.state" "$haiku_work/2-regex1/mid.state" "$haiku_work/0-regex1/full.state"
printf 'PASS: fifteen fresh-v3/migrated-v1/v2 restart variants, regex/SP, RAE0/1, native/scripted draws\n'
printf 'PASS: spoken text and every checkpoint byte agree across interpreter/compiled fresh processes\n'
