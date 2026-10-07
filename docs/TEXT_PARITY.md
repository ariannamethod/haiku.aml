# The owl finds the edges of words

2026-10-07. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

`src/text.aml` supplies the first word and line boundaries for the organism.
Its coherence value comes from `RecursiveRAESelector.extract_features()[4]` in
[`harmonix/haiku` at `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`](https://github.com/ariannamethod/harmonix/tree/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku).

| AML function | Behavior |
|---|---|
| `haiku_space(cp)` | CPython 3.12 whitespace classification for a Unicode codepoint |
| `haiku_word_count(text)` | Number of whitespace-delimited words, matching `len(text.split())` |
| `haiku_line_count(text)` | Number of literal LF-delimited lines, matching `len(text.split('\n'))` |
| `haiku_line(text, index)` | Exact contents of one LF-delimited line; negative integer indexes count from the end; out-of-range indexes return empty text |
| `haiku_line_coherence(text)` | Original RAE fifth feature: `1 / (1 + variance)` of the three word counts, or zero when there are not exactly three lines |

The whitespace table contains all **29 codepoints** found by scanning CPython
3.12's Unicode 15.0.0 character space with `chr(cp).isspace()`. Runs of whitespace
collapse for word counting. Punctuation, combining marks, and zero-width joiners
remain inside their words. Case and normalization are preserved. Each scan follows AML's 10,000-iteration
loop budget; longer input fails explicitly rather than returning partial counts.

Only LF starts another line. CRLF therefore leaves CR inside the returned line;
CR still separates words. Unicode line/paragraph separators separate words but
do not add an LF-delimited line. Empty text has one line; a trailing LF adds an
empty line. In the original coherence formula, three empty lines have zero
variance and coherence one. Those behaviors are retained explicitly.

AML owns immutable UTF-8 values, codepoint indexing, and source-relative shared
`IMPORT`. Haiku expresses the whitespace and coherence decisions in AML. These
boundaries feed the next text organs: unique-word sets, context overlap, native
tokenization, and language-specific syllable checks.

## Run it

Build the sibling AML v5.3.0 runtime/compiler with native strings, lists and `IMPORT`, then:

The complete test command now includes the lexical suite and its v5.3.0 list
operations. Rebuild the library and runner together against that header.
See [lexical parity](LEXICAL_PARITY.md) for the current toolchain pin.

```sh
make -C ../ariannamethod.ai all
make test
```

`make test-text` runs the text suite alone. `AML_ROOT`, or `AML`, `AMLC`, and
`AML_LIB`, select another build. `tests/fixtures/text.aml` imports `src/text.aml`
directly. The numerical suite also imports its two modules directly.

The shell scripts run the fixtures through the interpreter and `amlc --scalar`,
compare each output with its exact success receipt, and remove build products.
The compiled text executable is launched from a different directory. Runtime
tests require the two project toolchains and system build tools.

## Reference fixture

`tests/reference/text_inputs.json` records **146 cases / 282 reference values**:

- 39 text cases check word count, line count, and original RAE coherence.
- 29 separator cases insert each Python whitespace character between words.
- 58 classification cases cover every whitespace codepoint, its neighbours,
  and selected non-space characters.
- 20 line cases check exact string returns, including negative indexes,
  empty lines, CRLF, accents, Cyrillic, Hebrew, and emoji.

The fixture is 223 physical lines; its longest line is 227 bytes. Its imported
module has five functions. Integer counts and string returns compare exactly;
coherence has absolute tolerance **0.000002** for AML float32 arithmetic.

Verified on Linux x86_64 with CPython 3.12.14 reference calls: all **282 text
reference values** pass in the AML interpreter and the compiled scalar
executable. The existing **130 numerical reference values** also pass after
switching their fixture to native module imports.

```text
PASS: 130 Python reference values in AML interpreter and compiled --scalar
PASS: 282 Python text reference values in AML interpreter and compiled --scalar
```

The original selector supplies coherence by a direct method call. No model
initialization or random draw is needed for feature extraction. The fixture
also includes the candidate from the original selector's feature test.

## Reproduce the reference

From this repository, using the pinned Python Haiku reference environment:

```sh
python3 - ../harmonix <<'PY'
import json, subprocess, sys, unicodedata
from pathlib import Path
source = Path(sys.argv[1]).resolve()
data = json.loads(Path('tests/reference/text_inputs.json').read_text())
assert subprocess.check_output(['git', '-C', str(source), 'rev-parse', 'HEAD'], text=True).strip() == data['source_commit']
subprocess.run(['git', '-C', str(source), 'diff', '--exit-code', 'HEAD', '--', 'haiku/rae_recursive.py'], check=True)
assert unicodedata.unidata_version == data['unicode_version']
sys.path.insert(0, str(source / 'haiku'))
from rae_recursive import RecursiveRAESelector
fixture = Path('tests/fixtures/text.aml').read_text()
literal = lambda s: json.dumps(s, ensure_ascii=False)
assert [cp for cp in range(0x110000) if chr(cp).isspace()] == data['whitespace_codepoints']
checked = 0
for group in ('text_cases', 'separator_cases'):
    for c in data[group]:
        s = c['text']
        assert (len(s.split()), len(s.split('\n')), RecursiveRAESelector.extract_features(None, s)[4]) == (c['words'], c['lines'], c['coherence']), c['name']
        argument = literal(s)
        if group == 'separator_cases':
            cp = c['codepoint']
            assert s == 'one' + chr(cp) + 'two\nthree four\nfive six'
            construction = f'source = text_concat(text_concat("one", text_from_codepoint({cp})), "two\\nthree four\\nfive six")'
            assert construction in fixture
            argument = 'source'
        args = [literal(c['name']), argument, str(c['words']), str(c['lines']), format(c['coherence'], '.17g')]
        call = 'haiku_text_check(' + ', '.join(args) + ')'
        if len(('failures = failures + ' + call).encode()) >= 256:
            assert 'source = ' + literal(s) in fixture
            args[1] = 'source'
            call = 'haiku_text_check(' + ', '.join(args) + ')'
        assert call in fixture, c['name']
        checked += 3
for c in data['space_cases']:
    assert int(chr(c['codepoint']).isspace()) == c['space']
    assert f"haiku_space_check({c['codepoint']}, {c['space']})" in fixture
    checked += 1
for c in data['line_cases']:
    lines, index = c['text'].split('\n'), c['index']
    expected = lines[index] if -len(lines) <= index < len(lines) else ''
    assert expected == c['expected'], c['name']
    call = 'haiku_line_check(' + ', '.join([literal(c['name']), literal(c['text']), str(index), literal(expected)]) + ')'
    assert call in fixture, c['name']
    checked += 1
print(f'PASS: {checked} text fixture values match direct pinned Python calls')
PY
```

The input strings here are reference fixtures. New conversation examples will
come from Haiku AML's speaking loop when the generator and learning organs join
these boundaries.
