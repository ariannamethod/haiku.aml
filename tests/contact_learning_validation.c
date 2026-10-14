/* Every rejected teaching transition keeps the caller's owned bytes intact. */
#include "state_test.h"

static int learning_execute(const char *fragment) {
    size_t size = strlen(root) + strlen(fragment) + 128;
    char *program = malloc(size), origin[4096];
    require(program != NULL, "learning program allocation");
    snprintf(program, size, "IMPORT \"%s/src/parliament_contact.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/learning-validation.aml", work);
    int rc = am_exec_source(program, origin);
    free(program); return rc;
}

static void rejected(const char *fragment) {
    AM_Record *before = am_record_clone(am_get_var_record("live"));
    require(before != NULL, "learning rejection snapshot");
    require(learning_execute(fragment) != 0, "invalid learning transition was accepted");
    require(same_record(before, am_get_var_record("live")), "rejected teaching transition changed an owned leaf");
    am_record_free(before);
}

int main(int argc, char **argv) {
    require(argc == 3, "learning root and work directory");
    root = argv[1]; work = argv[2];
    (void)execute; (void)expect_error;
    am_init(); am_use_notorch(); am_persistent_mode(1);
    char path[4096], fragment[12000];
    snprintf(path, sizeof(path), "%s/interpreted.full", work);
    size_t size = 0;
    unsigned char *bytes = read_file(path, &size);
    snprintf(fragment, sizeof(fragment), "live = haiku_contact_load(\"%s\")", path);
    require(!learning_execute(fragment), am_get_error());
    AM_Record *original = am_record_clone(am_get_var_record("live"));
    require(original != NULL, "learning original snapshot");
    require(!learning_execute(
        "last = haiku_contact_event(\"human\", \"rhythm piano drum\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 12, 12)\n"
        "haiku_contact_learn(live, last)\nhaiku_contact_publish(live, last)"), am_get_error());
    require(same_record(original, am_get_var_record("live")), "learning replay changed any owned state or RNG");

    rejected("unsealed = haiku_contact_event(\"human\", \"mist water river\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 13, 13)\nhaiku_contact_learn(live, unsealed)");
    rejected("old = haiku_contact_event(\"human\", \"mist water river\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 13, 1)\nhaiku_contact_publish(live, old)");
    rejected("forged = haiku_contact_event(\"human\", \"mist water river\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 13, 13)\nrecord_set(forged, \"parent_source\", 4)\nrecord_set(forged, \"parent_event\", 1)\nrecord_set(forged, \"parent_owner\", \"haiku\")\nhaiku_contact_publish(live, forged)");
    rejected("other = haiku_contact_new(\"unassigned\", 49)\nhaiku_contact_enable_learning(other)");
    const char *corruptions[] = {
        "record_set(broken, \"extra\", 1)",
        "record_set(broken, \"version\", 1)",
        "record_set(broken, \"learn_counts\", zeros(1))",
        "clock = record_get(broken, \"learn_clock\")\nclock[0] = 999",
        "clock = record_get(broken, \"learn_clock\")\nclock[4] = 0",
        "rows = record_get(broken, \"learn_rows\")\nrows[2] = 999",
        "rows = record_get(broken, \"learn_rows\")\nrows[3] = 4",
        "rows = record_get(broken, \"learn_rows\")\nrows[6] = 4",
        "counts = record_get(broken, \"learn_counts\")\ncounts[0] = counts[0] + 1",
        "text = record_get(broken, \"learn_text\")\nlist_set(text, 1, \"stream river water\")\nrecord_set(broken, \"learn_text\", text)",
        "text = record_get(broken, \"learn_text\")\nlist_set(text, 0, \"invented\")\nrecord_set(broken, \"learn_text\", text)",
        "rows = record_get(broken, \"learn_rows\")\nrows[9] = 82",
        /* One event cannot acquire a second raw witness in another word slot. */
        "rows = record_get(broken, \"learn_rows\")\nrows[72] = rows[0]\nrows[73] = rows[1]\nrows[74] = rows[2]\nrows[77] = rows[5]",
        /* One learner revision cannot name two independent source events. */
        "rows = record_get(broken, \"learn_rows\")\nrows[74] = rows[2]"
    };
    for (size_t i = 0; i < sizeof(corruptions) / sizeof(corruptions[0]); ++i) {
        require(am_set_var_record("broken", original) == 0, "install corrupt learning candidate");
        require(!learning_execute(corruptions[i]), am_get_error());
        AM_Record *snapshot = am_record_clone(am_get_var_record("broken"));
        require(snapshot != NULL, "corrupt learning snapshot");
        if (!learning_execute("haiku_contact_check(broken)")) {
            fprintf(stderr, "accepted corruption %zu: %s\n", i, corruptions[i]);
            require(0, "corrupt learning state was accepted");
        }
        require(same_record(snapshot, am_get_var_record("broken")), "learning checker mutated corrupt input");
        require(same_record(original, am_get_var_record("live")), "corrupt learning state leaked into live owner");
        am_record_free(snapshot);
    }
    require(!learning_execute(
        "boundary = haiku_contact_new(\"haiku\", 50)\n"
        "past = haiku_contact_event(\"human\", \"water\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 7, 7)\n"
        "haiku_contact_publish(boundary, past)\nboundary = haiku_contact_enable_learning(boundary)\n"
        "later = haiku_contact_event(\"human\", \"stream water river\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 8, 8)\n"
        "haiku_contact_publish(boundary, later)\nrows = record_get(boundary, \"learn_rows\")\nrows[1] = 7"), am_get_error());
    AM_Record *boundary = am_record_clone(am_get_var_record("boundary"));
    require(boundary != NULL, "activation-boundary corrupt snapshot");
    require(learning_execute("haiku_contact_check(boundary)") != 0, "retained evidence was forged onto activation turn");
    require(same_record(boundary, am_get_var_record("boundary")), "boundary checker mutated corrupt input");
    require(same_record(original, am_get_var_record("live")), "boundary corruption leaked into another owner");
    am_record_free(boundary);
    require(!learning_execute(
        "empty = haiku_contact_new(\"haiku\", 51)\nhaiku_contact_publish(empty, past)\n"
        "empty = haiku_contact_enable_learning(empty)\n"
        "plain = haiku_contact_event(\"human\", \"water\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 8, 8)\n"
        "haiku_contact_publish(empty, plain)\nclock = record_get(empty, \"learn_clock\")\nclock[3] = 7"), am_get_error());
    boundary = am_record_clone(am_get_var_record("empty"));
    require(boundary != NULL, "uninformative lived-clock corrupt snapshot");
    require(learning_execute("haiku_contact_check(empty)") != 0, "lived learning clock remained on activation turn");
    require(same_record(boundary, am_get_var_record("empty")), "empty-clock checker mutated corrupt input");
    require(same_record(original, am_get_var_record("live")), "empty-clock corruption leaked into another owner");
    am_record_free(boundary);
    am_record_free(original);
    same_file(path, bytes, size); free(bytes);
    am_persistent_clear();
    puts("PASS: four rejected teaching transitions preserve all owned state");
    puts("PASS: sixteen corrupt learning records are rejected without mutation");
    puts("PASS: replay is exact and the preceding checkpoint remains unchanged");
    return 0;
}
