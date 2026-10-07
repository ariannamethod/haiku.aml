#!/usr/bin/env python3
"""Development-only replay of the pinned Python generator's actual methods.

Run with its original dependencies, e.g. PYTHONPATH=../reference-deps python
tests/reference/generator_oracle.py ../harmonix. Ordinary AML tests use the
committed fixtures and do not import this script or Python dependencies.
"""
import argparse
from collections import defaultdict
import json
import math
from pathlib import Path
import subprocess
import sys
from unittest.mock import patch
import warnings

import numpy as np


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    args = parser.parse_args()
    source = args.source.resolve()
    data = json.loads(Path(__file__).with_name("generator_inputs.json").read_text())
    revision = subprocess.check_output(
        ["git", "-C", str(source), "rev-parse", "HEAD"], text=True
    ).strip()
    assert revision == data["source"], (revision, data["source"])
    subprocess.run(
        ["git", "-C", str(source), "diff", "--exit-code", "HEAD", "--", "haiku/haiku.py"],
        check=True,
    )
    sys.path.insert(0, str(source / "haiku"))
    from haiku import HaikuGenerator

    preserved = repairs = 0
    for case in data["cases"]:
        generator = HaikuGenerator.__new__(HaikuGenerator)
        # Generation only iterates this attribute. Fix its order explicitly;
        # the source's set order is a separate hash-seed-dependent decision.
        generator.vocab = case["vocab"]
        generator.markov_chain = defaultdict(lambda: defaultdict(int))
        for first, second, third, count in case["edges"]:
            generator.markov_chain[first, second][third] = count
        generator._count_syllables = lambda word: case["syllables"][word]
        cursor = 0
        events = []

        def uniform(words):
            nonlocal cursor
            draw = case["draws"][cursor]
            cursor += 1
            index = math.floor(draw * len(words))
            events.append(dict(kind="uniform", words=list(words), draw=draw, index=index))
            return words[index]

        def weighted(words, p):
            nonlocal cursor
            # Match NumPy's rejection before consuming a scripted draw.
            if not np.isfinite(p).all():
                raise ValueError("probabilities contain NaN")
            draw = case["draws"][cursor]
            cursor += 1
            cumulative = np.cumsum(p)
            cumulative /= cumulative[-1]
            index = int(np.searchsorted(cumulative, draw, side="right"))
            events.append(dict(kind="weighted", words=list(words),
                               probabilities=p.tolist(), draw=draw, index=index))
            return words[index]

        with warnings.catch_warnings():
            warnings.simplefilter("ignore")
            try:
                with patch("random.choice", uniform), patch("numpy.random.choice", weighted):
                    if "n" in case:
                        result = generator.generate_candidates(case["n"], case["temperature"])
                    else:
                        result = " ".join(generator._generate_line(case["target"], case["temperature"]))
                actual = dict(result=result, cursor=cursor, events=events)
            except Exception as error:
                actual = dict(error=type(error).__name__ + ": " + str(error),
                              cursor=cursor, events=events)
        assert actual == case["original"], (case["name"], actual, case["original"])
        if case["decision"] == "preserved":
            assert case["expected"] == dict(result=actual["result"], cursor=cursor)
            preserved += 1
        else:
            # Repaired outputs are explicit decisions tested by AML fixtures.
            assert case["expected"] != {k: v for k, v in actual.items() if k != "events"}
            repairs += 1
    print(f"PASS: {preserved} preserved Python draw traces and {repairs} measured repair baselines")


if __name__ == "__main__":
    main()
