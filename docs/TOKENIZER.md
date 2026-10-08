# The words entering the cloud

`src/tokenizer.aml` ports `DualTokenizer` from
`harmonix@abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`.
The foreground chooses a named mode once; token identities then flow into
the observer, cloud and Markov chain unchanged.

| Mode | Token boundary |
| --- | --- |
| `regex` (default) | Lowercase, then runs of Unicode letters/numbers or `_` |
| `sentencepiece` | Lowercase, native Unigram pieces, remove every `▁`, drop empties |

`haiku_tokenize(source, mode, model)` returns a list of strings.
Regex accepts `0` for the unused model. SentencePiece takes an immutable
`tokenizer_load(path)` value; loading errors stop the operation. Relative
model paths follow the AML source file. `haiku_trigrams(tokens)` returns the
source's overlapping triples, flattened into groups of three list entries.

The original root launcher, `haiku_run.py`, left its working directory
unchanged. Its default relative model path missed `haiku/models/`, so it
used regex. Running inside `haiku/` loaded the 650-piece model. The AML
default preserves the root launcher's behavior; selecting SentencePiece
activates the original [model data](../models/README.md) explicitly.

Regex follows CPython 3.12 / Unicode 15: `\w` is `isalnum()` plus `_`.
Combining marks and joiners break runs. `ΟΣ` becomes `ος`; `İ` becomes
`i` followed by a combining dot, leaving the token `i`.

The model carries its own `nfkc_cf` normalization table and binary float32
piece scores. `Hello WORLD` becomes `he`, `ll`, `o`, `wor`, `ld`.
Unknown adjacent characters keep their normalized surface, so
`Привет мир,` becomes `привет`, `мир,`. Tabs and newlines survive as native
unknown pieces. The generator's word validator rejects whitespace tokens;
the foreground checks that boundary before publishing an exchange.

Input and lowercased text each have a 10,000-codepoint limit. Lowercase
expansion is checked before scanning. Native piece count is also bounded
at 10,000. Marker-containing pieces are checked before cleanup. Strings
and the loaded model remain immutable throughout tokenization.

`make test-tokenizer` checks 84 pinned Python cases in the interpreter and
compiled scalar runner: punctuation, unknown runs, compatibility forms,
normalization sequences, multilingual marks, specials, Unicode category
boundaries, and 48 seeded random inputs. It compares native pieces, final
tokens and complete rolling triples byte-for-byte. Both runners load the
model from a foreign working directory. Additional gates cover empty
results, all/interior markers, limits after lowercase expansion, explicit
mode errors, missing models and the generator's whitespace boundary.

The committed receipts and development-only replay are
`tests/reference/tokenizer_inputs.json` and `tokenizer_oracle.py`.
Python, SentencePiece and their packages are used to regenerate the oracle;
the committed runtime gates use AML and NoTorch.
