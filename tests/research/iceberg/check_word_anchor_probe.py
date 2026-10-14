#!/usr/bin/env python3
"""Compare an explicit AML probe run with the measured frozen-body receipt."""
import json
import math
from pathlib import Path
import sys


def check(condition, detail):
    if not condition:
        raise SystemExit(detail)


check(len(sys.argv) == 2, "usage: check_word_anchor_probe.py RUNNER_OUTPUT")
recorded = json.loads(Path(__file__).with_name("word_anchor_probe.json").read_text())
lines = Path(sys.argv[1]).read_text().splitlines()
check(len(lines) == 6 + 9 * len(recorded["rows"]) + 1, "probe output length differs")
for offset, tag, key in [(0, "IDENTITY_A", "identity_a"),
                         (2, "IDENTITY_B", "identity_b"),
                         (4, "TOKENIZER", "tokenizer")]:
    check(lines[offset] == tag and lines[offset + 1] == recorded[key],
          f"{tag} differs from the measured provider")
anchors = ["water", "tree", "light", "music"]
heldout_hits = alias_hits = 0
seed_targets = {"river": "water", "rain": "water", "forest": "tree",
                "leaf": "tree", "sun": "light", "bright": "light",
                "song": "music", "piano": "music"}
for index, row in enumerate(recorded["rows"]):
    at = 6 + index * 9
    check(lines[at] == "ROW" and int(lines[at + 1]) == index,
          f"sample {index} order differs")
    check(lines[at + 2] == row["text"], f"sample {index} spelling differs")
    check(int(lines[at + 3]) == row["tokens"], f"{row['text']}: BPE length differs")
    check(int(lines[at + 4]) == row["seed_bound"], f"{row['text']}: seed membership differs")
    values = [float(value) for value in lines[at + 5:at + 9]]
    for anchor, value in zip(anchors, values):
        check(math.isfinite(value) and abs(value - row[anchor]) <= 1e-6,
              f"{row['text']}: {anchor} cosine differs: {value} vs {row[anchor]}")
    top = anchors[max(range(4), key=values.__getitem__)]
    check(top == row["top"], f"{row['text']}: leading anchor differs")
    if row["group"].startswith("heldout_"):
        heldout_hits += top == row["group"].split("_", 1)[1]
    if row["group"] == "seed_alias":
        alias_hits += top == seed_targets[row["text"]]
check(lines[-1] == "QUIET_ABSENCE_AND_EXACT_REPEAT_OK", "probe completion is absent")
print("PASS: 50 samples, 200 raw cosines, provider identities and token lengths")
print(f"Measured intended-anchor agreement: held-out {heldout_hits}/16; seed aliases {alias_hits}/8")
