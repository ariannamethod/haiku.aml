"""Read-only receipts for pinned Python Haiku's save/load boundaries."""
from pathlib import Path
import argparse
import contextlib
import io
import json
import math
import os
import subprocess
import sys
import tempfile
from unittest.mock import patch

PARSER = argparse.ArgumentParser(description=__doc__)
PARSER.add_argument("--python-haiku", type=Path, required=True)
PARSER.add_argument("--check", action="store_true")
ARGS = PARSER.parse_args()
ROOT = Path(__file__).resolve().parent
SOURCE = ARGS.python_haiku.resolve().parent
assert subprocess.check_output(["git", "-C", str(SOURCE), "rev-parse", "HEAD"], text=True).strip() == "abb878c52d763b73e5ad7a4d6a68a9ea7a248a39"
subprocess.run(["git", "-C", str(SOURCE), "diff", "--exit-code", "HEAD", "--",
                "haiku/harmonix.py", "haiku/haiku.py", "haiku/rae_recursive.py"], check=True)
sys.path.insert(0, str(SOURCE / "haiku"))
import numpy as np
from haiku import HaikuGenerator, MLP
from rae_recursive import RecursiveRAESelector
from harmonix import Harmonix


def mathbrain(path):
    result = HaikuGenerator.__new__(HaikuGenerator)
    result.mlp_scorer = MLP(5, [8, 1])
    for i, param in enumerate(result.mlp_scorer.parameters()):
        param.data = (i - 28) / 64
    result.state_path = str(path)
    result.lr = .01
    result.observations, result.last_loss, result.running_loss = 90, .6, .4
    return result


def rae(path):
    result = RecursiveRAESelector.__new__(RecursiveRAESelector)
    result.selector = MLP(5, [8, 1])
    for param in result.selector.parameters():
        param.data = -.125
    result.state_path = str(path)
    result.learning_rate, result.observations = .01, 90
    result.feature_dim, result.hidden_dim, result.refinement_steps = 5, 8, 3
    return result


def main():
    report = dict(source="abb878c52d763b73e5ad7a4d6a68a9ea7a248a39")
    with tempfile.TemporaryDirectory() as tmp:
        tmp = Path(tmp)
        path = tmp / "mathbrain.json"
        original = mathbrain(path)
        original.lr = .075
        original._save_mathbrain_state()
        loaded = mathbrain(path)
        loaded._load_mathbrain_state()
        report["mathbrain_rate_roundtrip"] = dict(saved=json.loads(path.read_text())["lr"], loaded=loaded.lr)

        data = json.loads(path.read_text())
        data["parameters"][:4] = [2.125, 1.5, -2.0, "bad"]
        data["observations"] = 123
        path.write_text(json.dumps(data))
        loaded = mathbrain(path)
        before = [p.data for p in loaded.mlp_scorer.parameters()]
        loaded._load_mathbrain_state()
        after = [p.data for p in loaded.mlp_scorer.parameters()]
        report["mathbrain_bad_parameter"] = dict(changed_indexes=[i for i, (a,b) in enumerate(zip(before,after)) if a != b],
                                                  first_four=after[:4], observations=loaded.observations)
        data["parameters"] = [float("nan")] + [0.0] * 56
        path.write_text(json.dumps(data))
        loaded = mathbrain(path)
        loaded._load_mathbrain_state()
        report["mathbrain_nonfinite_load"] = dict(nan_first_weight=math.isnan(loaded.mlp_scorer.parameters()[0].data),
                                                   observations=loaded.observations)

        path = tmp / "rae.json"
        path.write_text(json.dumps(dict(observations=123, weights=[2.125, -1.5])))
        loaded = rae(path)
        with contextlib.redirect_stdout(io.StringIO()):
            loaded.load_state()
        report["rae_short_load"] = dict(first_four=[p.data for p in loaded.selector.parameters()][:4],
                                         observations=loaded.observations)
        path.write_text(json.dumps(dict(observations=321, weights=["bad"] + [0.0] * 56)))
        loaded = rae(path)
        with contextlib.redirect_stdout(io.StringIO()):
            loaded.load_state()
        report["rae_wrong_type_load"] = dict(first_weight=loaded.selector.parameters()[0].data,
                                              observations=loaded.observations)
        path.write_text(json.dumps(dict(observations=987, weights=None)))
        loaded = rae(path)
        with contextlib.redirect_stdout(io.StringIO()):
            loaded.load_state()
        report["rae_failed_load_counter"] = loaded.observations
        path.write_text(json.dumps(dict(observations=0, weights=[0.0] * 70)))
        loaded = rae(path)
        with contextlib.redirect_stdout(io.StringIO()):
            loaded.load_state()
        report["rae_extra_weights"] = dict(provided=70, loaded=len(loaded.selector.parameters()),
                                            all_zero=all(p.data == 0 for p in loaded.selector.parameters()))

        observer = Harmonix(str(tmp / "cloud.db"))
        previous = Path.cwd()
        try:
            os.chdir(tmp)
            with patch("time.time", return_value=1234.567):
                observer.create_shard(dict(input="first", output="first response", dissonance=.2))
                observer.create_shard(dict(input="second", output="second response", dissonance=.8))
            rows = observer.conn.execute("SELECT filepath,dissonance FROM shards ORDER BY id").fetchall()
            payload = np.load(rows[0][0], allow_pickle=True).item()
            report["same_millisecond_shards"] = dict(sql_rows=rows, files=len(list((tmp / "shards").glob("*.npy"))),
                                                      first_record_payload_input=payload["input"],
                                                      payload_keys=list(payload))
        finally:
            os.chdir(previous)
            observer.close()
    target = ROOT / "persistence_audit.json"
    if ARGS.check:
        assert json.loads(json.dumps(report)) == json.loads(target.read_text()), "Python persistence audit changed"
        print("PASS: pinned Python persistence audit")
    else:
        target.write_text(json.dumps(report, indent=2) + "\n")
        print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
