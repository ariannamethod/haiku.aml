/* History comparison must not serialize all utterances into one string. */
#include "ariannamethod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void check(int ok, const char *message) {
    if (!ok) {
        fprintf(stderr, "living history: %s; AML: %s\n", message, am_get_error());
        exit(1);
    }
}
static int same_list(const AM_List *a, const AM_List *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; ++i) {
        const AM_String *x = a->items[i], *y = b->items[i];
        if (!x || !y || x->byte_len != y->byte_len ||
            memcmp(x->data, y->data, (size_t)x->byte_len)) return 0;
    }
    return 1;
}
static void reject(const char *root, const char *call, const char *diagnostic) {
    AM_List *prior = am_list_clone(am_get_var_list("prior"));
    AM_List *later = am_list_clone(am_get_var_list("later"));
    check(prior && later, "snapshot both history owners");
    char program[8192];
    snprintf(program, sizeof(program), "IMPORT \"%s/src/living_state.aml\"\n%s\n", root, call);
    int rc = am_exec_source(program, "/tmp/living-history-validation.aml");
    check(rc != 0 && strstr(am_get_error(), diagnostic), "expected exact history refusal");
    check(same_list(prior, am_get_var_list("prior")), "rejection changed the prior owner");
    check(same_list(later, am_get_var_list("later")), "rejection changed the proposed owner");
    am_list_free(prior);
    am_list_free(later);
}
int main(int argc, char **argv) {
    check(argc == 2, "repository root argument");
    char path[4096], program[8192];
    snprintf(path, sizeof(path), "%s/tests/living_history.aml", argv[1]);
    am_init();
    am_use_notorch();
    am_persistent_mode(1);
    check(am_exec_file(path) == 0, "large valid history fixture");
    /* Negative control: the retired comparison fails on the SAME valid data. */
    reject(argv[1], "list_key(prior)", "composite key limit exceeded");
    snprintf(program, sizeof(program), "IMPORT \"%s/src/living_state.aml\"\n"
        "changed = text_concat(text_slice(word, 0, 9999), \"y\")\n"
        "list_set(later, 109, changed)\n", argv[1]);
    check(am_exec_source(program, "/tmp/living-history-validation.aml") == 0,
          "change the last byte beyond the first MiB");
    reject(argv[1], "haiku_living_words_prefix(prior, later)", "changed earlier words");
    am_persistent_clear();
    am_persistent_mode(0);
    puts("PASS living history: 1100000-byte prefix, old-key negative control, late mutation and exact owner preservation");
    return 0;
}
