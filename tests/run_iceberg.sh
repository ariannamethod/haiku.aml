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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-iceberg.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
sha256sum "$haiku_root/weights/haiku1.gguf" "$haiku_root/weights/haiku2.gguf" > "$haiku_work/before.sha"
for haiku_case in life models; do
    sed -e "s|../src/iceberg_session.aml|$haiku_root/src/iceberg_session.aml|" \
        -e "s|../weights/|$haiku_root/weights/|g" \
        "$haiku_root/tests/iceberg_$haiku_case.aml" > "$haiku_work/$haiku_case.aml"
    "$haiku_aml" "$haiku_work/$haiku_case.aml" > "$haiku_work/interpreted.out"
    cp "$haiku_work/iceberg-$haiku_case.state" "$haiku_work/interpreted.state"
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/$haiku_case.aml" --scalar -o "$haiku_work/check" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
        exit 1
    fi
    (cd "$haiku_work/foreign" && ../check) > "$haiku_work/compiled.out"
    cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
    cmp "$haiku_work/interpreted.state" "$haiku_work/iceberg-$haiku_case.state"
    cat "$haiku_work/interpreted.out"
done
"$haiku_cc" -std=c11 -O2 -Wall -Wextra -Werror -I"$haiku_include" \
    "$haiku_root/tests/iceberg_publication.c" "$haiku_bridge" "$haiku_notorch" "$haiku_lib" \
    "${haiku_link_flags[@]}" -lm -lpthread -o "$haiku_work/publication"
"$haiku_work/publication" "$haiku_root" "$haiku_work"
sha256sum -c "$haiku_work/before.sha"
