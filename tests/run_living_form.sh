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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-living-form.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
"$haiku_aml" "$haiku_root/tests/living_form.aml" > "$haiku_work/interpreted.out"
AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_root/tests/living_form.aml" --scalar -o "$haiku_work/check" > "$haiku_work/build.out" 2> "$haiku_work/build.err"
(cd "$haiku_work/foreign" && ../check) > "$haiku_work/compiled.out"
cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
cat "$haiku_work/interpreted.out"
"$haiku_cc" -std=c11 -O2 -Wall -Wextra -Werror -I"$haiku_include" \
    "$haiku_root/tests/living_form_validation.c" "$haiku_bridge" "$haiku_lib" "$haiku_notorch" \
    "${haiku_link_flags[@]}" -lm -lpthread -o "$haiku_work/validate"
"$haiku_work/validate" "$haiku_root"
haiku_count=$("$haiku_work/validate" "$haiku_root" --count)
for ((haiku_case=0; haiku_case<haiku_count; haiku_case++)); do
    "$haiku_work/validate" "$haiku_root" --bad "$haiku_case" > "$haiku_work/bad.aml"
    if "$haiku_aml" "$haiku_work/bad.aml" > "$haiku_work/bad-runner.out" 2> "$haiku_work/bad-runner.err"; then
        printf 'FAIL living form: interpreter accepted invalid fixture %s\n' "$haiku_case" >&2; exit 1
    fi
    cat >> "$haiku_work/bad.aml" <<'AML'
BLOOD MAIN {
#include <stdio.h>
int main(void) { puts("WRONG_C_MAIN"); return 0; }
}
AML
    AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/bad.aml" --scalar -o "$haiku_work/bad" > "$haiku_work/bad-build.out" 2> "$haiku_work/bad-build.err"
    if "$haiku_work/bad" > "$haiku_work/bad-compiled.out" 2> "$haiku_work/bad-compiled.err"; then
        printf 'FAIL living form: compiled program accepted invalid fixture %s\n' "$haiku_case" >&2; exit 1
    fi
    if grep -q WRONG "$haiku_work/bad-runner.out" "$haiku_work/bad-compiled.out"; then
        printf 'FAIL living form: invalid fixture continued execution\n' >&2; exit 1
    fi
done
printf 'PASS living form: %s invalid fixtures stop interpreted and compiled execution\n' "$haiku_count"
