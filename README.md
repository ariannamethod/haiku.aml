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

Haiku now listens, answers, learns, and remembers across AML conversations.
Its organs run in pure AML modules. Pulse,
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

MathBrain and RAE now give those lines a learned preference. Each has its own
57 parameters: five features, eight hidden neurons, one answer. MathBrain
scores a candidate; RAE passes its candidates through the original three-step
recurrence and chooses a voice. Both learn through canonical NoTorch arithmetic,
with their features, gradients, clamping, and experience order composed in AML.
Small networks. Actual changing weights. The octagonal wheel has opinions.

The foreground now joins these organs: input → tokens → living memory → five
candidates → RAE's choice → a response → quality → MathBrain learning.
The default preserves Python chat's fixed RAE selector; set `train_rae = 1`
in `haiku.aml` before starting a new state file to let that selector learn from
the same chosen experience, too.
Each learner and random stream has an explicit owner.

Close the terminal. Come back. The cloud remembers.

Each completed exchange saves one versioned state: cloud, ordered transitions,
recent memory, both learners and their statistics, both random streams, and
the latest bridge event. The next launch resumes `haiku.state` beside the
entrypoint. Its tokenizer identity and logical clock come with it. Set
`state_path = ""` for a fresh, unsaved conversation, or choose another path
for another voice. One running process owns each state file.

Run `../ariannamethod.ai/runner/aml-notorch haiku.aml`. The first exchange of a
fresh native state, with the checked-in seeds and settings:

```text
You: what is love

sweet sour doubt stable
more least appear frequently
to and reinforce
```

Still a little sour. Now he says it in AML.

Tokenization has two explicit modes. `regex` keeps lowercase Unicode words;
`sentencepiece` loads the original 650-piece Unigram model through NoTorch,
including its own normalization table. Python's choice depended on the launch
directory; AML's choice is written in `haiku.aml`. The default matches launching
Python from the Harmonix root. Model paths follow the AML source, so launching
from another directory does not change the voice. Acquire the optional model
with `bash scripts/setup-tokenizer.sh`; its exact size and SHA-256 are checked
before installation. Regex needs no model download. Existing state retains
its selected mode and verifies the identity of the loaded model. See
[the tokenizer](docs/TOKENIZER.md).

Try `../ariannamethod.ai/runner/aml examples/observe.aml`: one voice brings
`the / owl / listens`, the other `the / owl / dreams`. Their shared words give
dissonance **0.5**, novelty **0.5**, arousal **0**, entropy **0.2**, and the next
haiku temperature **0.9**. This small example supplies token boundaries
explicitly; `haiku.aml` obtains them from the selected tokenizer.

Try `../ariannamethod.ai/runner/aml examples/cloud.aml` for three successive
events. Two occurrences of `rain` raise its seeded weight to **1.21**. A short
two-word input grows the cloud; a later complete triple enters the generator's
vocabulary. The previous recent-memory snapshot keeps its original contents.

Try `../ariannamethod.ai/runner/aml-notorch examples/generator.aml` for candidates
grown from the original seed vocabulary. Try
`../ariannamethod.ai/runner/aml-notorch examples/selection.aml` to follow five
generated candidates through RAE's choice, an explicit learning event, and the
next scores. These examples expose individual generator and learner events;
`haiku.aml` carries them through the conversation.

Sessions now save after learning and resume with their logical-turn clock.
[Continuity](docs/STATE.md) records the complete schema, restore validation,
and restart checks. Full Phase4 transition aggregation, reflection, rings,
dreams, and asynchronous scheduling are next. The [foreground record](docs/FOREGROUND.md)
keeps that boundary explicit. The family album above still belongs to Python;
new inner dialogues will come from the AML dream path when it runs.

Build the sibling **AML v5.8.0** toolchain and NoTorch with native Unigram,
model identities, numerical values, and owned sampling, then run the organism's checks:

```sh
make -C ../notorch lib BLAS_FLAGS= BLAS_LIBS= X86_SIMD=0 ARM_SIMD=0
make -C ../ariannamethod.ai notorch NOTORCH_ROOT=../notorch
bash scripts/setup-tokenizer.sh
make test
../ariannamethod.ai/runner/aml-notorch haiku.aml
../ariannamethod.ai/runner/aml-notorch examples/generator.aml
../ariannamethod.ai/runner/aml-notorch examples/selection.aml
```

The fixtures use native `IMPORT`, UTF-8 values, string lists, and numeric maps.
See [numerical parity](docs/NUMERICAL_PARITY.md) and
[text parity](docs/TEXT_PARITY.md),
[lexical parity](docs/LEXICAL_PARITY.md) and
[cloud parity](docs/CLOUD_PARITY.md), plus
[English form](docs/FORM_PARITY.md) and
[generator parity](docs/GENERATOR_PARITY.md),
[MathBrain](docs/MATHBRAIN.md), [RAE](docs/RAE.md),
[tokenization](docs/TOKENIZER.md), [foreground exchanges](docs/FOREGROUND.md),
and [continuity](docs/STATE.md), for exact boundaries and Python
reference reproduction.

Reference checks completed on 2026-10-07–08:

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
- MathBrain adds **3,717 Python reference values**, including every gradient
  and parameter in 24 learning steps, unclipped negative predictions, loss
  statistics, and full parameter clamps. Twenty-two rejected observations
  preserve model, statistics, memory, context, and random state.
- RAE adds **1,657 reference fields**: Unicode features, every refinement pass,
  stable rule choices, ten complete gradient/parameter updates, and the native
  generator-to-learning trace. Twenty-three rejected operations preserve all
  inspected owners. Both learner suites pass interpreted and compiled.
- Tokenization adds **6,034 exact reference fields** from 84 Python cases in
  both modes, plus nine rejected operations. Both execution paths preserve
  native pieces, cleaned tokens, rolling triples, and source-relative loading.
- Foreground checks replay **seven Python turns** and the explicit punctuation
  repair, comparing candidates, complete memory, draw cursors, losses, and
  learner parameters. Real interpreted/compiled conversations agree; blank,
  quit, EOF, and rejected input preserve the inspected owners. Shared RAE
  experience has its own checked update.
- Continuity compares **five seven-turn variants** with a fresh-process restart:
  regex/SentencePiece, fixed/learning RAE, native/scripted draws, and a supplied
  syllable cache. Responses and complete checkpoint bytes agree through the
  interpreter and compiled executable. **34 invalid states** preserve every
  live owner and the previous file; failed staged generation and failed saving
  do the same. The actual CLI resumes saved configuration and withholds an
  unpublished response after a save failure.
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
[Q / PostGPT](https://github.com/ariannamethod/q) brings the direct line from
corpus and token identity to a coherent transition field. Q is the active
PostGPT continuation, with aging expectations and slow consolidation on that
substrate. Its [source audit](docs/Q.md) traces the working paths and ranks three
experiments for Haiku's future memory.
See [the audit and implementation order](docs/MIGRATION.md).

## Lineage

[Python HAiKU](https://github.com/ariannamethod/harmonix/tree/main/haiku) ·
[haiku.c](https://github.com/ariannamethod/haiku.c) ·
[haiku](https://github.com/ariannamethod/haiku) ·
[Klaus](https://github.com/ariannamethod/klaus.c) ·
[Yent AML](https://github.com/ariannamethod/yent.aml) ·
[Subjectivity](https://github.com/ariannamethod/subjectivity) ·
[Brodsky](https://github.com/ariannamethod/brodsky) ·
[Q / PostGPT](https://github.com/ariannamethod/q)

GNU GPL v3 — see [LICENSE](LICENSE).

*Built with constraint, powered by dissonance, coming home to AML.*
