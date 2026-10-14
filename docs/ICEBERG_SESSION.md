# Five organs, one remembered evening

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

[`iceberg.aml`](../iceberg.aml) is the experimental conversation that joins
the frozen lexical bodies and private parliaments to the complete AML Haiku.
The original `haiku.aml` remains the Python-lineage profile. Both entrypoints
use its existing Markov generator, form, RAE, learners, reflection and dreams.

```sh
../ariannamethod.ai/runner/aml-notorch iceberg.aml
```

Each accepted turn first produces the ordinary spoken haiku and its complete
inner life. Real encounters then enter the deeper organs. A bounded pair of
reviewed words can join the vocabulary for the next exchange. The response
already chosen stays fixed while its history grows underneath it.

## Shared bodies, private histories

NoTorch owns immutable PEN7 handles, native BPE, model arithmetic and
all-parameter interpolation. AML owns the handle lifetimes across workers.
Haiku owns contact, history, word admission and the moment a result becomes
part of its life. The two dependencies remain AML and NoTorch.

The launcher opens `weights/haiku1.gguf` and `weights/haiku2.gguf` independently.
Each presence can use both. Their initial parameter coefficient is **alpha=.5**,
where `alpha=1` selects A and `alpha=0` selects B. The selected coefficient is
fixed for one pass. Each presence persists its own preference; an A-only pass
records effective alpha=1 and a B-only pass records effective alpha=0.

| Owner | Retained lexical contexts | Contact organ |
|---|---|---|
| Haiku | Retrospective and prospective, at most 64 token IDs each | Private experts, immune memories, rewards, bindings and prototype cache |
| Friend | Retrospective and prospective, at most 64 token IDs each | Another complete private parliament and cache |

Haiku alternates directions once per accepted turn. A retrospective pass
revisits the preceding inner haiku; the first turn uses the current input.
A prospective pass starts from the current human text. The optional graft
adds the current inner voice and the last actually delivered friend utterance.
The friend takes one pass when a real dream dialogue contains a friend line;
its directions alternate across dreams. Its base fragment is that line and
its optional graft is the preceding Haiku dream line.

This is a bounded text-context return. The lexical module scores its 48
reviewed whole words by their mean BPE logits, retains its two leading
proposals and returns its final 448-value contextual representation. Each
direction has its own retained context. Runtime forward scratch and immutable
handles stay outside conversation checkpoints. The [lexical source contract](LEXICAL_SOURCES.md)
records fragment budgets, source identities, failure outcomes and history rows.

Gamma season evolution, expectation debt, embedding-direction injection and
recursive dream-born glyphs are the following experiments in [TODO](../TODO.md).

## Events that actually happened

`src/iceberg_session.aml` publishes the following order after the ordinary
learning/reflection/dream tail:

1. The actual human input, with the exchange's observation, quality and
   completed ring coherences. This later human encounter can reward retained
   earlier parliamentary receipts.
2. The actual recent cloud triples, followed by the actual internal haiku.
3. Every newly recorded dream line in its original alternating order. The
   speaker records its own event; the other presence receives a derived event
   with the speaker and original event still attached.
4. Haiku's lexical excursion and its returned model proposal; then bounded
   vocabulary admission. The friend's own excursion follows when it spoke in
   this dream.

Human, cloud, self, peer, dream and model encounters keep separate source
clocks. A returned model proposal preserves its parent and original source.
Only a native human encounter can be a parliamentary reward witness. The
friend acquires no direct human history during a waking turn without a dream.

The [contact bridge](PARLIAMENT_CONTACT.md) gives these events the shared
88 concept coordinates. Named aliases and reviewed private bindings preserve
the original words. Unknown words remain unbound. Four named prototypes—water,
tree, light and music—provide the first measured representation contribution.
Their cache belongs to the receiving presence and names the exact model pair,
tokenizer and effective coefficient. A changed identity or coefficient rebuilds
the cache through the same model forward.

## How words reach the next poem

Cloud membership and generator membership are separate in the original
organism. Admission therefore adds an eligible reviewed word to the cloud,
generator vocabulary and syllable cache. It creates no human count or fabricated
trigram. The original transitions, random draws, form completion and RAE then
determine how the expanded vocabulary participates in later poems.

The parliament contributes bounded concept pressure to the candidate's model
score. Stable ties preserve reviewed pool order. At most two words absent from
the generator vocabulary are admitted per turn. The last 32 admissions retain
word, language, owner, both source hashes, tokenizer hash, source fragment,
surface, turn, source/event, direction, effective alpha, syllables and both
the base and pressure-adjusted score. Syllables follow Haiku's existing English
estimator, including its original spellings and counting behavior.

The latest exchange journal records the completed cloud size after admission.
Its frozen learning features and original human observation retain their values.

## One publication, one checkpoint

The experimental checkpoint has **185 flat fields**:

| Component | Fields |
|---|---:|
| Existing version-3 voice | 55 |
| Haiku and friend lexical owners | 18 each |
| Haiku and friend contact owners | 42 each |
| Envelope, component key lists, settings and admission ledger | 10 |

The existing voice's 55-field schema is unchanged. `iceberg.state` is a separate
complete checkpoint. It holds all four contexts, both parliaments, identities,
prototype caches and admission history together with the original inner life.

The worker completion carries the entire exact predecessor, completed life,
frozen foreground experience and provider request. Publication compares every
predecessor leaf using `record_equal`; equal turn numbers cannot substitute
one presence's private history for another. A rejected completion leaves the
published owner intact. The launcher performs **join → validate → save → swap
→ display** on the whole life. Immutable handles are retained until their
workers finish.

The full module uses AML's bounded 8192-line import capacity, 16-argument
functions and 512-field flat records. Its complete predecessor and successor
fit in one transient result; the saved schema remains 185 fields.

## Quiet absence and controls

New-life settings at the top of `iceberg.aml` are saved with the checkpoint:

| Setting | Default | Effect |
|---|---:|---|
| `deeper_life` | 1 | Enables contact and lexical work |
| `lexical_bodies` | 1 | Enables optional PEN7 excursions |
| `admission_budget` | 2 | Allows zero, one or two future vocabulary admissions |
| `self_injection` | 1 | Adds explicit self/peer fragments to each selected context |
| `owned_tail` | 1 | Selects worker scheduling; zero runs the same tail serially |

An existing checkpoint retains its first four settings. The scheduling choice
can change without changing the complete result. For a new control condition,
choose a new state path. Set `state_path=""` for an unsaved conversation.

Empty paths create explicit unavailable handles. Missing or invalid files
remain inspectable through `model_ready` / `model_error`; they add no fallback
lines to poems. With zero available bodies, lexical owners and the admission
ledger stay unchanged while the enabled weightless contact field can continue.
The original voice retains its exact weightless continuation and draws.

An available source must match its recorded byte identity and tokenizer.
A previously available source may disappear without erasing its contexts or
history; it can return with the same identity at the next accepted boundary.
Provider handles are opened once per launcher lifetime, so a path change takes
effect on a later launch. There is no partial source change within a worker.

## Reproduction

Tested upstream trees are published in this dependency order:

| Dependency | Pull request | Revision |
|---|---|---|
| NoTorch immutable PEN7 | [#170](https://github.com/ariannamethod/notorch/pull/170) | `6f95f8ab1d1f0f5b2dd0e21a4eac0368aa0a2651` |
| AML v5.12 model values and capacity | [#38](https://github.com/ariannamethod/ariannamethod.ai/pull/38) | `dcfd4395582c5897a8134e420068f3233c2c2066` |

Build NoTorch, then AML, before running this profile. Both published trees
match the locally tested contents exactly.

```sh
make test-lexical-sources test-parliament-contact test-iceberg
make test-owned-tail
```

`tests/iceberg_life.aml` compares actual foreground state, draws and output
against the stable organism with the deeper organs disabled and with both
models absent. It also checks peer isolation, detached reads, full persistence
and serial/worker continuation.

`tests/iceberg_models.aml` exercises both real GGUF files, two deliberately
opened dream dialogues, all four contexts, explicit word admission, complete
serial/worker equality and save/restart continuation. The shell gate repeats
both fixtures through the interpreter and compiled scalar executable, compares
checkpoint bytes, and checks that both asset hashes remain unchanged.

`tests/iceberg_publication.c` rejects a completion from another private life,
altered provider or experience receipts, malformed envelopes and duplicate
publication. Every rejection preserves all 185 live fields and the prior file.
It also checks detached results and failure before the save/swap boundary.

The separate lexical and contact gates isolate source availability, context
interventions, actual representation pressure, source ancestry, delayed reward
and rejected updates. The ordinary `examples/evolution.aml` recording remains
the stable voice's independent 24-turn regression.
