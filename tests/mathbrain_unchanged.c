/* Test host: inspect AML-owned learner state after rejected operations. */
#include "ariannamethod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void fail(const char *path, const char *message) {
    fprintf(stderr, "MathBrain state: %s: %s\n", path, message);
    exit(1);
}

static int same_string(const AM_String *a, const AM_String *b) {
    return a && b && a->byte_len == b->byte_len &&
        !memcmp(a->data, b->data, (size_t)a->byte_len);
}

static int same_map(const AM_Map *a, const AM_Map *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; i++)
        if (!same_string(a->entries[i].key, b->entries[i].key) ||
            memcmp(&a->entries[i].value, &b->entries[i].value, sizeof(float))) return 0;
    return 1;
}

static int same_list(const AM_List *a, const AM_List *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; i++)
        if (!same_string(a->items[i], b->items[i])) return 0;
    return 1;
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
    am_use_notorch();
    am_persistent_mode(1);
    if (am_exec_source(source, argv[1])) fail(argv[1], am_get_error());

    int size = 0;
    const float *borrowed = am_get_var_array("parameters", &size);
    if (!borrowed || size < 1) return 2;
    float *parameters = malloc((size_t)size * sizeof(float));
    if (!parameters) return 2;
    memcpy(parameters, borrowed, (size_t)size * sizeof(float));
    const char *map_names[] = {"state", "counts", "rng"};
    const char *list_names[] = {"rows", "user"};
    AM_Map *maps[3];
    AM_List *lists[2];
    for (int i = 0; i < 3; i++) {
        maps[i] = am_map_clone(am_get_var_map(map_names[i]));
        if (!maps[i]) return 2;
    }
    for (int i = 0; i < 2; i++) {
        lists[i] = am_list_clone(am_get_var_list(list_names[i]));
        if (!lists[i]) return 2;
    }
    if (!am_exec_source(call, argv[1])) fail(argv[1], "invalid call succeeded");
    if (!strstr(am_get_error(), argv[2])) fail(argv[1], am_get_error());
    int actual_size = 0;
    borrowed = am_get_var_array("parameters", &actual_size);
    if (!borrowed || actual_size != size ||
        memcmp(borrowed, parameters, (size_t)size * sizeof(float))) fail(argv[1], "parameters changed");
    for (int i = 0; i < 3; i++) {
        if (!same_map(maps[i], am_get_var_map(map_names[i]))) fail(argv[1], map_names[i]);
        am_map_free(maps[i]);
    }
    for (int i = 0; i < 2; i++) {
        if (!same_list(lists[i], am_get_var_list(list_names[i]))) fail(argv[1], list_names[i]);
        am_list_free(lists[i]);
    }
    am_persistent_mode(0);
    free(parameters);
    free(call);
    free(source);
    return 0;
}
