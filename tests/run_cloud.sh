#!/usr/bin/env bash
# Living word/Markov state through AML, plus rejected-update state preservation.
set -euo pipefail

haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_include=${HAIKU_AML_INCLUDE:-"$haiku_root/../ariannamethod.ai/core"}
haiku_cc=${CC:-cc}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-cloud.XXXXXX")
trap 'rm -rf "$haiku_work"' EXIT

for haiku_tool in "$haiku_aml" "$haiku_amlc"; do
    if [ ! -x "$haiku_tool" ]; then
        printf 'Missing AML executable: %s\n' "$haiku_tool" >&2
        exit 1
    fi
done
if [ ! -f "$haiku_lib" ] || [ ! -f "$haiku_include/ariannamethod.h" ]; then
    printf 'Missing scalar AML library or matching header.\n' >&2
    exit 1
fi
mkdir -p "$haiku_work/prefix/lib"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
printf '[AML] HAIKU_CLOUD_PARITY_OK\n' > "$haiku_work/expected.out"

haiku_numeric=0
haiku_lists=0
for haiku_fixture in "$haiku_root"/tests/fixtures/cloud_*.aml; do
    case "$haiku_fixture" in */cloud_support.aml) continue;; esac
    if ! LC_ALL=C awk 'length($0) >= 256 {bad=1} END {exit bad || NR > 500}' "$haiku_fixture"; then
        printf 'Cloud fixture exceeds the AML source budget: %s\n' "$haiku_fixture" >&2
        exit 1
    fi
    if ! "$haiku_aml" "$haiku_fixture" > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"; then
        cat "$haiku_work/interpreted.out" "$haiku_work/interpreted.err" >&2
        exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/interpreted.out"
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_fixture" --scalar \
        -o "$haiku_work/cloud" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
        cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
        exit 1
    fi
    if ! (cd "$haiku_work" && ./cloud) > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"; then
        cat "$haiku_work/compiled.out" "$haiku_work/compiled.err" >&2
        exit 1
    fi
    diff -u "$haiku_work/expected.out" "$haiku_work/compiled.out"
    haiku_count=$(awk '/^# Numeric reference values:/ {print $5}' "$haiku_fixture")
    haiku_numeric=$((haiku_numeric + haiku_count))
    haiku_count=$(awk '/^# List reference results:/ {print $5}' "$haiku_fixture")
    haiku_lists=$((haiku_lists + haiku_count))
done

# The runner exits on an error. This temporary C host uses AML's borrowed-value
# API to inspect all eleven original containers after the rejected AML call.
# It contains no Haiku operations: setup and mutation live in AML; C compares
# owned snapshots with the borrowed runtime values after failure.
cat > "$haiku_work/unchanged.c" <<'C'
#include "ariannamethod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *map_names[] = {
    "weights", "frequency", "last_used", "observed_counts", "resonance", "counts"
};
static const char *list_names[] = {
    "sources", "observed", "edges", "vocab", "recent"
};
static int text_equal(const AM_String *a, const AM_String *b) {
    return a && b && a->byte_len == b->byte_len &&
        !memcmp(a->data, b->data, (size_t)a->byte_len);
}
static int map_equal(const AM_Map *a, const AM_Map *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; ++i)
        if (!text_equal(a->entries[i].key, b->entries[i].key) ||
            memcmp(&a->entries[i].value, &b->entries[i].value, sizeof(float))) return 0;
    return 1;
}
static int list_equal(const AM_List *a, const AM_List *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; ++i)
        if (!text_equal(a->items[i], b->items[i])) return 0;
    return 1;
}
static void fail(const char *path, const char *detail) {
    fprintf(stderr, "State preservation failed: %s: %s\n", path, detail);
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
    char *import_end = import ? strchr(import, '\n') : NULL;
    if (!marker || !import_end || import_end >= marker) return 2;
    size_t import_length = (size_t)(import_end - import + 1);
    char *tail = marker + strlen(marker_text);
    char *call = malloc(import_length + strlen(tail) + 1);
    if (!call) return 2;
    memcpy(call, import, import_length);
    strcpy(call + import_length, tail);
    *marker = 0;
    am_init();
    am_persistent_mode(1);
    if (am_exec_source(source, argv[1])) fail(argv[1], am_get_error());
    AM_Map *maps[sizeof(map_names) / sizeof(map_names[0])];
    AM_List *lists[sizeof(list_names) / sizeof(list_names[0])];
    for (size_t i = 0; i < sizeof(maps) / sizeof(maps[0]); ++i) {
        maps[i] = am_map_clone(am_get_var_map(map_names[i]));
        if (!maps[i]) fail(argv[1], "map snapshot unavailable");
    }
    for (size_t i = 0; i < sizeof(lists) / sizeof(lists[0]); ++i) {
        lists[i] = am_list_clone(am_get_var_list(list_names[i]));
        if (!lists[i]) fail(argv[1], "list snapshot unavailable");
    }
    if (!am_exec_source(call, argv[1])) fail(argv[1], "invalid call succeeded");
    if (!strstr(am_get_error(), argv[2])) fail(argv[1], am_get_error());
    for (size_t i = 0; i < sizeof(maps) / sizeof(maps[0]); ++i) {
        if (!map_equal(maps[i], am_get_var_map(map_names[i]))) fail(argv[1], map_names[i]);
        am_map_free(maps[i]);
    }
    for (size_t i = 0; i < sizeof(lists) / sizeof(lists[0]); ++i) {
        if (!list_equal(lists[i], am_get_var_list(list_names[i]))) fail(argv[1], list_names[i]);
        am_list_free(lists[i]);
    }
    am_persistent_mode(0);
    free(call);
    free(source);
    return 0;
}
C
"$haiku_cc" -O2 -Wall -Wextra -Werror -I"$haiku_include" "$haiku_work/unchanged.c" \
    "$haiku_lib" -lm -lpthread -o "$haiku_work/unchanged"

haiku_rejections=0
for haiku_fixture in "$haiku_root"/tests/fixtures/cloud_invalid/*.aml; do
    haiku_expected=$(sed -n 's/^# EXPECT: //p' "$haiku_fixture")
    if [ -z "$haiku_expected" ]; then
        printf 'Missing rejection diagnostic: %s\n' "$haiku_fixture" >&2
        exit 1
    fi
    if "$haiku_aml" "$haiku_fixture" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf 'Interpreter accepted invalid state: %s\n' "$haiku_fixture" >&2
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
        printf 'Compiled program accepted invalid state: %s\n' "$haiku_fixture" >&2
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
printf 'PASS: %s numerical and %s ordered-list reference results in AML interpreter and compiled --scalar\n' "$haiku_numeric" "$haiku_lists"
printf 'PASS: %s invalid updates rejected in both paths; all 11 state containers unchanged in host inspection\n' "$haiku_rejections"
