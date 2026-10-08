#!/usr/bin/env bash
# Real Python reflection receipts and native stream ownership in both AML paths.
set -euo pipefail
haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-meta.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
printf 'HAIKU_META_OK\n' > "$haiku_work/expected.out"

for haiku_fixture in "$haiku_root"/tests/fixtures/metahaiku_bootstrap_*.aml \
    "$haiku_root/tests/fixtures/metahaiku_reflections.aml" \
    "$haiku_root/tests/fixtures/metahaiku_native.aml"; do
    if ! "$haiku_aml" "$haiku_fixture" > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"; then
        cat "$haiku_work/interpreted.out" "$haiku_work/interpreted.err" >&2; exit 1
    fi
    cmp "$haiku_work/expected.out" "$haiku_work/interpreted.out"
    test ! -s "$haiku_work/interpreted.err"
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/check" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2; exit 1
    fi
    if ! (cd "$haiku_work/foreign" && ../check) > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"; then
        cat "$haiku_work/compiled.out" "$haiku_work/compiled.err" >&2; exit 1
    fi
    cmp "$haiku_work/expected.out" "$haiku_work/compiled.out"
    test ! -s "$haiku_work/compiled.err"
done

for haiku_case in history metrics matrix turn climate bootstrap snippet words inner empty full; do
    cat > "$haiku_work/invalid-$haiku_case.aml" <<EOF
IMPORT "$haiku_root/tests/fixtures/metahaiku_support.aml"
state = meta_test_new(0)
observation = zeros(4)
spoken = "two voices\none field\nsoftly"
haiku_meta_reflect(state, "are you here", spoken, observation, 1)
EOF
    case "$haiku_case" in
        history) printf 'list_push(record_get(state, "meta_history"), "partial")\n' >> "$haiku_work/invalid-$haiku_case.aml"; haiku_error='complete rows';;
        metrics) printf 'record_set(state, "meta_metrics", zeros(4))\n' >> "$haiku_work/invalid-$haiku_case.aml"; haiku_error='lengths differ';;
        matrix) printf 'record_set(state, "meta_metrics", matrix(1, 5, 0))\n' >> "$haiku_work/invalid-$haiku_case.aml"; haiku_error='must be a vector';;
        turn) printf 'metrics = record_get(state, "meta_metrics")\nmetrics[0] = 0\n' >> "$haiku_work/invalid-$haiku_case.aml"; haiku_error='turn order';;
        climate) printf 'metrics = record_get(state, "meta_metrics")\nmetrics[4] = 2\n' >> "$haiku_work/invalid-$haiku_case.aml"; haiku_error='outside [0, 1]';;
        bootstrap) printf 'cursor = 0\nwhile cursor < 9:\n    list_push(record_get(state, "meta_bootstrap"), "memory")\n    cursor = cursor + 1\n' >> "$haiku_work/invalid-$haiku_case.aml"; haiku_error='eight snippets';;
        snippet) printf 'text = ""\ncursor = 0\nwhile cursor < 101:\n    text = text_concat(text, "é")\n    cursor = cursor + 1\nlist_push(record_get(state, "meta_bootstrap"), text)\n' >> "$haiku_work/invalid-$haiku_case.aml"; haiku_error='snippet exceeds';;
        words) printf 'list_push(record_get(state, "meta_bootstrap"), "one two three four five six seven eight nine ten eleven")\n' >> "$haiku_work/invalid-$haiku_case.aml"; haiku_error='snippet exceeds';;
        inner) printf 'list_set(record_get(state, "meta_history"), 2, "one line")\n' >> "$haiku_work/invalid-$haiku_case.aml"; haiku_error='three lines';;
        empty) printf 'record_set(state, "meta_history", list_new())\nrecord_set(state, "meta_metrics", zeros(1))\nlist_push(record_get(state, "meta_bootstrap"), "memory")\n' >> "$haiku_work/invalid-$haiku_case.aml"; haiku_error='retained experience';;
        full)
            cat >> "$haiku_work/invalid-$haiku_case.aml" <<'EOF'
record_set(state, "meta_history", list_new())
metrics = zeros(50000)
cursor = 0
while cursor < 10000:
    list_push(record_get(state, "meta_history"), "user")
    list_push(record_get(state, "meta_history"), "spoken")
    list_push(record_get(state, "meta_history"), "a\nb\nc")
    metrics[cursor * 5] = cursor + 1
    cursor = cursor + 1
record_set(state, "meta_metrics", metrics)
haiku_meta_reflect(state, "another", spoken, observation, 10001)
EOF
            haiku_error='history is full';;
    esac
    printf 'haiku_meta_check(state)\nPRINT "INVALID_META_ACCEPTED"\n' >> "$haiku_work/invalid-$haiku_case.aml"
    if "$haiku_aml" "$haiku_work/invalid-$haiku_case.aml" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Invalid reflection state accepted: %s\n' "$haiku_case" >&2; exit 1
    fi
    if ! grep -Fq "$haiku_error" "$haiku_work/invalid.err"; then cat "$haiku_work/invalid.err" >&2; exit 1; fi
    AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/invalid-$haiku_case.aml" --scalar \
        -o "$haiku_work/invalid" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"
    if (cd "$haiku_work/foreign" && ../invalid) > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Invalid reflection state accepted after compilation: %s\n' "$haiku_case" >&2; exit 1
    fi
    if ! grep -Fq "$haiku_error" "$haiku_work/invalid.err"; then cat "$haiku_work/invalid.err" >&2; exit 1; fi
done
printf 'PASS: 23 Python bootstrap cases and four complete inner-voice traces, interpreted/compiled\n'
printf 'PASS: native shared draw order, frozen contexts, unchanged generator/cloud, eleven rejected reflection states\n'
