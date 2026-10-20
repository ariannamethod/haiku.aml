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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-living-session.XXXXXX")
trap 'haiku_exit=$?; if (( haiku_exit == 0 )); then rm -rf "$haiku_work"; else printf "Living form test files retained at %s\n" "$haiku_work" >&2; fi' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign" "$haiku_work/interpreted"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
sed "s|../src/iceberg_session.aml|$haiku_root/src/iceberg_session.aml|" \
    "$haiku_root/tests/living_session.aml" > "$haiku_work/living_session.aml"
haiku_run() {
    local output=$1
    shift
    if ! (cd "$haiku_work/foreign" && "$@") > "$output" 2> "$haiku_work/run.err"; then
        cat "$output" "$haiku_work/run.err" >&2
        exit 1
    fi
    if [[ -s "$haiku_work/run.err" ]]; then cat "$haiku_work/run.err" >&2; exit 1; fi
    grep -Fxq 'HAIKU_LIVING_SESSION_OK' "$output"
    for haiku_form in fragment haiku couplet tercet; do grep -Fxq "FORM=$haiku_form" "$output"; done
}
haiku_run "$haiku_work/interpreted.out" "$haiku_aml" "$haiku_work/living_session.aml"
cp "$haiku_work/"*.state "$haiku_work/interpreted/"
if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/living_session.aml" --scalar -o "$haiku_work/check" \
    > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
    cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
    exit 1
fi
rm "$haiku_work/"*.state
haiku_run "$haiku_work/compiled.out" "$haiku_work/check"
cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
for haiku_state in "$haiku_work/interpreted/"*.state; do
    cmp "$haiku_state" "$haiku_work/$(basename "$haiku_state")"
done
test ! -e "$haiku_work/foreign/living-session.state"
"$haiku_cc" -std=c11 -O2 -Wall -Wextra -Werror -I"$haiku_include" \
    "$haiku_root/tests/living_form_publication.c" "$haiku_bridge" "$haiku_notorch" "$haiku_lib" \
    "${haiku_link_flags[@]}" -lm -lpthread -o "$haiku_work/publication"
"$haiku_work/publication" "$haiku_root" "$haiku_work" | tee "$haiku_work/publication.out"
grep -Fxq 'HAIKU_LIVING_PUBLICATION_OK (14 refusals)' "$haiku_work/publication.out"
cat "$haiku_work/interpreted.out"
printf 'PASS: all four actual speaking forms, serial/worker equality, legacy upgrade, dreams and exact restart bytes\n'
