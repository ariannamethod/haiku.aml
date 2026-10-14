#define main actually_life_main
#include "l.c"
#undef main
#include <assert.h>

static void sums(long *cooc, float *field) {
    *cooc = 0; *field = 0;
    for (int a = 0; a < VOCAB_CAP; ++a) {
        *field += g_field_row[a];
        for (int b = 0; b < VOCAB_CAP; ++b) *cooc += g_cooc[a][b];
    }
}
static void checked_dream(Model *m, Modes *mo, float *scars, int *recent, int *rn) {
    long c0, c1; float f0, f1;
    sums(&c0, &f0);
    (void)dream_once(m, mo, scars, recent, rn, 0);
    sums(&c1, &f1);
    assert(c0 == c1 && f0 == f1);
    printf("dream external cooccurrence %ld->%ld field %.0f->%.0f symbols=%d\n", c0, c1, f0, f1, g_n_emerged);
}
int main(void) {
    seed_rng(42); mouth_build(); charges_init();
    Model *m = model_new(); Modes mo = {0, 0}; float scars[VOCAB_CAP] = {0};
    int recent[CTX] = {0}, rn = 0; long streak = 0;
    int pair[8]; int n = semtok_line("fire water", pair, 8);
    assert(n == 2 && pair[0] == 1 && pair[1] == 0);
    for (int i = 0; i < 13; ++i) ingest(m, &mo, scars, pair, n, recent, &rn, &streak);
    assert(g_n_emerged == 0);
    checked_dream(m, &mo, scars, recent, &rn);
    assert(g_n_emerged == 0);
    ingest(m, &mo, scars, pair, n, recent, &rn, &streak);
    assert(g_n_emerged == 0);
    checked_dream(m, &mo, scars, recent, &rn);
    assert(g_n_emerged == 1);
    char first[160]; sym_name(VOCAB, first, sizeof first);
    assert(strcmp(first, "fire+water") == 0);
    printf("first id=%d structure=%s threshold=%d\n", VOCAB, first, g_cooc[1][0]);
    n = semtok_line("fire water tree", pair, 8);
    assert(n == 3 && pair[0] == 1 && pair[1] == 0 && pair[2] == 4);
    printf("ordinary postbirth text remains %d,%d,%d\n", pair[0], pair[1], pair[2]);
    n = semtok_ether(m, "fire+water tree", pair, 8);
    assert(n == 2 && pair[0] == VOCAB && pair[1] == 4);
    for (int i = 0; i < 13; ++i) ingest(m, &mo, scars, pair, n, recent, &rn, &streak);
    checked_dream(m, &mo, scars, recent, &rn);
    assert(g_n_emerged == 1);
    ingest(m, &mo, scars, pair, n, recent, &rn, &streak);
    checked_dream(m, &mo, scars, recent, &rn);
    assert(g_n_emerged == 2);
    char recursive[160]; sym_name(VOCAB + 1, recursive, sizeof recursive);
    assert(strcmp(recursive, "(fire+water)+tree") == 0);
    printf("second id=%d structure=%s threshold=%d\n", VOCAB + 1, recursive, g_cooc[VOCAB][4]);
    float logits[VOCAB_CAP] = {0}; int context[1] = {VOCAB};
    field_fold(logits, context, 1);
    printf("compound-context pressure tree=%.9f music=%.9f\n", logits[4], logits[82]);
    assert(logits[4] > logits[82]);
    free(m);

    /* A new body already owns a different local ID90. Its source arrays,
       observations and model are private; only the parent structure travels. */
    memset(g_cooc, 0, sizeof g_cooc); memset(g_born, 0, sizeof g_born);
    memset(g_emerged_a, 0, sizeof g_emerged_a); memset(g_emerged_b, 0, sizeof g_emerged_b);
    memset(g_field_bi, 0, sizeof g_field_bi); memset(g_field_row, 0, sizeof g_field_row);
    g_n_emerged = 0; charges_init(); m = model_new();
    assert(birth_symbol(m, 4, 6) == VOCAB);
    int adopted = resolve_sym(m, recursive);
    assert(adopted == VOCAB + 2 && g_n_emerged == 3);
    char adopted_name[160]; sym_name(adopted, adopted_name, sizeof adopted_name);
    assert(strcmp(adopted_name, recursive) == 0);
    long c; float f; sums(&c, &f); assert(c == 0 && f == 0);
    assert(resolve_sym(m, recursive) == adopted && g_n_emerged == 3);
    printf("receiver adopts same structure at private id=%d; external counts=%ld field=%.0f; repeated resolve reuses id\n", adopted, c, f);
    free(m);
    puts("RECURSIVE_PRIMARY_SOURCE_OK");
    return 0;
}
