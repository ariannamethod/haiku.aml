#!/usr/bin/env python3
"""Call pinned Python island bridges and freeze their ordered AML receipts."""
import argparse
import json
import math
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
from unittest.mock import patch

PIN = "abb878c52d763b73e5ad7a4d6a68a9ea7a248a39"
ROOT = Path(__file__).resolve().parents[2]


def f32(value):
    return struct.unpack("f", struct.pack("f", value))[0]


def quoted(value):
    return json.dumps(value, ensure_ascii=False)


def number(value):
    # AML owns binary32; decimal spellings round back to the same input value.
    if value == 0 and math.copysign(1, value) < 0:
        return "-0.0"
    return repr(value)


def metrics(values):
    return {key: f32(value) for key, value in values.items()}


def replay(source):
    assert subprocess.check_output(["git", "-C", str(source.parent), "rev-parse", "HEAD"], text=True).strip() == PIN
    subprocess.run(["git", "-C", str(source.parent), "diff", "--exit-code", "HEAD", "--", "haiku/phase4_bridges.py", "haiku/chat.py"], check=True)
    sys.path.insert(0, str(source))
    import phase4_bridges as p

    ids = []
    for value in [0, .05, .15, .25, .35, .45, .55, .65, .75, .85, .95, 1, -0., -.25, -.75, 13.25]:
        stored = f32(value)
        ids.append(dict(input=stored, original=value, state=p.state_id_from_metrics(stored, stored, stored), python64=p.state_id_from_metrics(value, value, value)))
    cosine_inputs = [
        ({}, {}), ({"a": 1}, {"b": 2}), ({"shared": 1, "extra": 999}, {"shared": 1}),
        ({"x": 0}, {"x": 1}), ({"x": 1}, {"x": -1}),
        ({"x": 1, "y": 2}, {"x": 2, "y": 1}),
        ({"pluie": -.5, "дождь": .25, "שלום": .75}, {"שלום": .5, "pluie": .25, "дождь": -.5}),
        ({"x": 3e38, "y": 2e38}, {"x": 2e38, "y": -3e38}),
        ({"x": 3e38, "y": 2e38}, {"x": 3e38, "y": 2e38}),
        ({"x": 1e-45, "y": 3e-45}, {"x": 3e-45, "y": 1e-45}),
        ({"x": 1e-45, "y": 3e-45}, {"x": -1e-45, "y": -3e-45}),
        ({"x": 3e38, "y": 1e-45}, {"x": 1e-45, "y": 3e38}),
    ]
    cosines = []
    for before, after in cosine_inputs:
        before, after = metrics(before), metrics(after)
        p.NUMPY_AVAILABLE = True
        numpy_result = p.cosine_similarity(before, after)
        p.NUMPY_AVAILABLE = False
        scalar_result = p.cosine_similarity(before, after)
        assert math.isclose(numpy_result, scalar_result, rel_tol=1e-12, abs_tol=1e-12)
        cosines.append(dict(before=before, after=after, result=numpy_result))
    p.NUMPY_AVAILABLE = True

    events = [
        ("S", "", 1, {"only_before": 99, "quality": .5}, {"only_after": 99, "quality": .75}, [0, 0, 0]),
        ("S", "external", 1, {"only_before": 99, "quality": .5}, {"only_after": 99, "quality": .75}, [0, 1, 0]),
        ("S", "external", 0, {"quality": .25, "shared": 1}, {"shared": .5, "quality": .75}, [1, 0, 1]),
        ("S", "external", 2, {}, {}, [0, 0, 0]),
        ("b:c", "a", 3, {"x": 1}, {"x": -1}, [0, 0, 0]),
        ("c", "a:b", 4, {"x": 1}, {"x": 1, "quality": .875}, [0, 1, 0]),
        ("", "S", 5, {"quality": .25}, {}, [0, 0, 0]),
        ("pluie / дождь / שלום", "", 6, {"": .25, "quality": .5}, {"quality": .5, "": .75}, [0, 0, 0]),
    ]

    def record(bridge, row, index):
        current, previous, clock, before, after, flags = row
        before, after = metrics(before), metrics(after)
        with patch("phase4_bridges.time.time", return_value=clock):
            bridge.record_state(current, before, after, prev_state_id=previous,
                                turn_id=f"turn_{index + 1}", overwhelm=bool(flags[0]), boredom=bool(flags[1]), stuck=bool(flags[2]))
        return dict(current=current, previous=previous, clock=clock, before=before, after=after,
                    turn_id=f"turn_{index + 1}", flags=flags)

    def frozen(bridge):
        logs = [dict(row) for row in bridge.conn.execute("SELECT * FROM haiku_state_log ORDER BY id")]
        for row in logs:
            row["metrics_before"] = json.loads(row["metrics_before"])
            row["metrics_after"] = json.loads(row["metrics_after"])
        states = [dict(row) for row in bridge.conn.execute("SELECT * FROM haiku_state_snapshot ORDER BY rowid")]
        for row in states:
            row["last_metrics"] = json.loads(row["last_metrics"])
        edges = [dict(row) for row in bridge.conn.execute("SELECT * FROM haiku_transitions ORDER BY rowid")]
        return dict(logs=logs, states=states, edges=edges)

    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory) / "history.db"
        bridge = p.HaikuBridges(str(path))
        history_events = [record(bridge, row, index) for index, row in enumerate(events)]
        history = frozen(bridge)
        histories = []
        for state, limit in [("S", 10), ("S", 2), ("S", 0), ("S", -1), ("missing", 10)]:
            histories.append(dict(state=state, limit=limit, turns=[entry["turn_id"] for entry in bridge.get_state_history(state, limit)]))
        bridge.close()
        bridge = p.HaikuBridges(str(path))
        assert frozen(bridge) == history
        bridge.close()

        bridge = p.HaikuBridges(str(Path(directory) / "suggestions.db"))
        suggestion_events = []
        # Last row has the highest score and stays outside count-shortlist N=2.
        for name, count, delta, flags in [("z", 5, 0, [0, 0, 0]), ("b", 4, 0, [1, 0, 0]),
                                         ("y", 3, 0, [0, 1, 0]), ("a", 3, 0, [0, 0, 0]),
                                         ("x", 2, .25, [0, 0, 0]), ("c", 1, .5, [0, 0, 0])]:
            for occurrence in range(count):
                row = (name, "source", len(suggestion_events), {"quality": .5}, {"quality": .5 + delta}, flags)
                suggestion_events.append(record(bridge, row, len(suggestion_events)))
        suggestions = []
        for current, minimum, maximum in [("source", 1, 2), ("source", 1, 5), ("source", 2, 3),
                                           ("source", 4, 5), ("source", 1, 0), ("source", 1, -1),
                                           ("source", 1, -30), ("missing", 1, 5)]:
            flows = bridge.suggest_next_states(current, minimum, maximum)
            suggestions.append(dict(current=current, minimum=minimum, maximum=maximum,
                                    flows=[dict(from_state=t.from_state, to_state=t.to_state,
                                                statistics=[t.count, t.avg_similarity, t.avg_quality_delta,
                                                            t.overwhelm_rate, t.boredom_rate, t.stuck_rate, t.score]) for t in flows]))
        suggestion_state = frozen(bridge)
        bridge.close()
    return dict(source=PIN, ids=ids, cosines=cosines, history_events=history_events, history=history,
                histories=histories, suggestion_events=suggestion_events, suggestion_state=suggestion_state,
                suggestions=suggestions)


def emit(receipt):
    def header():
        return ['# Generated by tests/reference/phase4_oracle.py; direct pinned Python calls.',
                'IMPORT "../../src/phase4.aml"',
                'def phase4_close(actual, expected):',
                '    assert(abs(actual - expected) <= 0.00001, "Python Phase4 numerical receipt differs")',
                '    return 1']

    def put_map(lines, name, values):
        lines.append(f'{name} = map_new()')
        for key, value in values.items():
            lines.append(f'map_set({name}, {quoted(key)}, {number(value)})')

    def map_check(lines, expression, values):
        lines += [f'actual = {expression}', f'assert(map_len(actual) == {len(values)}, "Python Phase4 metric count differs")']
        for key, value in values.items():
            lines.append(f'assert(map_get(actual, {quoted(key)}) == {number(value)}, "Python Phase4 metric differs")')

    def events(lines, values):
        lines += ['state = record_new()', 'haiku_phase4_init(state)']
        for event in values:
            put_map(lines, 'before', event['before'])
            put_map(lines, 'after', event['after'])
            lines.append('flags = [' + ', '.join(map(str, event['flags'])) + ']')
            lines.append(f'current = {quoted(event["current"])}')
            lines.append(f'previous = {quoted(event["previous"])}')
            lines.append(f'haiku_phase4_record(state, current, before, after, previous, {quoted(event["turn_id"])}, {event["clock"]}, flags)')
        lines.append(f'assert(haiku_phase4_check(state) == {len(values)}, "Python Phase4 log count differs")')

    def check_tables(lines, tables):
        for i, row in enumerate(tables['logs']):
            lines.append(f'entry = haiku_phase4_entry(state, {i})')
            for field in ['state_id', 'prev_state_id', 'turn_id']:
                lines.append(f'assert(text_equal(record_get(entry, "{field}"), {quoted(row[field])}), "Python Phase4 log text differs")')
            for field, source in [('clock', 'ts'), ('overwhelm', 'overwhelm'), ('boredom', 'boredom'), ('stuck', 'stuck')]:
                lines.append(f'assert(record_get(entry, "{field}") == {row[source]}, "Python Phase4 log value differs")')
            map_check(lines, 'record_get(entry, "metrics_before")', row['metrics_before'])
            map_check(lines, 'record_get(entry, "metrics_after")', row['metrics_after'])
        lines.append(f'assert(list_len(record_get(state, "phase4_states")) == {len(tables["states"])}, "Python Phase4 snapshot count differs")')
        for row in tables['states']:
            lines.append(f'snapshot = haiku_phase4_snapshot(state, {quoted(row["state_id"])})')
            lines.append(f'assert(record_get(snapshot, "activations_total") == {row["activations_total"]}, "Python Phase4 activation count differs")')
            lines.append(f'assert(record_get(snapshot, "last_updated") == {row["last_updated"]}, "Python Phase4 snapshot clock differs")')
            map_check(lines, 'record_get(snapshot, "last_metrics")', row['last_metrics'])
        lines.append(f'assert(list_len(record_get(state, "phase4_edges")) == {len(tables["edges"]) * 2}, "Python Phase4 edge count differs")')
        lines.append('totals = record_get(state, "phase4_totals")')
        for i, row in enumerate(tables['edges']):
            for j, key in enumerate(['from_state_id', 'to_state_id']):
                lines.append(f'assert(text_equal(list_get(record_get(state, "phase4_edges"), {i * 2 + j}), {quoted(row[key])}), "Python Phase4 edge order differs")')
            for j, key in enumerate(['count', 'sum_similarity', 'sum_quality_delta', 'sum_overwhelm', 'sum_boredom', 'sum_stuck']):
                lines.append(f'phase4_close(totals[{i * 6 + j}], {number(row[key])})')

    numerical = header()
    for row in receipt['ids']:
        numerical.append(f'x = {number(row["input"])}')
        numerical.append(f'assert(text_equal(haiku_bridge_id(x, x, x), {quoted(row["state"])}), "Python binary32 state ID differs")')
    for row in receipt['cosines']:
        put_map(numerical, 'before', row['before'])
        put_map(numerical, 'after', row['after'])
        numerical.append(f'phase4_close(haiku_bridge_cosine(before, after), {number(row["result"])})')
    numerical.append('PRINT "HAIKU_PHASE4_OK"')

    history = header()
    events(history, receipt['history_events'])
    check_tables(history, receipt['history'])
    for row in receipt['histories']:
        history.append(f'result = haiku_phase4_history(state, {quoted(row["state"])}, {row["limit"]})')
        history.append(f'assert(record_get(result, "count") == {len(row["turns"])}, "Python Phase4 history size differs")')
        history.append('indices = record_get(result, "indices")')
        for i, turn in enumerate(row['turns']):
            history.append(f'entry = haiku_phase4_entry(state, indices[{i}])')
            history.append(f'assert(text_equal(record_get(entry, "turn_id"), {quoted(turn)}), "Python Phase4 history order differs")')
    history += ['assert(list_len(record_keys(haiku_phase4_snapshot(state, "missing"))) == 0, "Missing snapshot was created")',
                'PRINT "HAIKU_PHASE4_OK"']

    suggestions = header()
    events(suggestions, receipt['suggestion_events'])
    check_tables(suggestions, receipt['suggestion_state'])
    for row in receipt['suggestions']:
        suggestions.append(f'result = haiku_phase4_suggest(state, {quoted(row["current"])}, {row["minimum"]}, {row["maximum"]})')
        suggestions.append(f'assert(list_len(record_get(result, "to")) == {len(row["flows"])}, "Python Phase4 suggestion count differs")')
        suggestions.append('statistics = record_get(result, "statistics")')
        for i, flow in enumerate(row['flows']):
            for field, key in [('from', 'from_state'), ('to', 'to_state')]:
                suggestions.append(f'assert(text_equal(list_get(record_get(result, "{field}"), {i}), {quoted(flow[key])}), "Python Phase4 suggestion order differs")')
            for j, value in enumerate(flow['statistics']):
                suggestions.append(f'phase4_close(statistics[{i * 7 + j}], {number(value)})')
    suggestions.append('PRINT "HAIKU_PHASE4_OK"')
    return {'phase4_numerical.aml': numerical, 'phase4_history.aml': history, 'phase4_suggestions.aml': suggestions}


def same(actual, expected):
    if isinstance(expected, float):
        assert math.isclose(actual, expected, rel_tol=1e-12, abs_tol=1e-12), (actual, expected)
    elif isinstance(expected, dict):
        assert actual.keys() == expected.keys()
        for key in expected:
            same(actual[key], expected[key])
    elif isinstance(expected, list):
        assert len(actual) == len(expected)
        for left, right in zip(actual, expected):
            same(left, right)
    else:
        assert actual == expected, (actual, expected)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--python-haiku', type=Path, required=True)
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    receipt = replay(args.python_haiku.resolve())
    path = ROOT / 'tests/reference/phase4.json'
    if args.check:
        expected = json.loads(path.read_text())
        same(receipt, expected)
        # One-ulp set-iteration differences in Python stay within the receipt tolerance.
        generated = emit(expected)
    else:
        path.write_text(json.dumps(receipt, indent=2, ensure_ascii=False) + '\n')
        generated = emit(receipt)
    for name, lines in generated.items():
        assert all(len(line.encode()) < 256 for line in lines), name
        text = '\n'.join(lines) + '\n'
        target = ROOT / 'tests/fixtures' / name
        if args.check:
            assert target.read_text() == text, name
        else:
            target.write_text(text)
    print('PASS: pinned Python Phase4 IDs, 12 cosine cases, full event/snapshot/aggregate history, 8 suggestion paths')


if __name__ == '__main__':
    main()
