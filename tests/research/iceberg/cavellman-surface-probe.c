#define main cavellman_program_main
#include "cavellman.c"
#undef main

int main(void) {
    CaveModel *m = calloc(1, sizeof *m);
    CaveVocab *v = calloc(1, sizeof *v);
    v->bos_id = semtok_seed_vocab(v->tokens, &v->vocab_size);
    v->base_size = v->vocab_size;
    v->mask_id = v->bos_id + 1;
    int fire = semtok_find_glyph("fire"), fear = semtok_find_glyph("fear");
    int pair[] = {fire, fear};
    cooccur_update(&m->cooccur, pair, 2);
    printf("first pair count=%d strength=%.6f interactions=%d\n", m->cooccur.pair_count[fire][fear], m->cooccur.matrix[fire][fear], m->cooccur.total_interactions);
    int id = try_emerge_symbol(m, v);
    printf("first birth id=%d bos=%d mask=%d n_emerged=%d depth=%d\n", id, v->bos_id, v->mask_id, m->n_emerged, m->emerged[0].depth);
    for (int i = 0; i < 500; i++) cooccur_update(&m->cooccur, pair, 2);
    check_symbol_survival(m, v);
    printf("at 501 interactions alive=%d depth=%d strength=%.6f\n", m->emerged[0].alive, m->emerged[0].depth, m->cooccur.matrix[fire][fear]);
    m->cooccur.matrix[fire][fear] = 0.52f;
    check_symbol_survival(m, v);
    printf("after strength=.52 alive=%d\n", m->emerged[0].alive);
    free(m); free(v);

    m = calloc(1, sizeof *m);
    CaveField f = {0}; f.coherence_floor = f.baseline_floor = .6f;
    int heard[] = {semtok_find_glyph("before"), semtok_find_glyph("dream"), semtok_find_glyph("after"), semtok_find_glyph("fear")};
    for (int i = 0; i < 4; i++) {
        field_hear(&f, m, i ? heard[i-1] : -1, heard[i]);
        printf("heard %s excitement=%.6f dissonance=%.6f speak=%d\n", GLYPH_NAMES[heard[i]], f.excitement, f.dissonance, field_should_speak(&f));
    }
    printf("hear count=%d matrix=%.6f\n", m->cooccur.pair_count[heard[0]][heard[1]], m->cooccur.matrix[heard[0]][heard[1]]);
    cooccur_update(&m->cooccur, heard, 4);
    printf("normalized count=%d matrix=%.6f\n", m->cooccur.pair_count[heard[0]][heard[1]], m->cooccur.matrix[heard[0]][heard[1]]);
    field_after_speak(&f);
    field_decay(&f);
    printf("after speak and decay excitement=%.6f dissonance=%.6f\n", f.excitement, f.dissonance);
    free(m);

    float A[6] = {0}, B[6] = {0}, x[3] = {1, 2, 3};
    for (int i = 0; i < 100; i++) nt_hebbian_step(A, B, 3, 3, 2, x, x, 1, .001f, .9999f);
    float norm = 0; for (int i = 0; i < 6; i++) norm += fabsf(A[i]) + fabsf(B[i]);
    printf("zero-initialized Hebbian adapters L1 norm after100=%.9g\n", norm);
    return 0;
}
