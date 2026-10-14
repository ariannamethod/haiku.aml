# Haiku iceberg research: actually.life and Leo glyph organs

Read-only audit, 2026-10-13. No source or repository state edited. Repository instructions, manifesto and relevant README/code read. The probes below exercise the pinned C functions by including the original source with a renamed `main`; all probe files and executables live beside the checkouts, not inside them.

## Source identities

- `ariannamethod/actually.life`: `2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8`.
- `l.c` SHA-256: `db2b9d0a667147a4d344644e2926064d2841cb2e44ad49d75d6de037fe4c3147`.
- `l2.c` SHA-256: `77c9b045db495a331b661257c79aa2e07e2e01fd385d0cf5528c074cf4e346b7`.
- `ariannamethod/leo`: `b842f6f480da928c666f8685e87cac6f6b899e90`.

Primary code links: [actually.life l.c](https://github.com/ariannamethod/actually.life/blob/2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8/l.c), [l2.c](https://github.com/ariannamethod/actually.life/blob/2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8/l2.c), [Leo](https://github.com/ariannamethod/leo/blob/b842f6f480da928c666f8685e87cac6f6b899e90/leo.c).

## What L1 and L2 actually share

`l.c` and `l2.c` are two separately living bodies. Their exact source diff at this pin is `CHOOSE_TEMP0` 0.7 versus 0.85 (line 325) and the out-of-memory label (line 571). Each process owns its weights, state, scars, glyph counts and invented symbols. These are two participants sharing a world, not a text encoder and decoder stacked as abstraction levels.

Their coupling is explicit:

1. **Contested food.** `arena_index` now indexes immutable `lifeis/world.txt` only (`l.c:1370–1374`); earlier README language about every dropped `.txt` describes the old arena. `arena_next` reads a locked shared claims ledger and reserves a corpus line. A claim carries owner, time, hunger, tick and dissonance; claims expire after 20 seconds (`1313–1328,1383–1395`). Hunger and the observed other's state determine whether to continue one's own front or raid the other's (`1403–1416`).
2. **A shared ether.** `speak` publishes five glyph names with speaker and lineage IDs (`1095–1117`). `ether_graze` retains the latest eight foreign utterances, skips own echoes, and takes one as real input (`1057–1092`). At normal arena ticks the chance of hearing the other is 0.15 (`1318,1988–1989`). A heard symbol can be adopted whole into the receiver's own symbol table.
3. **An optional common field.** A damped 64-site wave ring carries both deposits, and read–deposit–propagate–collapse–write happens under a file lock (`1548–1597`). `NL_MONISM` reads dissonance from misalignment with that ring before one's own new deposit (`1730–1782`). H2 compares the shared ring with a private shadow ring containing only one's own past deposits (`1648–1651,1743–1749`). This is a particularly useful separation of self-history from the pressure arriving from another stream.

The forecast organ is also compact: two linear forecasts of next mood/dissonance from `[1,S,D]`, normalized LMS updates, and prediction error fed into sampling temperature (`912–938,883–886`). That provides an additional prospective/retrospective relation, separate from the two bodies.

## Glyphs: compression, pressure, attraction, growth

### Membrane

`l.c:23–50` defines 88 concept IDs. `54–155` contains 515 alias entries, 507 unique spellings: rain/river/ocean/sea collapse to `water`; sad/cry/mourn/sorrow to `grief`; and so on. Direct base-name lookup precedes the ordered aliases. `BE` is a reflexive operator. Stop-word handling happens before either lookup.

`242–259` lowercases ASCII, replaces all bytes other than ASCII letters/digits/space/apostrophe/hyphen, drops stop words, routes non-stop words, and collapses adjacent duplicate IDs. Input is bounded to 4095 bytes and a supplied token cap (normally 64).

Unknown words use a character-trigram router built from all aliases and base names: each of 4096 hashed trigram buckets stores its dominant concept, then a word votes across its own buckets. With no votes, FNV-1a modulo 88 chooses an ID (`198–236`). The actual measured routes matter: the current README's `inferno → fire` example runs as `inferno → give` at this pin. `AI` lowercases to `ai`, misses the uppercase base name and maps to `have`. A Cyrillic-only line yields no glyphs. These are concrete places to define the Haiku port's Unicode and unknown-word semantics explicitly.

### Two timescales of pressure

Each glyph has a direct mood/dissonance charge and a metabolism multiplier (`502–561`), plus a persistent per-glyph scar. For example fire supplies `+0.05 S`, `+0.10 D`, multiplier `0.2`; water supplies `+0.01 S`, `−0.08 D`, multiplier `1.8`. `BE fire` triples the mode charge. `BE me` instead applies `tanh(1.5*S+0.6)`.

`digest` updates the rank-4 value adapters using the original NoTorch Hebbian formula (`432–463,713–760`), modulates yield by surprise under the glyph-transition field, applies charges and adds per-glyph scars. Separately, eating deposits a full-rank trace onto the used `wv` pathways and time corrodes `wv`; a body-mass ceiling makes strengthened pathways compete for finite mass (`465–499,1938–1941,2007–2010`).

### Semantic attraction is executable sampling pressure

`field_observe` counts directed adjacent glyph transitions (`810–816`). `field_fold` looks back over eight glyphs and folds a geometrically weighted mean of log conditional probabilities into each candidate score; distance weight is `0.6^k` (`825–875`). This is a small context-dependent semantic gravity field. The model's earned-sharpness gate blends that field with transformer logits. `choose` adds `0.5*scar[g] + 5*charge[g].S*current_S` and samples with state-dependent temperature (`883–909`).

It therefore has both lexical equivalence from the membrane and lived semantic associations from the transition field. The map is a seed; the evolving transition matrix and state-dependent charges decide what becomes attractive now.

### Dreams can create a new alphabet

Adjacent pairs accumulate while awake. During a dream, the strongest unused observed pair at count at least 14 can become a new symbol (`590–626,1297–1305`). There are 64 reserved slots after 88 concepts and two control IDs. Both parents may already be compounds, but their IDs always precede the child.

The child receives the mean parent embedding and output-head row, mean mood/dissonance charges, and geometric-mean metabolism. It is a real new model ID, with its own scar/field rows available, not just display text. The recursive name is unambiguous, for example `(fire+water)+tree` (`763–779`). A receiving body recursively reconstructs or reuses the same parent structure when hearing it (`1009–1050`). New pair creation in dreams and adoption from another body are separate birth paths.

The ordinary English membrane does not automatically merge every later occurrence of a learned pair. Compounds enter through generation and whole-symbol ether parsing. This distinction should be kept explicit when designing compression in Haiku.

Dream replay calls digest and may create a symbol, but does **not** increment externally observed transition/co-occurrence counts (`1267–1276,1297–1305`). That is a valuable mechanism to preserve when coupling Haiku's own dreams to a new semantic field: internal replay has its own provenance and effect.

## Leo's complementary lesson

At its current pin, Leo uses BPE for the mouth and the same 88 concepts as a parallel School (`leo.c:126–147`). Its ordinary meaning vectors are separate 48-dimensional distributed co-occurrence vectors (`1463–1484`); later hearing adapts them toward contextual neighbors (`2180–2209`).

School keeps up to 256 unknown-word records with hearing count and either an assigned glyph or `−1` (`383–396,1757–1783`). Hearing alone does not assign a concept. A sufficiently safe chamber state permits a question; two matching known concepts in the context permit a tentative guess (`1903–1926`). The next human answer is compressed into a concept histogram and assigns the unknown word to the strongest eligible concept. A contradicted guess adds 0.4 to the complex chamber (`1883–1900`). The bindings and outstanding question are persisted (`2950–2962,3015–3026`).

For Haiku, the directly reusable part is the distinction between a known concept binding, a provisional orthographic suggestion and an unknown that is still open. The visible School question format is a separate interface choice.

## Bounded executable receipts

Built against the pinned source and ran `glyph_actual_probe.c`, `glyph_leo_probe.c`, plus the original `l.c --monismtest` and `--monismhalflife`. No full colony/reproduction suite was run for this design audit. GCC emits existing formatting warnings; strict `-std=c11` also exposes missing POSIX declarations. The original README default build and the bounded internal probes completed.

`glyph_actual_probe.txt`:

```text
I remember the river and the sea => me remember water and water
rain ocean water fire flame burn => water fire
the sun is fire and i feel fear in the dark => light BE fire and me BE fear dark
inferno dragons quantum spaghetti obliteration => give anger woman light idea
AI BE me => have BE me
дождь => [empty]
water row=4, water→tree=3, water→light=1
folded tree=-0.287681 light=-1.386290 fire=-13.815511
pair count13: 0 new symbols; count14: fire+water, ID90
fire+water: dS=.03, dD=.01, metabolism=.6
recursive ID91: (fire+water)+tree; resolver returns ID91
fire: S=.049958 D=.100050 scar=.001
BE fire: S=.148885 D=.300050 scar=.001
dream: external transition count 4→4; context grows 1→2
sizeof(Model)=409152 bytes; each 154×154 f32 table=94864 bytes
```

`glyph_leo_probe.txt`: unknown `nebula` stays `−1` after one hearing; answering its pending question with `river ocean` binds it to water. A prior fire guess becomes one recorded miss and raises the complex chamber from 0 to .4.

`glyph_monism_probe.txt`: foreign-pattern disorder `.9568`, self-pattern disorder `0`; forcing clamp holds at `4`; the supplied foreign-dent probe has half-life **2 ticks**. The source's experiment records in `tests/verdict_raw/results/` preserve the broader coupling measurements. For a durable Haiku semantic memory, named associations and explicit retained history are the natural storage, while a fast shared field can carry current pressure.

## Minimal Haiku organ — design proposal, not current implementation

1. Add an AML **glyph membrane** beside the existing tokenizer: input, spoken verse and inner dialogue receive concept IDs and origin/turn provenance. It does not replace the three-line mouth. Start from the shared concept seed, explicit Unicode normalization and persistent word bindings.
2. Maintain a **small concept transition field** and recent concept history. A plain 88×88 f32 table takes 30,976 bytes; two directional tables take 61,952 bytes. Even 154×154 takes only 94,864 bytes per table. They can initially be sparse AML data, then use canonical NoTorch numerical operations where appropriate.
3. Give prospective and retrospective streams different traversal and retained state, while both read the same concept identities. The forward stream asks what this lived association draws toward; the backward stream supplies the paths and fragments that made it attractive. Janus can provide the directional/calendar mechanics; actually.life supplies a shared semantic alphabet and differentiated self/other histories.
4. Convert their current field pressures into bounded score contributions over Haiku's actual available words. Haiku's existing syllable/form machinery composes the outward verse. Optional Penelope/Janus weights enrich the available lexical material through the same membrane and word provenance. With no weights, existing cloud/seed words still inhabit and feed this layer.
5. In dream consolidation, create stable, bounded, recursively defined compounds from recurrent lived paths. Retain parents, formation turn and contributing source events. Separate replay effects from new external observation counts, following the source's actual `dream_once` path.
6. Evaluate with fixed-input/restart receipts: different lexical aliases can cause the same glyph path; changed history changes attraction; forward/backward history changes different parts of pressure; a compound survives saving/restart; module disabled or absent weights consumes no extra foreground RNG. Each new organ should have an independent off-switch while the current Python-equivalent profile remains reproducible.

This gives the iceberg a concrete lower life: words are compressed into relations that persist and exert pressure, and a short poem is one surface event of that longer semantic trajectory.


This subaudit preceded Oleg's further choice of parameter mixing inside each
presence. For the combined design and that refinement,
see [The Haiku iceberg](../../ICEBERG.md). Portable probes are in
[the research bundle](../../../tests/research/iceberg/README.md).

## Follow-up, 2026-10-14: two births through the actual dream path

The historical audit above is retained. Its `glyph_actual_probe.c` creates the
second-level symbol by calling `birth_symbol` directly. The new
[`glyph_dream_probe.c`](../../../tests/research/iceberg/glyph_dream_probe.c)
exercises both levels through `ingest` and `dream_once`, including their
13/14-observation boundaries. This is measured Actually.life behavior; the
Haiku experiment described below is the next implementation step.

The source is again `ariannamethod/actually.life` at
`2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8`, with `l.c` SHA-256
`db2b9d0a667147a4d344644e2926064d2841cb2e44ad49d75d6de037fe4c3147`.
The exact pinned file was fetched again and its hash matched the earlier audit.
Primary paths are [`l.c:590–626`](https://github.com/ariannamethod/actually.life/blob/2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8/l.c#L590-L626)
for observed pairs and birth,
[`1009–1050`](https://github.com/ariannamethod/actually.life/blob/2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8/l.c#L1009-L1050)
for recursive recognition/adoption, and
[`1267–1305`](https://github.com/ariannamethod/actually.life/blob/2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8/l.c#L1267-L1305)
for the different ingestion and dream paths.

With seed 42, thirteen ingestions of `fire water` followed by a dream produce
no symbol. A fourteenth ingestion still leaves the alphabet unchanged; the
following dream creates `fire+water` as local ID90. Thirteen external
whole-symbol inputs `fire+water tree` and a dream retain one symbol. The
fourteenth such input and the next dream create `(fire+water)+tree` as ID91.
Every dream preserves both the co-occurrence total and directed field total.
The source counts adjacent observations; Haiku's proposed threshold uses
fourteen distinct external turns.

```text
dream external cooccurrence 13->13 field 13->13 symbols=0
dream external cooccurrence 14->14 field 14->14 symbols=1
first id=90 structure=fire+water threshold=14
ordinary postbirth text remains 1,0,4
dream external cooccurrence 27->27 field 27->27 symbols=1
dream external cooccurrence 28->28 field 28->28 symbols=2
second id=91 structure=(fire+water)+tree threshold=14
compound-context pressure tree=0.000000954 music=-13.815510750
receiver adopts same structure at private id=92; external counts=0 field=0; repeated resolve reuses id
RECURSIVE_PRIMARY_SOURCE_OK
```

Three distinctions matter for the port:

- `semtok_line("fire water tree")` still returns the three original IDs
  `1,0,4` after the first birth. The source's ordinary membrane does not
  compact a known pair. The probe's second phase uses `semtok_ether`, which
  resolves `fire+water` as one existing symbol before ingestion. Haiku needs
  an explicit recognition rule to obtain recursive observations from ordinary
  later words.
- The later pressure is executable: with the existing compound as context,
  `field_fold` gives tree `0.000000954` and music `-13.815510750` after the
  fourteen compound-to-tree observations. Those are source field logits;
  Haiku will translate its own retained relationships into bounded word-score
  contributions while keeping the PEN7 parameters frozen.
- Adoption is a separate path. A fresh receiver is deliberately given
  `tree+light` as its own ID90; resolving the sender's recursive name creates
  its `fire+water` at ID91 and the nested symbol at ID92. The parent structure
  agrees despite different local IDs. Repeating resolution reuses ID92, and
  adoption alone adds no observations or field counts. The C source transmits
  names and speaker/lineage IDs; Haiku's later exchange needs its exact
  retained encounter ancestry as well.

The probe executes the source's original `digest` updates on its small C
model. It does not load PEN7. Haiku's new symbols will live in owned AML
records, with immutable shared source weights.

To reproduce from the Haiku repository root, point `iceberg_sources` at the
external checkout directory:

```sh
iceberg_sources=/absolute/path/to/checkouts
iceberg_work=$(mktemp -d)
test "$(git -C "$iceberg_sources/actually.life" rev-parse HEAD)" = \
  2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8
sha256sum "$iceberg_sources/actually.life/l.c"
cc -O2 -std=gnu11 -I "$iceberg_sources/actually.life" \
  tests/research/iceberg/glyph_dream_probe.c -lm \
  -o "$iceberg_work/glyph-dream"
"$iceberg_work/glyph-dream" > "$iceberg_work/glyph-dream.txt" \
  2> "$iceberg_work/glyph-dream.err"
test ! -s "$iceberg_work/glyph-dream.err"
sha256sum tests/research/iceberg/glyph_dream_probe.c "$iceberg_work/glyph-dream.txt"
```

Measured exit status was zero; runtime stderr was empty. GCC emitted the
source's existing recursive-name truncation warning at `l.c:775`. Probe source
SHA-256 is `6da7e060d65ce5518d4ff0cc7bf241734603e675b2416c78a1bf5bf05deeeb80`;
stdout SHA-256 is
`f265b2c08ffefe3e1bc74eb518d75bbbc0c6323ad8100b3e4b41232f7d7632b2`.

### Next Haiku experiment

Keep four record leaves for the symbol organ and at most sixteen private
nodes. A tree has at most four levels and expands to at most sixteen anchor
leaves. Count a pair once per eligible external turn. Consolidate at
most one new node after an actual completed dream, after that turn's contacts
and admissions. A node's canonical ordered parent structure is shared meaning;
its local slot, supporting encounters, recognition history and pressure belong
to its owner. A newly created node starts affecting later encounters.

Use the reviewed seed aliases alone for the first corpus: rotate `fire water`,
`flame river`, `burn rain`, `fire sea`, `flame ocean`, and `burn lake` over
fourteen distinct turns. Check thirteen observations and the still-awake
fourteenth before the actual dream. After that birth, give fourteen new
three-word encounters by appending `tree`, `forest`, `wood`, or `leaf`. Their
recognized path is the existing compound followed by tree. Keep all earlier
receipts unchanged: retrospective rescanning must not mint support for the
second birth.

These words are already fixed aliases, so the fixture isolates composition
from learned-word promotion. Further controls should freeze a learned binding
at each original encounter, preserve unbound-word barriers, reject replay as
new evidence, and compare equal canonical structures in independently lived
owners with different local slot order. Compare later glyph features and word
scores at fixed candidate words, base scores, state and PEN7 mixture; verify
owned history, save/restart, capacity, off-switch and random-stream equality.

The first Haiku stage covers local birth, canonical recognition and later
bounded pressure. Structured peer adoption remains a separate next step:
only an actual delivered dream encounter may carry a sender-owned structure;
the receiver keeps its own counts and ancestry, and duplicate delivery remains
an exact no-op.
