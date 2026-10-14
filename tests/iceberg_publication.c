/* Exact composite ownership at Haiku's completed-turn publication boundary. */
#include "state_test.h"
#include <math.h>

static AM_Record *owner_before, *result_before, *experience_before, *request_before;
static char saved_path[4096];
static unsigned char *saved_bytes;
static size_t saved_size;
static int rejected;

static int iceberg_execute(const char *fragment) {
    size_t length = strlen(root) + strlen(fragment) + 128;
    char *source = malloc(length), origin[4096];
    require(source != NULL, "iceberg source allocation");
    snprintf(source, length, "IMPORT \"%s/src/iceberg_session.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/publication-probe.aml", work);
    int rc = am_exec_source(source, origin);
    free(source);
    return rc;
}

static void restore_inputs(void) {
    require(!am_set_var_record("live", owner_before), "restore composite owner");
    require(!am_set_var_record("result", result_before), "restore completion");
    require(!am_set_var_record("experience", experience_before), "restore experience");
    require(!am_set_var_record("request", request_before), "restore request");
}

static void unchanged(const AM_Record *expected, const char *detail) {
    require(same_record(am_get_var_record("live"), expected), detail);
    same_file(saved_path, saved_bytes, saved_size);
}

static void reject_publish(const char *change, const char *diagnostic) {
    restore_inputs();
    if (change && *change) require(!iceberg_execute(change), am_get_error());
    AM_Record *snapshot = am_record_clone(am_get_var_record("result"));
    require(snapshot != NULL, "retain rejected result");
    char source[10000];
    snprintf(source, sizeof(source),
        "checked = haiku_iceberg_result_check(live, result, experience, request, 0)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)\n", saved_path);
    int rc = iceberg_execute(source);
    if (!rc || !strstr(am_get_error(), diagnostic)) {
        fprintf(stderr, "expected refusal: %s\nactual: %s\n", diagnostic, am_get_error());
        require(0, "iceberg completion unexpectedly accepted or wrong diagnostic");
    }
    unchanged(owner_before, "rejected completion changed a composite owner leaf");
    require(same_record(am_get_var_record("result"), snapshot), "validation mutated the result");
    am_record_free(snapshot);
    ++rejected;
}

int main(int argc, char **argv) {
    require(argc == 3, "root and fresh work directory required");
    root = argv[1]; work = argv[2];
    am_init(); am_use_notorch(); am_persistent_mode(1);
    require(!iceberg_execute(
        "missing = model_try_load(\"\")\n"
        "core = haiku_state_enable_dreams(haiku_state_new(\"regex\", 0, 0, 575, 57), 0)\n"
        "live = haiku_iceberg_new(core, [0,0,0,0])\n"
        "before = haiku_iceberg_part(live, \"voice\")\n"
        "candidate = record_clone(before)\n"
        "experience = haiku_session_prepare(candidate, \"water remembers the tree\", 0)\n"
        "request = haiku_iceberg_request(live, \"water remembers the tree\", missing, missing)\n"
        "pending = record_clone(live)\nhaiku_iceberg_put(pending, \"voice\", candidate)\n"
        "SPAWN complete:\n"
        "    return haiku_iceberg_tail(pending, before, experience, request, 0, missing, missing)\n"
        "AWAIT complete INTO result\n"
        "checked = haiku_iceberg_result_check(live, result, experience, request, 0)\n"), am_get_error());
    owner_before = am_record_clone(am_get_var_record("live"));
    result_before = am_record_clone(am_get_var_record("result"));
    experience_before = am_record_clone(am_get_var_record("experience"));
    request_before = am_record_clone(am_get_var_record("request"));
    AM_Record *checked_before = am_record_clone(am_get_var_record("checked"));
    require(owner_before && result_before && experience_before && request_before && checked_before,
            "retain exact publication fixtures");
    AM_List *keys = am_record_keys(owner_before);
    require(keys && keys->len == 185, "unexpected composite owner field count");
    am_list_free(keys);
    char error[1024] = {0};
    snprintf(saved_path, sizeof(saved_path), "%s/published.state", work);
    require(am_checkpoint_save(owner_before, saved_path, error, sizeof(error)) == 1, error);
    saved_bytes = read_file(saved_path, &saved_size);
    unchanged(owner_before, "worker completion published before validation");

    /* A genuinely different same-turn private life gives the same old request,
       but its complete predecessor must reject publication into this owner. */
    require(!iceberg_execute(
        "other = record_clone(live)\nrecord_set(other, \"friend_lexical:alpha\", 0.75)\n"
        "haiku_iceberg_check(other, 0)\n"
        "other_request = haiku_iceberg_request(other, \"water remembers the tree\", missing, missing)\n"
        "assert(record_equal(other_request, request), \"request fixture no longer isolates private state\")\n"
        "other_pending = record_clone(other)\nhaiku_iceberg_put(other_pending, \"voice\", candidate)\n"
        "wrong_result = haiku_iceberg_tail(other_pending, before, experience, other_request, 0, missing, missing)\n"), am_get_error());
    reject_publish("result = record_clone(wrong_result)", "another private life");
    reject_publish("record_set(result, \"request:a_ready\", 1)", "another request");
    reject_publish("record_set(result, \"request:a_identity\", \"changed source bytes\")", "another request");
    reject_publish("record_set(result, \"experience:quality\", 0.125)", "another experience");
    reject_publish("record_set(experience, \"quality\", 0.125)", "another experience");
    reject_publish("record_set(result, \"unowned_extra\", 1)", "extra fields");
    reject_publish("record_set(result, \"life:unowned_extra\", 1)", "extra fields");
    reject_publish("keys = record_get(result, \"life:keys\")\nlist_push(keys, list_get(keys, 0))\nrecord_set(result, \"life:keys\", keys)", "namespace repeats a field");
    reject_publish("record_set(result, \"version\", 2)", "completion format differs");

    /* Even one representable bit in a private field belongs to its base. */
    restore_inputs();
    AM_String *name = am_string_new("base:haiku_contact:p:field");
    const AML_Var *field = am_record_get(am_get_var_record("result"), name);
    require(field && field->type == AML_TYPE_ARRAY && field->array->len == 88,
            "find private predecessor field");
    field->array->data[0] = nextafterf(field->array->data[0], INFINITY);
    am_string_free(name);
    AM_Record *bit_result = am_record_clone(am_get_var_record("result"));
    require(bit_result && !same_record(bit_result, result_before), "private one-ULP mutation did not differ");
    require(!am_set_var_record("bit_result", bit_result), "retain one-ULP result");
    reject_publish("result = record_clone(bit_result)", "another private life");
    am_record_free(bit_result);

    /* The validated result owns its leaves independently of its envelope. */
    restore_inputs();
    require(!am_set_var_record("checked", checked_before), "restore detached completion");
    require(!iceberg_execute("map_set(record_get(result, \"life:voice:weights\"), \"the\", 0)"), am_get_error());
    require(same_record(am_get_var_record("checked"), checked_before), "unwrapped life aliases its envelope");
    unchanged(owner_before, "envelope mutation changed live owner");

    /* Failed durable publication must leave the old live owner and file paired. */
    restore_inputs();
    char failed_save[8192];
    snprintf(failed_save, sizeof(failed_save),
        "checked = haiku_iceberg_result_check(live, result, experience, request, 0)\n"
        "checkpoint_save(checked, \"%s/no-such-parent/never.state\")\n"
        "record_swap(live, checked)", work);
    require(iceberg_execute(failed_save) != 0 && strstr(am_get_error(), "temporary create failed"),
            "failed-save gate did not hit its intended file boundary");
    unchanged(owner_before, "failed save changed live owner");
    require(same_record(am_get_var_record("checked"), checked_before), "failed save damaged detached result");

    /* Successful save/swap advances once. The old completion is stale then. */
    restore_inputs();
    char publish[8192];
    snprintf(publish, sizeof(publish),
        "checked = haiku_iceberg_result_check(live, result, experience, request, 0)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)", saved_path);
    require(!iceberg_execute(publish), am_get_error());
    require(same_record(am_get_var_record("live"), checked_before), "published life differs from checked result");
    free(saved_bytes); saved_bytes = read_file(saved_path, &saved_size);
    int rc = iceberg_execute(publish);
    require(rc != 0 && strstr(am_get_error(), "another private life"), "stale completion was accepted");
    unchanged(checked_before, "stale/duplicate completion changed the published owner");
    ++rejected;
    am_persistent_clear();
    am_record_free(owner_before); am_record_free(result_before);
    am_record_free(experience_before); am_record_free(request_before);
    am_record_free(checked_before); free(saved_bytes);
    printf("PASS: %d exact-base/request/experience/envelope/stale refusals preserve all 185 leaves and checkpoint bytes\n", rejected);
    puts("PASS: detached result ownership, failed save, and one-time save/swap publication");
    return 0;
}
