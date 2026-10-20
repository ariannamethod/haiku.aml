#!/usr/bin/env bash
set -euo pipefail
haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_work=${HAIKU_NEURAL_WORK:-$(mktemp -d "${TMPDIR:-/tmp}/haiku-contact-neural.XXXXXX")}
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
haiku_cleanup() {
    haiku_status=$?
    if [ "$haiku_status" -ne 0 ]; then
        for haiku_error in "$haiku_work"/*.err; do
            test ! -f "$haiku_error" || cat "$haiku_error" >&2
        done
    fi
    if [ -z "${HAIKU_NEURAL_WORK:-}" ]; then rm -rf "$haiku_work"; fi
    exit "$haiku_status"
}
trap haiku_cleanup EXIT
ln -sf "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -sf "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -sf "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
for haiku_mode in interpreted compiled; do
    python3 - "$haiku_root" "$haiku_work" "$haiku_mode" <<'PY'
import json, pathlib, sys
root, work, mode = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2]), sys.argv[3]
source = 'neural_test_prefix = ' + json.dumps(str(work / (mode + '.'))) + '\n'
source += 'IMPORT ' + json.dumps(str(root / 'tests/contact_neural_life.aml')) + '\n'
(work / (mode + '.aml')).write_text(source)
PY
    if [ "$haiku_mode" = interpreted ]; then
        "$haiku_aml" "$haiku_work/$haiku_mode.aml" > "$haiku_work/$haiku_mode.out" 2> "$haiku_work/$haiku_mode.err"
    else
        AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/$haiku_mode.aml" --scalar -o "$haiku_work/check" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"
        (cd "$haiku_work/foreign" && ../check) > "$haiku_work/$haiku_mode.out" 2> "$haiku_work/$haiku_mode.err"
    fi
    test ! -s "$haiku_work/$haiku_mode.err"
    cmp "$haiku_work/$haiku_mode.full" "$haiku_work/$haiku_mode.resumed"
done
cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
for haiku_checkpoint in first bound full resumed friend conflict source_a source_b source_expired; do
    cmp "$haiku_work/interpreted.$haiku_checkpoint" "$haiku_work/compiled.$haiku_checkpoint"
done
haiku_refusals=0
haiku_bad_proof() {
    local haiku_fixture=$1 haiku_diagnostic=$2 haiku_change=$3
    cat > "$haiku_work/invalid.aml" <<AML
IMPORT "$haiku_root/src/parliament_contact.aml"
state = haiku_contact_load("$haiku_work/interpreted.$haiku_fixture")
$haiku_change
haiku_contact_check(state)
PRINT "HAIKU_NEURAL_INVALID_ACCEPTED"
AML
    if "$haiku_aml" "$haiku_work/invalid.aml" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Interpreter accepted a changed neural proof: %s\n' "$haiku_diagnostic" >&2; exit 1
    fi
    grep -Fq "$haiku_diagnostic" "$haiku_work/invalid.err" || { cat "$haiku_work/invalid.err" >&2; exit 1; }
    ! grep -q HAIKU_NEURAL_INVALID_ACCEPTED "$haiku_work/invalid.out"
    AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/invalid.aml" --scalar -o "$haiku_work/invalid" > "$haiku_work/invalid-build.out" 2> "$haiku_work/invalid-build.err"
    if (cd "$haiku_work/foreign" && ../invalid) > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Compiled program accepted a changed neural proof: %s\n' "$haiku_diagnostic" >&2; exit 1
    fi
    grep -Fq "$haiku_diagnostic" "$haiku_work/invalid.err" || { cat "$haiku_work/invalid.err" >&2; exit 1; }
    ! grep -q HAIKU_NEURAL_INVALID_ACCEPTED "$haiku_work/invalid.out"
    haiku_refusals=$((haiku_refusals + 1))
}
haiku_changed_source='list_set(record_get(state, "neural_text"), 24, "stream rain the for of to on")'
haiku_bad_proof source_a 'Neural association published source differs' "$haiku_changed_source"
haiku_bad_proof source_expired 'Neural association retained source differs' "$haiku_changed_source"
haiku_bad_proof source_a 'Neural association retained source differs' "$haiku_changed_source
list_set(record_get(state, \"contact_text\"), 19, \"stream rain the for of to on\")"
haiku_bad_proof source_a 'Neural association absent B has evidence' $'rows = record_get(state, "neural_rows")\nrows[770] = 1e-30'
haiku_bad_proof source_b 'Neural association absent A has evidence' $'rows = record_get(state, "neural_rows")\nrows[766] = 1e-30'
haiku_bad_proof friend 'Neural association retained encounter is absent' $'rows = record_get(state, "neural_rows")\nrows[0] = 9\nrows[5] = 9\nrows[7] = 9'
cat "$haiku_work/interpreted.out"
printf 'PASS: %s source-provenance and absent-endpoint proof mutations rejected interpreted and compiled\n' "$haiku_refusals"
