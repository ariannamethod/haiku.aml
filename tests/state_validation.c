/* Whole-record publication receipts. The host inspects; AML owns behavior. */
#include "state_test.h"

struct BadCase { const char *name, *change, *error; };
static const struct BadCase cases[] = {
    {"version", "record_set(broken, \"version\", 3)", "version is unsupported"},
    {"profile", "record_set(broken, \"profile\", \"other-profile\")", "profile is unsupported"},
    {"extra", "record_set(broken, \"extra\", 0)", "twenty-nine fields"},
    {"missing", "keys = record_keys(broken)\nsmall = record_new()\ni = 0\nwhile i < list_len(keys):\n    key = list_get(keys, i)\n    if text_equal(key, \"clock\") == 0:\n        record_set(small, key, record_get(broken, key))\n    i = i + 1\nbroken = small", "twenty-nine fields"},
    {"type", "record_set(broken, \"turn\", \"three\")", "wrong type"},
    {"clock", "record_set(broken, \"clock\", 2)", "clock differs"},
    {"clock kind", "record_set(broken, \"clock_kind\", \"wall\")", "clock kind is unsupported"},
    {"training flag", "record_set(broken, \"train_rae\", 0.5)", "integer is outside"},
    {"tokenizer mode", "record_set(broken, \"tokenizer_mode\", \"guess\")", "mode is unsupported"},
    {"regex identity", "record_set(broken, \"tokenizer_identity\", \"not-empty\")", "regex state has"},
    {"cloud columns", "map_delete(record_get(broken, \"frequencies\"), \"the\")", "column lengths differ"},
    {"cloud weight", "map_set(record_get(broken, \"weights\"), \"the\", -1)", "weight must be nonnegative"},
    {"cloud frequency", "map_set(record_get(broken, \"frequencies\"), \"the\", 0.5)", "frequency must be an integer"},
    {"cloud time", "map_set(record_get(broken, \"last_used\"), \"the\", 4)", "integer is outside"},
    {"row order", "list_set(record_get(broken, \"rows\"), 0, \"changed\")", "row/count order differs"},
    {"vocab duplicate", "list_set(record_get(broken, \"vocab\"), 0, \"be\")", "duplicate words"},
    {"cache coverage", "map_delete(record_get(broken, \"syllables\"), \"the\")", "cache/vocabulary lengths differ"},
    {"observer counts", "key = list_get(map_keys(record_get(broken, \"observer_counts\")), 0)\nmap_set(record_get(broken, \"observer_counts\"), key, 16777216)", "observer count exceeds"},
    {"observer resonance", "key = list_get(map_keys(record_get(broken, \"observer_resonance\")), 0)\nmap_set(record_get(broken, \"observer_resonance\"), key, -1)", "observer resonance is invalid"},
    {"recent", "record_set(broken, \"recent\", haiku_words(\"absent triple here\"))", "recent triple is absent"},
    {"MathBrain shape", "record_set(broken, \"mathbrain\", zeros(56))", "parameter length differs"},
    {"RAE finite", "bad_params = zeros(57)\nbad_params[3] = 1e39\nrecord_set(broken, \"rae\", bad_params)", "parameters must be finite"},
    {"MathBrain count", "map_set(record_get(broken, \"mathbrain_state\"), \"observations\", 2)", "observations differ"},
    {"MathBrain rate", "map_set(record_get(broken, \"mathbrain_state\"), \"lr\", -1)", "rate must be nonnegative"},
    {"RAE count", "map_set(record_get(broken, \"rae_state\"), \"observations\", 4)", "integer is outside"},
    {"RAE enabled count", "map_set(record_get(broken, \"rae_state\"), \"observations\", 2)", "RAE observations differ from its training history"},
    {"RAE disabled count", "record_set(broken, \"train_rae\", 0)", "RAE observations differ from its training history"},
    {"RAE rate", "map_set(record_get(broken, \"rae_state\"), \"learning_rate\", -1)", "rate must be nonnegative"},
    {"RNG shape", "map_set(record_get(broken, \"voice_rng\"), \"extra\", 0)", "exactly five fields"},
    {"RNG limb", "map_set(record_get(broken, \"model_rng\"), \"state2\", 65536)", "integer is outside"},
    {"RNG algorithm", "map_set(record_get(broken, \"voice_rng\"), \"algorithm\", 2)", "algorithm is unsupported"},
    {"draw cursor", "tape = zeros(1)\ntape[0] = 1\nrecord_set(broken, \"draws\", tape)", "draw cursor is invalid"},
    {"native tape", "tape = zeros(2)\ntape[0] = -1\nrecord_set(broken, \"draws\", tape)", "must contain only -1"},
    {"draw range", "tape = zeros(2)\ntape[1] = 1\nrecord_set(broken, \"draws\", tape)", "draws must be finite"},
    {"bridge quality", "map_set(record_get(broken, \"bridge_event\"), \"quality_after\", 0.123)", "bridge event differs"},
    {"bridge flag", "map_set(record_get(broken, \"bridge_event\"), \"stuck\", 1)", "bridge event differs"}
};

int main(int argc, char **argv) {
    require(argc == 4, "root, temporary work directory, valid midpoint checkpoint");
    root = argv[1]; work = argv[2];
    char error[1024] = {0}, valid_path[4096], bad_path[4096], program[12000];
    snprintf(valid_path, sizeof(valid_path), "%s/valid.state", work);
    snprintf(bad_path, sizeof(bad_path), "%s/invalid.state", work);
    AM_Record *original = am_checkpoint_load(argv[3], error, sizeof(error));
    require(original != NULL, error);
    require(am_checkpoint_save(original, valid_path, error, sizeof(error)) == 1, error);
    size_t valid_size = 0;
    unsigned char *valid_bytes = read_file(valid_path, &valid_size);
    am_init(); am_use_notorch(); am_persistent_mode(1);
    require(am_set_var_record("live", original) == 0, "install live owner");
    require(!execute("haiku_state_check(live, 0)"), am_get_error());

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const struct BadCase *test = &cases[i];
        require(am_set_var_record("broken", original) == 0, "install detached corruption");
        require(!execute(test->change), test->name);
        const AM_Record *broken = am_get_var_record("broken");
        require(broken != NULL && am_checkpoint_save(broken, bad_path, error, sizeof(error)) == 1, error);
        snprintf(program, sizeof(program), "haiku_state_restore(live, \"%s\", 0)", bad_path);
        expect_error(program, test->error);
        require(same_record(am_get_var_record("live"), original), test->name);
        snprintf(program, sizeof(program), "haiku_state_save(broken, \"%s\", 0)", valid_path);
        expect_error(program, test->error);
        require(same_record(am_get_var_record("live"), original), "invalid save changed live state");
        same_file(valid_path, valid_bytes, valid_size);
    }

    /* A checksum/framing rejection also preserves the complete live owner. */
    FILE *bad = fopen(bad_path, "wb");
    require(bad && fwrite(valid_bytes, 1, valid_size - 1, bad) == valid_size - 1, "write truncated checkpoint");
    fclose(bad);
    snprintf(program, sizeof(program), "haiku_state_restore(live, \"%s\", 0)", bad_path);
    require(execute(program) != 0, "accepted truncated checkpoint");
    require(same_record(am_get_var_record("live"), original), "truncated restore changed live");

    /* Restore twice after a real intervening turn. No replay/reseed/merge. */
    require(!execute("haiku_session_turn(live, \"memory crosses process borders\", 0)"), am_get_error());
    require(!same_record(am_get_var_record("live"), original), "successful turn did not change state");
    snprintf(program, sizeof(program), "haiku_state_restore(live, \"%s\", 0)\nhaiku_state_restore(live, \"%s\", 0)", valid_path, valid_path);
    require(!execute(program), am_get_error());
    require(same_record(am_get_var_record("live"), original), "restore is not idempotent");
    same_file(valid_path, valid_bytes, valid_size);

    /* Input has already changed detached memory and consumed one draw when
       the second draw fails. The original live owner must remain exact. */
    require(!execute("tape = zeros(2)\ntape[1] = 0.25\nrecord_set(live, \"draws\", tape)"), am_get_error());
    AM_Record *limited = am_record_clone(am_get_var_record("live"));
    require(limited != NULL, "clone limited tape owner");
    expect_error("haiku_session_turn(live, \"new words reach memory\", 0)", "draw tape exhausted");
    require(same_record(am_get_var_record("live"), limited), "failed staged generation changed live");
    am_record_free(limited);

    /* The CLI's outer candidate boundary: a completed turn cannot publish
       when the temporary checkpoint cannot be created in a missing parent. */
    require(am_set_var_record("live", original) == 0, "restore live for file failure");
    snprintf(program, sizeof(program), "pending = record_clone(live)\nhaiku_session_turn(pending, \"continue through the doorway\", 0)\ncheckpoint_save(pending, \"%s/missing-parent/fail.state\")\nrecord_swap(live, pending)", work);
    require(execute(program) != 0, "accepted save into missing parent");
    require(same_record(am_get_var_record("live"), original), "failed file commit published candidate");
    require(!same_record(am_get_var_record("pending"), original), "file failure did not follow a completed candidate");
    same_file(valid_path, valid_bytes, valid_size);

    am_persistent_clear(); am_record_free(original); free(valid_bytes);
    printf("PASS: %zu semantic restore/save rejections preserve all ordered owners and previous file\n", sizeof(cases) / sizeof(cases[0]));
    puts("PASS: truncated load, idempotent restore, mid-turn exhaustion and precommit I/O failure preserve live state");
    return 0;
}
