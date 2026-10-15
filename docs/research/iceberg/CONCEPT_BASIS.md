# Frozen PEN7 concept bases

This experiment compares two ways to represent the same four concepts inside
the frozen PEN7 bodies: one exact anchor word, or the centre of several already
reviewed words. The preceding [word-anchor probe](WORD_ANCHOR_PROBE.md) records
the original isolated-anchor measurements. This stage keeps its four canonical
IDs and gives each concept a larger set of known examples.

## Protocol fixed before execution

The machine-readable protocol is
[`concept_basis_protocol.json`](../../../tests/research/iceberg/concept_basis_protocol.json).
It was published before execution in commit
[`54aa7e5`](https://github.com/ariannamethod/haiku.aml/commit/54aa7e5763f05100a007adb41d859c83c7eb7fcf),
SHA256 `729a2d56ac782e7adcd06e44dc5c0ed7c43eef3faeb9f47803074bdca834c811`.
Its primary comparison is top-one agreement on 24 newly selected words, using
the physical parameter mixture at alpha .5. The A-only and B-only lanes repeat
the same inputs and methods in their own spaces. Six words belong to each of
water, tree, light and music. Eight unrelated or invented words are separate
controls. The earlier fifty sample rows remain diagnostics, with their sixteen
previously unbound words reported as a separate regression group.

The training vocabulary comes directly from the existing English binding table
in `src/parliament_contact.aml`:

| Concept | Canonical ID | Reviewed training words |
| --- | --- | --- |
| water | 0 | water, rain, river, sea, ocean, lake |
| tree | 4 | tree, forest, wood, leaf |
| light | 6 | light, sun, sunrise, dawn, bright |
| music | 82 | music, song, melody, guitar, piano, drum |

These 21 words supply the complete training set. The new evaluation words and
the previous sixteen unbound words supply no prototype vectors. Every training
word must still resolve to its declared canonical ID through the actual table;
the new held-out words must remain absent from that table.

| Concept | New held-out words |
| --- | --- |
| water | brook, creek, pond, puddle, waterfall, lagoon |
| tree | cedar, willow, spruce, maple, sapling, acorn |
| light | illumination, luminosity, sunbeam, lantern, glimmer, gleam |
| music | cello, flute, trumpet, symphony, orchestra, sonata |

The eight controls are `receipt`, `screwdriver`, `omelette`, `kilogram`,
`vexlorn`, `prandek`, `zuvrel` and `norkesh`. All 32 new inputs were selected by
an independent reviewer before model access and are absent from the 130 static
English bindings and the preceding fifty-row probe. The new primary set and
the old sixteen-word regression set keep separate denominators.

## Two methods, three separate model spaces

The baseline uses the final 448-value hidden vector for the exact isolated word
`water`, `tree`, `light` or `music`. The candidate normalizes each training
word's hidden vector, sums these vectors within its concept in the declared
order, then normalizes that sum. Each word has equal weight within
its own concept. Both methods score an input with the same raw cosine formula.
Squared norms must be finite and positive before normalization.

Each exact input is forwarded once per provider lane and its hidden vector is
reused by both methods. The lanes are A alone, B alone, and the actual .5
parameter mixture. Each lane builds its own four anchors and four centroids.
All vectors retain the body and tokenizer identity of the forward that made
them. The midpoint mixes the frozen parameters before inference.

For a score tie, the earlier canonical ID wins. Results retain all four raw
scores, the selected ID, the runner-up margin, and whether the label agrees
with the predeclared group. Primary reporting includes the paired wins and
losses between methods and the counts for each concept. There is no fitted
acceptance threshold in this protocol.

## Measured on 2026-10-15

The [complete result](../../../tests/research/iceberg/concept_basis_probe.json)
retains every token ID, raw score, margin and paired outcome. The primary .5
mixture moves from **6/24 to 8/24**. Four words become correct, two become
incorrect, four stay correct and fourteen stay incorrect.

| Space | Isolated anchor /24 | Seed centroid /24 | Both correct | Centroid only | Isolated only | Both incorrect |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| A only | 11 | 8 | 6 | 2 | 5 | 11 |
| B only | 7 | 8 | 2 | 6 | 5 | 11 |
| A/B at .5 — primary | 6 | 8 | 4 | 4 | 2 | 14 |

The following counts are isolated anchor → seed centroid, each out of six:

| Space | Water | Tree | Light | Music |
| --- | ---: | ---: | ---: | ---: |
| A only | 4 → 5 | 4 → 0 | 1 → 0 | 2 → 3 |
| B only | 2 → 4 | 4 → 1 | 1 → 0 | 0 → 3 |
| A/B at .5 | 2 → 2 | 0 → 1 | 4 → 3 | 0 → 2 |

At .5, the centroid gains `creek`, `cedar`, `symphony` and `sonata`, and loses
`waterfall` and `luminosity`. A-only loses all four of its correct tree words.
The direction of the change therefore depends on the body and the concept.
The protocol's primary comparison stays at .5; A's larger baseline count is
reported as its prespecified secondary result.

On the separate sixteen-word regression set, A moves **1 → 4**, B **4 → 6**,
and .5 **2 → 5**. At .5, the five new correct cases are `stream`, `tide`,
`birch`, `rhythm` and `harmony`; the old correct `photon` and `radiance` are
lost. All fifty legacy diagnostic rows reproduce their original .5 baseline
winner and raw cosines within `1e-6`.

Controls have no expected concept. Their forced four-way choices stay outside
the accuracy counts. These are the .5 centroid observations:

| Control | Top concept | Cosine | Margin |
| --- | --- | ---: | ---: |
| receipt | music | .336412 | .004108 |
| screwdriver | water | .321038 | .007777 |
| omelette | water | .488853 | .016186 |
| kilogram | music | .488573 | .052688 |
| vexlorn | light | .466385 | .085985 |
| prandek | music | .435920 | .061843 |
| zuvrel | music | .286480 | .038015 |
| norkesh | music | .448295 | .071020 |

The invented `vexlorn → light` exceeds the correct `illumination → light` in
both cosine (`.466385` versus `.443781`) and margin (`.085985` versus `.064563`).
The experiment fits no confidence gate. Live word bindings continue to use
accepted independent external encounters and their existing ownership rules.

## What follows from this measurement

Several reviewed words change the geometry and recover some associations;
their equal-weight centre also loses existing associations. The next proposed
comparison gives those reviewed words personal weights from each presence's
accepted encounters, then measures which later word ranks and admissions change
against the working weightless field and this equal-weight baseline.

That proposal needs a versioned witness cutoff and provider-specific prototype
banks. A query and its prototypes must come from the same model space. A hidden
vector from the physical parameter mixture cannot be projected into an A-only
or B-only bank. Mixing endpoint scalar scores would be a separate experiment.
The first return path is word ranking through the existing contact boundary;
direct changes to Janus balance need their own later-witness comparison.

## Reproduction and receipts

The AML probe, checker and complete paired scores live beside the protocol.
AML and NoTorch perform the forwards and vector arithmetic; the checker uses
Python's standard library to validate the recorded protocol and outputs.
Interpreter and compiled runs use the same frozen inventory. Each lane repeats
one exact word and compares all 448 hidden values; unavailable model handles
exercise the quiet absence path. The measurement retains source, runtime,
model, protocol and output hashes.

The measured checkout was Haiku `54aa7e5763f05100a007adb41d859c83c7eb7fcf`,
on merged organism `702ae6ae5098d1002b362abb01b00eaf51f6ff28`, with the probe
files identified below. Dependencies were AML
`2e17a2887449c89fdd8c066ebdcdaa1e448346cf` and NoTorch
`f6504cf8e8799c6a01b26a75ab90f004a6a3d804`. NoTorch's complete tree is
`cf5952743685417d3bf63ddb34088565262dd165`, identical to merged revision
`ca3e17a24852ad1db08a6e6b908fb37036011d2d`. Both source pins are public.
The protocol and result record the full model/tokenizer identities and
fingerprints of the actual source and scalar runtime files.

| Receipt | SHA256 |
| --- | --- |
| AML probe | `f678f58db0b568b743a79329be4a2c0c1df2fb01252eaf6c8e06fadc52781ae7` |
| Checker used for measurement | `bbbea1d069ebdbe03ec2b24b5280517c80cd8b52a2ded2440f08fc296fb2efd5` |
| Checker after entrypoint verification fix | `274131309c1e1597397e20df0bf9186218460eedf358fa781f950019156f624a` |
| Captured generated entrypoint | `53d389b96dde63304edcba8ae3201aea894ac0eeceda29b7eb1ae2e3096d3282` |
| Complete measured result JSON | `15026c56e25441f529f650e642f03882d68147dc336c72e30ac711a3b70e3899` |
| Each mode's complete stdout | `8e91b5baf3845264aab507ef98012f84eb807c3ad857d5a7239c9e476a5b338a` |

There were **273 successful forwards in each mode, 546 total**. Inputs used
1–7 tokens. Interpreter and scalar executable stdout matched byte for byte;
their execution stderr files were empty. Measured durations were 22.616s and
22.508s. Compilation succeeded in .054s; its 327 stderr bytes were ordinary
compiler progress. Before/after source, runtime and model fingerprints agreed.

An initial generated entrypoint put a ninety-value array on one AML line.
The interpreter rejected that line before loading a model or forwarding any
input. Emitting bounded individual assignments fixed the launcher; the
registered inventory and numerical code stayed unchanged.

Review also found that the checker recorded the generated entrypoint's hash
without comparing it again. The measured entrypoint still exactly matched its
original hash. The published checker now requires that comparison, while the
original checker remains available in the measurement commit. The original
preflight receipt and result bytes are preserved. A separate
[post-run validation receipt](../../../tests/research/iceberg/concept_basis_validation.json)
records the independently verified entrypoint and the sole subsequent
checker-file change; it performed zero new forwards.

Use clean sibling AML/NoTorch checkouts at the revisions above, the scalar
build commands in the main README, and both checked-in GGUF files. The frozen
protocol fingerprints the complete contact source. Reproduce this historical
experiment from its report revision, which includes the stricter checker:

```sh
git worktree add --detach ../haiku-concept-basis b75ac3d4e941d237a4010c55f6ed03c5d1fe1749
cd ../haiku-concept-basis
```

The sibling location preserves the dependency paths. From that worktree root:

```sh
set -eu
haiku_probe_work=$(mktemp -d)
python3 tests/research/iceberg/check_concept_basis_probe.py prepare "$haiku_probe_work"
AML_PREFIX="$haiku_probe_work/prefix" ../ariannamethod.ai/tools/amlc \
  "$haiku_probe_work/run.aml" --scalar -o "$haiku_probe_work/probe" \
  > "$haiku_probe_work/compile.out" 2> "$haiku_probe_work/compile.err"
../ariannamethod.ai/runner/aml-notorch "$haiku_probe_work/run.aml" \
  > "$haiku_probe_work/interpreted.out" 2> "$haiku_probe_work/interpreted.err"
(cd "$haiku_probe_work/foreign" && ../probe) \
  > "$haiku_probe_work/compiled.out" 2> "$haiku_probe_work/compiled.err"
test ! -s "$haiku_probe_work/interpreted.err"
test ! -s "$haiku_probe_work/compiled.err"
python3 tests/research/iceberg/check_concept_basis_probe.py check \
  "$haiku_probe_work/interpreted.out" --compare "$haiku_probe_work/compiled.out" \
  --receipt "$haiku_probe_work/before.json" --write "$haiku_probe_work/result.json"
```

Each command must succeed. The work directory retains compilation logs,
execution outputs, generated sources, binary and receipts. The entrypoint hash
includes the absolute import path, so a different checkout location records
its own path-specific hash. The checker verifies that hash within the run;
the experiment's protocol, input inventory and measured scores remain explicit.
