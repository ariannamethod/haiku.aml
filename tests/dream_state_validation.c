/* Inspect every owner across dream migration and failed turn publication. */
#include "state_test.h"

struct BadCase { const char *name, *change, *error; };
static const struct BadCase cases[] = {
    {"profile", "record_set(broken, \"profile\", \"english-inner-v2\")", "profile is unsupported"},
    {"extra field", "record_set(broken, \"extra\", 0)", "fifty-five fields"},
    {"boundary type", "record_set(broken, \"phase4_started_turn\", \"zero\")", "wrong type"},
    {"boundary future", "record_set(broken, \"phase4_started_turn\", 8)", "integer is outside"},
    {"boundary count", "record_set(broken, \"phase4_started_turn\", 1)", "bridge history differs from completed dream turns"},
    {"previous state", "record_set(broken, \"phase4_prev\", \"different\")", "previous state differs"},
    {"turn identity", "list_set(record_get(broken, \"phase4_log\"), 2, \"turn_2\")", "bridge turn identity differs"},
    {"logical clock", "values = record_get(broken, \"phase4_log_rows\")\nvalues[0] = 0", "bridge turn clock differs"},
    {"activation count", "values = record_get(broken, \"phase4_snapshots\")\nvalues[0] = values[0] + 1", "activation count differs"},
    {"aggregate count", "values = record_get(broken, \"phase4_totals\")\nvalues[0] = values[0] + 1", "aggregate differs from history"},
    {"metric nonfinite", "values = record_get(broken, \"phase4_metric_rows\")\nvalues[1] = 1e39", "metric value is invalid"},
    {"metric dictionary", "list_push(record_get(broken, \"phase4_metric_names\"), \"unused\")", "dictionary has unused"},
    {"dream config type", "record_set(broken, \"dream_config\", map_new())", "wrong type"},
    {"dream interval", "values = record_get(broken, \"dream_config\")\nvalues[0] = 0", "dream interval is invalid"},
    {"dream probability", "values = record_get(broken, \"dream_config\")\nvalues[1] = 1.1", "dream probability is invalid"},
    {"dream exchanges", "values = record_get(broken, \"dream_config\")\nvalues[2] = 1.5", "dream exchange count is invalid"},
    {"dream fragment shape", "record_set(broken, \"dream_weights\", zeros(1))", "fragment/weight lengths differ"},
    {"dream fragment line", "list_set(record_get(broken, \"dream_fragments\"), 0, \"two\\nlines\")", "one stripped line"},
    {"dream fragment weight", "values = record_get(broken, \"dream_weights\")\nvalues[0] = 0.05", "fragment weight is invalid"},
    {"dream dialog shape", "record_set(broken, \"dream_dialogs\", zeros(2))", "complete rows"},
    {"dream dialog offset", "values = record_get(broken, \"dream_dialogs\")\nvalues[1] = 1", "text offset differs"},
    {"dream orphan text", "list_push(record_get(broken, \"dream_history\"), \"orphan\")", "dialog/history lengths differ"},
    {"dream generated lines", "list_set(record_get(broken, \"dream_history\"), 1, \"one line\")", "generated voice must have three lines"},
    {"dream opening", "list_set(record_get(broken, \"dream_history\"), 0, \"a different opening\")", "dream opening differs from its spoken turn"},
    {"dream last turn", "record_set(broken, \"dream_last_turn\", 0)", "last turn differs from its history"},
    {"dream future turn", "values = record_get(broken, \"dream_dialogs\")\nvalues[len(values) - 3] = 8\nrecord_set(broken, \"dream_last_turn\", 8)", "integer is outside"},
    {"provenance unknown word", "map_set(record_get(broken, \"dream_vocab\"), \"unknown-provenance-word\", record_get(broken, \"dream_last_turn\"))", "dream provenance word is absent"},
    {"provenance before boundary", "map_set(record_get(broken, \"dream_vocab\"), \"the\", 0)", "integer is outside"},
    {"provenance no dialog", "map_set(record_get(broken, \"dream_vocab\"), \"the\", 1)", "dream provenance turn has no dialog"},
    {"journal shape", "list_push(record_get(broken, \"journal_text\"), \"orphan\")", "Haiku journal"},
    {"journal nonfinite", "values = record_get(broken, \"journal_values\")\nvalues[0] = 1e39", "Haiku journal"}
};

static int name_is(const AM_String *name, const char *text) {
    return name->byte_len == (int)strlen(text) && !memcmp(name->data, text, strlen(text));
}
static void same_inherited_owners(const AM_Record *before, const AM_Record *after) {
    AM_List *keys = am_record_keys(before);
    require(keys != NULL, "inherited key inspection");
    for (int i = 0; i < keys->len; ++i) {
        if (name_is(keys->items[i], "version") || name_is(keys->items[i], "profile")) continue;
        require(same_value(am_record_get(before, keys->items[i]), am_record_get(after, keys->items[i])),
                "dream migration changed an inherited owner");
    }
    am_list_free(keys);
}

int main(int argc, char **argv) {
    require(argc == 6, "root, work, v1 midpoint, v2 midpoint, v3 complete checkpoint");
    root = argv[1]; work = argv[2];
    char error[1024] = {0}, valid_path[4096], bad_path[4096], program[24000];
    snprintf(valid_path, sizeof(valid_path), "%s/dream-valid.state", work);
    snprintf(bad_path, sizeof(bad_path), "%s/dream-invalid.state", work);
    AM_Record *legacy = am_checkpoint_load(argv[3], error, sizeof(error));
    require(legacy != NULL, error);
    AM_Record *inner = am_checkpoint_load(argv[4], error, sizeof(error));
    require(inner != NULL, error);
    AM_Record *original = am_checkpoint_load(argv[5], error, sizeof(error));
    require(original != NULL, error);
    require(am_checkpoint_save(original, valid_path, error, sizeof(error)) == 1, error);
    size_t valid_size = 0;
    unsigned char *valid_bytes = read_file(valid_path, &valid_size);
    am_init(); am_use_notorch(); am_persistent_mode(1);
    require(am_set_var_record("live", original) == 0, "install complete dream owner");
    require(am_set_var_record("legacy", legacy) == 0, "install foreground owner");
    require(am_set_var_record("inner", inner) == 0, "install inner owner");
    require(!execute("haiku_state_check(live, 0)\nfirst = haiku_state_enable_dreams(legacy, 0)\nsecond = haiku_state_enable_dreams(inner, 0)"), am_get_error());
    require(same_record(am_get_var_record("legacy"), legacy), "dream migration changed v1");
    require(same_record(am_get_var_record("inner"), inner), "dream migration changed v2");
    same_inherited_owners(legacy, am_get_var_record("first"));
    same_inherited_owners(inner, am_get_var_record("second"));
    require(!execute(
        "assert(record_get(first, \"inner_started_turn\") == 3, \"v1 inner boundary differs\")\n"
        "assert(record_get(first, \"phase4_started_turn\") == 3, \"v1 dream boundary differs\")\n"
        "assert(record_get(second, \"inner_started_turn\") == 0, \"v2 inner boundary changed\")\n"
        "assert(record_get(second, \"phase4_started_turn\") == 3, \"v2 dream boundary differs\")\n"
        "config = record_get(first, \"dream_config\")\n"
        "assert(config[0] == 10 and config[1] == 0.25 and config[2] == 4, \"dream defaults differ\")\n"
        "again = haiku_state_enable_dreams(live, 0)\nolder_api = haiku_state_enable_inner(live, 0)"), am_get_error());
    require(same_record(am_get_var_record("again"), original), "dream migration is not idempotent");
    require(same_record(am_get_var_record("older_api"), original), "inner upgrade downgraded an existing dream life");
    require(!execute("map_set(record_get(again, \"weights\"), \"the\", 0)"), am_get_error());
    require(same_record(am_get_var_record("live"), original), "dream clone alias escaped into live");

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const struct BadCase *test = &cases[i];
        require(am_set_var_record("broken", original) == 0, "install detached dream corruption");
        require(!execute(test->change), test->name);
        require(am_checkpoint_save(am_get_var_record("broken"), bad_path, error, sizeof(error)) == 1, error);
        snprintf(program, sizeof(program), "haiku_state_restore(live, \"%s\", 0)", bad_path);
        expect_error(program, test->error);
        require(same_record(am_get_var_record("live"), original), test->name);
        snprintf(program, sizeof(program), "haiku_state_save(broken, \"%s\", 0)", valid_path);
        expect_error(program, test->error);
        require(same_record(am_get_var_record("live"), original), "invalid dream save changed live");
        same_file(valid_path, valid_bytes, valid_size);
    }

    /* The inner prefix consumes the same draws in v2 and v3. Exhaust directly
       after the dream gate, while its first reply is being generated. */
    require(!execute(
        "tape = zeros(8193)\ni = 1\nwhile i < len(tape):\n"
        "    draw = i * 5 + 3\n    tape[i] = (draw - floor(draw / 16) * 16) / 16\n    i = i + 1\n"
        "prefix = record_clone(inner)\nrecord_set(prefix, \"draws\", tape)\n"
        "haiku_session_turn(prefix, \"!!!\", 0)\n"
        "prefix_tape = record_get(prefix, \"draws\")\ncut = prefix_tape[0] + 1"), am_get_error());
    int cut = (int)am_get_var_float("cut");
    require(cut > 1, "dream cut did not advance beyond foreground");
    snprintf(program, sizeof(program),
        "limited = haiku_state_enable_dreams(inner, 0)\nrecord_set(limited, \"dream_config\", [1, 1, 4])\n"
        "tape = zeros(%d)\ni = 1\nwhile i < len(tape):\n"
        "    draw = i * 5 + 3\n    tape[i] = (draw - floor(draw / 16) * 16) / 16\n    i = i + 1\n"
        "record_set(limited, \"draws\", tape)", cut + 1);
    require(!execute(program), am_get_error());
    AM_Record *limited = am_record_clone(am_get_var_record("limited"));
    require(limited != NULL, "clone limited dream state");
    expect_error("haiku_session_turn(limited, \"!!!\", 0)", "draw tape exhausted");
    require(same_record(am_get_var_record("limited"), limited), "dream failure published a partial life");
    same_file(valid_path, valid_bytes, valid_size);
    am_record_free(limited);

    /* Failed file publication retains all completed legacy and new owners. */
    snprintf(program, sizeof(program),
        "pending = record_clone(live)\nhaiku_session_turn(pending, \"!!!\", 0)\n"
        "checkpoint_save(pending, \"%s/missing-parent/dream.state\")\nrecord_swap(live, pending)", work);
    require(execute(program) != 0, "accepted dream save into missing parent");
    require(same_record(am_get_var_record("live"), original), "failed dream commit published candidate");
    require(!same_record(am_get_var_record("pending"), original), "file failure did not follow completed dream candidate");
    snprintf(program, sizeof(program),
        "migration = haiku_state_enable_dreams(inner, 0)\n"
        "checkpoint_save(migration, \"%s/missing-parent/upgrade.state\")\nrecord_swap(inner, migration)", work);
    require(execute(program) != 0, "accepted dream migration save into missing parent");
    require(same_record(am_get_var_record("inner"), inner), "failed migration commit changed old profile");
    same_file(valid_path, valid_bytes, valid_size);

    am_persistent_clear(); am_record_free(legacy); am_record_free(inner); am_record_free(original); free(valid_bytes);
    printf("PASS: %zu dream semantic restore/save rejections preserve all 55 leaves and previous file\n", sizeof(cases) / sizeof(cases[0]));
    printf("PASS: detached/idempotent v1/v2 migration, dream exhaustion at draw %d, and failed file publication\n", cut);
    return 0;
}
