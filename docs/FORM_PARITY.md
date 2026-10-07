# English syllables and 5–7–5

The English form organ now runs in AML. Its word and line counts follow
`HaikuGenerator._count_syllables` and `_count_line_syllables` in Python HAiKU,
commit `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`.
The reference uses **python-syllables 1.1.5**, Python **3.12.14**, and Unicode
**15.0.0**. Source attribution and the exact package-source digest are recorded
in [THIRD_PARTY.md](../THIRD_PARTY.md).

## API

Import [`src/form.aml`](../src/form.aml).

| Function | Result |
|---|---|
| `haiku_syllables(word)` | Original English estimate, at least one per token |
| `haiku_line_syllables(words)` | Exact sum over the supplied string list; an empty list gives zero |
| `haiku_form(source)` | Array of three syllable counts; requires exactly three literal-LF lines |
| `haiku_is_haiku(source)` | One for exactly three lines with counts 5, 7, 5; otherwise zero |

The last two functions compose the existing Unicode whitespace splitter with
word syllable counts. Empty lines count zero. A final LF creates another line;
CR and the other Python whitespace codepoints separate words within a line.
`haiku_line_syllables` preserves the supplied token boundaries, so an explicit
empty token contributes one. Calls preserve their input text and lists.

A word or form source can contain at most **10,000 codepoints**. A line list can
contain at most **10,000 words**. The exact integer sum is limited to **2^24**,
AML's float32 consecutive-integer boundary. Oversized inputs, incorrect types,
and larger sums fail explicitly. A per-call map caches repeated word counts
while summing a line.

## Preserve the original estimator

The original estimator counts runs of `aeiouy` after lowercasing, then applies
123 subtracting and 29 adding regex rules to the **original case-sensitive
word**. Every rule uses `re.match`, beginning at the first codepoint.

The AML implementation translates those rules into four string lists and
explicit predicates. The 108 subtracting and five adding dot/suffix rules
match exactly one initial non-LF codepoint followed by an exact suffix.
The prefix lists retain all entries, including the duplicate `^mc` rule.

These observed details are carried into the port:

- `mcdonald` counts four; `McDonald` counts two. `mc` counts two.
- `$` also matches immediately before a single final LF. Regex dot excludes
  LF; negated character classes include it.
- `([^aeiouy])1l$` contains a literal digit `1`; `A1l` counts two.
- The three-vowel regex class excludes `y`; the vowel-group scan includes it.
- Prefix matching makes the `ism` rule apply to `ism`, which counts two;
  `prism` counts one.
- Empty and vowelless tokens count at least one.

An exhaustive development scan of Unicode 15.0.0 found seven codepoints outside
`aeiouy` whose lowercase forms contain an ASCII vowel: `A E I O U Y` and `İ`
(U+0130). The latter lowers to `i` plus U+0307, which closes the vowel run.
AML reproduces that effect directly: `İa` counts two; `IY` counts one.
Regex rules continue to inspect the original codepoints.

## Verification

Run `bash tests/run_form.sh` with the sibling AML scalar toolchain built.
`HAIKU_AML`, `HAIKU_AMLC`, and `HAIKU_AML_LIB` can select another installation.
The test runner executes both the interpreter and a compiled scalar binary
from a separate working directory.

[`tests/reference/form_inputs.json`](../tests/reference/form_inputs.json)
records **1,930 cases / 1,966 reference results**:

- Every one of Python HAiKU's **587 seed entries**, in its original order.
  Their counts total **1,145**: 229 one-syllable, 209 two-syllable,
  105 three-syllable, 37 four-syllable, and seven five-syllable entries.
- **1,216 rule cases**: one witness for each of the 152 regex entries, then
  uppercase, prefix, extension, final-LF, final-CRLF, leading-LF, and multibyte
  initial-codepoint variants.
- **100 edge cases** covering case differences, empty strings, Unicode case
  expansion, combining marks, multilingual text, emoji, whitespace, punctuation,
  and the special regex predicates.
- **Nine token-list sums**, including repeated/empty tokens and the full seed
  corpus.
- **18 form predicates**, with all three counts checked separately for the
  twelve inputs containing exactly three lines. The historical Python dialogue
  examples are form inputs here; their lines retain the original estimates.

Six additional checks exercise the accepted 10,000-codepoint word/source,
10,000-word list, and exact **16,777,216**-syllable boundaries. Ten invalid-input
fixtures cover the corresponding excesses, wrong types, and wrong line counts.
Counts compare exactly. Fixtures use AML and shell; Python supplies the
reproducible development oracle.

```text
PASS: 1966 English form reference results and 6 boundary checks in AML interpreter and compiled --scalar
PASS: 10 invalid form inputs rejected in both execution paths
```

## Reproduce the reference values

With the pinned Python reference environment installed, run from this repository.
The original methods are called on an uninitialized generator object, so this
check neither creates a database nor initializes learning weights.

```sh
python3 - ../harmonix <<'PY'
import hashlib, json, subprocess, sys, unicodedata
from pathlib import Path
source = Path(sys.argv[1]).resolve()
data = json.loads(Path('tests/reference/form_inputs.json').read_text())
assert subprocess.check_output(['git', '-C', str(source), 'rev-parse', 'HEAD'], text=True).strip() == data['source_commit']
subprocess.run(['git', '-C', str(source), 'diff', '--exit-code', 'HEAD', '--', 'haiku/haiku.py'], check=True)
sys.path.insert(0, str(source / 'haiku'))
import syllables
from haiku import HaikuGenerator, SEED_WORDS
assert syllables.__version__ == data['syllables_version']
assert hashlib.sha256(Path(syllables.__file__).read_bytes()).hexdigest() == data['syllables_sha256']
assert unicodedata.unidata_version == data['unicode_version']
generator = HaikuGenerator.__new__(HaikuGenerator)
assert [c['word'] for c in data['word_cases'] if c['group'] == 'seed'] == SEED_WORDS
for case in data['word_cases']:
    assert generator._count_syllables(case['word']) == case['syllables'], case['name']
for case in data['line_cases']:
    assert generator._count_line_syllables(case['words']) == case['syllables'], case['name']
for case in data['form_cases']:
    counts = [generator._count_line_syllables(line.split()) for line in case['source'].split('\n')]
    assert counts == case['counts'], case['name']
    assert int(counts == [5, 7, 5]) == case['is_haiku'], case['name']
# Verify every word expectation is the exact assertion executed by AML.
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
fixtures = '\n'.join(p.read_text() for p in sorted(Path('tests/fixtures').glob('form_words_*.aml')))
for case in data['word_cases']:
    expected = f'assert(haiku_syllables({literal(case["word"])}) == {case["syllables"]}, "{case["name"]}")'
    assert expected in fixtures, case['name']
mappings = [(cp, chr(cp).lower()) for cp in range(0x110000)
            if chr(cp) not in 'aeiouy' and any(c in 'aeiouy' for c in chr(cp).lower())]
assert mappings == [(65, 'a'), (69, 'e'), (73, 'i'), (79, 'o'), (85, 'u'), (89, 'y'), (304, 'i\u0307')]
print('PASS: 1930 direct Python cases; all 1903 AML word assertions; Unicode vowel mappings')
PY
```
