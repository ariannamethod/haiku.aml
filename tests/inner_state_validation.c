/* Inspect all owners across explicit migration and rejected inner publication. */
#include "state_test.h"

struct BadCase { const char *name, *change, *error; };
static const struct BadCase cases[] = {
    {"profile", "record_set(broken, \"profile\", \"english-foreground-v1\")", "profile is unsupported"},
    {"extra field", "record_set(broken, \"extra\", 0)", "thirty-six fields"},
    {"inner boundary type", "record_set(broken, \"inner_started_turn\", \"zero\")", "wrong type"},
    {"inner boundary future", "record_set(broken, \"inner_started_turn\", 4)", "integer is outside"},
    {"inner boundary fractional", "record_set(broken, \"inner_started_turn\", 0.5)", "integer is outside"},
    {"inner history count", "record_set(broken, \"inner_started_turn\", 1)", "history differs from completed inner turns"},
    {"inner history shape", "list_push(record_get(broken, \"meta_history\"), \"extra\")", "complete rows"},
    {"inner metrics length", "record_set(broken, \"meta_metrics\", zeros(1))", "metrics/history lengths differ"},
    {"inner nonfinite climate", "metrics = record_get(broken, \"meta_metrics\")\nmetrics[3] = 1e39", "climate must be finite"},
    {"inner turn sequence", "metrics = record_get(broken, \"meta_metrics\")\ni = 0\nwhile i < len(metrics):\n    metrics[i] = metrics[i] + 1\n    i = i + 5", "history turn differs from its boundary"},
    {"inner climate cross-check", "metrics = record_get(broken, \"meta_metrics\")\nmetrics[len(metrics) - 1] = 0.123", "context differs from its bridge climate"},
    {"bootstrap width", "list_push(record_get(broken, \"meta_bootstrap\"), \"one two three four five six seven eight nine ten eleven\")", "snippet exceeds its bound"},
    {"internal line count", "list_set(record_get(broken, \"meta_history\"), 2, \"one line\")", "inner voice must have three lines"},
    {"ring trace count", "record_set(broken, \"ring_trigrams\", list_slice(record_get(broken, \"ring_trigrams\"), 0, 3))", "Haiku rings require"},
    {"ring score range", "scores = record_get(broken, \"ring_coherence\")\nscores[0] = -1", "ring coherence"},
    {"no rings after completed inner turn", "record_set(broken, \"ring_trigrams\", list_new())\nrecord_set(broken, \"ring_coherence\", zeros(1))\nrecord_set(broken, \"ring_admission\", zeros(1))", "completed inner turn needs three ring traces"},
    {"rings before first inner turn", "record_set(broken, \"inner_started_turn\", record_get(broken, \"turn\"))\nrecord_set(broken, \"meta_bootstrap\", list_new())\nrecord_set(broken, \"meta_history\", list_new())\nrecord_set(broken, \"meta_metrics\", zeros(1))", "fresh inner state has ring traces"},
    {"observer outside cloud", "extra = haiku_words(\"absent-from-this-cloud ring token\")\nhaiku_store_trigrams(record_get(broken, \"observer_rows\"), record_get(broken, \"observer_counts\"), record_get(broken, \"observer_resonance\"), extra)", "observer word is absent from its cloud"},
    {"RAE training history", "map_set(record_get(broken, \"rae_state\"), \"observations\", 2)", "RAE observations differ from its training history"}
};

static int name_is(const AM_String *name, const char *text) {
    return name->byte_len == (int)strlen(text) && !memcmp(name->data, text, strlen(text));
}
static void same_legacy_owners(const AM_Record *legacy, const AM_Record *inner) {
    AM_List *keys = am_record_keys(legacy);
    require(keys != NULL, "legacy key inspection");
    for (int i = 0; i < keys->len; ++i) {
        if (name_is(keys->items[i], "version") || name_is(keys->items[i], "profile")) continue;
        require(same_value(am_record_get(legacy, keys->items[i]), am_record_get(inner, keys->items[i])),
                "migration changed a foreground owner");
    }
    am_list_free(keys);
}

int main(int argc, char **argv) {
    require(argc == 5, "root, temporary work directory, v1 midpoint, v2 midpoint");
    root = argv[1]; work = argv[2];
    char error[1024] = {0}, valid_path[4096], bad_path[4096], program[24000];
    snprintf(valid_path, sizeof(valid_path), "%s/inner-valid.state", work);
    snprintf(bad_path, sizeof(bad_path), "%s/inner-invalid.state", work);
    AM_Record *legacy = am_checkpoint_load(argv[3], error, sizeof(error));
    require(legacy != NULL, error);
    AM_Record *original = am_checkpoint_load(argv[4], error, sizeof(error));
    require(original != NULL, error);
    require(am_checkpoint_save(original, valid_path, error, sizeof(error)) == 1, error);
    size_t valid_size = 0;
    unsigned char *valid_bytes = read_file(valid_path, &valid_size);
    am_init(); am_use_notorch(); am_persistent_mode(1);
    require(am_set_var_record("live", original) == 0, "install complete inner owner");
    require(am_set_var_record("legacy", legacy) == 0, "install legacy owner");
    require(!execute("haiku_state_check(live, 0)\nupgraded = haiku_state_enable_inner(legacy, 0)"), am_get_error());
    require(same_record(am_get_var_record("legacy"), legacy), "migration changed its source");
    same_legacy_owners(legacy, am_get_var_record("upgraded"));
    require(!execute("again = haiku_state_enable_inner(upgraded, 0)"), am_get_error());
    require(same_record(am_get_var_record("upgraded"), am_get_var_record("again")), "migration is not idempotent");
    require(!execute("map_set(record_get(again, \"weights\"), \"the\", 0)"), am_get_error());
    same_legacy_owners(legacy, am_get_var_record("upgraded"));
    require(same_record(am_get_var_record("legacy"), legacy), "upgraded alias escaped into v1");

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const struct BadCase *test = &cases[i];
        require(am_set_var_record("broken", original) == 0, "install detached inner corruption");
        require(!execute(test->change), test->name);
        const AM_Record *broken = am_get_var_record("broken");
        require(broken && am_checkpoint_save(broken, bad_path, error, sizeof(error)) == 1, error);
        snprintf(program, sizeof(program), "haiku_state_restore(live, \"%s\", 0)", bad_path);
        expect_error(program, test->error);
        require(same_record(am_get_var_record("live"), original), test->name);
        snprintf(program, sizeof(program), "haiku_state_save(broken, \"%s\", 0)", valid_path);
        expect_error(program, test->error);
        require(same_record(am_get_var_record("live"), original), "invalid inner save changed live");
        same_file(valid_path, valid_bytes, valid_size);
    }

    /* Rings can add an observer-only triple, including an exact whitespace-
       bearing cloud token. These identities never enter the Markov generator. */
    require(am_set_var_record("checked", original) == 0, "install observer-only candidate");
    require(!execute(
        "extra = list_new()\nlist_push(extra, \"two words\")\nlist_push(extra, \"rain\")\nlist_push(extra, \"moon\")\n"
        "haiku_cloud_morph(record_get(checked, \"weights\"), record_get(checked, \"frequencies\"), record_get(checked, \"last_used\"), record_get(checked, \"origins\"), extra, record_get(checked, \"clock\"))\n"
        "haiku_store_trigrams(record_get(checked, \"observer_rows\"), record_get(checked, \"observer_counts\"), record_get(checked, \"observer_resonance\"), extra)\n"
        "assert(map_has(record_get(checked, \"counts\"), list_key(extra)) == 0, \"observer-only triple reached generator\")\n"
        "haiku_state_check(checked, 0)"), am_get_error());

    /* A truncated frame cannot partially replace any of the 36 leaves. */
    FILE *bad = fopen(bad_path, "wb");
    require(bad && fwrite(valid_bytes, 1, valid_size - 1, bad) == valid_size - 1, "write truncated inner checkpoint");
    fclose(bad);
    snprintf(program, sizeof(program), "haiku_state_restore(live, \"%s\", 0)", bad_path);
    require(execute(program) != 0, "accepted truncated inner checkpoint");
    require(same_record(am_get_var_record("live"), original), "truncated inner restore changed live");

    /* Measure the event boundaries with the same initial state and draws, then
       exhaust inside reflection and rings, after foreground learning occurred. */
    require(!execute(
        "tape = zeros(4097)\ni = 1\nwhile i < len(tape):\n    tape[i] = 0.25\n    i = i + 1\n"
        "fore = record_clone(legacy)\nrecord_set(fore, \"draws\", tape)\n"
        "spoken = haiku_session_turn(fore, \"continue through the doorway\", 0)\n"
        "fore_tape = record_get(fore, \"draws\")\ncut_meta = fore_tape[0] + 1\n"
        "probe = haiku_state_enable_inner(fore, 0)\nobservation = zeros(4)\n"
        "names = haiku_words(\"dissonance novelty arousal entropy\")\ni = 0\nwhile i < 4:\n"
        "    observation[i] = map_get(record_get(fore, \"bridge_event\"), list_get(names, i))\n    i = i + 1\n"
        "haiku_meta_reflect(probe, \"continue through the doorway\", spoken, observation, record_get(fore, \"turn\"))\n"
        "probe_tape = record_get(probe, \"draws\")\ncut_rings = probe_tape[0] + 1"), am_get_error());
    int cuts[2] = {(int)am_get_var_float("cut_meta"), (int)am_get_var_float("cut_rings")};
    require(cuts[0] > 1 && cuts[1] > cuts[0], "inner cut points did not advance");
    for (int i = 0; i < 2; ++i) {
        snprintf(program, sizeof(program),
            "limited = haiku_state_enable_inner(legacy, 0)\ntape = zeros(%d)\ni = 1\n"
            "while i < len(tape):\n    tape[i] = 0.25\n    i = i + 1\nrecord_set(limited, \"draws\", tape)", cuts[i] + 1);
        require(!execute(program), am_get_error());
        AM_Record *before = am_record_clone(am_get_var_record("limited"));
        require(before != NULL, "clone bounded inner state");
        expect_error("haiku_session_turn(limited, \"continue through the doorway\", 0)", "draw tape exhausted");
        require(same_record(am_get_var_record("limited"), before), i ? "ring failure changed live leaves" : "Meta failure changed live leaves");
        same_file(valid_path, valid_bytes, valid_size);
        am_record_free(before);
    }

    /* The outer persistence boundary publishes neither a completed inner turn
       nor an explicit migration when the target file cannot be replaced. */
    snprintf(program, sizeof(program),
        "pending = record_clone(live)\nhaiku_session_turn(pending, \"continue through the doorway\", 0)\n"
        "checkpoint_save(pending, \"%s/missing-parent/inner.state\")\nrecord_swap(live, pending)", work);
    require(execute(program) != 0, "accepted inner save into missing parent");
    require(same_record(am_get_var_record("live"), original), "failed inner file commit published candidate");
    require(!same_record(am_get_var_record("pending"), original), "inner I/O failure preceded completed candidate");
    snprintf(program, sizeof(program),
        "migration = haiku_state_enable_inner(legacy, 0)\ncheckpoint_save(migration, \"%s/missing-parent/migration.state\")\nrecord_swap(legacy, migration)", work);
    require(execute(program) != 0, "accepted migration save into missing parent");
    require(same_record(am_get_var_record("legacy"), legacy), "failed migration file commit changed v1");
    same_file(valid_path, valid_bytes, valid_size);

    am_persistent_clear(); am_record_free(legacy); am_record_free(original); free(valid_bytes);
    printf("PASS: %zu inner semantic restore/save rejections preserve all 36 leaves and previous file\n", sizeof(cases) / sizeof(cases[0]));
    printf("PASS: detached/idempotent migration, observer-only identities, Meta/ring exhaustion at draws %d/%d, and failed file publication\n", cuts[0], cuts[1]);
    return 0;
}
