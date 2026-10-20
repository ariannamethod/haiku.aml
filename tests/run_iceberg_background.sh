#!/usr/bin/env bash
set -euo pipefail
haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_work=${HAIKU_BG_WORK:-$(mktemp -d "${TMPDIR:-/tmp}/haiku-iceberg-background.XXXXXX")}
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
trap 'haiku_status=$?; if [ "$haiku_status" -eq 0 ] && [ -z "${HAIKU_BG_WORK:-}" ]; then rm -rf "$haiku_work"; else printf "Background test files retained at %s\n" "$haiku_work" >&2; fi' EXIT
ln -sf "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -sf "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -sf "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
sha256sum "$haiku_root/weights/haiku1.gguf" "$haiku_root/weights/haiku2.gguf" > "$haiku_work/before.sha"
for haiku_mode in interpreted compiled; do
    python3 - "$haiku_root" "$haiku_work" "$haiku_mode" <<'PY'
import json, os, pathlib, sys
root, work, mode = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2]), sys.argv[3]
base, expected = os.environ.get('HAIKU_BG_BASE', ''), os.environ.get('HAIKU_BG_EXPECTED', '')
assert bool(base) == bool(expected), 'Provide both HAIKU_BG_BASE and HAIKU_BG_EXPECTED'
values = dict(bg_base_path=base, bg_expected_path=expected,
              bg_source=os.environ.get('HAIKU_BG_SOURCE', 'the stream rain'),
              bg_day=int(os.environ.get('HAIKU_BG_DAY', '20747')),
              bg_prefix=str(work / (mode + '.')))
source = ''.join(key + ' = ' + json.dumps(value) + '\n' for key, value in values.items())
source += 'IMPORT ' + json.dumps(str(root / 'tests/iceberg_background.aml')) + '\n'
(work / (mode + '.aml')).write_text(source)
PY
    if [ "$haiku_mode" = interpreted ]; then
        "$haiku_aml" "$haiku_work/$haiku_mode.aml" > "$haiku_work/$haiku_mode.out" 2> "$haiku_work/$haiku_mode.err"
    else
        AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/$haiku_mode.aml" --scalar -o "$haiku_work/check" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"
        (cd "$haiku_work/foreign" && ../check) > "$haiku_work/$haiku_mode.out" 2> "$haiku_work/$haiku_mode.err"
    fi
    grep -Fxq HAIKU_ICEBERG_BACKGROUND_OK "$haiku_work/$haiku_mode.out"
    test ! -s "$haiku_work/$haiku_mode.err"
done
cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
cmp "$haiku_work/interpreted.pending" "$haiku_work/compiled.pending"
cmp "$haiku_work/interpreted.completed" "$haiku_work/compiled.completed"
haiku_refusals=0
haiku_bad_pending() {
    local haiku_change=$1 haiku_check=$2
    cat > "$haiku_work/invalid.aml" <<AML
IMPORT "$haiku_root/src/iceberg_background.aml"
a = model_try_load("$haiku_root/weights/haiku1.gguf")
b = model_try_load("$haiku_root/weights/haiku2.gguf")
absent = model_try_load("")
job = checkpoint_load("$haiku_work/interpreted.pending")
$haiku_change
$haiku_check
PRINT "HAIKU_BACKGROUND_INVALID_ACCEPTED"
AML
    if "$haiku_aml" "$haiku_work/invalid.aml" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Interpreter accepted invalid pending\n' >&2; exit 1
    fi
    grep -Fq 'Haiku background pending differs' "$haiku_work/invalid.err"
    AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/invalid.aml" --scalar -o "$haiku_work/invalid" > "$haiku_work/invalid-compile.out" 2> "$haiku_work/invalid-compile.err"
    if (cd "$haiku_work/foreign" && ../invalid) > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Compiled program accepted invalid pending\n' >&2; exit 1
    fi
    grep -Fq 'Haiku background pending differs' "$haiku_work/invalid.err"
    haiku_refusals=$((haiku_refusals + 1))
}
haiku_bad_pending '' 'haiku_iceberg_bg_check(job, 0, absent, b)'
haiku_bad_pending '' 'haiku_iceberg_bg_check(job, 0, a, absent)'
haiku_bad_pending '' 'haiku_iceberg_bg_check(job, 0, b, a)'
haiku_bad_pending 'record_set(job, "request:a_identity", "changed")' 'haiku_iceberg_bg_check(job, 0, a, b)'
haiku_bad_pending 'record_set(job, "experience:output", "changed")' 'haiku_iceberg_bg_check(job, 0, a, b)'
haiku_bad_pending $'draws = record_get(job, "voice:draws")\ndraws[0] = draws[0] + 1' 'haiku_iceberg_bg_check(job, 0, a, b)'
haiku_bad_pending 'record_set(job, "extra", 1)' 'haiku_iceberg_bg_check(job, 0, a, b)'
haiku_bad_pending 'record_set(job, "request:day", record_get(job, "request:day") + 1)' 'haiku_iceberg_bg_check(job, 0, a, b)'
haiku_bad_pending 'record_set(job, "day", record_get(job, "day") + 1)' 'haiku_iceberg_bg_check(job, 0, a, b)'
sha256sum "$haiku_root/weights/haiku1.gguf" "$haiku_root/weights/haiku2.gguf" > "$haiku_work/after.sha"
cmp "$haiku_work/before.sha" "$haiku_work/after.sha"
cat "$haiku_work/interpreted.out"
printf 'PASS: %s pending/provider mutations rejected interpreted and compiled\n' "$haiku_refusals"
