#!/usr/bin/env python3
"""Run the pinned Python rings and render their dependency-free AML receipts.

Development only. Random floating draws and Random._randbelow are scripted;
the original choice, randint, sample and Overthinkg methods remain active.
"""
import argparse
import json
import math
from pathlib import Path
import random
import sqlite3
import struct
import subprocess
import sys
import tempfile
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
PIN = "abb878c52d763b73e5ad7a4d6a68a9ea7a248a39"


def f32(value):
    return struct.unpack("<f", struct.pack("<f", value))[0]


def cases():
    draws = [((i * 5 + 3) % 64) / 64 for i in range(300)]
    basic = dict(words=["d", "b", "a", "c"],
                 existing=[["a", "b", "c", 7, .25]],
                 recent=[["a", "d", "d"]], draws=draws)
    result = [dict(basic, name="frozen_snapshot", draws=[0, 0, .375, .375] * 5
                   + [0, 0, .875, .875] * 7 + [0, 0, .375, .375] * 3),
              dict(basic, name="mixed_drift"),
              dict(basic, name="no_recent", recent=[]),
              dict(basic, name="no_observer", existing=[]),
              dict(basic, name="no_context", existing=[], recent=[]),
              dict(basic, name="too_few_words", words=["b", "a"], existing=[]),
              dict(basic, name="ignored_duplicate", recent=[["a", "b", "c"]],
                   draws=[0, 0, 0, 0] * 15),
              dict(basic, name="repeated_word_sets", recent=[["a", "a", "a"]],
                   draws=[0, 0, 0, 0] * 15),
              dict(basic, name="new_context_word", recent=[["a", "b", "new"]],
                   draws=[0, 0, 0, 0] * 15),
              dict(basic, name="new_drift_word", recent=[["a", "b", "new"]],
                   draws=[.875, 0, 0, 0] * 5 + [0, 0, 0, 0] * 10),
              dict(basic, name="new_meta_word", recent=[["a", "b", "new"]],
                   draws=[.875, 0, 0, 0] * 12 + [0, 0, 0, 0] * 3),
              dict(basic, name="unicode_sql_order", words=["é", "α", "😀", "a b", "B", "aa", "a", "z"],
                   existing=[["a b", "é", "😀", 2, .5]], recent=[]),
              dict(basic, name="observer_token_identity", words=["rain", "a b", "moon"],
                   existing=[["a b", "rain", "moon", 2, .5]], recent=[]),
              dict(basic, name="sample_pool_21", words=[f"w{i:02}" for i in reversed(range(21))],
                   existing=[], recent=[]),
              dict(basic, name="sample_set_22", words=[f"w{i:02}" for i in reversed(range(22))],
                   existing=[], recent=[], draws=[0, 0, .125, .125, .25] + draws)]
    sparse = [f"w{i:02}" for i in range(75)]
    result.append(dict(basic, name="aggregate_coherence", words=list(reversed(sparse)),
                       existing=[sparse[i:i + 3] + [1, .5] for i in range(0, 75, 3)],
                       recent=[sparse[i:i + 3] for i in range(0, 75, 3)]))
    threshold_rows = [["a", "b", "x", 2, .5], ["a", "c", "y", 1, .5],
                      ["b", "d", "z", 1, .5], ["c", "p", "q", 1, .5],
                      ["u", "v", "w", 1, .5]]
    threshold_words = list(dict.fromkeys(word for row in threshold_rows for word in row[:3]))
    # The first sorted word is a; preserve recent abc with an a-for-a drift.
    result += [dict(basic, name="threshold_equal", words=threshold_words,
                    existing=threshold_rows, recent=[["a", "b", "c"]],
                    draws=[0, 0, 0, 0] * 15),
               dict(basic, name="threshold_above", words=threshold_words,
                    existing=threshold_rows[:-1] + [["c", "v", "w", 1, .5]],
                    recent=[["a", "b", "c"]], draws=[0, 0, 0, 0] * 15)]
    return result


def oracle(case, Harmonix, Overthinkg):
    with tempfile.TemporaryDirectory() as directory:
        path = str(Path(directory) / "cloud.db")
        harmonix = Harmonix(path)
        connection = harmonix.conn
        cloud = [[word, (i + 1) / 4, i, 3, "seed"] for i, word in enumerate(case["words"])]
        connection.executemany(
            "INSERT INTO words(word,weight,frequency,last_used,added_by) VALUES(?,?,?,?,?)", cloud)
        connection.executemany(
            "INSERT INTO trigrams(word1,word2,word3,count,resonance) VALUES(?,?,?,?,?)", case["existing"])
        connection.commit()
        over = Overthinkg(path)
        word_query = over._get_words()
        triple_query = over._get_trigrams()
        query_plan = {query: [row[3] for row in connection.execute("EXPLAIN QUERY PLAN " + query)]
                      for query in ["SELECT word FROM words", "SELECT word1, word2, word3 FROM trigrams"]}
        assert word_query == sorted(case["words"])
        assert triple_query == sorted(tuple(row[:3]) for row in case["existing"])
        cursor = 0
        events = []
        rings = []
        initial = list(triple_query)
        process = over._process_ring

        def floating():
            nonlocal cursor
            value = f32(case["draws"][cursor])
            cursor += 1
            events.append(dict(kind="float", draw=value))
            return value

        def below(size):
            nonlocal cursor
            value = f32(case["draws"][cursor])
            cursor += 1
            index = math.floor(value * size)
            events.append(dict(kind="index", size=size, draw=value, index=index))
            return index

        def capture(ring, existing):
            assert existing == initial
            rings.append(dict(ring=ring.ring, source=ring.source, trigrams=ring.trigrams,
                              coherence=ring.coherence,
                              admission=[over.compute_coherence(t, existing) for t in ring.trigrams],
                              cursor=cursor))
            process(ring, existing)
            assert existing == initial

        over._process_ring = capture
        with patch("random.random", floating), patch.object(random._inst, "_randbelow", below), \
                patch("time.time", return_value=7):
            over.expand(case["recent"])
        after_cloud = [list(row) for row in connection.execute(
            "SELECT word,weight,frequency,last_used,added_by FROM words ORDER BY id")]
        after_observer = [list(row) for row in connection.execute(
            "SELECT word1,word2,word3,count,resonance FROM trigrams ORDER BY id")]
        over.close()
        harmonix.close()
        return dict(case, draws=case["draws"][:cursor], clock=7, cloud=cloud, query_words=word_query,
                    query_trigrams=triple_query, query_plan=query_plan, rings=rings,
                    cursor=cursor, events=events, after_cloud=after_cloud, after_observer=after_observer)


def q(value):
    return json.dumps(value, ensure_ascii=False)


def scalar(value):
    return format(value, ".17g")


def new_list(lines, name, values):
    lines.append(f"{name} = list_new()")
    lines.extend(f"list_push({name}, {q(value)})" for value in values)


def row_call(lines, method, row):
    new_list(lines, "triple", row[:3])
    lines.append(f"{method}(state, triple, {scalar(row[3])}, {scalar(row[4])})")


def fixture(case):
    lines = ['# Generated by tests/reference/rings_oracle.py; exact pinned Python receipt.',
             'IMPORT "rings_support.aml"', 'state = ring_test_state()',
             'record_set(state, "clock", 7)']
    for word, weight, frequency, clock, origin in case["cloud"]:
        lines.append(f"ring_test_seed(state, {q(word)}, {scalar(weight)}, {frequency}, {clock}, {q(origin)})")
    for row in case["existing"]:
        row_call(lines, "ring_test_seed_row", row)
    new_list(lines, "recent", [word for row in case["recent"] for word in row])
    draws = case["draws"]
    lines.append(f"tape = zeros({len(draws) + 1})")
    lines.extend(f"tape[{i + 1}] = {scalar(value)}" for i, value in enumerate(draws))
    lines += ['record_set(state, "draws", tape)', 'old = record_clone(state)',
              'receipt = haiku_overthinkg(state, recent)',
              'actual = record_get(receipt, "trigrams")']
    expected = [word for ring in case["rings"] for row in ring["trigrams"] for word in row]
    new_list(lines, "wanted", expected)
    lines += ["ring_test_list(actual, wanted)",
              'actual_coherence = record_get(receipt, "coherence")',
              'actual_admission = record_get(receipt, "admission")']
    scores = [value for ring in case["rings"] for value in ring["admission"]]
    coherence = [ring["coherence"] for ring in case["rings"]]
    lines += [f"assert(len(actual_coherence) == {len(coherence) or 1}, \"Ring coherence size differs\")",
              f"assert(len(actual_admission) == {len(scores) or 1}, \"Ring admission size differs\")"]
    for i, value in enumerate(coherence or [0]):
        lines.append(f"ring_test_near(actual_coherence[{i}], {scalar(value)})")
    for i, value in enumerate(scores or [0]):
        lines.append(f"ring_test_near(actual_admission[{i}], {scalar(value)})")
    lines += ['actual_tape = record_get(state, "draws")',
              f"assert(actual_tape[0] == {case['cursor']}, \"Ring draw cursor differs\")",
              'ring_test_map(record_get(state, "voice_rng"), record_get(old, "voice_rng"))',
              'ring_test_untouched(state, old)',
              f'assert(map_len(record_get(state, "weights")) == {len(case["after_cloud"])}, "Ring cloud size differs")',
              f'assert(map_len(record_get(state, "observer_counts")) == {len(case["after_observer"])}, "Ring observer size differs")']
    for i, (word, weight, frequency, clock, origin) in enumerate(case["after_cloud"]):
        lines.append(f"ring_test_cloud(state, {q(word)}, {scalar(weight)}, {frequency}, {clock}, {q(origin)}, {i})")
    new_list(lines, "expected_rows", [word for row in case["after_observer"] for word in row[:3]])
    lines.append('ring_test_list(record_get(state, "observer_rows"), expected_rows)')
    for row in case["after_observer"]:
        row_call(lines, "ring_test_row", row)
    # Normal chat supplies cloud-subset recent triples. The direct API also
    # accepts external context; only admitted external words enter its cloud.
    cloud_words = {row[0] for row in case["after_cloud"]}
    if all(word in cloud_words for word in expected):
        lines += ['record_set(state, "ring_trigrams", actual)',
                  'record_set(state, "ring_coherence", actual_coherence)',
                  'record_set(state, "ring_admission", actual_admission)',
                  'haiku_rings_check(state)']
    lines.append("ECHO HAIKU_RINGS_OK")
    assert max(len(line.encode()) for line in lines) < 256
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--python-haiku", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    assert sys.version_info[:2] == (3, 12), "The pinned sampling oracle uses CPython 3.12"
    source = args.python_haiku.resolve().parent
    assert subprocess.check_output(["git", "-C", str(source), "rev-parse", "HEAD"], text=True).strip() == PIN
    subprocess.run(["git", "-C", str(source), "diff", "--exit-code", "HEAD", "--",
                    "haiku/overthinkg.py", "haiku/harmonix.py", "haiku/chat.py"], check=True)
    sys.path.insert(0, str(source / "haiku"))
    from harmonix import Harmonix
    from overthinkg import Overthinkg
    data = dict(source=PIN, python=sys.version.split()[0], sqlite=sqlite3.sqlite_version,
                cases=[oracle(case, Harmonix, Overthinkg) for case in cases()])
    outputs = {ROOT / "tests/reference/rings_inputs.json": json.dumps(data, ensure_ascii=False, indent=2) + "\n"}
    for case in data["cases"]:
        outputs[ROOT / f"tests/fixtures/rings_{case['name']}.aml"] = fixture(case)
    for path, body in outputs.items():
        if args.check:
            assert path.read_text() == body, path
        else:
            path.write_text(body)
    print(f"PASS: {len(data['cases'])} pinned Python ring receipts and generated AML fixtures")


if __name__ == "__main__":
    main()
