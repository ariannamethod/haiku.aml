#!/usr/bin/env bash
# Original RAE scores, stable selection, full gradients, and complete updates.
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
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-rae.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT

for haiku_tool in "$haiku_aml" "$haiku_amlc"; do
    if [ ! -x "$haiku_tool" ]; then
        printf 'Missing AML executable: %s\n' "$haiku_tool" >&2
        exit 1
    fi
done
for haiku_archive in "$haiku_lib" "$haiku_bridge" "$haiku_notorch"; do
    if [ ! -f "$haiku_archive" ]; then
        printf 'Missing numerical archive: %s\n' "$haiku_archive" >&2
        exit 1
    fi
done
mkdir -p "$haiku_work/prefix/lib"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
printf 'HAIKU_RAE_OK\n' > "$haiku_work/expected.out"

haiku_references=0
for haiku_fixture in "$haiku_root/tests/fixtures/rae_features.aml" \
    "$haiku_root"/tests/fixtures/rae_selection_*.aml \
    "$haiku_root"/tests/fixtures/rae_learning_*.aml \
    "$haiku_root"/tests/fixtures/rae_rule_*.aml \
    "$haiku_root/tests/fixtures/rae_boundaries.aml" \
    "$haiku_root/tests/fixtures/rae_generated.aml"; do
    if ! "$haiku_aml" "$haiku_fixture" > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"; then
        cat "$haiku_work/interpreted.out" "$haiku_work/interpreted.err" >&2
        exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/interpreted.out"
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/rae" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
        exit 1
    fi
    if ! (cd "$haiku_work" && ./rae) > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"; then
        cat "$haiku_work/compiled.out" "$haiku_work/compiled.err" >&2
        exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/compiled.out"
    haiku_count=$(awk '/^# RAE references:/ {print $4}' "$haiku_fixture")
    haiku_references=$((haiku_references + ${haiku_count:-0}))
done

# Inspect the real AML owners after a rejected operation. The host provides no
# RAE behavior; it compares parameter bytes and existing map/list contents.
cat > "$haiku_work/unchanged.c" <<'C'
#include "ariannamethod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int same_text(const AM_String *a, const AM_String *b) {
    return a && b && a->byte_len == b->byte_len &&
        !memcmp(a->data, b->data, (size_t)a->byte_len);
}
static int same_map(const AM_Map *a, const AM_Map *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; i++)
        if (!same_text(a->entries[i].key, b->entries[i].key) ||
            memcmp(&a->entries[i].value, &b->entries[i].value, sizeof(float))) return 0;
    return 1;
}
static int same_list(const AM_List *a, const AM_List *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; i++)
        if (!same_text(a->items[i], b->items[i])) return 0;
    return 1;
}
static void fail(const char *file, const char *detail) {
    fprintf(stderr, "RAE owner changed or rejection failed: %s: %s\n", file, detail);
    exit(1);
}
int main(int argc, char **argv) {
    if (argc != 3) return 2;
    FILE *file = fopen(argv[1], "rb");
    if (!file || fseek(file, 0, SEEK_END)) return 2;
    long length = ftell(file);
    if (length < 0 || length > 100000 || fseek(file, 0, SEEK_SET)) return 2;
    char *source = malloc((size_t)length + 1);
    if (!source || fread(source, 1, (size_t)length, file) != (size_t)length) return 2;
    fclose(file);
    source[length] = 0;
    const char *marker_text = "# INVALID_CALL\n";
    char *marker = strstr(source, marker_text);
    char *import = strstr(source, "IMPORT ");
    char *end = import ? strchr(import, '\n') : NULL;
    if (!marker || !end || end >= marker) return 2;
    size_t import_length = (size_t)(end - import + 1);
    char *tail = marker + strlen(marker_text);
    char *call = malloc(import_length + strlen(tail) + 1);
    if (!call) return 2;
    memcpy(call, import, import_length);
    strcpy(call + import_length, tail);
    *marker = 0;
    am_init();
    am_use_notorch();
    am_persistent_mode(1);
    if (am_exec_source(source, argv[1])) fail(argv[1], am_get_error());
    int count = 0;
    const float *values = am_get_var_array("params", &count);
    if (!values || count < 1) return 2;
    float *params = malloc((size_t)count * sizeof(float));
    if (!params) return 2;
    memcpy(params, values, (size_t)count * sizeof(float));
    AM_Map *state = am_map_clone(am_get_var_map("state"));
    AM_List *user = am_list_clone(am_get_var_list("user"));
    AM_List *candidates = am_list_clone(am_get_var_list("candidates"));
    if (!state || !user || !candidates) return 2;
    if (!am_exec_source(call, argv[1])) fail(argv[1], "invalid operation succeeded");
    if (!strstr(am_get_error(), argv[2])) fail(argv[1], am_get_error());
    int after_count = 0;
    values = am_get_var_array("params", &after_count);
    if (!values || after_count != count || memcmp(params, values, (size_t)count * sizeof(float)))
        fail(argv[1], "parameter bytes");
    if (!same_map(state, am_get_var_map("state"))) fail(argv[1], "learning state");
    if (!same_list(user, am_get_var_list("user"))) fail(argv[1], "context");
    if (!same_list(candidates, am_get_var_list("candidates"))) fail(argv[1], "candidates");
    am_map_free(state);
    am_list_free(user);
    am_list_free(candidates);
    am_persistent_mode(0);
    free(params);
    free(call);
    free(source);
    return 0;
}
C
"$haiku_cc" -O2 -Wall -Wextra -Werror -I"$haiku_include" "$haiku_work/unchanged.c" \
    "$haiku_bridge" "$haiku_lib" "$haiku_notorch" -lm -lpthread \
    "${haiku_link_flags[@]}" -o "$haiku_work/unchanged"

haiku_rejections=0
for haiku_fixture in "$haiku_root"/tests/fixtures/rae_invalid/*.aml; do
    haiku_expected=$(sed -n 's/^# EXPECT: //p' "$haiku_fixture")
    test -n "$haiku_expected"
    if "$haiku_aml" "$haiku_fixture" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Interpreter accepted invalid RAE input: %s\n' "$haiku_fixture" >&2
        exit 1
    fi
    if ! grep -Fq "$haiku_expected" "$haiku_work/invalid.err"; then
        cat "$haiku_work/invalid.err" >&2
        exit 1
    fi
    if grep -q HAIKU_INVALID_ACCEPTED "$haiku_work/invalid.out"; then exit 1; fi
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/invalid" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
        exit 1
    fi
    if (cd "$haiku_work" && ./invalid) > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Compiled AML accepted invalid RAE input: %s\n' "$haiku_fixture" >&2
        exit 1
    fi
    if ! grep -Fq "$haiku_expected" "$haiku_work/invalid.err"; then
        cat "$haiku_work/invalid.err" >&2
        exit 1
    fi
    if grep -q HAIKU_INVALID_ACCEPTED "$haiku_work/invalid.out"; then exit 1; fi
    if ! "$haiku_work/unchanged" "$haiku_fixture" "$haiku_expected" \
        > "$haiku_work/preserved.out" 2> "$haiku_work/preserved.err"; then
        cat "$haiku_work/preserved.out" "$haiku_work/preserved.err" >&2
        exit 1
    fi
    haiku_rejections=$((haiku_rejections + 1))
done
printf 'PASS: %s RAE reference fields in interpreter and compiled --scalar\n' "$haiku_references"
printf 'PASS: RAE short circuits, text/context/batch/depth/counter bounds and native generation-to-learning\n'
printf 'PASS: %s rejected RAE operations; parameters, state, context and candidates preserved\n' "$haiku_rejections"
