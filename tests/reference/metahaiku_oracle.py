#!/usr/bin/env python3
"""Replay the pinned Python inner voice; emit runtime-independent AML receipts."""
import argparse
from collections import defaultdict
import copy
import json
import math
from pathlib import Path
import subprocess
import sys
from types import SimpleNamespace
from unittest.mock import patch

PIN = "abb878c52d763b73e5ad7a4d6a68a9ea7a248a39"
ROOT = Path(__file__).resolve().parents[2]
VOCAB = "rain cloud wind light moon warm dream night".split()
EDGES = [VOCAB[i:i + 3] + [1] for i in range(len(VOCAB) - 2)]
EDGES.append(["rain", "cloud", "light", 3])


def replay(source):
    assert subprocess.check_output(["git", "-C", str(source.parent), "rev-parse", "HEAD"], text=True).strip() == PIN
    subprocess.run(["git", "-C", str(source.parent), "diff", "--exit-code", "HEAD", "--",
                    "haiku/metahaiku.py", "haiku/haiku.py"], check=True)
    sys.path.insert(0, str(source))
    from metahaiku import MetaHaiku
    from haiku import HaikuGenerator
    import numpy as np

    cases = [
        ("empty", "", .9, .9, .125),
        ("high_dissonance", "the first inner shard", .75, 0, .875),
        ("high_arousal", "a bright pulse", 0, .75, .875),
        ("exact_thresholds", "outside the buffer", .6, .6, .3),
        ("diversity", "a quieter moment", 0, 0, .25),
        ("diversity_boundary", "still outside", 0, 0, .3),
        ("whitespace", " \t\n\u00a0\u2028", .75, 0, .875),
        ("ten_words", "one two three four five six seven eight nine ten eleven twelve", .75, 0, .875),
        ("unicode_whitespace", "  pluie\u0085mémoire\u2007облако\u2028שלום\t🌧️\n", .75, 0, .875),
        ("hundred_codepoints", "é" * 60 + " " + "😀" * 60, .75, 0, .875),
        ("space_at_cut", "a" * 99 + " another", .75, 0, .875),
        ("combining_cut", "e\u0301" * 51, .75, 0, .875),
        ("duplicate", "a bright pulse", .75, 0, .875),
    ]
    cases += [(f"retention_{i}", f"memory number {i}", .75, 0, .875) for i in range(10)]

    class FixedGenerator:
        def generate_candidates(self, n, temp):
            assert n == 1 and temp == .7
            return ["rain holds the cloud\nour two voices move through night\nlight remembers us"]

    meta = MetaHaiku(FixedGenerator())
    bootstrap = []
    for name, spoken, dissonance, arousal, draw in cases:
        before = list(meta.bootstrap_buf)
        draws = []
        with patch("random.random", side_effect=lambda: draws.append(draw) or draw):
            meta._feed_bootstrap(dict(haiku=spoken, dissonance=dissonance,
                                      pulse=SimpleNamespace(arousal=arousal)))
        bootstrap.append(dict(name=name, spoken=spoken, dissonance=dissonance,
                              arousal=arousal, draw=draw, draws=len(draws), before=before,
                              after=list(meta.bootstrap_buf)))

    def generator():
        result = HaikuGenerator.__new__(HaikuGenerator)
        result.vocab = VOCAB.copy()
        result.markov_chain = defaultdict(lambda: defaultdict(int))
        for a, b, c, count in EDGES:
            result.markov_chain[a, b][c] = count
        assert [result._count_syllables(word) for word in VOCAB] == [1] * len(VOCAB)
        return result

    inputs = [
        ("are you here", "two voices\none field\nsoftly", [.75, .25, .125, .5]),
        ("where is the cloud", "rain speaks\nlight gathers\nwe listen", [.25, .5, .75, .125]),
        ("an empty earlier answer", "", [0, 0, 0, 0]),
        ("что помнит облако", "words live\na long night\nremembers", [.5, .75, 0, .25]),
    ]
    turns = []
    events = []
    model = generator()
    meta = MetaHaiku(model)
    model_before = copy.deepcopy((model.vocab, dict(model.markov_chain)))
    calls = []
    original_generate = model.generate_candidates

    def generate(n, temp):
        calls.append(dict(n=n, temperature=temp))
        return original_generate(n=n, temp=temp)

    def next_draw():
        # Start with rain, then choose between 1:3 successors at a draw which
        # differs between temperatures 0.7 and 1.0.
        if len(events) == 1:
            return 0
        if len(events) == 2:
            return .1875
        return ((len(events) * 5 + 3) % 16) / 16

    def diversity():
        draw = next_draw()
        events.append(dict(kind="admission", draw=draw))
        return draw

    def uniform(words):
        draw = next_draw()
        index = math.floor(draw * len(words))
        events.append(dict(kind="uniform", draw=draw, index=index, words=list(words)))
        return words[index]

    def weighted(words, p):
        draw = next_draw()
        cumulative = np.cumsum(p)
        cumulative /= cumulative[-1]
        index = int(np.searchsorted(cumulative, draw, side="right"))
        events.append(dict(kind="weighted", draw=draw, index=index, words=list(words), probabilities=p.tolist()))
        return words[index]

    for turn, (user, spoken, observation) in enumerate(inputs, 1):
        begin = len(events)
        context = dict(user=user, haiku=spoken, turn=turn, dissonance=observation[0],
                       pulse=SimpleNamespace(novelty=observation[1], arousal=observation[2], entropy=observation[3]))
        with patch("random.random", diversity), patch("random.choice", uniform), \
             patch("numpy.random.choice", weighted), patch.object(model, "generate_candidates", generate):
            internal = meta.reflect(context)
        assert meta.reflections[-1]["context"] is context
        assert (model.vocab, dict(model.markov_chain)) == model_before
        turns.append(dict(turn=turn, user=user, spoken=spoken, observation=observation,
                          internal=internal, bootstrap=list(meta.bootstrap_buf),
                          cursor_before=begin, cursor_after=len(events), events=copy.deepcopy(events[begin:])))
    assert calls == [dict(n=1, temperature=.7)] * len(turns)
    first_weighted = turns[0]["events"][2]
    assert first_weighted["kind"] == "weighted" and first_weighted["draw"] == .1875
    assert first_weighted["words"] == ["wind", "light"] and first_weighted["index"] == 1
    assert first_weighted["probabilities"][0] < .1875 < .25

    # Source reflection_seed and cloud_bias paths: different remembered shards
    # leave the same generated words and exact draw trace at fixed input/draws.
    seed_probes = []
    for initial in [[], ["a wholly different memory", "une nuit dans la pluie"]]:
        events.clear()
        model = generator()
        meta = MetaHaiku(model)
        meta.bootstrap_buf.extend(initial)
        context = dict(user=inputs[0][0], haiku=inputs[0][1], dissonance=.75,
                       pulse=SimpleNamespace(novelty=.25, arousal=.125, entropy=.5), turn=1)
        before = copy.deepcopy((model.vocab, dict(model.markov_chain)))
        with patch("random.random", diversity), patch("random.choice", uniform), patch("numpy.random.choice", weighted):
            internal = meta.reflect(context)
        seed_probes.append(dict(initial=initial, internal=internal, events=copy.deepcopy(events),
                                generation_state_unchanged=(model.vocab, dict(model.markov_chain)) == before))
    assert seed_probes[0]["internal"] == seed_probes[1]["internal"]
    assert seed_probes[0]["events"] == seed_probes[1]["events"]
    return dict(source=PIN, vocab=VOCAB, edges=EDGES, draw_overrides={1: 0, 2: .1875}, bootstrap=bootstrap, turns=turns,
                calls=calls, seed_probes=seed_probes)


def quoted(value):
    return json.dumps(value, ensure_ascii=False)


def text(lines, name, value):
    lines.append(f'{name} = ""')
    chunk = ""
    for cp in value:
        if len(quoted(chunk + cp).encode()) > 160:
            lines.append(f"{name} = text_concat({name}, {quoted(chunk)})")
            chunk = ""
        chunk += cp
    if chunk:
        lines.append(f"{name} = text_concat({name}, {quoted(chunk)})")


def expect_list(lines, expression, values):
    lines.append("expected = list_new()")
    for value in values:
        text(lines, "word", value)
        lines.append("list_push(expected, word)")
    lines.append(f'assert(text_equal(list_key({expression}), list_key(expected)), "Reflection ordered-list reference differs")')


def fixtures(data):
    # Independent chunks keep each emitted program comfortably under AML's
    # expanded source budget while preserving the real preceding deque contents.
    files = {}
    for start in range(0, len(data["bootstrap"]), 6):
        lines = ['IMPORT "../../src/metahaiku.aml"', 'rng = rng_new(575)', 'owner = record_new()',
                 'record_set(owner, "buffer", list_new())']
        for value in data["bootstrap"][start]["before"]:
            text(lines, "word", value)
            lines.append('list_push(record_get(owner, "buffer"), word)')
        for case in data["bootstrap"][start:start + 6]:
            lines.append(f'# {case["name"]}')
            text(lines, "spoken", case["spoken"])
            lines += ["tape = zeros(2)", f'tape[1] = {case["draw"]}',
                      'before = list_clone(record_get(owner, "buffer"))',
                      f'buffer = haiku_meta_bootstrap(record_get(owner, "buffer"), spoken, {case["dissonance"]}, {case["arousal"]}, rng, tape)',
                      'assert(text_equal(list_key(before), list_key(record_get(owner, "buffer"))), "Bootstrap changed its input owner")',
                      f'assert(tape[0] == {case["draws"]}, "Bootstrap admission draw order differs")']
            expect_list(lines, "buffer", case["after"])
            lines.append('record_set(owner, "buffer", buffer)')
        lines.append('PRINT "HAIKU_META_OK"')
        files[f"metahaiku_bootstrap_{start // 6 + 1:02}.aml"] = lines
    lines = ['IMPORT "metahaiku_support.aml"', 'state = meta_test_new(1)']
    history = []
    metrics = []
    for case in data["turns"]:
        lines.append(f'# Full Python reflection {case["turn"]}')
        text(lines, "user", case["user"])
        text(lines, "spoken", case["spoken"])
        lines.append("observation = zeros(4)")
        for index, value in enumerate(case["observation"]):
            lines.append(f"observation[{index}] = {value}")
        lines.append(f'inner = haiku_meta_reflect(state, user, spoken, observation, {case["turn"]})')
        text(lines, "wanted", case["internal"])
        lines += ['assert(text_equal(inner, wanted), "Reflection generated text differs from Python")',
                  f'assert(haiku_meta_check(state) == {case["turn"]}, "Reflection history count differs")',
                  'tape = record_get(state, "draws")',
                  f'assert(tape[0] == {case["cursor_after"]}, "Reflection complete draw order differs")']
        expect_list(lines, 'record_get(state, "meta_bootstrap")', case["bootstrap"])
        history += [case["user"], case["spoken"], case["internal"]]
        expect_list(lines, 'record_get(state, "meta_history")', history)
        metrics += [case["turn"]] + case["observation"]
        lines.append('metrics = record_get(state, "meta_metrics")')
        for index, value in enumerate(metrics):
            lines.append(f'assert(metrics[{index}] == {value}, "Reflection frozen context differs")')
        lines += ['user = "changed caller"', 'spoken = "changed caller"', 'observation[0] = 1']
    lines += ['assert(text_equal(list_key(record_get(state, "vocab")), list_key(haiku_words("rain cloud wind light moon warm dream night"))), "Reflection changed vocabulary")',
              'assert(map_len(record_get(state, "counts")) == 7, "Reflection changed transition memory")',
              'assert(map_get(record_get(state, "weights"), "sentinel") == 3, "Reflection changed cloud weights")',
              'PRINT "HAIKU_META_OK"']
    files["metahaiku_reflections.aml"] = lines
    return files


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--python-haiku", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    data = replay(args.python_haiku.resolve())
    outputs = {ROOT / "tests/reference/metahaiku_inputs.json": json.dumps(data, ensure_ascii=False, indent=2) + "\n"}
    for name, lines in fixtures(data).items():
        assert max(len(line.encode()) for line in lines) < 256
        assert len(lines) < 1200
        outputs[ROOT / "tests/fixtures" / name] = "\n".join(lines) + "\n"
    for path, body in outputs.items():
        if args.check:
            assert path.read_text() == body, f"Stale reflection receipt: {path}"
        else:
            path.write_text(body)
    print(f'PASS: {len(data["bootstrap"])} Python bootstrap cases, {len(data["turns"])} full reflection traces, unused-seed/cloud-bias probes')


if __name__ == "__main__":
    main()
