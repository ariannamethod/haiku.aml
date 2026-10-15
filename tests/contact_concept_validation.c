/* Closed snapshots derive from external history; rejection preserves inputs. */
#include "state_test.h"

static int concept_execute(const char *fragment) {
    size_t size = strlen(root) + strlen(fragment) + 160;
    char *program = malloc(size), origin[4096];
    require(program != NULL, "concept program allocation");
    snprintf(program, size, "IMPORT \"%s/src/parliament_contact.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/concept-validation.aml", work);
    int rc = am_exec_source(program, origin);
    free(program);
    return rc;
}

int main(int argc, char **argv) {
    require(argc == 3, "concept root and work directory");
    root = argv[1]; work = argv[2];
    (void)execute; (void)expect_error;
    am_init(); am_use_notorch(); am_persistent_mode(1);
    char path[4096], fragment[18000];
    snprintf(path, sizeof(path), "%s/interpreted.full", work);
    size_t bytes_size = 0;
    unsigned char *bytes = read_file(path, &bytes_size);
    snprintf(fragment, sizeof(fragment),
             "lived = haiku_contact_load(\"%s/interpreted.full\")\n"
             "haiku_contact_concepts_check(lived, 1)\n"
             "legacy = haiku_contact_load(\"%s/interpreted.legacy\")", work, work);
    require(!concept_execute(fragment), am_get_error());
    AM_Record *original = am_record_clone(am_get_var_record("lived"));
    AM_Record *legacy = am_record_clone(am_get_var_record("legacy"));
    require(original && legacy, "concept checkpoint snapshots");

    const char *corruptions[] = {
        "record_set(broken, \"extra\", 1)",
        "record_set(broken, \"concept_clock\", zeros(4))",
        "record_set(broken, \"concept_counts\", zeros(20))",
        "clock = record_get(broken, \"concept_clock\")\nclock[0] = 3",
        "clock = record_get(broken, \"concept_clock\")\nclock[0] = 0",
        "clock = record_get(broken, \"concept_clock\")\nclock[1] = 0",
        "clock = record_get(broken, \"concept_clock\")\nclock[1] = 999999",
        "clock = record_get(broken, \"concept_clock\")\nclock[2] = 0",
        "clock = record_get(broken, \"concept_clock\")\nclock[2] = 8",
        "clock = record_get(broken, \"concept_clock\")\nclock[3] = 8",
        "clock = record_get(broken, \"concept_clock\")\nclock[4] = clock[4] - 1",
        "counts = record_get(broken, \"concept_counts\")\ncounts[0] = -1",
        "counts = record_get(broken, \"concept_counts\")\ncounts[0] = 129",
        "counts = record_get(broken, \"concept_counts\")\ncounts[0] = 1.5",
        "counts = record_get(broken, \"concept_counts\")\ncounts[0] = counts[0] + 1",
        "counts = record_get(broken, \"concept_counts\")\ncounts[20] = 1",
        "identity = record_get(broken, \"concept_identity\")\nlist_set(identity, 1, \"other-teachers\")\nrecord_set(broken, \"concept_identity\", identity)",
        "values = record_get(broken, \"concept_units_a\")\nvalues[9407] = 1e-30",
        "values = record_get(broken, \"concept_units_b\")\nvalues[0] = 1",
        "values = record_get(broken, \"concept_tokens\")\nvalues[692] = 1"
    };
    size_t corrupt_count = sizeof(corruptions) / sizeof(corruptions[0]);
    for (size_t i = 0; i < corrupt_count; ++i) {
        require(am_set_var_record("broken", original) == 0, "install corrupt concept candidate");
        require(!concept_execute(corruptions[i]), am_get_error());
        AM_Record *snapshot = am_record_clone(am_get_var_record("broken"));
        require(snapshot != NULL, "corrupt concept snapshot");
        if (!concept_execute("haiku_contact_check(broken)\nhaiku_contact_concepts_check(broken, 1)")) {
            fprintf(stderr, "accepted concept corruption %zu: %s\n", i, corruptions[i]);
            require(0, "corrupt concept snapshot was accepted");
        }
        require(same_record(snapshot, am_get_var_record("broken")), "concept checker mutated its corrupt input");
        require(same_record(original, am_get_var_record("lived")), "corrupt concept state changed another owner");
        am_record_free(snapshot);
    }

    const char *bad_modes[] = {
        "haiku_contact_enable_concepts(candidate, -1)",
        "haiku_contact_enable_concepts(candidate, 3)"
    };
    for (size_t i = 0; i < sizeof(bad_modes) / sizeof(bad_modes[0]); ++i) {
        require(am_set_var_record("candidate", legacy) == 0, "install invalid-mode predecessor");
        require(concept_execute(bad_modes[i]) != 0, "invalid concept mode was accepted");
        require(same_record(legacy, am_get_var_record("candidate")), "rejected concept activation changed its predecessor");
    }
    require(!concept_execute(
        "disabled = haiku_contact_enable_learning(haiku_contact_new(\"haiku\", 51))\n"
        "disabled = haiku_contact_enable_symbols(disabled, 0, 0)\n"
        "disabled = haiku_contact_enable_exchange(disabled, 0, 0, 0)"), am_get_error());
    AM_Record *disabled = am_record_clone(am_get_var_record("disabled"));
    require(disabled != NULL, "symbols-off predecessor");
    require(concept_execute("haiku_contact_enable_concepts(disabled, 1)") != 0, "equal concepts accepted disabled history");
    require(same_record(disabled, am_get_var_record("disabled")), "equal concepts changed disabled predecessor");
    require(concept_execute("haiku_contact_enable_concepts(disabled, 2)") != 0, "personal concepts accepted disabled history");
    require(same_record(disabled, am_get_var_record("disabled")), "personal concepts changed disabled predecessor");

    /* A late event on an already closed turn cannot mint another snapshot. */
    require(am_set_var_record("candidate", original) == 0, "install closed-turn predecessor");
    require(!concept_execute(
        "late = haiku_contact_event(\"human\", \"water\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 10, 9)\n"
        "haiku_contact_publish(candidate, late)"), am_get_error());
    AM_Record *late = am_record_clone(am_get_var_record("candidate"));
    require(late != NULL, "late accepted encounter snapshot");
    require(concept_execute("haiku_contact_concepts_finish(candidate)") != 0, "same-turn second concept finish was accepted");
    require(strstr(am_get_error(), "Contact concept witness order differs") != NULL, "same-turn finish diagnostic differs");
    require(same_record(late, am_get_var_record("candidate")), "rejected late finish changed its pending input");
    require(same_record(original, am_get_var_record("lived")), "late finish changed another owner");

    same_file(path, bytes, bytes_size);
    free(bytes); am_record_free(original); am_record_free(legacy);
    am_record_free(disabled); am_record_free(late);
    am_persistent_clear();
    printf("PASS: %zu corrupt concept snapshots and five invalid boundary calls preserve all inputs\n", corrupt_count);
    return 0;
}
