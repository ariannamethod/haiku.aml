/* Rejected transitions and malformed symbol proofs preserve owned bytes. */
#include "state_test.h"

static int symbols_execute(const char *fragment) {
    size_t size = strlen(root) + strlen(fragment) + 128;
    char *program = malloc(size), origin[4096];
    require(program != NULL, "symbol program allocation");
    snprintf(program, size, "IMPORT \"%s/src/parliament_contact.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/symbol-validation.aml", work);
    int rc = am_exec_source(program, origin);
    free(program); return rc;
}

static void rejected(const char *fragment) {
    AM_Record *before = am_record_clone(am_get_var_record("live"));
    require(before != NULL, "symbol rejection snapshot");
    require(symbols_execute(fragment) != 0, "invalid symbol transition was accepted");
    require(same_record(before, am_get_var_record("live")), "rejected symbol transition changed an owned leaf");
    am_record_free(before);
}

int main(int argc, char **argv) {
    require(argc == 3, "symbol root and work directory");
    root = argv[1]; work = argv[2];
    (void)execute; (void)expect_error;
    am_init(); am_use_notorch(); am_persistent_mode(1);
    char path[4096], fragment[12000];
    snprintf(path, sizeof(path), "%s/interpreted.full", work);
    size_t size = 0;
    unsigned char *bytes = read_file(path, &size);
    snprintf(fragment, sizeof(fragment), "live = haiku_contact_load(\"%s\")", path);
    require(!symbols_execute(fragment), am_get_error());
    AM_Record *original = am_record_clone(am_get_var_record("live"));
    require(original != NULL, "symbol original snapshot");
    require(!symbols_execute(
        "last = haiku_contact_event(\"human\", \"flame river forest\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 28, 28)\n"
        "haiku_contact_symbols_observe(live, last)\nhaiku_contact_publish(live, last)\n"
        "haiku_contact_symbols_dream(live, 4, 28)\n"
        "echo = record_clone(last)\nrecord_set(echo, \"replay\", 1)\n"
        "record_set(echo, \"event\", 100)\nrecord_set(echo, \"origin_event\", 100)\nrecord_set(echo, \"turn\", 100)\n"
        "haiku_contact_symbols_observe(live, echo)\nhaiku_contact_publish(live, echo)"), am_get_error());
    require(same_record(original, am_get_var_record("live")), "symbol replay changed owned state or RNG");

    rejected("unsealed = haiku_contact_event(\"human\", \"fire water tree\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 29, 29)\nhaiku_contact_symbols_observe(live, unsealed)");
    rejected("old = haiku_contact_event(\"human\", \"fire water\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 29, 1)\nhaiku_contact_publish(live, old)");
    rejected("forged = haiku_contact_event(\"human\", \"fire water\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 29, 29)\nrecord_set(forged, \"parent_source\", 4)\nrecord_set(forged, \"parent_event\", 1)\nrecord_set(forged, \"parent_owner\", \"haiku\")\nhaiku_contact_publish(live, forged)");
    rejected("haiku_contact_symbols_dream(live, 5, 28)");
    rejected("haiku_contact_symbols_dream(live, -1, 29)");
    rejected("legacy = haiku_contact_new(\"haiku\", 80)\nhaiku_contact_enable_symbols(legacy, 1, 0)");

    const char *corruptions[] = {
        "record_set(broken, \"extra\", 1)",
        "record_set(broken, \"version\", 2)",
        "record_set(broken, \"symbol_rows\", zeros(1))",
        "clock = record_get(broken, \"symbol_clock\")\nclock[0] = 0",
        "clock = record_get(broken, \"symbol_clock\")\nclock[1] = 999",
        "clock = record_get(broken, \"symbol_clock\")\nclock[5] = 0",
        "clock = record_get(broken, \"symbol_clock\")\nclock[6] = 128",
        "clock = record_get(broken, \"symbol_clock\")\nclock[7] = 129",
        "clock = record_get(broken, \"symbol_clock\")\nclock[8] = 17",
        "clock = record_get(broken, \"symbol_clock\")\nclock[9] = -1",
        "clock = record_get(broken, \"symbol_clock\")\nclock[26] = clock[26] + 1",
        "clock = record_get(broken, \"symbol_clock\")\nclock[42] = 27",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[10] = 89",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[2] = 0",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[3] = 17",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[4] = 1",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[5] = 999",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[6] = 13",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[7] = 999",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[8] = 13",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[9] = 13",
        /* Original immutable seed spellings must match their frozen base IDs. */
        "rows = record_get(broken, \"symbol_rows\")\nrows[128 * 139 + 11] = 4",
        "rows = record_get(broken, \"symbol_rows\")\nrows[128 * 139 + 75] = 89",
        "rows = record_get(broken, \"symbol_rows\")\nrows[128 * 139] = 4",
        "rows = record_get(broken, \"symbol_rows\")\nrows[128 * 139 + 2] = 0",
        "rows = record_get(broken, \"symbol_rows\")\nrows[128 * 139 + 8] = 2",
        "rows = record_get(broken, \"symbol_rows\")\nrows[128 * 139 + 9] = 65",
        "rows = record_get(broken, \"symbol_rows\")\nrows[128 * 139 + 138] = 1",
        "text = record_get(broken, \"symbol_text\")\nlist_set(text, 128 * 4, \"tree water\")\nrecord_set(broken, \"symbol_text\", text)",
        "text = record_get(broken, \"symbol_text\")\nlist_set(text, 128 * 4 + 2, \"friend\")\nrecord_set(broken, \"symbol_text\", text)",
        /* Two proof rows cannot create independent support from the same turn. */
        "rows = record_get(broken, \"symbol_rows\")\nrows[129 * 139 + 2] = rows[128 * 139 + 2]",
        "rows = record_get(broken, \"symbol_rows\")\nrows[351 * 139 + 1] = 1",
        "rows = record_get(broken, \"symbol_rows\")\nrows[351 * 139 + 1] = 1e-30",
        /* Haiku's native-human count cannot outlive an erased retained ring.
           Keep both born-node proofs intact, so this isolates ring occupancy. */
        "clock = record_get(broken, \"symbol_clock\")\nclock[6] = 0\nclock[7] = 0\nclock[9] = -1\n"
        "rows = record_get(broken, \"symbol_rows\")\ni = 0\nwhile i < 128:\n"
        "    j = 0\n    while j < 139:\n        rows[i * 139 + j] = 0\n        j = j + 1\n    i = i + 1\n"
        "text = record_get(broken, \"symbol_text\")\ni = 0\nwhile i < 512:\n"
        "    list_set(text, i, \"\")\n    i = i + 1\nrecord_set(broken, \"symbol_text\", text)"
    };
    size_t count = sizeof(corruptions) / sizeof(corruptions[0]);
    for (size_t i = 0; i < count; ++i) {
        require(am_set_var_record("broken", original) == 0, "install corrupt symbol candidate");
        require(!symbols_execute(corruptions[i]), am_get_error());
        AM_Record *snapshot = am_record_clone(am_get_var_record("broken"));
        require(snapshot != NULL, "corrupt symbol snapshot");
        if (!symbols_execute("haiku_contact_check(broken)")) {
            fprintf(stderr, "accepted symbol corruption %zu: %s\n", i, corruptions[i]);
            require(0, "corrupt symbol state was accepted");
        }
        require(same_record(snapshot, am_get_var_record("broken")), "symbol checker mutated corrupt input");
        require(same_record(original, am_get_var_record("live")), "symbol corruption leaked into another owner");
        am_record_free(snapshot);
    }
    require(!symbols_execute(
        "fresh = haiku_contact_enable_learning(haiku_contact_new(\"haiku\", 81))\n"
        "fresh = haiku_contact_enable_symbols(fresh, 1, 0)\n"
        "clock = record_get(fresh, \"symbol_clock\")\nclock[18] = 1"), am_get_error());
    AM_Record *fresh = am_record_clone(am_get_var_record("fresh"));
    require(fresh != NULL, "fresh symbol corrupt snapshot");
    require(symbols_execute("haiku_contact_check(fresh)") != 0, "fresh symbol state forged a source clock");
    require(same_record(fresh, am_get_var_record("fresh")), "fresh checker mutated the corrupt input");
    require(same_record(original, am_get_var_record("live")), "fresh corruption leaked into lived owner");
    am_record_free(fresh);

    /* The ordinary ring has expired both early witnesses. Permanent proofs
       must still preserve their source-event and observation-revision order. */
    char expired_path[4096];
    snprintf(expired_path, sizeof(expired_path), "%s/interpreted.expiry", work);
    size_t expired_size = 0;
    unsigned char *expired_bytes = read_file(expired_path, &expired_size);
    snprintf(fragment, sizeof(fragment), "expired = haiku_contact_load(\"%s\")", expired_path);
    require(!symbols_execute(fragment), am_get_error());
    AM_Record *expired = am_record_clone(am_get_var_record("expired"));
    require(expired != NULL, "expired proof snapshot");
    const char *proof_swaps[] = {
        "rows = record_get(broken, \"symbol_rows\")\n"
        "saved = rows[128 * 139 + 1]\nrows[128 * 139 + 1] = rows[129 * 139 + 1]\nrows[129 * 139 + 1] = saved\n"
        "rows[128 * 139 + 4] = rows[128 * 139 + 1]\nrows[129 * 139 + 4] = rows[129 * 139 + 1]",
        "rows = record_get(broken, \"symbol_rows\")\n"
        "saved = rows[128 * 139 + 7]\nrows[128 * 139 + 7] = rows[129 * 139 + 7]\nrows[129 * 139 + 7] = saved"
    };
    for (size_t i = 0; i < sizeof(proof_swaps) / sizeof(proof_swaps[0]); ++i) {
        require(am_set_var_record("broken", expired) == 0, "install corrupt expired proof");
        require(!symbols_execute(proof_swaps[i]), am_get_error());
        AM_Record *snapshot = am_record_clone(am_get_var_record("broken"));
        require(snapshot != NULL, "expired proof rejection snapshot");
        require(symbols_execute("haiku_contact_check(broken)") != 0, "expired proof forgot its original event/revision order");
        require(same_record(snapshot, am_get_var_record("broken")), "expired proof checker mutated corrupt input");
        require(same_record(expired, am_get_var_record("expired")), "corrupt proof changed the saved owner");
        require(same_record(original, am_get_var_record("live")), "corrupt proof changed another owner");
        am_record_free(snapshot);
    }
    am_record_free(expired);
    same_file(expired_path, expired_bytes, expired_size); free(expired_bytes);
    am_record_free(original);
    same_file(path, bytes, size); free(bytes);
    am_persistent_clear();
    puts("PASS: six rejected symbol transitions preserve all owned state");
    printf("PASS: %zu corrupt symbol records are rejected without mutation\n", count + 3);
    puts("PASS: symbol replay is exact and the preceding checkpoint remains unchanged");
    return 0;
}
