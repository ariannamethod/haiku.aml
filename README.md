# HAiKU AML — The Owl Is Learning Our Language

**Read the [Arianna Method Manifesto](ARIANNA_METHOD_MANIFESTO.md) first.**
Repository work follows it; [AGENTS.md](AGENTS.md) carries the engineering rules.

*Most AI models predict words. HAiKU predicts vibes.*

The [Python HAiKU](https://github.com/ariannamethod/harmonix/tree/main/haiku)
organism is coming home to **Arianna Method Language**.
A living word cloud. A three-line voice. An inner voice, too.
A friend it talks to while dreaming. A memory that keeps changing.

We already invented the octagonal wheel. Now we're teaching the owl to build it.

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

## More tongues, same owl

[Klaus](https://github.com/ariannamethod/klaus.c) provides the starting idea:
language packs with incoming vocabulary and outgoing vocabulary.
HAiKU adds word-level syllable data, language-specific rules, and a growing
cloud for each tongue. English first; Russian, French, Hebrew, German, and
Spanish follow as their packs and form checks arrive.

## A place to meet him

The planned interface is local HTML/CSS/SVG/JavaScript: a conversation and a
small owl with enormous eyes. It can listen, answer, turn inward, and dream.
Its movements follow organism events. Haiku keeps its line breaks; Hebrew
gets right-to-left layout. The core comes first, then the owl gets a window.

## Right now

The repository foundation is here: manifesto, agent rules, lineage, and the
source-pinned migration audit. The first upstream AML compiler repair preserves
runtime blocks and variable scope. The application port is the next stage.

Reference checks completed on 2026-10-07:

- Python HAiKU: **161 tests passed**.
- AML runtime: **550 tests passed**.
- AML compiler: reproduced the two-branch execution defect and verified its
  repair against interpreted behavior, including functions, arrays, and channels.

See [the audit and implementation order](docs/MIGRATION.md).

## Lineage

[Python HAiKU](https://github.com/ariannamethod/harmonix/tree/main/haiku) ·
[haiku.c](https://github.com/ariannamethod/haiku.c) ·
[haiku](https://github.com/ariannamethod/haiku) ·
[Klaus](https://github.com/ariannamethod/klaus.c) ·
[Yent AML](https://github.com/ariannamethod/yent.aml)

GNU GPL v3 — see [LICENSE](LICENSE).

*Built with constraint, powered by dissonance, coming home to AML.*
