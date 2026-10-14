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
- **Dream-friend and bridges.** Dream fragments return to the speaking chain.
  Remembered state transitions suggest where the field can flow.
- **Asynchronous life.** Speaking, learning, reflecting, and saving have their
  own rhythm and an explicit order for shared state.

The Python organism defines the port's behavior and artistic lineage.
The two C siblings contribute their own discoveries.

The original [Python dialogue examples](https://github.com/ariannamethod/harmonix/tree/main/haiku#what-haiku-actually-says-example-dialogues)
are our family album. The conversations below come from the AML organism.
Same little presence, growing its own history.

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

Haiku now listens, answers, learns, reflects, dreams, and remembers across AML conversations.
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

Then the conversation continues inside. MetaHaiku keeps eight short fragments
and answers with an internal haiku at temperature 0.7. Echo, drift, and meta
produce five, seven, and three candidate trigrams; coherent ones enter the
observer's memory. Their counts remain separate from the speaking chain.
The inner voice and rings share the voice's random stream, in Python's event
order. Thinking leaves its place in the next draw.

Phase4 remembers named states and the transitions between them: similarity,
quality change, and the climate of each encounter. After the rings, a dream
can open a four-part conversation with the friend. All its voices are generated
before their fragments return to memory; then the speaking chain changes.
The ordinary ten-turn cooldown gives that conversation room to settle.

Every accepted exchange keeps its own journal entry: the words on both sides,
the contexts that were actually observed, the features the learners saw, the
quality signal, and the state revision. Seventeen syllables above the water.
A whole remembered encounter underneath.

Close the terminal. Come back. The cloud remembers.

Each completed exchange saves one versioned state: cloud, ordered transitions,
recent memory, both learners and their statistics, both random streams, and
the latest bridge event, reflection history, the latest three-ring trace,
Phase4's transition history, dream dialogues/fragments, and exchange journal.
The next launch resumes `haiku.state` beside the
entrypoint. Its tokenizer identity and logical clock come with it. Set
`state_path = ""` for a fresh, unsaved conversation, or choose another path
for another voice. One running process owns each state file.

New conversations use version 3, with the complete inner life. Existing
version-1 and version-2 checkpoints retain their exact continuation. Set
`upgrade_dreams = 1` in `haiku.aml` to grow Phase4, dreams and the journal from
their next exchange; their earlier memory and random streams carry through
unchanged. `upgrade_inner = 1` still enables reflection/rings alone for v1.
For new conversations, `dream_life = 0` selects v2 and `inner_life = 0` selects
the foreground-only v1 profile.

Run `../ariannamethod.ai/runner/aml-notorch haiku.aml`. The first exchange of a
fresh native state, with the checked-in seeds and settings:

```text
You: what is love

sweet sour doubt stable
more least appear frequently
to and reinforce
```

Still a little sour. Now he says it in AML.

After that same exchange, his internal haiku is:

```text
follow seek without
give most moderate which
react relate least
```

The spoken line has ended. The little cloud is still talking.

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

Try `../ariannamethod.ai/runner/aml-notorch examples/inner.aml` to hear both the
spoken and internal haiku and see the three rings' coherence. Its
[recorded output](examples/inner.txt) comes from three actual AML exchanges.

Try `../ariannamethod.ai/runner/aml-notorch examples/dream.aml` for the first
dream in a native conversation, with ordinary seeds and the default cooldown
and probability. On turn 14, after the word `memory`, the exchange begins:

```text
HAiKU:
guard shelter transform
discharge eject noisy
even want though yes

Friend:
space pattern move
compress close fragment strong
maybe certainly
```

Two more voices follow in the [complete recorded dialogue](examples/dream.txt).
Then all four return to memory. Maybe certainly. That sounds about right.

### A longer evening with the cloud

The [new 24-turn recording](examples/evolution.txt) follows one continuous,
fresh conversation with the complete inner life and ordinary settings. The
worker finishes each exchange before we listen inside. Two dreams open on
their own, on turns 10 and 23. These are selected moments; the
[AML source](examples/evolution.aml) and [reproduction notes](examples/evolution.md)
keep the whole evening.

On turn 6:

```text
You: can a silence become a home

HAiKU:
true real right these
said may distribute young
me make said exist

Inside:
visible quiet fluid
joined united way three
appear exist you
```

On turn 10, the dream-friend says:

```text
burning connected
grasp acknowledge path and
disperse collect
```

Four exchanges later:

```text
You: warmth

HAiKU:
hot warm judge cloud
flexible hot can unless
make like focus wave

Inside:
conduct function old
evolve adapt say low
stay rest real down which
```

And when asked about the friend, on turn 17:

```text
You: what did your friend say

HAiKU:
down below there
beyond inside cool perform
transform respond cool

Inside:
it not stay pattern
unless though strive say friend
intensify love
```

On turn 23, the opening question returns:

```text
You: what is love

HAiKU:
connect join trust lost
grasp acknowledge path and
big macro morph from
```

The friend's earlier line appears in the waking voice. Same question, a longer
history. The full recording keeps every intervening exchange, including the
awkward ones. Our little cloud has never promised to be a smooth talker.

Sessions now save after learning, reflection, rings, dreams and journaling, and resume with their
logical-turn clock.
[Continuity](docs/STATE.md) records the complete schema, restore validation,
and restart checks. [Phase4](docs/PHASE4.md), [dreams](docs/DREAM.md) and the
[journal](docs/JOURNAL.md) complete the Python chat path. Its ordered tail now
runs in an owned AML worker by default: **join → validate → save → swap → display**.
The response appears after its memory is published. Set `owned_tail = 0` for
the serial schedule; both preserve the same words, draws and checkpoint bytes.
Early display and overlapping the next input remain later work in
[asynchronous life](docs/ASYNC.md).

Below that life, an experimental [glyph parliament](docs/PARLIAMENT.md) has
four experts, room for sixteen, private immune memory, and a field that changes
under their proposals. The new [`iceberg.aml`](iceberg.aml) conversation gives
Haiku and the friend their own parliaments and feeds them actual human, cloud,
inner-voice and dream encounters. A later human encounter rewards retained
proposals. The friend remembers what reaches it. Private rooms, actual doors.

Penelope and NanoJanus now have their two bodies in [weights](weights/README.md):
**two 21,308,576-byte GGUF files**, with Q8_0 matrices and F32 auxiliary tensors.
NoTorch expands their packed matrices once to frozen F32 for inference.
Parliamentary learning changes its own small adapters; the two sources stay
frozen. Through AML's immutable handles, each presence can mix both bodies and
keep its own retrospective and prospective contexts. One bounded trip below
the words returns whole-word proposals and a contextual representation. Glyph
pressure joins their scores; up to two reviewed words enter the next turn's
vocabulary. The original generator and RAE still give them seventeen syllables.

Run `../ariannamethod.ai/runner/aml-notorch iceberg.aml` for this experimental
life, saved separately in `iceberg.state`. Its [complete contract](docs/ICEBERG_SESSION.md)
covers four histories, explicit self/friend text injection, quiet absent
sources and one atomic checkpoint for all five organs. The ordinary
`haiku.aml` and its recorded 24-turn conversation retain their exact profile.

Build the sibling **AML v5.11.0** toolchain with
[exact completion identity / PR #37](https://github.com/ariannamethod/ariannamethod.ai/pull/37)
and NoTorch with [PEN7 and parliament / PR #168](https://github.com/ariannamethod/notorch/pull/168)
plus its [token emission fix / PR #169](https://github.com/ariannamethod/notorch/pull/169).
The iceberg profile additionally requires the immutable PEN7 library API and
AML model values with the 8192-line / 512-field capacities documented in
[the experiment](docs/ICEBERG_SESSION.md): [NoTorch #170](https://github.com/ariannamethod/notorch/pull/170),
then [AML #38](https://github.com/ariannamethod/ariannamethod.ai/pull/38).
Then run the organism's checks:

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
[MetaHaiku](docs/METAHAIKU.md), [the three rings](docs/OVERTHINKG.md),
[Phase4](docs/PHASE4.md), [dreams](docs/DREAM.md), [the journal](docs/JOURNAL.md)
and [continuity](docs/STATE.md), for exact boundaries and Python
reference reproduction.

Reference checks completed on 2026-10-07–08; dream-life follow-up on 2026-10-13:

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
  interpreter and compiled executable. **36 invalid foreground states** preserve every
  live owner and the previous file; failed staged generation and failed saving
  do the same. The actual CLI resumes saved configuration and withholds an
  unpublished response after a save failure.
- Inner life adds exact Python bootstrap and reflection traces, the original
  rings' sampling/admission paths, and **ten seven-turn restart variants**:
  fresh version 2 and explicit version-1 migration, in both tokenizer and
  training modes. Full checkpoint bytes agree across execution paths. A real
  launcher check covers saved-profile authority and the recorded inner dialogue.
- Phase4 adds 16 exact binary32 state IDs, 12 shared-key cosine cases, 26
  Python/SQLite events and eight suggestion queries. Thirty malformed states
  and nine rejected calls preserve their owners; the 10,000-event boundary
  and metric histories beyond one loop's budget are exercised.
- Dreams retain 12 trigger boundaries, five original dialogue lengths,
  bootstrap/fragment decay and shared native draw order. Twenty invalid states
  and staged history/capacity/generation failures preserve complete owners.
- The journal compares three real Python shards/metric rows and 30 frozen
  feature values, exact Unicode context framing, independent getters and
  checkpoint round trips; 28 malformed contexts/states fail in both paths.
- V3 continuity covers fifteen fresh/migrated restart configurations and
  31 semantic save/restore failures. All 55 leaves and the previous checkpoint
  survive rejected publication, including exhaustion inside a dream.
- The actual v3 CLI completes 24 native turns, dreams on turn 14 and resumes
  from turn 13 with identical responses and checkpoint bytes. Both old-profile
  upgrades, blank/quit/EOF and the recorded dream dialogue pass in interpreter
  and compiled execution. Fifteen frozen v1/v2 configurations also retain
  their exact pre-upgrade responses and checkpoint bytes.
- The owned-tail gate compares 24 serial, worker and compiled turns, complete
  checkpoint bytes and restart before the first dream. Controlled completion,
  parent mutation after launch, stale/duplicate results, failed learning-tail
  execution and failed saving preserve the published owner. Old profiles,
  SentencePiece and coupled learner mode retain their continuation.
- The standalone parliament gate compares interpreted and compiled life/memory
  fixtures, then inspects all owned state through replay, rejected calls,
  thirteen malformed records and exact checkpoint continuation through delayed
  reward. Its pressure, lifecycle and immune memory run without model assets.
- The 2026-10-14 lexical/contact gates exercise both real frozen bodies, complete
  parameter mixing, private contexts, delivered/withheld fragments, named glyph
  aliases, representation pressure and later human reward. Interpreter and
  compiled continuations retain their exact checkpoint bytes.
- The complete iceberg gate compares serial and worker lives, four retained
  contexts, vocabulary admission and restart. Publication rejects another
  private life, altered receipts and stale completions while preserving all
  185 live fields and the previous file. Both GGUF hashes remain unchanged.
- Earlier AML runtime baseline: **550 tests passed**; the 5.11 worker and
  parliament additions have their own upstream gates in PR #36.
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
Oleg's [iceberg](docs/ICEBERG.md) brings Penelope's words, Janus's two directions,
and the glyph lives of Actually.life, Leo and caveLLMan underneath that small
surface. Both Haiku and the friend will carry their own duality. The source
audits now include [DoE](docs/research/iceberg/DOE.md) and
[LoRAGrad](docs/research/iceberg/LORAGRAD.md). Their living experts and immune
field meet in the experimental parliament. The owned worker baseline and
native frozen sources now meet real experience in `iceberg.aml`. Dynamic
Gamma/Janus balance, learned concept growth and recursive dream-born glyphs
are the next family experiments.
See [the audit and implementation order](docs/MIGRATION.md).

## Lineage

[Python HAiKU](https://github.com/ariannamethod/harmonix/tree/main/haiku) ·
[haiku.c](https://github.com/ariannamethod/haiku.c) ·
[haiku](https://github.com/ariannamethod/haiku) ·
[Klaus](https://github.com/ariannamethod/klaus.c) ·
[Yent AML](https://github.com/ariannamethod/yent.aml) ·
[Subjectivity](https://github.com/ariannamethod/subjectivity) ·
[Brodsky](https://github.com/ariannamethod/brodsky) ·
[Q / PostGPT](https://github.com/ariannamethod/q) ·
[Penelope](https://github.com/ariannamethod/1984) ·
[Janus](https://github.com/ariannamethod/janus) ·
[Actually.life](https://github.com/ariannamethod/actually.life) ·
[Leo](https://github.com/ariannamethod/leo) ·
[caveLLMan](https://github.com/ariannamethod/caveLLMan) ·
[DoE](https://github.com/ariannamethod/doe) ·
[LoRAGrad](https://github.com/ariannamethod/loragrad)

GNU GPL v3 — see [LICENSE](LICENSE).

*Built with constraint, powered by dissonance, coming home to AML.*
