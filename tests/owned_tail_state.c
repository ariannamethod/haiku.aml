/* A failed or late owned tail cannot publish half of an already saved life. */
#include "state_test.h"
#include <math.h>

/* Receipt errors must leave every leaf, including RNG and learner bits, with
   its previous owner. A persisted checkpoint stays outside the failed path. */
static void check_receipts(const char *checkpoint, int profile, int fields) {
    char error[1024] = {0}, path[4096];
    AM_Record *original = am_checkpoint_load(checkpoint, error, sizeof(error));
    require(original != NULL, error);
    AM_List *keys = am_record_keys(original);
    require(keys && keys->len == fields, "receipt profile has the wrong schema");
    am_list_free(keys);
    snprintf(path, sizeof(path), "%s/receipt-v%d.state", work, profile);
    require(am_checkpoint_save(original, path, error, sizeof(error)) == 1, error);
    size_t size = 0;
    unsigned char *bytes = read_file(path, &size);
    am_init(); am_use_notorch(); am_persistent_mode(1);
    require(am_set_var_record("live", original) == 0, "install receipt-test owner");
    require(!execute(
        "moon = record_clone(live)\n"
        "moon_receipt = haiku_session_prepare(moon, \"moon\", 0)\n"
        "rain = record_clone(live)\n"
        "rain_receipt = haiku_session_prepare(rain, \"rain\", 0)\n"
        "SPAWN receipt_tail:\n"
        "    return haiku_session_tail(moon, moon_receipt, 0)\n"
        "AWAIT receipt_tail INTO result\n"
        "completed = haiku_session_result_check(live, result, moon_receipt, 0)\n"), am_get_error());
    AM_Record *result = am_record_clone(am_get_var_record("result"));
    AM_Record *receipt = am_record_clone(am_get_var_record("moon_receipt"));
    AM_Record *completed = am_record_clone(am_get_var_record("completed"));
    require(result && receipt && completed, "retain receipt-test owners");
    expect_error(
        "haiku_session_result_check(live, result, rain_receipt, 0)\n"
        "record_replace(live, completed)", "belongs to another experience");
    require(same_record(am_get_var_record("live"), original), "same-base completion changed owner");
    require(same_record(am_get_var_record("result"), result), "rejected receipt changed result");
    same_file(path, bytes, size);

    const char *changes[] = {
        "record_set(experience, \"quality\", record_get(experience, \"quality\") + 0.01)",
        "observation = record_get(experience, \"observation\")\nobservation[0] = observation[0] + 0.01\nrecord_set(experience, \"observation\", observation)",
        "bridge = record_get(experience, \"bridge_event\")\nmap_set(bridge, \"quality_after\", map_get(bridge, \"quality_after\") + 0.01)\nrecord_set(experience, \"bridge_event\", bridge)",
        "record_set(experience, \"quality\", \"same text, another value type\")"
    };
    for (size_t i = 0; i < sizeof(changes) / sizeof(changes[0]); ++i) {
        require(am_set_var_record("experience", receipt) == 0, "restore full receipt");
        require(!execute(changes[i]), am_get_error());
        require(!execute(
            "assert(text_equal(record_get(experience, \"input\"), record_get(moon_receipt, \"input\")), \"changed receipt input\")\n"
            "assert(text_equal(record_get(experience, \"output\"), record_get(moon_receipt, \"output\")), \"changed receipt response\")"), am_get_error());
        AM_Record *changed = am_record_clone(am_get_var_record("experience"));
        require(changed && !same_record(changed, receipt), changes[i]);
        expect_error(
            "haiku_session_result_check(live, result, experience, 0)\n"
            "record_replace(live, completed)", "belongs to another experience");
        require(same_record(am_get_var_record("experience"), changed), "rejection rewrote supplied experience");
        require(same_record(am_get_var_record("live"), original), "changed experience published worker state");
        require(same_record(am_get_var_record("completed"), completed), "rejection changed detached state");
        require(same_record(am_get_var_record("result"), result), "rejection changed frozen envelope");
        same_file(path, bytes, size);
        am_record_free(changed);
    }

    /* Compare binary values, including a one-ULP feature and signed zero. */
    for (int change = 0; change < 2; ++change) {
        require(am_set_var_record("experience", receipt) == 0, "restore bit-exact receipt");
        AM_String *key = am_string_new("values");
        const AML_Var *value = am_record_get(am_get_var_record("experience"), key);
        require(value && value->type == AML_TYPE_ARRAY, "find captured features");
        if (change == 0) value->array->data[3] = nextafterf(value->array->data[3], INFINITY);
        else { require(value->array->data[2] == 0, "capture cloud-size slot is not zero"); value->array->data[2] = -0.0f; }
        am_string_free(key);
        expect_error(
            "haiku_session_result_check(live, result, experience, 0)\n"
            "record_replace(live, completed)", "belongs to another experience");
        require(same_record(am_get_var_record("live"), original), "receipt bits changed live state");
        require(same_record(am_get_var_record("result"), result), "receipt bits changed envelope");
        same_file(path, bytes, size);
    }

    const char *envelope_changes[] = {
        "record_set(result, \"version\", 2)",
        "record_set(result, \"state_keys\", list_new())",
        "record_set(result, \"experience_keys\", list_new())",
        "record_set(result, \"extra\", 1)",
        "record_set(result, \"experience:quality\", -1)",
        "record_set(result, \"state:turn\", record_get(result, \"state:turn\") + 1)"
    };
    const char *diagnostics[] = {
        "envelope is unsupported", "state fields differ", "experience fields differ",
        "envelope field count differs", "belongs to another experience", "tail revision differs"
    };
    for (size_t i = 0; i < sizeof(envelope_changes) / sizeof(envelope_changes[0]); ++i) {
        require(am_set_var_record("result", result) == 0, "restore envelope");
        require(!execute(envelope_changes[i]), am_get_error());
        AM_Record *changed = am_record_clone(am_get_var_record("result"));
        require(changed != NULL, "retain changed envelope");
        expect_error(
            "haiku_session_result_check(live, result, moon_receipt, 0)\n"
            "record_replace(live, completed)", diagnostics[i]);
        require(same_record(am_get_var_record("result"), changed), "validation rewrote envelope");
        require(same_record(am_get_var_record("live"), original), "bad envelope published state");
        require(same_record(am_get_var_record("completed"), completed), "bad envelope changed prior result");
        require(same_record(am_get_var_record("moon_receipt"), receipt), "bad envelope changed original experience");
        same_file(path, bytes, size);
        am_record_free(changed);
    }

    require(am_set_var_record("result", result) == 0, "restore joined envelope");
    require(!execute(
        "map_set(record_get(result, \"state:weights\"), \"the\", 0)\n"
        "record_set(result, \"experience:output\", \"changed after unwrap\")"), am_get_error());
    require(same_record(am_get_var_record("completed"), completed), "unwrapped state aliases envelope");
    require(same_record(am_get_var_record("moon_receipt"), receipt), "envelope aliases original experience");
    require(same_record(am_get_var_record("live"), original), "envelope mutation changed owner");
    same_file(path, bytes, size);
    am_persistent_clear(); am_record_free(original); am_record_free(result);
    am_record_free(receipt); am_record_free(completed); free(bytes);
    printf("PASS: v%d full receipt rejects 13 mismatches; all %d owner leaves and checkpoint bytes survive\n", profile, fields);
}

int main(int argc, char **argv) {
    require(argc == 6, "root, work, complete v1/v2/v3 checkpoints");
    root = argv[1]; work = argv[2];
    check_receipts(argv[3], 1, 29);
    check_receipts(argv[4], 2, 36);
    check_receipts(argv[5], 3, 55);
    char error[1024] = {0}, path[4096], program[24000];
    AM_Record *original = am_checkpoint_load(argv[5], error, sizeof(error));
    require(original != NULL, error);
    snprintf(path, sizeof(path), "%s/owned-valid.state", work);
    require(am_checkpoint_save(original, path, error, sizeof(error)) == 1, error);
    size_t size = 0;
    unsigned char *bytes = read_file(path, &size);
    am_init(); am_use_notorch(); am_persistent_mode(1);
    require(am_set_var_record("live", original) == 0, "install published owner");
    require(!execute(
        "candidate = record_clone(live)\n"
        "experience = haiku_session_prepare(candidate, \"memory crosses the quiet\", 0)\n"
        "receipt = record_clone(experience)\n"
        "SPAWN first:\n"
        "    return haiku_session_tail(candidate, experience, 0)\n"
        "AWAIT first INTO result\n"
        "completed = haiku_session_result_check(live, result, experience, 0)\n"), am_get_error());
    require(same_record(am_get_var_record("live"), original), "completed worker published before its owner");
    require(!same_record(am_get_var_record("completed"), original), "worker did not produce a new life");
    same_file(path, bytes, size);

    /* A detached result remains independent after its thread has exited. */
    AM_Record *completed = am_record_clone(am_get_var_record("completed"));
    require(completed != NULL, "retain completed result");
    require(!execute(
        "map_set(record_get(candidate, \"weights\"), \"the\", 0)\n"
        "record_set(experience, \"output\", \"a later parent mutation\")\n"), am_get_error());
    require(same_record(am_get_var_record("completed"), completed), "parent mutation changed joined result");
    expect_error("haiku_session_result_check(live, result, experience, 0)\nrecord_swap(live, completed)",
                 "belongs to another experience");
    require(same_record(am_get_var_record("live"), original), "changed receipt published another response");
    require(!execute("record_set(experience, \"base_revision\", record_get(live, \"turn\") - 1)"), am_get_error());
    expect_error("haiku_session_result_check(live, result, experience, 0)\nrecord_swap(live, completed)",
                 "stale base revision");
    require(same_record(am_get_var_record("live"), original), "stale completion changed published owner");
    same_file(path, bytes, size);

    require(!execute(
        "experience = record_clone(receipt)\n"
        "completed = haiku_session_result_check(live, result, experience, 0)\n"
        "record_replace(live, completed)\n"), am_get_error());
    expect_error("haiku_session_result_check(live, result, experience, 0)\nrecord_swap(live, completed)",
                 "stale base revision");
    require(same_record(am_get_var_record("live"), completed), "duplicate completion changed accepted owner");
    same_file(path, bytes, size);
    require(am_set_var_record("live", original) == 0, "restore prior published owner");

    /* Budget precisely the foreground draws. Learning completes in the child,
       then the first reflective draw fails. All 55 live leaves remain intact. */
    require(!execute(
        "limited = record_clone(live)\n"
        "tape = zeros(8193)\ni = 1\nwhile i < len(tape):\n"
        "    draw = i * 5 + 3\n    tape[i] = (draw - floor(draw / 16) * 16) / 16\n    i = i + 1\n"
        "record_set(limited, \"draws\", tape)\n"
        "prefix = record_clone(limited)\n"
        "prefix_experience = haiku_session_prepare(prefix, \"!!!\", 0)\n"
        "prefix_draws = record_get(prefix, \"draws\")\ncut = prefix_draws[0]\n"), am_get_error());
    int cut = (int)am_get_var_float("cut");
    require(cut > 0, "foreground did not consume scripted draws");
    snprintf(program, sizeof(program),
        "tape = zeros(%d)\ni = 1\nwhile i < len(tape):\n"
        "    draw = i * 5 + 3\n    tape[i] = (draw - floor(draw / 16) * 16) / 16\n    i = i + 1\n"
        "record_set(limited, \"draws\", tape)", cut + 1);
    require(!execute(program), am_get_error());
    AM_Record *limited = am_record_clone(am_get_var_record("limited"));
    require(limited != NULL, "retain draw-limited owner");
    expect_error("haiku_session_worker_turn(limited, \"!!!\", 0)", "draw tape exhausted");
    require(same_record(am_get_var_record("limited"), limited), "failed worker published foreground or partial learning");
    require(same_record(am_get_var_record("live"), original), "failed worker changed prior complete life");
    same_file(path, bytes, size);

    /* A later good worker succeeds after the failed job was joined. Its file
       failure still leaves the previous live record and checkpoint together. */
    snprintf(program, sizeof(program),
        "candidate = record_clone(live)\n"
        "haiku_session_worker_turn(candidate, \"memory returns after failure\", 0)\n"
        "checkpoint_save(candidate, \"%s/missing-parent/owned.state\")\n"
        "record_swap(live, candidate)", work);
    expect_error(program, "temporary create failed");
    require(same_record(am_get_var_record("live"), original), "failed save published worker result");
    require(!same_record(am_get_var_record("candidate"), original), "recovery worker did not complete");
    same_file(path, bytes, size);

    am_persistent_clear(); am_record_free(original); am_record_free(completed);
    am_record_free(limited); free(bytes);
    printf("PASS: typed result survives join; stale/duplicate completion and tail failure preserve all 55 leaves\n");
    printf("PASS: foreground draw boundary %d, failed-worker recovery, and failed save preserve prior checkpoint\n", cut);
    return 0;
}
