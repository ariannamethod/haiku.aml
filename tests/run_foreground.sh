#!/usr/bin/env bash
# Complete exchange receipts, then the actual stdin entrypoint and its owners.
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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-foreground.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
printf 'HAIKU_FOREGROUND_OK\n' > "$haiku_work/expected.out"

for haiku_fixture in "$haiku_root"/tests/fixtures/foreground_turns_*.aml \
    "$haiku_root/tests/fixtures/foreground_coupled.aml" \
    "$haiku_root/tests/fixtures/foreground_gates.aml"; do
    if ! "$haiku_aml" "$haiku_fixture" > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"; then
        cat "$haiku_work/interpreted.out" "$haiku_work/interpreted.err" >&2; exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/interpreted.out"
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/check" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2; exit 1
    fi
    if ! (cd "$haiku_work" && ./check) > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"; then
        cat "$haiku_work/compiled.out" "$haiku_work/compiled.err" >&2; exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/compiled.out"
done

# The complete native transcript is an unedited output of this entrypoint.
(cd "$haiku_work" && "$haiku_aml" "$haiku_root/haiku.aml") \
    < "$haiku_root/examples/foreground.input" > "$haiku_work/chat.out" 2> "$haiku_work/chat.err"
if [ -s "$haiku_work/chat.err" ]; then cat "$haiku_work/chat.err" >&2; exit 1; fi
diff -u "$haiku_root/examples/foreground.txt" "$haiku_work/chat.out"
if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_root/haiku.aml" --scalar \
    -o "$haiku_work/haiku" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
    cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2; exit 1
fi
(cd "$haiku_work" && ./haiku) < "$haiku_root/examples/foreground.input" \
    > "$haiku_work/chat-compiled.out" 2> "$haiku_work/chat-compiled.err"
if [ -s "$haiku_work/chat-compiled.err" ]; then cat "$haiku_work/chat-compiled.err" >&2; exit 1; fi
diff -u "$haiku_work/chat.out" "$haiku_work/chat-compiled.out"

"$haiku_cc" -O2 -Wall -Wextra -Werror -I"$haiku_include" "$haiku_root/tests/foreground_state.c" \
    "$haiku_bridge" "$haiku_lib" "$haiku_notorch" -lm -lpthread \
    "${haiku_link_flags[@]}" -o "$haiku_work/owners"
printf '\n\t\n QuIt \n' > "$haiku_work/empty.input"
"$haiku_work/owners" "$haiku_root/haiku.aml" 0 0 '' < "$haiku_work/empty.input" > "$haiku_work/owners.out"
"$haiku_work/owners" "$haiku_root/haiku.aml" 0 0 '' < /dev/null > "$haiku_work/owners.out"
"$haiku_work/owners" "$haiku_root/haiku.aml" 3 0 '' \
    < "$haiku_root/examples/foreground.input" > "$haiku_work/owners.out"
awk 'BEGIN {for(i=0;i<3336;i++) printf "x "; print ""}' > "$haiku_work/long.input"
"$haiku_work/owners" "$haiku_root/haiku.aml" 0 0 'foreground context limit' \
    < "$haiku_work/long.input" > "$haiku_work/owners.out"

# Alternate source configurations keep their own source-relative src/models.
for haiku_variant in coupled sentencepiece; do
    mkdir -p "$haiku_work/$haiku_variant"
    ln -s "$haiku_root/src" "$haiku_work/$haiku_variant/src"
    ln -s "$haiku_root/models" "$haiku_work/$haiku_variant/models"
done
sed 's/^train_rae = 0$/train_rae = 1/' "$haiku_root/haiku.aml" > "$haiku_work/coupled/haiku.aml"
"$haiku_work/owners" "$haiku_work/coupled/haiku.aml" 3 3 '' \
    < "$haiku_root/examples/foreground.input" > "$haiku_work/owners.out"
sed 's/^tokenizer_mode = "regex"$/tokenizer_mode = "sentencepiece"/' "$haiku_root/haiku.aml" \
    > "$haiku_work/sentencepiece/haiku.aml"
printf 'what is love\nquit\n' > "$haiku_work/sp.input"
(cd "$haiku_work" && "$haiku_aml" "$haiku_work/sentencepiece/haiku.aml") \
    < "$haiku_work/sp.input" > "$haiku_work/sp.out" 2> "$haiku_work/sp.err"
if [ -s "$haiku_work/sp.err" ]; then cat "$haiku_work/sp.err" >&2; exit 1; fi
if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/sentencepiece/haiku.aml" --scalar \
    -o "$haiku_work/sp" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
    cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2; exit 1
fi
(cd "$haiku_work" && ./sp) < "$haiku_work/sp.input" > "$haiku_work/sp-compiled.out" 2> "$haiku_work/sp-compiled.err"
if [ -s "$haiku_work/sp-compiled.err" ]; then cat "$haiku_work/sp-compiled.err" >&2; exit 1; fi
diff -u "$haiku_work/sp.out" "$haiku_work/sp-compiled.out"
"$haiku_work/owners" "$haiku_work/sentencepiece/haiku.aml" 1 0 '' \
    < "$haiku_work/sp.input" > "$haiku_work/owners.out"
printf 'rain\tmoon wind\n' > "$haiku_work/sp-invalid.input"
"$haiku_work/owners" "$haiku_work/sentencepiece/haiku.aml" 0 0 'cannot contain whitespace' \
    < "$haiku_work/sp-invalid.input" > "$haiku_work/owners.out"
printf '!!!\nquit\n' > "$haiku_work/punctuation.input"
"$haiku_work/owners" "$haiku_root/haiku.aml" 1 0 '' \
    < "$haiku_work/punctuation.input" > "$haiku_work/owners.out"

printf 'PASS: seven Python foreground turns + explicit punctuation repair, complete states/parameters/draws, both execution paths\n'
printf 'PASS: real stdin transcript, blank/quit/EOF state preservation, mode0/mode1 learning, source-relative native model\n'
printf 'PASS: oversized contexts and whitespace-bearing SP pieces rejected before accepting a turn\n'
