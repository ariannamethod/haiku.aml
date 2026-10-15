/* A warm, actually completed model turn supplies the adversarial publisher. */
#include "state_test.h"

static AM_Record *owner_before, *result_before, *experience_before, *request_before;
static char saved_path[4096];
static unsigned char *saved_bytes;
static size_t saved_size;
static int rejected;

static int concepts_execute(const char *fragment) {
    size_t length = strlen(root) + strlen(fragment) + 128;
    char *source = malloc(length), origin[4096];
    require(source != NULL, "concept source allocation");
    snprintf(source, length, "IMPORT \"%s/src/iceberg_session.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/concepts-publication-probe.aml", work);
    int rc = am_exec_source(source, origin);
    free(source);
    return rc;
}

static void restore_inputs(void) {
    require(!am_set_var_record("live", owner_before), "restore concept owner");
    require(!am_set_var_record("result", result_before), "restore concept completion");
    require(!am_set_var_record("experience", experience_before), "restore frozen experience");
    require(!am_set_var_record("request", request_before), "restore provider request");
}

static void unchanged(const AM_Record *expected, const char *detail) {
    require(same_record(am_get_var_record("live"), expected), detail);
    same_file(saved_path, saved_bytes, saved_size);
}

static void reject_publish(const char *change, const char *diagnostic) {
    restore_inputs();
    if (change && *change) require(!concepts_execute(change), am_get_error());
    AM_Record *snapshot = am_record_clone(am_get_var_record("result"));
    require(snapshot != NULL, "retain rejected concept completion");
    char source[10000];
    snprintf(source, sizeof(source),
        "IMPORT \"%s/src/iceberg_session.aml\"\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, a, b)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)\n", root, saved_path);
    expect_error(source, diagnostic);
    unchanged(owner_before, "rejected completion changed a concept owner leaf");
    require(same_record(am_get_var_record("result"), snapshot), "validation mutated its rejected completion");
    am_record_free(snapshot);
    ++rejected;
}

#define CHECK_LIFE \
    "haiku_iceberg_check(haiku_iceberg_part(result, \"life\"), 0)\n"
#define CONTACT_REJECT "concept contact completion differs from its witnessed transition"
#define ADMISSION_REJECT "concept admission differs from its witnessed ranking"

int main(int argc, char **argv) {
    require(argc == 3, "root and retained work directory required");
    root = argv[1]; work = argv[2];
    am_init(); am_use_notorch(); am_persistent_mode(1);
    char setup[12000];
    snprintf(setup, sizeof(setup),
        "a = model_try_load(\"%s/weights/haiku1.gguf\")\n"
        "b = model_try_load(\"%s/weights/haiku2.gguf\")\n"
        "live = checkpoint_load(\"%s/iceberg-concepts-publication-base.state\")\n"
        "result = checkpoint_load(\"%s/iceberg-concepts-publication-result.state\")\n"
        "experience = haiku_iceberg_part(result, \"experience\")\n"
        "request = haiku_iceberg_part(result, \"request\")\n"
        "assert(haiku_iceberg_concepts_audit(live, a, b) == 84, \"Publication fixture needs both audited private banks\")\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, a, b)\n",
        root, root, work, work);
    require(!concepts_execute(setup), am_get_error());
    owner_before = am_record_clone(am_get_var_record("live"));
    result_before = am_record_clone(am_get_var_record("result"));
    experience_before = am_record_clone(am_get_var_record("experience"));
    request_before = am_record_clone(am_get_var_record("request"));
    AM_Record *checked_before = am_record_clone(am_get_var_record("checked"));
    require(owner_before && result_before && experience_before && request_before && checked_before,
            "retain exact concept transition");
    snprintf(saved_path, sizeof(saved_path), "%s/published-concepts.state", work);
    char error[1024] = {0};
    require(am_checkpoint_save(owner_before, saved_path, error, sizeof(error)) == 1, error);
    saved_bytes = read_file(saved_path, &saved_size);
    AM_List *keys = am_record_keys(owner_before);
    require(keys && keys->len == 265, "unexpected concept owner field count");
    am_list_free(keys);
    keys = am_record_keys(result_before);
    require(keys && keys->len == 555, "unexpected concept completion field count");
    am_list_free(keys);
    unchanged(owner_before, "worker published its concepts before validation");

    /* These alternatives pass closed schema validation. Their actual source
       transition is proved only by replay from the exact private predecessor. */
    reject_publish(
        "clock = record_get(result, \"life:haiku_contact:concept_clock\")\n"
        "assert(clock[1] >= 2, \"fixture needs a lived concept snapshot\")\nclock[1] = clock[1] - 1\n"
        CHECK_LIFE, CONTACT_REJECT);
    reject_publish(
        "clock = record_get(result, \"life:haiku_contact:symbol_clock\")\n"
        "list_set(record_get(result, \"life:haiku_contact:symbol_text\"), clock[9] * 4, \"song song melody guitar piano\")\n"
        "counts = record_get(result, \"life:haiku_contact:concept_counts\")\n"
        "assert(counts[15] > 0, \"fixture needs its actual music teacher\")\ncounts[15] = counts[15] - 1\n"
        CHECK_LIFE, CONTACT_REJECT);
    reject_publish(
        "units = record_get(result, \"life:haiku_contact:concept_units_a\")\ni = 0\n"
        "while i < 448:\n    value = units[i]\n    units[i] = units[448 + i]\n    units[448 + i] = value\n    i = i + 1\n"
        CHECK_LIFE, CONTACT_REJECT);
    reject_publish(
        "units = record_get(result, \"life:haiku_contact:concept_units_b\")\ni = 0\n"
        "while i < 448:\n    value = units[i]\n    units[i] = units[448 + i]\n    units[448 + i] = value\n    i = i + 1\n"
        CHECK_LIFE, CONTACT_REJECT);
    reject_publish(
        "tokens = record_get(result, \"life:haiku_contact:concept_tokens\")\n"
        "tokens[1] = 2047 - (tokens[1] == 2047)\n"
        CHECK_LIFE, CONTACT_REJECT);
    reject_publish(
        "list_set(record_get(result, \"life:haiku_contact:concept_identity\"), 2, \"0000000000000000000000000000000000000000000000000000000000000000\")\n"
        CHECK_LIFE, CONTACT_REJECT);
    reject_publish(
        "clock = record_get(result, \"life:haiku_contact:concept_clock\")\nclock[0] = 1\n"
        CHECK_LIFE, CONTACT_REJECT);
    reject_publish(
        "field = record_get(result, \"life:haiku_contact:p:field\")\nfield[0] = field[0] * 0.5 + 0.01\n"
        CHECK_LIFE, CONTACT_REJECT);
    reject_publish(
        "slot = record_get(result, \"life:haiku_contact:p:receipt_cursor\") - 1\n"
        "if slot < 0:\n    slot = 31\n"
        "features = record_get(result, \"life:haiku_contact:p:receipt_x\")\n"
        "features[slot * 96] = features[slot * 96] * 0.5 + 0.01\n"
        CHECK_LIFE, CONTACT_REJECT);
    reject_publish(
        "slot = record_get(result, \"life:haiku_contact:p:receipt_cursor\") - 1\n"
        "if slot < 0:\n    slot = 31\n"
        "meta = record_get(result, \"life:haiku_contact:contact_meta\")\nmeta[slot * 8 + 5] = 0.2\n"
        CHECK_LIFE, CONTACT_REJECT);
    reject_publish(
        "values = record_get(result, \"life:admission_values\")\n"
        "assert(len(values) >= 16, \"fixture needs actual lexical admissions\")\n"
        "values[len(values) - 1] = values[len(values) - 1] + 0.001\n"
        CHECK_LIFE, ADMISSION_REJECT);
    reject_publish(
        "values = record_get(result, \"life:admission_values\")\n"
        "texts = record_get(result, \"life:admission_text\")\n"
        "start = len(values) - 16\ni = 0\n"
        "while i < 8:\n    value = values[start + i]\n    values[start + i] = values[start + 8 + i]\n    values[start + 8 + i] = value\n"
        "    text = list_get(texts, start + i)\n    list_set(texts, start + i, list_get(texts, start + 8 + i))\n    list_set(texts, start + 8 + i, text)\n    i = i + 1\n"
        "record_set(result, \"life:admission_text\", texts)\n"
        CHECK_LIFE, ADMISSION_REJECT);
    reject_publish(
        "vocab = record_get(result, \"life:voice:vocab\")\nn = list_len(vocab)\n"
        "word = list_get(vocab, n - 1)\nlist_set(vocab, n - 1, list_get(vocab, n - 2))\n"
        "list_set(vocab, n - 2, word)\nrecord_set(result, \"life:voice:vocab\", vocab)\n"
        CHECK_LIFE, ADMISSION_REJECT);
    reject_publish("record_set(result, \"base:haiku_contact:concept_counts\", zeros(21))", "another private life");
    reject_publish("record_set(result, \"life:version\", 5)", "contact profile differs");

    restore_inputs();
    require(!am_set_var_record("checked", checked_before), "restore detached concept result");
    require(!concepts_execute("counts = record_get(result, \"life:haiku_contact:concept_counts\")\ncounts[0] = 128"), am_get_error());
    require(same_record(am_get_var_record("checked"), checked_before), "checked result aliases its worker envelope");
    unchanged(owner_before, "envelope mutation changed the live concept owner");
    restore_inputs();
    char publication[8192];
    snprintf(publication, sizeof(publication),
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, a, b)\n"
        "checkpoint_save(checked, \"%s/no-such-parent/never.state\")\nrecord_swap(live, checked)", work);
    require(concepts_execute(publication) != 0 && strstr(am_get_error(), "temporary create failed"),
            "concept failed-save gate missed its durable boundary");
    unchanged(owner_before, "failed save changed the published concept owner");
    require(same_record(am_get_var_record("checked"), checked_before), "failed save damaged the detached result");
    restore_inputs();
    snprintf(publication, sizeof(publication),
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, a, b)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)", saved_path);
    require(!concepts_execute(publication), am_get_error());
    require(same_record(am_get_var_record("live"), checked_before), "published concepts differ from their checked result");
    free(saved_bytes); saved_bytes = read_file(saved_path, &saved_size);
    require(concepts_execute(publication) != 0 && strstr(am_get_error(), "another private life"),
            "stale concept completion was accepted");
    unchanged(checked_before, "stale completion changed the published owner");
    ++rejected;

    am_persistent_clear();
    am_record_free(owner_before); am_record_free(result_before);
    am_record_free(experience_before); am_record_free(request_before);
    am_record_free(checked_before); free(saved_bytes);
    printf("HAIKU_ICEBERG_CONCEPTS_PUBLICATION_OK (%d refusals, 265 leaves, 555-field envelope)\n", rejected);
    return 0;
}
