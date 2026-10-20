/* A real third sparse encounter supplies the adversarial promotion gate. */
#include "state_test.h"

static AM_Record *owner_before, *result_before, *experience_before, *request_before;
static char saved_path[4096];
static unsigned char *saved_bytes;
static size_t saved_size;
static int rejected;

static int neural_execute(const char *fragment) {
    size_t length = strlen(root) + strlen(fragment) + 128;
    char *source = malloc(length), origin[4096];
    require(source != NULL, "neural source allocation");
    snprintf(source, length, "IMPORT \"%s/src/iceberg_session.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/neural-publication-probe.aml", work);
    int rc = am_exec_source(source, origin);
    free(source);
    return rc;
}

static void restore_inputs(void) {
    require(!am_set_var_record("live", owner_before), "restore neural owner");
    require(!am_set_var_record("result", result_before), "restore neural completion");
    require(!am_set_var_record("experience", experience_before), "restore neural experience");
    require(!am_set_var_record("request", request_before), "restore neural request");
}

static void unchanged(const AM_Record *expected, const char *detail) {
    require(same_record(am_get_var_record("live"), expected), detail);
    same_file(saved_path, saved_bytes, saved_size);
}

static void reject_publish(const char *change, const char *diagnostic) {
    restore_inputs();
    if (change && *change) require(!neural_execute(change), am_get_error());
    AM_Record *snapshot = am_record_clone(am_get_var_record("result"));
    require(snapshot != NULL, "retain rejected neural completion");
    char source[10000];
    snprintf(source, sizeof(source),
        "IMPORT \"%s/src/iceberg_session.aml\"\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, a, b)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)\n", root, saved_path);
    expect_error(source, diagnostic);
    unchanged(owner_before, "rejected neural completion changed a published owner");
    require(same_record(am_get_var_record("result"), snapshot), "neural validator changed its rejected envelope");
    am_record_free(snapshot);
    ++rejected;
}

#define CHECK_LIFE "haiku_iceberg_check(haiku_iceberg_part(result, \"life\"), 0)\n"
#define CONTACT_REJECT "concept contact completion differs from its witnessed transition"
#define PROOF "rows = record_get(result, \"life:haiku_contact:neural_rows\")\n"

int main(int argc, char **argv) {
    require(argc == 3, "root and retained work directory required");
    root = argv[1]; work = argv[2];
    am_init(); am_use_notorch(); am_persistent_mode(1);
    char setup[12000];
    snprintf(setup, sizeof(setup),
        "a = model_try_load(\"%s/weights/haiku1.gguf\")\n"
        "b = model_try_load(\"%s/weights/haiku2.gguf\")\n"
        "live = checkpoint_load(\"%s/iceberg-neural-publication-base.state\")\n"
        "result = checkpoint_load(\"%s/iceberg-neural-publication-result.state\")\n"
        "experience = haiku_iceberg_part(result, \"experience\")\n"
        "request = haiku_iceberg_part(result, \"request\")\n"
        "assert(haiku_iceberg_neural_audit(live, a, b) > 0, \"Publication fixture needs source-audited neural history\")\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, a, b)\n"
        "assert(text_equal(list_get(record_get(checked, \"haiku_contact:neural_words\"), 1), \"stream\"), \"Publication fixture lost stream\")\n"
        "rows = record_get(checked, \"haiku_contact:neural_rows\")\n"
        "assert(rows[0] == 1 and rows[242] == 2 and rows[484] == 3, \"Publication fixture lost its three actual witnesses\")\n",
        root, root, work, work);
    require(!neural_execute(setup), am_get_error());
    owner_before = am_record_clone(am_get_var_record("live"));
    result_before = am_record_clone(am_get_var_record("result"));
    experience_before = am_record_clone(am_get_var_record("experience"));
    request_before = am_record_clone(am_get_var_record("request"));
    AM_Record *checked_before = am_record_clone(am_get_var_record("checked"));
    require(owner_before && result_before && experience_before && request_before && checked_before,
            "retain exact neural publication inputs");
    AM_List *keys = am_record_keys(owner_before);
    require(keys && keys->len == 276, "unexpected neural life field count");
    am_list_free(keys);
    keys = am_record_keys(result_before);
    require(keys && keys->len == 578, "unexpected neural publication field count");
    am_list_free(keys);
    snprintf(saved_path, sizeof(saved_path), "%s/published-neural.state", work);
    char error[1024] = {0};
    require(am_checkpoint_save(owner_before, saved_path, error, sizeof(error)) == 1, error);
    saved_bytes = read_file(saved_path, &saved_size);
    unchanged(owner_before, "neural worker published before validation");

    /* These changed proofs still pass their closed structural validator. The
       full publisher independently regenerates their actual endpoint scores,
       frozen prior counts and query inputs from the predecessor and sources. */
    reject_publish(PROOF "rows[524] = rows[524] * 0.999\n" CHECK_LIFE, CONTACT_REJECT);
    reject_publish(PROOF "rows[528] = rows[528] * 0.999\n" CHECK_LIFE, CONTACT_REJECT);
    reject_publish(PROOF "rows[503] = rows[503] + 1\n" CHECK_LIFE, CONTACT_REJECT);
    reject_publish(PROOF "rows[532] = 2047 - (rows[532] == 2047)\n" CHECK_LIFE, CONTACT_REJECT);
    reject_publish(PROOF
        "texts = record_get(result, \"life:haiku_contact:neural_text\")\ni = 0\n"
        "while i < 242:\n    rows[484 + i] = 0\n    i = i + 1\n"
        "i = 0\nwhile i < 8:\n    list_set(texts, 16 + i, \"\")\n    i = i + 1\n"
        "record_set(result, \"life:haiku_contact:neural_text\", texts)\n"
        CHECK_LIFE, CONTACT_REJECT);

    reject_publish(
        "list_set(record_get(result, \"life:haiku_contact:neural_text\"), 3, \"haiku\")\n",
        "Neural association human owner differs");
    reject_publish(
        "list_set(record_get(result, \"life:haiku_contact:neural_text\"), 5, \"0000000000000000000000000000000000000000000000000000000000000000\")\n",
        "Neural association A identity differs");
    reject_publish(PROOF "rows[489] = 2\n", "Neural association human ancestry differs");
    reject_publish("record_set(result, \"base:haiku_contact:neural_rows\", zeros(46464))\n", "another private life");
    reject_publish(
        "record_set(experience, \"quality\", record_get(experience, \"quality\") + 0.001)\n"
        "haiku_iceberg_put(result, \"experience\", experience)\n",
        "neural foreground differs from its prior owned pressure");

    /* Checked state is detached from the returned worker envelope. */
    restore_inputs();
    require(!am_set_var_record("checked", checked_before), "restore checked neural result");
    require(!neural_execute(PROOF "rows[40] = 0\n"), am_get_error());
    require(same_record(am_get_var_record("checked"), checked_before), "checked neural state aliases its worker envelope");
    unchanged(owner_before, "worker mutation changed the neural predecessor");

    restore_inputs();
    char publication[8192];
    snprintf(publication, sizeof(publication),
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, a, b)\n"
        "checkpoint_save(checked, \"%s/no-such-parent/never.state\")\nrecord_swap(live, checked)", work);
    require(neural_execute(publication) != 0 && strstr(am_get_error(), "temporary create failed"),
            "neural failed-save gate missed its durable boundary");
    unchanged(owner_before, "failed save changed the neural published owner");
    require(same_record(am_get_var_record("checked"), checked_before), "failed save damaged detached neural state");
    restore_inputs();
    snprintf(publication, sizeof(publication),
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, a, b)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)", saved_path);
    require(!neural_execute(publication), am_get_error());
    require(same_record(am_get_var_record("live"), checked_before), "published neural state differs from its checked result");
    free(saved_bytes); saved_bytes = read_file(saved_path, &saved_size);
    require(neural_execute(publication) != 0 && strstr(am_get_error(), "another private life"),
            "stale neural completion was accepted");
    unchanged(checked_before, "stale neural completion changed the published owner");
    ++rejected;

    am_persistent_clear();
    am_record_free(owner_before); am_record_free(result_before);
    am_record_free(experience_before); am_record_free(request_before);
    am_record_free(checked_before); free(saved_bytes);
    printf("HAIKU_ICEBERG_NEURAL_PUBLICATION_OK (%d refusals, 276 leaves, 578-field envelope)\n", rejected);
    return 0;
}
