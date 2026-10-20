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
if [[ -n "${HAIKU_NEURAL_WORK:-}" ]]; then
    haiku_work=$HAIKU_NEURAL_WORK
    haiku_keep=1
    mkdir -p "$haiku_work"
else
    haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-iceberg-neural.XXXXXX")
fi
trap 'haiku_exit=$?; if (( haiku_exit == 0 && haiku_keep == 0 )); then rm -rf "$haiku_work"; else printf "Neural association test files retained at %s\n" "$haiku_work" >&2; fi' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign" "$haiku_work/interpreted"
ln -sf "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -sf "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -sf "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
sha256sum "$haiku_root/weights/haiku1.gguf" "$haiku_root/weights/haiku2.gguf" > "$haiku_work/before.sha"
for haiku_case in neural neural_models; do
    sed -e "s|../src/iceberg_session.aml|$haiku_root/src/iceberg_session.aml|" \
        -e "s|../weights/|$haiku_root/weights/|g" \
        "$haiku_root/tests/iceberg_$haiku_case.aml" > "$haiku_work/$haiku_case.aml"
done
printf 'Checking neural activation, disabled and missing-body parity, actual dreams and restart\n'
"$haiku_aml" "$haiku_work/neural.aml" > "$haiku_work/interpreted.out"
grep -Fxq 'HAIKU_ICEBERG_NEURAL_LIFE_OK' "$haiku_work/interpreted.out"
for haiku_state in iceberg-neural-first.state iceberg-neural.state iceberg-neural-resumed.state; do
    cp "$haiku_work/$haiku_state" "$haiku_work/interpreted/"
done
if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/neural.aml" --scalar -o "$haiku_work/check" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
    cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
    exit 1
fi
rm "$haiku_work/iceberg-neural-first.state" "$haiku_work/iceberg-neural.state" "$haiku_work/iceberg-neural-resumed.state"
(cd "$haiku_work/foreign" && ../check) > "$haiku_work/compiled.out"
cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
for haiku_state in "$haiku_work/interpreted/"*.state; do
    cmp "$haiku_state" "$haiku_work/$(basename "$haiku_state")"
done
test ! -e "$haiku_work/foreign/iceberg-neural.state"
cat "$haiku_work/interpreted.out"
printf 'Measuring real sparse encounters, personal bindings and next-turn continuation\n'
"$haiku_aml" "$haiku_work/neural_models.aml" > "$haiku_work/models.out"
grep -Fxq 'HAIKU_ICEBERG_NEURAL_MODELS_OK' "$haiku_work/models.out"
cat "$haiku_work/models.out"
printf 'Checking the same learned history and draws with foreground word pressure off and on\n'
sed "s|../src/iceberg_session.aml|$haiku_root/src/iceberg_session.aml|" \
    "$haiku_root/tests/neural_foreground.aml" > "$haiku_work/neural_foreground.aml"
"$haiku_aml" "$haiku_work/neural_foreground.aml" > "$haiku_work/foreground-interpreted.out"
grep -Fxq 'HAIKU_NEURAL_FOREGROUND_OK' "$haiku_work/foreground-interpreted.out"
if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/neural_foreground.aml" --scalar -o "$haiku_work/foreground" \
    > "$haiku_work/foreground-compile.out" 2> "$haiku_work/foreground-compile.err"; then
    cat "$haiku_work/foreground-compile.out" "$haiku_work/foreground-compile.err" >&2
    exit 1
fi
(cd "$haiku_work/foreign" && ../foreground) > "$haiku_work/foreground-compiled.out"
cmp "$haiku_work/foreground-interpreted.out" "$haiku_work/foreground-compiled.out"
cat "$haiku_work/foreground-interpreted.out"
printf 'Checking exact-source neural publication and refusal ownership\n'
"$haiku_cc" -std=c11 -O2 -Wall -Wextra -Werror -I"$haiku_include" \
    "$haiku_root/tests/iceberg_neural_publication.c" "$haiku_bridge" "$haiku_notorch" "$haiku_lib" \
    "${haiku_link_flags[@]}" -lm -lpthread -o "$haiku_work/publication"
"$haiku_work/publication" "$haiku_root" "$haiku_work"
sha256sum -c "$haiku_work/before.sha"
