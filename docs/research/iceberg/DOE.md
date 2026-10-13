# DoE's parliament underneath Haiku

Research receipt, 2026-10-14 Jerusalem. The Arianna Method Manifesto governs the
language and lineage of this work. This is a source audit and a proposed organ;
no parliament is connected to Haiku by this research.

## Pinned bodies

- DoE: [`588cf0009d3933dc5b7e72ac553fdd6e7932fa65`](https://github.com/ariannamethod/doe/tree/588cf0009d3933dc5b7e72ac553fdd6e7932fa65).
  Read its manifesto, `CLAUDE.md`, README, architecture notes and executable C.
- Haiku: `a7d13aacd790dae64975465a81679cdb9c9ffc4f`, including
  `docs/ICEBERG.md`, `src/session.aml`, `src/foreground.aml`, `src/harmonix.aml`,
  `src/metahaiku.aml`, `src/overthinkg.aml`, `src/dream.aml` and `src/state.aml`.
- NoTorch: `bfde06669ad34896421b86147e919ca902a0d337`; inspected canonical
  numerical APIs, Hebbian implementation and existing Metal parliament kernels.
- LoRAGrad coordination: sibling audit at
  `57c5553a867b363b2eb2029f1827618026fd49fb`; its separate receipt owns the immune
  details and executed evidence. Findings from that audit are attributed below.

The new user direction is explicit: both PEN7 sources become read-only assets;
the living parliament shapes glyphs from cloud, weights, human input, inner
dialogue and remembered experience. Each presence owns its parliament and both
weight currents. No backward/update pass enters either PEN7 substrate.

## What the executing DoE does

| Mechanism | Executing source | Actual contract |
|---|---|---|
| Frozen substrate | `doe.c:2488` | The GGUF file is mapped `PROT_READ, MAP_PRIVATE`; expert matrices are separate heap allocations. |
| Expert geometry | `doe.c:2176–2207`, `2399–2417` | `A[D,rank]`, `B[rank,D]`; both begin as small nonzero random values. Expert owns vitality, frequency, age, usage and low-vitality count. |
| Population | `doe.c:117–122`, `2813–2844` | Minimum 2, maximum 16 per model layer; initial 4/6/8 according to host depth; rank 16, learning window 4. |
| Routing | `doe.c:2914–2959` | Vote is routing-row dot input plus `0.1 * harmonic_resonance`. Consensus is an EMA of bounded vote spread. Higher consensus lowers the elected count; minimum 2. Elected votes receive softmax weights. |
| Delta voice | `doe.c:3756–3779` | After host attention, before host FFN: `x += alpha * sum_e gate[e] * A_e(B_e x)`. Every selected expert receives one usage count. |
| Hebbian reflection | `doe.c:2967–3090` | Current update is a signal-clamped, noisy Oja variant on A and B. Co-activations are accumulated during the utterance and averaged; the bounded update happens once after that utterance for experts that participated. |
| Vitality | `doe.c:3094–3111` | Usage relative to `total_tokens / alive_count` changes vitality by `0.05*(ratio-1)`, then clamps to `[0,1]`. Each check increments age and resets usage. |
| Mitosis | `doe.c:3114–3137` | First expert with vitality > .8 and age > 20 divides into the first free slot. Child A/B inherit parent plus noise, child vitality .5, parent vitality multiplied by .8. |
| Apoptosis | `doe.c:3139–3153` | First expert below .1 vitality for 8 consecutive checks dies; ordinary apoptosis preserves two live experts. |
| Numerical quarantine | `doe.c:165–172`, `3083–3086`, `3280–3288` | A/B containing NaN, infinity or absolute value > 10,000 cause post-update death or prevent spore save. |
| Mycelium | `doe.c:3279–3389` | Spore is keyed by host statistical fingerprint; saves live A/B, voting matrix, consensus, vitality and frequency. It selects the highest step on load. |

The current implementation's reflection boundary is especially useful for
Haiku. Its comment records the move away from resewing an expert while the
same utterance is still being spoken. `g_train` defaults to 0 (`doe.c:145`);
explicitly enabling it collects during generation and flushes after the turn
(`4481`, `4506`; corresponding server path `4762`, `4778`). The README and older
architecture chapter still describe per-token updates. Port the executed
between-turn boundary.

Lifecycle is a separate path. The interactive loop calls vitality, birth and
death every ten generation-loop iterations (`4483–4494`) without a `g_train`
condition. Thus the current interactive CPU lifecycle remains active even when
Oja learning is disabled. The Metal arena comments describe experts as static;
that comment is not the condition executed by this CLI loop. Haiku should have
one explicit publication boundary for all expert mutations.

DoE's voting rows are initialized and persisted but do not receive an online
update in the inspected file. Birth copies parent A/B, while the child's vote
row remains the row originally assigned to that slot. In a Haiku adaptation,
inheritance of routing identity needs its own chosen rule; it is not established
by copying A/B alone.

The Dario field, seasonal controller, calendar and drift tracker live in globals
or ephemeral session structs. They offer useful mechanisms, but each Haiku
presence needs an owned version of whichever mechanisms are selected. Copying
the singleton body would merge their histories implicitly.

Portable commands for all probes are in [the research README](../../../tests/research/iceberg/README.md).

## Executed measurements

The unchanged source test suite passed **113/113**:

```sh
cc doe/tests/test_doe.c -O2 -w -lm -lpthread -o doe-parliament-tests
./doe-parliament-tests
```

The repository probe `tests/research/iceberg/doe-parliament-probe.c` includes the pinned source with `main`
renamed. It calls the actual election, vitality, Oja and spore functions, with
small arrays and no model assets. It writes its spore in a dedicated scratch
directory, never into the source checkout.

```sh
cc doe-parliament-probe.c -O2 -w -lm -lpthread -o doe-parliament-probe
./doe-parliament-probe
```

Recorded results:

| Intervention | Result |
|---|---|
| Eight identical votes, 100 elections, initial consensus .5 | Final consensus `.000113278002`, seven elected. The `1e-8` spread floor retains a small positive consensus. |
| Eight votes `[-7,-5,-3,-1,1,3,5,7]`, same initial consensus and repetitions | Consensus `.999986768`, two elected: experts 7 and 6, weights `.880797029` and `.119202919`. |
| Four equally used experts, each elected in five of ten two-expert elections | Every vitality rises `.7 -> .75`. The source fair-share denominator counts token events, while total expert selections are twice that number. |
| Four stable experts, rank 16/window 4, eight turns of Oja updates | Expert update masks remain `000f`, `00f0`, `0f00`, `f000`: each expert updates only its same four ranks. The one global cursor rotates between experts instead of independently within each expert. |
| Actual spore save/load | Consensus `.370000005` survives; age `31 -> 0`, low-vitality streak `7 -> 0`, election count `91 -> 0`. These fields are absent from the serialized format. |

The global rank cursor is `notorch_offset` at `doe.c:2967`. A constant number
of experts can keep all individual experts permanently confined to a rank
window. Give each Haiku expert its own persisted cursor, or update all of the
small chosen rank in one bounded step. This also avoids cross-presence coupling.

The source usage policy rewards equal participation when multiple experts are
elected per event. For a Haiku parliament, compute vitality from **elected gate
mass** or explicitly normalize by total selection count. Preserve a distinction
between useful dissent, election popularity and measured contribution; otherwise
expert population pressure merely follows how many voices the router consults.

The source spore is adaptation persistence, not Haiku's exact continuation
record. A Haiku checkpoint must also retain RNG, per-expert cursor, lifecycle
counters, routing state, pending evidence, provenance and immune memory. Host
statistical fingerprints also need the actual asset SHA-256 and tokenizer/schema
identity for the shared immutable sources already specified in `ICEBERG.md`.

## A bounded AML organ

Put the first parliament at the **conceptual membrane**. The NoTorch PEN7
forward produces word scores and representations. The same conceptual organ
also receives cloud words, human text, remembered paths and inner dialogue.
This location gives the requested sources a common place to act while retaining
Haiku's existing spoken form, learner and RAE selection.

One candidate starting budget is a 96-component feature vector: 88 canonical
concept activities plus eight named climate coordinates. A concrete initial
climate vector can use current dissonance, novelty, `arousal/(1+arousal)`,
entropy, the three **last published** ring coherences and the previous published
foreground quality. These are available before a new utterance; it does not
need to learn from the result it has not yet produced. Values come from
`harmonix.aml:7–40`, `foreground.aml:61–92` and the published ring receipts in
`session.aml:47–50`. Version and name the feature schema.

Keep availability masks beside the vector: a ring not yet run differs from a
measured zero coherence, and an absent model differs from a model returning
zero similarity. Raw Haiku arousal can exceed one (`harmonix.aml:6–12`;
`state.aml:60` checks only its lower bound), hence the named bounded transform.

Start four experts, allow 2–16, rank 4. At capacity, dense voting rows plus
two low-rank matrices occupy `16*(96 + 2*96*4)*4 = 55,296` bytes: **54 KiB per
presence**, before receipts and immune memory. This proposed size can be tested
without building a parliament per each of the eight PEN7 transformer layers.
If all four components update each turn, the first version needs no rotating
rank window at all.

The organ emits candidate changes, rather than directly replacing the poem:

1. An accepted event supplies semantic activities and climate. Each source is
   tagged before aggregation: human, cloud, model A, model B, mixed model,
   own inner voice, peer, dream/replay. Keep its original words and references.
2. Experts vote on this feature vector. Their low-rank contributions alter
   proposed concept pressure, binding confidence or directed associations.
   Climate coordinates are read-only inputs to the proposal; expert output does
   not overwrite the measured climate.
3. The immune field examines the **proposed concept-state delta** with its
   provenance and climate. Its outcome can pass, scale, defer, scar or silence
   that delta, preserving the event receipt.
4. Admitted concept pressure affects bounded word admission, the next Janus
   balance, or a later directional walk. This gives the parliament an observable
   causal output in Haiku's existing machinery.
5. After the utterance, evidence from subsequent contact can change expert
   credit, vitality and small A/B/prototype state. Accumulate on a detached
   candidate and publish once through validate/checkpoint/swap. Base GGUF
   matrices remain immutable throughout.

This order joins DoE's after-turn reflection with Haiku's existing detached
session (`session.aml:18–60`) and planned ordered worker. Model and mixture
identity stay with every representation. Mixture drift alone does not count as
a newly lived concept observation.

Expert birth and glyph birth are distinct events. A busy, useful expert may
divide; repeated accepted relations may later become a composed glyph. Give
each its own threshold and receipt. Recursive glyph IDs can retain their
structural parents while their first parliament representation remains projected
through the shared 88 anchors. New glyph creation then need not resize all
expert matrices or reinterpret old checkpoints.

Independent evidence matters to the credit path. A vote cannot approve itself
and immediately use that approval as proof of its own contribution. Later human
contact, held-out alias/reconstruction consistency and explicit accepted peer
events can provide separate witnesses. Inner repetition and dreams have their
own counts and cannot manufacture additional waking observations.

## Immune boundary coordinated with LoRAGrad

The sibling audit found an actual 64-coordinate immune field with eight fixed
expert directions and mutable expert credits. It has pass/weaken/freeze/scar/
dark/silence outcomes, cosine memory recall and bounded scar/dark banks. Its
current learning signal comes from paired clean/adversarial examples in its
own experiment. Haiku's endogenous signal therefore needs an explicit new
contract, tied to retained event outcomes; the source does not already supply
a Haiku metric-to-reward formula.

For Haiku, distinguish two decisions: whether to admit a proposed concept
change, and how much credit to assign its parliamentary proponents later.
Metrics can change the sensitivity of that decision, and metric interactions
can create the requested rich internal variation. Derive them from recorded
values and owned draws; unexplained fresh wall-clock randomness would break
continuity without adding a traceable mechanism.

The sibling probe found that `lg_signature_from_buffer` hashes **float value
bits** to choose bins: changing `.5` by one f32 ULP produced an orthogonal
signature. For smoothly changing Haiku climate use stable feature IDs with a
fixed projection/count-sketch, normalize each source block, and preserve the
original tags alongside the signature. Do not use that raw buffer helper for
semantic similarity of metric vectors.

Its current scar and dark banks stop appending at 256 entries; there is no
eviction/deduplication rule or immune checkpoint in the inspected experiment.
Haiku should make bounded memory, age, recurrence and eviction explicit and
persist them with each presence. The source expert credits can adapt while its
expert directions remain fixed; DoE's mitosis/apoptosis supplies the separate
living population mechanism the user requested.

## The NoTorch seam

DoE vendors packed `nt_qmatvec` code into its monolith. Haiku should call its
existing NoTorch dependency. The canonical package already has:

- `nt_blas_matvec` and `nt_linear_values` for pure numerical projections;
- owned RNG helpers, including `nt_rng_normal_values`;
- `nt_metal_parliament_votes`, `nt_metal_parliament_elect` and
  `nt_metal_parliament_inject` (`notorch_metal.h:147–159`), with arena layout
  `[routing rows | each expert B,A]`;
- `nt_hebbian_step`, which is a **different update** from the current DoE Oja
  rule (`notorch.c:5448–5510`). It updates all ranks, uses the newly updated A
  for B's projection and applies multiplicative decay. It has no DoE Oja
  subtraction, injected noise or window cursor. Port the chosen mathematics
  explicitly instead of treating the similar names as equivalence.

The smallest useful reusable CPU surface, if profiling justifies fused calls:

| Operation | Inputs/output contract |
|---|---|
| Votes | Read-only routing matrix `[E,D]`, x `[D]`, alive mask and resonance `[E]` -> votes `[E]`. |
| Election | Votes, alive mask, old consensus, minimum/maximum count -> gates `[E]`, selected count, new consensus. Stable slot order breaks equal-vote ties. |
| Low-rank mixture | Read-only A `[E,D,R]`, B `[E,R,D]`, gates and x -> delta `[D]`; caller-owned scratch `[E,R]`; no tape/optimizer allocation. |
| Bounded Oja update | A/B, x/dy, step parameters and explicit caller-supplied noise -> detached A/B candidate, status. Any rank cursor is supplied and returned by the owner. |
| Signature/projection | Existing matrix projection plus bounded normalization is enough for a fixed immune basis; semantic provenance remains AML data. |

Generic arithmetic belongs to NoTorch. Election policy, expert birth/death,
credit, immune interpretation, glyph admission and event ownership belong to
AML. No DoE or LoRAGrad runtime dependency is added. Their source bodies provide
mechanisms and reference fixtures.

## Evidence required when this becomes code

- Substrate hashes and read-only handles unchanged after adaptation, birth,
  rejection, dreams and restart; no optimizer slot owns model A or B.
- Same input, cloud, history and draws with one expert suppressed: identify the
  changed glyph delta and later word attraction in its receipt. A poem may
  still match when the form gate selects the same candidate.
- Private Haiku/friend state remains identical to control until an explicit
  peer event is delivered; delivery changes only the intended owner's field.
- Zero, one and two model sources: the parliament still receives cloud/human/
  memory evidence without weights. Missing sources consume no optional draws.
- Exact save/restart retains expert identities, matrices, vitality, age,
  routing/immune memory, feature schema, RNG and pending-event revisions.
- Bounded population and candidate budgets; repeated unknown evidence stays
  eligible without being mistaken for numerical poison or a corrupt state.
- One-source ablations isolate human, cloud, model, inner and peer effects.
  Immune credit uses a separate outcome witness from the vote being evaluated.
- Quarantine of malformed proposed updates leaves the last valid owner state
  unchanged. Optional numerical failure cannot publish a half-updated concept.

The first concrete organ can remain small: a frozen lexical substrate, a living
parliament over concepts, and private immune memory. Complexity comes from their
recorded exchange with Haiku's existing life, while the surface remains three
lines.
