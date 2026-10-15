/* Exact rejection and restart checks for the exchange's new owned leaves. */
#include "state_test.h"

static int exchange_execute(const char *fragment) {
    size_t size = strlen(root) + strlen(fragment) + 160;
    char *program = malloc(size), origin[4096];
    require(program != NULL, "exchange program allocation");
    snprintf(program, size, "IMPORT \"%s/tests/contact_exchange_fixture.aml\"\n%s\n", root, fragment);
    snprintf(origin, sizeof(origin), "%s/exchange-validation.aml", work);
    int rc = am_exec_source(program, origin);
    free(program);
    return rc;
}

int main(int argc, char **argv) {
    require(argc == 3, "exchange root and work directory");
    root = argv[1]; work = argv[2];
    (void)execute; (void)expect_error;
    am_init(); am_use_notorch(); am_persistent_mode(1);
    char path[4096], fragment[18000];
    snprintf(path, sizeof(path), "%s/interpreted.adopted", work);
    size_t bytes_size = 0;
    unsigned char *bytes = read_file(path, &bytes_size);
    snprintf(fragment, sizeof(fragment),
             "lived = haiku_contact_load(\"%s/interpreted.adopted\")\n"
             "before = haiku_contact_load(\"%s/interpreted.pre-adopt\")\n"
             "packet = checkpoint_load(\"%s/interpreted.packet\")\n"
             "peer = checkpoint_load(\"%s/interpreted.peer\")", work, work, work, work);
    require(!exchange_execute(fragment), am_get_error());
    AM_Record *original = am_record_clone(am_get_var_record("lived"));
    AM_Record *predecessor = am_record_clone(am_get_var_record("before"));
    AM_Record *packet = am_record_clone(am_get_var_record("packet"));
    AM_Record *peer = am_record_clone(am_get_var_record("peer"));
    require(original && predecessor && packet && peer, "exchange original snapshots");
    require(am_set_var_record("live", predecessor) == 0, "install exchange predecessor");
    require(!exchange_execute("haiku_contact_exchange_recv(live, peer, packet, 2)"), am_get_error());
    require(same_record(original, am_get_var_record("live")), "source receipt replay changed adopted bytes");
    require(!exchange_execute("haiku_contact_exchange_recv(live, peer, packet, 2)"), am_get_error());
    require(same_record(original, am_get_var_record("live")), "duplicate exchange changed owned bytes");

    const char *bad_packets[] = {
        "record_set(offer, \"format\", \"alien\")",
        "record_set(offer, \"count\", 16)",
        "record_set(offer, \"count\", -1)",
        "record_set(offer, \"sender\", \"friend\")",
        "record_set(offer, \"text\", \"music\")",
        "record_set(offer, \"root_turn\", 43)",
        "record_set(offer, \"sender_revision\", 0)",
        "record_set(offer, \"root\", \"(fire+(water+tree))\")",
        "record_set(offer, \"span\", 65)",
        "record_set(offer, \"offset\", -1)",
        "record_set(offer, \"base\", zeros(65))",
        "tree = record_get(offer, \"tree\")\ntree[0] = 88",
        "tree = record_get(offer, \"tree\")\ntree[0] = 0\ntree[1] = 1",
        "tree = record_get(offer, \"tree\")\ntree[2] = 2",
        "tree = record_get(offer, \"tree\")\ntree[3] = 100",
        "tree = record_get(offer, \"tree\")\ntree[59] = 1e-30",
        "record_set(incoming, \"parent_owner\", \"friend\")",
        "record_set(incoming, \"parent_source\", 4)\nrecord_set(incoming, \"origin_source\", 4)",
        "record_set(incoming, \"parent_event\", 99)",
        /* Same semantic path, different exact text: no retained seal exists. */
        "record_set(incoming, \"text\", \"FIRE water tree\")\nrecord_set(offer, \"text\", \"FIRE water tree\")"
    };
    size_t packet_count = sizeof(bad_packets) / sizeof(bad_packets[0]);
    for (size_t i = 0; i < packet_count; ++i) {
        require(am_set_var_record("live", predecessor) == 0, "install rejected exchange predecessor");
        require(am_set_var_record("offer", packet) == 0, "install invalid exchange offer");
        require(am_set_var_record("incoming", peer) == 0, "install invalid exchange delivery");
        require(!exchange_execute(bad_packets[i]), am_get_error());
        AM_Record *offer_before = am_record_clone(am_get_var_record("offer"));
        AM_Record *peer_before = am_record_clone(am_get_var_record("incoming"));
        require(offer_before && peer_before, "invalid packet snapshots");
        if (!exchange_execute("haiku_contact_exchange_recv(live, incoming, offer, 2)")) {
            fprintf(stderr, "accepted exchange input %zu: %s\n", i, bad_packets[i]);
            require(0, "invalid exchange delivery was accepted");
        }
        require(same_record(predecessor, am_get_var_record("live")), "rejected delivery changed the receiver");
        require(same_record(offer_before, am_get_var_record("offer")), "receiver changed rejected packet input");
        require(same_record(peer_before, am_get_var_record("incoming")), "receiver changed rejected contact input");
        require(same_record(original, am_get_var_record("lived")), "rejected exchange changed another owner");
        am_record_free(offer_before); am_record_free(peer_before);
    }
    require(am_set_var_record("live", predecessor) == 0, "install wrong-boundary predecessor");
    require(exchange_execute("haiku_contact_exchange_recv(live, peer, packet, 1)") != 0, "exchange ignored its actual receiver dream");
    require(same_record(predecessor, am_get_var_record("live")), "wrong dream changed the receiver");

    const char *corruptions[] = {
        "record_set(broken, \"extra\", 1)",
        "record_set(broken, \"exchange_origin\", zeros(1))",
        "clock = record_get(broken, \"exchange_clock\")\nclock[0] = 0",
        "clock = record_get(broken, \"exchange_clock\")\nclock[4] = 99",
        "clock = record_get(broken, \"exchange_clock\")\nclock[5] = 42",
        "clock = record_get(broken, \"exchange_clock\")\nclock[6] = 0",
        "clock = record_get(broken, \"exchange_clock\")\nclock[7] = 0",
        "clock = record_get(broken, \"exchange_clock\")\nclock[7] = 17",
        "clock = record_get(broken, \"exchange_clock\")\nclock[8] = 1",
        "origin = record_get(broken, \"exchange_origin\")\norigin[1] = 1",
        "origin = record_get(broken, \"exchange_origin\")\norigin[3] = 0",
        "origin = record_get(broken, \"exchange_origin\")\norigin[4] = 16",
        "origin = record_get(broken, \"exchange_origin\")\norigin[5] = 2",
        "origin = record_get(broken, \"exchange_origin\")\norigin[8] = 0",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[14] = 42",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[16] = 1",
        "nodes = record_get(broken, \"symbol_nodes\")\nnodes[18] = 14\nnodes[19] = 14",
        "rows = record_get(broken, \"symbol_rows\")\nrows[(128 + 14) * 139] = 1e-30",
        "rows = record_get(broken, \"exchange_rows\")\nrows[0] = 99",
        "rows = record_get(broken, \"exchange_rows\")\nrows[3] = 0",
        "rows = record_get(broken, \"exchange_rows\")\nrows[4] = 43",
        "rows = record_get(broken, \"exchange_rows\")\nrows[10] = 0",
        "rows = record_get(broken, \"exchange_rows\")\nrows[11] = 0",
        "rows = record_get(broken, \"exchange_rows\")\nrows[12] = 0",
        "rows = record_get(broken, \"exchange_rows\")\nrows[13] = 0",
        "rows = record_get(broken, \"exchange_rows\")\nrows[14] = 88",
        "rows = record_get(broken, \"exchange_rows\")\nrows[16] = 2",
        "rows = record_get(broken, \"exchange_rows\")\nrows[17] = 100",
        "rows = record_get(broken, \"exchange_rows\")\nrows[74] = 4",
        "rows = record_get(broken, \"exchange_rows\")\nrows[137] = 1e-30",
        "rows = record_get(broken, \"exchange_rows\")\nrows[16 * 138 - 1] = 1e-30",
        "text = record_get(broken, \"exchange_text\")\nlist_set(text, 0, \"music\")\nrecord_set(broken, \"exchange_text\", text)",
        "text = record_get(broken, \"exchange_text\")\nlist_set(text, 2, \"friend\")\nrecord_set(broken, \"exchange_text\", text)",
        "text = record_get(broken, \"exchange_text\")\nlist_set(text, 3, \"(fire+(water+tree))\")\nrecord_set(broken, \"exchange_text\", text)",
        "text = record_get(broken, \"exchange_text\")\nlist_set(text, 63, \"unused\")\nrecord_set(broken, \"exchange_text\", text)",
        "clock = record_get(broken, \"exchange_clock\")\nclock[6] = 2"
    };
    size_t corrupt_count = sizeof(corruptions) / sizeof(corruptions[0]);
    for (size_t i = 0; i < corrupt_count; ++i) {
        require(am_set_var_record("broken", original) == 0, "install corrupt exchange candidate");
        require(!exchange_execute(corruptions[i]), am_get_error());
        AM_Record *snapshot = am_record_clone(am_get_var_record("broken"));
        require(snapshot != NULL, "corrupt exchange snapshot");
        if (!exchange_execute("haiku_contact_check(broken)")) {
            fprintf(stderr, "accepted exchange corruption %zu: %s\n", i, corruptions[i]);
            require(0, "corrupt exchange state was accepted");
        }
        require(same_record(snapshot, am_get_var_record("broken")), "exchange checker mutated its corrupt input");
        require(same_record(original, am_get_var_record("lived")), "corrupt exchange state changed another owner");
        am_record_free(snapshot);
    }
    same_file(path, bytes, bytes_size); free(bytes);
    am_record_free(original); am_record_free(predecessor);
    am_record_free(packet); am_record_free(peer);
    am_persistent_clear();
    printf("PASS: %zu invalid exchange deliveries preserve receiver and inputs\n", packet_count + 1);
    printf("PASS: %zu corrupt exchange records are rejected without mutation\n", corrupt_count);
    puts("PASS: exchange receipt replay and preceding checkpoint are exact");
    return 0;
}
