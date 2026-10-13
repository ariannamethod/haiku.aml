#!/usr/bin/env bash
# Direct Python island-bridge receipts, detached ownership and source-free replay.
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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-phase4.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
printf 'HAIKU_PHASE4_OK\n' > "$haiku_work/expected.out"

for haiku_case in numerical history suggestions native capacity; do
    haiku_fixture="$haiku_root/tests/fixtures/phase4_$haiku_case.aml"
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

"$haiku_cc" -std=c11 -O2 -Wall -Wextra -Werror -I"$haiku_include" \
    "$haiku_root/tests/phase4_validation.c" "$haiku_bridge" "$haiku_notorch" "$haiku_lib" \
    "${haiku_link_flags[@]}" -lm -o "$haiku_work/validate"
"$haiku_work/validate" "$haiku_root" "$haiku_work"
printf 'PASS: Python Phase4 IDs, shared-key cosine, all event/snapshot/edge rows and eight suggestion paths, interpreted/compiled\n'
printf 'PASS: isolated owners, frozen snapshots, unchanged random streams and 40000-scalar metric history\n'
