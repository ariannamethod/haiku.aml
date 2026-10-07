#!/usr/bin/env python3
"""Replay committed RAE receipts using the pinned Python organism itself.

Development only: PYTHONPATH=../reference-deps python
tests/reference/rae_oracle.py ../harmonix
Ordinary AML checks consume the fixtures and need neither Python nor its deps.
"""
import argparse
from collections import defaultdict
import json
import math
from pathlib import Path
import subprocess
import sys
import tempfile


def close(actual, expected, path="receipt"):
    if isinstance(expected, float):
        assert math.isclose(actual, expected, rel_tol=1e-12, abs_tol=1e-12), (path, actual, expected)
    elif isinstance(expected, dict):
        assert actual.keys() == expected.keys(), (path, actual.keys(), expected.keys())
        for key in expected:
            close(actual[key], expected[key], path + "." + key)
    elif isinstance(expected, list):
        assert len(actual) == len(expected), (path, len(actual), len(expected))
        for index, (left, right) in enumerate(zip(actual, expected)):
            close(left, right, f"{path}[{index}]")
    else:
        assert actual == expected, (path, actual, expected)


def original(source, data):
    revision = subprocess.check_output(["git", "-C", str(source), "rev-parse", "HEAD"], text=True).strip()
    assert revision == data["source"], (revision, data["source"])
    subprocess.run(["git", "-C", str(source), "diff", "--exit-code", "HEAD", "--",
                    "haiku/haiku.py", "haiku/rae.py", "haiku/rae_recursive.py"], check=True)
    sys.path.insert(0, str(source / "haiku"))
    from haiku import HaikuGenerator, MLP, Value
    from rae import RecursiveAdapterEngine
    from rae_recursive import RecursiveRAESelector

    def selector(params, steps=3):
        result = RecursiveRAESelector.__new__(RecursiveRAESelector)
        result.selector = MLP(5, [8, 1])
        result.feature_dim, result.hidden_dim = 5, 8
        result.refinement_steps = steps
        result.observations, result.learning_rate = 0, 0.01
        for param, value in zip(result.selector.parameters(), params):
            param.data = value
        return result

    result = {"features": [], "selections": [], "learning": [], "rules": []}
    bare = RecursiveRAESelector.__new__(RecursiveRAESelector)
    for case in data["features"]:
        result["features"].append(bare.extract_features(case["text"], {"user_trigrams": case["user"]}))

    for case in data["selections"]:
        model = selector(case["params"], case["steps"])
        captured = []
        count = len(case["candidates"])

        # Observe the source method's own local score lists after each complete
        # pass. Keep the list objects alive so object ids cannot be recycled.
        def trace(frame, event, argument):
            if frame.f_code is RecursiveRAESelector.select_recursive.__code__:
                scores = frame.f_locals.get("scores")
                if scores is not None and len(scores) == count:
                    if all(scores is not previous for previous in captured):
                        captured.append(scores)
            return trace

        before = sys.gettrace()
        try:
            sys.settrace(trace)
            selected, confidence = model.select_recursive(case["candidates"], {"user_trigrams": case["user"]})
        finally:
            sys.settrace(before)
        assert len(captured) == case["steps"] + 1
        result["selections"].append({"trace": [value for row in captured for value in row],
                                      "index": case["candidates"].index(selected), "confidence": confidence})

    with tempfile.TemporaryDirectory() as directory:
        for number, case in enumerate(data["learning"]):
            model = selector(case["params"])
            model.state_path = str(Path(directory) / f"rae-{number}.json")
            model.learning_rate = case.get("rate", 0.01)
            model.observations = case.get("observations", 0)
            events = []
            for event in case["events"]:
                context = {"user_trigrams": event["user"]}
                features = model.extract_features(event["selected"], context)
                raw = model.selector([Value(value) for value in features]).data
                # The candidate list is deliberately unrelated to selected.
                loss = model.observe(["another candidate"], event["selected"], event["quality"], context)
                params = model.selector.parameters()
                events.append({"features": features, "gradient": [raw, loss] + [p.grad for p in params],
                               "params": [p.data for p in params], "observations": model.observations})
            saved = None
            if Path(model.state_path).exists():
                saved = json.loads(Path(model.state_path).read_text())
            result["learning"].append({"events": events, "saved_observations": None if saved is None else saved["observations"]})

    engine = RecursiveAdapterEngine(use_recursive=False)
    for case in data["rules"]:
        scorer = None
        if case["scored"]:
            scorer = HaikuGenerator.__new__(HaikuGenerator)
            scorer.markov_chain = defaultdict(lambda: defaultdict(int))
            for first, second, third, count in case["edges"]:
                scorer.markov_chain[first, second][third] = count
            scorer.mlp_scorer = MLP(5, [8, 1])
            for param, value in zip(scorer.mlp_scorer.parameters(), case["params"]):
                param.data = value
        selected = engine.reason({"user_trigrams": case["user"]}, case["candidates"], scorer)
        scores = None
        if scorer is not None:
            valid = engine._filter_by_structure(case["candidates"]) or case["candidates"]
            scores = [scorer.score_haiku(candidate, case["user"]) for candidate in valid]
        result["rules"].append({"selected": selected, "scores": scores})
    return result


def lower_boundaries():
    # Python's complete whitespace set, including all its control separators.
    spaces = [chr(cp) for cp in range(0x110000) if chr(cp).isspace()]
    left = ["Α", "a", "'", "\u0301", "a\u0301", "\u0345", "Σ", "ǅ", "1", ""]
    right = ["", "Α", "a", "\u0301", "'", "\u0345", "Σ", "1"]
    count = 0
    for space in spaces:
        for first in left:
            for last in right:
                for source in [first + "Σ" + space + last, first + space + "Σ" + last]:
                    assert source.lower().split() == [word.lower() for word in source.split()], repr(source)
                    count += 1
    return count


def emit_fixtures(data, directory):
    """Render the measured JSON into dependency-free AML assertions."""
    def number(value):
        return format(value, ".12g")

    def vector(lines, name, values):
        lines.append(f"{name} = zeros({len(values)})")
        lines.extend(f"{name}[{index}] = {number(value)}" for index, value in enumerate(values))

    def string(lines, name, value):
        lines.append(f'{name} = ""')
        pending = ""

        def flush():
            nonlocal pending
            if pending:
                lines.append(f"{name} = text_concat({name}, {json.dumps(pending, ensure_ascii=False)})")
                pending = ""

        for character in value:
            if ord(character) < 32 and character not in "\n\r\t":
                flush()
                lines.append(f"{name} = text_concat({name}, text_from_codepoint({ord(character)}))")
            else:
                pending += character
                if len(pending.encode()) >= 120:
                    flush()
        flush()

    def strings(lines, name, values):
        lines.append(f"{name} = list_new()")
        for value in values:
            string(lines, "piece", value)
            lines.append(f"list_push({name}, piece)")

    def context(lines, values):
        strings(lines, "user", [word for triple in values for word in triple])

    def save(name, lines, count):
        body = ['IMPORT "rae_support.aml"', f"# RAE references: {count}"] + lines + ['PRINT "HAIKU_RAE_OK"']
        assert max(len(line.encode()) for line in body) < 256
        assert len(body) < 1000
        (directory / name).write_text("\n".join(body) + "\n")

    lines = []
    for case, expected in zip(data["features"], data["expected"]["features"]):
        lines.append("# " + case["name"])
        string(lines, "source", case["text"])
        context(lines, case["user"])
        vector(lines, "expected", expected)
        lines += ["actual = haiku_rae_features(source, user)", "haiku_rae_test_vector(actual, expected)"]
    save("rae_features.aml", lines, 5 * len(data["features"]))

    for number_case, (case, expected) in enumerate(zip(data["selections"], data["expected"]["selections"]), 1):
        lines = ["# " + case["name"]]
        vector(lines, "params", case["params"])
        strings(lines, "candidates", case["candidates"])
        context(lines, case["user"])
        vector(lines, "expected", expected["trace"])
        lines += [f"actual = haiku_rae_trace(params, candidates, user, {case['steps']})",
                  "haiku_rae_test_vector(actual, expected)",
                  f"selected = haiku_rae_select(params, candidates, user, {case['steps']})",
                  f"assert(selected[0] == {expected['index']}, \"RAE winner differs\")",
                  f"haiku_rae_test_number(selected[1], {number(expected['confidence'])})",
                  f"spoken = haiku_rae_reason(params, candidates, user, {case['steps']})",
                  f"assert(text_equal(spoken, list_get(candidates, {expected['index']})), \"RAE selected text differs\")"]
        save(f"rae_selection_{number_case:02d}.aml", lines, len(expected["trace"]) + 3)

    for number_case, (case, expected) in enumerate(zip(data["learning"], data["expected"]["learning"]), 1):
        lines = ["# " + case["name"]]
        vector(lines, "params", case["params"])
        lines += ["state = haiku_rae_state()",
                  f"map_set(state, \"learning_rate\", {number(case.get('rate', .01))})",
                  f"map_set(state, \"observations\", {case.get('observations', 0)})"]
        for event, receipt in zip(case["events"], expected["events"]):
            string(lines, "source", event["selected"])
            context(lines, event["user"])
            vector(lines, "expected", receipt["features"])
            lines += ["features = haiku_rae_features(source, user)", "haiku_rae_test_vector(features, expected)"]
            vector(lines, "expected", receipt["gradient"])
            lines += [f"gradient = haiku_learner_gradient(params, features, 5, 8, {number(event['quality'])})",
                      "haiku_rae_test_vector(gradient, expected)",
                      f"loss = haiku_rae_observe(params, state, source, {number(event['quality'])}, user)",
                      f"haiku_rae_test_number(loss, {number(receipt['gradient'][1])})"]
            vector(lines, "expected", receipt["params"])
            lines += ["haiku_rae_test_vector(params, expected)",
                      f"assert(map_get(state, \"observations\") == {receipt['observations']}, \"RAE observation count differs\")",
                      f"assert(map_get(state, \"learning_rate\") == {number(case.get('rate', .01))}, \"RAE learning rate changed\")"]
        save(f"rae_learning_{number_case:02d}.aml", lines, len(case["events"]) * 124)

    for number_case, (case, expected) in enumerate(zip(data["rules"], data["expected"]["rules"]), 1):
        lines = ["# " + case["name"]]
        vector(lines, "params", case["params"])
        strings(lines, "candidates", case["candidates"])
        context(lines, case["user"])
        lines += ["rows = list_new()", "counts = map_new()"]
        for first, second, third, count in case["edges"]:
            strings(lines, "edge", [first, second, third])
            lines += ["list_push(rows, list_get(edge, 0))", "list_push(rows, list_get(edge, 1))",
                      "list_push(rows, list_get(edge, 2))", f"map_set(counts, list_key(edge), {number(count)})"]
        lines += [f"selected = haiku_rae_rule(candidates, params, rows, counts, user, {int(case['scored'])})"]
        string(lines, "expected", expected["selected"])
        lines.append('assert(text_equal(selected, expected), "RAE rule choice differs")')
        if expected["scores"] is not None:
            valid = [text for text in case["candidates"] if len(text.split("\n")) == 3] or case["candidates"]
            for source, score in zip(valid, expected["scores"]):
                string(lines, "source", source)
                lines += ["score = haiku_mathbrain_score(params, source, rows, counts, user)",
                          f"haiku_rae_test_number(score, {number(score)})"]
        save(f"rae_rule_{number_case:02d}.aml", lines, 1 + len(expected["scores"] or []))

    initial_index = next(i for i, case in enumerate(data["selections"]) if case["name"] == "generated")
    after_index = next(i for i, case in enumerate(data["selections"]) if case["name"] == "generated_after_observe")
    generated = data["selections"][initial_index]
    before = data["expected"]["selections"][initial_index]
    after = data["expected"]["selections"][after_index]
    event = data["learning"][0]["events"][0]
    learned = data["expected"]["learning"][0]["events"][0]
    lines = ['IMPORT "../../src/generator.aml"', 'IMPORT "../../src/english_seeds.aml"',
             "rows = list_new()", "counts = map_new()", "vocab = list_new()",
             "haiku_chain_seed(rows, counts, vocab, haiku_seed_words)",
             "syllables = haiku_syllable_cache(vocab)", "rng = rng_new(575)",
             "native = zeros(1)", "native[0] = -1",
             "candidates = haiku_generate_candidates(rows, counts, vocab, syllables, 5, 0.9, rng, native)"]
    for index, candidate in enumerate(generated["candidates"]):
        string(lines, "expected_text", candidate)
        lines += [f"assert(text_equal(list_get(candidates, {index}), expected_text), \"RAE native candidate differs\")",
                  f"assert(haiku_is_haiku(list_get(candidates, {index})), \"RAE native candidate form differs\")"]
    vector(lines, "params", generated["params"])
    context(lines, generated["user"])
    vector(lines, "expected", before["trace"])
    lines += ["trace = haiku_rae_trace(params, candidates, user, 3)", "haiku_rae_test_vector(trace, expected)",
              "selected = haiku_rae_select(params, candidates, user, 3)",
              f"assert(selected[0] == {before['index']}, \"RAE native winner differs\")",
              "source = list_get(candidates, selected[0])", "state = haiku_rae_state()",
              f"loss = haiku_rae_observe(params, state, source, {number(event['quality'])}, user)",
              f"haiku_rae_test_number(loss, {number(learned['gradient'][1])})"]
    vector(lines, "expected", learned["params"])
    lines += ["haiku_rae_test_vector(params, expected)",
              'assert(map_get(state, "observations") == 1, "RAE native observation differs")']
    vector(lines, "expected", after["trace"])
    lines += ["trace = haiku_rae_trace(params, candidates, user, 3)", "haiku_rae_test_vector(trace, expected)",
              "selected = haiku_rae_select(params, candidates, user, 3)",
              f"assert(selected[0] == {after['index']}, \"RAE next native winner differs\")"]
    save("rae_generated.aml", lines, 10 + len(before["trace"]) + 1 + 1 + 57 + 1 + len(after["trace"]) + 1)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("--record", action="store_true", help="refresh original-Python measurements in the JSON")
    parser.add_argument("--emit-aml", action="store_true", help="render the checked receipts as AML fixtures")
    args = parser.parse_args()
    path = Path(__file__).with_name("rae_inputs.json")
    data = json.loads(path.read_text())
    actual = original(args.source.resolve(), data)
    if args.record:
        data["expected"] = actual
        path.write_text(json.dumps(data, ensure_ascii=False, indent=2) + "\n")
    else:
        close(actual, data["expected"])
    if args.emit_aml:
        emit_fixtures(data, path.parent.parent / "fixtures")
    boundaries = lower_boundaries()
    events = sum(len(case["events"]) for case in data["learning"])
    print(f"PASS: {len(data['features'])} RAE feature vectors, {len(data['selections'])} score traces, "
          f"{events} complete gradient/update events, {len(data['rules'])} rule selections")
    print(f"PASS: {boundaries} whole-text/per-word Unicode lower boundary comparisons")


if __name__ == "__main__":
    main()
