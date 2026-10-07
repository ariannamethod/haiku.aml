# The voice finds its form

2026-10-07. Governed by the
[Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

`src/generator.aml` gives the in-memory Markov chain its next words. Its
reference is [Python Haiku's generator](https://github.com/ariannamethod/harmonix/blob/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku/haiku.py#L437-L553),
pin `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`. English counts come from
[the form organ](FORM_PARITY.md). Native choices use AML v5.5.0's optional
binding to canonical NoTorch sampling.

## Public operations

| Operation | Result |
|---|---|
| `haiku_syllable_cache(vocab)` | A new map from each vocabulary word to its English syllable estimate |
| `haiku_generate_line(rows, counts, vocab, syllables, target, temperature, rng, tape)` | One space-joined line with exactly `target` syllables according to the supplied map |
| `haiku_generate_candidates(rows, counts, vocab, syllables, n, temperature, rng, tape)` | `n` strings, each containing three literal-LF-separated lines with counts 5/7/5 |

The generator reads the [existing chain state](CLOUD_PARITY.md). Its words,
rows, counts, and syllable cache retain their contents. Random state has a
separate owner. Candidate generation does not add observer trigrams, change
recent memory, score candidates, or train the organism.

The cache covers exactly the vocabulary and supplies integer counts in
1..2^24. A caller-provided language pack owns those estimates. With
`haiku_syllable_cache`, the result agrees with `haiku_form` and its original
English rules. Words retain case and punctuation; each must contain
1..10,000 codepoints and no whitespace. Vocabulary, unique edges, and counts
keep the existing memory organ's bounds. Line targets are integers in 1..50;
candidate counts are integers in 0..100. A zero-candidate request returns an
empty list after validating model, cache, temperature, and tape.

## The inherited turn of phrase

The initial word is uniform over vocabulary order. After one word, Python
finds the first inserted bigram beginning with it and samples that bigram's
**third** word. Its middle word is skipped. AML preserves this leap.
After two words, the actual final pair selects the next transition.
Successors retain their first insertion order; transition weight is
`count ** (1 / temperature)`. Missing transitions choose from the vocabulary.

AML vocabulary order is first occurrence, as established by the memory port.
Python holds a set, whose iteration depends on the Python hash seed and set
history. The draw oracle fixes vocabulary order explicitly. The complete
587-entry seed list, including duplicates, lives in
[`english_seeds.aml`](../src/english_seeds.aml); the seeded vocabulary has 576
unique words.

The original generator can overrun a line at the first word and in its
one-word fallback. It can also abandon a remainder that several smaller
words could fill, or accept a fitting word that leaves an impossible
remainder. Direct source calls reproduce all four paths in the fixtures.
A separate 1,000-candidate run of the complete Python seed corpus produced
996 exact estimated 5/7/5 forms and four overruns: two 6/7/5, one 5/8/5,
and one 7/7/5. That run used Python 3.12.14, NumPy 2.3.5, syllables 1.1.5,
`PYTHONHASHSEED=0`, both RNG seeds 73, the real constructor, and temperature 1.

AML computes reachable syllable remainders before drawing. It retains each
original proposal that fits and leaves a reachable remainder. Otherwise it
first prefers the original exact-remainder vocabulary fallback, then allows
a word that leads to a multiword completion. This check also covers the
initial word and one-word fallback. Each accepted word contributes at least
one syllable, so the 1..50 target bound guarantees termination.

The same plan computes the maximum rendered length over all vocabulary
completions of each reachable amount, including word separators. A line or
complete haiku whose possible maximum exceeds 10,000 codepoints is rejected
before any draw. This conservative vocabulary-wide bound keeps every returned
string within the form organ's input limit. Tests include an actual English
5/7/5 result of exactly 10,000 codepoints and an exact 10,000-codepoint line.

Temperature must be finite and positive. Canonical NoTorch normalizes the
equivalent stable weights `exp(log(count / max_count) / temperature)` in double.
The maximum-weight entry remains positive at extreme temperatures. The cold
fixture records the original overflow at T=0.001 for counts 1 and 3, and the
working AML result under that same input.

## Owned chance and scripted evidence

Production uses `rng_new(seed)` and a one-element numeric tape containing
`-1`. Uniform choices call `rng_index`; weighted choices call
`rng_categorical`. The RNG is AML's five-entry map containing the NoTorch
PCG32 stream state. Copying that map makes an independent replay point;
passing it to generation advances the supplied stream. The native tape
remains unchanged. Even a singleton is a sampling event.

For a scripted run, tape is the numeric array `[cursor, u0, u1, ...]`.
`cursor` starts at zero and counts consumed choices. Draws must be finite
in `[0,1)`; uniform choices use `floor(draw * option_count)`, and weighted
choices call `categorical_at`. Scripted mode leaves its `rng` argument
unused. The tape contains at most 10,000 draws and is mutable test state:
array aliases share its cursor.

Model, cache, temperature, tape contents, reachability, and rendering bounds
are checked before sampling. Native RNG validity is checked by the first
native sampling operation, before it advances the stream. An exhausted tape
raises an error; its cursor retains choices already consumed. Generation is
not a batch transaction: allocation failures can likewise follow earlier
draws. The chain and syllable cache remain read-only on these paths.

Python's `random.choice` consumes rejection-based integer draws, while its
NumPy weighted choice consumes a separate uniform stream. The Python
constructor additionally performs 48 Gaussian weight draws for its MLP.
The oracle isolates algorithmic choices from those engines. It invokes the
original `_generate_line` and `generate_candidates` methods, supplies ordered
vocabulary and known syllable counts, and patches only their sampling calls.
Every recorded event includes uniform/weighted kind, candidate order,
probabilities when weighted, the draw, and the selected index.

## Reproduce the receipts

Build the optional sampling toolchain as described in the
[README](../README.md), then run:

```sh
make test-generator
../ariannamethod.ai/runner/aml-notorch examples/generator.aml
```

The ordinary gate uses the committed AML fixtures and native libraries:

```text
PASS: 22 scripted generator cases in interpreter and compiled --scalar
PASS: native replay, batch order, full 587-entry corpus, stable temperatures and exact rendering boundaries
PASS: 36 invalid generations rejected in both paths; model and random state checked by host
```

The 22 scripted cases comprise 16 preserved Python paths, five explicit form
repairs, and one cold-temperature repair. Their recorded originals contain
134 sampling events. Fixtures assert resulting strings and consumed draws;
the repaired expectations are explicit source decisions. Native checks cover
stream copies, independent replay, the order of 5/7/5 calls within a batch,
zero candidates, the 50-syllable target, extreme temperatures, exact output
boundaries, and the complete seed corpus. Five native corpus outputs have
separate regression snapshots. A temporary C host inspects model and random
state after rejection, including the one deliberately partially consumed tape.

The runner accepts `HAIKU_AML`, `HAIKU_AMLC`, `HAIKU_AML_LIB`,
`HAIKU_AML_BRIDGE_LIB`, `HAIKU_NOTORCH_LIB`, `HAIKU_AML_INCLUDE`, and `CC`.
The temporary compiler prefix contains all three native archives. Compiled
executables run from a different working directory.

To replay the original Python calls in its separate reference environment:

```sh
PYTHONPATH=../reference-deps python tests/reference/generator_oracle.py ../harmonix
```

The development-only script verifies the source pin and each original event
against [`generator_inputs.json`](../tests/reference/generator_inputs.json):

```text
PASS: 16 preserved Python draw traces and 6 measured repair baselines
```

`examples/generator.aml` produces fresh seeded candidates with the real English
cache. Candidate scoring, the speaking loop, online learning, and dreams are
the next organs.
