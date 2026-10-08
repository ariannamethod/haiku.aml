/* Shared host inspection helpers for AML-owned state transitions. */
#ifndef HAIKU_STATE_TEST_H
#define HAIKU_STATE_TEST_H
#include "ariannamethod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *root, *work;
static void require(int yes, const char *detail) {
    if (!yes) { fprintf(stderr, "state continuity: %s\n", detail); exit(1); }
}
static int same_string(const AM_String *a, const AM_String *b) {
    return a && b && a->byte_len == b->byte_len &&
           !memcmp(a->data, b->data, (size_t)a->byte_len);
}
static int same_list(const AM_List *a, const AM_List *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; ++i)
        if (!same_string(a->items[i], b->items[i])) return 0;
    return 1;
}
static int same_map(const AM_Map *a, const AM_Map *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; ++i)
        if (!same_string(a->entries[i].key, b->entries[i].key) ||
            memcmp(&a->entries[i].value, &b->entries[i].value, sizeof(float))) return 0;
    return 1;
}
static int same_value(const AML_Var *a, const AML_Var *b) {
    if (!a || !b || a->type != b->type) return 0;
    switch (a->type) {
    case AML_TYPE_FLOAT: return !memcmp(&a->value, &b->value, sizeof(float));
    case AML_TYPE_STRING: return same_string(a->string, b->string);
    case AML_TYPE_LIST: return same_list(a->list, b->list);
    case AML_TYPE_MAP: return same_map(a->map, b->map);
    case AML_TYPE_ARRAY:
        return a->array && b->array && a->array->len == b->array->len &&
               a->array->rows == b->array->rows && a->array->cols == b->array->cols &&
               !memcmp(a->array->data, b->array->data, (size_t)a->array->len * sizeof(float));
    default: return 0;
    }
}
static int same_record(const AM_Record *a, const AM_Record *b) {
    if (!a || !b) return 0;
    AM_List *ak = am_record_keys(a), *bk = am_record_keys(b);
    require(ak && bk, "record key allocation");
    int same = same_list(ak, bk);
    for (int i = 0; same && i < ak->len; ++i)
        same = same_value(am_record_get(a, ak->items[i]), am_record_get(b, bk->items[i]));
    am_list_free(ak); am_list_free(bk);
    return same;
}
static unsigned char *read_file(const char *path, size_t *size) {
    FILE *file = fopen(path, "rb");
    require(file && !fseek(file, 0, SEEK_END), "open checkpoint bytes");
    long length = ftell(file);
    require(length > 0 && !fseek(file, 0, SEEK_SET), "checkpoint byte length");
    unsigned char *bytes = malloc((size_t)length);
    require(bytes && fread(bytes, 1, (size_t)length, file) == (size_t)length, "read checkpoint bytes");
    fclose(file); *size = (size_t)length; return bytes;
}
static void same_file(const char *path, const unsigned char *before, size_t size) {
    size_t after_size = 0;
    unsigned char *after = read_file(path, &after_size);
    require(after_size == size && !memcmp(before, after, size), "previous checkpoint changed after rejection");
    free(after);
}
static int execute(const char *fragment) {
    size_t size = strlen(root) + strlen(fragment) + 100;
    char *program = malloc(size), origin[4096];
    require(program != NULL, "program allocation");
    snprintf(program, size, "IMPORT \"%s/src/session.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/validation.aml", work);
    int result = am_exec_source(program, origin);
    free(program); return result;
}
static void expect_error(const char *fragment, const char *detail) {
    int result = execute(fragment);
    if (!result || !strstr(am_get_error(), detail)) {
        fprintf(stderr, "expected: %s\nactual: %s\n", detail, am_get_error());
        require(0, "rejected operation has wrong diagnostic");
    }
}

#endif
