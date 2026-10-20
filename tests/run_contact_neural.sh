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
for haiku_checkpoint in first bound full resumed friend conflict; do
    cmp "$haiku_work/interpreted.$haiku_checkpoint" "$haiku_work/compiled.$haiku_checkpoint"
done
cat "$haiku_work/interpreted.out"
