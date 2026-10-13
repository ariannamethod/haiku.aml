/* Host witnesses preflight and staged failure; the dream behavior stays AML. */
#include "ariannamethod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *root, *work;
static void require(int okay, const char *why) {
    if (!okay) { fprintf(stderr, "dream validation: %s\n", why); exit(1); }
}
static int execute(const char *fragment) {
    size_t size = strlen(root) + strlen(fragment) + 128;
    char *program = malloc(size), origin[4096];
    require(program != NULL, "allocate source");
    snprintf(program, size, "IMPORT \"%s/tests/fixtures/dream_support.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/dream-check.aml", work);
    int result = am_exec_source(program, origin);
    free(program);
    return result;
}
static void expected_failure(const char *program, const char *detail) {
    int result = execute(program);
    if (!result || !strstr(am_get_error(), detail)) {
        fprintf(stderr, "expected %s; actual %s\n", detail, am_get_error());
        require(0, "wrong rejection");
    }
}
static void save(const char *name, const char *suffix) {
    char path[4096], error[1024];
    snprintf(path, sizeof(path), "%s/%s.state", work, suffix);
    const AM_Record *record = am_get_var_record(name);
    require(record && am_checkpoint_save(record, path, error, sizeof(error)) == 1, "save inspected record");
}
static void unchanged(void) {
    char path[4096];
    snprintf(path, sizeof(path), "%s/before.state", work);
    FILE *before = fopen(path, "rb");
    snprintf(path, sizeof(path), "%s/after.state", work);
    FILE *after = fopen(path, "rb");
    require(before && after, "open inspected bytes");
    for (;;) {
        int a = fgetc(before), b = fgetc(after);
        require(a == b, "failed dream changed an owner");
        if (a == EOF) break;
    }
    require(!ferror(before) && !ferror(after), "read inspected bytes");
    fclose(before); fclose(after);
}
int main(int argc, char **argv) {
    require(argc == 3, "root and work directory required");
    root = argv[1]; work = argv[2];
    am_init(); am_use_notorch(); am_persistent_mode(1);
    require(!execute("live = dream_test_new(1, \"regex\")\nrecord_set(live, \"turn\", 10)\ntape = zeros(2)\ntape[1] = 0.25\nrecord_set(live, \"draws\", tape)"), am_get_error());
    save("live", "before");
    expected_failure("pending = record_clone(live)\nhaiku_dream_run(pending, \"rain\\ncloud\\nwind\", 0)\nrecord_replace(live, pending)", "draw tape exhausted");
    save("live", "after"); unchanged();

    /* Both history bounds reject before generation changes even a detached
       owner. Metadata is internally complete at each of these boundaries. */
    const char *capacity[] = {
        "live = dream_test_new(1, \"regex\")\nmetadata = zeros(12288)\ni = 0\nwhile i < 4096:\n    metadata[i * 3] = i + 1\n    metadata[i * 3 + 1] = i * 16\n    metadata[i * 3 + 2] = 16\n    j = 0\n    while j < 16:\n        list_push(record_get(live, \"dream_history\"), \"a\\nb\\nc\")\n        j = j + 1\n    i = i + 1\nrecord_set(live, \"dream_dialogs\", metadata)\nrecord_set(live, \"dream_last_turn\", 4096)\nrecord_set(live, \"turn\", 4097)",
        "live = dream_test_new(1, \"regex\")\nmetadata = zeros(30000)\ni = 0\nwhile i < 10000:\n    metadata[i * 3] = i + 1\n    metadata[i * 3 + 1] = i\n    metadata[i * 3 + 2] = 1\n    list_push(record_get(live, \"dream_history\"), \"a\\nb\\nc\")\n    i = i + 1\nrecord_set(live, \"dream_dialogs\", metadata)\nrecord_set(live, \"dream_last_turn\", 10000)\nrecord_set(live, \"turn\", 10001)"
    };
    for (size_t i = 0; i < sizeof(capacity) / sizeof(capacity[0]); ++i) {
        require(!execute(capacity[i]), am_get_error());
        require(!execute("haiku_dream_check(live)"), am_get_error());
        save("live", "before");
        expected_failure("haiku_dream_run(live, \"rain\\ncloud\\nwind\", 0)", i == 0 ? "exchange history is full" : "dialog history is full");
        save("live", "after"); unchanged();
    }
    require(!execute("live = dream_test_new(1, \"regex\")\nweights = zeros(10000)\ni = 0\nwhile i < 10000:\n    list_push(record_get(live, \"dream_fragments\"), \"old\")\n    weights[i] = 1\n    i = i + 1\nrecord_set(live, \"dream_weights\", weights)"), am_get_error());
    save("live", "before");
    expected_failure("haiku_dream_fragments(live, \"another\")", "fragment limit is 10000");
    save("live", "after"); unchanged();
    am_persistent_clear();
    puts("PASS: dream history/fragment capacity and mid-generation failure preserve every prior owner");
    return 0;
}
