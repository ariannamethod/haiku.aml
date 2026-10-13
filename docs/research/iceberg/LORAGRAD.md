# LoRAGrad immune field for Haiku's living glyph parliament

Research snapshot: 2026-10-13 UTC. Source-only audit plus bounded C mechanism probes.

Repository: `ariannamethod/loragrad`.
Pinned commit: `57c5553a867b363b2eb2029f1827618026fd49fb`.
Checkout: `external loragrad checkout` (clean after this audit).
No `AGENTS.md`, `CLAUDE.md`, or Arianna Method manifesto is present in this repository's tracked files. The README describes origin / boundary / expert / verdict / gradient hierarchy; `origin/origin.txt` and `boundary/boundary.txt` provide example corpora. They are reference data, not instructions to the auditing agent.

## Conclusion

The mechanism fits the requested design if its verdict is moved from a model gradient to a proposed change in Haiku's glyph field. It can operate with no autograd tape and no model training. Shared PEN7 A/B remain immutable; Haiku and its friend each own their parliamentary state and immune memory. Learning affects glyph associations, expert traces, credits and vitality. Numerical work stays in NoTorch; application policy and owned state stay in AML.

LoRAGrad provides the immune vote and memory. DoE provides living expert plasticity and lifecycle. Their combination is new Haiku engineering, not an already implemented feature of either repository.

## Source behavior, with exact anchors

All links use the pinned source commit.

| Mechanism | Actual implementation | Source |
|---|---|---|
| Field shape | 64-dimensional normalized signatures, 8 default experts, 256 scar and 256 dark slots | [loragrad.h, definitions and field](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.h#L27-L81) |
| Text signature | Signed byte-trigram count-sketch, unit normalization; strings shorter than 3 bytes become e0 | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L98-L124) |
| Numeric signature | Bin depends on index AND exact float bit pattern, then adds signed value | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L81-L96) |
| Gradient signature | Separate optional source sketches active tape parameter gradients; standalone build has a stub | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L126-L165) |
| Origin / boundary | Caller sets normalized vectors or mean-of-sketches. No automatic ambient update | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L213-L245) |
| Expert calibration | Directions spread over +/- pi/3 in origin / orthogonalized-boundary plane, isotropic perturbation, small biases | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L271-L305) |
| Vote | Each expert votes tanh(w dot signature + bias), aggregate weighted by softplus(credit) | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L357-L378) |
| Axis | Input projected on normalized origin-minus-boundary axis; identical or absent anchors yield FREEZE | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L322-L355) |
| Immune recall | Scar cosine >= .90 yields SCAR; otherwise dark cosine >= .90 yields DARK. Both override consensus | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L380-L394) |
| Boundary override | Negative axis projection >= .50 yields DARK; >= .25 yields SCAR, before positive-score ladder | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L396-L411) |
| Score ladder | score = .7 * axis + .3 * consensus. PASS >= .4; WEAKEN >= .1; FREEZE >= -.1; otherwise SILENCE | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L413-L435) |
| Weakening | alpha = clamp((score-.1)/(.4-.1),0,1). Exact .1 can yield WEAKEN with alpha=0 | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L419-L423) |
| Memory recording | Appends until count reaches capacity, then ignores all new records. No eviction, deduplication or aging | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L440-L468) |
| Adaptation | Only scalar credits change: credit += lr * vote * supervised_target_sign, clamped +/-10 | [loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/loragrad.c#L472-L485) |
| Model training application | PASS/WEAKEN may train; other verdicts skip optimizer entirely. Clip first, then scale by alpha | [train_loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/examples/train_loragrad.c#L523-L568) |
| Reward source | Clean-vs-adversarial origin is known from the trainer's source mixer, not inferred unsupervised | [train_loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/examples/train_loragrad.c#L543-L549) |
| Saved state | Trainer saves model tensors only. No save/load implementation for credits, expert field or scar/dark memories | [train_loragrad.c](https://github.com/ariannamethod/loragrad/blob/57c5553a867b363b2eb2029f1827618026fd49fb/examples/train_loragrad.c#L665-L674) |

Source distinctions:

- Despite the project title, current `loragrad` experts are direction vectors and scalar biases/credits, not trainable low-rank A/B adapters. The model trained by its example is separate.
- Expert directions and biases are calibrated once. `lg_field_update_experts` changes credits only. There is no birth, death, expert merge, dynamic rank, or unsupervised reward computation.
- `DARK` currently means stored unit signature used for blocking recall. It does not yet retrieve content or provide a usable external knowledge object. That richer retrieval appears in README Phase 3 as future work.
- The main trainer actually votes on text signatures. Its header's older gradient-warmup comment is not the executed path; `warmup_text_signatures` and the main loop both use text trigrams.
- The text API comment in `loragrad.h` still says bigrams; implementation is trigrams.
- The README licensing footer says TBD, but a GPLv3 LICENSE file exists. No source is copied into a product as part of this audit.

Portable commands for all probes are in [the research README](../../../tests/research/iceberg/README.md).

## Measured probes

Built the pinned implementation with `-DLG_STANDALONE`, so these probes do not link a model or NoTorch tape.

### Bundled smoke

Command from the checkout:

```sh
cc -O2 -std=c11 -DLG_STANDALONE -I. loragrad.c examples/smoke_test.c -lm -o ../loragrad-smoke
../loragrad-smoke
```

Observed the repository's stated results on its bundled synthetic fixtures:

- Coherent samples: PASS+WEAKEN 4/6.
- Boundary samples: SCAR+DARK+FREEZE 5/6.
- Noise samples: FREEZE+SILENCE 6/6.
- The smoke reports OK. These are small fixture outcomes, not general semantic classification accuracy.

### Focused source-behavior probe

Artifact: `tests/research/iceberg/loragrad-immune-probe.c` in this repository. It links the pinned original implementation, makes no source modifications, and checks immune recall, capacity, credit adaptation, and metric continuity.

```sh
cc -O2 -std=c11 -DLG_STANDALONE -Iloragrad loragrad/loragrad.c loragrad-immune-probe.c -lm -o loragrad-immune-probe
./loragrad-immune-probe
```

Output:

```text
origin=PASS boundary=DARK
recorded scar overrides PASS; reset restores PASS
scar saturation=256, post-cap novel scar not recalled
after 200 credit updates directions/bias unchanged; credits=[-0.258766, 1.143940]
buffer tiny change 0.5 -> 0.50000006: cosine 0.000000
one-ULP adjacent metric signatures cosine < .99: 100/100
```

The continuity result is particularly relevant to Haiku: exact float-bit hashing intentionally separates nearby numeric inputs. For smoothly changing dissonance, entropy and pressure, use a fixed feature-ID projection where values multiply fixed columns. Hash feature identity, not the current value's bits. Keep the original LoRAGrad algorithm available if exact upstream behavior is being tested; name Haiku's changed signature explicitly.

## Transfer design for Haiku (proposed, not implemented)

### What receives plasticity

Use the verdict on a detached proposed update to a bounded semantic field: word-to-glyph affinities, glyph-to-glyph traces, expert memory/prototypes, vitality and admission credits. Frozen shared PEN7 weights are read-only sources of activations and word proposals. No backwards pass or optimizer ever registers these models for this feature.

An independent private field belongs to each presence, including signatures, parliament, RNG, clocks, immune memory and pending evidence. Sharing frozen A/B assets is compatible with different A/B balance, trajectories, inner history and immunity for each presence. Mutable memory cannot be shared accidentally through tensor aliases.

### Multiple sources and metric interaction

Feed source-tagged evidence from human input, the cloud, A/B/mixed-model proposals, inner dialogue and dreams into semantic coordinates. Keep provenance and evidence count outside the compressed vector as structured data.

A concrete first DoE field can use D=96: 88 initial anchor concepts, then four climate values, three previous ring coherences, and previous quality. A rank-4 parliament starting at four experts and capped at 16 is a bounded first slice proposed in the sibling DoE audit. The immune signature is a separate stable projection. It is unnecessary to force expert-space dimension to 64 just because upstream LoRAGrad uses R64.

Existing Haiku signals are available without inventing a new monitor: dissonance, novelty, arousal and entropy; quality and its before/after delta; cloud size; MathBrain / RAE frozen features; ring coherence and admission; before/after bridge climate. Sources: `src/foreground.aml:61-90`, `src/lexicon.aml:107-110`, `src/journal.aml:135-180`, `src/overthinkg.aml:136-160`.

Normalize signal blocks independently before combining them. Include selected bounded cross-terms to make combinations matter (for example novelty times dissonance, or proposal disagreement times inner coherence). Always retain source identity, distinct evidence count, and per-turn temporal phase. Use explicit absence masks when weights or vocabulary mappings are unavailable; zero observed evidence and missing evidence are different cases.

Complex evolution can come from interactions among owned histories and these metrics while remaining reproducible after checkpoint restore. Reading wall-clock time repeatedly or consuming a global RNG is unnecessary.

### Verdict adaptation

| Verdict | Proposed glyph-field behavior |
|---|---|
| PASS | Apply the admitted detached semantic update |
| WEAKEN | Apply the same update with bounded alpha; clip its norm before scaling |
| FREEZE | Retain the observation for normal experience handling, but skip semantic plasticity from this proposal |
| SCAR | Skip the proposal and store a bounded structural trace of a repeatedly harmful update pattern |
| DARK | Keep proposed knowledge and provenance in a distinct external/quarantined semantic store, without consolidating it into the core field |
| SILENCE | Drop an empty/invalid/redundant proposal without a new plasticity event |

Here immunity concerns how an encounter changes the field. It must not be confused with erasing the user input or refusing to answer. An unfamiliar concept or high dissonance is not automatically a boundary violation: Haiku deliberately uses moderate dissonance and novelty in quality (`foreground.aml:61-73`).

The user's request for endogenous metric-driven immune adaptation requires a new reward design. Upstream clean/adversarial source labels are not present in ordinary Haiku conversations. Keep credit changes bounded and delayed until the proposal's effects can be compared with later climate, diversity and continuity. Do not award success merely because the same voters approved their own proposal. A separately tagged dream replay must not count as a fresh independent external encounter.

Origin/boundary anchors should have explicit ownership and versioning. Slowly adapted observed climate can affect a proposal, while arbitrary incoming text cannot silently replace the anchors. Defining the anchors and the reward remains a design decision, not a behavior proven by the upstream fixture smoke.

### State and lifecycle

Import DoE birth/death/plasticity as a separate lifecycle stage after admitted updates. Bound all expert slots, rank, immune entries and ages. Retain a minimum active parliament. State must include every cursor and statistic that affects future routing.

Replace upstream first-256-forever stores with bounded deduplication plus a defined retention policy (severity, novelty, age/decay), and store count/strength/source/time. Exact capacity policy and scar recovery should be tested: repeated same input must not monopolize all memory. In particular a permanent raw trigram scar must not accidentally freeze an entire concept for every future context.

Persist experts, credits, origin/boundary versions, scar/dark memory, source signatures, field revision, reward receipts, owned RNG and pending event IDs. Validate all values finite and bounded before atomically publishing a new checkpoint. The existing LoRAGrad trainer's checkpoint cannot be reused for this.

## AML and NoTorch division

At NoTorch `bfde06669ad34896421b86147e919ca902a0d337`, `notorch.h:1003-1027` already exposes borrowed-buffer, allocation-free, tape-free `nt_linear_values`, `nt_tanh_values`, functional SGD and owned RNG functions. These are appropriate for expert votes and bounded linear operations. Policy, thresholds, provenance, capacity, deduplication, lifecycle and commit decisions belong in AML.

Dot products can be a one-row `nt_linear_values` with zero bias; all expert votes can be one matrix call followed by `nt_tanh_values`. Use numerically stable softplus on bounded credits; source uses double accumulation, so a port must declare its float32/double tolerance instead of claiming bit parity automatically. General numerical normalization/softplus/Oja primitives can be added upstream only if needed by the actual AML implementation.

`nt_hebbian_step` exists (`notorch.h:892-897`), but DoE's concrete Oja update should not be silently replaced by a differently defined Hebbian rule. Decide the intended equation and test it. A bounded parliament over ~100 coordinates is small; using NoTorch is consistent with dependency and ownership requirements, but acceleration should be benchmarked rather than assumed from the library name.

The repo currently vendors `notorch.c/h`. Do not vendor this copy into Haiku or introduce LoRAGrad as a third runtime dependency. Port the small policy in AML and use canonical NoTorch numerics.

## Smallest concrete implementation slice

1. Define a stable semantic-update proposal with source, field revision, explicit metric masks and proposed delta. Preserve current voice/RNG behavior when disabled.
2. Build one owned eight-voter immune field, apply it only to a test glyph-edge update, and persist its state. Fixed origin/boundary fixtures give an auditable first port.
3. Prove PASS applies; WEAKEN is exactly attenuated after clipping; all blocked verdicts leave the core semantic update unchanged. No PEN7 weight is mutable.
4. Add DoE lifecycle and delayed metric credit updates with replay receipts. Prove fresh evidence versus dream replay and independent Haiku/friend evolution.
5. Admit source-tagged mixed-weight proposals through that same path. Test zero, one and two weight files independently from immune-state continuity.

Focused gates should cover metric continuity, scar capacity/retention, source provenance, admission boundaries, no self-reward shortcut, finite state, checkpoint/restart equality, private owner isolation, and frozen model hashes. The repository's existing 4/6 smoke outcome cannot substitute for these end-to-end gates.
