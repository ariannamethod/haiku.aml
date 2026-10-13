#!/usr/bin/env python3
"""Pin Python exchange shards/metrics and emit dependency-free AML receipts.

The original alias path and the explicitly frozen scoring-context repair are
both retained. --check regenerates and compares every committed output byte.
"""
import argparse
from collections import defaultdict
from copy import deepcopy
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
from unittest.mock import patch

PIN = "abb878c52d763b73e5ad7a4d6a68a9ea7a248a39"
ROOT = Path(__file__).resolve().parents[2]
SEEDS = "rain cloud wind light moon warm dream night".split()
SPOKEN = "rain cloud wind\nlight moon warm dream\nnight cloud rain"
CASES = [
    ("rain cloud wind", ["rain", "cloud", "wind"], .5, .25, .125, .5),
    ("雨 mémoire שלום", ["雨", "mémoire", "שלום"], .85, .75, .625, .8),
    ("!!!", [], .5, 0, 0, 0),
]


def quoted(value):
    return json.dumps(value, ensure_ascii=False)


def flat(triples):
    return [word for triple in triples for word in triple]


def list_lines(name, values):
    return [f"{name} = list_new()"] + [f"list_push({name}, {quoted(word)})" for word in values]


def vector_lines(name, values):
    return [f"{name} = zeros({len(values)})"] + [f"{name}[{i}] = {value:.17g}" for i, value in enumerate(values)]


def replay(source):
    assert subprocess.check_output(["git", "-C", str(source.parent), "rev-parse", "HEAD"], text=True).strip() == PIN
    subprocess.run(["git", "-C", str(source.parent), "diff", "--exit-code", "HEAD", "--",
                    "haiku/harmonix.py", "haiku/haiku.py", "haiku/rae_recursive.py", "haiku/chat.py"], check=True)
    sys.path.insert(0, str(source))
    from harmonix import Harmonix
    from haiku import HaikuGenerator, Value
    from rae_recursive import RecursiveRAESelector
    import numpy as np

    class Capture:
        def __call__(self, values):
            self.features = [value.data for value in values]
            return Value(0)

    model = HaikuGenerator.__new__(HaikuGenerator)
    model.markov_chain = defaultdict(lambda: defaultdict(int))
    for i in range(len(SEEDS) - 2):
        model.markov_chain[tuple(SEEDS[i:i + 2])][SEEDS[i + 2]] += 1
    model.vocab, model.recent_trigrams = set(SEEDS), []
    model._save_markov_chain = lambda: None
    model._save_recent_trigrams = lambda: None
    model.mlp_scorer = Capture()
    selector = RecursiveRAESelector.__new__(RecursiveRAESelector)
    results = []
    old_cwd = Path.cwd()
    try:
        with tempfile.TemporaryDirectory(prefix="haiku-journal-reference-") as work:
            os.chdir(work)
            observer = Harmonix(str(Path(work) / "cloud.db"))
            for word in SEEDS:
                observer.conn.execute("INSERT INTO words(word,weight,frequency,last_used,added_by) VALUES(?,1,0,0,'seed')", (word,))
            observer.conn.commit()
            for index, (text, words, d, novelty, arousal, entropy) in enumerate(CASES):
                turn = index + 4
                user = [tuple(words[i:i + 3]) for i in range(len(words) - 2)]
                if words:
                    observer.morph_cloud(words)
                model.update_chain(user)
                original_system = model.get_recent_trigrams()
                frozen_system = deepcopy(original_system)
                model.score_haiku(SPOKEN, user)
                learned = model.mlp_scorer.features.copy()
                selected = selector.extract_features(SPOKEN, {"user_trigrams": user})
                temperatures = observer.adjust_temperature(d)
                quality = .5 + (.2 if .3 < d < .7 else 0) + (.15 if .4 < entropy < .8 else 0) + (.15 if .3 < novelty < .7 else 0)
                # Distinct first/later feedback demonstrates the original alias.
                # Generator-only dream tokens never enter observer.words.
                dreams = [[("rain", "cloud", "wind"), (f"dream{turn}", "new", "voice")],
                          [("later", f"dream{turn}", "voice")]]
                for trigrams in dreams:
                    model.update_chain(trigrams)
                model.score_haiku(SPOKEN, user)
                after_features = model.mlp_scorer.features.copy()
                assert learned[0] != after_features[0]
                payload = dict(input=text, output=SPOKEN, dissonance=d,
                               user_trigrams=user, system_trigrams=original_system,
                               haiku_temp=temperatures[0], harmonix_temp=temperatures[1])
                timestamp = 1700000000000 + index
                with patch("harmonix.time.time", return_value=timestamp / 1000):
                    observer.create_shard(payload)
                sql = observer.conn.execute("SELECT filepath,dissonance,haiku_temp,harmonix_temp FROM shards ORDER BY id DESC LIMIT 1").fetchone()
                original_shard = np.load(sql[0], allow_pickle=True).item()
                observer.record_metrics(.5, entropy, 1 - d)
                metrics = observer.conn.execute("SELECT perplexity,entropy,resonance,cloud_size FROM metrics ORDER BY id DESC LIMIT 1").fetchone()
                assert original_shard["system_trigrams"] == frozen_system + dreams[0]
                assert len(model.vocab) > metrics[3]
                results.append(dict(turn=turn, input=text, output=SPOKEN, words=words,
                                    user=user, system=frozen_system, dreams=dreams,
                                    observation=[d, novelty, arousal, entropy], temperatures=temperatures,
                                    quality=quality, mathbrain=learned, rae=selected,
                                    after_dream_mathbrain=after_features,
                                    original_shard=original_shard, sql_shard=list(sql[1:]), metrics=metrics))
            observer.close()
    finally:
        os.chdir(old_cwd)
    return {"source": PIN, "events": results}


def fixture(data):
    lines = ['IMPORT "journal_support.aml"', 'state = journal_test_new()',
             'assert(haiku_journal_check(state) == 0, "Journal upgrade invented earlier rows")']
    for index, event in enumerate(data["events"]):
        turn = event["turn"]
        lines += [f"# Python exchange {turn}: freeze before the first dream changes the chain.",
                  f"source = {quoted(event['input'])}", f"spoken = {quoted(event['output'])}"]
        lines += list_lines("user", flat(event["user"]))
        lines += ["recent = haiku_chain_update(record_get(state, 'rows'), record_get(state, 'counts'), record_get(state, 'vocab'), record_get(state, 'recent'), user)",
                  "record_set(state, 'recent', recent)", "system = list_clone(recent)"]
        lines += list_lines("expected_context", flat(event["system"]))
        lines += ["journal_test_list(system, expected_context)"]
        for word in event["words"]:
            lines += [f"map_set(record_get(state, 'weights'), {quoted(word)}, 1)"]
        lines += vector_lines("observation", event["observation"])
        lines += ["temperatures = haiku_temperatures(observation[0])",
                  f"captured = haiku_journal_capture(state, source, spoken, user, system, temperatures, {turn})",
                  "quality = haiku_foreground_quality(observation)",
                  f"event = haiku_foreground_bridge(observation, quality, {turn}, {turn})",
                  "haiku_phase4_turn(state, event)", f"record_set(state, 'turn', {turn})",
                  "list_push(record_get(state, 'meta_history'), source)",
                  "list_push(record_get(state, 'meta_history'), spoken)",
                  "list_push(record_get(state, 'meta_history'), " + quoted("the\ninner\nvoice") + ")"]
        for triples in event["dreams"]:
            lines += list_lines("feedback", flat(triples))
            lines += ["recent = haiku_chain_update(record_get(state, 'rows'), record_get(state, 'counts'), record_get(state, 'vocab'), record_get(state, 'recent'), feedback)",
                      "record_set(state, 'recent', recent)"]
        lines += [f"assert(haiku_journal_append(state, captured) == {index + 1}, 'Journal append count differs')",
                  f"assert(haiku_journal_check(state) == {index + 1}, 'Journal validation count differs')",
                  f"entry = haiku_journal_get(state, {index})",
                  "journal_test_list(record_get(entry, 'system_trigrams'), expected_context)",
                  "journal_test_list(record_get(entry, 'user_trigrams'), user)"]
        lines += vector_lines("expected", event["mathbrain"])
        lines += ["journal_test_vector(record_get(entry, 'mathbrain_features'), expected)"]
        lines += vector_lines("expected", event["rae"])
        lines += ["journal_test_vector(record_get(entry, 'rae_features'), expected)"]
        lines += vector_lines("expected", event["after_dream_mathbrain"])
        lines += ["journal_test_vector(haiku_mathbrain_features(spoken, record_get(state, 'rows'), record_get(state, 'counts'), user), expected)",
                  "frozen = record_get(entry, 'mathbrain_features')",
                  "assert(abs(frozen[0] - expected[0]) > 0.001, 'Journal recomputed a feature after dream feedback')",
                  "metrics = record_get(entry, 'metrics')"]
        for field, value in zip(("perplexity", "entropy", "resonance", "cloud_size"), event["metrics"]):
            lines += [f"assert(abs(map_get(metrics, '{field}') - {value:.17g}) <= 0.000002, 'Journal metric differs from Python')"]
        lines += ["assert(map_get(metrics, 'cloud_size') < list_len(record_get(state, 'vocab')), 'Journal confused observer and generator words')",
                  f"assert(abs(record_get(entry, 'haiku_temp') - {event['temperatures'][0]:.17g}) <= 0.000002, 'Journal haiku temperature differs from Python')",
                  f"assert(abs(record_get(entry, 'harmonix_temp') - {event['temperatures'][1]:.17g}) <= 0.000002, 'Journal observer temperature differs from Python')",
                  f"assert(abs(record_get(entry, 'dissonance') - {event['observation'][0]:.17g}) <= 0.000002, 'Journal dissonance differs from Python')",
                  "assert(text_equal(record_get(entry, 'format'), 'haiku-experience') and record_get(entry, 'version') == 1, 'Journal experience version differs')",
                  "assert(text_equal(record_get(entry, 'source'), 'foreground') and text_equal(record_get(entry, 'clock_kind'), 'turn'), 'Journal experience source/clock differs')",
                  f"assert(record_get(entry, 'base_revision') == {turn - 1}, 'Journal base revision differs')",
                  f"assert(record_get(entry, 'state_revision') == {turn}, 'Journal state revision differs')",
                  f"assert(record_get(entry, 'publication_order') == {turn}, 'Journal publication order differs')",
                  "assert(text_equal(record_get(entry, 'input'), source), 'Journal upgrade input index differs')",
                  "assert(text_equal(record_get(entry, 'output'), spoken), 'Journal upgrade output index differs')",
                  f"assert(abs(record_get(entry, 'quality') - {event['quality']:.17g}) <= 0.000002, 'Journal quality differs')"]
    lines += ["# Getter results own their mutable contexts/features independently.",
              "list_push(record_get(entry, 'user_trigrams'), 'caller mutation')",
              "features = record_get(entry, 'mathbrain_features')", "features[0] = 99",
              "map_set(record_get(entry, 'metrics'), 'cloud_size', 999)",
              "assert(haiku_journal_check(state) == 3, 'Journal getter aliases persistent history')",
              "# Earlier rows still refer to their own turn, before the upgrade's offset.",
              "entry = haiku_journal_get(state, 0)",
              "assert(text_equal(record_get(entry, 'input'), 'rain cloud wind'), 'Earlier journal input changed')",
              "assert(record_get(entry, 'turn') == 4, 'Earlier journal turn changed')"]
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--python-haiku", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    data = replay(args.python_haiku.resolve())
    outputs = {
        ROOT / "tests/reference/journal_receipt.json": json.dumps(data, ensure_ascii=False, indent=2) + "\n",
        ROOT / "tests/fixtures/journal_reference.aml": fixture(data),
    }
    for path, content in outputs.items():
        if args.check:
            assert path.read_text() == content, f"Journal reference changed: {path}"
        else:
            path.write_text(content)
    print("PASS: 3 Python shards/metrics, 30 frozen features, first-dream alias and generator-only feedback")


if __name__ == "__main__":
    main()
