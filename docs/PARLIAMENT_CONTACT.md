# The parliament hears the cloud

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

[`parliament_contact.aml`](../src/parliament_contact.aml) gives the experimental
parliament words, their actual climate, and a return into lexical ranking.
Haiku and the friend each own one contact record. Their shared vocabulary names
the same 88 coordinates; their pressure, experts, bindings, receipts and model
reference vectors remain private. The little cloud has acquired a basement
with two front doors.

The stable `haiku.aml` path keeps its established organs and draws. The opt-in
[iceberg session](../src/iceberg_session.aml) joins this contact organ to complete
foreground/inner/dream events and lexical admission.

## Names before numbers

The anchor order follows [Actually.life `l.c`, lines 23–50](https://github.com/ariannamethod/actually.life/blob/2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8/l.c),
at `2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8`. All 88 names are retained.
`haiku_contact_bindings` adds a reviewed subset of its direct English aliases,
plus explicit French and Russian words for water, tree, light and music.
The source and this repository use GPL-3.0. This AML implementation, Unicode
language namespaces, unknown-word handling and model return were added here
on 2026-10-14; the source's approximate character router is not ported.

| Text and language | Coordinate |
|---|---|
| `water`, `rain`, `river`, `ocean` — English | 0, water |
| `eau`, `pluie`, `rivière` — French | 0, water |
| `вода`, `дождь`, `река` — Russian | 0, water |
| `tree`, `forest`, `leaf` — English | 4, tree |
| `AI`, `computer`, `machine` — English | 15, AI |
| `music`, `song`, `melody` — English | 82, music |
| A word without a reviewed or available learned binding | Unbound; exact source text retained |

The existing Unicode regex tokenizer supplies lowercase words. A binding key
contains both language and spelling. For a concept seen `n` times in an event,
activation is `n / (1 + n)`: `rain ocean water` gives water `.75`. Nothing hashes
an unknown spelling into a concept. The original text survives in the receipt,
including Unicode and unbound words.

`haiku_contact_bind` can store an explicit reviewed private binding. It requires
a retained encounter containing that exact normalized word and language; its
history keeps the source, event, original text and chosen anchor. Existing seed
or private choices cannot be overwritten. This operation is explicit; model
similarity contributes to current features without silently naming an unknown.

Version-2 contact adds a private [learned word layer](LEARNED_WORDS.md).
Repeated accepted external contexts can earn a bounded association while the
reviewed table keeps priority. Its five additional leaves retain candidate
spellings, counts, activation clocks and the exact contexts behind them.
Contact features are frozen before the confirming encounter updates this layer;
later glyph extraction and ranking use the new strength. The 42-field v1
contact retains its exact previous profile.

The remaining feature columns keep the [parliament contract](PARLIAMENT.md):
actual dissonance, novelty, normalized arousal and entropy at 88–91, completed
echo/drift/meta coherence at 92–94, and the actual quality metric at 95.
Missing rings use `[0]` with availability bit zero. The contact constructor's
immune origin is the fixed neutral climate `[.5,.5,1,.5]`, three `.5` rings and
quality `.5`; its boundary is the opposite normalized direction.

## Four model reference words

`haiku_contact_enrich` uses PEN7's actual 448-value final hidden representation.
Its first bounded reference set is **water, tree, light, music**, at coordinates
0, 4, 6 and 82. Four one-word forwards build their reference vectors with the
same loaded provider, tokenizer and parameter mixture as the lexical receipt.
Positive cosine similarity becomes their activation; all other model-contributed
coordinates stay zero. NoTorch supplies dot products and the model forwards.

Each contact caches these 1,792 F32 values, the complete provider/tokenizer
identity and the exact mixture coefficient. A change rebuilds all four vectors.
The cache adds 7,168 numeric bytes per presence. Rebuilding the reference cache
alone neither observes an encounter nor changes parliamentary pressure.

Model activation adds at most `.25` to each named feature, clamped to `[0,1]`.
The event freezes provider identity, actual alpha, availability and gain.
One-source A means alpha `1`; one-source B means alpha `0`. The lexical owner's
requested balance remains its separate preference. A failed reference forward
returns an event with representation availability zero and no contribution;
it publishes no partial cache. An absent lexical provider produces no event.

`haiku_contact_rank` adds `.15 * strength * field[glyph]` to a bound candidate's
base score. Reviewed bindings have strength 1; available learned associations
have at most .5. Parliament bounds its field to `[-5,5]`, so corrections stay
within `[-.75,.75]` for reviewed words and `[-.375,.375]` for learned ones.
Unresolved words keep their base scores. The experimental session uses the
resulting order when admitting its bounded lexical proposals.

## A voice keeps its ancestry

Native events name an emitting owner, source, source-local event ID and turn.
`haiku_contact_derive` copies root ancestry and adds the immediate parent.
Friend delivery and a model's returned words therefore retain who began the
encounter, through each intermediate speaker. Only a native source-0 event
emitted by `human` can become a reward witness. Derivation into source 0 is
rejected. A model return rooted in human input stays a model return.

The experimental session supplies native human events from accepted input and
its completed climate. Cloud and inner events follow; native dream events name
their actual speaker. A friend event affects its own contact until an explicit
delivery enters the receiver. The receiving call never mutates the sender.

On each new native human event, `publish` rewards retained, unrewarded receipts
from strictly earlier turns using that event's saved features. The existing
parliament checks the actual witness and updates its private credits/adapters.
Internal, dream, friend and model events can add pressure but cannot perform
this reward step. Same-turn events wait for a later human encounter.

An explicit replay or an event no newer than its source clock is a complete
no-op, even when its original turn precedes the current clock. The same check
precedes model enrichment, so replay also leaves the reference cache untouched.

## Flat ownership and entry points

The version-1 contact checkpoint has **42 fields**: 32 namespaced parliamentary leaves,
format/version and the namespace list, binding map/history, two provenance
tables, and three prototype-cache leaves. The 32 receipt slots retain six
strings (emitter, exact text, language, root owner, parent owner, model identity)
and eight numbers (parent/root source and event, alpha, representation gain and
availability, complete availability mask). They rotate with parliament receipts.
Version 2 retains those leaves and adds the five learned-word leaves, for **47**.

| Entry point | Result |
|---|---|
| `haiku_contact_new(owner, seed)` | New independent contact |
| `haiku_contact_event(owner, text, language, climate, rings, quality, source, event, turn)` | Native flat event |
| `haiku_contact_derive(parent, owner, text, source, event, turn)` | Event retaining ancestry |
| `haiku_contact_enrich(contact, event, receipt, A, B)` | Enriched event clone; complete private cache may update |
| `haiku_contact_publish(contact, event)` | Atomic pressure/provenance/reward publication |
| `haiku_contact_rank(contact, words, scores, language)` | New bounded candidate scores |
| `haiku_contact_check`, `save`, `load` | Schema validation and exact continuation |

`p:owner`, `p:clock`, `p:revision` and `p:field` expose current identity and
pressure. Mutations validate a detached candidate before replacing the owner.
The enclosing iceberg checkpoint saves both owners with their lexical and
stable chat components at one completion boundary.

## Executed gates

Run `bash tests/run_parliament_contact.sh` with the matching AML/NoTorch build.
The suite compares interpreted and compiled scalar execution from an unrelated
working directory, then inspects every checkpoint leaf in C.

The life fixture runs actual foreground/tail exchanges, reads their real
climate/rings, obtains real inner haiku and dream dialogue, and measures field
and ranking changes. It checks language-qualified aliases, retained unknown
words, explicit private bindings, source ancestry, friend locality until
delivery, delayed human reward and exact replay.

The model fixture uses both checked-in GGUFs. With contact state and draws held
fixed, enabling the bounded representation changes features and pressure. It
also exercises A-only, B-only and absent models, effective endpoint alpha and
replayed returns. Changing alpha from `.5` to `.75` rebuilds the actual reference
vectors while every parliamentary leaf stays unchanged. The C gate saves the
actual model reference cache, resumes
through a later witness/Oja update, and rejects malformed ancestry, unavailable
sources, excessive gain and corrupted checkpoint leaves without changing the
live owner.

Python behavioral lineage was read at Harmonix
`abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`: `haiku/chat.py`, `metahaiku.py`,
`dream_haiku.py`, their tests and README. Its foreground, reflection and dream
order remain in the existing AML session; contact follows completed events.
