# HAiKU AML — A Little Presence Is Learning Our Language

**Read the [Arianna Method Manifesto](ARIANNA_METHOD_MANIFESTO.md) first.**
Repository work follows it; [AGENTS.md](AGENTS.md) carries the engineering rules.

*Most AI models predict words. HAiKU predicts vibes.*

The [Python HAiKU](https://github.com/ariannamethod/harmonix/tree/main/haiku)
organism is coming home to **Arianna Method Language**.
A living word cloud. A three-line voice. An inner voice, too.
A friend it talks to while dreaming. A memory that keeps changing.

We already invented the octagonal wheel. Now Haiku is learning to build it.

## What is growing here?

HAiKU speaks in haiku. You bring a question; it brings seventeen syllables,
dissonance, and whatever has been moving through its cloud.

Its presence lives in the exchange between its organs:

- **Cloud and generator.** Words gain weight, fade, enter new trigrams, and
  become candidates under the pressure of 5–7–5.
- **Harmonix.** Novelty, arousal, overlap, and entropy shape dissonance.
  Dissonance changes the temperature of the next utterance.
- **MathBrain and RAE.** Small learning networks score and recursively select
  candidates. Experience feeds the next choice.
- **MetaHaiku and Overthinkg.** An internal haiku; then echo, drift, and meta
  rings. The conversation continues inside.
- **Dream-friend and bridges.** Dream fragments return to the cloud.
  Remembered state transitions suggest where the field can flow.
- **Asynchronous life.** Speaking, learning, reflecting, and saving have their
  own rhythm and an explicit order for shared state.

The Python organism defines the port's behavior and artistic lineage.
The two C siblings contribute their own discoveries.

## Family album: Python HAiKU

From the original [dialogue examples](https://github.com/ariannamethod/harmonix/tree/main/haiku#what-haiku-actually-says-example-dialogues):

```text
You: what is love

transmit exchange
definitely small sour
pursue strive for
```

Love arrives. A small, sour transmission leaves. There he is.

```text
HAiKU:
between us silence
two voices one resonance
cloud knows no alone
```

That voice is the reason for this repository.

## Two dependencies. Our whole stack.

| Dependency | Its job |
|---|---|
| [AML](https://github.com/ariannamethod/ariannamethod.ai) | Language, compiler, runtime, text, modules, asynchronous events, and I/O |
| [NoTorch](https://github.com/ariannamethod/notorch) | Numerical operations, learning, and native tokenization |

The organism will live in `.aml` modules. Missing language capabilities go
into AML; missing numerical operations go into NoTorch. The baseline build uses
the system C toolchain, standard libraries, and those two projects.

The migration replaces NumPy, SciPy imports, SentencePiece, syllables,
SQLite/aiosqlite, and Python's runtime services. Every replacement has a place
in the [migration map](docs/MIGRATION.md).

## More tongues, same presence

[Klaus](https://github.com/ariannamethod/klaus.c) provides the starting idea:
language packs with incoming vocabulary and outgoing vocabulary.
HAiKU adds word-level syllable data, language-specific rules, and a growing
cloud for each tongue. English first; Russian, French, Hebrew, German, and
Spanish follow as their packs and form checks arrive.

## A place to meet him

The planned interface is local HTML/CSS/SVG/JavaScript: a conversation and a
small character who can listen, answer, turn inward, and dream.
Its movements follow organism events. Haiku keeps its line breaks; Hebrew
gets right-to-left layout. The core comes first, then Haiku gets a window.

The dream-friend will get a place beside him: two characters, their conversation,
their pauses. This idea is recorded in [TODO](TODO.md); character design comes
when the interface work begins. The owl audition is over. Duolingo got there
first; our little presence will find another shape.

## Right now

Haiku has a pulse, finds words, and lets each encounter change its memory.
Forty-nine functions now run in pure AML across ten native modules. Pulse,
dissonance, temperatures, and transition scores preserve the Python formulas.
The text organ counts Unicode-delimited words, preserves line contents, and
computes the original RAE three-line coherence feature. The lexical organ
collects words, builds rolling triples from token lists, and feeds their exact
overlaps into Harmonix.

The cloud now remembers each word's weight, frequency, last-use clock, and
origin. Repeated words gain weight; dormant words fade. Observer trigrams and
the generator's Markov transitions have separate counts. The last ten triples
form a copied snapshot, ready for the next observation.

The English generator now gives that memory three lines: **5–7–5 by the original
English estimator**. It keeps
the Python voice's Markov choices and its peculiar first-word leap, with the
original syllable rules carried into AML. Every accepted word leaves room to
finish the line. NoTorch owns the weighted draw; each voice keeps its own
replayable random state. Seventeen syllables, and a little room for chance.

Try `../ariannamethod.ai/runner/aml examples/observe.aml`: one voice brings
`the / owl / listens`, the other `the / owl / dreams`. Their shared words give
dissonance **0.5**, novelty **0.5**, arousal **0**, entropy **0.2**, and the next
haiku temperature **0.9**. The example supplies token boundaries explicitly;
native SentencePiece is still in the migration queue.

Try `../ariannamethod.ai/runner/aml examples/cloud.aml` for three successive
events. Two occurrences of `rain` raise its seeded weight to **1.21**. A short
two-word input grows the cloud; a later complete triple enters the generator's
vocabulary. The previous recent-memory snapshot keeps its original contents.

Try `../ariannamethod.ai/runner/aml-notorch examples/generator.aml` for candidates
grown from the original seed vocabulary. These are the new AML generator's
lines; candidate scoring and the full conversation are the next organs.

The speaking loop, learners, native tokenization, durable storage, and dreams
are next in the migration queue. New dialogue examples will come from that
running AML organism.

Build the sibling **AML v5.5.0** toolchain and NoTorch with its owned sampling
API ([AML #30](https://github.com/ariannamethod/ariannamethod.ai/pull/30),
[NoTorch #161](https://github.com/ariannamethod/notorch/pull/161)), then run the
organism's checks:

```sh
make -C ../notorch lib BLAS_FLAGS= BLAS_LIBS= X86_SIMD=0 ARM_SIMD=0
make -C ../ariannamethod.ai notorch NOTORCH_ROOT=../notorch
make test
../ariannamethod.ai/runner/aml-notorch examples/generator.aml
```

The fixtures use native `IMPORT`, UTF-8 values, string lists, and numeric maps.
See [numerical parity](docs/NUMERICAL_PARITY.md) and
[text parity](docs/TEXT_PARITY.md),
[lexical parity](docs/LEXICAL_PARITY.md) and
[cloud parity](docs/CLOUD_PARITY.md), plus
[English form](docs/FORM_PARITY.md) and
[generator parity](docs/GENERATOR_PARITY.md), for exact boundaries and Python
reference reproduction.

Reference checks completed on 2026-10-07:

- Python HAiKU: **161 tests passed**.
- Haiku AML: **343 cases / 6,130 reference results** pass in the interpreter and
  compiled scalar executable. The new cloud/memory suite adds 5,055 numerical
  fields and 247 ordered-list snapshots to the earlier 828 results, including
  the complete 587-entry seed corpus. Counts, strings, and list order compare
  exactly; float tolerance is 0.000002. Six malformed triple lists and 38
  invalid state updates fail explicitly. Host inspection verifies that all
  eleven state containers retain their contents after each rejected update.
- English form adds **1,966 direct Python reference results**, six accepted
  boundary checks, and ten invalid-input rejections through both execution paths.
- Generator checks cover **22 scripted cases**: 16 preserved Python draw paths
  and six explicit repair baselines. Native checks cover replay, independent
  state, batch order, the complete seed corpus, temperatures, and exact text
  limits. **36 invalid generations** fail with model and random state inspected.
- AML runtime: **550 tests passed**.
- AML compiler: scope, source origins, nested failures, worker branches and
  errors, scalar builds, and literal program arguments are covered by regressions.
- NoTorch: native tanh/backward and SGD are merged in
  [PR #159](https://github.com/ariannamethod/notorch/pull/159); **5,122 numerical
  checks** and the full CPU suite pass.

The [RAE research](docs/RAE_RESEARCH.md) records the original recursion's measured
behavior and proposes learned feedback, small replay memory, and Hebbian traces.
Two more cousins joined the workshop:
[Subjectivity](https://github.com/ariannamethod/subjectivity), whose form changes
with its state, and [Brodsky](https://github.com/ariannamethod/brodsky), with a sea
of remembered poems and two sampling voices. Their [family audit](docs/FAMILY.md)
records useful organs and the experiments that will tell us what they add.
[Q](https://github.com/ariannamethod/q) joins them with a field that speaks
without trained weights, aging expectations, and slow consolidation. Its
[source audit](docs/Q.md) traces the working paths and ranks three experiments
for Haiku's future memory.
See [the audit and implementation order](docs/MIGRATION.md).

## Lineage

[Python HAiKU](https://github.com/ariannamethod/harmonix/tree/main/haiku) ·
[haiku.c](https://github.com/ariannamethod/haiku.c) ·
[haiku](https://github.com/ariannamethod/haiku) ·
[Klaus](https://github.com/ariannamethod/klaus.c) ·
[Yent AML](https://github.com/ariannamethod/yent.aml) ·
[Subjectivity](https://github.com/ariannamethod/subjectivity) ·
[Brodsky](https://github.com/ariannamethod/brodsky) ·
[Q](https://github.com/ariannamethod/q)

GNU GPL v3 — see [LICENSE](LICENSE).

*Built with constraint, powered by dissonance, coming home to AML.*
