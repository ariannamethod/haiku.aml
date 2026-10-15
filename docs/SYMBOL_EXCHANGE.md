# A structure crosses the dream

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

Iceberg version 5 lets a compound symbol travel with an actual spoken dream
line. The receiver resolves its ordered parents into its own alphabet, remembers
who delivered it, and recognizes it from the next turn. Its own subsequent
encounters can grow another layer around the received structure.

The source mechanism is Actually.life's recursive `resolve_sym` at
[`2bf11b3`](https://github.com/ariannamethod/actually.life/blob/2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8/l.c#L1014-L1033):
resolve both parents, reuse an existing pair, or adopt the missing symbol.
Haiku carries that structure through its explicit dream deliveries, bounded
owned records and single publication boundary.

## What travels

An offer comes from the sender's exact state at the beginning of the turn and
the text of its actual native dream event, source 7. The receiver hears the
corresponding derived peer event, source 6. Sender, event number, turn, language,
text, root and immediate parent must agree with that delivery.

Each spoken line offers at most one known compound: the longest expanded match
anywhere in its normalized path, then the earliest position and canonical name
to break ties. Unknown tokens retain barriers; empty or oversized paths supply
no offer. The chosen root carries its whole dependency closure.

The packet names an ordered parent tree over the 88 shared anchors. Its private
numeric IDs belong to the packet; the receiver resolves parents before children
and reuses canonical structures it already knows. Parent order and grouping
remain significant: `((fire+water)+tree)` and `(fire+(water+tree))` are different
structures even though their expanded anchors occur in the same order.

The sender's frozen spelling associations determine the offered path. New words
learned during the turn, a symbol born in this dream and another structure
received in this dream become available to offers on later turns. Previously
adopted structures can travel again, retaining the next receiver's own first
delivery record.

## Two ways into one alphabet

Each owner has one append-only catalogue of at most 16 compounds, with private
IDs 88–103. Both locally born and adopted structures use the existing limits:
depth four, sixteen expanded anchors and a canonical name of at most 256
characters. A complete missing parent closure is adopted atomically. If it
cannot fit, the previous catalogue remains intact. Hearing an existing
canonical structure reuses its ID and first provenance.

A locally born node keeps the [fourteen external witnesses](DREAM_SYMBOLS.md)
that earned it. An adopted node records its first peer delivery and has zero
local witnesses. Sender witnesses and local support counts stay with the
sender. The receiver's old observations keep their frozen paths.

The receiver can then meet `C → music`, where `C` is an adopted compound.
Fourteen distinct eligible turns after `C` becomes available can support that
new relation; its own later dream can preserve `(C+music)` with fourteen local
proofs. The received `C` retains its peer origin throughout this history.

All ordinary contacts, word learning and lexical passes finish first. Each
owner consolidates its own eligible local symbol at the actual dream boundary;
then the recorded deliveries are resolved in their original order. A local
birth therefore has the first opportunity to occupy a matching canonical
structure. Every new node becomes visible from the next turn.

Recognition and predictive pressure use this shared alphabet. The existing
longest-match rule and total prediction mass **.25** apply across all nodes
together. Repeated or returned structures cannot acquire duplicate IDs or an
extra pressure budget. The layer uses no random draw or model forward.

## Owned state and publication

Contact version 4 adds four leaves to the earlier 51 fields:

| Leaf | Shape | Contents |
|---|---|---|
| `exchange_clock` | 9 numbers | Mode, activation, delivery clocks and receipt count |
| `exchange_origin` | 16 × 3 | Local/adopted origin and first packet reference for each symbol |
| `exchange_rows` | 16 × 138 | First-adoption packets, ordered parent closure and frozen sender path |
| `exchange_text` | 16 × 4 texts | Exact text, language, sender and canonical root |

Every retained packet introduces at least one node, so sixteen immutable packet
slots cover the catalogue's whole lifetime. The original symbol leaves retain
their observation ring and permanent local proof slots. Adopted nodes leave
their local proof slots empty.

The contact has **55 fields**; iceberg v5 has **253 saved fields** and a
**531-field** detached completion. Existing v4 lives retain their 245 fields
until an explicit upgrade.

Publication independently reconstructs the actual native and peer events from
the voice's accepted dream history. It derives each offer from the exact sender
predecessor, repeats local consolidation and receiver resolution, and compares
all symbol and exchange leaves. The completed sender cannot supply its own
replacement account of what it knew before speaking. Restore also ties retained
deliveries to their actual dream lines and turns. The whole life is validated
before save and owner replacement.

## Activation

New lives use `symbol_exchange=1` after the word and dream-symbol upgrades.
`symbol_exchange=0` saves an inert exchange profile; disabling `dream_symbols`
also leaves exchange inert. Existing checkpoints keep their saved profile and
mode. Set `upgrade_exchange=1` to explicitly upgrade a version-4 life.
For a saved symbols-off control, also set `symbol_exchange=0`: its saved symbol
mode remains off regardless of the launcher's current `dream_symbols` value.

`haiku_iceberg_enable_exchange(life, enabled, tokenizer)` returns a detached,
idempotent upgrade. Earlier lives first enable Janus, word memory and symbols.
`deeper_life=0` pauses the contact organs together.

The kernel lives in [`contact_exchange.aml`](../src/contact_exchange.aml), beside
[`contact_symbols.aml`](../src/contact_symbols.aml). Reproduction commands and
measured results belong in [HAIKULOG](../HAIKULOG.md).

```sh
make test-contact-exchange test-iceberg-exchange
python3 tests/compare_exchange_profiles.py
```
