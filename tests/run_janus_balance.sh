#!/usr/bin/env bash
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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-janus.XXXXXX")
haiku_cleanup() {
    haiku_status=$?
    if [ "$haiku_status" -ne 0 ]; then
        for haiku_error in "$haiku_work"/*.err; do
            test ! -f "$haiku_error" || cat "$haiku_error" >&2
        done
    fi
    rm -rf "$haiku_work"
    exit "$haiku_status"
}
trap haiku_cleanup EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
sha256sum "$haiku_root/weights/haiku1.gguf" "$haiku_root/weights/haiku2.gguf" > "$haiku_work/weights.before"
for haiku_mode in interpreted compiled; do
    python3 - "$haiku_root" "$haiku_work" "$haiku_mode" <<'PY'
import json, pathlib, sys
root, work, mode = map(pathlib.Path, sys.argv[1:])
source = 'janus_test_prefix = ' + json.dumps(str(work / (str(mode) + '.'))) + '\n'
source += 'IMPORT ' + json.dumps(str(root / 'tests/janus_balance_life.aml')) + '\n'
source += 'IMPORT ' + json.dumps(str(root / 'tests/janus_balance_models.aml')) + '\n'
(work / (str(mode) + '.aml')).write_text(source)
PY
    if [ "$haiku_mode" = interpreted ]; then
        "$haiku_aml" "$haiku_work/$haiku_mode.aml" > "$haiku_work/$haiku_mode.out" 2> "$haiku_work/$haiku_mode.err"
    else
        AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/$haiku_mode.aml" --scalar -o "$haiku_work/check" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"
        (cd "$haiku_work/foreign" && ../check) > "$haiku_work/$haiku_mode.out" 2> "$haiku_work/$haiku_mode.err"
    fi
    test ! -s "$haiku_work/$haiku_mode.err"
    for haiku_pair in pure ignored owner replay peer_replay; do
        cmp "$haiku_work/$haiku_mode.$haiku_pair.before" "$haiku_work/$haiku_mode.$haiku_pair.after"
    done
    cmp "$haiku_work/$haiku_mode.full" "$haiku_work/$haiku_mode.resumed"
done
cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
cmp "$haiku_work/interpreted.full" "$haiku_work/compiled.full"
"$haiku_cc" -std=c11 -O2 -Wall -Wextra -Werror -I"$haiku_include" \
    "$haiku_root/tests/janus_balance_validation.c" "$haiku_bridge" "$haiku_notorch" "$haiku_lib" \
    "${haiku_link_flags[@]}" -lm -lpthread -o "$haiku_work/validate"
"$haiku_work/validate" "$haiku_root" "$haiku_work"
sha256sum "$haiku_root/weights/haiku1.gguf" "$haiku_root/weights/haiku2.gguf" > "$haiku_work/weights.after"
cmp "$haiku_work/weights.before" "$haiku_work/weights.after"
cat "$haiku_work/interpreted.out"
