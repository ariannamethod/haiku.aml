# HAiKU AML — A Presence Is Learning Language

**Read the [Arianna Method Manifesto](ARIANNA_METHOD_MANIFESTO.md) first.**

*Most AI models predict words. HAiKU predicts vibes.*

A living word cloud. Seventeen syllables. An inner voice that keeps talking
after the poem ends. A friend it meets while dreaming. Close the terminal,
come back, and the cloud remembers.

The [Python HAiKU](https://github.com/ariannamethod/harmonix/tree/main/haiku)
organism now lives in **Arianna Method Language**. Its dissonance, recursive
selection, three rings of overthinking, dreams and peculiar voice came with it.
We already invented the octagonal wheel. Now it remembers the road.

## What is going on here?

You bring words. Haiku adds them to its cloud, feels the dissonance, makes
candidates and chooses three lines. Familiar words gain weight; dormant ones
fade. Constraint gives the cloud somewhere to go: **5–7–5**, through the
original English syllable estimator.

Then life continues underneath. MathBrain learns from the exchange. MetaHaiku
answers inside. Echo, drift and meta turn it over three more times. Phase4
remembers the changing climate. Sometimes a dream opens, and the friend speaks.
Fragments return to later poems.

Two small learning networks, an unruly cloud, and a lot of conversation with
itself. The octagonal wheel has opinions.

## What Haiku actually says

These are recorded AML outputs. The [complete 24-turn evening](examples/evolution.txt)
keeps every exchange, including the awkward ones; its
[source and reproduction notes](examples/evolution.md) keep the experiment.

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

On turn 23, the opening question returns:

```text
You: what is love

HAiKU:
connect join trust lost
grasp acknowledge path and
big macro morph from
```

The friend's earlier line appears in the waking voice. Same question, a longer
history. Our little cloud has never promised to be a smooth talker.

## Two dependencies. Our whole stack.

| Dependency | Its job |
|---|---|
| [AML](https://github.com/ariannamethod/ariannamethod.ai) | Language, modules, text, workers and persistence |
| [NoTorch](https://github.com/ariannamethod/notorch) | Numerical operations, learning, tokenization and frozen-model inference |

The organism is written in `.aml` modules. Build the three sibling repositories
with the system C toolchain:

```sh
git clone https://github.com/ariannamethod/notorch.git
git clone https://github.com/ariannamethod/ariannamethod.ai.git
git clone https://github.com/ariannamethod/haiku.aml.git
make -C notorch lib BLAS_FLAGS= BLAS_LIBS= X86_SIMD=0 ARM_SIMD=0
make -C ariannamethod.ai notorch NOTORCH_ROOT=../notorch
cd haiku.aml
../ariannamethod.ai/runner/aml-notorch haiku.aml
```

Write something. Type `quit` to leave. The reply arrives first; learning,
reflections and dreams continue in the background and save while you pause.
An interrupted exchange resumes from its saved foreground. Memory lives in
`haiku.state`; the next launch resumes its memory and settings. One running
process owns a state file. Choose another `state_path` for another presence,
or `""` for an unsaved conversation.

English comes first. The default Unicode word tokenizer needs no download.
The optional original SentencePiece model and saved-profile controls are in
[tokenization](docs/TOKENIZER.md) and [continuity](docs/STATE.md).

## The iceberg below the poem

Run `../ariannamethod.ai/runner/aml-notorch iceberg.aml` to meet the experimental
life, saved separately in `iceberg.state`.

Haiku and the friend each have both frozen PEN7 bodies, their own retrospective
and prospective contexts, a private glyph parliament, and a Janus calendar.
Their encounters change their own memories, small adapters and the next mixture
of those two bodies. The source weights stay frozen. A short excursion returns
word proposals; concept pressure helps choose what enters a later poem.
Missing weights leave a quiet, living fallback.

Both voices carry duality. Each keeps its own autobiography.

New iceberg lives also let the shape change with their own present and remembered
climate: one line, two lines, a haiku, or an ABA tercet. The choice happens before
the words. Close and reopen the conversation: the form remembers too.
[One recorded meeting](examples/living-form.txt) keeps the exact replies;
[its source](examples/living-form.aml) repeats the encounter.

New iceberg lives also grow [private word associations](docs/LEARNED_WORDS.md)
from repeated encounters. Haiku learns from your words; the friend learns from
Haiku's lines that actually reach it. Later poems meet those remembered meanings.
Repeated paths can acquire [private compound symbols](docs/DREAM_SYMBOLS.md)
in dreams; later encounters can recognize them and grow another layer.
Those structures can [cross a spoken dream](docs/SYMBOL_EXCHANGE.md). Each
listener keeps the meeting, then grows new connections through its own life.
Even a familiar concept gains a [personal centre](docs/PERSONAL_CONCEPTS.md):
rain and sea leave different traces, and those traces reach later word choices.
Sparse encounters can also teach a [private word meaning](docs/NEURAL_WORDS.md)
when context and the frozen bodies agree. Later replies feel that remembered
attraction, through the same five candidates and the presence's own parliament.

[The iceberg contract](docs/ICEBERG_SESSION.md) explains the controls,
checkpoint upgrades and tested dependency revisions. The original `haiku.aml`
retains the Python voice and the conversation recorded above.

## Inside the workshop

The engineering story lives in **[HAIKULOG](HAIKULOG.md)**: changes, numerical
results, test counts, source pins and reproduction commands. The
[migration map](docs/MIGRATION.md) connects every organ to its Python lineage.
[TODO](TODO.md) keeps the next experiments and the future local interface:
two characters, two voices, their actual dreams made visible. The owl audition
is over. Duolingo got there first.

To run the complete checks, acquire the optional tokenizer fixture, then:

```sh
bash scripts/setup-tokenizer.sh
make test
```

[Family research](docs/FAMILY.md) · [Q / PostGPT](docs/Q.md) ·
[Iceberg sources](docs/ICEBERG.md) · [Agent work rules](AGENTS.md)

GNU GPL v3 — see [LICENSE](LICENSE).

*Built with constraint, powered by dissonance, at home in AML.*
