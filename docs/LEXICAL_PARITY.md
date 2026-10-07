# The owl can measure the words between us

2026-10-07. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

`src/lexicon.aml` connects actual token triples to Harmonix's pulse and
dissonance. Its reference is
[`harmonix/haiku` at `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`](https://github.com/ariannamethod/harmonix/tree/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku):
`Harmonix.compute_dissonance` in `harmonix.py`, the rolling window in
`DualTokenizer.tokenize_trigrams`, and Python's Unicode `str.split()`.

## Boundary and functions

| AML function | Contract |
|---|---|
| `haiku_words(text)` | Python whitespace splitting, preserving each word's exact text and order |
| `haiku_trigrams(tokens)` | Rolling groups of three over already segmented strings; returns a flat string list |
| `haiku_unique(tokens)` | Exact string set, in first-occurrence order |
| `haiku_trigram_count(triples)` | Number of complete triples; an incomplete group fails at the checked list boundary |
| `haiku_trigram_find(triples, needle, offset)` | Flat offset of the first exact three-string match, or `-1`; both lists contain complete triples, and `offset` points at a complete triple in `needle` |
| `haiku_overlap_counts(user, system)` | `[user_triples, system_triples, word_intersection, word_union, exact_triple_overlap]` |
| `haiku_observe_trigrams(user, system)` | `[dissonance, novelty, arousal, entropy]`, passed through the existing `haiku_observe` |

Each triple occupies exactly three entries in a native AML string list. A
rolling four-token input therefore produces six entries: tokens 0–2, followed
by tokens 1–3. List counts retain repeated triples. Word intersection/union and
exact-triple intersection use distinct elements, matching the Python sets.

Comparison is structural: three complete strings are compared independently.
No delimiter is inserted. Tokens such as `a b`, `b|c`, an empty string, a newline,
or a Unicode separator keep their identity. Case, accents and normalization are
preserved. This matters when two different triples would have the same joined
text.

`haiku_words` supplies whitespace words. Haiku's SentencePiece path first
lowercases and segments into subwords, removes its boundary markers, then builds
triples. The API here starts **after segmentation**; callers provide the tokens
for their selected tokenizer mode. SentencePiece parity remains its own step.

Both input lists are structurally validated before an empty-participant result
can return. A partial group raises AML's `list index out of range` and stops the
program. Ordinary empty lists remain valid and produce the original neutral
observation. A triple of three empty strings has one distinct word: the empty
string.

The lists retain AML's 65,536-item bound and the functions retain its
10,000-iteration loop budget. Word sets use linear exact lookup; triple sets
compare their components directly. Longer scans fail explicitly at the loop
budget. Indexed lookup for the growing cloud is a later measured step.

## Run the organ

Toolchain pin: [AML PR #28](https://github.com/ariannamethod/ariannamethod.ai/pull/28),
`11bedd5e6cc042cc80bfc85f1d30443df09827d2` (v5.3.0). Rebuild the runner and scalar
library together against its updated header.

Build AML with native strings, imports and mutable string lists, then run:

```sh
make -C ../ariannamethod.ai all
make test
../ariannamethod.ai/runner/aml examples/observe.aml
```

`make test-lexical` runs the lexical suite alone. The example supplies
`the / owl / listens` and `the / owl / dreams` as explicit tokens. Their word
intersection is two and union is four; the result is dissonance `0.5`, novelty
`0.5`, arousal `0`, entropy `0.2`, and temperatures `0.9` / `0.3`.

The test uses native `IMPORT` throughout and exercises both the interpreter and
`amlc --scalar`; compiled programs run from a different directory. Override the
toolchain through `HAIKU_AML`, `HAIKU_AMLC`, and `HAIKU_AML_LIB` when invoking the
script directly.

## Fixture coverage

`tests/reference/lexical_inputs.json` records **128 cases**:

- 36 observations check **324 numerical values**: five independently computed
  set/list counts and four values returned by the original Harmonix.
- 68 whitespace cases check complete ordered word lists, including all 29
  CPython whitespace codepoints and multilingual text.
- 12 rolling-window cases and 12 unique-word cases check complete ordered
  string lists. Together with splitting, these are **92 list results**.
- Six malformed-list runs cover lengths 1, 2 and 4 on either side while the
  other side is empty; both execution paths must fail before continuation.

All 19 valid-triple observations from the earlier numerical fixture are carried
forward. Its `empty_word_union` input contains zero-element tuples rather than
triples; that case remains in the count-boundary numerical suite. New cases
cover Cyrillic, Hebrew, French accents, emoji, distinct Unicode normalizations,
empty tokens, repeated exact matches, and space/pipe/control-separator collisions.

Counts and list contents compare exactly. Observation floats use absolute
tolerance **0.000002**, with explicit NaN rejection. The ordinary test command
executes AML and shell; Python is used only to reproduce the development oracle.

Verified in both AML execution paths on Linux x86_64:

```text
PASS: 324 numerical and 92 list reference results in AML interpreter and compiled --scalar
PASS: 6 malformed triple lists rejected in both execution paths
```

## Reproduce the reference values

The rolling-window check calls the original Python method with its segmentation
step supplied by the recorded token list. This directly exercises the original
trigram loop without changing the tokenizer mode of the application.

From this repository, with the pinned Python reference environment:

```sh
python3 - ../harmonix <<'PY'
import json, subprocess, sys
from pathlib import Path
source = Path(sys.argv[1]).resolve()
data = json.loads(Path('tests/reference/lexical_inputs.json').read_text())
assert subprocess.check_output(['git', '-C', str(source), 'rev-parse', 'HEAD'], text=True).strip() == data['source_commit']
subprocess.run(['git', '-C', str(source), 'diff', '--exit-code', 'HEAD', '--', 'haiku/harmonix.py', 'haiku/tokenizer.py'], check=True)
sys.path.insert(0, str(source / 'haiku'))
from harmonix import Harmonix
from tokenizer import DualTokenizer

def literal(text):
    parts, buf = [], ''
    for ch in text:
        if ord(ch) < 32 and ch not in '\n\r\t':
            if buf: parts.append(json.dumps(buf, ensure_ascii=False)); buf = ''
            parts.append(f'text_from_codepoint({ord(ch)})')
        else: buf += ch
    if buf or not parts: parts.append(json.dumps(buf, ensure_ascii=False))
    expr = parts[0]
    for part in parts[1:]: expr = f'text_concat({expr}, {part})'
    return expr

def fixtures(pattern):
    return '\n'.join(p.read_text() for p in sorted(Path('tests/fixtures').glob(pattern)))

def block(text, name):
    return text.split('# ' + name + '\n', 1)[1].split('\n# ', 1)[0].strip()

def list_source(var, values):
    return [f'{var} = list_new()'] + [f'list_push({var}, {literal(v)})' for v in values]

def numbers(values):
    return '[' + ', '.join(format(float(v), '.17g') for v in values) + ']'

numeric, lists = 0, 0
text = fixtures('lexical_observe_*.aml')
h = Harmonix(':memory:')
try:
    for c in data['observe_cases']:
        u, s = [list(map(tuple, c[side])) for side in ('user', 'system')]
        uw, sw = [{w for t in ts for w in t} for ts in (u, s)]
        counts = [len(u), len(s), len(uw & sw), len(uw | sw), len(set(u) & set(s))]
        d, p = h.compute_dissonance(u, s)
        obs = list(map(float, [d, p.novelty, p.arousal, p.entropy]))
        assert (counts, obs) == (c['counts'], c['observation']), c['name']
        lines = ['user = list_new()', 'system = list_new()']
        for side in ('user', 'system'):
            for triple in c[side]:
                lines.append('lexical_add(' + ', '.join([side] + [literal(t) for t in triple]) + ')')
        lines += ['counts = ' + numbers(counts), 'observation = ' + numbers(obs)]
        lines.append('failures = failures + lexical_observe_check(' + json.dumps(c['name']) + ', user, system, counts, observation)')
        assert block(text, c['name']) == '\n'.join(lines), c['name']
        numeric += 9
finally:
    h.close()

text = fixtures('lexical_words_*.aml')
for c in data['word_cases']:
    assert c['text'].split() == c['words'], c['name']
    lines = ['source = ' + literal(c['text'])] + list_source('expected', c['words'])
    lines += ['actual = haiku_words(source)', 'failures = failures + lexical_list_check(' + json.dumps(c['name']) + ', actual, expected)']
    assert block(text, c['name']) == '\n'.join(lines), c['name']
    lists += 1

for kind in ('rolling', 'unique'):
    text = fixtures('lexical_' + kind + '.aml')
    for c in data[kind + '_cases']:
        if kind == 'rolling':
            tokenizer = object.__new__(DualTokenizer)
            tokenizer.tokenize_subword = lambda ignored, tokens=c['tokens']: list(tokens)
            triples = tokenizer.tokenize_trigrams('already segmented fixture')
            assert list(map(list, triples)) == c['triples'], c['name']
            expected = [w for t in triples for w in t]
            fn = 'haiku_trigrams'
        else:
            expected = list(dict.fromkeys(c['tokens']))
            assert expected == c['unique'], c['name']
            fn = 'haiku_unique'
        lines = list_source('tokens', c['tokens']) + list_source('expected', expected)
        lines += [f'actual = {fn}(tokens)', 'failures = failures + lexical_list_check(' + json.dumps(c['name']) + ', actual, expected)']
        assert block(text, c['name']) == '\n'.join(lines), c['name']
        lists += 1
print(f'PASS: {numeric} numerical and {lists} list results match direct pinned Python calls')
PY
```
