/* Negative form calls must reject before draws or model publication. */
#include "ariannamethod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const struct {
    const char *setup, *call, *error;
} cases[] = {
    {"", "haiku_living_targets(-1)", "integer in [0, 3]"},
    {"", "haiku_living_generate(rows, counts, vocab, syllables, 1, 1, rng, tape, 4)", "integer in [0, 3]"},
    {"", "haiku_living_generate(rows, counts, vocab, syllables, -1, 1, rng, tape, 0)", "candidate count"},
    {"", "haiku_living_generate(rows, counts, vocab, syllables, 1, 0, rng, tape, 0)", "temperature"},
    {"vocab = haiku_words('water later')\nsyllables = haiku_syllable_cache(vocab)\n",
     "haiku_living_generate(rows, counts, vocab, syllables, 1, 1, rng, tape, 3)", "target is unreachable"},
    {"vocab = haiku_words('night light')\nsyllables = haiku_syllable_cache(vocab)\n",
     "haiku_living_generate(rows, counts, vocab, syllables, 1, 1, rng, tape, 3)", "distinct A rhymes"},
    {"vocab = haiku_words('night stone')\nsyllables = haiku_syllable_cache(vocab)\n",
     "haiku_living_generate(rows, counts, vocab, syllables, 1, 1, rng, tape, 3)", "distinct A rhymes"},
    {"vocab = haiku_words('night Night stone')\nsyllables = haiku_syllable_cache(vocab)\n",
     "haiku_living_generate(rows, counts, vocab, syllables, 1, 1, rng, tape, 3)", "distinct A rhymes"},
    {"vocab = haiku_words('night light antidisestablishmentarianism')\nsyllables = haiku_syllable_cache(vocab)\n",
     "haiku_living_generate(rows, counts, vocab, syllables, 1, 1, rng, tape, 3)", "distinct A rhymes"},
    {"map_set(syllables, 'night', 2)\n",
     "haiku_living_generate(rows, counts, vocab, syllables, 1, 1, rng, tape, 0)", "English syllable cache"},
    {"", "haiku_living_output_check('night night', 1)", "line count"},
    {"", "haiku_living_output_check('night', 0)", "syllable budget"},
    {"", "haiku_living_output_check('night night night night night night night\nnight night night night night stone\nnight night night night night night night', 3)", "distinct words"},
    {"", "haiku_living_output_check('night night night night night night night\nnight night night night night stone\nnight night night night night night cat', 3)", "A endings must rhyme"},
    {"", "haiku_living_output_check('night night night night remote\nnight night night night night stone\nnight night night night night night hot', 3)", "A endings must rhyme"},
    {"", "haiku_living_output_check('night night night night night night night\nnight night night night night night light\nnight night night night night night light', 3)", "different rhyme"},
};

static void check(int ok, const char *detail) {
    if (!ok) { fprintf(stderr, "living form: %s; AML: %s\n", detail, am_get_error()); exit(1); }
}
static int same_text(const AM_String *a, const AM_String *b) {
    return a && b && a->byte_len == b->byte_len && !memcmp(a->data, b->data, (size_t)a->byte_len);
}
static int same_map(const AM_Map *a, const AM_Map *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; ++i)
        if (!same_text(a->entries[i].key, b->entries[i].key) ||
            memcmp(&a->entries[i].value, &b->entries[i].value, sizeof(float))) return 0;
    return 1;
}
static int same_list(const AM_List *a, const AM_List *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; ++i) if (!same_text(a->items[i], b->items[i])) return 0;
    return 1;
}
static void make_setup(char *out, size_t size, const char *root, size_t i) {
    snprintf(out, size, "IMPORT \"%s/src/living_form.aml\"\n"
        "rows = list_new()\ncounts = map_new()\nvocab = haiku_words('night light stone bone')\n"
        "syllables = haiku_syllable_cache(vocab)\nrng = rng_new(575)\ntape = zeros(200)\n%s", root, cases[i].setup);
}
static void make_call(char *out, size_t size, const char *root, size_t i) {
    /* Escape embedded source newlines into AML quoted-string escapes. */
    char escaped[4096]; size_t n = 0;
    for (const char *p = cases[i].call; *p; ++p) {
        check(n + 3 < sizeof(escaped), "invalid fixture length");
        if (*p == '\n') { escaped[n++] = '\\'; escaped[n++] = 'n'; }
        else escaped[n++] = *p;
    }
    escaped[n] = 0;
    snprintf(out, size, "IMPORT \"%s/src/living_form.aml\"\n%s\nPRINT 'WRONG_CONTINUATION'\n", root, escaped);
}
int main(int argc, char **argv) {
    const size_t count = sizeof(cases) / sizeof(cases[0]);
    check(argc >= 2, "root argument");
    if (argc == 3 && !strcmp(argv[2], "--count")) { printf("%zu\n", count); return 0; }
    if (argc == 4 && !strcmp(argv[2], "--bad")) {
        size_t i = (size_t)atoi(argv[3]); check(i < count, "fixture index");
        char setup[10000], call[10000];
        make_setup(setup, sizeof(setup), argv[1], i); make_call(call, sizeof(call), argv[1], i);
        fputs(setup, stdout); fputs(call, stdout); return 0;
    }
    check(argc == 2, "unexpected arguments");
    am_init(); am_use_notorch(); am_persistent_mode(1);
    const char *maps[] = {"counts", "syllables", "rng"};
    const char *lists[] = {"rows", "vocab"};
    for (size_t i = 0; i < count; ++i) {
        am_persistent_clear();
        char setup[10000], call[10000];
        make_setup(setup, sizeof(setup), argv[1], i); make_call(call, sizeof(call), argv[1], i);
        check(am_exec_source(setup, "/tmp/living-form-negative.aml") == 0, "fixture setup");
        AM_Map *before_maps[3]; AM_List *before_lists[2];
        for (int j = 0; j < 3; ++j) { before_maps[j] = am_map_clone(am_get_var_map(maps[j])); check(before_maps[j] != NULL, "map snapshot"); }
        for (int j = 0; j < 2; ++j) { before_lists[j] = am_list_clone(am_get_var_list(lists[j])); check(before_lists[j] != NULL, "list snapshot"); }
        int rc = am_exec_source(call, "/tmp/living-form-negative.aml");
        if (!rc || !strstr(am_get_error(), cases[i].error)) fprintf(stderr, "fixture %zu wanted %s\n", i, cases[i].error);
        check(rc != 0 && strstr(am_get_error(), cases[i].error), "expected rejection");
        for (int j = 0; j < 3; ++j) { check(same_map(before_maps[j], am_get_var_map(maps[j])), "rejection changed map"); am_map_free(before_maps[j]); }
        for (int j = 0; j < 2; ++j) { check(same_list(before_lists[j], am_get_var_list(lists[j])), "rejection changed list"); am_list_free(before_lists[j]); }
        int len = 0; const float *tape = am_get_var_array("tape", &len);
        check(tape && len == 200 && tape[0] == 0, "rejection consumed a draw");
    }
    am_persistent_clear(); am_persistent_mode(0);
    printf("PASS living form: %zu invalid forms, rhyme/budget failures and forged outputs preserve owners and draws\n", count);
    return 0;
}
