/* Dream-born symbol successors are checked against actual source encounters. */
#include "state_test.h"

static AM_Record *owner_before, *result_before, *experience_before, *request_before;
static char saved_path[4096];
static unsigned char *saved_bytes;
static size_t saved_size;
static int rejected;

static int symbol_execute(const char *fragment) {
    size_t length = strlen(root) + strlen(fragment) + 128;
    char *source = malloc(length), origin[4096];
    require(source != NULL, "symbol source allocation");
    snprintf(source, length, "IMPORT \"%s/src/iceberg_session.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/symbols-publication-probe.aml", work);
    int rc = am_exec_source(source, origin);
    free(source);
    return rc;
}

static void restore_inputs(void) {
    require(!am_set_var_record("live", owner_before), "restore symbol owner");
    require(!am_set_var_record("result", result_before), "restore symbol completion");
    require(!am_set_var_record("experience", experience_before), "restore frozen experience");
    require(!am_set_var_record("request", request_before), "restore provider request");
}

static void unchanged(const AM_Record *expected, const char *detail) {
    require(same_record(am_get_var_record("live"), expected), detail);
    same_file(saved_path, saved_bytes, saved_size);
}

static AM_Record *retain_fixture(void) {
    am_record_free(owner_before); am_record_free(result_before);
    am_record_free(experience_before); am_record_free(request_before);
    owner_before = am_record_clone(am_get_var_record("live"));
    result_before = am_record_clone(am_get_var_record("result"));
    experience_before = am_record_clone(am_get_var_record("experience"));
    request_before = am_record_clone(am_get_var_record("request"));
    AM_Record *checked = am_record_clone(am_get_var_record("checked"));
    require(owner_before && result_before && experience_before && request_before && checked,
            "retain exact symbol transition");
    char error[1024] = {0};
    require(am_checkpoint_save(owner_before, saved_path, error, sizeof(error)) == 1, error);
    free(saved_bytes); saved_bytes = read_file(saved_path, &saved_size);
    return checked;
}

static void reject_publish(const char *change, const char *diagnostic) {
    restore_inputs();
    if (change && *change) require(!symbol_execute(change), am_get_error());
    AM_Record *snapshot = am_record_clone(am_get_var_record("result"));
    require(snapshot != NULL, "retain rejected symbol completion");
    char source[10000];
    snprintf(source, sizeof(source),
        "IMPORT \"%s/src/iceberg_session.aml\"\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)\n", root, saved_path);
    expect_error(source, diagnostic);
    unchanged(owner_before, "rejected completion changed a symbol owner leaf");
    require(same_record(am_get_var_record("result"), snapshot), "validation mutated the rejected completion");
    am_record_free(snapshot);
    ++rejected;
}

#define SYMBOL_FIELDS "symbol_clock symbol_nodes symbol_rows symbol_text"
#define PUT_SYMBOLS(owner) \
    "names = haiku_words(\"" SYMBOL_FIELDS "\")\ni = 0\n" \
    "while i < list_len(names):\n" \
    "    name = list_get(names, i)\n" \
    "    record_set(result, text_concat(\"life:" owner "_contact:\", name), record_get(forged, name))\n" \
    "    i = i + 1\n" \
    "haiku_contact_check(haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"" owner "_contact\"))\n"

int main(int argc, char **argv) {
    require(argc == 3, "root and fresh work directory required");
    root = argv[1]; work = argv[2];
    am_init(); am_use_notorch(); am_persistent_mode(1);
    char setup[12000];
    snprintf(setup, sizeof(setup),
        "missing = model_try_load(\"\")\n"
        "live = checkpoint_load(\"%s/iceberg-symbols-taught.state\")\n"
        "haiku_iceberg_check(live, 0)\n"
        "assert(record_get(live, \"voice:turn\") == 14, \"publisher needs fourteen real external turns\")\n"
        "before = haiku_iceberg_part(live, \"voice\")\ncandidate = record_clone(before)\n"
        "input = \"fire\"\n"
        "experience = haiku_session_prepare(candidate, input, 0)\n"
        "request = haiku_iceberg_request_at(live, input, missing, missing, 20754)\n"
        "pending = record_clone(live)\nhaiku_iceberg_put(pending, \"voice\", candidate)\n"
        "SPAWN complete:\n"
        "    return haiku_iceberg_tail(pending, before, experience, request, 0, missing, missing)\n"
        "AWAIT complete INTO result\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "clock = record_get(checked, \"haiku_contact:symbol_clock\")\n"
        "assert(clock[8] == 1 and clock[4] == 1, \"publisher fixture did not reach its actual birth\")\n", work);
    require(!symbol_execute(setup), am_get_error());
    snprintf(saved_path, sizeof(saved_path), "%s/published-symbols.state", work);
    AM_Record *checked_before = retain_fixture();
    AM_List *keys = am_record_keys(owner_before);
    require(keys && keys->len == 245, "unexpected symbol owner field count");
    am_list_free(keys);
    keys = am_record_keys(result_before);
    require(keys && keys->len == 515, "unexpected symbol completion field count");
    am_list_free(keys);
    unchanged(owner_before, "worker published a symbol before validation");

    /* An older, internally valid private symbol history is not this successor. */
    reject_publish("forged = haiku_iceberg_part(live, \"haiku_contact\")\n" PUT_SYMBOLS("haiku"),
                   "symbol completion differs from its witnessed transition");
    /* Correct ordinal and parent pair cannot excuse a different source text. */
    reject_publish(
        "forged = haiku_iceberg_part(live, \"haiku_contact\")\n"
        "event = haiku_contact_event(\"human\", \"tree water\", \"en\", record_get(experience, \"observation\"), record_get(candidate, \"ring_coherence\"), record_get(experience, \"quality\"), 0, 15, 15)\n"
        "haiku_contact_symbols_step(forged, event)\n"
        "event = haiku_contact_event(\"haiku\", \"internal\", \"en\", record_get(experience, \"observation\"), record_get(candidate, \"ring_coherence\"), 0.5, 1, 15, 15)\n"
        "haiku_contact_symbols_step(forged, event)\n"
        "event = haiku_contact_event(\"haiku\", \"internal\", \"en\", record_get(experience, \"observation\"), record_get(candidate, \"ring_coherence\"), 0.5, 5, 15, 15)\n"
        "haiku_contact_symbols_step(forged, event)\n"
        "event = haiku_contact_event(\"haiku\", \"internal\", \"en\", record_get(experience, \"observation\"), record_get(candidate, \"ring_coherence\"), 0.5, 7, 1, 15)\n"
        "haiku_contact_symbols_step(forged, event)\n"
        "haiku_contact_symbols_dream(forged, 1, 15)\n" PUT_SYMBOLS("haiku"),
        "symbol completion differs from its witnessed transition");
    /* Friend receives the precise native Haiku poem, even when it never speaks. */
    reject_publish(
        "forged = haiku_iceberg_part(live, \"friend_contact\")\n"
        "native = haiku_contact_event(\"haiku\", \"fire water\", \"en\", record_get(experience, \"observation\"), record_get(candidate, \"ring_coherence\"), 0.5, 7, 1, 15)\n"
        "delivered = haiku_contact_derive(native, \"friend\", \"fire water\", 6, 1, 15)\n"
        "haiku_contact_symbols_step(forged, delivered)\n"
        "haiku_contact_symbols_dream(forged, 1, 15)\n" PUT_SYMBOLS("friend"),
        "symbol completion differs from its witnessed transition");
    reject_publish("forged = haiku_iceberg_part(live, \"friend_contact\")\n" PUT_SYMBOLS("friend"),
                   "symbol completion differs from its witnessed transition");
    /* The enclosing life only creates native H deliveries for Friend. Generic
       standalone peer clocks are broader, but this ring cannot lose a row. */
    reject_publish(
        "forged = haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"friend_contact\")\n"
        "clock = record_get(forged, \"symbol_clock\")\nclock[6] = 0\nclock[7] = 0\nclock[9] = -1\n"
        "record_set(forged, \"symbol_rows\", zeros(48928))\n"
        "texts = list_new()\ni = 0\nwhile i < 1408:\n    list_push(texts, \"\")\n    i = i + 1\n"
        "record_set(forged, \"symbol_text\", texts)\n" PUT_SYMBOLS("friend"),
        "symbols lost accepted external encounters");
    /* The birth may never replace the native feature vector accepted earlier. */
    reject_publish(
        "contact = haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"haiku_contact\")\n"
        "event = haiku_contact_event(\"human\", input, \"en\", record_get(experience, \"observation\"), record_get(candidate, \"ring_coherence\"), record_get(experience, \"quality\"), 0, 16, 16)\n"
        "future = haiku_contact_features(contact, event)\n"
        "rows = record_get(result, \"life:haiku_contact:p:receipt_rows\")\ni = 0\n"
        "while i < 32:\n"
        "    if rows[i * 9] == 15 and rows[i * 9 + 2] == 0:\n"
        "        features = record_get(result, \"life:haiku_contact:p:receipt_x\")\n"
        "        assert(features[i * 96] != future[0], \"future symbol feature fixture did not differ\")\n"
        "        features[i * 96] = future[0]\n"
        "    i = i + 1\n"
        "haiku_contact_check(haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"haiku_contact\"))", "receipt features differ");
    /* A standalone-valid birth cannot claim a global dream that never occurred. */
    reject_publish(
        "forged = haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"haiku_contact\")\n"
        "clock = record_get(forged, \"symbol_clock\")\nclock[4] = 2\n"
        "nodes = record_get(forged, \"symbol_nodes\")\nnodes[5] = 2\n"
        PUT_SYMBOLS("haiku"), "claim an absent dream boundary");
    /* A real ordinal names its precise recorded turn, not an arbitrary earlier
       turn with enough external support for a standalone-valid birth. */
    reject_publish(
        "forged = haiku_iceberg_part(live, \"haiku_contact\")\n"
        "haiku_contact_symbols_dream(forged, 1, 14)\n" PUT_SYMBOLS("haiku"),
        "name another processed dream turn");
    reject_publish("record_set(result, \"life:version\", 3)", "contact profile differs");
    reject_publish("record_set(result, \"base:haiku_contact:symbol_clock\", zeros(43))", "another private life");
    reject_publish("record_set(result, \"extra\", 1)", "extra fields");

    restore_inputs();
    require(!am_set_var_record("checked", checked_before), "restore detached symbol result");
    require(!symbol_execute("list_set(record_get(result, \"life:haiku_contact:symbol_text\"), 0, \"changed\")"), am_get_error());
    require(same_record(am_get_var_record("checked"), checked_before), "symbol result aliases its worker envelope");
    unchanged(owner_before, "envelope mutation changed a published symbol owner");

    restore_inputs();
    char publication[8192];
    snprintf(publication, sizeof(publication),
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "checkpoint_save(checked, \"%s/no-such-parent/never.state\")\nrecord_swap(live, checked)", work);
    require(symbol_execute(publication) != 0 && strstr(am_get_error(), "temporary create failed"),
            "symbol failed-save gate missed its intended durable boundary");
    unchanged(owner_before, "failed save changed the published symbol owner");
    require(same_record(am_get_var_record("checked"), checked_before), "failed save damaged detached symbol result");
    restore_inputs();
    snprintf(publication, sizeof(publication),
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)", saved_path);
    require(!symbol_execute(publication), am_get_error());
    require(same_record(am_get_var_record("live"), checked_before), "published symbol life differs from checked result");
    free(saved_bytes); saved_bytes = read_file(saved_path, &saved_size);
    require(symbol_execute(publication) != 0 && strstr(am_get_error(), "another private life"),
            "stale symbol completion was accepted");
    unchanged(checked_before, "stale completion changed the published symbol owner");
    ++rejected;

    /* Fourteen external witnesses remain insufficient on an ordinary waking
       turn. A schema-valid kernel result still needs an actual dream receipt. */
    snprintf(setup, sizeof(setup),
        "live = checkpoint_load(\"%s/iceberg-symbols-taught.state\")\n"
        "record_set(live, \"voice:dream_config\", [10000,0,1])\n"
        "before = haiku_iceberg_part(live, \"voice\")\ncandidate = record_clone(before)\n"
        "experience = haiku_session_prepare(candidate, \"fire water\", 0)\n"
        "request = haiku_iceberg_request_at(live, \"fire water\", missing, missing, 20754)\n"
        "pending = record_clone(live)\nhaiku_iceberg_put(pending, \"voice\", candidate)\n"
        "result = haiku_iceberg_tail(pending, before, experience, request, 0, missing, missing)\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "assert(haiku_dream_check(haiku_iceberg_part(checked, \"voice\")) == 0, \"waking fixture invented a dream\")\n", work);
    require(!symbol_execute(setup), am_get_error());
    am_record_free(checked_before); checked_before = retain_fixture();
    reject_publish(
        "forged = haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"haiku_contact\")\n"
        "haiku_contact_symbols_dream(forged, 1, 15)\n" PUT_SYMBOLS("haiku"),
        "claim an absent dream boundary");
    restore_inputs();
    require(symbol_execute(
        "legacy = haiku_iceberg_enable_janus(haiku_iceberg_new(before, [1,0,0,0]), 20740, 1, 0)\n"
        "invalid = haiku_iceberg_enable_symbols(legacy, 1, 0)") != 0 &&
        strstr(am_get_error(), "require an owned word-learning life"), "symbol activation skipped learned-word ownership");
    unchanged(owner_before, "rejected legacy activation changed the published owner");
    ++rejected;

    am_persistent_clear();
    am_record_free(owner_before); am_record_free(result_before);
    am_record_free(experience_before); am_record_free(request_before);
    am_record_free(checked_before); free(saved_bytes);
    printf("PASS: %d symbol source/birth/receipt/profile/stale refusals preserve all 245 leaves and checkpoint bytes\n", rejected);
    puts("PASS: 515-field detached completion, failed save and one-time symbol save/swap");
    puts("PASS: independently replayed human and one-line dream evidence, including schema-valid forged symbols");
    return 0;
}
