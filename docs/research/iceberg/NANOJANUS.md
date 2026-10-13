# NanoJanus / Janus — Haiku iceberg source audit

Read-only research, 2026-10-13. Repository `ariannamethod/janus`, pinned commit
`5a945a9d7c1dc9abfa217f685f0a81455f9ac493`. Read `CLAUDE.md`, the Method Manifesto,
README and the implementation. Repository remains clean. This receipt separates
measured source behavior from the integration design proposed at the end.

## What is immediately reusable

NanoJanus has an actual checked-in trained PEN7 checkpoint, the same model layout
and identical 1,792 BPE merge rules as Penelope. The payloads are different.

| Checkpoint | Bytes | Parameters | SHA-256 |
|---|---:|---:|---|
| `janus/weights/nanojanus.bin` | 78,477,152 | 19,619,280 f32 | `d675df2ee3a150d63bc73935a84060280d0f6b0aa84283e4b78ab905180b576c` |
| `1984/weights/penelope.bin` | 78,477,152 | 19,619,280 f32 | `afff67c780a7c78fdc1f76e521dfda30d65e68b95fe5bd324bb392b84ba3cd38` |

Each has the exact 32-byte little-endian eight-int header
`[0x50454e37, 2048, 1984, 448, 896, 7, 8, 256]`, then precisely the f32 model
payload and no trailing Janus state. Both together occupy 156,954,304 bytes on
disk, about 149.68 MiB. Activation buffers and runtime objects are additional.

NanoJanus weight introduction commit
`4b0f344eba3014bdcdd047eb0a2ebf4b2032670a` records 85 MB Gutenberg, 25,000 AdamW
steps, loss 1.97, and the replacement of old dual A/B matrices with a single
weight set. These training figures are the repository's recorded training
receipt; this audit ran inference with the checked-in payload.

The model is eight sequential layers: DIM 448, SwiGLU hidden 896, seven heads of
64, max sequence 256, BPE vocabulary 2,048. Each layer performs RMSNorm, causal
QKV attention with RoPE, a DIM×DIM RRPRAM projection, a learned two-way softmax
blend, residual, RMSNorm, SwiGLU, residual. Learned absolute position embeddings
coexist with RoPE. Final RMSNorm and vocabulary projection produce BPE logits.
NanoJanus has no third Janus self-echo attention matrix. Sources:
`nanojanus.py:478–511,562–707`; binary layout `1304–1417`.

There is no AML implementation in this pinned Janus repository. The existing
AML Janus under `ariannamethod.ai/janus/` is a separate project and was not
treated as a NanoJanus parity implementation.

## A bounded vocabulary mouth over a BPE model

`nanojanus.txt` contains 1,984 distinct curated words. The input is encoded to
BPE. A vocabulary candidate receives the arithmetic mean of the current
next-token logits of all its BPE pieces. This is the actual word-scoring rule;
the engine does not autoregressively spell each candidate before choosing it.
It then adds Dario forces and samples from the top eight whole-word candidates.

When weights are present, the candidate set also includes filtered strings
decoded from individual BPE tokens. These strings are derivable from the merge
table; the loaded model supplies their context-dependent scores. Whole-word
output keeps the channel narrow while the model has a much richer internal
context. Sources: `nanojanus.py:207–283,710–870`.

Measured candidate inventories at this pin:

| Implementation | Curated entries | Additional BPE candidates | Total |
|---|---:|---:|---:|
| Python | 1,984 | 765 | 2,749 |
| C | 1,984 | 796 | 2,780 |
| HTML | 1,984 | 594 | 2,578 |

Python strips surrounding spaces and applies a longer fragment filter and vowel
check. HTML requires a complete alphabetic token before any whitespace stripping
and has a shorter fragment blocklist (`nanojanus.html:453–493`). C has its own
filter. An actual C weighted run selected `ers`, which the current C filter
admits. Porting needs one explicit word-admission canon.

An AST comparison with current Penelope confirms all 1,792 merge tuples match.
The curated vocabularies differ: 886 unique words are shared; NanoJanus has
1,098 unique words absent from Penelope; Penelope has 817 absent from NanoJanus.
Penelope's 1,984 entries represent 1,703 unique words. Never combine their word
indices as though they represented the same words; align by strings or BPE IDs.

Python BPE input lowercases and encodes ASCII with non-ASCII characters ignored
(`nanojanus.py:149–165`). This particular lexical source is English; Haiku's
other text/tokenization streams should retain their existing Unicode behavior.

## What the twelve steps actually do

`nanojanus.py:894–1002` and `nanojanus.html:995–1088` generate twelve association
words in total, split between two branches:

`backward_count = clamp(floor(12*(0.3 + 0.4*debt + 0.1*calendar_dissonance)),1,11)`.

The origin is the input's curated word having the strongest accumulated
co-occurrence charge; ties retain the first recognized input word. No recognized
word gives a random curated origin. Both BPE contexts start with the entire
input. The loop alternates a backward word then a forward word while both have
budget. Each selected word is appended to its own branch context. Both branches
share a forbidden set, so the origin and previous choices are excluded globally.

The backward branch is a separate associative trajectory using the same causal
PEN7 forward computation. Tokens are not reversed and no backwards-trained
checkpoint is selected. Its Dario contribution is multiplied by 0.8; the forward
branch uses 1.2. Only forward selections update stored bigram/co-occurrence
counts. The backward branch sees those updates during subsequent alternating
steps. The display stacks the backward trajectory upward in reverse order and
the forward trajectory downward. That topology is directly useful as an
internal Haiku conversation; the visible answer can remain a haiku.

Six Kuramoto chambers are updated for each step; depth selects FLOW/FEAR/VOID
drive, with late COMPLEX, trauma-dependent RAGE, sinusoidal coupling and decay
(`nanojanus.py:393–422`). The Dario overlay uses transition counts, Hebbian
co-occurrence, debt-scaled random fulfillment and a destiny offset, modulated
by chambers, calendar, resonance and direction (`748–786`).

## Calendar, pressure and continuation findings

The intended calendar state uses 11.25 days annual drift, seven 30-day corrections
in a 19-year cycle, a 33-day dissonance scale, and a birth snapshot. The three
implementations currently differ in remainder semantics:

| Days from epoch | Python drift / dissonance | HTML drift / dissonance |
|---:|---|---|
| 365 | 11.2422998 / 0.34067575 | same |
| 740 | −7.2073922 / 0.78159418 | −7.2073922 / 0.21840582 |
| −365 | −221.2422998 / 0.29568789 | 198.7577002 / 0.02296061 |

At day 740 with zero debt this produces four backward Python steps versus three
HTML/C steps. Python `%` makes the negative drift remainder positive, whereas
JS `%` and C `fmod` retain its sign. Negative-year cycle decomposition also
differs (`nanojanus.py:330–347`, `nanojanus.html:533–550`). A shared AML calendar
must define one forward/past convention and carry measured boundary fixtures.

Three specific pressure/continuity paths need attention if adopted:

1. Both Python and HTML call `compute_prophecy_debt([selected],0)`. With only one
   candidate, the selected score is the maximum, so new debt is always zero.
   A source-control probe starts at 0.6 and obtains exactly
   `0.6*0.9**12 = 0.16945772188860006`. From zero it stays zero.
   `nanojanus.c:949–953` explicitly returns zero for this path.
   To make the declared pressure operative in Haiku, calculate the contrast
   against the actual full candidate scores before discarding them.
2. Python/HTML's post-chain prophecy average reads `debt` from `all_steps`, but
   that list holds selector results without a `debt` field. It therefore reads
   zero. The recorded per-step structures have debt, but are not the structures
   used in this average (`nanojanus.py:943–959,973–998`). Record and use the actual
   selected-step receipt.
3. NanoJanus wormholes only mark display rows; they do not skip generation
   steps. The default 0.02 additionally does not pass its `>0.1` activation guard
   (`nanojanus.py:963–974`). Full Janus has actual step skipping.

Python's optional PEN7 save tail stores co-occurrence, bigrams, prophecy accuracy
and debt. It does not save the birth snapshot, chamber values or RNG state
(`1304–1417`). Each CLI launch constructs a new birth snapshot. Haiku's existing
transactional checkpoint should own all optional-organ state, birth, RNG and
checkpoint identities for exact continuation.

## Weightless behavior is already demonstrated in HTML

Python and C always allocate a randomly initialized 19.6M model and run it even
if the requested weight file is absent (`nanojanus.py:800,1464–1467`). Their
weightless mode changes the vocabulary but retains model work and memory.

HTML starts with `model=null`; `selectWord` uses zero learned logits and Dario
alone when no model exists (`nanojanus.html:917–929`). An actual unmodified field
run with fixed RNG produced twelve words with `model_null=true`. This gives the
right optional-source pattern for Haiku: absent payload means no loaded neural
source and no neural forward call. Haiku already supplies its own richer cloud,
generator, history and pressure, so its established behavior is the fallback.

A fresh HTML field has zero co-occurrence and zero debt. Its initial candidates
tie, so stable top-eight selection favors the beginning of the curated word
list. The measured first chain is dominated by body words (`tongue`, `heart`,
`bone`, `eye`, ...). Reusing Haiku's established field avoids importing that
fresh-empty-field selection behavior.

## What full Janus means by two matrices

`janus.c:437–469` and `janus-bpe.c:287–297` hold two entire homologous model
parameter arrays A and B, plus a third blended array:

`alpha = clamp(0.5 + 0.3*(calendar_dissonance-0.5) - 0.2*debt + 0.1*personal_dissonance)`

`W_effective[i] = alpha*A[i] + (1-alpha)*B[i]`.

All parameters, including embeddings, attention, norms and FFN, participate in
this blending. Training alternates the active model on odd/even steps
(`janus.c:1315–1350`, `janus-bpe.c:564–579`). Both learn next-token prediction in
the original direction. A and B are not assigned to past/future directions;
both generation branches use the same blended model.

Full Janus also has the third Janus attention mechanism: projection through Wj,
return through its transpose, self-recognition score, then mutual echo attention.
That is a different model ABI from PEN7, which has QKV plus RRPRAM. The full
`janus-bpe.c` checkpoint magic is `0x4a425045`, with learned BPE merges, both full
model arrays and training state (`588–612`); the Nano/Penelope binaries are PEN7.

Full Janus branches generate sentences instead of single words. Forward
temperature cools per step; backward temperature warms and starts from the
original prompt (`janus.c:1153–1237`, `janus-bpe.c:508–533`). Each sentence
generator receives context length by value: generated bytes/tokens grow the
context inside that sentence, but the outer length is not returned to the next
sentence call (`janus.c:1093–1130`). NanoJanus explicitly retains its branch's
new word context between steps.

## Actual execution receipts

All programs and output receipts are outside the repository:

- `nanojanus-audit`: compiled unchanged `nanojanus.c` with `cc -O2 -lm`.
- `nanojanus-weighted-run.txt`: actual checked-in weighted twelve-word chain,
  prompt `what is the meaning of life?`, exit zero, 3.477 s, 79,160 KiB peak RSS.
- `nanojanus-random-run.txt`: same C path without weights, exit zero, 2.897 s.
- `nanojanus-python-probe.py/.json`: actual Python loader and forward on
  `consciousness` (BPE `[1473]`), all 2,048 logits finite, 0.968 s. Exact loaded
  f32 values were held in standard-library arrays to avoid boxed-float storage;
  original forward arithmetic was unchanged. A separately labeled selector
  stub traced branch order, context growth, memory updates and the debt path.
- `nanojanus-html-probe.js/.json/.log`: source script evaluated in Node with a
  minimal DOM stub, fixed date/RNG and immediate display waits. Actual field
  chain, actual PEN7 load, actual forward and complete weighted chain. Weighted
  twelve-word chain took about 4.5 s. Top twelve logits on `[1473]` match Python
  token IDs and differ by at most `3.737450064988934e-7`.

The Node weighted trace, in actual generation order, was:
`←brass →sole ←pan →ash ←sand →ally →stretch →ginger →alms →oar →rain →sled`.

## Proposed Haiku integration

1. Keep optional PEN7 models as deep lexical sources. They return bounded
   candidate word scores/provenance to Haiku; Haiku's existing cloud, generator,
   syllable/form constraints, MathBrain, RAE and learning own the spoken result.
2. One optional model works by itself. With both payloads available, run two
   distinct branch contexts/model sources. Align their output by word or common
   BPE ID. Start with separate branch proposals or a score blend controlled by
   measured Haiku climate; this preserves the individual checkpoint voices.
   Direct interpolation of homologous PEN7 parameters is an additional concrete
   experiment, evaluated at alpha endpoints and intermediate points.
3. Twelve is an internal association budget, not an obligation to print twelve
   lines. Store branch traces, debts, selected alternatives and subsequent
   admissions. Forward can propose continuations; retrospective exploration can
   use incoming transitions and real journal memories. That would give Haiku's
   Janus layer a backward route through its own accumulated life.
4. Missing weights should leave the ordinary Haiku path operational without
   allocation, model RNG draws, changes to checkpoints or dialogue messages.
   Weight availability/identity can remain inspectable through explicit status.
5. Persist independent model-source RNG, birth/calendar convention, pressure and
   admitted word provenance in the Haiku checkpoint. Failed optional admission
   should not partially publish a foreground exchange.

This route deepens the source of Haiku's words and its private association life
while keeping its compressed external form and the AML + NoTorch dependency
boundary. No product edits or upstream fixes were made during this audit.


This subaudit preceded Oleg's further choice of parameter mixing inside each
presence. For the combined design and that refinement,
see [The Haiku iceberg](../../ICEBERG.md). Portable probes are in
[the research bundle](../../../tests/research/iceberg/README.md).
