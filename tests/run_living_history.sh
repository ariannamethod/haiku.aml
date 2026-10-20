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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-living-history.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
"$haiku_aml" "$haiku_root/tests/living_history.aml" > "$haiku_work/interpreted.out"
grep -q '^HAIKU_LIVING_HISTORY_OK$' "$haiku_work/interpreted.out"
AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_root/tests/living_history.aml" --scalar -o "$haiku_work/check" > "$haiku_work/build.out" 2> "$haiku_work/build.err"
(cd "$haiku_work/foreign" && ../check) > "$haiku_work/compiled.out"
cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
cat "$haiku_work/interpreted.out"
"$haiku_cc" -std=c11 -O2 -Wall -Wextra -Werror -I"$haiku_include" \
    "$haiku_root/tests/living_history_validation.c" "$haiku_bridge" "$haiku_lib" "$haiku_notorch" \
    "${haiku_link_flags[@]}" -lm -lpthread -o "$haiku_work/validate"
"$haiku_work/validate" "$haiku_root"
for haiku_case in key changed; do
    printf 'IMPORT "%s/tests/living_history.aml"\n' "$haiku_root" > "$haiku_work/bad.aml"
    if [[ "$haiku_case" == key ]]; then
        printf 'list_key(prior)\n' >> "$haiku_work/bad.aml"
        haiku_diagnostic='composite key limit exceeded'
    else
        cat >> "$haiku_work/bad.aml" <<'AML'
changed = text_concat(text_slice(word, 0, 9999), "y")
list_set(later, 109, changed)
haiku_living_words_prefix(prior, later)
AML
        haiku_diagnostic='changed earlier words'
    fi
    printf 'PRINT "WRONG_HISTORY_CONTINUATION"\n' >> "$haiku_work/bad.aml"
    if "$haiku_aml" "$haiku_work/bad.aml" > "$haiku_work/bad-runner.out" 2> "$haiku_work/bad-runner.err"; then
        printf 'FAIL living history: interpreter accepted %s\n' "$haiku_case" >&2; exit 1
    fi
    AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/bad.aml" --scalar -o "$haiku_work/bad" > "$haiku_work/bad-build.out" 2> "$haiku_work/bad-build.err"
    if "$haiku_work/bad" > "$haiku_work/bad-compiled.out" 2> "$haiku_work/bad-compiled.err"; then
        printf 'FAIL living history: compiled program accepted %s\n' "$haiku_case" >&2; exit 1
    fi
    grep -q "$haiku_diagnostic" "$haiku_work/bad-runner.err"
    grep -q "$haiku_diagnostic" "$haiku_work/bad-compiled.err"
    if grep -q WRONG_HISTORY_CONTINUATION "$haiku_work/bad-runner.out" "$haiku_work/bad-compiled.out"; then
        printf 'FAIL living history: invalid history continued execution\n' >&2; exit 1
    fi
done
printf 'PASS living history: both negative controls stop interpreted and compiled execution\n'
