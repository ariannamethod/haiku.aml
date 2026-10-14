/* Complete learned successors are checked against actual source encounters. */
#include "state_test.h"

static AM_Record *owner_before, *result_before, *experience_before, *request_before;
static char saved_path[4096];
static unsigned char *saved_bytes;
static size_t saved_size;
static int rejected;

static int learning_execute(const char *fragment) {
    size_t length = strlen(root) + strlen(fragment) + 128;
    char *source = malloc(length), origin[4096];
    require(source != NULL, "learning source allocation");
    snprintf(source, length, "IMPORT \"%s/src/iceberg_session.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/learning-publication-probe.aml", work);
    int rc = am_exec_source(source, origin);
    free(source);
    return rc;
}

static void restore_inputs(void) {
    require(!am_set_var_record("live", owner_before), "restore learned owner");
    require(!am_set_var_record("result", result_before), "restore learned completion");
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
            "retain exact learning transition");
    char error[1024] = {0};
    require(am_checkpoint_save(owner_before, saved_path, error, sizeof(error)) == 1, error);
    free(saved_bytes); saved_bytes = read_file(saved_path, &saved_size);
    return checked;
}

static void reject_publish(const char *change, const char *diagnostic) {
    restore_inputs();
    if (change && *change) require(!learning_execute(change), am_get_error());
    AM_Record *snapshot = am_record_clone(am_get_var_record("result"));
    require(snapshot != NULL, "retain rejected learned completion");
    char source[10000];
    snprintf(source, sizeof(source),
        "IMPORT \"%s/src/iceberg_session.aml\"\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)\n", root, saved_path);
    expect_error(source, diagnostic);
    unchanged(owner_before, "rejected completion changed a learned owner leaf");
    require(same_record(am_get_var_record("result"), snapshot), "validation mutated the rejected completion");
    am_record_free(snapshot);
    ++rejected;
}

#define LEARN_FIELDS "learn_clock learn_words learn_counts learn_rows learn_text"
#define PUT_H_LEARNING \
    "names = haiku_words(\"" LEARN_FIELDS "\")\ni = 0\n" \
    "while i < list_len(names):\n" \
    "    name = list_get(names, i)\n" \
    "    record_set(result, text_concat(\"life:haiku_contact:\", name), record_get(forged, name))\n" \
    "    i = i + 1\n" \
    "haiku_contact_check(haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"haiku_contact\"))\n"

int main(int argc, char **argv) {
    require(argc == 3, "root and fresh work directory required");
    root = argv[1]; work = argv[2];
    am_init(); am_use_notorch(); am_persistent_mode(1);
    require(!learning_execute(
        "missing = model_try_load(\"\")\n"
        "core = haiku_state_enable_dreams(haiku_state_new(\"regex\", 0, 0, 575, 57), 0)\n"
        "record_set(core, \"dream_config\", [10000,0,4])\n"
        "legacy = haiku_iceberg_new(core, [1,0,0,0])\n"
        "janus = haiku_iceberg_enable_janus(legacy, 20740, 1, 0)\n"
        "live = haiku_iceberg_enable_words(janus, 0)\n"
        "haiku_iceberg_turn_at(live, \"stream water river\", 0, missing, missing, 0, 20740)\n"
        "haiku_iceberg_turn_at(live, \"stream sea ocean\", 0, missing, missing, 1, 20741)\n"
        "before = haiku_iceberg_part(live, \"voice\")\ncandidate = record_clone(before)\n"
        "experience = haiku_session_prepare(candidate, \"stream rain lake\", 0)\n"
        "request = haiku_iceberg_request_at(live, \"stream rain lake\", missing, missing, 20742)\n"
        "pending = record_clone(live)\nhaiku_iceberg_put(pending, \"voice\", candidate)\n"
        "SPAWN complete:\n"
        "    return haiku_iceberg_tail(pending, before, experience, request, 0, missing, missing)\n"
        "AWAIT complete INTO result\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "glyphs = haiku_contact_glyphs(haiku_iceberg_part(checked, \"haiku_contact\"), \"stream\", \"en\")\n"
        "assert(glyphs[0] > 0, \"publication fixture did not confirm its third actual water encounter\")\n"), am_get_error());
    snprintf(saved_path, sizeof(saved_path), "%s/published-learning.state", work);
    AM_Record *checked_before = retain_fixture();
    AM_List *keys = am_record_keys(owner_before);
    require(keys && keys->len == 237, "unexpected learned owner field count");
    am_list_free(keys);
    keys = am_record_keys(result_before);
    require(keys && keys->len == 499, "unexpected learned completion field count");
    am_list_free(keys);
    unchanged(owner_before, "worker published before validation");

    /* An internally valid older learner is not this turn's successor. */
    reject_publish("forged = haiku_iceberg_part(live, \"haiku_contact\")\n" PUT_H_LEARNING,
                   "differs from its witnessed transition");
    /* Nor is a valid successor trained on a different current text. */
    reject_publish(
        "forged = haiku_iceberg_part(live, \"haiku_contact\")\n"
        "event = haiku_contact_event(\"human\", \"branch tree forest\", \"en\", record_get(experience, \"observation\"), record_get(candidate, \"ring_coherence\"), record_get(experience, \"quality\"), 0, 3, 3)\n"
        "haiku_contact_learning_step(forged, event)\n" PUT_H_LEARNING,
        "differs from its witnessed transition");
    /* Current receipt text and its actual source clock are independently bound. */
    reject_publish(
        "rows = record_get(result, \"life:haiku_contact:p:receipt_rows\")\ni = 0\n"
        "while i < 32:\n"
        "    if rows[i * 9] == 3 and rows[i * 9 + 2] == 0:\n"
        "        list_set(record_get(result, \"life:haiku_contact:contact_text\"), i * 6 + 1, \"branch tree forest\")\n"
        "    i = i + 1\n"
        "haiku_contact_check(haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"haiku_contact\"))",
        "receipt text differs");
    /* The third teacher may confirm a word only after its own frozen vector. */
    reject_publish(
        "contact = haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"haiku_contact\")\n"
        "event = haiku_contact_event(\"human\", \"stream rain lake\", \"en\", record_get(experience, \"observation\"), record_get(candidate, \"ring_coherence\"), record_get(experience, \"quality\"), 0, 3, 3)\n"
        "future = haiku_contact_features(contact, event)\n"
        "rows = record_get(result, \"life:haiku_contact:p:receipt_rows\")\ni = 0\n"
        "while i < 32:\n"
        "    if rows[i * 9] == 3 and rows[i * 9 + 2] == 0:\n"
        "        features = record_get(result, \"life:haiku_contact:p:receipt_x\")\n"
        "        assert(features[i * 96] != future[0], \"retrofitted feature fixture did not differ\")\n"
        "        features[i * 96] = future[0]\n"
        "    i = i + 1\n"
        "haiku_contact_check(haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"haiku_contact\"))",
        "receipt features differ");
    reject_publish(
        "seen = record_get(result, \"life:haiku_contact:p:seen\")\nseen[0] = 4\n"
        "haiku_contact_check(haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"haiku_contact\"))",
        "source clocks differ");
    reject_publish(
        "record_set(result, \"life:haiku_contact:p:receipt_cursor\", 12)\n"
        "haiku_contact_check(haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"haiku_contact\"))",
        "receipt boundary differs");
    /* Reviewed/manual choices remain explicit foreground actions. */
    reject_publish(
        "completed = haiku_iceberg_part(result, \"life\")\n"
        "contact = haiku_iceberg_part(completed, \"haiku_contact\")\n"
        "haiku_contact_bind(contact, \"en\", \"stream\", 1, 0, 3)\n"
        "haiku_iceberg_put(completed, \"haiku_contact\", contact)\n"
        "haiku_iceberg_put(result, \"life\", completed)",
        "differs from its witnessed transition");
    reject_publish("record_set(result, \"life:version\", 2)", "contact profile differs");
    reject_publish("record_set(result, \"base:haiku_contact:learn_clock\", [0,0,0,0,1])", "another private life");
    reject_publish("record_set(result, \"extra\", 1)", "extra fields");
    restore_inputs();
    require(learning_execute("invalid = haiku_iceberg_enable_words(legacy, 0)") != 0 &&
            strstr(am_get_error(), "requires an owned Janus life"), "v1 learner activation skipped Janus ownership");
    unchanged(owner_before, "rejected v1 activation changed the published owner");
    ++rejected;

    restore_inputs();
    require(!am_set_var_record("checked", checked_before), "restore detached learned result");
    require(!learning_execute("list_set(record_get(result, \"life:haiku_contact:learn_words\"), 1, \"changed\")"), am_get_error());
    require(same_record(am_get_var_record("checked"), checked_before), "learned result aliases its worker envelope");
    unchanged(owner_before, "envelope mutation changed a published owner");

    restore_inputs();
    char failed_save[8192];
    snprintf(failed_save, sizeof(failed_save),
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "checkpoint_save(checked, \"%s/no-such-parent/never.state\")\nrecord_swap(live, checked)", work);
    require(learning_execute(failed_save) != 0 && strstr(am_get_error(), "temporary create failed"),
            "learned failed-save gate missed its intended durable boundary");
    unchanged(owner_before, "failed save changed the published learned owner");
    require(same_record(am_get_var_record("checked"), checked_before), "failed save damaged detached learned result");
    restore_inputs();
    char publish[8192];
    snprintf(publish, sizeof(publish),
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)", saved_path);
    require(!learning_execute(publish), am_get_error());
    require(same_record(am_get_var_record("live"), checked_before), "published learned life differs from checked result");
    free(saved_bytes); saved_bytes = read_file(saved_path, &saved_size);
    require(learning_execute(publish) != 0 && strstr(am_get_error(), "another private life"),
            "stale learned completion was accepted");
    unchanged(checked_before, "stale completion changed the published learned owner");
    ++rejected;

    /* A real dream delivers native Haiku lines to the friend. The candidate's
       otherwise-valid delivery ancestry must match those precise events. */
    require(!learning_execute(
        "core = haiku_state_enable_dreams(haiku_state_new(\"regex\", 0, 0, 575, 57), 0)\n"
        "record_set(core, \"dream_config\", [1,1,16])\n"
        "live = haiku_iceberg_enable_words(haiku_iceberg_enable_janus(haiku_iceberg_new(core, [1,0,0,0]), 20740, 1, 0), 0)\n"
        "before = haiku_iceberg_part(live, \"voice\")\ncandidate = record_clone(before)\n"
        "experience = haiku_session_prepare(candidate, \"rain\", 0)\n"
        "request = haiku_iceberg_request_at(live, \"rain\", missing, missing, 20740)\n"
        "pending = record_clone(live)\nhaiku_iceberg_put(pending, \"voice\", candidate)\n"
        "result = haiku_iceberg_tail(pending, before, experience, request, 0, missing, missing)\n"
        "checked = haiku_iceberg_check_sources(live, result, experience, request, 0, missing, missing)\n"
        "assert(list_len(record_get(checked, \"voice:dream_history\")) == 16, \"publication fixture did not open its full dream\")\n"), am_get_error());
    am_record_free(checked_before); checked_before = retain_fixture();
    reject_publish(
        "rows = record_get(result, \"life:friend_contact:p:receipt_rows\")\ni = 0\n"
        "while i < 32:\n"
        "    if rows[i * 9] == 1 and rows[i * 9 + 2] == 6:\n"
        "        meta = record_get(result, \"life:friend_contact:contact_meta\")\n"
        "        meta[i * 8] = 5\n"
        "    i = i + 1\n"
        "haiku_contact_check(haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"friend_contact\"))",
        "receipt ancestry differs");
    reject_publish(
        "forged = haiku_iceberg_part(live, \"friend_contact\")\n"
        "names = haiku_words(\"" LEARN_FIELDS "\")\ni = 0\n"
        "while i < list_len(names):\n"
        "    name = list_get(names, i)\n"
        "    record_set(result, text_concat(\"life:friend_contact:\", name), record_get(forged, name))\n"
        "    i = i + 1\n"
        "haiku_contact_check(haiku_iceberg_part(haiku_iceberg_part(result, \"life\"), \"friend_contact\"))",
        "differs from its witnessed transition");

    am_persistent_clear();
    am_record_free(owner_before); am_record_free(result_before);
    am_record_free(experience_before); am_record_free(request_before);
    am_record_free(checked_before); free(saved_bytes);
    printf("PASS: %d learned transition/source/receipt/profile/stale refusals preserve all 237 leaves and checkpoint bytes\n", rejected);
    puts("PASS: 499-field detached completion, failed save and one-time learning save/swap");
    puts("PASS: exact human and sixteen-line dream provenance, including schema-valid forged learned successors");
    return 0;
}
