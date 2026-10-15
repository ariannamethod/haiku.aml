#!/usr/bin/env bash
# Build real small source histories once, then compare the new exchange path.
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
haiku_work=${HAIKU_EXCHANGE_WORK:-$(mktemp -d "${TMPDIR:-/tmp}/haiku-contact-exchange.XXXXXX")}
mkdir -p "$haiku_work"
test ! -e "$haiku_work/prepared.donor"
haiku_cleanup() {
    haiku_status=$?
    if [ "$haiku_status" -ne 0 ]; then
        for haiku_error in "$haiku_work"/*.err; do
            test ! -f "$haiku_error" || cat "$haiku_error" >&2
        done
        printf 'Exchange gate evidence retained in %s\n' "$haiku_work" >&2
    elif [ "${HAIKU_KEEP_TEST_WORK:-0}" != 1 ]; then
        rm -rf "$haiku_work"
    fi
    exit "$haiku_status"
}
trap haiku_cleanup EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
python3 - "$haiku_root" "$haiku_work" <<'PY'
import json, pathlib, sys
root, work = map(pathlib.Path, sys.argv[1:])
source = 'exchange_fixture_prefix = ' + json.dumps(str(work / 'prepared.')) + '\n'
source += 'IMPORT ' + json.dumps(str(root / 'tests/contact_exchange_prepare.aml')) + '\n'
(work / 'prepare.aml').write_text(source)
for mode in ('interpreted', 'compiled'):
    source = 'exchange_fixture_prefix = ' + json.dumps(str(work / 'prepared.')) + '\n'
    source += 'exchange_test_prefix = ' + json.dumps(str(work / (mode + '.'))) + '\n'
    source += 'IMPORT ' + json.dumps(str(root / 'tests/contact_exchange_life.aml')) + '\n'
    (work / (mode + '.aml')).write_text(source)
PY
if [ -n "${HAIKU_EXCHANGE_PREPARED:-}" ]; then
    # Reuse only genuine saved histories from a completed preparation run.
    # This avoids rebuilding them after a later exchange-only gate failure.
    for haiku_prepared in donor receiver-v3 off receiver capacity-sender; do
        cp "$HAIKU_EXCHANGE_PREPARED/prepared.$haiku_prepared" "$haiku_work/prepared.$haiku_prepared"
    done
    cp "$HAIKU_EXCHANGE_PREPARED/prepare.out" "$haiku_work/prepare.out"
    cp "$HAIKU_EXCHANGE_PREPARED/prepare.err" "$haiku_work/prepare.err"
else
    "$haiku_aml" "$haiku_work/prepare.aml" > "$haiku_work/prepare.out" 2> "$haiku_work/prepare.err"
fi
test ! -s "$haiku_work/prepare.err"
grep -Fxq HAIKU_CONTACT_EXCHANGE_PREPARED_OK "$haiku_work/prepare.out"
for haiku_mode in interpreted compiled; do
    if [ "$haiku_mode" = interpreted ]; then
        "$haiku_aml" "$haiku_work/$haiku_mode.aml" > "$haiku_work/$haiku_mode.out" 2> "$haiku_work/$haiku_mode.err"
    else
        AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/$haiku_mode.aml" --scalar -o "$haiku_work/check" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"
        (cd "$haiku_work/foreign" && ../check) > "$haiku_work/$haiku_mode.out" 2> "$haiku_work/$haiku_mode.err"
    fi
    test ! -s "$haiku_work/$haiku_mode.err"
    grep -Fxq HAIKU_CONTACT_EXCHANGE_LIFE_OK "$haiku_work/$haiku_mode.out"
    cmp "$haiku_work/$haiku_mode.child" "$haiku_work/$haiku_mode.resumed"
    cmp "$haiku_work/$haiku_mode.off-base" "$haiku_work/$haiku_mode.off-control"
done
cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
for haiku_checkpoint in pre-adopt packet peer adopted reused returned thirteen child resumed capacity off-base off-control; do
    cmp "$haiku_work/interpreted.$haiku_checkpoint" "$haiku_work/compiled.$haiku_checkpoint"
done
"$haiku_cc" -std=c11 -O2 -Wall -Wextra -Werror -I"$haiku_include" \
    "$haiku_root/tests/contact_exchange_validation.c" "$haiku_bridge" "$haiku_notorch" "$haiku_lib" \
    "${haiku_link_flags[@]}" -lm -lpthread -o "$haiku_work/validate"
"$haiku_work/validate" "$haiku_root" "$haiku_work"
cat "$haiku_work/prepare.out" "$haiku_work/interpreted.out"
