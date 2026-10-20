#!/usr/bin/env bash
# Experimental AML organ: independent of stable chat and all model assets.
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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-parliament.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
for haiku_case in vote life memory; do
    haiku_fixture="$haiku_root/tests/parliament_$haiku_case.aml"
    "$haiku_aml" "$haiku_fixture" > "$haiku_work/interpreted.out"
    AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar -o "$haiku_work/check" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"
    (cd "$haiku_work/foreign" && ../check) > "$haiku_work/compiled.out"
    cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
    cat "$haiku_work/interpreted.out"
done
"$haiku_cc" -std=c11 -O2 -Wall -Wextra -Werror -I"$haiku_include" \
    "$haiku_root/tests/parliament_validation.c" "$haiku_bridge" "$haiku_notorch" "$haiku_lib" \
    "${haiku_link_flags[@]}" -lm -o "$haiku_work/validate"
"$haiku_work/validate" "$haiku_root" "$haiku_work"
