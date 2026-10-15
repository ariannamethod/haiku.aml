#!/usr/bin/env python3
"""Prepare/verify the fixed AML experiment; no model or vector math in Python."""
import argparse
import hashlib
import json
import math
from pathlib import Path
import re
import subprocess

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
PROTOCOL = HERE / "concept_basis_protocol.json"
PROTOCOL_SHA = "729a2d56ac782e7adcd06e44dc5c0ed7c43eef3faeb9f47803074bdca834c811"
REGISTRATION = "54aa7e5763f05100a007adb41d859c83c7eb7fcf"
METHODS = ("isolated_anchor", "normalized_seed_centroid")


def require(condition, detail):
    if not condition:
        raise SystemExit(detail)


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def preflight():
    require(sha(PROTOCOL) == PROTOCOL_SHA, "preregistered protocol hash differs")
    protocol = json.loads(PROTOCOL.read_text())
    pins = protocol["source_pins"]
    source = ROOT / pins["training_bindings_file"]
    require(sha(source) == pins["training_bindings_sha256"], "reviewed binding source differs")
    text = source.read_text()
    body = re.search(r"def haiku_contact_anchors\(\):(.*?)return haiku_words\(names\)", text, re.S).group(1)
    names = "".join(re.findall(r'"([^"]*)"', body)).split()
    require(len(names) == 88, "static anchor inventory differs")
    bindings = {word.lower(): index for index, word in enumerate(names)}
    for words, glyph in re.findall(r'haiku_contact_aliases\(bindings, "en", "([^"]*)", (\d+)\)', text):
        for word in words.split():
            require(word not in bindings or bindings[word] == int(glyph), "conflicting static alias")
            bindings[word] = int(glyph)
    require(len(bindings) == 130, "static English binding count differs")
    training = set()
    for anchor in protocol["anchors"]:
        expected = {word for word, glyph in bindings.items() if glyph == anchor["glyph"]}
        require(set(anchor["training_words"]) == expected, "training aliases differ from the complete reviewed group")
        require(len(anchor["training_words"]) == len(expected), "duplicate training alias")
        require(training.isdisjoint(expected), "training groups overlap")
        training.update(expected)
    require(len(training) == 21, "training inventory differs")
    legacy_path = ROOT / pins["legacy_receipt_file"]
    require(sha(legacy_path) == pins["legacy_receipt_sha256"], "legacy receipt hash differs")
    legacy = json.loads(legacy_path.read_text())
    legacy_words = {row["text"] for row in legacy["rows"]}
    primary = protocol["sets"]["primary_heldout"]
    regression = protocol["sets"]["regression"]
    controls = protocol["sets"]["controls"]
    require((len(primary), len(regression), len(controls)) == (24, 16, 8), "evaluation inventory differs")
    new = [row["text"] for row in primary + controls]
    require(len(set(new)) == 32 and set(new).isdisjoint(bindings) and set(new).isdisjoint(legacy_words), "new test words overlap prior evidence")
    require(all(row["expected_anchor"] is None for row in controls), "control acquired a truth label")
    prior_regression = {row["text"]: row["group"].split("_", 1)[1] for row in legacy["rows"] if row["group"].startswith("heldout_")}
    require({row["text"]: row["expected_anchor"] for row in regression} == prior_regression, "regression grouping differs")
    require(set(prior_regression).isdisjoint(bindings), "regression word is now a reviewed teacher")
    inputs = protocol["inputs"]
    require(len(inputs) == 90 and len({row["text"] for row in inputs}) == 90, "unique input inventory differs")
    for index, row in enumerate(inputs):
        require(row["index"] == index, "input order differs")
        require(row["seed_bound"] == int(row["text"] in bindings), "input membership differs")
        require(row["bound_glyph"] == bindings.get(row["text"]), "input glyph differs")
    require(len(protocol["legacy_rows"]) == 50, "legacy row count differs")
    for row, old in zip(protocol["legacy_rows"], legacy["rows"]):
        require(row["text"] == old["text"] and row["group"] == old["group"], "legacy diagnostic row differs")
        require(inputs[row["input_index"]]["text"] == row["text"], "legacy input reference differs")
    for side in ("a", "b"):
        require(sha(ROOT / pins[f"model_{side}_path"]) == pins[f"model_{side}_identity"], f"model {side} bytes differ")
    return protocol, legacy


def fingerprints():
    paths = {"haiku/" + str(p.relative_to(ROOT)): p for p in ROOT.glob("src/*.aml")}
    for name in ("concept_basis_protocol.json", "concept_basis_probe.aml", "check_concept_basis_probe.py", "word_anchor_probe.json"):
        paths["haiku/tests/research/iceberg/" + name] = HERE / name
    for name in ("haiku1.gguf", "haiku2.gguf", "manifest.json"):
        paths["haiku/weights/" + name] = ROOT / "weights" / name
    for repo, names in {
        "ariannamethod.ai": ["core/ariannamethod.c", "core/aml_notorch.c", "runner/aml-notorch", "tools/amlc", "libaml.a", "libaml_notorch.a"],
        "notorch": ["pen7.c", "pen7.h", "libnotorch.a"],
    }.items():
        for name in names:
            paths[repo + "/" + name] = ROOT.parent / repo / name
    return {name: sha(path) for name, path in sorted(paths.items())}


def prepare(work, protocol):
    work.mkdir(parents=True, exist_ok=True)
    require(not (work / "before.json").exists(), "work directory already has a receipt")
    inputs = protocol["inputs"]
    lookup = {row["text"]: row["index"] for row in inputs}
    train = [(lookup[word], i) for i, anchor in enumerate(protocol["anchors"]) for word in anchor["training_words"]]
    pins = protocol["source_pins"]
    config = {
        "protocol_sha256": PROTOCOL_SHA, "registration_commit": REGISTRATION,
        "identity_a": pins["model_a_identity"], "identity_b": pins["model_b_identity"],
        "tokenizer": pins["tokenizer_identity"],
        "bound": [row["seed_bound"] for row in inputs],
        "glyphs": [row["bound_glyph"] if row["bound_glyph"] is not None else -1 for row in inputs],
        "train_inputs": [index for index, _ in train], "train_anchors": [anchor for _, anchor in train],
        "anchor_inputs": [lookup[anchor["name"]] for anchor in protocol["anchors"]],
        "training_counts": [len(anchor["training_words"]) for anchor in protocol["anchors"]],
        "repeat_index": lookup[protocol["validation"]["repeat"]["text"]],
    }
    source = ["concept_config = record_new()", "concept_inputs = list_new()"]
    source += ["list_push(concept_inputs, " + json.dumps(row["text"]) + ")" for row in inputs]
    source += ['record_set(concept_config, "inputs", concept_inputs)']
    for key, value in config.items():
        if isinstance(value, list):
            source.append(f"concept_array = zeros({len(value)})")
            source += [f"concept_array[{index}] = {number}" for index, number in enumerate(value)]
            source.append(f"record_set(concept_config, {json.dumps(key)}, concept_array)")
        else:
            source.append(f"record_set(concept_config, {json.dumps(key)}, {json.dumps(value)})")
    source += ["IMPORT " + json.dumps(str(HERE / "concept_basis_probe.aml"))]
    require(all(len(line) <= 240 for line in source), "generated AML line exceeds its bounded format")
    (work / "run.aml").write_text("\n".join(source) + "\n")
    commits = {repo: subprocess.check_output(["git", "-C", str(path), "rev-parse", "HEAD"], text=True).strip() for repo, path in {
        "haiku": ROOT, "aml": ROOT.parent / "ariannamethod.ai", "notorch": ROOT.parent / "notorch"}.items()}
    require(all(commits[repo] == pins["commits"][repo] for repo in ("aml", "notorch")), "dependency commit differs")
    receipt = {"protocol_sha256": PROTOCOL_SHA, "registration_commit": REGISTRATION, "actual_commits": commits, "files": fingerprints(), "entrypoint_path": "run.aml", "entrypoint_sha256": sha(work / "run.aml")}
    (work / "before.json").write_text(json.dumps(receipt, indent=2) + "\n")
    (work / "prefix/lib").mkdir(parents=True)
    (work / "foreign").mkdir()
    for name, repo in (("libaml.a", "ariannamethod.ai"), ("libaml_notorch.a", "ariannamethod.ai"), ("libnotorch.a", "notorch")):
        (work / "prefix/lib" / name).symlink_to(ROOT.parent / repo / name)
    print(f"PREFLIGHT_OK {PROTOCOL_SHA}: 90 unique texts, 273 successful forwards per mode")


class Lines:
    def __init__(self, text):
        self.lines, self.index = text.splitlines(), 0

    def take(self):
        require(self.index < len(self.lines), "probe output ended early")
        value = self.lines[self.index]
        self.index += 1
        return value

    def tag(self, expected):
        require(self.take() == expected, f"expected {expected} at output line {self.index}")

    def number(self):
        value = float(self.take())
        require(math.isfinite(value), "nonfinite probe output")
        return value

    def integer(self):
        value = self.number()
        require(value.is_integer(), "nonintegral probe metadata")
        return int(value)


def scores(lines, anchors):
    norm = lines.number()
    require(norm > 0, "nonpositive hidden norm")
    result = {"hidden_norm": norm}
    for method in METHODS:
        values = [lines.number() for _ in anchors]
        require(all(-1.00001 <= value <= 1.00001 for value in values), "cosine outside its numerical range")
        winner, margin = lines.integer(), lines.number()
        ordered = sorted(range(4), key=lambda i: -values[i])
        require(winner == ordered[0], "raw argmax/tie order differs")
        require(abs(margin - (values[ordered[0]] - values[ordered[1]])) <= 1e-6, "reported margin differs")
        result[method] = {"cosines": values, "top": anchors[winner], "margin": margin}
    return result


def parse_output(path, protocol, legacy):
    lines = Lines(path.read_text())
    lines.tag("CONCEPT_BASIS_V1"); lines.tag(PROTOCOL_SHA); lines.tag(REGISTRATION)
    anchors = [anchor["name"] for anchor in protocol["anchors"]]
    pins = protocol["source_pins"]
    repeat_index = next(row["index"] for row in protocol["inputs"] if row["text"] == "stream")
    spaces = []
    for spec in protocol["spaces"]:
        lines.tag("SPACE"); lines.tag(spec["id"])
        require((lines.integer(), lines.number(), lines.integer()) == (spec["source"], spec["alpha"], spec["availability"]), "provider space metadata differs")
        lines.tag(pins["model_a_identity"] if spec["a_ready"] else "")
        lines.tag(pins["model_b_identity"] if spec["b_ready"] else "")
        lines.tag(pins["tokenizer_identity"])
        lines.tag("EXACT_REPEAT")
        require((lines.integer(), lines.integer()) == (repeat_index, 448), "full hidden repeat receipt differs")
        centers = []
        for index, anchor in enumerate(protocol["anchors"]):
            lines.tag("CENTROID")
            require((lines.integer(), lines.integer()) == (index, len(anchor["training_words"])), "centroid teacher count differs")
            total_norm, unit_norm = lines.number(), lines.number()
            require(total_norm > 0 and abs(unit_norm - 1) <= 1e-5, "centroid normalization differs")
            centers.append({"anchor": anchor["name"], "unit_sum_norm": total_norm, "centroid_norm": unit_norm})
        rows = []
        for item in protocol["inputs"]:
            lines.tag("ROW")
            require(lines.integer() == item["index"], "row order differs")
            lines.tag(item["text"])
            count = lines.integer()
            require(1 <= count <= 32, "token length differs")
            tokens = [lines.integer() for _ in range(count)]
            require(all(0 <= token < 2048 for token in tokens), "token outside frozen vocabulary")
            glyph = item["bound_glyph"] if item["bound_glyph"] is not None else -1
            require((lines.integer(), lines.integer()) == (item["seed_bound"], glyph), "runtime seed membership differs")
            rows.append({"input_index": item["index"], "text": item["text"], "tokens": tokens, **scores(lines, anchors)})
        lines.tag("REPEAT_ROW")
        require(lines.integer() == repeat_index, "repeat score input differs")
        repeated = scores(lines, anchors)
        require(repeated == {key: rows[repeat_index][key] for key in repeated}, "separate repeated hidden produced different scores")
        lines.tag("SPACE_CALLS")
        require(lines.integer() == 91, "provider successful-forward count differs")
        spaces.append({**spec, "successful_forwards": 91, "centroids": centers, "rows": rows, "repeat": {"input_index": repeat_index, **repeated}})
    lines.tag("TOTAL_CALLS")
    require(lines.integer() == 273, "total successful-forward count differs")
    lines.tag("QUIET_ABSENCE_AND_EXACT_REPEATS_OK")
    require(lines.index == len(lines.lines), "unexpected trailing probe output")
    require(all([row["tokens"] for row in space["rows"]] == [row["tokens"] for row in spaces[0]["rows"]] for space in spaces[1:]), "equal tokenizer identities produced different tokens")
    for mapping, old in zip(protocol["legacy_rows"], legacy["rows"]):
        row = spaces[2]["repeat"] if mapping["source_row"] == 49 else spaces[2]["rows"][mapping["input_index"]]
        baseline = row[METHODS[0]]
        require(all(abs(value - old[anchor]) <= 1e-6 for anchor, value in zip(anchors, baseline["cosines"])), "legacy mixed baseline cosine differs")
        require(baseline["top"] == old["top"], "legacy mixed baseline winner differs")
    return spaces


def agreement(items, rows, anchors):
    by_text = {row["text"]: row for row in rows}
    hits = {method: 0 for method in METHODS}
    per_class = {anchor: {method: 0 for method in METHODS} for anchor in anchors}
    paired = dict.fromkeys(("both_correct", "centroid_only_correct", "isolated_only_correct", "both_incorrect"), 0)
    cases = []
    for item in items:
        row, expected = by_text[item["text"]], item["expected_anchor"]
        correct = [row[method]["top"] == expected for method in METHODS]
        category = {(True, True): "both_correct", (False, True): "centroid_only_correct", (True, False): "isolated_only_correct", (False, False): "both_incorrect"}[tuple(correct)]
        paired[category] += 1
        for method, ok in zip(METHODS, correct):
            hits[method] += ok
            per_class[expected][method] += ok
        cases.append({"text": item["text"], "expected_anchor": expected, "outcome": category})
    return {"total": len(items), "correct": hits, "centroid_minus_isolated": hits[METHODS[1]] - hits[METHODS[0]], "per_class": per_class, "paired": paired, "cases": cases}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    prepare_args = commands.add_parser("prepare")
    prepare_args.add_argument("work", type=Path)
    check_args = commands.add_parser("check")
    check_args.add_argument("output", type=Path)
    check_args.add_argument("--compare", type=Path)
    check_args.add_argument("--receipt", type=Path, required=True)
    check_args.add_argument("--write", type=Path)
    args = parser.parse_args()
    protocol, legacy = preflight()
    if args.command == "prepare":
        prepare(args.work.resolve(), protocol)
        return
    before = json.loads(args.receipt.read_text())
    require(before["protocol_sha256"] == PROTOCOL_SHA and before["registration_commit"] == REGISTRATION, "preflight receipt differs")
    entrypoint = args.receipt.parent / before.get("entrypoint_path", "run.aml")
    require(sha(entrypoint) == before["entrypoint_sha256"], "generated AML entrypoint changed")
    require(before["files"] == fingerprints(), "source, runtime, protocol or model bytes changed")
    spaces = parse_output(args.output, protocol, legacy)
    outputs = [{"mode": "interpreted", "sha256": sha(args.output)}]
    if args.compare:
        require(args.output.read_bytes() == args.compare.read_bytes(), "interpreter/compiled output bytes differ")
        require(parse_output(args.compare, protocol, legacy) == spaces, "interpreter/compiled parsed data differ")
        outputs.append({"mode": "compiled", "sha256": sha(args.compare)})
    anchors = [anchor["name"] for anchor in protocol["anchors"]]
    metrics = {space["id"]: {name: agreement(protocol["sets"][name], space["rows"], anchors) for name in ("primary_heldout", "regression")} for space in spaces}
    result = {"schema": "haiku-concept-basis-result-v1", "protocol_sha256": PROTOCOL_SHA, "registration_commit": REGISTRATION, "source_pins": protocol["source_pins"], "execution": {"successful_forwards_per_mode": 273, "successful_forwards_total": 273 * len(outputs), "outputs": outputs, "before": before, "all_fingerprints_unchanged": True, "exact_hidden_repeat_per_space": 448, "quiet_absence": True, "legacy_mixed_baseline_rows_verified": 50}, "anchor_order": anchors, "inputs": protocol["inputs"], "legacy_rows": protocol["legacy_rows"], "metrics": metrics, "spaces": spaces}
    if args.write:
        args.write.write_text(json.dumps(result, indent=2) + "\n")
    print("PASS: 90 texts x3 spaces; paired raw cosines, full hidden repeats, immutable sources, all50 legacy baseline rows")
    for space in spaces:
        measure = metrics[space["id"]]["primary_heldout"]
        print(f"{space['id']}: isolated {measure['correct'][METHODS[0]]}/24; centroid {measure['correct'][METHODS[1]]}/24; delta {measure['centroid_minus_isolated']:+d}")


if __name__ == "__main__":
    main()
