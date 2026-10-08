/* Inspect the real entrypoint after input/EOF/quit and learning. No host organs. */
#include "ariannamethod.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void require(int yes, const char *detail) {
    if (!yes) { fprintf(stderr, "foreground owner: %s\n", detail); exit(1); }
}
static float field(const AM_Map *map, const char *name) {
    AM_String *key = am_string_new(name);
    float value = 0;
    require(key && am_map_get(map, key, &value) == 1, name);
    am_string_free(key);
    return value;
}
static int same_array(const char *a, const char *b) {
    int na = 0, nb = 0;
    const float *pa = am_get_var_array(a, &na), *pb = am_get_var_array(b, &nb);
    return pa && pb && na == nb && !memcmp(pa, pb, (size_t)na * sizeof(float));
}
static int same_map(const AM_Map *a, const AM_Map *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; i++) {
        if (a->entries[i].key->byte_len != b->entries[i].key->byte_len ||
            memcmp(a->entries[i].key->data, b->entries[i].key->data,
                   (size_t)a->entries[i].key->byte_len) ||
            memcmp(&a->entries[i].value, &b->entries[i].value, sizeof(float))) return 0;
    }
    return 1;
}
static int same_list(const AM_List *a, const AM_List *b) {
    if (!a || !b || a->len != b->len) return 0;
    for (int i = 0; i < a->len; i++)
        if (a->items[i]->byte_len != b->items[i]->byte_len ||
            memcmp(a->items[i]->data, b->items[i]->data, (size_t)a->items[i]->byte_len)) return 0;
    return 1;
}

int main(int argc, char **argv) {
    require(argc == 5, "source, accepted turns, RAE observations, expected error");
    int turns = atoi(argv[2]), rae = atoi(argv[3]);
    FILE *file = fopen(argv[1], "rb");
    require(file && !fseek(file, 0, SEEK_END), "read source");
    long size = ftell(file);
    require(size > 0 && size < 100000 && !fseek(file, 0, SEEK_SET), "source size");
    char *source = malloc((size_t)size + 1);
    require(source && fread(source, 1, (size_t)size, file) == (size_t)size, "source bytes");
    fclose(file);
    source[size] = 0;
    char *marker = strstr(source, "PRINT \"HAiKU");
    require(marker != NULL, "entrypoint initialization marker");
    const char *snapshots =
        "fg_initial_math = record_get(state, \"mathbrain\")\n"
        "fg_initial_rae = record_get(state, \"rae\")\n"
        "fg_initial_rng = record_get(state, \"voice_rng\")\n"
        "fg_initial_rows = record_get(state, \"rows\")\n"
        "fg_initial_counts = record_get(state, \"counts\")\n"
        "fg_initial_vocab = record_get(state, \"vocab\")\n"
        "fg_initial_syllables = record_get(state, \"syllables\")\n";
    size_t prefix = (size_t)(marker - source), extra = strlen(snapshots);
    char *program = malloc((size_t)size + extra + 1);
    require(program != NULL, "snapshot source");
    memcpy(program, source, prefix);
    memcpy(program + prefix, snapshots, extra);
    strcpy(program + prefix + extra, marker);
    am_init();
    am_use_notorch();
    am_persistent_mode(1);
    int result = am_exec_source(program, argv[1]);
    if (*argv[4]) require(result && strstr(am_get_error(), argv[4]), am_get_error());
    else require(!result, am_get_error());
    /* Inspect the published owner even when a detached turn failed. */
    require(!am_exec(
        "turn = record_get(state, \"turn\")\n"
        "mathbrain = record_get(state, \"mathbrain\")\n"
        "mathbrain_state = record_get(state, \"mathbrain_state\")\n"
        "rae = record_get(state, \"rae\")\n"
        "rae_state = record_get(state, \"rae_state\")\n"
        "voice_rng = record_get(state, \"voice_rng\")\n"
        "rows = record_get(state, \"rows\")\n"
        "counts = record_get(state, \"counts\")\n"
        "vocab = record_get(state, \"vocab\")\n"
        "syllables = record_get(state, \"syllables\")\n"
        "recent = record_get(state, \"recent\")\n"
        "weights = record_get(state, \"weights\")\n"
        "frequencies = record_get(state, \"frequencies\")\n"
        "last_used = record_get(state, \"last_used\")\n"
        "bridge_event = record_get(state, \"bridge_event\")\n"
        "observer_counts = record_get(state, \"observer_counts\")\n"), am_get_error());
    require(am_get_var_float("turn") == turns, "accepted turn count");
    require(field(am_get_var_map("mathbrain_state"), "observations") == turns, "MathBrain count");
    require(field(am_get_var_map("rae_state"), "observations") == rae, "RAE count");
    require(rae || same_array("rae", "fg_initial_rae"), "baseline RAE parameters changed");
    const AM_List *recent = am_get_var_list("recent");
    require(recent && recent->len % 3 == 0 && recent->len <= 30, "recent triples");
    const AM_Map *weights = am_get_var_map("weights");
    const AM_Map *freq = am_get_var_map("frequencies");
    const AM_Map *used = am_get_var_map("last_used");
    const AM_Map *event = am_get_var_map("bridge_event");
    require(weights && freq && used && event && weights->len == freq->len && weights->len == used->len,
            "cloud column sizes");
    if (!turns) {
        require(same_array("mathbrain", "fg_initial_math"), "skipped input trained MathBrain");
        require(same_map(am_get_var_map("voice_rng"), am_get_var_map("fg_initial_rng")), "skipped input consumed RNG");
        require(same_list(am_get_var_list("rows"), am_get_var_list("fg_initial_rows")), "skipped input changed generator rows");
        require(same_map(am_get_var_map("counts"), am_get_var_map("fg_initial_counts")), "skipped input changed generator counts");
        require(same_list(am_get_var_list("vocab"), am_get_var_list("fg_initial_vocab")), "skipped input changed vocabulary");
        require(same_map(am_get_var_map("syllables"), am_get_var_map("fg_initial_syllables")), "skipped input changed syllable cache");
        require(recent->len == 0 && event->len == 0, "skipped input published a recent/event record");
        require(am_get_var_map("observer_counts")->len == 0, "skipped input updated observer");
        require(weights->len == 576, "seed vocabulary changed");
        for (int i = 0; i < weights->len; i++) {
            require(weights->entries[i].value == 1 && freq->entries[i].value == 0 &&
                    used->entries[i].value == 0, "skipped input changed cloud");
        }
    } else {
        require(!same_array("mathbrain", "fg_initial_math"), "accepted exchange did not learn");
        require(event->len == 11 && field(event, "turn") == turns, "bridge handoff");
        for (int i = 0; i < weights->len; i++)
            require(isfinite(weights->entries[i].value) && used->entries[i].value <= turns,
                    "finite cloud and logical clocks");
    }
    am_persistent_clear();
    free(program);
    free(source);
    puts("FOREGROUND_STATE_OK");
    return 0;
}
