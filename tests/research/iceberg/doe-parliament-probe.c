#define main doe_source_main
#include "doe/doe.c"
#undef main
#include <assert.h>

static void setup(GGUFIndex *ps, int dim, int rank, int count) {
    memset(ps, 0, sizeof(*ps));
    ps->host_dim = dim; ps->lora_rank = rank; ps->n_field_layers = 1;
    ps->profile.fingerprint = 0x5a112345ULL;
    FieldLayer *fl = &ps->field_layers[0];
    fl->n_alive = count;
    fl->parliament.w_vote = calloc(MAX_EXPERTS * dim, sizeof(float));
    fl->parliament.consensus = .5f;
    for (int e = 0; e < count; e++) init_lora_expert(&fl->experts[e], dim, rank, 0);
}

static void teardown(GGUFIndex *ps) {
    FieldLayer *fl = &ps->field_layers[0];
    for (int e = 0; e < MAX_EXPERTS; e++) if (fl->experts[e].alive) free_lora_expert(&fl->experts[e]);
    free(fl->parliament.w_vote);
}

int main(void) {
    rng_state = 42; F.notorch_lr = .005f;
    GGUFIndex *ps = calloc(1, sizeof(*ps));
    setup(ps, 4, 4, 8);
    HarmonicState hs = {0};
    float x4[4] = {1, 0, 0, 0}; int sel[MAX_EXPERTS]; float weights[MAX_EXPERTS];
    FieldLayer *fl = &ps->field_layers[0]; int k = 0;
    for (int i = 0; i < 100; i++) k = parliament_elect(&fl->parliament, fl->experts, x4, 4, &hs, sel, weights);
    printf("equal_votes k=%d consensus=%.9g\n", k, fl->parliament.consensus);
    assert(k == 7);
    fl->parliament.consensus = .5f;
    for (int e = 0; e < 8; e++) fl->parliament.w_vote[e*4] = 2*e - 7;
    for (int i = 0; i < 100; i++) k = parliament_elect(&fl->parliament, fl->experts, x4, 4, &hs, sel, weights);
    printf("spread_votes k=%d consensus=%.9g selected=%d,%d weights=%.9g,%.9g\n", k, fl->parliament.consensus, sel[0], sel[1], weights[0], weights[1]);
    assert(k == 2 && sel[0] == 7 && sel[1] == 6);
    teardown(ps);

    setup(ps, 4, 4, 4); fl = &ps->field_layers[0];
    for (int e = 0; e < 4; e++) fl->experts[e].tokens_seen = 5;
    update_expert_vitality(fl, 10);
    printf("uniform_two_of_four_election vitality=%.9g (initial=.7)\n", fl->experts[0].vitality);
    assert(fl->experts[0].vitality > .74f);
    teardown(ps);

    setup(ps, 4, 16, 4); fl = &ps->field_layers[0];
    float ones[4] = {1,1,1,1};
    for (int e = 0; e < 4; e++) for (int i = 0; i < 64; i++) fl->experts[e].lora_A[i] = fl->experts[e].lora_B[i] = .02f;
    notorch_offset = 0;
    for (int turn = 0; turn < 8; turn++) for (int e = 0; e < 4; e++) notorch_step(fl->experts[e].lora_A, fl->experts[e].lora_B, 4, 4, 16, ones, ones, .5f);
    for (int e = 0; e < 4; e++) {
        unsigned mask = 0;
        for (int r = 0; r < 16; r++) if (fl->experts[e].lora_A[r] != .02f) mask |= 1U << r;
        printf("shared_rank_cursor expert=%d changed_mask=0x%04x after=8_turns\n", e, mask);
        assert(mask == (15U << (e * 4)));
    }
    teardown(ps);

    setup(ps, 4, 4, 2); fl = &ps->field_layers[0];
    fl->experts[0].age = 31; fl->experts[0].low_vitality_count = 7;
    fl->parliament.election_count = 91; fl->parliament.consensus = .37f;
    assert(mkdir("doe-parliament-probe-data",0700) == 0 || errno == EEXIST);
    assert(chdir("doe-parliament-probe-data") == 0);
    assert(mkdir(MYCELIUM_DIR,0700) == 0 || errno == EEXIST);
    mycelium_save(ps, 27, .5f);
    GGUFIndex *loaded = calloc(1,sizeof(*loaded)); setup(loaded,4,4,2);
    assert(mycelium_load(loaded,ps->profile.fingerprint));
    FieldLayer *lf = &loaded->field_layers[0];
    printf("spore_roundtrip age=%d->%d low_count=%d->%d elections=%d->%d consensus=%.9g->%.9g\n",
        fl->experts[0].age,lf->experts[0].age,fl->experts[0].low_vitality_count,lf->experts[0].low_vitality_count,
        fl->parliament.election_count,lf->parliament.election_count,fl->parliament.consensus,lf->parliament.consensus);
    assert(lf->experts[0].age == 0 && lf->experts[0].low_vitality_count == 0 && lf->parliament.election_count == 0);
    teardown(ps); teardown(loaded); free(ps); free(loaded);
    puts("doe_parliament_probe PASS");
    return 0;
}
