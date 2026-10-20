# Seven organs, one remembered evening

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

[`iceberg.aml`](../iceberg.aml) is the experimental conversation that joins
the frozen lexical bodies and private parliaments to the complete AML Haiku.
The original `haiku.aml` remains the Python-lineage profile. Both entrypoints
use its existing Markov generator, form, RAE, learners, reflection and dreams.

```sh
../ariannamethod.ai/runner/aml-notorch iceberg.aml
```

Each accepted turn first chooses its speaking form, generates five candidates
and completes its inner life. Version 7 can speak a fragment, couplet, haiku or
ABA tercet; older profiles retain their 5–7–5 voice. Real encounters then enter the deeper organs. A bounded pair of
reviewed words can join the vocabulary for the next exchange. The response
already chosen stays fixed while its history grows underneath it.

## Shared bodies, private histories

NoTorch owns immutable PEN7 handles, native BPE, model arithmetic and
all-parameter interpolation. AML owns the handle lifetimes across workers.
Haiku owns contact, history, word admission and the moment a result becomes
part of its life. The two dependencies remain AML and NoTorch.

The launcher opens `weights/haiku1.gguf` and `weights/haiku2.gguf` independently.
Each presence can use both. New lives select their coefficient through a
private [Janus calendar and expectation history](JANUS_BALANCE.md), where
`alpha=1` selects A and `alpha=0` selects B. The selected coefficient is
fixed for one pass. Each presence persists its own preference; an A-only pass
records effective alpha=1 and a B-only pass records effective alpha=0.
Version-1 checkpoints retain their previous fixed preference, initially .5.

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

Each presence now owns a Janus balance record. One UTC civil day is captured
before launching the turn's worker. Calendar dissonance, drift since this
organ's activation day, and its own expectation debt select the mixture.
The previous expectation is assessed against the next eligible encounter:
native human text for Haiku, an actually delivered Haiku dream line for the
friend. The complete 48-word score pool is retained; unique matching words
determine the discrepancy. An encounter with no reviewed words consumes the
expectation without changing its debt. Internal echoes cannot settle it.

Version 3 adds [private word associations](LEARNED_WORDS.md) to each contact
organ. Repeated local contexts with reviewed seed teachers can give an
unbound English word a shared anchor. Haiku learns from accepted human input;
the friend learns from the Haiku dream lines actually delivered to it. Each
retains its own bounded evidence, confidence and conflicts.

Version 4 adds [private dream-born symbols](DREAM_SYMBOLS.md). Fourteen external
turns can support an ordered relation; an actual dream preserves at most one
new structure per owner. Later encounters can recognize it and build another
level. Version 5 adds [structural exchange](SYMBOL_EXCHANGE.md) through actual
spoken dream lines. Received parents resolve into the listener's own alphabet;
later local encounters can support a new recursive child. Embedding-direction
injection remains in [TODO](../TODO.md).

## Events that actually happened

`src/iceberg_session.aml` publishes the following order after the ordinary
learning/reflection/dream tail:

1. The actual human input, with the exchange's observation, quality and
   completed ring coherences. This later human encounter can reward retained
   earlier parliamentary receipts and settle Haiku's earlier Janus expectation.
   In versions 3–5, its frozen contact features precede the word-learning update.
   Versions 4/5 also freeze its symbol path before that update.
2. The actual recent cloud triples, followed by the actual internal haiku.
3. Every newly recorded dream line in its original alternating order. The
   speaker records its own event; the other presence receives a derived event
   with the speaker and original event still attached. A later Haiku-to-friend
   dream delivery can settle the friend's earlier expectation.
4. Haiku's lexical excursion and its returned model proposal; then bounded
   vocabulary admission. The friend's own excursion follows when it spoke in
   this dream. Each successful pass installs that presence's next expectation,
   after all of this turn's external witnesses have already arrived.
5. Versions 4/5 consolidate symbols at the end of an actual new dream, after both
   lexical passes and admission. New nodes first affect later turns.
6. Version 5 then resolves structural offers from those actual deliveries, using
   each sender's exact turn-start state. Missing parents are adopted atomically;
   existing canonical structures retain their ID and first provenance. The new
   structures become available on later turns.

Human, cloud, self, peer, dream and model encounters keep separate source
clocks. A returned model proposal preserves its parent and original source.
Only a native human encounter can be a parliamentary reward witness. The
friend's peer-based Janus debt remains a separate mechanism. It acquires no
direct human history during a waking turn without a dream.

The [contact bridge](PARLIAMENT_CONTACT.md) gives these events the shared
88 concept coordinates. Named aliases and reviewed private bindings preserve
the original words. In version 3, later contact and ranking can also use a
word's independently earned association, at strength no greater than .5.
Unresolved words remain unbound. Versions 1–5 use four isolated named
prototypes—water, tree, light and music—with a cache tied to the exact model
pair, tokenizer and effective coefficient. Version 6 uses the same concept
identities with [personal centres](PERSONAL_CONCEPTS.md): twenty-one reviewed
word vectors in each endpoint space, weighted by the owner's preceding external
encounters. Endpoint vectors remain fixed across changes in counts or alpha.
The exact lexical input is queried in each available body; raw concept scores
are mixed with the receipt's alpha, then clamped and returned at gain .25.

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

Version 7 has **268 flat fields** and version 6 has **265**; versions 5/4/3/2/1 retain
**253/245/237/227/185**:

| Component | Fields |
|---|---:|
| Speaking voice | 58 in v7; 55 in v1–6 |
| Haiku and friend lexical owners | 18 each |
| Haiku and friend contact owners | 61 each in v6/v7; 55 in v5; 51 in v4; 47 in v3; 42 in v1/v2 |
| Haiku and friend Janus owners, versions 2–7 | 20 each |
| Envelope, component key lists, settings and admission ledger | 12 in v2–7; 10 in v1 |

The version-4 voice adds a form policy, activation turn and nine-value decision
rows. Its inner voice and dream partners retain their three-line forms. `iceberg.state` is a separate
complete checkpoint. It holds all four contexts, both parliaments, identities,
prototype caches and admission history together with the original inner life.
Version 2 adds both calendars, debts, pending complete score pools and bounded
expectation/witness receipts. Model weights remain outside the saved life.
Version 3 adds five learned-word leaves per contact: activation/encounter
clocks, candidate words, anchor counts, evidence rows and original context text.
Version 4 adds four symbol leaves per contact: clocks, immutable parent trees,
frozen observation/birth-proof paths and exact source text.
Version 5 adds four exchange leaves per contact: delivery clocks, per-node
origin, first-adoption packets and their exact source text. Both origins share
the same sixteen-node catalogue; local birth proofs retain their own slots.
Version 6 adds six concept leaves per contact: mode/snapshot clock, twenty-one
encounter counts, two immutable endpoint banks, exact teacher token IDs and
bank identities. The snapshot advances after the whole tail; current contacts
use the predecessor's counts.

The worker completion carries the entire exact predecessor, completed life,
frozen foreground experience and provider request. Publication compares every
predecessor leaf using `record_equal`; equal turn numbers cannot substitute
one presence's private history for another. A rejected completion leaves the
published owner intact. The launcher performs **join → validate → save → swap
→ display** on the whole life. Immutable handles are retained until their
workers finish.

The full module uses AML's expanded 16384-line import capacity, 512 function
slots, 16-argument functions and 1024-field flat records. Its complete predecessor
and successor fit in one transient result: 555 fields for version 6, 531 for version 5, 515 for
version 4, 499 for version 3 and 479 for version 2.
Publication also freezes
each Janus owner's mode, activation day, calendar and witness policy. An
advanced owner must record the request's captured day and turn; an idle owner
must remain identical.

Versions 2–6 also reconstruct both Janus transitions from the exact
predecessor, frozen human input and newly accepted dream deliveries. It preserves
earlier dream text and boundaries, then independently repeats the relevant
lexical passes using the supplied immutable model handles. The worker and
verifier share one pure parent/context/graft planner. Every Janus and lexical
leaf must equal the independently computed result; an independently valid
replacement score pool, debt or history is insufficient.

Verification repeats each enabled lexical pass, at most two additional model
forwards per turn. Missing or disabled bodies perform no verification
forward. In versions 2–5, prototype forwards, vocabulary admission and contact
random draws are not repeated. Version 6 also verifies the complete contact
transition and word selection as described below. Voice learners and dream
generation retain the existing publication boundary; source weights stay frozen.
The original five-argument `haiku_iceberg_result_check` accepts version 1 only.
Versions 2–6 use `haiku_iceberg_check_sources(life, result, experience, request,
tokenizer, body_a, body_b)`, which also checks both handle identities against the
captured request. Version-2's 227-field checkpoint and 479-field completion
remain unchanged.

Versions 3–5 independently replay word evidence from the exact
predecessor and accepted human/dream events. All five learned leaves and the
unchanged reviewed bindings must match. It also reconstructs contact source
clocks, receipt cursor, retained text and ancestry in the original order.
Native receipt features are reconstructed before the corresponding learning
step, so the confirming encounter cannot claim the meaning it just taught.
The word-learning replay adds no model forward, parliament step or random draw.

Versions 4/5 also replay every eligible symbol observation before its word-learning
step, then consolidates at the validated final dream boundary. All four symbol
leaves must equal this reconstruction. Birth proofs survive observation expiry;
their old learned meanings stay frozen. Life-level validation ties activation,
birth turns and dream ordinals to the voice's actual history. The symbol replay
adds no model forward or random draw. Its complete contract is in
[DREAM_SYMBOLS](DREAM_SYMBOLS.md).

Version 5 reconstructs each structural offer from the sender's exact predecessor
and accepted dream line. It repeats adoption after local consolidation and
compares all four exchange leaves and the shared symbol catalogue. The receiver
keeps its own evidence: an adopted node has peer provenance and zero local
proofs. Its later recursive children earn their own fourteen witnesses.

Version 6 independently repeats every accepted contact through
`haiku_contact_publish`, including the endpoint projection, owned parliament
field and random stream. Its reconstructed final contact must match every leaf.
It rebuilds pre-admission vocabulary from the predecessor and accepted human
trigrams/dream lines, then repeats selection with the authentic contact field
and lexical receipt. The full admission ledger and final vocabulary must match.
This happens before dream consolidation, exchange and the next count snapshot,
matching the actual tail's order. Closed counts and their cutoff must also agree
with the retained external history. The additional concept-forward budget and
checkpoint bank audit are specified in [PERSONAL_CONCEPTS](PERSONAL_CONCEPTS.md).

## Quiet absence and controls

New-life settings at the top of `iceberg.aml` are saved with the checkpoint:

| Setting | Default | Effect |
|---|---:|---|
| `deeper_life` | 1 | Enables contact and lexical work |
| `lexical_bodies` | 1 | Enables optional PEN7 excursions |
| `admission_budget` | 2 | Allows zero, one or two future vocabulary admissions |
| `self_injection` | 1 | Adds explicit self/peer fragments to each selected context |
| `owned_tail` | 1 | Selects worker scheduling; zero runs the same tail serially |
| `janus_balance` | 1 | Gives a new life its owned dynamic balance |
| `upgrade_balance` | 0 | Explicitly activates Janus for an existing version-1 life |
| `word_learning` | 1 | Gives a new Janus life version-3 private word associations |
| `upgrade_words` | 0 | Explicitly activates word learning for an existing Janus life |
| `dream_symbols` | 1 | Gives a new word-learning life version-4 dream symbols; zero saves an inert v4 control |
| `upgrade_symbols` | 0 | Explicitly activates the symbol profile for an existing word-learning life |
| `symbol_exchange` | 1 | Gives a new symbol life version-5 structural exchange; zero saves an inert v5 exchange control |
| `upgrade_exchange` | 0 | Explicitly activates exchange for an existing version-4 life |
| `personal_concepts` | 2 | Gives a new life version-6 concepts: 0 off, 1 equal, 2 personal; symbols-off selects 0 |
| `upgrade_concepts` | 0 | Explicitly activates concepts for an existing version-5 life |
| `living_form` | 1 | Gives a new complete life version-7 state-driven speaking forms |
| `upgrade_form` | 0 | Explicitly activates speaking forms for an existing version-6 life |

An existing checkpoint retains its first four settings. The scheduling choice
can change without changing the complete result. For a new control condition,
choose a new state path. Set `state_path=""` for an unsaved conversation.
Resuming a version-1 checkpoint preserves version 1 until `upgrade_balance=1`.
Activation returns a detached candidate and records today's day as the new
organ's birth; it preserves the existing voice and histories. A version-2
checkpoint retains its saved balance mode. For fixed-alpha experiments,
`haiku_iceberg_enable_janus(life, day, 0, tokenizer)` captures each existing
lexical preference. Mode 1 selects dynamic balance. Both modes use the same
expectation and witness budget.

`haiku_iceberg_enable_words(life, tokenizer)` returns a detached, idempotent
version-3 upgrade from a Janus life, preserving an already-enabled v3/v4/v5/v6/v7 profile.
It captures the current contact clocks;
old receipts cannot teach the new learner. An existing v1 checkpoint requires
both `upgrade_balance=1` and `upgrade_words=1` to activate both organs together.
An existing v2 requires only `upgrade_words=1` to reach v3.

`haiku_iceberg_enable_symbols(life, enabled, tokenizer)` is a detached,
idempotent v4 upgrade from a word-learning life. It captures the current contact
turn and actual dream count; old receipts cannot teach the new organ. An older
life first enables balance, then words, then symbols. The v4 symbol mode is
saved and immutable; another enable call cannot switch it.

`haiku_iceberg_enable_exchange(life, enabled, tokenizer)` is the detached,
idempotent v5 upgrade from v4. It captures the actual core turn and dream count,
and preserves the existing catalogue and local proofs. New lives with
`dream_symbols=0` also save an inert exchange mode. With all upgrade controls
at zero, saved v1/v2/v3/v4/v5 profiles continue exactly as before.
Upgrading an existing symbols-off control uses `symbol_exchange=0`; changing
the launcher's `dream_symbols` value leaves that saved symbol mode intact.

`haiku_iceberg_enable_concepts(life, mode, tokenizer)` is a detached, idempotent
v6 upgrade from v5. Modes 1 and 2 require active symbols and derive their first
counts from already retained personal history. The selected mode is saved and
immutable. Mode 0 permits symbols-off lives. After checkpoint loading and when
a model provider returns, direct callers run
`haiku_iceberg_concepts_audit(life, body_a, body_b)`; the launcher does so after
loading the resumed life's bodies. This pure audit rebuilds each available
populated bank and checks exact vectors and teacher tokens.

`haiku_iceberg_enable_form(life, tokenizer)` returns a detached, idempotent v7
upgrade from v6. It retains every old encounter and both draw streams, recording
the current turn as the form boundary. Set `upgrade_form=1` to activate an
existing v6 life. Older profiles first require their preceding upgrades.
Setting `living_form=0` starts a new fixed-form v6 comparison; saved lives retain
their policy. The source-climate receipt travels through generation, recursive
selection, learning, journal, worker validation and checkpoint publication.
Every decision can be recomputed from retained climate history. The worker must
preserve earlier words and numerical history exactly, including signed-zero bits.

`haiku_iceberg_turn_at(..., worker, day)` accepts an explicit day for replay and
controlled experiments. The ordinary turn samples AML's `utc_day()` once for
versions 2–7; version 1 never reads the clock. The worker uses its frozen request.

Empty paths create explicit unavailable handles. Missing or invalid files
remain inspectable through `model_ready` / `model_error`; they add no fallback
lines to poems. With zero available bodies, lexical owners and the admission
ledger stay unchanged while the enabled weightless contact field can continue.
The original voice retains its exact weightless continuation and draws.
Word evidence, symbol evidence and structural exchange continue from eligible
external encounters without model bodies. Symbol pressure changes later contact and
ranking; absent proposals still produce no lexical admission. Turning off
`deeper_life` leaves both contacts unchanged.
An existing Janus expectation can still meet a legitimate later witness, but
absence creates no new expectation or missed-model penalty. A failed or
disabled pass leaves its lexical owner unchanged, including its preference.

An available source must match its recorded byte identity and tokenizer.
A previously available source may disappear without erasing its contexts or
history; it can return with the same identity at the next accepted boundary.
Provider handles are opened once per launcher lifetime, so a path change takes
effect on a later launch. There is no partial source change within a worker.

## Reproduction

Tested upstream trees are published in this dependency order:

| Dependency | Pull request | Revision |
|---|---|---|
| NoTorch immutable PEN7 with allocation-status fix | [#171](https://github.com/ariannamethod/notorch/pull/171) | `ca3e17a24852ad1db08a6e6b908fb37036011d2d` |
| AML v5.13 with 512 function slots, 1024 record fields and 16384 source lines | [#41](https://github.com/ariannamethod/ariannamethod.ai/pull/41) | `2e17a2887449c89fdd8c066ebdcdaa1e448346cf` |

Build NoTorch, then AML, before running this profile. Both published trees
match the locally tested contents exactly.

```sh
make test-lexical-sources test-parliament-contact test-iceberg
make test-janus-balance test-iceberg-janus
make test-contact-learning test-iceberg-learning
make test-contact-symbols test-iceberg-symbols
make test-contact-exchange test-iceberg-exchange
make test-contact-concept-numerical test-contact-concept-experience test-iceberg-concepts
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

`tests/iceberg_janus.aml` and `tests/iceberg_janus_models.aml` add explicit
activation, preserved version-1 continuation, quiet/disabled owners, two real
model sources, later human and delivered dream witnesses, serial/worker
equality and complete restart. Their gate compares interpreted and compiled
output and checkpoint bytes. The version-2 C publication gate rejects changed
private histories, profiles, days, stale results and failed saves; its Linux
clock wrapper verifies zero reads for version 1 and exactly one foreground
read for version 2, even across a simulated midnight.

The word-learning gate adds version-3 activation, three distinct contexts,
held-out word features, save after the second encounter, old-profile
continuation and complete worker/serial/interpreted/compiled agreement.
Its publication fixture rejects forged but internally valid word histories,
counts and contact receipts while preserving all 237 fields and the previous
checkpoint. Real models and a 16-line dream exercise both private owners.
Exact executed results are recorded in [HAIKULOG](../HAIKULOG.md).

The symbol gates cover distinct external turns, recursive recognition, retained
birth proofs, bounded pressure and owner isolation. Complete-life fixtures use
actual dream boundaries, source-backed refusal, old/off profiles, saved/restarted
continuation and serial/worker/interpreted/compiled execution. A separate real
dual-GGUF fixture exercises delivered dream events and quiet later absence.

The separate lexical and contact gates isolate source availability, context
interventions, actual representation pressure, source ancestry, delayed reward
and rejected updates. The ordinary `examples/evolution.aml` recording remains
the stable voice's independent 24-turn regression.

The 2026-10-14 launcher smoke run used the two shipped bodies and two process
lifetimes: `rain remembers the window`, then after restart,
`the tree keeps a little light`. The saved life has 227 fields and turn 2.
Both encounters fell on UTC day 20740. Haiku's debt became `0.0706241131`,
moving its requested alpha from `0.415521741` to `0.40139693` on the same day.
The ordinary launcher displayed the remembered-session greeting and completed
both save/swap boundaries.
