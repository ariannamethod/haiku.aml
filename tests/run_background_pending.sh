#!/usr/bin/env bash
set -euo pipefail
haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_work=${HAIKU_BACKGROUND_WORK:-$(mktemp -d "${TMPDIR:-/tmp}/haiku-background-pending.XXXXXX")}
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
trap 'haiku_status=$?; if [[ $haiku_status == 0 && -z ${HAIKU_BACKGROUND_WORK:-} ]]; then rm -rf "$haiku_work"; else printf "Background pending files retained at %s\n" "$haiku_work" >&2; fi' EXIT
ln -sf "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -sf "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -sf "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
for haiku_mode in interpreted compiled; do
    python3 - "$haiku_root" "$haiku_work" "$haiku_mode" <<'PY'
import json, pathlib, sys
root, work, mode = pathlib.Path(sys.argv[1]), pathlib.Path(sys.argv[2]), sys.argv[3]
source = 'background_prefix = ' + json.dumps(str(work / (mode + '.'))) + '\n'
source += 'IMPORT ' + json.dumps(str(root / 'tests/background_pending.aml')) + '\n'
(work / (mode + '.aml')).write_text(source)
PY
    if [[ "$haiku_mode" == interpreted ]]; then
        "$haiku_aml" "$haiku_work/$haiku_mode.aml" > "$haiku_work/$haiku_mode.out"
    else
        AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/$haiku_mode.aml" --scalar -o "$haiku_work/check" \
            > "$haiku_work/build.out" 2> "$haiku_work/build.err"
        (cd "$haiku_work/foreign" && ../check) > "$haiku_work/$haiku_mode.out"
    fi
    grep -Fxq 'HAIKU_BACKGROUND_PENDING_OK' "$haiku_work/$haiku_mode.out"
done
cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
for haiku_profile in 1 2 3 4; do
    for haiku_stage in pending complete; do
        cmp "$haiku_work/interpreted.$haiku_profile.$haiku_stage" "$haiku_work/compiled.$haiku_profile.$haiku_stage"
    done
done
haiku_refusals=0
haiku_bad_pending() {
    local haiku_diagnostic=$1 haiku_change=$2
    cat > "$haiku_work/invalid.aml" <<AML
IMPORT "$haiku_root/src/background.aml"
job = checkpoint_load("$haiku_work/interpreted.4.pending")
$haiku_change
haiku_background_check(job, 0)
PRINT "HAIKU_BACKGROUND_INVALID_ACCEPTED"
AML
    if "$haiku_aml" "$haiku_work/invalid.aml" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Interpreter accepted invalid pending work\n' >&2; exit 1
    fi
    grep -Fq "$haiku_diagnostic" "$haiku_work/invalid.err" || { cat "$haiku_work/invalid.err" >&2; exit 1; }
    AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/invalid.aml" --scalar -o "$haiku_work/invalid" \
        > "$haiku_work/invalid-build.out" 2> "$haiku_work/invalid-build.err"
    if (cd "$haiku_work/foreign" && ../invalid) > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Compiled program accepted invalid pending work\n' >&2; exit 1
    fi
    grep -Fq "$haiku_diagnostic" "$haiku_work/invalid.err" || { cat "$haiku_work/invalid.err" >&2; exit 1; }
    haiku_refusals=$((haiku_refusals + 1))
}
haiku_bad_pending 'pending format differs' 'record_set(job, "format", "other-pending")'
haiku_bad_pending 'pending version differs' 'record_set(job, "version", 2)'
haiku_bad_pending 'pending differs from its exact foreground' 'record_set(job, "extra", 1)'
haiku_bad_pending 'pending differs from its exact foreground' 'record_set(job, "experience:output", "another spoken reply")'
haiku_bad_pending 'pending differs from its exact foreground' 'record_set(job, "experience:quality", 0.987)'
haiku_bad_pending 'pending differs from its exact foreground' 'record_set(job, "voice:voice_rng", rng_new(123))'
haiku_bad_pending 'pending differs from its exact foreground' 'record_set(job, "base:voice_rng", rng_new(123))'
haiku_bad_pending 'namespace repeats a field' 'record_set(job, "base:keys", haiku_words("turn turn"))'
cat "$haiku_work/interpreted.out"
printf 'PASS: four complete profiles, worker/recovery equality and eight exact checkpoint files\n'
printf 'PASS: %s changed pending envelopes refused interpreted and compiled\n' "$haiku_refusals"
