# A little parliament below the cloud

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

[`src/parliament.aml`](../src/parliament.aml) is an experimental, private organ.
Its experts propose a change in glyph pressure, its immune field decides how
much of that change enters memory, and a later human encounter can teach the
experts what followed. Four voices to begin with. Room for sixteen.

The stable `haiku.aml` profile retains its original imports. The separate
[`iceberg.aml`](ICEBERG_SESSION.md) conversation now connects this organ through
the [contact bridge](PARLIAMENT_CONTACT.md): real human, cloud, inner and friend
encounters, named word bindings and bounded PEN7 representations feed its
88-value pressure field. That pressure contributes to lexical admission for
future poems. Recursive new glyph identities follow this foundation in the
[iceberg](ICEBERG.md) design.

## Two frozen bodies, private changing lives

The two lexical sources are [`weights/haiku1.gguf`](../weights/haiku1.gguf) and
[`weights/haiku2.gguf`](../weights/haiku2.gguf), **21,308,576 bytes each**. Their
matrices use Q8_0 storage; positions, normalization weights and blend gates stay
F32. NoTorch expands Q8_0 once into owned F32 for its `penelope` forward, while
unquantized tensors borrow the read-only mapping. Each expanded parameter body
is about 74.84 MiB, plus mapped bytes, caches and scratch.

These bodies stay frozen. They are absent from the parliament record; its
small `a` and `b` arrays are its own rank-four adapters. They are separate from
the two lexical models A and B. There is no base-model optimizer in this organ.
The [weight manifest](../weights/manifest.json) and [conversion receipt](../weights/README.md)
record hashes, licenses, quantization errors and the native forward comparisons.

Constructing `haiku_parliament_new("haiku", ...)` and
`haiku_parliament_new("friend", ...)` gives two independent owners. Each retains
its own experts, pressure, RNG, immune memory and receipts. The experimental
conversation persists both owners in one complete checkpoint.

## What enters an encounter

`haiku_parliament_features(glyphs, climate, rings, quality)` builds a fixed
96-value vector. Its coordinate identities stay stable as values change.

| Columns | Input | Contract |
|---|---|---|
| 0–87 | Glyph activations | 88 caller-supplied values in `[0,1]` |
| 88–91 | Dissonance, novelty, arousal, entropy | Arousal becomes `a / (1 + a)`; the other three values are in `[0,1]` |
| 92–94 | Echo, drift, meta coherence | Three values in `[0,1]`; missing rings use the `[0]` sentinel at feature construction |
| 95 | Quality | One value in `[0,1]` |

An encounter also carries a source, source-local event ID, logical turn,
explicit replay flag and availability mask. These remain receipt metadata.

| Source code | Origin |
|---|---|
| 0 | Human encounter |
| 1 | Cloud |
| 2 | Lexical model A |
| 3 | Lexical model B |
| 4 | Parameter mixture |
| 5 | Own inner voice |
| 6 | Friend |
| 7 | Dream |

Mask bits are **rings=1, A=2, B=4**. A model source requires its availability
bit; a mixture requires both. Missing rings must have zero feature columns.
Thus an absent ring and an observed zero coherence remain distinguishable.
These codes reserve the integration boundary: the current organ performs no
model loading or model inference.

Valid event IDs increase independently within each source. Turns order the
owner's encounters globally. A model's event 100 can therefore precede a
human's event 1. After argument validation, an explicit replay or an event no
newer than `seen[source]` returns `committed=0` without changing any state or
drawing randomness.

## Election, pressure and immune memory

The live population starts at four, keeps a floor of four, and has sixteen
slots. The **election minimum is two**. Population and elected count are
different quantities.

The routing vote combines a fixed expert row's dot product with the current
features and `0.1 * credit * (novelty - dissonance)`. NoTorch updates the
consensus from vote spread, chooses the leading live experts and normalizes
their gates. Dead and unelected slots have zero gate. Their rank-four proposal
is:

```text
mixed = x + 0.1 * sum(gate[e] * a[e](b[e] x))
delta = clamp(mixed - x, -1, 1)
```

The immune signature projects `x + 0.25 * delta` and a `0.125` source marker
onto 64 coordinates, then normalizes it. Its sparse projection depends on
**feature identity**, so a tiny metric change moves the signature smoothly.
The caller supplies distinct nonzero origin and boundary anchors when creating
the owner; this stage does not infer them from text.

Every live expert votes `tanh(immune_direction · signature)`. Softplus of its
credit weights that vote. The score is `0.7 * axis + 0.3 * consensus`, where
the axis is the normalized origin-minus-boundary direction.

| Code | Verdict | Effect on proposed pressure |
|---|---|---|
| 0 | PASS | `alpha=1` at score ≥ .4 |
| 1 | WEAKEN | `alpha=clamp((score-.1)/.3,0,1)` at score ≥ .1 |
| 2 | FREEZE | `alpha=0` at score ≥ -.1 |
| 3 | SCAR | Boundary projection ≥ .25 or remembered scar; `alpha=0` |
| 4 | DARK | Boundary projection ≥ .5 or remembered dark encounter; `alpha=0` |
| 5 | SILENCE | Remaining score range; `alpha=0` |

Boundary and memory decisions override the score ladder. These are decisions
about an internal pressure update; source names do not prescribe a verdict.
The first 88 delta components enter `field` with scale
`alpha / max(1, norm(delta))`. Each resulting field value stays in `[-5,5]`.

SCAR and DARK share **64 memory slots**. Cosine similarity ≥ .98 refreshes an
existing encounter's turn and hit count; a new encounter replaces the least
recently used slot when full. Similarity ≥ .90 recalls a stored verdict and
sets alpha to zero. Blocked encounters still retain their event receipt and
advance the observation clock; their pressure and adapter weights stay put.

## An expert has a life after its vote

After an encounter with positive alpha, gate mass changes vitality by
`.01 * (gate * live_count - 1)`, bounded to `[0,1]`. An expert below .1 for
eight such checks can retire while the population is above four. A voice older
than twenty checks and stronger than .8 can produce one child per encounter.

The child inherits its parent's adapter values with small owned random noise,
plus its routing and immune directions. It receives a fresh identity, zero
credit and its own rank cursor. Both parent and child begin that next stage at
vitality .5. Slot reuse therefore cannot make an old receipt reward a new voice.

Election alone does not train the adapters. `haiku_parliament_reward` requires
both the original receipt and a **retained later human-source receipt**. The
supplied later feature vector and turn must exactly match that witness. A dream,
an invented witness, an expired receipt or a second reward for the same proposal
is rejected before publication.

The witness must still be on the owner's current logical turn: complete this
reward before another turn advances its clock. Direct calls and loaded receipt
features enforce the same `[0,1]` coordinate domain as feature construction;
signed proposals and immune anchors keep their own separate domains.

The reward compares later quality, the three ring coherences when both events
have rings, and novelty's distance from .5:

```text
reward = clamp(
    later_quality - prior_quality
    + sum(later_rings - prior_rings) / 6       # only when both are present
    + .25 * (abs(2*prior_novelty-1) - abs(2*later_novelty-1)),
    -1, 1)
```

That later encounter can assess several explicitly named earlier proposals;
each proposal receives reward once. Surviving expert identities receive
`credit += .05 * reward * saved_immune_vote`, clamped to `[-10,10]`, when the
original proposal had positive alpha. Selected experts also receive a bounded
NoTorch Oja update with signal `reward * saved_gate * alpha`, learning rate
.001 and one owned normal draw. Each has a persisted cursor that visits its
four ranks independently. Adapter values stay in `[-2,2]`.

The reward equation has no approval term. It uses the climate retained at the
later human encounter. Supplying real observations and attributing their source
is now the responsibility of the [contact bridge](PARLIAMENT_CONTACT.md) and
the experimental [iceberg conversation](ICEBERG_SESSION.md).

## Ownership and restart

Version 1 is one flat **32-field record**. It contains all sixteen adapter,
routing and immune slots; identities, ages, vitality, strikes, credits and
cursors; the 88-value pressure field; the eight source clocks; RNG; anchors;
64 immune memories; and **32 round-robin receipts**. A receipt keeps its input,
proposed delta, gates, immune votes, expert identities, availability, revision
and eventual witness ID. Overwritten receipts can no longer receive reward.

`step` and `reward` validate the owner, work on a detached clone, validate the
complete candidate and replace the owner once. Rejected calls preserve the
previous owner, including every RNG word. `haiku_parliament_save` and
`haiku_parliament_load` validate around AML's native checkpoint boundary.
This standalone schema is separate from Haiku's stable v1/v2/v3 chat state.

AML owns event policy, lifecycle, signatures, retention and publication.
NoTorch owns linear operations, tanh, election, adapter injection, Oja and
normal draws. The numerical APIs use caller-owned arrays without a global tape
or optimizer. DoE and LoRAGrad add no runtime dependency.

## Executed gates and lineage

Build [AML **5.11 / PR #36**](https://github.com/ariannamethod/ariannamethod.ai/pull/36)
with [NoTorch PR #168](https://github.com/ariannamethod/notorch/pull/168), then run:

```sh
make test-parliament
```

[`tests/run_parliament.sh`](../tests/run_parliament.sh) compares interpreted
and compiled scalar output from the life and memory fixtures. The C inspection
gate checks whole-record ownership and exact checkpoint continuation through a
later observed witness and Oja update. Coverage includes independent Haiku and
friend owners, field influence, replay with unchanged RNG, birth, retirement
and the population floor, six verdicts, smooth neighboring signatures, memory
deduplication and replacement, unavailable-source rejection, wrong or absent
witnesses, source-local IDs, duplicate reward and thirteen malformed states.

The independent NoTorch gate covers the numerical equations, overflow and
alias rejection, concurrent callers, rank coverage, unchanged legacy tape and
deliberately wrong injection/Oja controls. Those primitive checks complement
the actual AML organ checks.

| Reference | Pinned source | What Haiku takes forward |
|---|---|---|
| [DoE](research/iceberg/DOE.md) | `588cf0009d3933dc5b7e72ac553fdd6e7932fa65` | Election, rank adapters, Oja, birth and retirement |
| [LoRAGrad](research/iceberg/LORAGRAD.md) | `57c5553a867b363b2eb2029f1827618026fd49fb` | Origin/boundary axis, credit-weighted vote, six verdicts and immune recall |
| [Penelope](../weights/manifest.json) | `a0d20019587ff14e5516eb050ba0876d6edd0baa` | Frozen lexical model A |
| [NanoJanus](../weights/manifest.json) | `5a945a9d7c1dc9abfa217f685f0a81455f9ac493` | Frozen lexical model B |

The audits preserve their original source observations. Haiku's implementation
changes the audited global Oja cursor to per-expert persisted cursors, uses gate
mass for vitality, inherits routing identity, replaces float-bit hashing with
stable columns, and retains bounded immune memory with deduplication and
replacement. It also checkpoints the whole private organ. These are Haiku's
implemented choices; the pinned upstream reports remain historical evidence.
