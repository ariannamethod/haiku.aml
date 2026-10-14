/* Inspect every private leaf across restart, reward and rejected ancestry. */
#include "state_test.h"

static int contact_execute(const char *fragment) {
    size_t size = strlen(root) + strlen(fragment) + 120;
    char *program = malloc(size), origin[4096];
    require(program != NULL, "contact program allocation");
    snprintf(program, size, "IMPORT \"%s/src/parliament_contact.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/contact-validation.aml", work);
    int rc = am_exec_source(program, origin);
    free(program); return rc;
}

static void rejected(const char *fragment, const char *error) {
    AM_Record *before = am_record_clone(am_get_var_record("live"));
    require(before != NULL, "contact rejection snapshot");
    int rc = contact_execute(fragment);
    if (!rc || !strstr(am_get_error(), error)) {
        fprintf(stderr, "contact expected: %s\nactual: %s\n", error, am_get_error());
        require(0, "wrong contact rejection");
    }
    require(same_record(before, am_get_var_record("live")), "rejected contact call changed a leaf");
    am_record_free(before);
}

int main(int argc, char **argv) {
    require(argc == 3, "contact root and work directory");
    root = argv[1]; work = argv[2];
    (void)execute; (void)expect_error;
    am_init(); am_use_notorch(); am_persistent_mode(1);
    char fragment[12000], path[4096], error[1024];
    snprintf(fragment, sizeof(fragment), "IMPORT \"%s/tests/parliament_contact_models.aml\"", root);
    require(!contact_execute(fragment), am_get_error());
    /* Live includes actual mixed PEN7 prototypes, receipt features and pressure. */
    snprintf(path, sizeof(path), "%s/contact.state", work);
    require(am_checkpoint_save(am_get_var_record("live"), path, error, sizeof(error)) == 1, error);
    size_t size = 0;
    unsigned char *bytes = read_file(path, &size);
    snprintf(fragment, sizeof(fragment), "resumed = haiku_contact_load(\"%s\")", path);
    require(!contact_execute(fragment), am_get_error());
    require(same_record(am_get_var_record("live"), am_get_var_record("resumed")), "contact restart changed model cache or parliament");
    require(!contact_execute(
        "later = haiku_contact_event(\"human\", \"rain tree music\", \"en\", [0.4,0.5,2,0.6], [0.5,0.4,0.3], 0.8, 0, 2, 2)\n"
        "haiku_contact_publish(live, later)\nhaiku_contact_publish(resumed, later)\n"
        "inside = haiku_contact_derive(later, \"haiku\", \"music tree\", 5, 1, 2)\n"
        "haiku_contact_publish(live, inside)\nhaiku_contact_publish(resumed, inside)"), am_get_error());
    require(same_record(am_get_var_record("live"), am_get_var_record("resumed")), "contact restarted reward/Oja continuation differs");
    same_file(path, bytes, size); free(bytes);
    AM_Record *before = am_record_clone(am_get_var_record("live"));
    require(before != NULL, "contact replay snapshot");
    require(!contact_execute("haiku_contact_publish(live, event)\nrecord_set(inside, \"replay\", 1)\nhaiku_contact_publish(live, inside)"), am_get_error());
    require(same_record(before, am_get_var_record("live")), "replay changed contact cache, RNG or witness history");
    am_record_free(before);

    rejected("haiku_contact_derive(inside, \"human\", \"rain\", 0, 3, 3)", "derivative cannot become a human witness");
    rejected("fake = record_clone(inside)\nrecord_set(fake, \"source\", 0)\nrecord_set(fake, \"owner\", \"human\")\nhaiku_contact_publish(live, fake)", "derivative cannot become a human witness");
    rejected("fake = record_clone(event)\nrecord_set(fake, \"representation_gain\", 0.251)\nhaiku_contact_publish(live, fake)", "representation gain is invalid");
    rejected("fake = record_clone(event)\nrecord_set(fake, \"identity\", \"\")\nhaiku_contact_publish(live, fake)", "model identity is absent");
    rejected("fake = record_clone(event)\nrecord_set(fake, \"availability\", 3)\nhaiku_contact_publish(live, fake)", "mixed source needs both models");
    rejected("haiku_contact_bind(live, \"en\", \"invented\", 0, 0, 2)", "word is absent from witness");
    rejected("wrong_receipt = record_clone(receipt)\nrecord_set(wrong_receipt, \"parent_event\", 99)\nhaiku_contact_enrich(live, event, wrong_receipt, a, b)", "lexical parent differs");

    const char *corruptions[] = {
        "record_set(broken, \"extra\", 1)",
        "list_set(record_get(broken, \"contact_text\"), 5, \"\")",
        "m = record_get(broken, \"contact_meta\")\nm[2] = 7",
        "m = record_get(broken, \"contact_meta\")\nm[7] = 0",
        "m = record_get(broken, \"contact_meta\")\nm[5] = 0.3",
        "m = record_get(broken, \"contact_meta\")\nm[14] = 0.5",
        "map_set(record_get(broken, \"bindings\"), haiku_contact_key(\"en\",\"rain\"), 4)",
        "map_set(record_get(broken, \"bindings\"), haiku_contact_key(\"en\",\"fake\"), 4)",
        "record_set(broken, \"prototype_values\", zeros(448))",
        "record_set(broken, \"prototype_alpha\", 1.1)"
    };
    AM_Record *original = am_record_clone(am_get_var_record("live"));
    require(original != NULL, "contact original snapshot");
    for (size_t i = 0; i < sizeof(corruptions) / sizeof(corruptions[0]); ++i) {
        require(am_set_var_record("broken", original) == 0, "install contact corrupt candidate");
        require(!contact_execute(corruptions[i]), am_get_error());
        AM_Record *snapshot = am_record_clone(am_get_var_record("broken"));
        require(snapshot != NULL, "corrupt contact snapshot");
        require(contact_execute("haiku_contact_check(broken)") != 0, "corrupt contact accepted");
        require(same_record(snapshot, am_get_var_record("broken")), "contact checker mutated corrupt input");
        require(same_record(original, am_get_var_record("live")), "corrupt contact leaked to live owner");
        am_record_free(snapshot);
    }
    am_record_free(original); am_persistent_clear();
    puts("PASS: contact exact restart with PEN7 prototypes and later real-source witness/Oja continuation");
    puts("PASS: replay, seven rejected calls and ten corrupt private states preserve all owned leaves");
    return 0;
}
