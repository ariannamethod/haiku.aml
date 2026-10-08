#!/usr/bin/env bash
# A+B must equal A, save, fresh process, restore, B: text and every saved bit.
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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-state.XXXXXX")
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
    haiku_case="$haiku_work/$haiku_variant"
    mkdir -p "$haiku_case"
    cat > "$haiku_case/run.aml" <<EOF
IMPORT "$haiku_root/tests/fixtures/state_turns.aml"
phase = list_get(read_line(), 0)
model = 0
if text_equal("$haiku_mode", "sentencepiece"):
    model = tokenizer_load("$haiku_root/models/haiku_sp.model")
if text_equal(phase, "b"):
    state = haiku_state_new("regex", 0, 1 - $haiku_train, 191, 919)
    haiku_state_restore(state, "$haiku_case/mid.state", model)
    haiku_state_restore(state, "$haiku_case/mid.state", model)
    assert(record_get(state, "train_rae") == $haiku_train, "restored training mode differs")
    assert(text_equal(record_get(state, "tokenizer_mode"), "$haiku_mode"), "restored tokenizer mode differs")
else:
    state = haiku_state_new("$haiku_mode", model, $haiku_train, 575, 57)
    if $haiku_scripted:
        tape = zeros(2049)
        cursor = 1
        while cursor < len(tape):
            draw = cursor * 5 + 3
            tape[cursor] = (draw - floor(draw / 16) * 16) / 16
            cursor = cursor + 1
        record_set(state, "draws", tape)
        map_set(record_get(state, "syllables"), "the", 2)
    state_part_a(state, model)
if text_equal(phase, "a"):
    assert(haiku_state_save(state, "$haiku_case/mid.state", model) == 1, "midpoint save was not durable")
else:
    state_part_b(state, model)
    assert(record_get(state, "turn") == 7, "continuation turn count differs")
    assert(map_get(record_get(state, "mathbrain_state"), "observations") == 7, "MathBrain count differs")
    assert(map_get(record_get(state, "rae_state"), "observations") == 7 * $haiku_train, "RAE count differs")
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
    # Cross the execution paths as well as the process boundary.
    haiku_run a '' "$haiku_case/compiled-a.out" "$haiku_case/run"
    cmp "$haiku_case/mid.state" "$haiku_case/mid-before.state"
    haiku_run b '' "$haiku_case/compiled-b.out" "$haiku_case/run"
    cmp "$haiku_case/full.out" "$haiku_case/compiled-b.out"
    cmp "$haiku_case/full.state" "$haiku_case/split.state"
done

"$haiku_cc" -O2 -Wall -Wextra -Werror -I"$haiku_include" "$haiku_root/tests/state_validation.c" \
    "$haiku_bridge" "$haiku_lib" "$haiku_notorch" -lm -lpthread \
    "${haiku_link_flags[@]}" -o "$haiku_work/validate"
"$haiku_work/validate" "$haiku_root" "$haiku_work" "$haiku_work/regex1/mid.state"

# The actual CLI resumes saved SP/training choices even after its fresh source
# defaults change. This also checks its save-before-display integration.
for haiku_cli in cli-full cli-split cli-failure; do
    mkdir -p "$haiku_work/$haiku_cli"
    ln -s "$haiku_root/src" "$haiku_work/$haiku_cli/src"
    ln -s "$haiku_root/models" "$haiku_work/$haiku_cli/models"
done
sed -e 's/^tokenizer_mode = "regex"$/tokenizer_mode = "sentencepiece"/' \
    -e 's/^train_rae = 0$/train_rae = 1/' "$haiku_root/haiku.aml" > "$haiku_work/cli-full/haiku.aml"
cp "$haiku_work/cli-full/haiku.aml" "$haiku_work/cli-split/haiku.aml"
printf 'what is resonance\nwhere does silence hold meaning\nhow do patterns emerge\nquit\n' > "$haiku_work/cli-full.input"
printf 'what is resonance\nquit\n' > "$haiku_work/cli-a.input"
printf 'where does silence hold meaning\nhow do patterns emerge\nquit\n' > "$haiku_work/cli-b.input"
for haiku_part in full a b; do
    haiku_cli=cli-split
    if [ "$haiku_part" = full ]; then haiku_cli=cli-full; fi
    if [ "$haiku_part" = b ]; then cp "$haiku_root/haiku.aml" "$haiku_work/cli-split/haiku.aml"; fi
    (cd "$haiku_work/foreign" && "$haiku_aml" "$haiku_work/$haiku_cli/haiku.aml") \
        < "$haiku_work/cli-$haiku_part.input" > "$haiku_work/cli-$haiku_part.out" 2> "$haiku_work/cli-$haiku_part.err"
    if [ -s "$haiku_work/cli-$haiku_part.err" ]; then cat "$haiku_work/cli-$haiku_part.err" >&2; exit 1; fi
done
cmp "$haiku_work/cli-full/haiku.state" "$haiku_work/cli-split/haiku.state"
awk '/^HAiKU:$/ {remaining=3; next} remaining > 0 {print; remaining--}' "$haiku_work/cli-full.out" > "$haiku_work/cli-whole-replies.out"
awk '/^HAiKU:$/ {remaining=3; next} remaining > 0 {print; remaining--}' "$haiku_work/cli-a.out" "$haiku_work/cli-b.out" > "$haiku_work/cli-split-replies.out"
cmp "$haiku_work/cli-whole-replies.out" "$haiku_work/cli-split-replies.out"

awk -v destination="$haiku_work/cli-failure/missing/haiku.state" \
    '$0 == "state_path = \"haiku.state\"" {print "state_path = \"" destination "\""; next} {print}' \
    "$haiku_root/haiku.aml" > "$haiku_work/cli-failure/haiku.aml"
"$haiku_cc" -O2 -Wall -Wextra -Werror -I"$haiku_include" "$haiku_root/tests/foreground_state.c" \
    "$haiku_bridge" "$haiku_lib" "$haiku_notorch" -lm -lpthread \
    "${haiku_link_flags[@]}" -o "$haiku_work/cli-owners"
"$haiku_work/cli-owners" "$haiku_work/cli-failure/haiku.aml" 0 0 'temporary create failed' \
    < "$haiku_work/cli-a.input" > "$haiku_work/cli-failure.out"
if grep -q '^HAiKU:$' "$haiku_work/cli-failure.out"; then
    printf 'CLI displayed a response before failed checkpoint commit\n' >&2; exit 1
fi

# A protobuf field unknown to the model reader leaves a valid model with new
# exact bytes. Loading it succeeds; a checkpoint bound to the original refuses it.
cp "$haiku_root/models/haiku_sp.model" "$haiku_work/other.model"
printf '\230\006\001' >> "$haiku_work/other.model"
cat > "$haiku_work/model-mismatch.aml" <<EOF
IMPORT "$haiku_root/src/state.aml"
other = tokenizer_load("$haiku_work/other.model")
haiku_state_load("$haiku_work/sp1/mid.state", other)
PRINT "HAIKU_INVALID_ACCEPTED"
EOF
if "$haiku_aml" "$haiku_work/model-mismatch.aml" > "$haiku_work/mismatch.out" 2> "$haiku_work/mismatch.err"; then
    printf 'Accepted a different tokenizer model identity\n' >&2; exit 1
fi
if ! grep -Fq 'Haiku tokenizer model identity differs' "$haiku_work/mismatch.err"; then
    cat "$haiku_work/mismatch.err" >&2; exit 1
fi
printf 'PASS: seven-turn continuation, regex/SP and RAE0/1, exact native/scripted RNG state, interpreted/compiled\n'
printf 'PASS: text and every checkpoint byte survive a fresh process, repeated restore and model verification\n'
printf 'PASS: actual CLI resumes saved model/training settings and keeps live state/response unpublished after failed save\n'
