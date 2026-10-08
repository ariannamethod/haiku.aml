#!/usr/bin/env python3
"""Replay original DualTokenizer receipts and render dependency-free AML gates.

Development only: PYTHONPATH=../reference-deps python
tests/reference/tokenizer_oracle.py ../harmonix [--record] [--emit-aml]
Runtime tests use the committed AML fixtures and require AML plus NoTorch.
"""
import argparse
import contextlib
import hashlib
import io
import json
from pathlib import Path
import subprocess
import sys
import unicodedata


def original(source, data):
    revision = subprocess.check_output(
        ["git", "-C", str(source), "rev-parse", "HEAD"], text=True).strip()
    assert revision == data["source"], (revision, data["source"])
    subprocess.run(["git", "-C", str(source), "diff", "--exit-code", "HEAD", "--",
                    "haiku/tokenizer.py", "haiku/models/haiku_sp.model"], check=True)
    model = source / "haiku/models/haiku_sp.model"
    assert hashlib.sha256(model.read_bytes()).hexdigest() == data["model_sha256"]
    assert unicodedata.unidata_version == data["oracle"]["unicode"]
    sys.path.insert(0, str(source / "haiku"))
    import sentencepiece
    from tokenizer import DualTokenizer
    assert sentencepiece.__version__ == data["oracle"]["sentencepiece"]
    with contextlib.redirect_stdout(io.StringIO()):
        regex = DualTokenizer(use_sentencepiece=False)
        native = DualTokenizer(model_path=str(model))
    assert native.sp_model is not None
    results = []
    for case in data["cases"]:
        text = case["text"]
        results.append({
            "regex": regex.tokenize_dual(text),
            "sentencepiece": native.tokenize_dual(text),
            "pieces": native.sp_model.encode_as_pieces(text.lower()),
        })
    # Make the tuple/list distinction portable through the JSON receipt.
    return json.loads(json.dumps(results, ensure_ascii=False))


def emit_string(lines, name, value):
    lines.append(f'{name} = ""')
    pending = ""

    def flush():
        nonlocal pending
        if pending:
            lines.append(f"{name} = text_concat({name}, {json.dumps(pending, ensure_ascii=False)})")
            pending = ""

    for char in value:
        if ord(char) < 32 and char not in "\n\r\t":
            flush()
            lines.append(f"{name} = text_concat({name}, text_from_codepoint({ord(char)}))")
        else:
            pending += char
            if len(pending.encode()) >= 100:
                flush()
    flush()


def emit_list(lines, values):
    lines.append("expected = list_new()")
    for value in values:
        literal = json.dumps(value, ensure_ascii=False)
        if all(ord(char) >= 32 or char in "\n\r\t" for char in value) and len(literal.encode()) < 170:
            lines.append(f"list_push(expected, {literal})")
        else:
            emit_string(lines, "piece", value)
            lines.append("list_push(expected, piece)")


def emit_fixtures(data, directory):
    chunks, lines, references = [], [], 0
    for case in data["cases"]:
        body = ["# " + case["name"]]
        emit_string(body, "source", case["text"])
        expected = case["expected"]
        count = 0
        for mode in ("regex", "sentencepiece"):
            result = expected[mode]
            body.append(f'actual = haiku_tokenize(source, "{mode}", model)')
            emit_list(body, result["subwords"])
            label = json.dumps(case["name"] + " " + mode, ensure_ascii=False)
            body.append(f"haiku_tokenizer_test_list(actual, expected, {label})")
            body.append("actual = haiku_trigrams(actual)")
            triples = [token for triple in result["trigrams"] for token in triple]
            emit_list(body, triples)
            body.append(f"haiku_tokenizer_test_list(actual, expected, {label})")
            count += 2 + len(result["subwords"]) + len(triples)
        body.append("actual = tokenizer_pieces(model, text_lower(source))")
        emit_list(body, expected["pieces"])
        body.append(f"haiku_tokenizer_test_list(actual, expected, {json.dumps(case['name'] + ' native')})")
        count += 1 + len(expected["pieces"])
        if len(lines) + len(body) > 1400:
            chunks.append((lines, references))
            lines, references = [], 0
        lines.extend(body)
        references += count
    if lines:
        chunks.append((lines, references))
    for old in directory.glob("tokenizer_parity_*.aml"):
        old.unlink()
    for index, (lines, count) in enumerate(chunks, 1):
        body = ['IMPORT "tokenizer_support.aml"', f"# Tokenizer references: {count}",
                'model = tokenizer_load("../../models/haiku_sp.model")']
        body.extend(lines)
        body.append('PRINT "HAIKU_TOKENIZER_OK"')
        assert max(len(line.encode()) for line in body) < 256
        assert len(body) < 1600
        (directory / f"tokenizer_parity_{index:02}.aml").write_text("\n".join(body) + "\n")
    print(f"Rendered {len(chunks)} AML fixtures")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("--record", action="store_true")
    parser.add_argument("--emit-aml", action="store_true")
    args = parser.parse_args()
    path = Path(__file__).with_name("tokenizer_inputs.json")
    data = json.loads(path.read_text())
    measured = original(args.source.resolve(), data)
    if args.record:
        for case, result in zip(data["cases"], measured):
            case["expected"] = result
        path.write_text(json.dumps(data, ensure_ascii=False, indent=2) + "\n")
    else:
        for case, result in zip(data["cases"], measured):
            assert result == case["expected"], case["name"]
    if args.emit_aml:
        emit_fixtures(data, path.parent.parent / "fixtures")
    print(f"PASS: {len(measured)} original Python tokenizer cases, both modes")


if __name__ == "__main__":
    main()
