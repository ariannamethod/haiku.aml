#define main actually_life_main
#include "l.c"
#undef main

static void probe_line(const char *s) {
    int out[64], n = semtok_line(s, out, 64);
    printf("TOK %s =>", s);
    for (int i = 0; i < n; i++) printf(" %s", glyph_name(out[i]));
    puts("");
}

int main(void) {
    seed_rng(42); mouth_build(); charges_init();
    const char *lines[] = {
        "I remember the river and the sea",
        "rain ocean water fire flame burn",
        "the sun is fire and i feel fear in the dark",
        "inferno dragons quantum spaghetti obliteration",
        "the and of", "AI BE me", "\320\264\320\276\320\266\320\264\321\214",
        NULL
    };
    for (int i = 0; lines[i]; i++) probe_line(lines[i]);
    int water = semtok_find_glyph("water"), fire = semtok_find_glyph("fire");
    int tree = semtok_find_glyph("tree"), light = semtok_find_glyph("light");
    int a[2] = {water, tree}, b[2] = {water, light};
    for (int i = 0; i < 3; i++) field_observe(a, 2);
    field_observe(b, 2);
    float logits[VOCAB_CAP] = {0}; int context[1] = {water};
    field_fold(logits, context, 1);
    printf("FIELD water row %.0f tree %.0f light %.0f; folded tree %.6f light %.6f fire %.6f\n",
           (double)g_field_row[water], (double)g_field_bi[water][tree], (double)g_field_bi[water][light],
           (double)logits[tree], (double)logits[light], (double)logits[fire]);
    Model *m = model_new();
    int pair[2] = {fire, water};
    for (int i = 0; i < 13; i++) cooc_track(pair, 2);
    try_emerge(m); printf("BIRTH count13 emerged %d\n", g_n_emerged);
    cooc_track(pair, 2); try_emerge(m);
    printf("BIRTH count14 emerged %d id %d name %s dS %.6f dD %.6f metabolism %.6f\n",
           g_n_emerged, VOCAB, glyph_name(VOCAB), (double)charge[VOCAB].mode_dS,
           (double)charge[VOCAB].mode_dDiss, (double)charge[VOCAB].metab_factor);
    int child = birth_symbol(m, VOCAB, tree);
    char name[160]; sym_name(child, name, sizeof name);
    printf("RECURSION id %d name %s resolve %d\n", child, name, resolve_sym(m, name));
    int native_pair[2] = {BE_ID, fire}; Modes ordinary = {0,0}, reflexive = {0,0};
    float scarsA[VOCAB_CAP] = {0}, scarsB[VOCAB_CAP] = {0};
    (void)digest(m, &ordinary, scarsA, &fire, -1, 1);
    (void)digest(m, &reflexive, scarsB, native_pair, -1, 2);
    printf("CHARGE fire S %.6f D %.6f scar %.6f; BE fire S %.6f D %.6f scar %.6f\n",
           (double)ordinary.S, (double)ordinary.dissonance, (double)scarsA[fire],
           (double)reflexive.S, (double)reflexive.dissonance, (double)scarsB[fire]);
    float sum_before = 0, sum_after = 0;
    for (int i = 0; i < VOCAB_CAP; i++) sum_before += g_field_row[i];
    int recent[CTX] = {water}, nr = 1;
    (void)dream_once(m, &ordinary, scarsA, recent, &nr, 1);
    for (int i = 0; i < VOCAB_CAP; i++) sum_after += g_field_row[i];
    printf("DREAM observation counts before %.0f after %.0f; recent %d\n", (double)sum_before, (double)sum_after, nr);
    printf("SIZE Model %zu bytes; glyph transition table %zu bytes; cooccurrence %zu bytes\n",
           sizeof(Model), sizeof(g_field_bi), sizeof(g_cooc));
    free(m);
    return 0;
}
