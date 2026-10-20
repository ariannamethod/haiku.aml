/* A real completed form-bearing turn is the source of every refusal. */
#include "state_test.h"

static AM_Record *owner_before, *result_before, *experience_before;
static unsigned char *saved_bytes;
static size_t saved_size;
static char saved_path[4096];
static int rejected;

static void restore_inputs(void) {
    require(!am_set_var_record("live", owner_before), "restore living owner");
    require(!am_set_var_record("result", result_before), "restore living completion");
    require(!am_set_var_record("experience", experience_before), "restore living receipt");
}

static void unchanged(void) {
    require(same_record(am_get_var_record("live"), owner_before), "rejected form changed its live owner");
    same_file(saved_path, saved_bytes, saved_size);
}

static void reject_publish(const char *change, const char *diagnostic) {
    restore_inputs();
    require(!execute(change), am_get_error());
    AM_Record *snapshot = am_record_clone(am_get_var_record("result"));
    require(snapshot != NULL, "retain rejected living completion");
    char publication[8192];
    snprintf(publication, sizeof(publication),
        "checked = haiku_session_result_check(live, result, experience, 0)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)", saved_path);
    expect_error(publication, diagnostic);
    unchanged();
    require(same_record(am_get_var_record("result"), snapshot), "form validation mutated rejected completion");
    am_record_free(snapshot);
    ++rejected;
}

int main(int argc, char **argv) {
    require(argc == 3, "root and retained work directory required");
    root = argv[1]; work = argv[2];
    am_init(); am_use_notorch(); am_persistent_mode(1);
    char setup[14000];
    snprintf(setup, sizeof(setup),
        "live = checkpoint_load(\"%s/living-publication-base.state\")\n"
        "result = checkpoint_load(\"%s/living-publication-result.state\")\n"
        "experience = checkpoint_load(\"%s/living-publication-experience.state\")\n"
        "checked = haiku_session_result_check(live, result, experience, 0)\n", work, work, work);
    require(!execute(setup), am_get_error());
    owner_before = am_record_clone(am_get_var_record("live"));
    result_before = am_record_clone(am_get_var_record("result"));
    experience_before = am_record_clone(am_get_var_record("experience"));
    AM_Record *checked_before = am_record_clone(am_get_var_record("checked"));
    require(owner_before && result_before && experience_before && checked_before, "retain living publication inputs");
    snprintf(saved_path, sizeof(saved_path), "%s/published-living.state", work);
    char error[1024] = {0};
    require(am_checkpoint_save(owner_before, saved_path, error, sizeof(error)) == 1, error);
    saved_bytes = read_file(saved_path, &saved_size);

    reject_publish(
        "values = record_get(result, \"state:form_values\")\nvalues[1] = values[1] + 0.001\n",
        "form receipt differs from its lived climate");
    reject_publish(
        "values = record_get(result, \"state:form_values\")\n"
        "values[len(values) - 9] = 3 - values[len(values) - 9]\n",
        "form receipt differs from its lived climate");
    reject_publish(
        "values = record_get(result, \"experience:form_receipt\")\nvalues[1] = values[1] + 0.001\n",
        "another experience");
    /* Coherently alter both frozen receipt copies; the canonical historical
       climate, rather than agreement between two copies, still rejects it. */
    reject_publish(
        "values = record_get(result, \"experience:form_receipt\")\nvalues[1] = values[1] + 0.001\n"
        "values = record_get(experience, \"form_receipt\")\nvalues[1] = values[1] + 0.001\n",
        "form receipt differs from its lived climate");
    /* Dropping the oldest row and advancing the activation boundary is a
       closed-schema-valid state. Publication must retain its predecessor. */
    reject_publish(
        "record_set(result, \"state:form_started_turn\", 2)\n"
        "old = record_get(result, \"state:form_values\")\nvalues = zeros(len(old) - 9)\ni = 0\n"
        "while i < len(values):\n    values[i] = old[i + 9]\n    i = i + 1\n"
        "record_set(result, \"state:form_values\", values)\n"
        "candidate = record_new()\nkeys = record_get(result, \"state_keys\")\ni = 0\n"
        "while i < list_len(keys):\n    key = list_get(keys, i)\n"
        "    record_set(candidate, key, record_get(result, text_concat(\"state:\", key)))\n    i = i + 1\n"
        "haiku_state_check(candidate, 0)\n",
        "changed its form boundary");
    reject_publish(
        "list_set(record_get(result, \"state:meta_history\"), 1, \"rain rain rain rain rain\\nrain rain rain rain rain rain rain\\nrain rain rain rain rain\")\n",
        "changed earlier words");
    reject_publish(
        "values = record_get(result, \"state:meta_metrics\")\nvalues[0] = values[0] + 1\n",
        "reflection turn order is invalid");
    reject_publish(
        "values = record_get(result, \"state:journal_values\")\nvalues[3] = values[3] * 0.5 + 0.01\n",
        "changed earlier climate or learning");
    reject_publish(
        "values = record_get(result, \"state:journal_values\")\n"
        "assert(values[5] == 0, \"Signed-zero probe needs the real zero overlap\")\nvalues[5] = -0.0\n",
        "changed earlier climate or learning");
    /* Same adaptive syllables, word counts, diversity and context overlap:
       the closed state remains valid, but those are different lived words. */
    reject_publish(
        "list_set(record_get(result, \"state:meta_history\"), 4, \"gently large should\\nuncover explore could\\nliving growing fluid\")\n"
        "list_set(record_get(result, \"state:dream_history\"), 0, \"gently large should\\nuncover explore could\\nliving growing fluid\")\n"
        "candidate = record_new()\nkeys = record_get(result, \"state_keys\")\ni = 0\n"
        "while i < list_len(keys):\n    key = list_get(keys, i)\n"
        "    record_set(candidate, key, record_get(result, text_concat(\"state:\", key)))\n    i = i + 1\n"
        "haiku_state_check(candidate, 0)\n",
        "changed earlier words");
    reject_publish("record_set(result, \"state:form_policy\", \"other-policy\")", "form policy is unsupported");
    reject_publish("record_set(result, \"state:form_values\", zeros(1))", "form history length differs");

    restore_inputs();
    char publication[8192];
    snprintf(publication, sizeof(publication),
        "checked = haiku_session_result_check(live, result, experience, 0)\n"
        "checkpoint_save(checked, \"%s/no-such-parent/never.state\")\nrecord_swap(live, checked)", work);
    expect_error(publication, "temporary create failed");
    unchanged();
    require(same_record(am_get_var_record("checked"), checked_before), "failed save damaged living result");
    ++rejected;
    restore_inputs();
    snprintf(publication, sizeof(publication),
        "checked = haiku_session_result_check(live, result, experience, 0)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)", saved_path);
    require(!execute(publication), am_get_error());
    require(same_record(am_get_var_record("live"), checked_before), "living publication differs from checked result");
    free(saved_bytes); saved_bytes = read_file(saved_path, &saved_size);
    expect_error(publication, "stale base revision");
    require(same_record(am_get_var_record("live"), checked_before), "stale form completion changed owner");
    same_file(saved_path, saved_bytes, saved_size);
    ++rejected;
    am_persistent_clear();
    am_record_free(owner_before); am_record_free(result_before); am_record_free(experience_before);
    am_record_free(checked_before); free(saved_bytes);
    printf("HAIKU_LIVING_PUBLICATION_OK (%d refusals)\n", rejected);
    return 0;
}
