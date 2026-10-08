#!/usr/bin/env python3
"""Render pinned foreground receipts into dependency-free AML checks."""
import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DATA = json.loads((ROOT / "tests/reference/foreground_inputs.json").read_text())
CASES = [case for case in DATA["cases"] if "candidates" in case]


def quoted(value):
    return json.dumps(value, ensure_ascii=False)


def scalar(value):
    return format(value, ".17g")


def text(lines, name, value):
    lines.append(f'{name} = ""')
    for start in range(0, len(value), 110):
        lines.append(f"{name} = text_concat({name}, {quoted(value[start:start + 110])})")


def list_check(lines, expression, values):
    text(lines, "wanted_text", " ".join(values))
    lines.append(f"fg_list({expression}, wanted_text)")


def vector(lines, name, values):
    lines.append(f"{name} = zeros({len(values)})")
    lines.extend(f"{name}[{i}] = {scalar(value)}" for i, value in enumerate(values))


def near(lines, expression, expected):
    lines.append(f"fg_near({expression}, {scalar(expected)})")


def ordered_rows(before):
    rows = dict.fromkeys(tuple(DATA["seeds"][i:i + 3]) for i in range(len(DATA["seeds"]) - 2))
    if before is not None:
        for case in CASES:
            rows.update(dict.fromkeys(tuple(row) for row in case["triples"]))
            if case is before:
                break
    return list(rows)


def init(lines, before=None):
    for name in ["weights", "frequencies", "last_used", "observer_counts", "observer_resonance", "counts"]:
        lines.append(f"{name} = map_new()")
    for name in ["origins", "observer_rows", "rows", "vocab", "recent"]:
        lines.append(f"{name} = list_new()")
    text(lines, "seed_source", " ".join(DATA["seeds"]))
    lines.append("seeds = haiku_words(seed_source)")
    if before is None:
        lines += ["haiku_cloud_seed(weights, frequencies, last_used, origins, seeds, 0)",
                  "haiku_chain_seed(rows, counts, vocab, seeds)"]
    else:
        for word, weight, frequency, clock, origin in before["cloud"]:
            lines += [f"map_set(weights, {quoted(word)}, {scalar(weight)})",
                      f"map_set(frequencies, {quoted(word)}, {frequency})",
                      f"map_set(last_used, {quoted(word)}, {clock})",
                      f"list_push(origins, {quoted(origin)})"]
        for a, b, c, count, resonance in before["observer_trigrams"]:
            lines += [f"triple = haiku_words({quoted(' '.join([a,b,c]))})",
                      "haiku_store_trigrams(observer_rows, observer_counts, observer_resonance, triple)",
                      f"map_set(observer_counts, list_key(triple), {count})",
                      f"map_set(observer_resonance, list_key(triple), {resonance})"]
        counts = {(a,b,c): count for a,b,c,count in before["chain"]}
        for row in ordered_rows(before):
            lines.extend(f"list_push(rows, {quoted(word)})" for word in row)
            lines.append(f"map_set(counts, list_key(list_slice(rows, list_len(rows) - 3, list_len(rows))), {counts[row]})")
        lines.extend(f"list_push(vocab, {quoted(word)})" for word in before["vocab"])
        lines.extend(f"list_push(recent, {quoted(word)})" for row in before["recent"] for word in row)
    lines += ["syllables = haiku_syllable_cache(vocab)", "voice_rng = rng_new(575)",
              f"draws = zeros({len(DATA['draws']) + 1})", "cursor = 0",
              f"while cursor < {len(DATA['draws'])}:",
              "    residue = cursor * 5 + 3", "    residue = residue - floor(residue / 16) * 16",
              "    draws[cursor + 1] = residue / 16", "    cursor = cursor + 1",
              f"draws[0] = {0 if before is None else before['cursor_after']}",
              "mathbrain_state = haiku_mathbrain_state()", "rae_state = haiku_rae_state()",
              f"turn = {0 if before is None else before['turn']}"]
    vector(lines, "mathbrain", DATA["parameters"] if before is None else before["parameters"])
    vector(lines, "rae", DATA["parameters"])
    if before is not None:
        for key in ["observations", "running_loss"]:
            lines.append(f"map_set(mathbrain_state, {quoted(key)}, {scalar(before[key])})")
        lines.append(f'map_set(mathbrain_state, "last_loss", {scalar(before["loss"])})')


def exchange(lines, case):
    lines += [f"# Turn {case['turn']}: {case['input']}",
              f"source = haiku_input_strip({quoted(case['input'])})",
              "assert(haiku_input_action(source) == 1, \"Expected an accepted exchange\")",
              'words = haiku_tokenize(source, "regex", 0)', "user = haiku_trigrams(words)",
              "haiku_foreground_input(words, user)", "turn = turn + 1", "clock = turn",
              "haiku_cloud_morph(weights, frequencies, last_used, origins, words, clock)",
              "haiku_store_trigrams(observer_rows, observer_counts, observer_resonance, user)",
              "recent = haiku_chain_update(rows, counts, vocab, recent, user)",
              "system = list_clone(recent)", "observation = haiku_observe_trigrams(user, system)",
              "temperatures = haiku_temperatures(observation[0])",
              "haiku_foreground_cache(syllables, vocab)",
              "candidates = haiku_generate_candidates(rows, counts, vocab, syllables, 5, temperatures[0], voice_rng, draws)",
              "choice = haiku_rae_select(rae, candidates, user, 3)",
              "spoken = list_get(candidates, choice[0])",
              "quality = haiku_foreground_quality(observation)",
              "bridge_event = haiku_foreground_bridge(observation, quality, turn, clock)",
              "loss = haiku_mathbrain_observe(mathbrain, mathbrain_state, spoken, quality, rows, counts, user)"]
    near(lines, "turn", case["turn"])
    near(lines, "draws[0]", case["cursor_after"])
    list_check(lines, "words", case["tokens"])
    list_check(lines, "user", [word for row in case["triples"] for word in row])
    list_check(lines, "system", [word for row in case["recent"] for word in row])
    list_check(lines, "recent", [word for row in case["recent"] for word in row])
    for i, value in enumerate(case["observation"]):
        near(lines, f"observation[{i}]", value)
    for i, value in enumerate(case["temperatures"]):
        near(lines, f"temperatures[{i}]", value)
    near(lines, "list_len(candidates)", 5)
    for i, value in enumerate(case["candidates"]):
        text(lines, "wanted_text", value)
        lines.append(f'assert(text_equal(list_get(candidates, {i}), wanted_text), "Foreground candidate/draw trace differs")')
    near(lines, "choice[0]", case["selected"])
    near(lines, "choice[1]", case["confidence"])
    near(lines, "quality", case["quality"])
    near(lines, "loss", case["loss"])
    for key in ["observations", "running_loss"]:
        near(lines, f'map_get(mathbrain_state, "{key}")', case[key])
    near(lines, 'map_get(mathbrain_state, "last_loss")', case["loss"])
    vector(lines, "expected", case["parameters"])
    lines.append("fg_vector(mathbrain, expected)")
    vector(lines, "expected", DATA["parameters"])
    lines.append("fg_vector(rae, expected)")
    near(lines, 'map_get(rae_state, "observations")', 0)
    list_check(lines, "map_keys(weights)", [row[0] for row in case["cloud"]])
    list_check(lines, "map_keys(frequencies)", [row[0] for row in case["cloud"]])
    list_check(lines, "map_keys(last_used)", [row[0] for row in case["cloud"]])
    list_check(lines, "origins", [row[4] for row in case["cloud"]])
    for word, weight, frequency, clock, _ in case["cloud"]:
        for name, value in [("weights", weight), ("frequencies", frequency), ("last_used", clock)]:
            near(lines, f"map_get({name}, {quoted(word)})", value)
    list_check(lines, "observer_rows", [word for row in case["observer_trigrams"] for word in row[:3]])
    near(lines, "map_len(observer_counts)", len(case["observer_trigrams"]))
    near(lines, "map_len(observer_resonance)", len(case["observer_trigrams"]))
    for a,b,c,count,resonance in case["observer_trigrams"]:
        lines.append(f"key = list_key(haiku_words({quoted(' '.join([a,b,c]))}))")
        near(lines, "map_get(observer_counts, key)", count)
        near(lines, "map_get(observer_resonance, key)", resonance)
    list_check(lines, "rows", [word for row in ordered_rows(case) for word in row])
    near(lines, "map_len(counts)", len(case["chain"]))
    for a,b,c,count in case["chain"]:
        near(lines, f"haiku_chain_count(counts, {quoted(a)}, {quoted(b)}, {quoted(c)})", count)
    list_check(lines, "vocab", case["vocab"])
    near(lines, "map_len(syllables)", len(case["vocab"]))
    for word in case["vocab"]:
        lines.append(f'assert(map_get(syllables, {quoted(word)}) == haiku_syllables({quoted(word)}), "Syllable cache differs")')
    fields = dict(turn=case["turn"], clock=case["turn"], dissonance=case["observation"][0],
                  novelty=case["observation"][1], arousal=case["observation"][2], entropy=case["observation"][3],
                  quality_before=.5, quality_after=case["quality"], **case["flags"])
    near(lines, "map_len(bridge_event)", len(fields))
    for key, value in fields.items():
        near(lines, f"map_get(bridge_event, {quoted(key)})", value)


def emit(check):
    for group in range(4):
        lines = ['IMPORT "foreground_support.aml"']
        init(lines, None if group == 0 else CASES[2 * group - 1])
        for case in CASES[2 * group:2 * group + 2]:
            exchange(lines, case)
        lines.append('PRINT "HAIKU_FOREGROUND_OK"')
        assert max(len(line.encode()) for line in lines) < 256
        assert len(lines) < 1700
        body = "\n".join(lines) + "\n"
        path = ROOT / f"tests/fixtures/foreground_turns_{group + 1}.aml"
        if check:
            assert path.read_text() == body, f"Stale fixture: {path}"
        else:
            path.write_text(body)
        print(path.name, len(lines), "lines")
    lines = ['IMPORT "foreground_support.aml"']
    init(lines)
    first = CASES[0]
    lines += [f"spoken = {quoted(first['candidates'][first['selected']])}", "user = list_new()",
              "train_rae = 1", "if train_rae:",
              f"    loss = haiku_rae_observe(rae, rae_state, spoken, {first['quality']}, user)"]
    near(lines, "loss", DATA["coupled"]["loss"])
    near(lines, 'map_get(rae_state, "observations")', DATA["coupled"]["observations"])
    vector(lines, "expected", DATA["coupled"]["parameters"])
    lines += ["fg_vector(rae, expected)", 'PRINT "HAIKU_FOREGROUND_OK"']
    body = "\n".join(lines) + "\n"
    path = ROOT / "tests/fixtures/foreground_coupled.aml"
    if check:
        assert path.read_text() == body, f"Stale fixture: {path}"
    else:
        path.write_text(body)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    emit(parser.parse_args().check)
