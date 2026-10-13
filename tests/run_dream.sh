#!/usr/bin/env bash
# Pinned Python dialogs and actual native events through both AML paths.
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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-dream.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
printf 'HAIKU_DREAM_OK\n' > "$haiku_work/expected.out"

haiku_compile() {
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$1" --scalar -o "$haiku_work/check" \
        > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2; exit 1
    fi
}
for haiku_fixture in "$haiku_root"/tests/fixtures/dream_*.aml; do
    if [ "${haiku_fixture##*/}" = dream_support.aml ]; then continue; fi
    if ! "$haiku_aml" "$haiku_fixture" > "$haiku_work/out" 2> "$haiku_work/err"; then
        printf '%s\n' "$haiku_fixture" >&2; cat "$haiku_work/out" "$haiku_work/err" >&2; exit 1
    fi
    cmp "$haiku_work/expected.out" "$haiku_work/out"
    test ! -s "$haiku_work/err"
    haiku_compile "$haiku_fixture"
    if ! (cd "$haiku_work/foreign" && ../check) > "$haiku_work/out" 2> "$haiku_work/err"; then
        cat "$haiku_work/out" "$haiku_work/err" >&2; exit 1
    fi
    cmp "$haiku_work/expected.out" "$haiku_work/out"
    test ! -s "$haiku_work/err"
done

for haiku_case in config matrix interval probability exchanges fragment_shape fragment_empty fragment_strip fragment_lines fragment_weight history partial offset count turn last generated empty_dialog empty_weight trigger; do
    cat > "$haiku_work/invalid.aml" <<EOF
IMPORT "$haiku_root/tests/fixtures/dream_support.aml"
state = dream_test_new(1, "regex")
record_set(state, "turn", 10)
record_set(state, "dream_config", [10, 0.25, 2])
haiku_dream_run(state, "rain\ncloud\nwind", 0)
EOF
    case "$haiku_case" in
        config) printf 'record_set(state, "dream_config", zeros(2))\n' >> "$haiku_work/invalid.aml"; haiku_error='requires three values';;
        matrix) printf 'record_set(state, "dream_config", matrix(1, 3, 0))\n' >> "$haiku_work/invalid.aml"; haiku_error='must be a vector';;
        interval) printf 'record_set(state, "dream_config", [0, 0.25, 4])\n' >> "$haiku_work/invalid.aml"; haiku_error='interval is invalid';;
        probability) printf 'record_set(state, "dream_config", [10, 1.1, 4])\n' >> "$haiku_work/invalid.aml"; haiku_error='probability is invalid';;
        exchanges) printf 'record_set(state, "dream_config", [10, 0.25, 17])\n' >> "$haiku_work/invalid.aml"; haiku_error='exchange count is invalid';;
        fragment_shape) printf 'record_set(state, "dream_weights", zeros(1))\n' >> "$haiku_work/invalid.aml"; haiku_error='lengths differ';;
        fragment_empty) printf 'list_set(record_get(state, "dream_fragments"), 0, "")\n' >> "$haiku_work/invalid.aml"; haiku_error='fragment text is invalid';;
        fragment_strip) printf 'list_set(record_get(state, "dream_fragments"), 0, " rain")\n' >> "$haiku_work/invalid.aml"; haiku_error='one stripped line';;
        fragment_lines) printf 'list_set(record_get(state, "dream_fragments"), 0, "rain\\ncloud")\n' >> "$haiku_work/invalid.aml"; haiku_error='one stripped line';;
        fragment_weight) printf 'values = record_get(state, "dream_weights")\nvalues[0] = 0.099\n' >> "$haiku_work/invalid.aml"; haiku_error='fragment weight is invalid';;
        history) printf 'list_push(record_get(state, "dream_history"), "extra")\n' >> "$haiku_work/invalid.aml"; haiku_error='dialog/history lengths differ';;
        partial) printf 'record_set(state, "dream_dialogs", zeros(4))\n' >> "$haiku_work/invalid.aml"; haiku_error='complete rows';;
        offset) printf 'values = record_get(state, "dream_dialogs")\nvalues[1] = 1\n' >> "$haiku_work/invalid.aml"; haiku_error='text offset differs';;
        count) printf 'values = record_get(state, "dream_dialogs")\nvalues[2] = 1.5\n' >> "$haiku_work/invalid.aml"; haiku_error='exchange count is invalid';;
        turn) printf 'values = record_get(state, "dream_dialogs")\nvalues[0] = 0\n' >> "$haiku_work/invalid.aml"; haiku_error='turn order is invalid';;
        last) printf 'record_set(state, "dream_last_turn", 9)\n' >> "$haiku_work/invalid.aml"; haiku_error='last turn differs';;
        generated) printf 'list_set(record_get(state, "dream_history"), 1, "one line")\n' >> "$haiku_work/invalid.aml"; haiku_error='three lines';;
        empty_dialog) printf 'record_set(state, "dream_dialogs", [1])\n' >> "$haiku_work/invalid.aml"; haiku_error='zero sentinel';;
        empty_weight) printf 'record_set(state, "dream_fragments", list_new())\nrecord_set(state, "dream_weights", [1])\n' >> "$haiku_work/invalid.aml"; haiku_error='zero sentinel';;
        trigger) printf 'event = map_new()\nmap_set(event, "turn", 20)\nmap_set(event, "quality_after", -1)\nmap_set(event, "novelty", 0)\nmap_set(event, "dissonance", 0.5)\nhaiku_dream_should(state, event)\n' >> "$haiku_work/invalid.aml"; haiku_error='trigger quality is invalid';;
    esac
    printf 'haiku_dream_check(state)\nPRINT "INVALID_DREAM_ACCEPTED"\n' >> "$haiku_work/invalid.aml"
    if "$haiku_aml" "$haiku_work/invalid.aml" > "$haiku_work/out" 2> "$haiku_work/err"; then
        printf 'Invalid dream accepted: %s\n' "$haiku_case" >&2; exit 1
    fi
    if ! grep -Fq "$haiku_error" "$haiku_work/err"; then cat "$haiku_work/err" >&2; exit 1; fi
    haiku_compile "$haiku_work/invalid.aml"
    if (cd "$haiku_work/foreign" && ../check) > "$haiku_work/out" 2> "$haiku_work/err"; then
        printf 'Compiled invalid dream accepted: %s\n' "$haiku_case" >&2; exit 1
    fi
    if ! grep -Fq "$haiku_error" "$haiku_work/err"; then cat "$haiku_work/err" >&2; exit 1; fi
done
"$haiku_cc" -O2 -Wall -Wextra -Werror -I"$haiku_include" "$haiku_root/tests/dream_validation.c" \
    "$haiku_bridge" "$haiku_lib" "$haiku_notorch" -lm -lpthread "${haiku_link_flags[@]}" -o "$haiku_work/validate"
"$haiku_work/validate" "$haiku_root" "$haiku_work"
printf 'PASS: 12 Python dream triggers, five full dialog traces, fragments and native shared streams, interpreted/compiled\n'
printf 'PASS: actual-turn cooldown, SentencePiece separator/provenance repair, 20 invalid dream states\n'
