"""Replay pinned Python foreground methods and the explicit empty-cloud repair."""
from collections import defaultdict
from pathlib import Path
from unittest.mock import patch
import argparse
import json
import math
import subprocess
import sys
import tempfile

import numpy as np

PARSER = argparse.ArgumentParser(description=__doc__)
PARSER.add_argument("--python-haiku", type=Path, required=True)
PARSER.add_argument("--check", action="store_true")
ARGS = PARSER.parse_args()
ROOT = Path(__file__).resolve().parents[2]
SOURCE = ARGS.python_haiku.resolve().parent
assert subprocess.check_output(["git", "-C", str(SOURCE), "rev-parse", "HEAD"], text=True).strip() == "abb878c52d763b73e5ad7a4d6a68a9ea7a248a39"
subprocess.run(["git", "-C", str(SOURCE), "diff", "--exit-code", "HEAD", "--",
                "haiku/chat.py", "haiku/tokenizer.py", "haiku/harmonix.py", "haiku/haiku.py",
                "haiku/rae.py", "haiku/rae_recursive.py", "haiku/phase4_bridges.py"], check=True)
sys.path.insert(0, str(SOURCE / "haiku"))
from haiku import HaikuGenerator, MLP, Value
from harmonix import Harmonix
from tokenizer import DualTokenizer
from rae import RecursiveAdapterEngine
from rae_recursive import RecursiveRAESelector
from phase4_bridges import HaikuBridges, state_id_from_metrics


class OrderedWords:
    def __init__(self, values):
        self.items = dict.fromkeys(values)
    def __iter__(self):
        return iter(self.items)
    def __len__(self):
        return len(self.items)
    def add(self, value):
        self.items[value] = None


def close(actual, expected, path="foreground"):
    # Phase4 iterates a set of metric names; changing that order can alter its
    # double-precision dot product by one ulp. Text/order/counts remain exact.
    if isinstance(expected, float):
        assert math.isclose(actual, expected, rel_tol=1e-12, abs_tol=1e-12), (path, actual, expected)
    elif isinstance(expected, dict):
        assert actual.keys() == expected.keys(), path
        for key in expected:
            close(actual[key], expected[key], path + "." + key)
    elif isinstance(expected, list):
        assert len(actual) == len(expected), path
        for i, (left, right) in enumerate(zip(actual, expected)):
            close(left, right, path + "." + str(i))
    else:
        assert actual == expected, (path, actual, expected)


def main():
    seeds = "rain moon wind cloud rest warm dream night light sky word sun".split()
    params = [(((17 * i) % 31) - 15) / 32 for i in range(57)]
    inputs = [" \t", "rain", "rain moon", "rain moon wind", "wind cloud rest",
              "rain moon wind rain moon wind rain moon wind rain moon wind rain moon",
              "rain moon wind cloud rest warm dream night light sky word sun sea sand star leaf stone dust fire frost",
              " \n", "CLOUD rain MOON", "!!!", " QuIt "]
    draws = [((i * 5 + 3) % 16) / 16 for i in range(4096)]
    cursor = 0
    draws_used = []

    def uniform(words):
        nonlocal cursor
        draw = draws[cursor]
        index = math.floor(draw * len(words))
        draws_used.append(dict(kind="uniform", draw=draw, index=index, words=list(words)))
        cursor += 1
        return words[index]

    def weighted(words, p):
        nonlocal cursor
        draw = draws[cursor]
        cdf = np.cumsum(p)
        cdf /= cdf[-1]
        index = int(np.searchsorted(cdf, draw, side="right"))
        draws_used.append(dict(kind="weighted", draw=draw, index=index, words=list(words), probabilities=p.tolist()))
        cursor += 1
        return words[index]

    with tempfile.TemporaryDirectory() as tmp:
        db = str(Path(tmp) / "cloud.db")
        observer = Harmonix(db)
        with patch("time.time", return_value=0):
            for word in seeds:
                observer.conn.execute("INSERT INTO words(word,weight,frequency,last_used,added_by) VALUES(?,1,0,0,'seed')", (word,))
            observer.conn.commit()
        generator = HaikuGenerator(seeds, str(Path(tmp) / "mathbrain.json"), db)
        generator.vocab = OrderedWords(seeds)
        for p, value in zip(generator.mlp_scorer.parameters(), params):
            p.data = value
        selector = RecursiveRAESelector.__new__(RecursiveRAESelector)
        selector.selector = MLP(5, [8, 1])
        selector.feature_dim, selector.hidden_dim = 5, 8
        selector.refinement_steps = 3
        selector.observations, selector.learning_rate = 0, 0.01
        selector.state_path = str(Path(tmp) / "rae.json")
        for p, value in zip(selector.selector.parameters(), params):
            p.data = value
        engine = RecursiveAdapterEngine(use_recursive=False)
        engine.recursive_selector = selector
        bridges = HaikuBridges(db)
        tokenizer = DualTokenizer(use_sentencepiece=False)
        turn = 0
        previous_state = None
        cases = []
        for raw in inputs:
            value = raw.strip()
            before_rng = cursor
            if value.lower() in ["quit", "exit", "q"]:
                cases.append(dict(input=raw, gate="quit", turn=turn, cursor=cursor))
                break
            if not value:
                cases.append(dict(input=raw, gate="empty", turn=turn, cursor=cursor))
                continue
            turn += 1
            events = ["input", "tokens"]
            tokens = tokenizer.tokenize_dual(value)
            triples = tokens["trigrams"]
            old_recent = list(generator.get_recent_trigrams())
            source_error = None
            try:
                with patch("time.time", return_value=turn):
                    observer.morph_cloud(tokens["subwords"])
            except Exception as error:
                assert not tokens["subwords"]
                source_error = type(error).__name__ + ": " + str(error)
                # The committed cloud organ already repairs this empty-active
                # SQL binding failure to no-op. Replay that declared boundary
                # and then call the unmodified remaining foreground methods.
            events.append("cloud")
            observer.update_trigrams(triples)
            events.append("observer_trigrams")
            generator.update_chain(triples)
            events.append("generator_chain_and_recent")
            recent = generator.get_recent_trigrams()
            saved_recent = list(recent)
            events.append("recent_snapshot")
            dissonance, pulse = observer.compute_dissonance(triples, recent)
            events.append("observation")
            temperature, observer_temperature = observer.adjust_temperature(dissonance)
            events.append("temperature")
            with patch("random.choice", uniform), patch("numpy.random.choice", weighted):
                candidates = generator.generate_candidates(n=5, temp=temperature)
            events.append("candidates")
            context = dict(user=value, user_trigrams=triples, pulse=pulse, dissonance=dissonance)
            selected = engine.reason(context, candidates, scorer=generator)
            index = candidates.index(selected)
            selected_check, confidence = selector.select_recursive(candidates, context)
            assert selected_check == selected
            events.extend(["rae", "response"])
            quality = .5
            if .3 < dissonance < .7:
                quality += .2
            if .4 < pulse.entropy < .8:
                quality += .15
            if .3 < pulse.novelty < .7:
                quality += .15
            quality = min(1, max(0, quality))
            events.append("quality")
            state = state_id_from_metrics(dissonance, pulse.entropy, quality)
            before = dict(dissonance=float(dissonance), entropy=pulse.entropy,
                          novelty=pulse.novelty, arousal=pulse.arousal, quality=.5)
            after = dict(before, quality=quality)
            flags = dict(boredom=dissonance < .3 and pulse.entropy < .4,
                         overwhelm=dissonance > .8 or pulse.arousal > .8,
                         stuck=quality < .4)
            with patch("time.time", return_value=turn):
                bridges.record_state(state, before, after, prev_state_id=previous_state,
                                     turn_id=f"turn_{turn}", **flags)
            events.append("state_transition")
            loss = generator.observe(selected, quality, user_context=triples)
            events.append("mathbrain_observe")
            previous_state = state
            assert recent == saved_recent == generator.get_recent_trigrams()
            cases.append(dict(input=raw, turn=turn, cursor_before=before_rng, cursor_after=cursor, source_error=source_error,
                              events=events, tokens=tokens["subwords"], triples=triples,
                              old_recent=old_recent, recent=saved_recent,
                              observation=[float(dissonance), pulse.novelty, pulse.arousal, pulse.entropy],
                              temperatures=[float(temperature), float(observer_temperature)],
                              candidates=candidates, selected=index, confidence=confidence, quality=quality,
                              state=state, flags={k: bool(v) for k, v in flags.items()}, loss=loss,
                              observations=generator.observations, running_loss=generator.running_loss,
                              parameters=[p.data for p in generator.mlp_scorer.parameters()],
                              rae_observations=selector.observations,
                              cloud=[list(row) for row in observer.conn.execute("SELECT word,weight,frequency,last_used,added_by FROM words ORDER BY id")],
                              observer_trigrams=[list(row) for row in observer.conn.execute("SELECT word1,word2,word3,count,resonance FROM trigrams ORDER BY id")],
                              chain=[[a,b,c,n] for (a,b), successors in generator.markov_chain.items() for c,n in successors.items()],
                              vocab=list(generator.vocab),
                              transitions=[list(row) for row in bridges.conn.execute("SELECT from_state_id,to_state_id,count,sum_similarity,sum_quality_delta,sum_overwhelm,sum_boredom,sum_stuck FROM haiku_transitions ORDER BY rowid")]))
        first = next(case for case in cases if "candidates" in case)
        coupled_loss = selector.observe(first["candidates"], first["candidates"][first["selected"]],
                                        first["quality"], {"user_trigrams": first["triples"]})
        coupled = dict(loss=coupled_loss, parameters=[p.data for p in selector.selector.parameters()],
                       observations=selector.observations)
        result = dict(source="abb878c52d763b73e5ad7a4d6a68a9ea7a248a39", seeds=seeds,
                      parameters=params, inputs=inputs, draws=draws[:cursor], cases=cases,
                      coupled=coupled)
        path = ROOT / "tests/reference/foreground_inputs.json"
        body = json.dumps(result, ensure_ascii=False, indent=2) + "\n"
        if ARGS.check:
            close(result, json.loads(path.read_text()))
        else:
            path.write_text(body)
        print(path)
        for case in cases:
            if "gate" in case:
                print(repr(case["input"]), case["gate"], case.get("error", ""))
            else:
                print(case["turn"], repr(case["input"]), case["observation"], "T", case["temperatures"][0], "q", case["quality"], "choice", case["selected"], "loss", case["loss"], "draws", case["cursor_after"] - case["cursor_before"])
        generator.close()
        observer.close()
        bridges.close()


if __name__ == "__main__":
    main()
