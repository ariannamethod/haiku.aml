#!/usr/bin/env bash
# Same utterances, same complete bytes; worker scheduling changes no experience.
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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-owned-tail.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"

haiku_run() {
    local input=$1 output=$2
    shift 2
    if ! (cd "$haiku_work/foreign" && "$@") < "$input" > "$output" 2> "$haiku_work/run.err"; then
        cat "$output" "$haiku_work/run.err" >&2; exit 1
    fi
    if [ -s "$haiku_work/run.err" ]; then cat "$haiku_work/run.err" >&2; exit 1; fi
}
haiku_compile() {
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$1" --scalar -o "$2" \
        > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2; exit 1
    fi
}
haiku_replies() {
    awk '/^HAiKU:$/ {remaining=3; next} remaining > 0 {print; remaining--}' "$@"
}

for haiku_repeat in 1 2 3; do
    printf '%s\n' rain moon silence presence warmth memory return home
done > "$haiku_work/full.input"
head -n 13 "$haiku_work/full.input" > "$haiku_work/a.input"
tail -n 11 "$haiku_work/full.input" > "$haiku_work/b.input"
printf 'what is love\nthe cloud remembers our words\n!!!\nquit\n' > "$haiku_work/short.input"
printf '\n\t\n QuIt \n' > "$haiku_work/blank.input"

for haiku_case in serial worker compiled split; do
    mkdir -p "$haiku_work/$haiku_case"
    ln -s "$haiku_root/src" "$haiku_work/$haiku_case/src"
    cp "$haiku_root/haiku.aml" "$haiku_work/$haiku_case/haiku.aml"
done
sed 's/^owned_tail = 1$/owned_tail = 0/' "$haiku_root/haiku.aml" > "$haiku_work/serial/haiku.aml"
haiku_run "$haiku_work/full.input" "$haiku_work/serial.out" "$haiku_aml" "$haiku_work/serial/haiku.aml"
haiku_run "$haiku_work/full.input" "$haiku_work/worker.out" "$haiku_aml" "$haiku_work/worker/haiku.aml"
cmp "$haiku_work/serial.out" "$haiku_work/worker.out"
cmp "$haiku_work/serial/haiku.state" "$haiku_work/worker/haiku.state"
haiku_compile "$haiku_work/compiled/haiku.aml" "$haiku_work/compiled/run"
haiku_run "$haiku_work/full.input" "$haiku_work/compiled.out" "$haiku_work/compiled/run"
cmp "$haiku_work/serial.out" "$haiku_work/compiled.out"
cmp "$haiku_work/serial/haiku.state" "$haiku_work/compiled/haiku.state"

# Restart immediately before the first native dream, then finish with workers.
haiku_run "$haiku_work/a.input" "$haiku_work/a.out" "$haiku_aml" "$haiku_work/split/haiku.aml"
haiku_run "$haiku_work/b.input" "$haiku_work/b.out" "$haiku_aml" "$haiku_work/split/haiku.aml"
haiku_replies "$haiku_work/worker.out" > "$haiku_work/full.replies"
haiku_replies "$haiku_work/a.out" "$haiku_work/b.out" > "$haiku_work/split.replies"
cmp "$haiku_work/full.replies" "$haiku_work/split.replies"
cmp "$haiku_work/serial/haiku.state" "$haiku_work/split/haiku.state"
cp "$haiku_work/worker/haiku.state" "$haiku_work/before.state"
haiku_run "$haiku_work/blank.input" "$haiku_work/blank.out" "$haiku_aml" "$haiku_work/worker/haiku.aml"
haiku_run /dev/null "$haiku_work/eof.out" "$haiku_aml" "$haiku_work/worker/haiku.aml"
cmp "$haiku_work/before.state" "$haiku_work/worker/haiku.state"

# Old profiles, coupled learning, and the immutable native tokenizer are also
# carried through function-local worker snapshots without schema migration.
for haiku_variant in foreground inner sentencepiece; do
    for haiku_schedule in serial worker; do
        haiku_case="$haiku_work/$haiku_variant-$haiku_schedule"
        mkdir -p "$haiku_case"
        ln -s "$haiku_root/src" "$haiku_case/src"
        ln -s "$haiku_root/models" "$haiku_case/models"
        case "$haiku_variant" in
            foreground) sed -e 's/^inner_life = 1$/inner_life = 0/' -e 's/^train_rae = 0$/train_rae = 1/' "$haiku_work/$haiku_schedule/haiku.aml" > "$haiku_case/haiku.aml";;
            inner) sed 's/^dream_life = 1$/dream_life = 0/' "$haiku_work/$haiku_schedule/haiku.aml" > "$haiku_case/haiku.aml";;
            sentencepiece) sed -e 's/^tokenizer_mode = "regex"$/tokenizer_mode = "sentencepiece"/' -e 's/^train_rae = 0$/train_rae = 1/' "$haiku_work/$haiku_schedule/haiku.aml" > "$haiku_case/haiku.aml";;
        esac
        haiku_run "$haiku_work/short.input" "$haiku_case/out" "$haiku_aml" "$haiku_case/haiku.aml"
    done
    cmp "$haiku_work/$haiku_variant-serial/out" "$haiku_work/$haiku_variant-worker/out"
    cmp "$haiku_work/$haiku_variant-serial/haiku.state" "$haiku_work/$haiku_variant-worker/haiku.state"
done

# These saves resolve beside the imported fixture. Give it a private location.
sed "s|IMPORT \"../../src/session.aml\"|IMPORT \"$haiku_root/src/session.aml\"|" \
    "$haiku_root/tests/fixtures/owned_tail_schedule.aml" > "$haiku_work/schedule.aml"
haiku_run /dev/null "$haiku_work/schedule.out" "$haiku_aml" "$haiku_work/schedule.aml"
cmp "$haiku_work/owned-schedule-serial.state" "$haiku_work/owned-schedule-worker.state"
haiku_compile "$haiku_work/schedule.aml" "$haiku_work/schedule"
haiku_run /dev/null "$haiku_work/schedule-compiled.out" "$haiku_work/schedule"
cmp "$haiku_work/schedule.out" "$haiku_work/schedule-compiled.out"
cmp "$haiku_work/owned-schedule-serial.state" "$haiku_work/owned-schedule-worker.state"

"$haiku_cc" -O2 -Wall -Wextra -Werror -I"$haiku_include" "$haiku_root/tests/owned_tail_state.c" \
    "$haiku_bridge" "$haiku_lib" "$haiku_notorch" -lm -lpthread \
    "${haiku_link_flags[@]}" -o "$haiku_work/owners"
"$haiku_work/owners" "$haiku_root" "$haiku_work" "$haiku_work/serial/haiku.state"
printf 'PASS: 24 serial/worker/compiled turns, exact checkpoint bytes, native dream and restart at13\n'
printf 'PASS: v1/v2 continuation, native tokenizer, coupled learning, blank/quit/EOF and controlled completion\n'
