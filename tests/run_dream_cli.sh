#!/usr/bin/env bash
# Actual default v3 launcher: resume just before the first native dream.
set -euo pipefail
haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-dream-cli.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"

haiku_compile() {
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$1" --scalar -o "$2" \
        > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2; exit 1
    fi
}
haiku_run() {
    local input=$1 output=$2
    shift 2
    if ! (cd "$haiku_work/foreign" && "$@") < "$input" > "$output" 2> "$haiku_work/run.err"; then
        cat "$output" "$haiku_work/run.err" >&2; exit 1
    fi
    if [ -s "$haiku_work/run.err" ]; then cat "$haiku_work/run.err" >&2; exit 1; fi
}
haiku_replies() {
    awk '/^HAiKU:$/ {remaining=3; next} remaining > 0 {print; remaining--}' "$@"
}

for haiku_case in full compiled split upgrade-v1 upgrade-v2 blank; do
    mkdir -p "$haiku_work/$haiku_case"
    ln -s "$haiku_root/src" "$haiku_work/$haiku_case/src"
    cp "$haiku_root/haiku.aml" "$haiku_work/$haiku_case/haiku.aml"
done
for haiku_repeat in 1 2 3; do
    printf '%s\n' rain moon silence presence warmth memory return home
done > "$haiku_work/full.input"
head -n 13 "$haiku_work/full.input" > "$haiku_work/a.input"
tail -n 11 "$haiku_work/full.input" > "$haiku_work/b.input"
printf '\n\t\n QuIt \n' > "$haiku_work/blank.input"

haiku_run "$haiku_work/full.input" "$haiku_work/full.out" "$haiku_aml" "$haiku_work/full/haiku.aml"
haiku_compile "$haiku_work/compiled/haiku.aml" "$haiku_work/compiled/run"
haiku_run "$haiku_work/full.input" "$haiku_work/compiled.out" "$haiku_work/compiled/run"
cmp "$haiku_work/full.out" "$haiku_work/compiled.out"
cmp "$haiku_work/full/haiku.state" "$haiku_work/compiled/haiku.state"

haiku_run "$haiku_work/a.input" "$haiku_work/a.out" "$haiku_aml" "$haiku_work/split/haiku.aml"
cp "$haiku_work/split/haiku.state" "$haiku_work/before-dream.state"
# Saved v3 is authoritative even when new-life defaults request v1.
sed -e 's/^inner_life = 1$/inner_life = 0/' -e 's/^dream_life = 1$/dream_life = 0/' \
    "$haiku_root/haiku.aml" > "$haiku_work/split/haiku.aml"
haiku_compile "$haiku_work/split/haiku.aml" "$haiku_work/split/run"
haiku_run "$haiku_work/b.input" "$haiku_work/b.out" "$haiku_work/split/run"
haiku_replies "$haiku_work/full.out" > "$haiku_work/full.replies"
haiku_replies "$haiku_work/a.out" "$haiku_work/b.out" > "$haiku_work/split.replies"
cmp "$haiku_work/full.replies" "$haiku_work/split.replies"
cmp "$haiku_work/full/haiku.state" "$haiku_work/split/haiku.state"

# Exercise both real upgrade switches against direct AML migration.
printf 'what is love\n' > "$haiku_work/old.input"
printf 'the cloud remembers our words\n' > "$haiku_work/next.input"
for haiku_profile in 1 2; do
    haiku_case="$haiku_work/upgrade-v$haiku_profile"
    haiku_inner=1
    if [ "$haiku_profile" = 1 ]; then haiku_inner=0; fi
    sed -e "s/^inner_life = 1$/inner_life = $haiku_inner/" -e 's/^dream_life = 1$/dream_life = 0/' \
        "$haiku_root/haiku.aml" > "$haiku_case/haiku.aml"
    haiku_run "$haiku_work/old.input" "$haiku_case/old.out" "$haiku_aml" "$haiku_case/haiku.aml"
    cp "$haiku_case/haiku.state" "$haiku_case/old.state"
    sed 's/^upgrade_dreams = 0$/upgrade_dreams = 1/' "$haiku_root/haiku.aml" > "$haiku_case/haiku.aml"
    haiku_run "$haiku_work/next.input" "$haiku_case/next.out" "$haiku_aml" "$haiku_case/haiku.aml"
    cat > "$haiku_case/check.aml" <<EOF
IMPORT "$haiku_root/src/session.aml"
state = haiku_state_load("$haiku_case/old.state", 0)
assert(record_get(state, "version") == $haiku_profile, "legacy CLI profile differs")
state = haiku_state_enable_dreams(state, 0)
haiku_session_turn(state, "the cloud remembers our words", 0)
haiku_state_save(state, "$haiku_case/manual.state", 0)
assert(record_get(state, "phase4_started_turn") == 1, "upgrade fabricated older journal")
assert(haiku_journal_check(state) == 1, "upgrade lost its first experience")
EOF
    haiku_run /dev/null "$haiku_case/check.out" "$haiku_aml" "$haiku_case/check.aml"
    cmp "$haiku_case/manual.state" "$haiku_case/haiku.state"
done

cat > "$haiku_work/check.aml" <<EOF
IMPORT "$haiku_root/src/session.aml"
before = haiku_state_load("$haiku_work/before-dream.state", 0)
assert(record_get(before, "turn") == 13 and record_get(before, "dream_last_turn") == 0, "split did not precede the native dream")
state = haiku_state_load("$haiku_work/full/haiku.state", 0)
assert(record_get(state, "version") == 3 and record_get(state, "turn") == 24, "default CLI profile differs")
assert(record_get(state, "dream_last_turn") == 14 and haiku_dream_check(state) == 1, "native dream changed")
assert(haiku_phase4_check(state) == 24 and haiku_journal_check(state) == 24, "complete journal differs")
assert(list_len(record_get(state, "dream_history")) == 4, "dream conversation lost a voice")
EOF
haiku_run /dev/null "$haiku_work/check.out" "$haiku_aml" "$haiku_work/check.aml"
cp "$haiku_work/full/haiku.state" "$haiku_work/before.state"
haiku_run "$haiku_work/blank.input" "$haiku_work/blank.out" "$haiku_aml" "$haiku_work/full/haiku.aml"
haiku_run /dev/null "$haiku_work/eof.out" "$haiku_aml" "$haiku_work/full/haiku.aml"
cmp "$haiku_work/before.state" "$haiku_work/full/haiku.state"
haiku_run "$haiku_work/blank.input" "$haiku_work/fresh-blank.out" "$haiku_aml" "$haiku_work/blank/haiku.aml"
test ! -e "$haiku_work/blank/haiku.state"

haiku_run /dev/null "$haiku_work/dream.out" "$haiku_aml" "$haiku_root/examples/dream.aml"
cmp "$haiku_root/examples/dream.txt" "$haiku_work/dream.out"
haiku_compile "$haiku_root/examples/dream.aml" "$haiku_work/dream"
haiku_run /dev/null "$haiku_work/dream-compiled.out" "$haiku_work/dream"
cmp "$haiku_work/dream.out" "$haiku_work/dream-compiled.out"
printf 'PASS: default v3 CLI, 24 native turns, dream at14, split-before-dream and compiled checkpoint bytes\n'
printf 'PASS: explicit v1/v2 upgrades, saved profile, blank/quit/EOF and recorded native dream dialogue\n'
