#!/usr/bin/env bash
# Python shard/metric receipts, frozen contexts/features, and exact native restart.
set -euo pipefail
haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-journal.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"

cat > "$haiku_work/journal.aml" <<EOF
IMPORT "$haiku_root/tests/fixtures/journal_reference.aml"
assert(checkpoint_save(state, "$haiku_work/before.state") == 1, "Journal save failed")
restored = checkpoint_load("$haiku_work/before.state")
assert(haiku_journal_check(restored) == 3, "Journal restore lost rows")
assert(checkpoint_save(restored, "$haiku_work/after.state") == 1, "Restored journal save failed")
PRINT "HAIKU_JOURNAL_OK"
EOF
printf 'HAIKU_JOURNAL_OK\n' > "$haiku_work/expected.out"
if ! "$haiku_aml" "$haiku_work/journal.aml" > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"; then
    cat "$haiku_work/interpreted.out" "$haiku_work/interpreted.err" >&2; exit 1
fi
cmp "$haiku_work/expected.out" "$haiku_work/interpreted.out"
test ! -s "$haiku_work/interpreted.err"
cmp "$haiku_work/before.state" "$haiku_work/after.state"
cp "$haiku_work/before.state" "$haiku_work/interpreted.state"
AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/journal.aml" --scalar -o "$haiku_work/check" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"
if ! (cd "$haiku_work/foreign" && ../check) > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"; then
    cat "$haiku_work/compiled.out" "$haiku_work/compiled.err" >&2; exit 1
fi
cmp "$haiku_work/expected.out" "$haiku_work/compiled.out"
test ! -s "$haiku_work/compiled.err"
cmp "$haiku_work/before.state" "$haiku_work/after.state"
cmp "$haiku_work/before.state" "$haiku_work/interpreted.state"

for haiku_path in interpreted compiled; do
    if test "$haiku_path" = interpreted; then
        "$haiku_aml" "$haiku_root/tests/fixtures/journal_encoding.aml"
    else
        AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_root/tests/fixtures/journal_encoding.aml" --scalar -o "$haiku_work/encoding" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"
        (cd "$haiku_work/foreign" && ../encoding)
    fi
done

haiku_rejected=0
for haiku_case in empty prefix leading count count_limit truncated utf8 trailing negative length_zero digit oversized \
    history shape matrix source temperature feature feature_nan rae cloud cloud_fraction cloud_last backwards phase4 boundary reflection system; do
    case "$haiku_case" in
        empty|prefix|leading|count|count_limit|truncated|utf8|trailing|negative|length_zero|digit|oversized)
            printf 'IMPORT "%s/src/journal.aml"\n' "$haiku_root" > "$haiku_work/invalid.aml"
            case "$haiku_case" in
                empty) haiku_key='';;
                prefix) haiku_key='0';;
                leading) haiku_key='00:';;
                count) haiku_key='1:1:a';;
                count_limit) haiku_key='10002:';;
                truncated) haiku_key='3:1:a1:b1:';;
                utf8) haiku_key='3:1:é1:a1:b';;
                trailing) haiku_key='0:trailing';;
                negative) haiku_key='3:-1:x0:0:';;
                length_zero) haiku_key='3:01:a1:b1:c';;
                digit) haiku_key='3:x:a1:b1:c';;
                oversized) haiku_key='3:1048577:';;
            esac
            printf 'haiku_journal_unpack("%s")\n' "$haiku_key" >> "$haiku_work/invalid.aml";;
        *)
            printf 'IMPORT "%s/tests/fixtures/journal_reference.aml"\n' "$haiku_root" > "$haiku_work/invalid.aml"
            case "$haiku_case" in
                history) printf 'list_push(record_get(state, "journal_text"), "partial")\n' >> "$haiku_work/invalid.aml";;
                shape) printf 'record_set(state, "journal_values", zeros(12))\n' >> "$haiku_work/invalid.aml";;
                matrix) printf 'record_set(state, "journal_values", matrix(3, 13, 0))\n' >> "$haiku_work/invalid.aml";;
                source) printf 'list_set(record_get(state, "journal_text"), 0, "unknown")\n' >> "$haiku_work/invalid.aml";;
                temperature) printf 'values = record_get(state, "journal_values")\nvalues[0] = 1.4\n' >> "$haiku_work/invalid.aml";;
                feature) printf 'values = record_get(state, "journal_values")\nvalues[3] = 2\n' >> "$haiku_work/invalid.aml";;
                feature_nan) printf 'values = record_get(state, "journal_values")\nvalues[3] = 1e39 - 1e39\n' >> "$haiku_work/invalid.aml";;
                rae) printf 'values = record_get(state, "journal_values")\nvalues[8] = 0.4\n' >> "$haiku_work/invalid.aml";;
                cloud) printf 'values = record_get(state, "journal_values")\nvalues[2] = -1\n' >> "$haiku_work/invalid.aml";;
                cloud_fraction) printf 'values = record_get(state, "journal_values")\nvalues[2] = 8.5\n' >> "$haiku_work/invalid.aml";;
                cloud_last) printf 'values = record_get(state, "journal_values")\nvalues[28] = 100\n' >> "$haiku_work/invalid.aml";;
                backwards) printf 'values = record_get(state, "journal_values")\nvalues[15] = 7\n' >> "$haiku_work/invalid.aml";;
                phase4) printf 'list_push(record_get(state, "phase4_log"), "extra")\n' >> "$haiku_work/invalid.aml";;
                boundary) printf 'record_set(state, "phase4_started_turn", 2)\n' >> "$haiku_work/invalid.aml";;
                reflection) printf 'record_set(state, "meta_history", list_new())\n' >> "$haiku_work/invalid.aml";;
                system)
                    cat >> "$haiku_work/invalid.aml" <<'EOF'
words = list_new()
cursor = 0
while cursor < 33:
    list_push(words, "word")
    cursor = cursor + 1
list_set(record_get(state, "journal_text"), 2, list_key(words))
EOF
                    ;;
            esac
            printf 'haiku_journal_check(state)\n' >> "$haiku_work/invalid.aml";;
    esac
    printf 'PRINT "INVALID_JOURNAL_ACCEPTED"\n' >> "$haiku_work/invalid.aml"
    if "$haiku_aml" "$haiku_work/invalid.aml" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Invalid journal accepted: %s\n' "$haiku_case" >&2; exit 1
    fi
    grep -Eq 'Haiku .*journal' "$haiku_work/invalid.err"
    test ! -s "$haiku_work/invalid.out"
    AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/invalid.aml" --scalar -o "$haiku_work/invalid" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"
    if (cd "$haiku_work/foreign" && ../invalid) > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Invalid journal accepted after compilation: %s\n' "$haiku_case" >&2; exit 1
    fi
    grep -Eq 'Haiku .*journal' "$haiku_work/invalid.err"
    test ! -s "$haiku_work/invalid.out"
    haiku_rejected=$((haiku_rejected + 1))
done
printf 'PASS: 3 Python shard/metric receipts, 30 frozen features, exact pre-dream contexts and upgrade indices\n'
printf 'PASS: Unicode framing/capacity, detached getters and byte-identical journal restart, interpreted/compiled\n'
printf 'PASS: %s malformed contexts and journal states rejected in both execution paths\n' "$haiku_rejected"
