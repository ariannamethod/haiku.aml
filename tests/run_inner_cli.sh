#!/usr/bin/env bash
# The real launcher: fresh inner life, saved profile authority, explicit upgrade.
set -euo pipefail
haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-inner-cli.XXXXXX")
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

for haiku_case in full compiled split old-full old-split upgrade blank; do
    mkdir -p "$haiku_work/$haiku_case"
    ln -s "$haiku_root/src" "$haiku_work/$haiku_case/src"
    cp "$haiku_root/haiku.aml" "$haiku_work/$haiku_case/haiku.aml"
done
printf 'what is love\nquit\n' > "$haiku_work/a.input"
printf 'the cloud remembers our words\nbetween us silence\nquit\n' > "$haiku_work/b.input"
printf '\n\t\n QuIt \n' > "$haiku_work/blank.input"

haiku_run "$haiku_root/examples/foreground.input" "$haiku_work/full.out" \
    "$haiku_aml" "$haiku_work/full/haiku.aml"
haiku_compile "$haiku_work/compiled/haiku.aml" "$haiku_work/compiled/run"
haiku_run "$haiku_root/examples/foreground.input" "$haiku_work/compiled.out" "$haiku_work/compiled/run"
cmp "$haiku_work/full.out" "$haiku_work/compiled.out"
cmp "$haiku_work/full/haiku.state" "$haiku_work/compiled/haiku.state"

haiku_run "$haiku_work/a.input" "$haiku_work/a.out" "$haiku_aml" "$haiku_work/split/haiku.aml"
# A resumed v2 life keeps its inner organs even when fresh defaults request v1.
sed 's/^inner_life = 1$/inner_life = 0/' "$haiku_root/haiku.aml" > "$haiku_work/split/haiku.aml"
haiku_compile "$haiku_work/split/haiku.aml" "$haiku_work/split/run"
haiku_run "$haiku_work/b.input" "$haiku_work/b.out" "$haiku_work/split/run"
haiku_replies "$haiku_work/full.out" > "$haiku_work/full.replies"
haiku_replies "$haiku_work/a.out" "$haiku_work/b.out" > "$haiku_work/split.replies"
cmp "$haiku_work/full.replies" "$haiku_work/split.replies"
cmp "$haiku_work/full/haiku.state" "$haiku_work/split/haiku.state"

for haiku_case in old-full old-split upgrade; do
    sed 's/^inner_life = 1$/inner_life = 0/' "$haiku_root/haiku.aml" > "$haiku_work/$haiku_case/haiku.aml"
done
haiku_run "$haiku_root/examples/foreground.input" "$haiku_work/old-full.out" \
    "$haiku_aml" "$haiku_work/old-full/haiku.aml"
haiku_run "$haiku_work/a.input" "$haiku_work/old-a.out" "$haiku_aml" "$haiku_work/old-split/haiku.aml"
cp "$haiku_root/haiku.aml" "$haiku_work/old-split/haiku.aml"
haiku_run "$haiku_work/b.input" "$haiku_work/old-b.out" "$haiku_aml" "$haiku_work/old-split/haiku.aml"
cmp "$haiku_work/old-full/haiku.state" "$haiku_work/old-split/haiku.state"
haiku_replies "$haiku_work/old-full.out" > "$haiku_work/old-full.replies"
haiku_replies "$haiku_work/old-a.out" "$haiku_work/old-b.out" > "$haiku_work/old-split.replies"
cmp "$haiku_work/old-full.replies" "$haiku_work/old-split.replies"

haiku_run "$haiku_work/a.input" "$haiku_work/upgrade-a.out" "$haiku_aml" "$haiku_work/upgrade/haiku.aml"
sed 's/^upgrade_inner = 0$/upgrade_inner = 1/' "$haiku_root/haiku.aml" > "$haiku_work/upgrade/haiku.aml"
haiku_compile "$haiku_work/upgrade/haiku.aml" "$haiku_work/upgrade/run"
haiku_run "$haiku_work/b.input" "$haiku_work/upgrade-b.out" "$haiku_work/upgrade/run"
cat > "$haiku_work/check.aml" <<EOF
IMPORT "$haiku_root/src/session.aml"
state = haiku_state_new("regex", 0, 0, 575, 57)
haiku_session_turn(state, "what is love", 0)
state = haiku_state_enable_inner(state, 0)
haiku_session_turn(state, "the cloud remembers our words", 0)
haiku_session_turn(state, "between us silence", 0)
haiku_state_save(state, "$haiku_work/manual.state", 0)
assert(record_get(state, "inner_started_turn") == 1, "upgrade boundary differs")
assert(list_len(record_get(state, "meta_history")) == 6, "upgrade fabricated history")
loaded = haiku_state_load("$haiku_work/full/haiku.state", 0)
assert(record_get(loaded, "version") == 2, "fresh CLI did not enable inner life")
assert(record_get(loaded, "inner_started_turn") == 0, "fresh CLI boundary differs")
assert(list_len(record_get(loaded, "meta_history")) == 9, "fresh CLI lost reflections")
loaded = haiku_state_load("$haiku_work/old-split/haiku.state", 0)
assert(record_get(loaded, "version") == 1, "resuming silently upgraded v1")
EOF
haiku_run /dev/null "$haiku_work/check.out" "$haiku_aml" "$haiku_work/check.aml"
test ! -s "$haiku_work/check.out"
cmp "$haiku_work/manual.state" "$haiku_work/upgrade/haiku.state"

haiku_run "$haiku_work/blank.input" "$haiku_work/blank.out" "$haiku_aml" "$haiku_work/blank/haiku.aml"
test ! -e "$haiku_work/blank/haiku.state"
haiku_run /dev/null "$haiku_work/eof.out" "$haiku_aml" "$haiku_work/blank/haiku.aml"
test ! -e "$haiku_work/blank/haiku.state"
cp "$haiku_work/full/haiku.state" "$haiku_work/before.state"
haiku_run "$haiku_work/blank.input" "$haiku_work/resumed-blank.out" "$haiku_aml" "$haiku_work/full/haiku.aml"
cmp "$haiku_work/before.state" "$haiku_work/full/haiku.state"

# A visible internal dialogue is recorded from real calls, including ring scores.
haiku_run /dev/null "$haiku_work/inner.out" "$haiku_aml" "$haiku_root/examples/inner.aml"
cmp "$haiku_root/examples/inner.txt" "$haiku_work/inner.out"
haiku_compile "$haiku_root/examples/inner.aml" "$haiku_work/inner"
haiku_run /dev/null "$haiku_work/inner-compiled.out" "$haiku_work/inner"
cmp "$haiku_work/inner.out" "$haiku_work/inner-compiled.out"
haiku_replies "$haiku_work/inner.out" > "$haiku_work/inner.replies"
cmp "$haiku_work/full.replies" "$haiku_work/inner.replies"
printf 'PASS: real v2 CLI, interpreted/compiled, split restart, saved profile authority and explicit v1 upgrade\n'
printf 'PASS: blank/quit/EOF preserve saved life; recorded inner dialogue agrees with actual launcher\n'
