/* Inspect whole AML organ owners across restart and rejected admissions. */
#include "state_test.h"

static int parliament_execute(const char *fragment) {
    size_t size = strlen(root) + strlen(fragment) + 100;
    char *program = malloc(size), origin[4096];
    require(program != NULL, "parliament program allocation");
    snprintf(program, size, "IMPORT \"%s/src/parliament.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/parliament-validation.aml", work);
    int result = am_exec_source(program, origin);
    free(program);
    return result;
}

static void rejected(const char *fragment, const char *error) {
    AM_Record *before = am_record_clone(am_get_var_record("live"));
    require(before != NULL, "snapshot before rejected parliament call");
    int rc = parliament_execute(fragment);
    if (!rc || !strstr(am_get_error(), error)) {
        fprintf(stderr, "parliament expected: %s\nactual: %s\n", error, am_get_error());
        require(0, "wrong rejection");
    }
    require(same_record(before, am_get_var_record("live")), "rejected parliament call changed owner");
    am_record_free(before);
}

int main(int argc, char **argv) {
    require(argc == 3, "parliament root and work directory");
    root = argv[1]; work = argv[2];
    (void)execute; (void)expect_error;
    am_init(); am_use_notorch(); am_persistent_mode(1);
    require(!parliament_execute(
        "g = zeros(88)\ng[2] = 1\nc = [0.45, 0.5, 2, 0.5]\nr = [0.5, 0.4, 0.3]\n"
        "x = haiku_parliament_features(g, c, r, 0.5)\n"
        "later = haiku_parliament_features(g, c, r, 0.8)\n"
        "origin = haiku_parliament_signature(x, zeros(96), 0)\nboundary = zeros(64)\ni = 0\n"
        "while i < 64:\n    boundary[i] = -origin[i]\n    i = i + 1\n"
        "live = haiku_parliament_new(\"haiku\", 42, origin, boundary)\n"
        "haiku_parliament_step(live, x, 0, 1, 1, 0, 1)"), am_get_error());

    AM_Record *replay_before = am_record_clone(am_get_var_record("live"));
    require(replay_before != NULL, "replay snapshot");
    require(!parliament_execute(
        "haiku_parliament_step(live, x, 0, 1, 1, 0, 1)\n"
        "haiku_parliament_step(live, x, 7, 50, 1, 1, 1)"), am_get_error());
    require(same_record(replay_before, am_get_var_record("live")), "replay changed a leaf or random stream");
    am_record_free(replay_before);

    rejected("haiku_parliament_step(live, x, 2, 2, 2, 0, 1)", "A source is unavailable");
    rejected("haiku_parliament_step(live, x, 3, 2, 2, 0, 1)", "B source is unavailable");
    rejected("haiku_parliament_step(live, x, 4, 2, 2, 0, 3)", "mixed source needs both");
    rejected("haiku_parliament_step(live, x, 1, 2, 2, 0, 0)", "absent rings carry");
    rejected("haiku_parliament_reward(live, 0, 0, 2, 2, later)", "reward event is invalid");
    rejected("haiku_parliament_reward(live, 1, 0, 19, 2, later)", "retained observed witness");
    rejected("haiku_parliament_reward(live, 1, 0, 1, 1, later)", "later turn");
    require(!parliament_execute("haiku_parliament_step(live, later, 7, 19, 2, 0, 1)"), am_get_error());
    rejected("haiku_parliament_reward(live, 1, 0, 19, 2, later)", "retained observed witness");

    /* Native checkpoint is exact, and both owners continue through a real
       observed witness, credit/Oja update and fresh source event identically. */
    char path[4096], error[1024], fragment[10000];
    snprintf(path, sizeof(path), "%s/parliament.state", work);
    require(am_checkpoint_save(am_get_var_record("live"), path, error, sizeof(error)) == 1, error);
    size_t size = 0;
    unsigned char *bytes = read_file(path, &size);
    snprintf(fragment, sizeof(fragment), "resumed = haiku_parliament_load(\"%s\")", path);
    require(!parliament_execute(fragment), am_get_error());
    require(same_record(am_get_var_record("live"), am_get_var_record("resumed")), "checkpoint changed parliament");
    require(!parliament_execute(
        "haiku_parliament_step(live, later, 0, 2, 3, 0, 1)\n"
        "haiku_parliament_step(resumed, later, 0, 2, 3, 0, 1)"), am_get_error());
    rejected("haiku_parliament_reward(live, 1, 0, 2, 3, x)", "climate differs from its witness");
    require(!parliament_execute(
        "haiku_parliament_reward(live, 1, 0, 2, 3, later)\n"
        "haiku_parliament_reward(resumed, 1, 0, 2, 3, later)\n"
        "haiku_parliament_step(live, x, 1, 4, 4, 0, 1)\n"
        "haiku_parliament_step(resumed, x, 1, 4, 4, 0, 1)"), am_get_error());
    require(same_record(am_get_var_record("live"), am_get_var_record("resumed")), "restored parliament continuation differs");
    rejected("haiku_parliament_reward(live, 1, 0, 2, 4, later)", "already rewarded");
    same_file(path, bytes, size);
    free(bytes);

    /* Namespaces belong to sources: model event 100 can be assessed by later
       human event 1. Chronology, not unrelated numeric IDs, makes it later. */
    require(!parliament_execute(
        "other = haiku_parliament_new(\"namespace\", 3, origin, boundary)\n"
        "haiku_parliament_step(other, x, 2, 100, 1, 0, 3)\n"
        "haiku_parliament_step(other, later, 0, 1, 2, 0, 1)\n"
        "haiku_parliament_reward(other, 100, 2, 1, 2, later)"), am_get_error());

    require(!parliament_execute("negative = haiku_parliament_part(x, 0, 96)\nnegative[0] = -0.01"), am_get_error());
    rejected("haiku_parliament_step(live, negative, 1, 5, 5, 0, 1)", "features must be nonnegative");
    rejected("haiku_parliament_signature(negative, zeros(96), 0)", "features must be nonnegative");
    rejected("haiku_parliament_reward(live, 2, 0, 3, 5, negative)", "features must be nonnegative");

    const char *corruptions[] = {
        "record_set(broken, \"extra\", 1)",
        "v = record_get(broken, \"ids\")\nv[1] = v[0]",
        "v = record_get(broken, \"cursor\")\nv[0] = 4",
        "v = record_get(broken, \"credit\")\nv[0] = 1e39",
        "map_set(record_get(broken, \"rng\"), \"state0\", 0.5)",
        "v = record_get(broken, \"memory_rows\")\nv[0] = 2",
        "v = record_get(broken, \"receipt_rows\")\nv[2] = 8",
        "v = record_get(broken, \"receipt_rows\")\nv[8] = 99",
        "v = record_get(broken, \"receipt_gates\")\nv[0] = -0.5",
        "v = record_get(broken, \"receipt_rows\")\nv[6] = 0",
        "v = record_get(broken, \"receipt_x\")\nv[0] = -0.01",
        "record_set(broken, \"origin\", zeros(64))",
        "record_set(broken, \"a\", matrix(64, 96, 0))"
    };
    AM_Record *original = am_record_clone(am_get_var_record("live"));
    require(original != NULL, "original parliament snapshot");
    for (size_t i = 0; i < sizeof(corruptions) / sizeof(corruptions[0]); ++i) {
        require(am_set_var_record("broken", original) == 0, "install corruption candidate");
        require(!parliament_execute(corruptions[i]), am_get_error());
        AM_Record *before = am_record_clone(am_get_var_record("broken"));
        require(before != NULL, "corrupt parliament snapshot");
        require(parliament_execute("haiku_parliament_check(broken)") != 0, "corrupt parliament accepted");
        require(same_record(before, am_get_var_record("broken")), "validation mutated corrupt parliament");
        require(same_record(original, am_get_var_record("live")), "corrupt candidate leaked into live parliament");
        am_record_free(before);
    }
    am_record_free(original);
    am_persistent_clear();
    puts("PASS: parliament rejected calls and thirteen malformed states preserve all owners");
    puts("PASS: exact checkpoint and continued observed reward/Oja state; independent source namespaces");
    return 0;
}
