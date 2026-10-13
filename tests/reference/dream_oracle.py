#!/usr/bin/env python3
"""Call the pinned Python dream organs and emit dependency-free AML receipts."""
import argparse
from collections import defaultdict
from contextlib import redirect_stdout
import copy
import io
import json
import math
from pathlib import Path
import sqlite3
import subprocess
import sys
import tempfile
from unittest.mock import patch

PIN = "abb878c52d763b73e5ad7a4d6a68a9ea7a248a39"
ROOT = Path(__file__).resolve().parents[2]
VOCAB = "rain cloud wind light moon warm dream night".split()
EDGES = [VOCAB[i:i + 3] + [1] for i in range(len(VOCAB) - 2)]
EDGES.append(["rain", "cloud", "light", 3])


class OrderedWords:
    def __init__(self, words):
        self.words = dict.fromkeys(words)
    def __iter__(self):
        return iter(self.words)
    def __len__(self):
        return len(self.words)
    def add(self, word):
        self.words[word] = None


def replay(source):
    assert subprocess.check_output(["git", "-C", str(source.parent), "rev-parse", "HEAD"], text=True).strip() == PIN
    subprocess.run(["git", "-C", str(source.parent), "diff", "--exit-code", "HEAD", "--",
                    "haiku/dream_haiku.py", "haiku/chat.py", "haiku/haiku.py", "haiku/tokenizer.py"], check=True)
    sys.path.insert(0, str(source))
    import dream_haiku as dream
    from haiku import HaikuGenerator
    from tokenizer import DualTokenizer
    import numpy as np

    def rows(path, sql):
        with sqlite3.connect(path) as db:
            return [list(row) for row in db.execute(sql)]

    with tempfile.TemporaryDirectory() as directory:
        root = Path(directory)
        bootstrap = []
        for index, seeds in enumerate([
            [], [" alpha\n \t\nbeta \n\n gamma ", "alpha\ndelta"],
            [f"seed {i}\n\u2007ligne {i}\u00a0" for i in range(12)],
        ]):
            path = str(root / f"bootstrap-{index}.db")
            dream.init_dream(path, seeds)
            first = rows(path, "SELECT text,weight FROM dream_fragments ORDER BY id")
            dream.init_dream(path, ["ignored initialization"])
            assert first == rows(path, "SELECT text,weight FROM dream_fragments ORDER BY id")
            bootstrap.append(dict(seeds=seeds, fragments=first))

        path = str(root / "triggers.db")
        dream.init_dream(path)
        triggers = []
        cases = [
            ("cooldown", 9, .1, 1, .5, 0, .25),
            ("quality_equality", 10, .45, .7, .4, 0, .25),
            ("quality_below", 10, .449, .7, .4, .25, .25),
            ("novelty_equality", 10, .5, .7, .7, 0, .25),
            ("novelty_above", 10, .5, .701, .7, .125, .25),
            ("probability_above", 10, .5, .701, .7, .251, .25),
            ("dissonance_inside", 10, .5, .7, .5, .25, .25),
            ("dissonance_lower", 10, .5, .7, .4, 0, .25),
            ("dissonance_upper", 10, .5, .7, .7, 0, .25),
            ("zero_probability_equal", 10, .1, 0, 0, 0, 0),
            ("zero_probability_above", 10, .1, 0, 0, .0625, 0),
            ("one_probability", 10, .1, 0, 0, .9375, 1),
        ]
        for name, turn, quality, novelty, dissonance, draw, probability in cases:
            consumed = []
            context = dream.HaikuDreamContext("a\nb\nc", dissonance, 0, novelty, 0, quality, 3, turn)
            with patch("random.random", side_effect=lambda: consumed.append(draw) or draw):
                result = dream.should_run_dream(context, path, dream.DreamConfig(trigger_probability=probability))
            triggers.append(dict(name=name, turn=turn, quality=quality, novelty=novelty,
                                 dissonance=dissonance, draw=draw, probability=probability,
                                 accepted=int(result), draws=len(consumed)))

        fragment_cases = []
        for name, initial, updates in [
            ("unicode_lines", [], ["\u2007pluie\u00a0\n\n облако \nשלום\t\n", "rain\u2028cloud\n \r\n", "rain\nrain\nrain"]),
            ("boundaries", [["exact tenth", .1], ["just above", .1001], ["fresh", 1]], ["\t\n \n\u2007\n", "new\nlines\narrive"]),
            ("decay", [["old fragment", 1]], ["same\nsame\nsame"] * 46),
            ("blank_updates", [["rain", 1]], ["", "\n" * 10000]),
        ]:
            path = str(root / f"fragments-{name}.db")
            dream.init_dream(path)
            with sqlite3.connect(path) as db:
                db.executemany("INSERT INTO dream_fragments(text,weight) VALUES(?,?)", initial)
            snapshots = []
            for step, update in enumerate(updates):
                dream.update_dream_fragments(update, path)
                if name != "decay" or step in (0, 42, 43, 44, 45):
                    snapshots.append(dict(step=step + 1, fragments=rows(path, "SELECT text,weight FROM dream_fragments ORDER BY id")))
            fragment_cases.append(dict(name=name, initial=initial, updates=updates, snapshots=snapshots))

        def generator():
            result = HaikuGenerator.__new__(HaikuGenerator)
            result.vocab = OrderedWords(VOCAB)
            result.markov_chain = defaultdict(lambda: defaultdict(int))
            for a, b, c, count in EDGES:
                result.markov_chain[a, b][c] = count
            result.recent_trigrams = []
            result.db_conn = None
            assert [result._count_syllables(word) for word in result.vocab] == [1] * len(VOCAB)
            return result

        def trace(exchanges, seeds):
            path = str(root / f"dialog-{exchanges}-{len(seeds)}.db")
            dream.init_dream(path, seeds)
            model = generator()
            events = []
            calls = []
            original = model.generate_candidates
            def uniform(words):
                draw = ((len(events) * 5 + 3) % 16) / 16
                index = math.floor(draw * len(words))
                events.append(dict(kind="uniform", draw=draw, index=index))
                return list(words)[index]
            def weighted(words, p):
                draw = ((len(events) * 5 + 3) % 16) / 16
                cdf = np.cumsum(p)
                cdf /= cdf[-1]
                index = int(np.searchsorted(cdf, draw, side="right"))
                events.append(dict(kind="weighted", draw=draw, index=index))
                return words[index]
            def generate(n, temp):
                begin = len(events)
                result = original(n=n, temp=temp)
                calls.append(dict(n=n, temperature=temp, text=result[0], start=begin, end=len(events)))
                return result
            spoken = "rain holds the cloud\nour two voices move through night\nlight remembers us"
            config = dream.DreamConfig(max_exchanges=exchanges)
            with patch("random.choice", uniform), patch("numpy.random.choice", weighted), \
                 patch.object(model, "generate_candidates", generate), patch("time.time", side_effect=[100, 101]):
                returned = dream.run_dream_dialog(model, spoken, path, config)
            assert returned == [spoken] + [call["text"] for call in calls[:-1]]
            assert len(calls) == exchanges
            assert list(model.vocab) == VOCAB
            assert model.recent_trigrams == []
            # chat.py feeds all returned haikus after the whole dialog completes.
            tokenizer = DualTokenizer(model_path=str(root / "missing.model"))
            ordered_edges = [edge[:3] for edge in EDGES]
            feedback = []
            for haiku in returned:
                dream.update_dream_fragments(haiku, path)
                tokenized = tokenizer.tokenize_dual(haiku)
                model.update_chain(tokenized["trigrams"])
                for triple in tokenized["trigrams"]:
                    if list(triple) not in ordered_edges:
                        ordered_edges.append(list(triple))
                feedback.append(dict(tokens=tokenized["subwords"], recent=copy.deepcopy(model.recent_trigrams)))
            edges = [triple + [model.markov_chain[triple[0], triple[1]][triple[2]]] for triple in ordered_edges]
            assert rows(path, "SELECT value FROM dream_meta WHERE key='last_run_turn'") == [["1"]]
            with patch("random.random", return_value=0):
                at11 = dream.should_run_dream(dream.HaikuDreamContext("", .5, 0, 0, 0, .5, 8, 11), path)
            return dict(exchanges=exchanges, bootstrap=seeds, spoken=spoken, returned=returned,
                        calls=calls, events=events, feedback=feedback, edges=edges, vocab=list(model.vocab),
                        fragments=rows(path, "SELECT text,weight FROM dream_fragments ORDER BY id"),
                        exchange_rows=rows(path, "SELECT speaker,text,quality,dissonance FROM dream_exchanges ORDER BY id"),
                        dialog_rows=rows(path, "SELECT started_at,finished_at,exchanges_count,avg_quality,note FROM dream_dialogs ORDER BY id"),
                        source_last_turn=1, source_allows_turn11=bool(at11))

        dialogs = [trace(count, []) for count in (1, 2, 4, 5, 16)]
        seeded = trace(4, ["a different friend\na different memory\na quiet seed"])
        assert seeded["returned"] == dialogs[2]["returned"]
        assert seeded["calls"] == dialogs[2]["calls"] and seeded["events"] == dialogs[2]["events"]
        with redirect_stdout(io.StringIO()):
            tokenizer = DualTokenizer(model_path=str(source / "models/haiku_sp.model"))
        repair_text = "sweet sour doubt stable\nmore least appear frequently\nto and reinforce"
        original_tokens = tokenizer.tokenize_dual(repair_text)
        fixed_tokens = tokenizer.tokenize_dual(repair_text.replace("\n", " "))
        assert "\n" in original_tokens["subwords"] and "\n" not in fixed_tokens["subwords"]
        return dict(source=PIN, vocab=VOCAB, edges=EDGES, bootstrap=bootstrap, triggers=triggers,
                    fragments=fragment_cases, dialogs=dialogs, unused_seed_probe=seeded,
                    separator_repair=dict(text=repair_text, source=original_tokens, aml=fixed_tokens))


def quoted(value):
    return json.dumps(value, ensure_ascii=False)


def text(lines, name, value):
    lines.append(f'{name} = ""')
    chunk = ""
    for cp in value:
        if len(quoted(chunk + cp).encode()) > 150:
            lines.append(f"{name} = text_concat({name}, {quoted(chunk)})")
            chunk = ""
        chunk += cp
    if chunk:
        lines.append(f"{name} = text_concat({name}, {quoted(chunk)})")


def expect_list(lines, expression, values):
    lines.append("expected = list_new()")
    for value in values:
        text(lines, "item", value)
        lines.append("list_push(expected, item)")
    lines.append(f'assert(text_equal(list_key({expression}), list_key(expected)), "Dream ordered text differs from Python")')


def expect_fragments(lines, values):
    # Decay cases contain duplicates; check positional values without building
    # a second giant JSON key, while keeping fixture programs compact.
    lines += [f'assert(list_len(record_get(state, "dream_fragments")) == {len(values)}, "Dream fragment count differs")',
              'weights = record_get(state, "dream_weights")']
    for index, (value, weight) in enumerate(values):
        text(lines, "item", value)
        lines.append(f'assert(text_equal(list_get(record_get(state, "dream_fragments"), {index}), item), "Dream fragment text differs")')
        lines.append(f'assert(abs(weights[{index}] - {weight}) < 0.000002, "Dream fragment decay differs")')


def fixtures(data):
    files = {}
    lines = ['IMPORT "dream_support.aml"', 'state = dream_test_new(1, "regex")']
    for case in data["triggers"]:
        lines += [f'# {case["name"]}', f'record_set(state, "dream_config", [10, {case["probability"]}, 4])',
                  'tape = zeros(2)', f'tape[1] = {case["draw"]}', 'record_set(state, "draws", tape)',
                  'event = map_new()', f'map_set(event, "turn", {case["turn"]})',
                  f'map_set(event, "quality_after", {case["quality"]})', f'map_set(event, "novelty", {case["novelty"]})',
                  f'map_set(event, "dissonance", {case["dissonance"]})',
                  f'assert(haiku_dream_should(state, event) == {case["accepted"]}, "Dream source trigger differs")',
                  'tape = record_get(state, "draws")', f'assert(tape[0] == {case["draws"]}, "Dream trigger draw order differs")']
    lines.append('PRINT "HAIKU_DREAM_OK"')
    files["dream_triggers.aml"] = lines
    for index, case in enumerate(data["bootstrap"]):
        lines = ['IMPORT "../../src/dream.aml"', 'state = record_new()', 'bootstrap = list_new()']
        for seed in case["seeds"]:
            text(lines, "seed", seed)
            lines.append('list_push(bootstrap, seed)')
        lines += ['haiku_dream_init(state, bootstrap)', 'haiku_dream_check(state)',
                  'haiku_dream_init(state, haiku_words("ignored"))']
        expect_fragments(lines, case["fragments"])
        lines.append('PRINT "HAIKU_DREAM_OK"')
        files[f"dream_bootstrap_{index + 1}.aml"] = lines
    for case in data["fragments"]:
        lines = ['IMPORT "../../src/dream.aml"', 'state = record_new()', 'haiku_dream_init(state, list_new())',
                 f'weights = zeros({max(1, len(case["initial"]))})']
        for index, (value, weight) in enumerate(case["initial"]):
            text(lines, "item", value)
            lines += ['list_push(record_get(state, "dream_fragments"), item)', f'weights[{index}] = {weight}']
        lines.append('record_set(state, "dream_weights", weights)')
        if case["name"] == "decay":
            previous = 0
            for snapshot in case["snapshots"]:
                lines += ['cursor = 0', f'while cursor < {snapshot["step"] - previous}:',
                          '    haiku_dream_fragments(state, "same\\nsame\\nsame")', '    cursor = cursor + 1']
                # Known identical fresh lines keep the checked fields compact.
                values = snapshot["fragments"]
                lines += [f'assert(list_len(record_get(state, "dream_fragments")) == {len(values)}, "Dream decay retention differs")',
                          'weights = record_get(state, "dream_weights")']
                expect = [weight for _, weight in values]
                for index, weight in enumerate(expect):
                    lines.append(f'assert(abs(weights[{index}] - {weight}) < 0.000002, "Dream long decay differs")')
                text(lines, "item", values[0][0])
                lines.append('assert(text_equal(list_get(record_get(state, "dream_fragments"), 0), item), "Dream old fragment pruning differs")')
                previous = snapshot["step"]
        else:
            for update, snapshot in zip(case["updates"], case["snapshots"]):
                if len(update) == 10000:
                    lines += ['source = ""', 'cursor = 0', 'while cursor < 10000:',
                              '    source = text_concat(source, "\\n")', '    cursor = cursor + 1']
                else:
                    text(lines, "source", update)
                lines.append('haiku_dream_fragments(state, source)')
                expect_fragments(lines, snapshot["fragments"])
        lines += ['haiku_dream_check(state)', 'PRINT "HAIKU_DREAM_OK"']
        files[f'dream_fragments_{case["name"]}.aml'] = lines
    for case in data["dialogs"]:
        lines = ['IMPORT "dream_support.aml"', 'state = dream_test_new(1, "regex")',
                 f'record_set(state, "dream_config", [10, 0.25, {case["exchanges"]}])', 'record_set(state, "turn", 10)']
        text(lines, "spoken", case["spoken"])
        lines.append('haikus = haiku_dream_run(state, spoken, 0)')
        expect_list(lines, "haikus", case["returned"])
        expect_list(lines, 'record_get(state, "dream_history")', case["returned"])
        lines += ['tape = record_get(state, "draws")', f'assert(tape[0] == {len(case["events"])}, "Dream final unrecorded response draw order differs")',
                  'assert(record_get(state, "dream_last_turn") == 10, "Dream cooldown must save the actual foreground turn")',
                  'dialogs = record_get(state, "dream_dialogs")',
                  f'assert(len(dialogs) == 3 and dialogs[0] == 10 and dialogs[1] == 0 and dialogs[2] == {case["exchanges"]}, "Dream dialog metadata differs")']
        expect_fragments(lines, case["fragments"])
        expect_list(lines, 'record_get(state, "vocab")', case["vocab"])
        expect_list(lines, 'record_get(state, "recent")', [word for triple in case["feedback"][-1]["recent"] for word in triple])
        for a, b, c, count in case["edges"]:
            lines.append(f'assert(haiku_chain_count(record_get(state, "counts"), {quoted(a)}, {quoted(b)}, {quoted(c)}) == {count}, "Dream feedback transition differs")')
        lines += [f'assert(map_len(record_get(state, "counts")) == {len(case["edges"])}, "Dream feedback added extra transitions")',
                  'assert(map_len(record_get(state, "weights")) == 8, "Dream changed the living cloud")',
                  'event = map_new()', 'map_set(event, "quality_after", 0.5)', 'map_set(event, "novelty", 0)',
                  'map_set(event, "dissonance", 0.5)', 'map_set(event, "turn", 19)',
                  'assert(haiku_dream_should(state, event) == 0, "Dream actual-turn cooldown ended early")',
                  'record_set(state, "dream_config", [10, 1, 4])', 'map_set(event, "turn", 20)',
                  'assert(haiku_dream_should(state, event) == 1, "Dream actual-turn cooldown failed to reopen")',
                  'haiku_dream_check(state)', 'PRINT "HAIKU_DREAM_OK"']
        files[f'dream_dialog_{case["exchanges"]:02}.aml'] = lines
    repair = data["separator_repair"]
    lines = ['IMPORT "dream_support.aml"', 'model = tokenizer_load("../../models/haiku_sp.model")',
             'state = dream_test_new(1, "sentencepiece")', 'record_set(state, "dream_config", [10, 0.25, 1])', 'record_set(state, "turn", 10)']
    text(lines, "spoken", repair["text"])
    lines.append('original = haiku_tokenize_sentencepiece(spoken, model)')
    expect_list(lines, "original", repair["source"]["subwords"])
    lines += ['haikus = haiku_dream_run(state, spoken, model)', 'assert(list_len(haikus) == 1, "Dream separator repair changed recorded exchanges")']
    expect_list(lines, 'record_get(state, "recent")', [word for triple in repair["aml"]["trigrams"][-10:] for word in triple])
    for word in dict.fromkeys(repair["aml"]["subwords"]):
        if word not in VOCAB:
            lines.append(f'assert(map_get(record_get(state, "dream_vocab"), {quoted(word)}) == 10, "Dream-only piece provenance differs")')
    lines += ['assert(list_find(record_get(state, "vocab"), "\\n") < 0, "Dream newline entered the speaking vocabulary")',
              'assert(map_len(record_get(state, "weights")) == 8, "Dream pieces changed the living cloud")',
              'haiku_gen_check(record_get(state, "rows"), record_get(state, "counts"), record_get(state, "vocab"), record_get(state, "syllables"))',
              'haiku_dream_check(state)', 'PRINT "HAIKU_DREAM_OK"']
    files["dream_separator_repair.aml"] = lines
    return files


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--python-haiku", type=Path, required=True)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    with redirect_stdout(io.StringIO()):
        data = replay(args.python_haiku.resolve())
    outputs = {ROOT / "tests/reference/dream_inputs.json": json.dumps(data, ensure_ascii=False, indent=2) + "\n"}
    for name, lines in fixtures(data).items():
        assert max(len(line.encode()) for line in lines) < 256, name
        assert len(lines) < 1500, (name, len(lines))
        outputs[ROOT / "tests/fixtures" / name] = "\n".join(lines) + "\n"
    for path, body in outputs.items():
        if args.check:
            assert path.read_text() == body, f"Stale dream receipt: {path}"
        else:
            path.write_text(body)
    print(f'PASS: {len(data["triggers"])} Python dream triggers, {len(data["dialogs"])} complete dialogs, fragments, unused seeds, cooldown and separator receipts')


if __name__ == "__main__":
    main()
