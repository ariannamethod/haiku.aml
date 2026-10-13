/* Inspect Phase4 leaves after rejected validation and rejected event admission. */
#include "state_test.h"

static int phase4_execute(const char *fragment) {
    size_t size = strlen(root) + strlen(fragment) + 100;
    char *program = malloc(size), origin[4096];
    require(program != NULL, "Phase4 program allocation");
    snprintf(program, size, "IMPORT \"%s/src/phase4.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/phase4-validation.aml", work);
    int result = am_exec_source(program, origin);
    free(program);
    return result;
}

static void phase4_error(const char *fragment, const char *detail) {
    int result = phase4_execute(fragment);
    if (!result || !strstr(am_get_error(), detail)) {
        fprintf(stderr, "Phase4 expected: %s\nactual: %s\n", detail, am_get_error());
        require(0, "Phase4 rejected operation has wrong diagnostic");
    }
}

struct BadCase { const char *change, *error; };
static const struct BadCase cases[] = {
    {"record_set(broken, \"phase4_prev\", 1)", "previous state must be text"},
    {"record_set(broken, \"phase4_log\", 1)", "needs string lists"},
    {"record_set(broken, \"phase4_log_rows\", 1)", "needs numeric vectors"},
    {"record_set(broken, \"phase4_log_rows\", matrix(2, 8, 0))", "must be a vector"},
    {"list_push(record_get(broken, \"phase4_log\"), \"partial\")", "complete rows"},
    {"list_push(record_get(broken, \"phase4_edges\"), \"partial\")", "complete pairs"},
    {"record_set(broken, \"phase4_log_rows\", zeros(1))", "history row lengths"},
    {"record_set(broken, \"phase4_snapshots\", zeros(1))", "snapshot lengths"},
    {"record_set(broken, \"phase4_totals\", zeros(1))", "aggregate lengths"},
    {"list_set(record_get(broken, \"phase4_metric_names\"), 1, \"quality\")", "dictionary has duplicates"},
    {"values = record_get(broken, \"phase4_log_rows\")\nvalues[0] = 1e39", "clock must be finite"},
    {"values = record_get(broken, \"phase4_log_rows\")\nvalues[1] = 1", "offsets are not contiguous"},
    {"values = record_get(broken, \"phase4_log_rows\")\nvalues[2] = 0.5", "metric count is invalid"},
    {"record_set(broken, \"phase4_metric_rows\", zeros(1))", "history is truncated"},
    {"values = record_get(broken, \"phase4_metric_rows\")\nvalues[0] = 2", "metric index is invalid"},
    {"values = record_get(broken, \"phase4_metric_rows\")\nvalues[0] = 0.5", "metric value is invalid"},
    {"values = record_get(broken, \"phase4_metric_rows\")\nvalues[1] = 1e39", "metric value is invalid"},
    {"values = record_get(broken, \"phase4_metric_rows\")\nvalues[2] = 0", "map has duplicate keys"},
    {"values = record_get(broken, \"phase4_metric_rows\")\nvalues[0] = 1", "dictionary order differs"},
    {"values = record_get(broken, \"phase4_log_rows\")\nvalues[5] = 0.5", "flags must be boolean"},
    {"list_set(record_get(broken, \"phase4_states\"), 0, \"elsewhere\")", "snapshot order differs"},
    {"list_set(record_get(broken, \"phase4_edges\"), 0, \"elsewhere\")", "aggregate is missing"},
    {"list_push(record_get(broken, \"phase4_metric_names\"), \"unused\")", "unused keys"},
    {"values = record_get(broken, \"phase4_snapshots\")\nvalues[0] = 2", "activation count differs"},
    {"values = record_get(broken, \"phase4_snapshots\")\nvalues[1] = 1", "latest snapshot differs"},
    {"values = record_get(broken, \"phase4_totals\")\nvalues[0] = 2", "aggregate differs from history"},
    {"values = record_get(broken, \"phase4_totals\")\nvalues[1] = 1e39", "aggregate must be finite"},
    {"values = record_get(broken, \"phase4_totals\")\nvalues[2] = 0", "aggregate differs from history"},
    {"record_set(broken, \"phase4_prev\", \"elsewhere\")", "previous state differs from history"},
    {"values = record_get(broken, \"phase4_metric_rows\")\nrecord_set(broken, \"phase4_metric_rows\", haiku_phase4_extend(values, len(values), 2))", "trailing values"}
};

int main(int argc, char **argv) {
    require(argc == 3, "Phase4 root and temporary directory");
    root = argv[1]; work = argv[2];
    /* The shared header also serves whole-session tests. This organ uses its
       own importer; referencing the other helpers keeps -Wall clean. */
    (void)execute; (void)expect_error; (void)read_file; (void)same_file;
    am_init(); am_use_notorch(); am_persistent_mode(1);
    require(!phase4_execute(
        "live = record_new()\nhaiku_phase4_init(live)\n"
        "before = map_new()\nafter = map_new()\nflags = zeros(3)\n"
        "map_set(before, \"quality\", 0.5)\nmap_set(before, \"shared\", 0.25)\n"
        "map_set(after, \"quality\", 0.75)\nmap_set(after, \"shared\", 0.5)\n"
        "haiku_phase4_record(live, \"a\", before, after, \"\", \"turn_1\", 1, flags)\n"
        "haiku_phase4_record(live, \"b\", before, after, \"a\", \"turn_2\", 2, flags)"), am_get_error());
    AM_Record *original = am_record_clone(am_get_var_record("live"));
    require(original != NULL, "Phase4 original clone");
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        require(am_set_var_record("broken", original) == 0, "Phase4 install corruption");
        require(!phase4_execute(cases[i].change), am_get_error());
        AM_Record *before = am_record_clone(am_get_var_record("broken"));
        require(before != NULL, "Phase4 corrupt clone");
        phase4_error("haiku_phase4_check(broken)", cases[i].error);
        require(same_record(am_get_var_record("broken"), before), "Phase4 validation mutated its input");
        phase4_error("haiku_phase4_record(broken, \"c\", before, after, \"b\", \"turn_3\", 3, flags)", cases[i].error);
        require(same_record(am_get_var_record("broken"), before), "Phase4 rejected record mutated its input");
        require(same_record(am_get_var_record("live"), original), "Phase4 corrupt candidate changed live owner");
        am_record_free(before);
    }
    phase4_error("haiku_phase4_record(live, \"c\", before, after, \"b\", \"turn_3\", 1e39, flags)", "clock must be finite");
    require(same_record(am_get_var_record("live"), original), "Phase4 bad clock changed owner");
    phase4_error("bad = [0, 0.5, 0]\nhaiku_phase4_record(live, \"c\", before, after, \"b\", \"turn_3\", 3, bad)", "flags must be boolean");
    require(same_record(am_get_var_record("live"), original), "Phase4 bad flags changed owner");
    phase4_error("bad = matrix(1, 3, 0)\nhaiku_phase4_record(live, \"c\", before, after, \"b\", \"turn_3\", 3, bad)", "flags must be a vector");
    require(same_record(am_get_var_record("live"), original), "Phase4 matrix flags changed owner");
    phase4_error("map_set(before, \"quality\", -3e38)\nmap_set(after, \"quality\", 3e38)\nhaiku_phase4_record(live, \"c\", before, after, \"b\", \"turn_3\", 3, flags)", "quality change exceeds finite");
    require(same_record(am_get_var_record("live"), original), "Phase4 overflowing quality changed owner");
    phase4_error("haiku_phase4_suggest(live, \"a\", 1, 0.5)", "result limit is invalid");
    phase4_error("haiku_phase4_history(live, \"a\", 0.5)", "history limit is invalid");
    phase4_error("haiku_phase4_metrics(live, 0, 2)", "metric side must be zero or one");
    phase4_error("haiku_phase4_entry(live, 3)", "history index is invalid");
    require(same_record(am_get_var_record("live"), original), "Phase4 rejected query changed owner");

    /* Reaching the declared history bound is valid; admitting one more is not. */
    require(!phase4_execute(
        "full = record_new()\nhaiku_phase4_init(full)\nrows = zeros(80000)\ni = 0\n"
        "while i < 10000:\n    list_push(record_get(full, \"phase4_log\"), \"same\")\n"
        "    list_push(record_get(full, \"phase4_log\"), \"\")\n"
        "    list_push(record_get(full, \"phase4_log\"), \"turn\")\n"
        "    rows[i * 8] = i\n    i = i + 1\n"
        "record_set(full, \"phase4_log_rows\", rows)\n"
        "list_push(record_get(full, \"phase4_states\"), \"same\")\n"
        "snapshot = [10000, 9999]\nrecord_set(full, \"phase4_snapshots\", snapshot)\n"
        "record_set(full, \"phase4_prev\", \"same\")\nhaiku_phase4_check(full)"), am_get_error());
    AM_Record *full = am_record_clone(am_get_var_record("full"));
    require(full != NULL, "Phase4 capacity clone");
    phase4_error("haiku_phase4_record(full, \"same\", before, after, \"same\", \"turn\", 10001, flags)", "history is full");
    require(same_record(am_get_var_record("full"), full), "Phase4 full history changed after rejection");
    am_record_free(full); am_record_free(original); am_persistent_clear();
    printf("PASS: %zu malformed Phase4 states and nine rejected calls preserve every inspected leaf\n", sizeof(cases) / sizeof(cases[0]));
    puts("PASS: 10000 complete bridge events remain valid; the next admission fails before mutation");
    return 0;
}
