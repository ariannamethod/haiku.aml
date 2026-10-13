/* A failed or late owned tail cannot publish half of an already saved life. */
#include "state_test.h"

int main(int argc, char **argv) {
    require(argc == 4, "root, work, complete v3 checkpoint");
    root = argv[1]; work = argv[2];
    char error[1024] = {0}, path[4096], program[24000];
    AM_Record *original = am_checkpoint_load(argv[3], error, sizeof(error));
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
        "SPAWN first:\n"
        "    return haiku_session_tail(candidate, experience, 0)\n"
        "AWAIT first INTO completed\n"
        "haiku_session_result_check(live, completed, experience, 0)\n"), am_get_error());
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
    expect_error("haiku_session_result_check(live, completed, experience, 0)\nrecord_swap(live, completed)",
                 "belongs to another response");
    require(same_record(am_get_var_record("live"), original), "changed receipt published another response");
    require(!execute("record_set(experience, \"base_revision\", record_get(live, \"turn\") - 1)"), am_get_error());
    expect_error("haiku_session_result_check(live, completed, experience, 0)\nrecord_swap(live, completed)",
                 "stale base revision");
    require(same_record(am_get_var_record("live"), original), "stale completion changed published owner");
    same_file(path, bytes, size);

    require(!execute(
        "record_set(experience, \"base_revision\", record_get(live, \"turn\"))\n"
        "history = record_get(completed, \"meta_history\")\n"
        "record_set(experience, \"output\", list_get(history, list_len(history) - 2))\n"
        "haiku_session_result_check(live, completed, experience, 0)\n"
        "record_replace(live, completed)\n"), am_get_error());
    expect_error("haiku_session_result_check(live, completed, experience, 0)\nrecord_swap(live, completed)",
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
