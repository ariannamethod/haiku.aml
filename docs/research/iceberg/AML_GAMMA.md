# AML gamma, Janus duality, and the Haiku iceberg

Read-only source audit, 2026-10-13. No product files changed.

## Pins and scope

- `ariannamethod/ariannamethod.ai`: fetched `origin/main` **f2d4496790be33505715ba90373fea5225799625** after PR #35 merged. Its tree is identical to the local tested branch at `5fdfeb7da703c554a44d8cdbd85d1c74c6c639f5` (`git diff --stat HEAD origin/main` empty).
- `ariannamethod/janus`: **5a945a9d7c1dc9abfa217f685f0a81455f9ac493**.
- `ariannamethod/1984`: **a0d20019587ff14e5516eb050ba0876d6edd0baa**.
- Supplemental AML carrier inventory: local `ariannamethod/yent.aml` **28baf3199b649a0b8cad6e03045b979cbead1684**. The AML language's `janus/README.md:30–33` links Janus-R here. No execution claims about this supplemental checkout.

Read `ariannamethod.ai/ARIANNA_METHOD_MANIFESTO.md` and `CLAUDE.md`. Language main checkout stayed on its existing branch and clean; no shared builds or checkouts were changed.

Path convention: the **physical-mixing section** below uses paths in `ariannamethod/janus` (local `<research-workspace>/janus`). All **AML gamma / execution / model bridge** section paths are in `ariannamethod/ariannamethod.ai` (local `<research-workspace>/ariannamethod.ai`). Thus `ariannamethod/janus/janus.c` is a sibling repository's root file; it is not a file in the language repository's `janus/` folder. Penelope paths are explicitly prefixed `1984/`.

## The user's parameter-mixing idea is the literal Janus mechanism

The Janus repository describes and implements physical blending of every parameter:

```text
alpha = clamp01(0.5 + 0.3*(calendar_dissonance - 0.5)
                   - 0.2*prophecy_debt + 0.1*personal_dissonance)
W_effective[i] = alpha*A[i] + (1-alpha)*B[i]
```

Evidence:

- [`janus/README.md:136–146`](https://github.com/ariannamethod/janus/blob/5a945a9d7c1dc9abfa217f685f0a81455f9ac493/README.md#L136-L146): dual weight matrices, the equation, alternating training.
- [`janus.c:437–465`](https://github.com/ariannamethod/janus/blob/5a945a9d7c1dc9abfa217f685f0a81455f9ac493/janus.c#L437-L465): `DualModel` owns `A`, `B`, an entire `blended` buffer, and pointers `bw`; `dual_blend()` traverses all `A.n_params` entries.
- `janus.c:1153–1157,1202`: `associative_reasoning()` calls `dual_blend()` once before reasoning and `generate_sentence(&dm->bw, ...)` reads the mixed weights.
- [`janus-bpe.c:287–297`](https://github.com/ariannamethod/janus/blob/5a945a9d7c1dc9abfa217f685f0a81455f9ac493/janus-bpe.c#L287-L297): same physical all-parameter blend.
- `janus-bpe.c:508–535`: `reason()` blends once, then both forward and backward sentence paths use `dm->bw`.
- `janus-bpe.c:564–565`: training alternates active A/B by step parity.

One comment in `janus.c:460` describes the calendar direction opposite to the arithmetic: increasing calendar dissonance increases the coefficient of A. Port the equation and verified direction, not that comment.

For Haiku + friend, the user's preferred design can therefore be expressed directly:

```text
Shared immutable source weights: A = Penelope, B = NanoJanus
Haiku:  W_H = alpha_H*A + (1-alpha_H)*B
Friend: W_F = alpha_F*A + (1-alpha_F)*B
```

Each organism owns its alpha/field/memory and its two directional association contexts. The source weights stay shared. A forward/backward direction is a context trajectory; it does not require another permanent copy of A or B. This preserves internal duality in both organisms.

## AML already names and evolves the dual-facing field

[`README.md:918–959`](https://github.com/ariannamethod/ariannamethod.ai/blob/f2d4496790be33505715ba90373fea5225799625/README.md#L918-L959) explicitly documents `theta = epsilon + gamma + alpha*delta`, `GAMMA`, `ESSENCE`, `JANUS <a> <b>`, `JANUS_BLEND`, `JANUS CYCLE`, and `GAMMA_DRIFT`. It also explicitly assigns actual weight deltas to the host inference engine.

- `core/ariannamethod.h:99–124`: eight gamma slots; each `AM_GammaSlot` is `{name, alpha, active}`. No tensor pointer or weight bytes live in a slot.
- `core/ariannamethod.h:245–253`: `AM_State` stores slots, essence strength, faces, blend, drift.
- `core/ariannamethod.c:5443–5487`: actual directive parser.
- `core/ariannamethod.c:9519–9603`: load/update/unload slots, dominant face, `am_gamma_get_blend()`, `am_janus_set()`.
- `core/ariannamethod.c:10463–10497`: CYCLE changes `janus_blend` with seasons. Summer moves toward face A, winter toward face B; spring/autumn add a sinusoidal drift. Seasonal energy also changes `essence_alpha`.
- `core/ariannamethod.c:4903–4921`: `janus_gaze()` selects symmetric temporal state and dual mode when two slots exist; `field_assemble()` selects CYCLE, drift .01, essence 1.
- `spec/AML_SPEC.md:534–553,797–816`: formal directives and builtins.

**Coefficient orientation must be explicit:** AML's `JANUS_BLEND = b` uses `0=A, 1=B`. Full Janus's `alpha` uses `1=A, 0=B`. A direct bridge to the equation therefore uses `alpha = 1 - b`. The existing name `alpha` appears in multiple separate roles (gamma slot intensity, language/delta injection, full-weight mixture); retain those distinctions in the interface.

## What the current gamma execution actually computes

`core/ariannamethod.c:9578–9589,9608–9622` computes:

```text
dual_gamma_strength = gamma_a.alpha*(1-janus_blend)
                    + gamma_b.alpha*janus_blend
scale = 1 + dual_gamma_strength*essence_alpha
logit_i = mean(logits) + (logit_i - mean(logits))*scale
```

The numerical intrinsic accepts one existing logits array. It implements field sharpening; it does not fetch or blend A/B parameter tensors. The physical tensor mix must be supplied by the numerical model backend.

Bounded executable proof: `aml-gamma-iceberg-probe.c`, compiled directly against the pinned language source into scratch. With slots .8/.6, `JANUS_BLEND .3`, and `ESSENCE .7`:

```text
gamma blend=0.740000
[1,2,3,4] -> [0.223000,1.741000,3.259000,4.777000]
```

Renaming both slots while preserving their intensities produced identical float bytes. These results match the implementation's scalar formula.

## Existing model bridge and owned-state boundary

- `core/ariannamethod.h:1212–1235`: `am_janus_register()` accepts process callbacks `load_model(path)`, `load_gamma(name,path)`, `generate(prompt,...)`, etc. No model/context handle is passed to these callbacks.
- `core/ariannamethod.c:176–208`: one process-global callback table.
- `core/ariannamethod.c:5664–5687`: `LOAD_MODEL` dispatches the loader; `LOAD_GAMMA` first creates a gamma slot, then calls the loader. Missing backend emits a diagnostic.
- `janus/janus.go:38–44,68–98`: one global `*yent.Yent`, protected by a mutex. Loading a new model closes the previous engine. This path is the Go/Yent GGUF carrier.
- `janus/janus.go:160–176`: Go-exported gamma loader accepts only a path, while the language callback contract accepts name + path; a host adapter is required. The language checkout has no call registering these exact Go functions into the AML callback table.
- `core/aml_notorch.c:56–74`: the current canonical NoTorch bridge registers numerical value functions, RNG/sampling, and the tokenizer. It does not register a PEN7 model loader or dual-weight handle.
- `core/ariannamethod.h:1373–1376` describes an immutable *tokenizer* model handle. It is not an immutable neural inference model handle.

The scratch probe also supplied two failing loader callbacks. Both returned `-1`; `am_exec()` returned `0`, and a gamma slot still existed. Exact output:

```text
callback failures: model=1 gamma=1, am_exec rc=0, configured gamma slots=1
```

Reproduce from `<research-workspace>` (writes only the scratch executable):

```sh
cc -O2 -Iariannamethod.ai/core aml-gamma-iceberg-probe.c ariannamethod.ai/core/ariannamethod.c -lm -lpthread -o aml-gamma-iceberg-probe
./aml-gamma-iceberg-probe
```

Consequently Haiku's silent absent-weight fallback needs an explicit successful provider result/availability contract. A configured `GAMMA` slot or a successful `am_exec("LOAD_GAMMA ...")` cannot establish that weight loading succeeded.

The field itself is shared: `core/ariannamethod.c:99` declares `static AM_State G`; calendar/birth/key variables are also process globals (`:286–292`). `spec/AML_SPEC.md:1357–1363` documents that SPAWN snapshots variables but shares field state. The two organisms should own their gamma/mixture/temporal state in Haiku records, or AML should gain an owned field-instance API before both mutate these directives concurrently. Generic mutable context ownership belongs upstream; Haiku's policies remain AML.

The calendar implementation already has a useful stable-input rule: `am_step()` samples the day once (`core/ariannamethod.c:10234–10235`), derives both clocks from it, and `janus_temporal_alpha` is a pure function of the birthday/yahrzeit gap (`:10253–10278`). Its temporal alpha is separate from the weight-mixing alpha. The same single-clock and explicit checkpoint rule should be retained in Haiku.

## Penelope and Janus AML examples

`ariannamethod.ai` names Penelope in its family table (`README.md:1488`) and provides compiler usage `amlc penelope.aml` (`tools/amlc.c:519–523`). The actual Penelope source is in `1984/penelope.aml`:

- `:9–25`: declares the native `BLOOD COMPILE` design and actual PEN7 layer architecture.
- `:582–635`: native `Model` definition.
- `:814–867`: native `model_load()`.
- `:926–1060`: native PEN7 forward pass.
- `:1538–1581`: native 12-step chain calls that forward pass.
- `:1729–1802`: `BLOOD MAIN` owns one model, load, CLI/REPL.

This is a working AML-to-native carrier and useful porting reference. Its numerical engine and policy are C blocks; Haiku's goal still calls for AML application modules over canonical NoTorch operations.

The language checkout's `janus/janus.aml` is the triple-attention architecture training example: learned QKV, low-rank positional RRPRAM, Echo, three-way per-head gate, and Dario overlay. It is a different tensor layout from Penelope/NanoJanus PEN7, whose eight blocks use the two-way content/linear-resonance gate. Do not feed PEN7 bytes directly into the triple-attention layout.

The complete language git tree has no `nanojanus.aml`; it links Janus-R's AML carrier in `yent.aml/janus-r/janus-r.aml`. NanoJanus Python/C/HTML remain in the `janus` repository. This inventory does not change the confirmed full-weight mixing mechanism above.

## Integration consequences for the updated Haiku design

1. Prioritize the requested **physical parameter mixture**. Borrow full Janus's all-parameter equation; use AML gamma/season/state concepts to control each organism's own mixture. Both Haiku and friend have both faces.
2. Shared immutable A/B + owned inference contexts is the required API shape. Loader status, architecture/tokenizer identity, and missing-weight behavior must be explicit. The existing singleton GGUF callback does not supply that shape or the PEN7 format.
3. Two PEN7 source tensors occupy about **149.68 MiB**. Two materialized whole-model mixtures add another **149.68 MiB** (~299.36 MiB before activations). One sequentially reused mixture buffer gives ~224.52 MiB before activations. Matrix/layer-at-a-time blending can bound additional scratch further. Four directional contexts do not imply four source copies.
4. Freeze the mixture coefficient for a bounded association pass, following upstream `dual_blend()` at the start of `reason()`. If the coefficient changes while using a future KV cache, invalidate/recompute history under the new weights; old cache values belong to the old mixture.
5. The compatible PEN7 shape and shared BPE support the operation directly. Evaluate mixture endpoints and interior coefficients on the actual two checkpoints before selecting the default dynamics. Record logits/candidate quality, association diversity, latency, and Haiku's syllable/form acceptance. The useful mixture behavior is a measured property of this specific pair of weights.
6. Keep absent weights entirely silent at the application surface and preserve the established weightless route. The optional field can still evolve from Haiku memory/cloud/glyphs; an absent provider does not allocate a random 19.6M model or consume voice RNG.

No implementation changes are claimed by this audit.


See [The Haiku iceberg](../../ICEBERG.md) for the combined design and
[the research bundle](../../../tests/research/iceberg/README.md) for portable commands.
