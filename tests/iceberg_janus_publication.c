/* Janus profiles and one captured day travel through the exact publication. */
#include "state_test.h"
#include <math.h>
#ifdef HAIKU_WRAP_CLOCK
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#endif

static AM_Record *owner_before, *result_before, *experience_before, *request_before;
static char saved_path[4096];
static unsigned char *saved_bytes;
static size_t saved_size;
static int rejected;
#ifdef HAIKU_WRAP_CLOCK
static atomic_int clock_reads, worker_reads;
static pthread_t foreground;

/* A second read crosses midnight. Every operation below gets a fresh clock. */
time_t __wrap_time(time_t *destination) {
    int read = atomic_fetch_add(&clock_reads, 1);
    if (!pthread_equal(pthread_self(), foreground)) atomic_fetch_add(&worker_reads, 1);
    time_t now = (time_t)20740 * 86400 + 86399 + read * 2;
    if (destination) *destination = now;
    return now;
}
#endif

static int janus_execute(const char *fragment) {
    size_t length = strlen(root) + strlen(fragment) + 128;
    char *source = malloc(length), origin[4096];
    require(source != NULL, "Janus source allocation");
    snprintf(source, length, "IMPORT \"%s/src/iceberg_session.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/janus-publication-probe.aml", work);
    int rc = am_exec_source(source, origin);
    free(source);
    return rc;
}

static void restore_inputs(void) {
    require(!am_set_var_record("live", owner_before), "restore Janus owner");
    require(!am_set_var_record("result", result_before), "restore Janus completion");
    require(!am_set_var_record("experience", experience_before), "restore experience");
    require(!am_set_var_record("request", request_before), "restore captured request");
}

static void unchanged(const AM_Record *expected, const char *detail) {
    require(same_record(am_get_var_record("live"), expected), detail);
    same_file(saved_path, saved_bytes, saved_size);
}

static void reject_publish(const char *change, const char *diagnostic) {
    restore_inputs();
    if (change && *change) require(!janus_execute(change), am_get_error());
    AM_Record *snapshot = am_record_clone(am_get_var_record("result"));
    require(snapshot != NULL, "retain rejected Janus result");
    char source[10000];
    snprintf(source, sizeof(source),
        "IMPORT \"%s/src/iceberg_session.aml\"\n"
        "checked = haiku_iceberg_result_check(live, result, experience, request, 0)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)\n", root, saved_path);
    expect_error(source, diagnostic);
    unchanged(owner_before, "rejected completion changed a Janus owner leaf");
    require(same_record(am_get_var_record("result"), snapshot), "validation mutated the Janus envelope");
    am_record_free(snapshot);
    ++rejected;
}

#ifdef HAIKU_WRAP_CLOCK
static void check_clock(const char *fragment, int expected) {
    atomic_store(&clock_reads, 0);
    atomic_store(&worker_reads, 0);
    require(!janus_execute(fragment), am_get_error());
    if (atomic_load(&clock_reads) != expected || atomic_load(&worker_reads) != 0) {
        fprintf(stderr, "clock reads: expected %d foreground, observed %d total / %d worker\n",
                expected, atomic_load(&clock_reads), atomic_load(&worker_reads));
        require(0, "the completed turn did not retain one foreground calendar capture");
    }
}
#endif

int main(int argc, char **argv) {
    require(argc == 3, "root and fresh work directory required");
    root = argv[1]; work = argv[2];
#ifdef HAIKU_WRAP_CLOCK
    foreground = pthread_self();
#endif
    am_init(); am_use_notorch(); am_persistent_mode(1);
    require(!janus_execute(
        "missing = model_try_load(\"\")\n"
        "core = haiku_state_enable_dreams(haiku_state_new(\"regex\", 0, 0, 575, 57), 0)\n"
        "legacy = haiku_iceberg_new(core, [1,0,0,0])\n"
        "live = haiku_iceberg_enable_janus(legacy, 20740, 1, 0)\n"
        "before = haiku_iceberg_part(live, \"voice\")\n"
        "candidate = record_clone(before)\n"
        "experience = haiku_session_prepare(candidate, \"water remembers the tree\", 0)\n"
        "request = haiku_iceberg_request_at(live, \"water remembers the tree\", missing, missing, 20741)\n"
        "pending = record_clone(live)\nhaiku_iceberg_put(pending, \"voice\", candidate)\n"
        "SPAWN complete:\n"
        "    return haiku_iceberg_tail(pending, before, experience, request, 0, missing, missing)\n"
        "AWAIT complete INTO result\n"
        "checked = haiku_iceberg_result_check(live, result, experience, request, 0)\n"
        "assert(record_get(checked, \"haiku_janus:revision\") > 0, \"publication fixture did not meet its human witness\")\n"
        "assert(record_get(checked, \"friend_janus:revision\") == 0, \"publication fixture unexpectedly dreamed\")\n"), am_get_error());
    owner_before = am_record_clone(am_get_var_record("live"));
    result_before = am_record_clone(am_get_var_record("result"));
    experience_before = am_record_clone(am_get_var_record("experience"));
    request_before = am_record_clone(am_get_var_record("request"));
    AM_Record *checked_before = am_record_clone(am_get_var_record("checked"));
    require(owner_before && result_before && experience_before && request_before && checked_before,
            "retain exact Janus publication fixtures");
    AM_List *keys = am_record_keys(owner_before);
    require(keys && keys->len == 227, "unexpected Janus owner field count");
    am_list_free(keys);
    keys = am_record_keys(result_before);
    require(keys && keys->len == 479, "unexpected Janus completion field count");
    am_list_free(keys);
    char error[1024] = {0};
    snprintf(saved_path, sizeof(saved_path), "%s/published-janus.state", work);
    require(am_checkpoint_save(owner_before, saved_path, error, sizeof(error)) == 1, error);
    saved_bytes = read_file(saved_path, &saved_size);
    unchanged(owner_before, "Janus worker published before validation");

    reject_publish("record_set(result, \"request:day\", 20742)", "another request");
    reject_publish("record_set(request, \"day\", 20742)", "another request");
    reject_publish("record_set(request, \"day\", 20742)\nrecord_set(result, \"request:day\", 20742)", "another calendar day");
    reject_publish("record_set(result, \"base:friend_janus:fixed_alpha\", 0.75)", "another private life");
    reject_publish("record_set(result, \"life:haiku_janus:mode\", 0)", "changed its Janus profile");
    reject_publish("record_set(result, \"life:friend_janus:fixed_alpha\", 0.75)", "changed its Janus profile");
    reject_publish("record_set(result, \"life:haiku_janus:birth_day\", 20739)", "changed its Janus profile");
    reject_publish("record_set(result, \"life:haiku_janus:calendar\", \"another-calendar\")", "calendar identity differs");
    reject_publish("record_set(result, \"life:haiku_janus:witness_source\", 6)", "witness role differs");
    reject_publish("record_set(result, \"life:friend_janus:last_day\", 20741)", "empty history has lived state");
    reject_publish("record_set(request, \"extra\", 1)\nhaiku_iceberg_put(result, \"request\", request)", "request fields differ");
    reject_publish("record_set(result, \"unowned_extra\", 1)", "extra fields");
    reject_publish("keys = record_get(result, \"life:haiku_janus:keys\")\nlist_push(keys, list_get(keys, 0))\nrecord_set(result, \"life:haiku_janus:keys\", keys)", "namespace repeats a field");

    /* A structurally valid v1 completion still cannot downgrade this life. */
    reject_publish(
        "completed = haiku_iceberg_part(result, \"life\")\n"
        "old_core = haiku_iceberg_part(completed, \"voice\")\n"
        "old_hlex = haiku_iceberg_part(completed, \"haiku_lexical\")\n"
        "old_flex = haiku_iceberg_part(completed, \"friend_lexical\")\n"
        "old_hcontact = haiku_iceberg_part(completed, \"haiku_contact\")\n"
        "old_fcontact = haiku_iceberg_part(completed, \"friend_contact\")\n"
        "short = haiku_iceberg_pack(old_core, old_hlex, old_flex, old_hcontact, old_fcontact, record_get(completed, \"settings\"), record_get(completed, \"admission_text\"), record_get(completed, \"admission_values\"))\n"
        "result = record_new()\nrecord_set(result, \"format\", \"haiku-iceberg-result\")\nrecord_set(result, \"version\", 1)\n"
        "haiku_iceberg_put(result, \"base\", live)\nhaiku_iceberg_put(result, \"life\", short)\n"
        "haiku_iceberg_put(result, \"request\", request)\nhaiku_iceberg_put(result, \"experience\", experience)",
        "changed its profile");

    /* Every representable bit in the private expectation belongs to its base. */
    restore_inputs();
    AM_String *name = am_string_new("base:haiku_janus:pending_scores");
    const AML_Var *field = am_record_get(am_get_var_record("result"), name);
    require(field && field->type == AML_TYPE_ARRAY && field->array->len == 48,
            "find private Janus predecessor scores");
    field->array->data[0] = nextafterf(field->array->data[0], INFINITY);
    am_string_free(name);
    AM_Record *bit_result = am_record_clone(am_get_var_record("result"));
    require(bit_result && !same_record(bit_result, result_before), "Janus one-ULP mutation did not differ");
    require(!am_set_var_record("bit_result", bit_result), "retain Janus one-ULP result");
    reject_publish("result = record_clone(bit_result)", "another private life");
    am_record_free(bit_result);

    restore_inputs();
    require(!am_set_var_record("checked", checked_before), "restore detached Janus completion");
    require(!janus_execute("record_set(result, \"life:haiku_janus:debt\", 0.75)"), am_get_error());
    require(same_record(am_get_var_record("checked"), checked_before), "unwrapped Janus aliases its envelope");
    unchanged(owner_before, "envelope mutation changed live Janus owner");

    restore_inputs();
    char failed_save[8192];
    snprintf(failed_save, sizeof(failed_save),
        "checked = haiku_iceberg_result_check(live, result, experience, request, 0)\n"
        "checkpoint_save(checked, \"%s/no-such-parent/never.state\")\n"
        "record_swap(live, checked)", work);
    require(janus_execute(failed_save) != 0 && strstr(am_get_error(), "temporary create failed"),
            "Janus failed-save gate missed the durable boundary");
    unchanged(owner_before, "failed save changed live Janus owner");
    require(same_record(am_get_var_record("checked"), checked_before), "failed save damaged detached Janus result");

    restore_inputs();
    char publish[8192];
    snprintf(publish, sizeof(publish),
        "checked = haiku_iceberg_result_check(live, result, experience, request, 0)\n"
        "checkpoint_save(checked, \"%s\")\nrecord_swap(live, checked)", saved_path);
    require(!janus_execute(publish), am_get_error());
    require(same_record(am_get_var_record("live"), checked_before), "published Janus differs from checked result");
    free(saved_bytes); saved_bytes = read_file(saved_path, &saved_size);
    int rc = janus_execute(publish);
    require(rc != 0 && strstr(am_get_error(), "another private life"), "stale Janus completion was accepted");
    unchanged(checked_before, "stale completion changed the published Janus owner");
    ++rejected;

#ifdef HAIKU_WRAP_CLOCK
    require(!janus_execute(
        "clock_legacy = record_clone(legacy)\n"
        "clock_live = haiku_iceberg_enable_janus(clock_legacy, 20740, 1, 0)"), am_get_error());
    check_clock("clock_request = haiku_iceberg_request(clock_legacy, \"rain\", missing, missing)", 0);
    check_clock(
        "clock_request = haiku_iceberg_request(clock_live, \"rain\", missing, missing)\n"
        "assert(record_get(clock_request, \"day\") == 20740, \"request did not capture foreground day\")", 1);
    check_clock("haiku_iceberg_turn_at(clock_live, \"rain\", 0, missing, missing, 1, 20740)", 0);
    check_clock(
        "haiku_iceberg_turn(clock_live, \"memory\", 0, missing, missing, 1)\n"
        "assert(record_get(clock_live, \"haiku_janus:last_day\") == 20740, \"worker crossed the captured midnight\")", 1);
    check_clock("haiku_iceberg_turn(clock_legacy, \"rain\", 0, missing, missing, 1)", 0);
    unchanged(checked_before, "clock probes changed the published owner");
#endif

    am_persistent_clear();
    am_record_free(owner_before); am_record_free(result_before);
    am_record_free(experience_before); am_record_free(request_before);
    am_record_free(checked_before); free(saved_bytes);
    printf("PASS: %d Janus day/profile/base/envelope/stale refusals preserve all 227 leaves and checkpoint bytes\n", rejected);
    puts("PASS: 479-field detached completion, failed save, and one-time Janus save/swap");
#ifdef HAIKU_WRAP_CLOCK
    puts("PASS: v1 reads no calendar; v2 captures once in foreground across worker midnight; supplied day reads none");
#else
    puts("SKIP: host clock interception requires Linux --wrap=time");
#endif
    return 0;
}
