/* Actual dream deliveries bind adopted structures to their frozen sender. */
#include "state_test.h"

static AM_Record *owner_before, *result_before, *experience_before, *request_before;
static char saved_path[4096];
static unsigned char *saved_bytes;
static size_t saved_size;
static int rejected;

static int exchange_execute(const char *fragment) {
    size_t length = strlen(root) + strlen(fragment) + 128;
    char *source = malloc(length), origin[4096];
    require(source != NULL, "exchange source allocation");
    snprintf(source, length, "IMPORT \"%s/src/iceberg_session.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/exchange-publication-probe.aml", work);
    int rc = am_exec_source(source, origin);
    free(source);
    return rc;
}

static void restore_inputs(void) {
    require(!am_set_var_record("live", owner_before), "restore exchange owner");
    require(!am_set_var_record("result", result_before), "restore exchange completion");
    require(!am_set_var_record("experience", experience_before), "restore frozen experience");
    require(!am_set_var_record("request", request_before), "restore provider request");
}

static void unchanged(const AM_Record *expected, const char *detail) {
    require(same_record(am_get_var_record("live"), expected), detail);
    same_file(saved_path, saved_bytes, saved_size);
}

static void reject_publish(const char *change, const char *diagnostic) {
    restore_inputs();
    if (change && *change) require(!exchange_execute(change), am_get_error());
    AM_Record *snapshot = am_record_clone(am_get_var_record("result"));
    require(snapshot != NULL, "retain rejected exchange completion");
    char source[10000];
    snprintf(source, sizeof(source),
        "IMPORT \"%s/src/iceberg_session.aml\"\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)\n", root, saved_path);
    expect_error(source, diagnostic);
    unchanged(owner_before, "rejected completion changed an exchange owner leaf");
    require(same_record(am_get_var_record("result"), snapshot), "validation mutated the rejected completion");
    am_record_free(snapshot);
    ++rejected;
}

#define CHECK_CONTACT \
    "haiku_contact_check(haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"haiku_contact\"))\n"
#define CHECK_LIFE \
    "haiku_iceberg_check(haiku_iceberg_part(result, \"life\"), 0)\n"

int main(int argc, char **argv) {
    require(argc == 3, "root and prepared work directory required");
    root = argv[1]; work = argv[2];
    am_init(); am_use_notorch(); am_persistent_mode(1);
    char setup[10000];
    snprintf(setup, sizeof(setup),
        "missing = model_try_load(\"\")\n"
        "live = haiku_iceberg_enable_exchange(checkpoint_load(\"%s/iceberg-exchange-prepared.state\"), 1, 0)\n"
        "before = haiku_iceberg_part(live, \"voice\")\ncandidate = record_clone(before)\n"
        "experience = haiku_session_prepare(candidate, \"moon\", 0)\n"
        "request = haiku_iceberg_request_at(live, \"moon\", missing, missing, 20754)\n"
        "pending = record_clone(live)\nhaiku_iceberg_put(pending, \"voice\", candidate)\n"
        "SPAWN complete:\n"
        "    return haiku_iceberg_tail(pending, before, experience, request, 0, missing, missing)\n"
        "AWAIT complete INTO result\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "owners = haiku_words(\"haiku friend\")\ni = 0\n"
        "while i < 2:\n"
        "    clock = record_get(checked, text_concat(list_get(owners, i), \"_contact:exchange_clock\"))\n"
        "    assert(clock[7] == 1, \"publisher needs actual two-way adoption\")\n"
        "    i = i + 1\n", work);
    require(!exchange_execute(setup), am_get_error());
    owner_before = am_record_clone(am_get_var_record("live"));
    result_before = am_record_clone(am_get_var_record("result"));
    experience_before = am_record_clone(am_get_var_record("experience"));
    request_before = am_record_clone(am_get_var_record("request"));
    AM_Record *checked_before = am_record_clone(am_get_var_record("checked"));
    require(owner_before && result_before && experience_before && request_before && checked_before,
            "retain exact exchange transition");
    snprintf(saved_path, sizeof(saved_path), "%s/published-exchange.state", work);
    char error[1024] = {0};
    require(am_checkpoint_save(owner_before, saved_path, error, sizeof(error)) == 1, error);
    saved_bytes = read_file(saved_path, &saved_size);
    AM_List *keys = am_record_keys(owner_before);
    require(keys && keys->len == 253, "unexpected exchange owner field count");
    am_list_free(keys);
    keys = am_record_keys(result_before);
    require(keys && keys->len == 531, "unexpected exchange completion field count");
    am_list_free(keys);
    unchanged(owner_before, "worker published an adoption before validation");

    /* This alternative history passes both validators: source event IDs have
       gaps, so only exact predecessor replay disproves an extra earlier
       delivery. The actual latest adoption retains the matching revision. */
    reject_publish(
        "clock = record_get(result, \"life:haiku_contact:exchange_clock\")\n"
        "assert(clock[6] == 1, \"fixture needs one actual delivery\")\nclock[6] = 2\n"
        "rows = record_get(result, \"life:haiku_contact:exchange_rows\")\nrows[12] = 2\n"
        CHECK_LIFE, "exchange completion differs from its witnessed deliveries");

    /* Consuming the real delivery while withholding its available structure is
       also schema-valid; publication must reproduce the deterministic offer. */
    reject_publish(
        "clock = record_get(result, \"life:haiku_contact:symbol_clock\")\nclock[8] = 1\n"
        "nodes = record_get(result, \"life:haiku_contact:symbol_nodes\")\ni = 10\n"
        "while i < 20:\n    nodes[i] = 0\n    i = i + 1\n"
        "clock = record_get(result, \"life:haiku_contact:exchange_clock\")\nclock[7] = 0\n"
        "record_set(result, \"life:haiku_contact:exchange_origin\", zeros(48))\n"
        "record_set(result, \"life:haiku_contact:exchange_rows\", zeros(2208))\n"
        "texts = list_new()\ni = 0\nwhile i < 64:\n    list_push(texts, \"\")\n    i = i + 1\n"
        "record_set(result, \"life:haiku_contact:exchange_text\", texts)\n"
        CHECK_LIFE, "symbol completion differs from its witnessed transition");

    /* Reviewed sender aliases are private. A self-consistent packet cannot
       invent a canonical tree absent from the actual sender catalog. */
    reject_publish(
        "nodes = record_get(result, \"life:haiku_contact:symbol_nodes\")\nnodes[10] = 1\nnodes[11] = 4\n"
        "rows = record_get(result, \"life:haiku_contact:exchange_rows\")\n"
        "rows[14] = 1\nrows[15] = 4\nrows[74] = 1\nrows[75] = 4\n"
        "list_set(record_get(result, \"life:haiku_contact:exchange_text\"), 3, \"(fire+tree)\")\n"
        CHECK_CONTACT, "exchange dependency is absent from its sender");
    reject_publish(
        "rows = record_get(result, \"life:haiku_contact:exchange_rows\")\nrows[16] = 1\nrows[17] = 1\n"
        CHECK_CONTACT, "exchange dependency changed its sender origin");
    reject_publish(
        "texts = record_get(result, \"life:haiku_contact:exchange_text\")\n"
        "list_set(texts, 0, text_concat(list_get(texts, 0), \"!\"))\n"
        "record_set(result, \"life:haiku_contact:exchange_text\", texts)\n"
        CHECK_CONTACT, "exchange receipt changed its actual spoken text");
    reject_publish(
        "rows = record_get(result, \"life:haiku_contact:exchange_rows\")\nrows[0] = 29\n",
        "exchange latest delivery differs");
    reject_publish(
        "rows = record_get(result, \"life:haiku_contact:exchange_rows\")\nrows[4] = 13\nrows[5] = 13\n"
        CHECK_CONTACT, "exchange root changed its sender acquisition");
    reject_publish(
        "origin = record_get(result, \"life:haiku_contact:exchange_origin\")\norigin[4] = 1\n",
        "Contact");
    reject_publish(
        "nodes = record_get(result, \"life:haiku_contact:symbol_nodes\")\nnodes[19] = 14\n",
        "Contact");
    reject_publish("record_set(result, \"life:version\", 4)", "contact profile differs");
    reject_publish("record_set(result, \"base:haiku_contact:exchange_clock\", zeros(9))", "another private life");
    reject_publish("record_set(result, \"extra\", 1)", "extra fields");

    restore_inputs();
    require(!am_set_var_record("checked", checked_before), "restore detached exchange result");
    require(!exchange_execute(
        "list_set(record_get(result, \"life:haiku_contact:exchange_text\"), 0, \"changed\")"), am_get_error());
    require(same_record(am_get_var_record("checked"), checked_before), "checked adoption aliases its worker envelope");
    unchanged(owner_before, "envelope mutation changed the published exchange owner");

    restore_inputs();
    char publication[8192];
    snprintf(publication, sizeof(publication),
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "checkpoint_save(checked, \"%s/no-such-parent/never.state\")\nrecord_swap(live, checked)", work);
    require(exchange_execute(publication) != 0 && strstr(am_get_error(), "temporary create failed"),
            "exchange failed-save gate missed its intended durable boundary");
    unchanged(owner_before, "failed save changed the published exchange owner");
    require(same_record(am_get_var_record("checked"), checked_before), "failed save damaged the detached exchange result");
    restore_inputs();
    snprintf(publication, sizeof(publication),
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)", saved_path);
    require(!exchange_execute(publication), am_get_error());
    require(same_record(am_get_var_record("live"), checked_before), "published exchange differs from checked result");
    free(saved_bytes); saved_bytes = read_file(saved_path, &saved_size);
    require(exchange_execute(publication) != 0 && strstr(am_get_error(), "another private life"),
            "stale exchange completion was accepted");
    unchanged(checked_before, "stale completion changed the published exchange owner");
    ++rejected;

    am_persistent_clear();
    am_record_free(owner_before); am_record_free(result_before);
    am_record_free(experience_before); am_record_free(request_before);
    am_record_free(checked_before); free(saved_bytes);
    printf("PASS: %d exchange source/tree/origin/profile/stale refusals preserve all 253 leaves and checkpoint bytes\n", rejected);
    puts("PASS: 531-field detached completion, failed save and one-time exchange save/swap");
    puts("PASS: actual two-way adoption; schema-valid alternate histories fail exact predecessor replay");
    return 0;
}
