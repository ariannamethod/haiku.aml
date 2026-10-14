/* Inspect owner bytes on rejected input; compare the calendar to pinned C.
 * Oracle: ariannamethod/janus 5a945a9d7c1dc9abfa217f685f0a81455f9ac493,
 * janus.c calendar_cumulative_drift/calendar_dissonance. The explicit UTC
 * civil-day -> noon-epoch conversion belongs to Haiku's integration policy. */
#include "state_test.h"
#include <math.h>

static int janus_execute(const char *fragment) {
    size_t size = strlen(root) + strlen(fragment) + 128;
    char *program = malloc(size), origin[4096];
    require(program != NULL, "Janus program allocation");
    snprintf(program, size, "IMPORT \"%s/tests/janus_balance_support.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/janus-validation.aml", work);
    int rc = am_exec_source(program, origin);
    free(program); return rc;
}

static void rejected(const char *fragment, const char *error) {
    AM_Record *before = am_record_clone(am_get_var_record("live"));
    require(before != NULL, "Janus rejection snapshot");
    int rc = janus_execute(fragment);
    if (!rc || !strstr(am_get_error(), error)) {
        fprintf(stderr, "Janus expected: %s\nactual: %s\n", error, am_get_error());
        require(0, "wrong Janus rejection");
    }
    require(same_record(before, am_get_var_record("live")), "rejected Janus call changed an owned leaf");
    am_record_free(before);
}

static void calendar_case(int day) {
    int elapsed = (int)((float)day - 19999.5f);
    float years = (float)elapsed / 365.25f;
    float base = years * 11.25f;
    int cycles = (int)(years / 19.0f);
    float corrections = (float)(cycles * 7 * 30);
    int year = (int)fmodf(years, 19.0f) + 1;
    const int leaps[] = {3, 6, 8, 11, 14, 17, 19};
    for (int i = 0; i < 7; ++i) if (leaps[i] <= year) corrections += 30.0f;
    float drift = base - corrections;
    float dissonance = fabsf(fmodf(drift, 33.0f)) / 33.0f;
    char source[128];
    snprintf(source, sizeof(source), "calendar = haiku_janus_calendar(%d)", day);
    require(!janus_execute(source), am_get_error());
    int n = 0;
    const float *actual = am_get_var_array("calendar", &n);
    if (!actual || n != 3 || actual[0] != elapsed ||
        actual[1] != drift || actual[2] != dissonance) {
        fprintf(stderr, "Janus calendar day %d expected %.9g %.9g %.9g, actual %.9g %.9g %.9g\n",
                day, (double)elapsed, (double)drift, (double)dissonance,
                actual ? (double)actual[0] : 0, actual ? (double)actual[1] : 0,
                actual ? (double)actual[2] : 0);
        require(0, "owned calendar differs from independent pinned C equation");
    }
}

int main(int argc, char **argv) {
    require(argc == 3, "Janus root and work directory");
    root = argv[1]; work = argv[2];
    (void)execute; (void)expect_error;
    am_init(); am_use_notorch(); am_persistent_mode(1);
    const int boundaries[] = {-1000000, -999999, -365, -1, 0, 1, 19633, 19634,
        19998, 19999, 20000, 20001, 20364, 20365, 20729, 20730, 20731, 20740,
        999999, 1000000};
    for (size_t i = 0; i < sizeof(boundaries) / sizeof(boundaries[0]); ++i)
        calendar_case(boundaries[i]);
    for (int i = 0; i < 160; ++i) calendar_case(-999999 + i * 12578);
    require(!janus_execute(
        "live = haiku_janus_new(\"haiku\", 1, 0.5, 20740, 0)\n"
        "alpha = record_get(haiku_janus_snapshot(live, 20740), \"alpha\")\n"
        "receipt = janus_test_receipt(\"haiku\", alpha, 1, 1, 1)\n"
        "haiku_janus_expect(live, receipt, 20740)"), am_get_error());
    char path[4096], error[1024], fragment[8000];
    snprintf(path, sizeof(path), "%s/janus.state", work);
    require(am_checkpoint_save(am_get_var_record("live"), path, error, sizeof(error)) == 1, error);
    size_t size = 0;
    unsigned char *bytes = read_file(path, &size);
    snprintf(fragment, sizeof(fragment), "resumed = haiku_janus_load(\"%s\")", path);
    require(!janus_execute(fragment), am_get_error());
    require(same_record(am_get_var_record("live"), am_get_var_record("resumed")), "Janus pending restart changed an owned leaf");
    rejected("haiku_janus_snapshot(live, 20739)", "snapshot day moved backward");
    rejected("haiku_janus_calendar(1000001)", "integer is outside");
    rejected("haiku_janus_calendar(20740.5)", "integer is outside");
    rejected("fake = janus_test_receipt(\"friend\", alpha, 2, 2, 0)\nhaiku_janus_expect(live, fake, 20740)", "another presence");
    rejected("fake = janus_test_receipt(\"haiku\", 0.5, 2, 2, 0)\nhaiku_janus_expect(live, fake, 20740)", "freeze its captured balance");
    rejected("fake = janus_test_receipt(\"haiku\", alpha, 2, 2, 0)\nrecord_set(fake, \"scores\", zeros(47))\nhaiku_janus_expect(live, fake, 20740)", "vector shape differs");
    rejected("fake = janus_test_receipt(\"haiku\", alpha, 2, 2, 0)\nrecord_set(fake, \"words\", haiku_words(\"water tree\"))\nhaiku_janus_expect(live, fake, 20740)", "complete reviewed pool");
    rejected("fake = janus_test_receipt(\"haiku\", alpha, 2, 2, 0)\nrecord_set(fake, \"alpha\", 1.1)\nhaiku_janus_expect(live, fake, 20740)", "pending alpha is invalid");
    rejected("fake = janus_test_receipt(\"haiku\", alpha, 2, 2, 0)\nrecord_set(fake, \"availability\", 2)\nhaiku_janus_expect(live, fake, 20740)", "source availability differs");
    rejected("later = haiku_contact_event(\"human\", \"water\", \"en\", [0.5,0.5,1,0.5], [0], 0.5, 0, 2, 2)\nhaiku_janus_witness(live, later, 20739)", "witness day moved backward");
    const char *corruptions[] = {
        "record_set(broken, \"extra\", 1)",
        "record_set(broken, \"witness_source\", 6)",
        "record_set(broken, \"debt\", 1.01)",
        "record_set(broken, \"debt\", 0.1)",
        "record_set(broken, \"calendar\", \"different-calendar\")",
        "m = record_get(broken, \"pending_meta\")\nm[13] = 0",
        "m = record_get(broken, \"pending_meta\")\nm[15] = 1",
        "m = record_get(broken, \"seen\")\nm[7] = 1",
        "m = record_get(broken, \"history_rows\")\nm[0] = 0",
        "m = record_get(broken, \"history_rows\")\nm[12] = 0.5",
        "ids = record_get(broken, \"pending_identity\")\nlist_set(ids, 2, \"\")\nrecord_set(broken, \"pending_identity\", ids)",
        "record_set(broken, \"last_day\", 20739)"
    };
    AM_Record *original = am_record_clone(am_get_var_record("live"));
    require(original != NULL, "Janus original snapshot");
    for (size_t i = 0; i < sizeof(corruptions) / sizeof(corruptions[0]); ++i) {
        require(am_set_var_record("broken", original) == 0, "install Janus corrupt candidate");
        require(!janus_execute(corruptions[i]), am_get_error());
        AM_Record *snapshot = am_record_clone(am_get_var_record("broken"));
        require(snapshot != NULL, "Janus corrupt snapshot");
        require(janus_execute("haiku_janus_check(broken)") != 0, "corrupt Janus state accepted");
        require(same_record(snapshot, am_get_var_record("broken")), "Janus checker mutated corrupt input");
        require(same_record(original, am_get_var_record("live")), "corrupt Janus candidate leaked into owner");
        am_record_free(snapshot);
    }
    am_record_free(original);
    const char *fresh_corruptions[] = {
        "record_set(fresh, \"debt\", 0.1)",
        "record_set(fresh, \"last_day\", 20741)",
        "record_set(fresh, \"clock\", 1)",
        "m = record_get(fresh, \"seen\")\nm[0] = 1"
    };
    for (size_t i = 0; i < sizeof(fresh_corruptions) / sizeof(fresh_corruptions[0]); ++i) {
        snprintf(fragment, sizeof(fragment),
                 "fresh = haiku_janus_new(\"haiku\", 1, 0.5, 20740, 0)\n%s",
                 fresh_corruptions[i]);
        require(!janus_execute(fragment), am_get_error());
        AM_Record *snapshot = am_record_clone(am_get_var_record("fresh"));
        require(snapshot != NULL, "Janus fresh corrupt snapshot");
        require(janus_execute("haiku_janus_check(fresh)") != 0, "fresh Janus state invented an encounter");
        require(same_record(snapshot, am_get_var_record("fresh")), "Janus fresh checker mutated corrupt input");
        am_record_free(snapshot);
    }
    require(!janus_execute("haiku_janus_witness(live, later, 20740)\nhaiku_janus_witness(resumed, later, 20740)"), am_get_error());
    require(same_record(am_get_var_record("live"), am_get_var_record("resumed")), "Janus resumed later witness changed its result");
    original = am_record_clone(am_get_var_record("live"));
    require(original != NULL, "Janus replay snapshot");
    require(!janus_execute("haiku_janus_witness(live, later, 20000)\nhaiku_janus_expect(live, receipt, 20000)"), am_get_error());
    require(same_record(original, am_get_var_record("live")), "Janus replay changed any owned leaf");
    am_record_free(original);
    same_file(path, bytes, size); free(bytes);
    am_persistent_clear();
    puts("PASS: 180 UTC calendar boundaries match the pinned C float equation exactly");
    puts("PASS: ten rejected calls and sixteen corrupt states preserve every owned leaf");
    puts("PASS: pending restart and delayed witness agree bytewise; replay preserves the checkpoint");
    return 0;
}
