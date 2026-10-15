# A dream gives a familiar path its own name

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

The version-4 iceberg lets each presence remember an ordered relation as a
private compound symbol. Fourteen external turns can establish `fire → water`;
an actual dream can preserve it as `(fire+water)`. Later encounters can recognize
that whole structure, and fourteen later turns can support
`((fire+water)+tree)`. The second history begins after the first symbol exists.

[`contact_symbols.aml`](../src/contact_symbols.aml) owns this bounded organ.
The [Actually.life follow-up](research/iceberg/GLYPHS.md) records its source
mechanism and an executable ingest/dream probe. Haiku adds private ownership,
frozen encounter paths, explicit publication and exact restart.

## What counts as experience

Haiku accepts native human input, source 0. The friend accepts only Haiku's
native dream lines actually delivered through peer source 6. Both their root
and immediate parent must be Haiku/source 7, with equal delivery, parent and
origin event IDs. Other human, peer, cloud, inner, self and model lanes supply
no symbol evidence. Dream consolidation itself creates no external observation.

Each eligible encounter freezes its ordered base-anchor path before that
encounter updates the word learner. Reviewed bindings and already available
learned associations can supply anchors. Unknown words leave barriers; they
cannot join two known words into a false adjacent pair. Consecutive aliases
for the same anchor collapse before recognition. More than 64 normalized
tokens quietly yield an unavailable path while retaining the exact text.
Empty and oversized inputs use a single unknown barrier in the stored vectors;
they provide no adjacent relation or predictive pressure.

Recognition proceeds left to right, choosing the longest expansion of an
already-born symbol; equal spans use canonical structural names to break ties.
An encounter can use only nodes born on earlier turns. Its base and compressed
paths remain frozen even if a word later acquires or loses an association.
Old observations are never reinterpreted as fresh recursive evidence.

Each ordered adjacent pair receives at most one support per external turn.
Repeating a pair within a turn or replaying a delivery adds no support. Meeting
the same path on a new eligible turn does count: this is recurring experience,
separate from the word learner's distinct-context rule.

## The actual dream boundary

At least **14 distinct external turns** must support a pair. At the end of an
actual dream, after all contact, lexical passes and admission for that turn,
each owner may preserve at most one eligible pair. The new node cannot affect
its birth turn or trigger another birth in that same dream.

The organ captures its activation turn and the voice's existing dream count.
An older or repeated dream ordinal is a no-op; repeating the same turn with a
different ordinal cannot mint another boundary. Ordinals may skip while deeper
life is disabled. The enclosing life checks the recorded ordinal and turn
against the voice's actual dream history. A one-line dream still has a real
boundary: the friend can hear Haiku without speaking its own line.

Nodes are append-only: at most **16**, depth at most **4**, expanded leaves at
most **16**, canonical name at most **256 characters**. Shared anchors retain
IDs 0–87; private compound IDs start at 88. Parents always precede children.
An ordered canonical name identifies the structure independently of local ID
allocation. Different owners can recognize the same structure while retaining
different evidence and pressure. Version 4 transmits the existing spoken text;
version 5 adds [structural adoption](SYMBOL_EXCHANGE.md) with explicit peer
provenance and the receiver's own subsequent history.

The rolling window holds 128 external observation records. Each birth also
copies fourteen witness records into permanent proof slots. Those proofs and
their exact source text survive rolling-window expiry; parent nodes are never
evicted. Capacity exhaustion refuses further birth without deleting history.

## What a symbol changes

An already-born `L → R` can anticipate the descendants of `R` when the expanded
`L` matches the current known path's suffix. Unknown suffix words stop a match.
Only the longest matching left span contributes. All tied predictions share
**total mass .25** across the 88 anchors; repeated descendants cannot increase
that total. The projection joins later contact features before their final
clamp, and the existing parliament observes those actual features.

Lexical ranking uses the latest genuine external context. In addition to its
existing field correction, a candidate receives
`.15 * projection[anchor] * binding_strength`. The extra correction is between
0 and **.0375**, with reviewed strength 1 and learned strength at most .5.
For `(fire+water)`, a later `fire` therefore anticipates water; `fire water`
recognizes the compound and can support a later recursive relation.

This layer needs neither model weights nor a random draw. Its contact and rank
effects work in the weightless field. Actual lexical admission still requires
the existing proposal path; a missing model does not make this projection
rewrite the spoken Markov poem. The two frozen PEN7 bodies, their vocabulary
and the original voice keep their existing identities.

## Four owned leaves

Contact version 3 retains the earlier 47 fields and adds:

| Leaf | Shape | Contents |
|---|---|---|
| `symbol_clock` | 43 numbers | Mode, activation, event/turn/dream clocks, revision, ring and node cursors |
| `symbol_nodes` | 16 × 10 | Ordered parents, shape, birth boundary, first witness, revision and support |
| `symbol_rows` | 352 × 139 | 128 observations plus 224 permanent proofs, ancestry and frozen paths |
| `symbol_text` | 352 × 4 texts | Exact source text, language, root owner and immediate parent owner |

The contact has **51 fields**. Iceberg v4 has **245 saved fields** and its
detached result has **515**. V1/v2/v3 lives retain their 185/227/237 fields.
Version 5 adds four exchange leaves to each contact, growing the life to 253
fields while preserving local birth proofs and the shared 16-node bound.
The symbol profile requires AML's expanded 1024-field record capacity; complete
modules also use its 512 function slots and 16384-line source bound.

`haiku_contact_enable_symbols(contact, enabled, dream_count)` returns a detached,
idempotent upgrade. Its saved enabled/off mode cannot be reconfigured by another
call. `haiku_contact_symbols_step(contact, event)` changes only the four symbol
leaves; `haiku_contact_symbols_dream(contact, dream_count, turn)` consolidates
at a boundary established by the caller. `haiku_contact_symbols_check` checks
the retained structure, ancestry, paths and proofs.

The iceberg publisher begins with the exact predecessor and independently
reconstructs accepted source events in their original order. Each event's
symbol step precedes word learning. It repeats the final dream consolidation
and compares every symbol leaf, alongside the existing source, contact,
word-learning, Janus and lexical checks. A schema-valid alternative history
cannot replace the actual transition. This replay adds no model forward or RNG
draw. Save and owner replacement still happen only after complete validation.

## Activation and reproduction

New word-learning iceberg lives use `dream_symbols = 1`; setting it to 0
creates a v4 control with an inert saved symbol organ. Existing lives keep
their profile until `upgrade_symbols = 1` is selected.
`haiku_iceberg_enable_symbols(life, enabled, tokenizer)` requires a v3 life;
older lives first enable their Janus and word-learning upgrades. There is no
automatic upgrade on resume. Turning off `deeper_life` leaves the contacts
unchanged, including the symbol organ.

Focused gates: `make test-contact-symbols test-iceberg-symbols`. The standalone
gate covers the recursive lifecycle and bounded evidence; the complete-life
gate covers actual dream boundaries, source-backed publication, persistence,
old profiles and worker scheduling. Executed results and dependency pins live
in [HAIKULOG](../HAIKULOG.md).
