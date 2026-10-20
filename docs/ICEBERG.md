# The Haiku iceberg

2026-10-13. Governed by the
[Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

Oleg's direction: a small presence at the surface, a longer life underneath.
Penelope supplies another source of words; Janus gives association two
directions; glyphs let differently worded experiences leave a shared conceptual
trace. His further choice is **duality inside both Haiku and the dream-friend**.
Each can carry both currents into their conversation.

This document separates source research, executed foundations and the proposed
full extension. The [owned worker tail](ASYNC.md) now preserves the complete
chat's dreams, Phase4 memory and journal through join, save, swap and display.
The two frozen sources are packaged as [Q8_0/F32 GGUF](../weights/README.md).
The new [iceberg conversation](ICEBERG_SESSION.md) connects immutable AML
handles, four private lexical contexts, parameter mixing and two private
parliaments to actual encounters. Bounded text-context returns provide word
scores and representations; concept pressure joins lexical admission. Its
version-8 checkpoint retains 276 fields across the voice, both lexical/contact
lives and both Janus organs. Private [word associations](LEARNED_WORDS.md) grow
from repeated accepted encounters; [recursive symbols](DREAM_SYMBOLS.md) can be
born at an actual dream boundary and [cross a spoken dream](SYMBOL_EXCHANGE.md).
The four reviewed model concepts now acquire [personal centres](PERSONAL_CONCEPTS.md)
from each presence's retained encounters and contribute to later word ranking.
Their representations also corroborate [private word bindings](NEURAL_WORDS.md)
from sparse external contexts, reaching later symbol paths and foreground speech.
The [engineering log](../HAIKULOG.md)
records each stage and its evidence. The speaking form now grows from present
and remembered climate: fragment, couplet, haiku or an ABA tercet. Python Haiku remains
the behavioral and artistic reference; `haiku.aml` retains that stable profile.

The sections below preserve the research and broader design. The executing
boundary is detailed in [ICEBERG_SESSION](ICEBERG_SESSION.md),
[LEXICAL_SOURCES](LEXICAL_SOURCES.md) and [PARLIAMENT_CONTACT](PARLIAMENT_CONTACT.md).
Dynamic [Janus balance](JANUS_BALANCE.md), learned word associations and local
recursive glyph birth now run in the separate profile. Structural adoption
between owners preserves first-delivery provenance and later local growth.

## Sources and receipts

The family research inspected implementations and exercised bounded paths.
The source revisions below make each observation reproducible.

| Source | Inspected revision | Detailed receipt |
|---|---|---|
| [Penelope / 1984](https://github.com/ariannamethod/1984/tree/a0d20019587ff14e5516eb050ba0876d6edd0baa) | `a0d20019587ff14e5516eb050ba0876d6edd0baa` | [Words, model, field and fallback](research/iceberg/PENELOPE.md) |
| [NanoJanus / Janus](https://github.com/ariannamethod/janus/tree/5a945a9d7c1dc9abfa217f685f0a81455f9ac493) | `5a945a9d7c1dc9abfa217f685f0a81455f9ac493` | [Directions, calendars and A/B parameters](research/iceberg/NANOJANUS.md) |
| [Actually.life](https://github.com/ariannamethod/actually.life/tree/2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8) | `2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8` | [L1/L2, attraction and symbol birth](research/iceberg/GLYPHS.md) |
| [Leo](https://github.com/ariannamethod/leo/tree/b842f6f480da928c666f8685e87cac6f6b899e90) | `b842f6f480da928c666f8685e87cac6f6b899e90` | [Learned word-to-concept bindings](research/iceberg/GLYPHS.md) |
| [caveLLMan](https://github.com/ariannamethod/caveLLMan/tree/471aed21abe9b7b882a464f07bc66b8ef3d5c411) | `471aed21abe9b7b882a464f07bc66b8ef3d5c411` | [Compression and pressure](research/iceberg/CAVELLMAN.md) |
| [NoTorch](https://github.com/ariannamethod/notorch/tree/bfde06669ad34896421b86147e919ca902a0d337) | `bfde06669ad34896421b86147e919ca902a0d337` | [PEN7 numerical proof](research/iceberg/NOTORCH.md) |
| [AML](https://github.com/ariannamethod/ariannamethod.ai/tree/f2d4496790be33505715ba90373fea5225799625) | `f2d4496790be33505715ba90373fea5225799625` | [Gamma, Janus and model ownership](research/iceberg/AML_GAMMA.md) |
| [DoE](https://github.com/ariannamethod/doe/tree/588cf0009d3933dc5b7e72ac553fdd6e7932fa65) | `588cf0009d3933dc5b7e72ac553fdd6e7932fa65` | [Election, expert lifecycle and Oja](research/iceberg/DOE.md) |
| [LoRAGrad](https://github.com/ariannamethod/loragrad/tree/57c5553a867b363b2eb2029f1827618026fd49fb) | `57c5553a867b363b2eb2029f1827618026fd49fb` | [Immune axis, credits and memory](research/iceberg/LORAGRAD.md) |
| [Dario](https://github.com/ariannamethod/dario/tree/8ccf9053298a8039e3d5e9df7fa053f4aed481e7) | `8ccf9053298a8039e3d5e9df7fa053f4aed481e7` | [V2 paper, thought boundaries and provenance](research/iceberg/DARIO.md) |
| [Arianna](https://github.com/ariannamethod/arianna.c/tree/868773f300968536989391d3de84922f5b21be42) | `868773f300968536989391d3de84922f5b21be42` | [Text, direction and the returning lower voice](research/iceberg/ARIANNA_INJECTION.md) |

Small portable probes and commands live in
[tests/research/iceberg](../tests/research/iceberg/README.md). They use optional
reference checkouts and are outside the runtime and normal test gate. The
pinned reports preserve their historical findings; the implemented foundations
below describe what has changed since those audits. The two approved converted
assets live in `weights/` with their manifest. Probe executables and live state
remain outside the repository.

## Words below the cloud

Both original PEN7 files, checked into their source repositories, are trained
assets:

| Asset | Bytes | Float32 parameters | SHA-256 |
|---|---:|---:|---|
| `1984/weights/penelope.bin` | 78,477,152 | 19,619,280 | `afff67c780a7c78fdc1f76e521dfda30d65e68b95fe5bd324bb392b84ba3cd38` |
| `janus/weights/nanojanus.bin` | 78,477,152 | 19,619,280 | `d675df2ee3a150d63bc73935a84060280d0f6b0aa84283e4b78ab905180b576c` |

Haiku carries their converted descendants as `weights/haiku1.gguf` and
`weights/haiku2.gguf`: **21,308,576 bytes each**, with Q8_0 matrices and F32
positions, norms and gates. The [manifest](../weights/manifest.json) pins both
source revisions and both converted identities. The source weights stay frozen;
learning in the parliament changes its own small state.

They have the same tensor layout and exactly the same 1,792 BPE merge tuples:
2,048 tokens, width 448, eight layers, seven heads, FFN width 896, context 256.
Their values and curated word vocabularies differ. Whole-word IDs must be aligned
by spelling/language; only the BPE coordinates are already shared.

Each model gives token scores. Penelope and NanoJanus score a candidate word by
the mean of its BPE-token logits, add their live field and select a whole word.
The extension vocabulary comes from filtered, decoded BPE entries; the weights
give those entries context-dependent attraction. This creates a bounded word
source with a much larger internal calculation.

Haiku can admit a small number of these proposals to its cloud, recording source,
language, syllables and the event that admitted them. Its existing form,
MathBrain and RAE then determine the spoken haiku. Twelve internal associations
can leave their pressure in three lines.

Admission needs its own explicit policy. The existing C filters can admit
fragments such as `ular` or `ers`; the Python, C and HTML candidate sets differ.
The first integration can score known cloud words and a reviewed whole-word
pool. These shipped sources are English; multilingual growth still belongs to
Haiku's language packs and Unicode text path.

### NoTorch can execute the body

The scratch proof redirected the pinned Penelope C architecture's heavy
operations to existing NoTorch matrix, RMSNorm, RoPE and attention kernels.
At a nine-token context, **all 4,096 logits across both files matched the source
C forward bit for bit**. A deliberately wrong RoPE convention changes the
results and fails the comparison. The receipt fixes the context, scalar backend
and source revisions of this numerical proof.

A second probe interpolated **all 19,619,280 parameters at alpha=.5**.
Its 2,048 logits also matched source C bit for bit. All weights and outputs
were finite, the global numerical tape stayed unchanged and no optimizer slots
were allocated. This parameter mixture differed from averaging the two endpoint
logits in all 2,048 positions (mean absolute difference 4.88956). The
[recorded arrays](../tests/research/iceberg/pen7_blend_probe.json) make the
distinction directly inspectable. Choosing a useful default balance belongs to
the word-admission and trajectory experiments.

NoTorch now has a native PEN7-to-GGUF converter and `penelope` family loader,
tokenizer and forward. Its Q8 path expands matrices once to owned F32 and
executes them without a training tape; auxiliary F32 tensors borrow the
read-only mapping. Learned positions alongside RoPE, RMSNorm epsilon `1e-5`,
and the dense `Wr @ h` branch retain their exact architecture. The
[conversion/forward receipt](../weights/README.md) records four tested context
lengths, chunked input, quantization differences and deliberate wrong-RoPE
controls. The earlier research loader limitation is fixed at the NoTorch layer.

The [iceberg conversation](ICEBERG_SESSION.md) now uses immutable PEN7 values in
AML for independent source availability, bounded word admission and contextual
representation extraction. Its reusable NoTorch workspace mixes every parameter
through bounded layer scratch; the two source bodies remain unchanged.

Both expanded F32 parameter bodies together are **149.68 MiB**, before mapped
Q8 bytes and working buffers. The smaller GGUF files reduce storage; the current
forward still holds expanded matrices. A short scalar forward in the original
F32 probe took about 136 ms per model on this host. The source
C twelve-step chains took about 3.5–4 seconds. Those are different workloads
and single-run observations, not interface latency promises.

## Two directions inside each presence

NanoJanus performs **twelve associations total**, divided between two contexts.
Both contexts begin with the prompt and grow separately. The implementation
uses ordinary causal inference for both; direction changes field pressure and
which history is extended. This is a useful starting topology.

Full Janus supplies another mechanism: two complete parameter sets A/B are
interpolated using calendar dissonance, debt and personal dissonance. It also
has a self-echo attention absent from PEN7. Oleg specifically chose to explore
**mixing the parameters themselves**, following the Janus example in AML:

`W_i = alpha_i * A + (1 - alpha_i) * B`.

Each presence owns its balance and its history. Both PEN7 sources have matching
tensor and BPE coordinates, so this experiment can use the actual shipped pair.
Interpolating parameters acts inside every layer; averaging final model logits
is a different calculation. Keep the endpoint models as comparison controls.

### The language already has words for this

AML's [Gamma and Janus reference](https://github.com/ariannamethod/ariannamethod.ai/blob/f2d4496790be33505715ba90373fea5225799625/README.md#L918-L959)
defines `GAMMA`, `ESSENCE`, `JANUS`, `JANUS_BLEND`, `JANUS CYCLE` and
`GAMMA_DRIFT`. Seasons move the balance between two faces. This gives the new
organ an existing Method vocabulary and field dynamics to build upon.

The executing core stores names and strengths in gamma slots. Its built-in
logit operation scales one distribution around its mean; the README delegates
actual parameter deltas to the inference host. Physical all-parameter mixing
is implemented in the sibling Janus repository's `dual_blend()`. Connect these
two parts explicitly through the new PEN7 execution boundary.

Keep the coefficient convention visible: AML `JANUS_BLEND = b` means `0=A,
1=B`, whereas the full-weight equation above means `alpha=1` selects A.
Thus its coefficient is `alpha = 1 - b`.

The audited AML field `G` and the historical Janus model callbacks are process
globals. The Go carrier holds one active engine. Haiku needs owned field/model
contexts so the two presences can evolve independently and use the canonical
NoTorch path. A configured gamma slot also does not establish load success:
the measured `LOAD_GAMMA` path retains its slot when a callback returns failure.
Provider readiness must be a separate explicit result. The pinned
[gamma audit](research/iceberg/AML_GAMMA.md) records the source paths and probes.

### Four trajectories, two autobiographies

For Haiku, the retrospective direction can follow **incoming Phase4 transitions
and journal memories**. The prospective direction can extend outgoing
associations and expectations. That gives the two directions different access
to its lived history. It does not require a reverse-trained language model.

Oleg chose the following ownership:

| Presence | Retrospective trajectory | Prospective trajectory | Lexical access |
|---|---|---|---|
| Haiku | Its remembered paths and retained context | Its continuations and expectations | Both PEN7 sources |
| Dream-friend | The friend's remembered paths and retained context | The friend's continuations and expectations | Both PEN7 sources |

Two shared immutable model handles serve **four owned trajectories**. Each
presence has its own field, contexts, pressure, source balance and random
stream. Sharing weights does not require sharing its autobiography. The large
stored-weight allocation remains two bodies of parameters. A naive materialized
mix needs a further 74.84 MiB working body; a reusable scratch mixture or
layerwise mixing can bound that cost. Do not keep four permanent full copies
in the conversational record. Additional contexts and per-call buffers also
need a budget. More trajectories require more computation; the internal step
budget must remain explicit.

Capture each presence's mixing coefficient for its association pass, as Janus
does at the start of reasoning. A later coefficient change needs a fresh
forward or an invalidated cache; cached keys/values computed under an earlier
mixture are not the state of the new one.

The dream-friend's utterances still originate in Haiku's established dream
machinery. The experimental checkpoint gives it separate lexical
contexts, a mixing preference and a complete private parliament. Version 2
adds its own [Janus calendar and expectation debt](JANUS_BALANCE.md), bringing
the complete checkpoint from 185 to 227 fields. Its history
advances only through its own events and explicitly delivered encounters.

Each presence may come to favor the two lexical sources differently through
its history. Compare interpolated models across several balances with each
endpoint and a repeated single source at equal compute and admission budgets.
Test the difference in trajectories, word proposals and later haiku.

A first ownership experiment can fork one checkpoint with identical weights
and draws. Have friend-forward produce the same `water -> tree` event in both
runs; deliver it to Haiku in one and withhold it in the other. Haiku must remain
identical until delivery. Afterwards, inspect its measured attraction toward
tree and the event responsible for it. The final poem may still coincide: the
first question is whether contact changes the intended field without leaking
through shared state. Each trajectory owns its context, cache, scratch and RNG;
an event names sender, direction, turn/revision and provenance.

## A conceptual life underneath words

Actually.life provides a small, concrete semantic organ:

- A membrane maps lexical variants onto 88 initial concepts.
- Directed transitions remember their order. The last eight concepts pull on
  the next choice with distance weights `0.6^k`.
- Current charges and persistent scars add two timescales of pressure.
- Repeated waking pairs can become new, recursively composed glyphs in dreams.

Executed examples compressed `rain ocean water fire flame burn` into
`water fire`. Three `water -> tree` observations and one `water -> light`
made tree more attractive. Thirteen observations of a pair produced no new
glyph; fourteen allowed `fire+water`, then `(fire+water)+tree`. These are real
new IDs with inherited embedding/head rows in that source.

Haiku can retain its actual words while the concept trace survives changes of
wording. Different histories can then attract different words from an identical
current cloud. A dense `88 x 88` float32 transition table is only 30,976 bytes;
the first field does not need another large neural model.

L1 and L2 in Actually.life are two independent bodies with separate state.
They hear each other's glyph utterances and can read a shared wave field.
Each also retains a private shadow of its own contributions, comparing that
history with the shared field before depositing again. For Haiku and the friend,
this suggests shared base meanings, private traces and explicit exchange events.
A peer's influence should be distinguishable from an internally replayed echo.
Recursive new symbols should travel by their parent structure, rather than
assuming two private numeric IDs mean the same thing.

Leo contributes a useful learning boundary: an unknown word can remain open,
then acquire a persistent concept binding through later contact. caveLLMan
contributes a compact surprise-to-pressure gate. Actually.life provides the
stronger recursive-symbol implementation. The combined design can keep both
slow, named associations and faster current pressure.

The source membranes are mostly English/ASCII. Actually.life's approximate
orthographic router can map `inferno` to `give`, and Cyrillic is discarded.
Haiku needs explicit Unicode handling, language-qualified bindings and an
unknown state. Concept compression should preserve useful ambiguity and the
original words from which a binding was learned.

### Weights can participate in forming glyphs

Oleg extended the design in both directions: the neural sources can contribute
to the conceptual membrane itself. NoTorch should run the model forwards,
parameter mixing, vector comparisons and numerical field updates; AML owns
which experiences become concepts and how they affect the next exchange.

Begin with the 88 shared anchors and propose bindings from model representations
of words in context. Repeated close representations, recurring co-activation
and the organism's retained encounters can support a new binding or a dream-born
concept. Retain the original words, source context and contributing events so
the new glyph has a recoverable history. This is a proposed experiment beyond
the literal alias maps measured above.

The two models can each supply a representation of a shared concept. Keep the
provider and mixture identity with each representation, and compare relations
through the common concept anchors. Each presence owns its learned associations,
charges and history around those anchors. A change of mixture alone must not
masquerade as a newly lived association. Contextual features and any cached
prototypes need a defined mixture/version identity.

The return path is equally useful: glyph attraction can influence each
presence's next mixing balance, lexical admission and association trajectory.
This makes a cycle from model representations through conceptual experience
and back to word selection. Weightless operation keeps the seed concepts and
experience-derived bindings active; a neural source adds another contribution.

For the first measurement, hold the diary, cloud and draws fixed, enable only
the representation contribution, and inspect changed concept bindings,
admissions and later haiku. Compare repeated paraphrases, unrelated contexts
and a held-out set of word/concept bindings. Then test whether a new concept
survives restart and influences a later exchange through its recorded history.

## A private parliament now has a body

Oleg's next direction adds DoE's living experts and LoRAGrad's immune field to
that conceptual life. [`src/parliament.aml`](../src/parliament.aml) implements
the first bounded organ: 96 fixed feature coordinates, rank-four adapters,
four initial experts and a population floor of four, up to sixteen slots. Its
election can select as few as two experts. The 64-dimensional immune signature
uses stable feature columns; tiny metric changes preserve nearby signatures.

The admitted proposal changes a persistent 88-value glyph-pressure field.
Six immune verdicts govern its magnitude; 64 combined SCAR/DARK memories retain
encounters with deduplication and least-recently-used replacement. Thirty-two
receipts bind later learning to a retained human-source witness with its actual
saved climate. Private credits and adapters change through that delayed reward;
the PEN7 bodies stay frozen. Birth, retirement, expert identities, per-expert
rank cursors, RNG and all receipts survive checkpoint continuation.

These are executed mechanisms, documented and tested in [PARLIAMENT](PARLIAMENT.md).
The experimental [contact bridge](PARLIAMENT_CONTACT.md) now derives features
from actual human, cloud, inner and dream encounters. Reviewed word aliases and
four model prototypes contribute to the 88 coordinates; private pressure adjusts
later vocabulary admission. Each presence has its own complete owner. Private
word associations and dream-born recursive symbols extend these features.
Model-corroborated private word associations now grow from repeated sparse
contexts and reach later symbol paths. Representation-driven compound formation
and its feedback into parameter balance remain next steps.

## A thought can travel down and return

Oleg's next layer lets Haiku and the friend send their own utterances into the
frozen lexical bodies. A returned continuation can change word attraction and
glyph pressure, giving the next three lines a longer internal conversation.
The [Dario](research/iceberg/DARIO.md) and
[Arianna](research/iceberg/ARIANNA_INJECTION.md) audits pin executable source
paths, distinguish the transfers and record small policy/numerical probes.
The bounded text-context return is now implemented in [lexical sources](LEXICAL_SOURCES.md)
and connected by `iceberg.aml`. The following research contract also describes
later direction-only and recursive experiments; the stable `haiku.aml` profile
retains its original path.

Dario contributes selective continuation at a generation boundary: a retrieved
fragment enters the token context and the following generation grows from it.
Arianna's ordinary duet uses two different transfers. Previous Resonance words
enter Janus's next prompt; current Janus words steer Resonance through a
separate embedding-derived direction added to logits. Its deeper chorus also
has neighbor attention, which belongs to a later experiment with an explicit
coordinate contract.

For Haiku, start at a completed haiku or internal association boundary. Select
one fragment from a named encounter, send it through a receiving presence's
owned PEN7 context, then return a bounded lexical or representation proposal.
Both presences retain both frozen sources and their own retrospective and
prospective histories. Keep parameter balance fixed during this first trial;
text injection, direction pressure and parameter mixing each need a separately
measured contribution. PEN7 uses its native BPE and 256-position window. Budget
the fragment together with retained context and reserved continuation space.

Each excursion carries its owner, original source, event and parent ancestry,
direction, source hashes, tokenizer, mixture, receiving revision and work
budget. Publish it once through the owned completion boundary. Start with one
trip down and back per selected boundary, preserving the already-published
utterance. A reformulated self/friend utterance retains that origin throughout
its return; it cannot become a new human witness for parliamentary reward.

Compare no injection, a relevant fragment and a matched shuffled fragment with
identical checkpoints and draws. Then compare text-only and direction-only
routes before combining them. Inspect word ranks, glyph-pressure changes,
copied spans, topic continuity, form and later learning. An absent model or a
disabled route must preserve the established weightless path and its draws.
Replayed events, stale results and completion order need the same exact
ownership checks as the existing learning tail.

The direction probe also gives a concrete design constraint: apply the immune
gain after normalization, so normalization cannot cancel its attenuation.
Invalidate dependent caches when the mixture or timed pressure changes. Keep
all selection, provenance and admission in AML, with numerical execution in
NoTorch; these references add no runtime dependency.

## Quiet absence, explicit continuity

The HTML Penelope and NanoJanus paths demonstrate actual model-free field
selection. Their Python/C paths labeled weightless still allocate and execute
randomly initialized transformers. Haiku's optional providers follow Oleg's
requested quiet absence contract:

1. Load each optional source independently. An absent file produces no proposals
   and draws no foreground randomness. A present second source can still work.
2. Keep admitted words and conceptual history when a source becomes unavailable.
   The ordinary cloud and any enabled glyph field continue their own life.
3. Retain immutable model bytes outside the per-turn cloned record. Store source
   identities, tokenizer identity, contexts and small learned state in checkpoints.
4. A loader validates the complete file before exposing a model. Failures publish
   no partial candidates. Availability and mismatch details remain inspectable
   through explicit diagnostics, without inserting fallback messages in poems.
5. An old profile with all new organs disabled keeps its exact continuation.
   An enabled glyph/Janus profile still works with zero, one or two model sources.
   Changing source availability can change future poems; it must not silently
   erase history or pretend to be an identical two-source continuation.

Freeze the actual source availability and identities for each accepted event.
A durable pending exchange also needs enough recorded provider results to
replay if the optional assets disappear before restart. New availability takes
effect at the next event boundary. In-process jobs retain their model handles
until completion; optional-source changes cannot alter a half-published event.

Dream replay can affect internal state without incrementing external-observation
counts. Actually.life demonstrates that separation. Haiku's journal can carry
origin, persona, direction, turn and the contributing source identity. Publish
each completed change through its existing validate/checkpoint/swap boundary.

## Porting findings that affect the design

The detailed receipts preserve divergences measured in the pinned source
revisions so the port can choose an explicit contract:

- The audited NanoJanus computes new prophecy debt from a one-candidate list, so
  it is always zero; an initial debt only decays. Its prophecy average also
  reads a missing field. Retain the real candidate contrast and step receipt.
- Negative calendar drift differs between Python modulo and C/JS remainder.
  At the measured day 740, dissonance is `.781594` versus `.218406`, changing
  the directional budget. Choose one signed-time convention and checkpoint
  the calendar input used by each event.
- The audited caveLLMan compound metadata collides with reserved model IDs and
  its zero-initialized Hebbian factors stay zero. The successor's recursive
  allocation and working initialization provide better references.
- Penelope's AML file embeds the C engine through `BLOOD COMPILE`. Its vendor
  cleanup is a separate task. Haiku's behavior remains ordinary AML modules;
  generic model execution belongs to NoTorch and generic ownership to AML.
- DoE's audited Oja cursor is global and can confine each expert to the same
  rank window; its spore omits lifecycle and RNG continuation. Haiku's organ
  implements persisted per-expert cursors and a complete owned checkpoint.
- LoRAGrad's audited numeric signature hashes changing float bits, and its
  append-only memories stop recording when full. Haiku implements a fixed
  feature projection and bounded deduplicated replacement. The original audit
  results remain attached to their original source revisions.

## Implementation order and evidence

| Stage | Current boundary | Evidence or next gate |
|---|---|---|
| Owned Python baseline | Implemented reusable worker and join/save/swap/display publication in ASYNC | Serial/worker/compiled continuation, restart, controlled completion and rejected publication checked; early display/pending intent remain later work |
| Frozen lexical bodies | Implemented NoTorch converter, GGUF loader/forward and two Q8_0/F32 assets | Source pins, all-tensor conversion, multiple lengths, chunking and numerical controls recorded with weights |
| Private parliament | Implemented AML field, experts, immunity and delayed reward; actual contact in the experimental conversation | Two private owners, real encounter order, measured prototype contribution and exact restart checked |
| Optional lexical sources | Implemented immutable AML handles, 48-word reviewed pool, bounded admission and 448-value representations | Zero/one/two sources, quiet absence, exact hashes and unchanged disabled-profile draws checked |
| Duality in each presence | Implemented separate owners, four retained contexts, all-parameter mixing, private calendars and witnessed debt | Explicit captured day, fixed/dynamic controls, delayed external witnesses and complete restart; Janus contract records the boundaries |
| Conceptual attraction | Implemented named aliases, reviewed bindings, model prototypes, tagged exchange and private word associations | Model contribution changes measured features; repeated external contexts teach bounded associations; private pressure ranks later admission |
| Returning thoughts | Implemented one bounded text-context return per selected boundary; direction injection remains planned | Source ancestry, replay, exact disabled behavior, serial/worker equality and one publication checked |
| Dream-born glyphs | Local recursive formation from frozen external paths; one birth per owner per actual dream | Fourteen distinct turns, later recursive recognition, permanent proofs, bounded contact/rank pressure and source-backed publication |
| Structural peer adoption | Ordered parent trees travel with actual dream lines and resolve into the receiver's private IDs | Exact predecessor offers, atomic bounded closure, first-delivery provenance, next-turn recognition and later local recursive evidence |

Keep input, initial state, admission budget and sampling draws fixed when
isolating an organ. Record how it changes the field, admitted words and eventual
haiku. Measure form completion, repeated phrases, responsiveness and restart
continuity alongside the internal traces. The useful result is a short surface
utterance shaped by a deeper, persistent history.
