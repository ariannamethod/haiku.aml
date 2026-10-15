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
haiku_keep=0
if [[ -n "${HAIKU_CONCEPTS_WORK:-}" ]]; then
    haiku_work=$HAIKU_CONCEPTS_WORK
    haiku_keep=1
    mkdir -p "$haiku_work"
else
    haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-iceberg-concepts.XXXXXX")
fi
trap 'haiku_exit=$?; if (( haiku_exit == 0 && haiku_keep == 0 )); then rm -rf "$haiku_work"; else printf "Concept test files retained at %s\n" "$haiku_work" >&2; fi' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
sha256sum "$haiku_root/weights/haiku1.gguf" "$haiku_root/weights/haiku2.gguf" > "$haiku_work/before.sha"
for haiku_case in concepts concepts_models; do
    sed -e "s|../src/iceberg_session.aml|$haiku_root/src/iceberg_session.aml|" \
        -e "s|../weights/|$haiku_root/weights/|g" \
        "$haiku_root/tests/iceberg_$haiku_case.aml" > "$haiku_work/$haiku_case.aml"
done
printf 'Checking frozen personal snapshots, modes, worker publication and restart\n'
"$haiku_aml" "$haiku_work/concepts.aml" > "$haiku_work/interpreted.out"
cp "$haiku_work/iceberg-concepts.state" "$haiku_work/interpreted.state"
if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/concepts.aml" --scalar -o "$haiku_work/check" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
    cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
    exit 1
fi
# The runtime resolves checkpoint paths from source origin, including binaries
# launched from another directory. An absent output proves the writer ran.
rm "$haiku_work/iceberg-concepts.state"
(cd "$haiku_work/foreign" && ../check) > "$haiku_work/compiled.out"
test -f "$haiku_work/iceberg-concepts.state"
test ! -e "$haiku_work/foreign/iceberg-concepts.state"
cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
cmp "$haiku_work/interpreted.state" "$haiku_work/iceberg-concepts.state"
cat "$haiku_work/interpreted.out"
printf 'Measuring one fixed model history in off, equal and personal modes\n'
"$haiku_aml" "$haiku_work/concepts_models.aml" > "$haiku_work/models.out"
cat "$haiku_work/models.out"
printf 'Checking exact-source concept and admission publication\n'
"$haiku_cc" -std=c11 -O2 -Wall -Wextra -Werror -I"$haiku_include" \
    "$haiku_root/tests/iceberg_concepts_publication.c" "$haiku_bridge" "$haiku_notorch" "$haiku_lib" \
    "${haiku_link_flags[@]}" -lm -lpthread -o "$haiku_work/publication"
"$haiku_work/publication" "$haiku_root" "$haiku_work"
sha256sum -c "$haiku_work/before.sha"
