# Penelope as a deeper Haiku layer — source receipt

Read-only investigation, 2026-10-13. Repository: `ariannamethod/1984`, pinned
`a0d20019587ff14e5516eb050ba0876d6edd0baa`. Checkout was clean before and after the
bounded probes. No AGENTS.md, CLAUDE.md, or manifesto file is present in this repo.
README and the actual Python, C, HTML, AML, tests, and behavioral reports were read.

## The useful mechanism

Penelope separates her subword internal computation from word-level speech.
Each step runs a transformer on the chain's BPE context, computes one scalar per
candidate word by averaging the logits of that word's BPE tokens, overlays the
live Dario field, excludes used words, and samples from a short list. The chosen
word's BPE tokens are appended to the next context without an inserted separator.
The visible chain is one initial seed plus 12 generated words.

The trained source is therefore directly useful as Haiku's optional lexical
spring: it can suggest and score words underneath the existing syllable, cloud,
MathBrain, RAE, memory, and dream mechanisms. Twelve internal associations need
not become twelve words in the answer. Haiku can still compress their pressure
into three lines and 5–7–5.

Python references: `penelope.py:1002–1020` (BPE-to-word mean), `1254–1277`
(seed), `1280–1388` (chain), `1301–1340` (pool, overlay, masking, top-12 sampling),
`1360–1381` (next context and field updates). The input's longest non-stop word
selects the seed; the original full prompt does not become transformer context.

## Real asset and model layout

- Tracked file: `weights/penelope.bin`, Git blob
  `697664269c3b016f1187376bf7b7557832cf8aeb`.
- SHA-256: `afff67c780a7c78fdc1f76e521dfda30d65e68b95fe5bd324bb392b84ba3cd38`.
- Size: **78,477,152 bytes = 74.8416 MiB** including the 32-byte header.
- **19,619,280 float32 parameters**. Header `<8I>` is
  `[0x50454e37,2048,1984,448,896,7,8,256]`.
- Eight layers, width448, FFN896, seven attention heads of64, maximum256 BPE tokens.
- Global tensors in file order: token embedding2048×448, learned positional
  embedding256×448, final RMSNorm448, LM head2048×448.
- Each layer in file order: attention RMSNorm448; five448×448 matrices
  Wq,Wk,Wv,Wo,Wr; two gate values; FFN RMSNorm448; Wgate896×448,
  Wup896×448, Wdown448×896. The source's comments sometimes state transposed
  shapes, but `matmul_mv` consumes output-row-major matrices in these dimensions.
- RMSNorm epsilon **1e-5**; learned absolute positions and split-half RoPE are
  both active; RoPE base10000. The RRPRAM branch here is a plain `Wr @ h`
  projection, blended with ordinary causal attention by a learned 2-way softmax.
  It must not be replaced by NoTorch's separately named positional RRPRAM
  attention operation. FFN is SwiGLU; source SiLU returns zero for x≤−20.
- Model source: `penelope.py:735–999`, C `2304–2777`.

The numerical model and the word table are distinct. The file contains learned
numbers; its BPE vocabulary is the static1792-merge table plus256 initial byte
tokens in the source. The public README reports training from scratch on85MB
Gutenberg, loss1.96; this audit loads and executes the shipped asset.

## Candidate vocabulary and live field

Direct Python initialization measured **1984 entries /1703 unique spellings**,
then **586 additional BPE-decoded entries**, for2570 candidate slots. C/AML's
shorter fragment filter produces2581 slots (+597). The README's ~3000 is an
approximation from a different filter stage. The extension is decoded from the
static BPE merge table (`558–657`), then evaluated by learned model logits.

Python includes a larger suffix-fragment blocklist and an additional post-pick
vowel/fragment check. C does not have that second check. A real C weighted probe
selected `ular`; thus an AML lexical-source module should keep provenance and
an explicit whole-word admission policy. It can initially score known Haiku/cloud
and curated Penelope words, then add the reviewed decoded whole-word pool. This
keeps admission behavior inspectable without a new runtime dependency.

The Python Dario overlay (`1168–1238`) has a symmetric Hebbian co-occurrence map,
eight destiny categories, a prophecy word/age, trauma, and six coupled chambers
(fear,love,rage,void,flow,complex). For each core candidate it adds:

1. Up to0.3 times the chamber-modulated recent-eight co-occurrence signal.
2. `0.5*log(1+prophecy_age)` for the prophecy target.
3. A chamber-modulated0.25 times normalized category destiny.

The prophecy is chosen from deep categories2,5,7. Chamber updates use step/8
depth, Kuramoto coupling0.02 and individual decays. Chosen core words reinforce
their category as `0.3 +0.7*old`; trauma rises after step7 and decays0.97.
The field persists between chains in one REPL process. It is not saved in PEN7.
Python's bigram container is unused in this path; extended candidates receive
the learned score, while the named field overlay applies to core entries.

## Weightless behavior and the correct Haiku fallback

The HTML implementation is the genuine no-model source reference. It starts
`model=null` (`penelope.html:706`), and `runChain` selects `selectWordField`
when no weights are loaded (`1230`, `1292–1300`). It does not initialize or run
the transformer in that branch. The browser file-loader catch also restores
`model=null` (`1391–1409`).

Its full field scorer (`1035–1087`) contains bigram, Hebbian, prophecy, destiny,
trauma, occasional category jumps, step-dependent vibe and temperature; the
field-only sampler adds shrinking noise and samples top12 (`1120–1140`).
This is richer than Python v7's H/F/A overlay. HTML's trained sampler uses top16,
and field updates include bigrams and recent-four co-occurrences.

Python/C/AML v7 all still initialize and forward a random19.6M model in their
mode labeled "weightless". In Python `has_weights` mainly chooses the candidate
pool and repeated-word masking. Python ignores an absent load path and retains
that random model (`1436–1439`). C and AML set `has_weights=1` after attempting
load even if `model_load` returns0 (`penelope.c:3474–3475`, AML1777–1778).

For the requested Haiku behavior: missing optional model means the lexical
branch is absent; normal seed/cloud Haiku continues silently and with identical
RNG advancement to the no-neural baseline. Load optional tensors once, validate
whole-file identity/layout, and only expose a fully loaded model. Model tensors
should remain outside clone-per-turn conversational state; persist model identity,
field state and a separate optional-source RNG instead.

## AML implementation and executable receipts

`penelope.aml` is1819 lines, with14 `BLOOD COMPILE` blocks and `BLOOD MAIN`
containing the C engine. `ariannamethod/ariannamethod.c` is the vendored mini
compiler, providing BLOOD COMPILE/MAIN/LINK, ECHO, and C compilation. This is a
source/reference path for the port; the Haiku organism should remain actual AML
orchestration over canonical NoTorch primitives. Vendoring cleanup belongs to a
separate Penelope task, as the user requested.

Executed, bounded, with all binaries/probes outside the tracked repo:

- C existing unit suite: **7652 passed,0 failed**,1.99s,peak191352KiB; includes
  valid PEN7 save/load, matrix shapes, BPE, non-flat forward, field checks.
- AML existing suite: **15 passed,0 failed**,4.56s,peak79772KiB;14 BLOOD blocks,
  compiled output, word generation, exact saved byte count, emitted1667 C lines.
- C full weighted chain, srand42 reset after model initialization:4.04s,
  peak80160KiB. Prompt `darkness eats the city`:
  `darkness → bud → shadow → sing → ular → hinterland → glass → sand → psalm → loss → wing → crossing → satin`.
- C no-file/random-initialized chain, same controlled seed:3.70s,peak79904KiB:
  `darkness → lemon → nausea → towel → dream → wound → spacetime → vault → child → murmur → muscle → hearth → murmur`.
  The repeated spelling follows from index-only exclusion in no-file mode and
  duplicate core spellings. Weighted mode excludes by word string.
- Unchanged HTML field algorithm in Node, only UI/delays stubbed, declared
  LCG32 seed42, neural `forward` replaced with a throw to verify no call:
  `darkness → cell → ghost → fever → temple → translation → distance → pilgrimage → tremor → ministry → iguana → corpse → skin`.
  Twelve generated steps;45 co-occurrence entries;12 bigram roots;0.274s,
  peak52700KiB including Node. This is a headless algorithm receipt, not browser
  rendering or cross-language sequence parity.
- Original Python `Penelope.forward`, with memoryviews of the shipped float32
  tensors instead of random initialization: `consciousness` encodes to `[1473]`;
  one forward1.006s, all19,619,280 tensor values and2048 output logits finite.
  Leading distinct word scores before the field overlay:
  sand4.795668117895421, oath4.447393413217365, hero4.246591368877356,
  boat4.243682096628874. All2048 logits are in
  `penelope_python_forward_probe.json` for the NoTorch comparison.

Raw logs and harnesses: `penelope_unit_tests.log`, `penelope_aml_tests.log`,
`penelope_weighted_probe.log`, `penelope_weightless_probe.log`,
`penelope_html_field_probe.log`, `penelope_probe.c`,
`penelope_html_field_probe.cjs`, all under the shared scratch root.

## Paired Penelope / NanoJanus proposal

The NanoJanus audit independently confirmed both models have the same PEN7 tensor
shapes and exactly matching1792 BPE merges, but different weight bytes. Its
SHA-256 is `d675df2ee3a150d63bc73935a84060280d0f6b0aa84283e4b78ab905180b576c`.
Two loaded f32 parameter sets occupy **149.68MiB** plus activations and optional
caches; no optimizer buffers are needed for this inference use.

Their shared BPE coordinates make two learned lexical currents concrete: each can
score a common word pool, while AML owns forward/backward association histories,
their agreement or tension, and what reaches the cloud. Penelope's12 associations
can remain an internal path. Haiku's form/quality selection remains the visible
mouth. An independent recorded source budget avoids overwhelming the seed/cloud
with all candidate words at once. A disabled/missing branch leaves the previous
Haiku trajectory intact. The glyph layer can later consume the association path
and return semantic attraction without having to replace any of these interfaces.

My recommended first integration is optional PEN7 lexical proposals/scoring with
the exact no-file continuation check, then the bidirectional path and its saved
field state, then glyph attraction. This yields three measurable layers while
preserving the small surface the user wants.


This subaudit preceded Oleg's further choice of parameter mixing inside each
presence. For the combined design and that refinement,
see [The Haiku iceberg](../../ICEBERG.md). Portable probes are in
[the research bundle](../../../tests/research/iceberg/README.md).
