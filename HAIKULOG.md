# HAIKULOG

The running engineering log of HAiKU AML. Changes, source revisions, experiments
and their evidence live here. The README carries the organism, its voice and
how to meet it. Work follows the [Arianna Method Manifesto](ARIANNA_METHOD_MANIFESTO.md).

Record fixes, schema changes, numerical details, test counts and reproduction
commands here, newest first. A major new capability also earns a short README
update. The module documents hold the complete contracts; measured failures
belong beside measured successes.

## 2026-10-20 — The cloud continues while you write

From merged `896cf2f80ca79caafca5e03a2a18dad4b24a0050`, both launchers now
default to `background_life=1`. The foreground saves its complete predecessor,
prepared voice and frozen experience before displaying the response. Iceberg
also retains its request and captured day. An owned worker completes learning,
reflection, rings, dreams and deeper contacts, runs the existing validation and
atomically saves the completed life while main reads input. Main joins and
adopts exactly that saved state before processing the next input, quit or EOF.

The temporary pending envelope occupies the same state path. Recovery checks
every prepared foreground leaf and finishes its original exchange before any
requested upgrade; it adds no new response or teaching event. A source absent
at acceptance remains absent for that exchange if it later appears. A ready
source must retain its identity. Save status 2 remains a committed replacement.
`background_life=0` retains the joined control; complete checkpoint schemas and
the Python voice keep their existing order and random streams.

The four stable profiles pass serial/worker/recovery comparisons in interpreter
and compiled execution, with eight byte-identical checkpoint files and eight
rejected pending mutations. The two actual PEN7 bodies carry the retained third
turn through `the stream flows`, day 20747, to the exact saved fourth-turn bytes
in both execution paths; nine provider/request/foreground mutations are refused.
An 18-turn foreground run also matches serial/background/compiled replies and
complete checkpoint bytes, exercising reuse beyond sixteen worker slots.
Native SentencePiece recovery also matches the sequential second turn byte for
byte in interpreter and compiled execution, including launch from another
directory with the saved tokenizer overriding the launcher's regex default.

The actual CLI gates use a test-only pthread barrier and filesystem faults:
the response is already visible while its worker is held; after release the
checkpoint completes with stdin idle. Next input, quit and EOF drain in order.
Process termination, worker failure and failed completed save preserve a pending
exchange that recovers to the sequential bytes; failed pending save withholds
the response. Refused worker launch also retains pending; termination after
completed rename and before join resumes the committed turn once. Final process
checks use native temporary directories and recheck earlier files after later
cases. Earlier workspace artifacts returned older contents with new inodes and
no matching traced write, including serial controls; the final byte comparisons
come from fresh temporary-directory runs. The gates caught an absent-provider
mask that used numeric zero instead of an immutable absent handle.

One warm compiled Iceberg v8 sample, using regex and absent model files, showed
the full reply 2.003175 seconds after input at the ready prompt, while the worker
remained held behind the test barrier. Startup is excluded from that sample.

AML remains `a97abc557730737a6762faf19a4876cc584c1006`, scalar NoTorch
`e84262669b2e349c6436ed0422028107b3d166b3`; both GGUF hashes remain unchanged.
Reproduction uses `make test-background-pending test-background-cli test-iceberg-background AML_ROOT=../ariannamethod.ai`.
The real-model runner
also accepts an existing predecessor/reference pair through `HAIKU_BG_BASE`,
`HAIKU_BG_EXPECTED`, `HAIKU_BG_SOURCE` and `HAIKU_BG_DAY`.

## 2026-10-20 — Audit repairs evidence and the completed voice

Audit of merged `a30992f2fee5fa8396b6ef3191a7c34a58fdd23a` reproduced
three defects. A neural proof could change source text outside its local query,
or name a never-published event inside retained history. An absent endpoint
could retain `1e-30`: its squared-norm zero check underflowed. The source-backed
publisher also accepted a substituted voice RNG; it regenerated the experience
but trusted the completed voice tail.

`src/contact_neural.aml` now checks original text, turn and ancestry against
retained contact/external witnesses, requires events inside the current external
ring to exist, and checks absent endpoint scores individually against zero.
`src/iceberg_session.aml` ties every retained neural source to the actual human
reflection or delivered Haiku dream line. Publication reproduces the complete
voice tail and verified word admission, then compares every completed voice leaf.

The targeted gates cover source and absent-endpoint mutations in interpreter and
compiled execution, seven false complete-life witnesses in both paths, and
three C-host voice substitutions with exact owner/result/checkpoint preservation.
Genuine saved third/fourth turns, missing-body dream, disabled contact work,
prior-centre friend delivery and same-turn dedupe pass. Both source GGUF hashes
remain unchanged. The complete publication fixture now includes 14 refusals;
this audit reran its three new cases. Dependency pins remain AML `a97abc5` and
scalar NoTorch `e842626`. No new documentation files or checkpoint schema.

## 2026-10-20 — Sparse encounters acquire personal meanings

After merged form PR #25, `3a20a95b1e4dfbe6e0a31a548b724df7f4c2e45a`,
model representations now participate in an externally witnessed word binding.
The new [neural word organ](docs/NEURAL_WORDS.md) occupies contact v6 inside
iceberg v8. Both presences own their words and proofs; their two PEN7 source
bodies remain frozen.

The existing learner requires two distinct reviewed local aliases. The new
fallback considers exactly one, from water/tree/light/music, and requires a
positive matching PEN7 concept score with a margin of at least .01. Each
available endpoint uses its own personal centre from the prior-turn snapshot;
endpoint cosine scores mix at fixed alpha .5, or the available endpoint alone.
Three distinct supporting contexts, a 75% majority and a two-context lead
resolve a binding with strength at most .25. Disagreement remains in the six-row
history and can suspend it. Native human encounters teach Haiku; only actually
delivered native Haiku dream lines teach the friend. Existing resolved word
learning and reviewed/manual bindings retain their authority.

Four leaves per contact retain activation/cursors, 32 private words, six
242-value proof rows per word, and their exact text/provider identities.
A row retains the source ancestry, local query and tokens, prior concept clock
and counts, raw endpoint scores, availability and numerical verdict. Each
external encounter considers at most four eligible words. Repeated target
occurrences, duplicate contexts and same-turn repeats supply no extra vote.
Source-backed publication rebuilds every newly accepted observation from the
exact predecessor. Resume audits rebuild available teacher banks and re-forward
retained queries against their recorded personal snapshots.

The original admission route only ranks words absent from the cloud. Review
caught that a newly heard word usually already belongs to the cloud, so its
changed admission score alone would not reach foreground selection. Version 8
now supplies prior neural word pressure directly to the existing five-candidate
recursive RAE selection: `.15 * strength * field[glyph]`, averaged over unique
candidate words, bounded by ±.1875. Candidate generation, form choice and draws
are unchanged. Empty pressure follows the original path exactly. The publisher
also regenerates the frozen foreground experience from the prior contact;
current-turn learning cannot rewrite the reply that taught it.

The complete life has 276 fields. New complete lives select
`neural_bindings=1`; existing v7 checkpoints opt in with `upgrade_neural=1`.
The existing concept-audit API dispatches to the combined bank/proof audit for
v8, avoiding a second bank rebuild. Missing models add no new neural evidence
or random draws and retain earlier associations.

Independent review also caught an AML identifier-length violation, an invalid
activation-array expression, and missing historical-snapshot/event consistency
bounds. These were corrected before the complete-life gates.

The [recorded four-turn conversation](examples/neural-words.txt), generated by
`tests/iceberg_neural_models.aml`, hears `the stream rain`, `the stream river`
and `the stream ocean`. The binding is unresolved after the first two, then
water at strength .25 after the third. The saved-off control hears the same
inputs and retains the original unresolved association. Their three foreground
experiences match exactly: confirmation changes later speech. The fourth
exchange, `the stream flows`, is identical after save/load and worker scheduling.

A controlled foreground comparison from that actual third-turn checkpoint sets
both generation streams to seed 67 and asks `rain`. Actual prior pressure on
`stream` is `0.0000132726582`. With pressure disabled the reply is:

```text
balance tune at
validate visible
configure flow
```

With its learned pressure:

```text
any give pursue
demonstrate reveal stream
yes maybe that fake
```

`tests/neural_foreground.aml` checks both exact replies, identical prepared
voice state and RNG, and unchanged source life in interpreter and compiled
execution. The seed was selected by a bounded 1–128 scan of the retained live
history; it is an explicit mechanism comparison. The original four-turn
conversation keeps its uninterrupted saved stream.

Completed focused gates: pressure arithmetic and all four forms in both
execution paths, including six refusal cases; real-model contact learning in
both paths with six byte-identical checkpoint files, conflict revocation,
private friend delivery, same-turn/context dedupe and manual precedence;
complete-life missing-body/off equivalence in both paths with three byte-identical
checkpoint files and an actual dream. The ordinary living-feature and contact
learning regressions also pass. The full completion has **578 fields**; a test's
initial 577-field expectation omitted the existing form receipt and was corrected.

The real-model fourth turn passes source-backed resume audit and exact
serial/worker result and checkpoint comparison. Both source GGUF hashes remain
unchanged. The C publication gate covers 11 refusals, failed save, stale
completion and detached ownership: the live owner, rejected result and saved
bytes remain exact. Its first four refusal cases passed before a test-only
list-writeback correction; the remaining seven passed in the recovery run.
The complete model test likewise resumed from the corrected envelope assertion
with a fresh three-encounter life and fourth-turn restart; the earlier run had
already checked the three disabled-control comparisons.

Verification uses AML `a97abc557730737a6762faf19a4876cc584c1006` and scalar
NoTorch `e84262669b2e349c6436ed0422028107b3d166b3`.

```sh
make test-living-pressure test-contact-neural test-iceberg-neural AML_ROOT=../ariannamethod.ai
make test-living-features test-contact-learning AML_ROOT=../ariannamethod.ai
```

## 2026-10-20 — Form grows from the remembered climate

The merged scalar-math correction is now the runtime beneath this experiment:
AML `a97abc557730737a6762faf19a4876cc584c1006`, NoTorch
`e84262669b2e349c6436ed0422028107b3d166b3` (scalar), Haiku base
`51fb82f4854522b9580178169e3bb8c00506fb36`. No dependency code or weights change.

The foreground chooses once, **before its five candidates**. Subjectivity
`221157935d84d5037768f34c4661b11ca6865f91`, `subjectivity.c:602–608,745–787`,
supplies `clamp(.55 * dominant + .9 * (dominant - mean), 0, 1)` and contraction
by `1 - .4 * tanh(pressure)`. Haiku's six chambers are current dissonance,
novelty, `arousal/(1+arousal)`, entropy, and the previous encounter's novelty
and entropy. Pressure is previous dissonance; an absent previous encounter
contributes zeros. All values come from the owned climate history. The chooser
consumes no draw; subsequent word sampling uses the existing owned voice stream.

| Contracted resonance | Form | Syllables | Endings |
|---|---|---|---|
| `< .34` | fragment | 4 | free |
| `.34` to `< .66` | haiku | 5/7/5 | free |
| `.66` to `< .82` | couplet | 6/4 | free |
| `>= .82` | tercet | 7/7/7 | distinct A words, matching rhyme key; different B key |

The four-shape thresholds and climate mapping are Haiku's adaptation.
Brodsky `501d617d5da8f9f00ff194ce6765d6749647bda8`,
`brodsky.c:2728–2850`, supplies closing-word reservation. The tercet selects
all three endings before filling the prefixes and subtracts each ending's
syllables from that line's budget. Brodsky's source meter is 5/7/5; this branch
uses 7/7/7. English rhyme keys are lowercase spelling tails from the final
vowel, locating it before a final consonant-following e while retaining that e
in the key. A live-model probe caught `remote`/`hot` being conflated; the corrected
keys separate them while retaining `remote`/`note` and `stone`/`bone`. Missing rhyme groups or
unreachable budgets fail before any draw. The present contract is one ABA
tercet; ABA → BCB continuation and phonetic rhyme data remain future work.

`src/living_form.aml` owns generation and emitted-form checks.
`src/living_features.aml` keeps both five-dimensional learners and their
original recursive selection. Short speech has actual zero trigram evidence
when it contains fewer than three words, syllable normalization uses the
chosen budget, and coherence uses the actual line count. A valid one-word
utterance now receives one MathBrain update. Five/seven/five retains the
original generator, draw order, features and learner updates exactly.

The core's explicit v4 upgrade adds three fields to the v3 dream life:
`form_policy`, `form_started_turn`, `form_values`. Each completed decision
stores nine values: form, contracted resonance, pressure and six chambers.
Every load recomputes decisions from canonical present/previous MetaHaiku
climates and checks the actual emitted words. Adaptive journal experiences
are version 2 and expose the receipt, form name and syllable targets. Frozen
RAE and word-derived MathBrain features are also checked against those words;
transition strength remains the pre-learning snapshot.

Iceberg v7 contains **268 fields**, including its 58-field speaking core.
New complete iceberg lives enable it; existing saved profiles retain their
behavior until `upgrade_form=1`. Direct activation is
`haiku_iceberg_enable_form(life, tokenizer)` after the v6 concept upgrade.
`living_form=0` starts a new fixed-form v6 control. Activation preserves old
turns and both random streams. Inner reflection and dream-generated voices
retain their existing three-line contracts. The actual variable foreground
is still the dream's opening and enters the same owned encounter history.

Publication checks the frozen receipt, recomputed decision and all prior
words, climates and learning history, including their float32 bits. Review
caught and reproduced an older same-shape word substitution and a `+0` to
`-0` substitution; the new profile rejects both. Review also caught a
regression in the standalone partial journal owner; that original API is
preserved. The old unsupported-version fixture now uses version 5 because
version 4 has an implemented schema.
A final capacity check found that encoding the entire old word history with
`list_key` would hit AML's 1 MiB string bound. Publication now compares list
leaves directly; the focused history gate covers a larger valid history and
a changed final word, alongside the original encoding failure as a control.

Verification commands:

```sh
make test-living-form test-living-features test-living-history test-living-session AML_ROOT=../ariannamethod.ai
make test-generator test-mathbrain test-rae test-state test-inner test-dream-life \
  test-owned-tail test-iceberg test-iceberg-concepts AML_ROOT=../ariannamethod.ai
../ariannamethod.ai/runner/aml-notorch examples/living-form.aml
```

The two unit gates compare interpreter and compiled execution. Generation
covers all four forms, exact legacy output/RNG/tape parity, reserved ending
order, multi-syllable closers and 16 rejected operations with unchanged owners.
A mutation removing reserved syllables fails the gate. Features cover actual
one/two-word outputs, independent loss/update values, recursive depths 0–5,
all 57 learner parameters and 14 rejected operations with unchanged owners.
The integration fixture covers independent selector references and threshold
sides, all four forms from real first encounters, serial/worker equality,
explicit upgrade after a legacy turn, actual dreams and restart. Its eight
checkpoints are compared byte for byte between interpreter and compiled runs.
The publication host tests 14 rejected completions/state operations.

The [recorded three-turn meeting](examples/living-form.txt), reproduced by
[its AML source](examples/living-form.aml), loads both actual frozen PEN7 files,
uses seeds 575/57 and fixed day 20746, and saves/loads the complete life after
every turn. It speaks fragment → tercet → couplet and opens one actual dream.
The corrected tercet reserves `think` / `five` / `shrink`. Both source GGUF
SHA-256 values are unchanged after the run. Form receipts remain attached to
the exact printed replies; quiet missing-body operation is covered separately
by the full v7 integration fixture.

## 2026-10-20 — Credit-weighted immune votes reach the decision

Fable found the missing scalar math in AML. On Haiku
`b9b03179166f54dc81bce64e7a0dbd11361f6530` with AML
`9f3cc1b674b9704bc2eab84d0f033913bb7f98c6`, every
`log(1 + exp(credit))` returned zero: both names reached the scalar parser's
unknown-function fallback. Normalization then evaluated `0/0` as zero. The
entire `.3 * consensus` contribution disappeared from the immune decision.

The old memory test repeated the defect in its reference: scalar `tanh(0.3)`
also returned zero. It accepted attenuation **0.366666675** where the
independent reference is **0.6579792791182575**. Earlier recorded runs used
this zero consensus; the old gate did not establish credit-weighted voting.

The runtime correction implements scalar `exp`, `log` and `tanh`, and makes
unresolved function calls errors. Haiku now checks each weight and the total
live mass before division. An old AML build stops with a rebuild diagnostic
instead of returning a verdict with its expert contribution missing. The
credit bounds, score equation, six decisions and checkpoint schema stay the
same; subsequent votes from existing histories now include their experts.

`tests/parliament_vote.aml` fixes seven independent reference cases with four
opposing experts. Changing only credits moves attenuation from **0.1333333333**
to **0.2417768312** or **0.02488983543**. Reversing credits at the legal
`+10/-10` endpoints moves **WEAKEN / 0.3307050694** to **FREEZE / 0**.
Equal endpoint credits test normalization; inactive rows with high credits
test exclusion. The fixture also checks individual votes and read-only owner
identity. The existing memory test now stores its independent numeric
reference, and the C gate rejects a zero-population vote explicitly.

Before adding the production guards, both numerical fixtures failed on the
old runtime in **interpreter and compiled** execution. After adding the
guards, that runtime fails at the first nonpositive weight. This records both
the wrong-answer regression and the runtime capability check.

Dependency pair for this correction:

- AML [PR #46](https://github.com/ariannamethod/ariannamethod.ai/pull/46),
  `7105cb8c2ea21038f63fa11bbdf308db82241a1d`, tested tree
  `0b22366c820043196abcbefb966f8fb9f075c259`.
- NoTorch `e84262669b2e349c6436ed0422028107b3d166b3`, scalar archive built
  with `BLAS_FLAGS= BLAS_LIBS= X86_SIMD=0 ARM_SIMD=0`.

Merge the AML correction, rebuild `libaml.a` and `runner/aml-notorch`, and
recompile any existing Haiku executable before using this gate. The ordinary
interpreted launcher needs the rebuilt runner. NoTorch's code is unchanged by
this correction.

Proof on Linux x86_64 / GCC 13.3.0 with the dependency pair above:

```sh
make test-parliament test-parliament-contact test-iceberg AML_ROOT=../aml-verification
```

All three gates pass on the rebuilt runtime. Credit-vote, life and memory
fixtures agree between interpreted and compiled execution. Contact checks
include PEN7 prototypes, exact restart, a later observed reward/Oja update,
seven refused calls and ten corrupt private states. Iceberg checks include
weightless and real-model conversation, matching compiled/interpreted output
and checkpoint bytes, eleven publication refusals, detached ownership,
failed-save handling and one-time publication. Both GGUF SHA-256 checks pass.

The upstream gate separately passes **550/550** core checks, **877** scalar
math checks with **39** rejected calls across five execution paths, **1,201**
expression checks with **48** rejected expressions, and all four compiler
suites. Independent review recomputed the seven vote references and inspected
error propagation before publication.

## 2026-10-15 — The same concept, two personal histories

Merged [#22](https://github.com/ariannamethod/haiku.aml/pull/22),
`fab2a4aad1dbfdf05f7487582b7f6acbe3352271`, supplies this stage's predecessor.
Iceberg v6 gives each presence [personal concept centres](docs/PERSONAL_CONCEPTS.md)
for water, tree, light and music. The twenty-one reviewed English teachers stay
fixed. Each word's weight is `1 + count` in the owner's retained 128 external
encounters: native human input for Haiku, actual Haiku dream deliveries for the
friend. Repeated spellings count once per encounter. Other languages, model
proposals, private speech, replay and newly learned aliases add no teachers.

The three saved modes are representation off, equal teacher weights and personal
weights. Modes 1/2 retain their own genuine encounter counts; mode 1 projects
with equal weights. The complete current tail uses the predecessor's snapshot.
Only its final publication advances the next snapshot and exact event/turn/
symbol-revision cutoff. Activation derives counts from already retained history.

Each frozen body has its own 21 × 448 bank of unit teacher vectors. The exact
lexical input tokens query each available endpoint independently. The runtime
mixes raw endpoint cosines with the lexical receipt's alpha, then clamps and
returns them at the existing .25 gain. Word logits retain their physical
parameter-mixture pass. This endpoint-score method has its own name and receipt;
the preceding experiment's physical .5-space accuracy remains its own result.

The declared live comparison uses seeds **575/57**, days **20740–20742**, and
these three inputs in order:

1. `rain river water ocean lake` creates the common weightless v5 history.
2. `tree forest wood leaf light` compares modes 0/1/2 from that predecessor.
3. `music song melody guitar piano` checks the warm continuation and restart.

The first input produced no dream; the second produced an actual dream and
populated both owners' model banks. The equal control reuses the already
source-verified immutable banks, while retaining its own preceding core and
encounter counts. Foreground experience and physical lexical receipts agree
across the three modes.

| Measured second-turn difference | Result |
| --- | ---: |
| Equal → personal, largest concept-score change | 0.0018619895 |
| Equal → personal, largest private-field change | 0.000000073458068 |
| Equal → personal, largest final word-rank change | 0 |
| Off → equal, largest final word-rank change | 0.0000059604645 |
| Changed pairwise orders among 48 words, either comparison | 0 |
| Actual admissions in all three modes | `root`, then `sea` |

Personal water changes from **0.7678155303 to 0.7659535408**. The other three
centres have unchanged prior counts. The field receives the difference; its
contribution to final word scores rounds away in this float32 run. The next
snapshot contains ten teacher words with one Haiku encounter each. The friend's
real delivered poem contains none of the twenty-one teachers, so its counts
remain zero while its encounter boundary advances. Inputs, gains and the
admission budget were kept fixed after observing these results.
On the third turn, the friend's own line contains `guitar` and `water`; those
words still add no external teacher count to the friend.

Contact v5 adds six leaves, reaching **61 fields per owner**. Iceberg v6 has
**265 saved fields** and a **555-field** completion. Its publisher independently
replays the complete contact transition, endpoint calculation and private
parliament field. It reconstructs pre-admission vocabulary from accepted human
trigrams and dream lines, repeats ranked selection at the original point in the
tail, and compares the entire ledger and final vocabulary. Voice learning and
generation retain their existing accepted-core boundary.

After checkpoint loading and provider return, the pure model-aware audit
rebuilds each populated available bank and compares exact vectors and teacher
tokens. The launcher calls it after loading the resumed life's bodies; direct
API callers use `haiku_iceberg_concepts_audit`. Cold construction/query costs
44 forwards per owner with both bodies, warm query costs 2, and resume audit
costs 42. The publisher's independent verification is additional work. Missing
bodies preserve history and banks while contributing zero representation.

The focused numerical gate passed in **21.050 seconds**, with byte-identical
interpreter/compiled stdout and checkpoints, empty successful-execution stderr
and unchanged GGUF hashes. Its 196 forwards include both rejected vector audits;
wrong teacher tokens fail before a forward. The preceding diagnostic used 190,
for 386 in this numerical lane. The mixed-sign `zuvrel` case checks that clamp
follows scalar mixing. A separate synthetic-count `stream` fixture checks the
weighted formula; it is distinct from the live encounter comparison above.

The focused experience gate passed with zero model forwards: genuine source
policies, deferred snapshots, exact alias counts, replay, activation, all modes,
128-encounter expiry and restart. Eight saved artifact pairs and full stdout
agree between interpreter and compiler. Twenty corrupt snapshots and five
invalid boundary calls are rejected without changing their inputs.

The actual model-history gate passes cold construction, all three controls,
both owners' real dream paths, an 84-forward audit after loading and exact warm
serial/worker continuation of the entire life and completion envelope. Its
309 forward calls comprise 252 teacher calls, 36 endpoint queries and 21
physical parameter-mixture calls. The publication gate uses the retained warm
completion: **16 refusals**, unchanged 265-field owner and saved bytes, a failed
save that leaves the owner intact, and one successful save/swap. Its 141 calls
comprise 84 teacher audits, 38 endpoint queries and 19 physical mixtures.
Together those two phases use **450 calls**; the publication phase took
445.986 seconds. No voice or dream lifecycle is regenerated for a corruption.

The weightless v6 lifecycle also agrees between interpreter and compiler in
complete stdout and the full checkpoint. The final compiled pass took 172
seconds and ran from a foreign directory. Three fixture corrections preceded
that final pass: the first input's actual no-dream result, order-independent
comparison of upgrade versus canonical packing, and a shorter assertion within
AML's source-line limit. Production source remained unchanged; the interpreter
completed its corrected remainder from the genuine saved prior.

The single interpreted v1–v5 compatibility comparison against `fab2a4a` passed:
**10/10 responses**, **10/10 complete checkpoints** and all five combined stdout
files match byte for byte over two actual dream turns per profile. It took
247.645 seconds, with empty stderr and unchanged hashes for all 31 current source
files. Both model paths were empty; these legacy runs performed zero forwards.

AML remains `2e17a2887449c89fdd8c066ebdcdaa1e448346cf`; NoTorch remains
`f6504cf8e8799c6a01b26a75ab90f004a6a3d804`, tree-identical to merged
`ca3e17a24852ad1db08a6e6b908fb37036011d2d`. The launcher import graph has
9,872 source lines and 281 user functions, within the existing runtime limits.

| Source | SHA256 |
| --- | --- |
| `src/contact_concepts.aml` | `f4e88b58a6ef73f1dc85b8a8f788d0ca1d8a15efe92712632d101c4fb97f55b1` |
| `src/parliament_contact.aml` | `a1929c1139ff9238bea4b31d1f2f53bae3d7fc57c7b0800b26d7f3b07bc30c39` |
| `src/iceberg_session.aml` | `05d6de726fdab7cea37b52a85ab23eec03c7734cbbb07dc9905ecb494d8c1d62` |
| `iceberg.aml` | `d4004d8c7109a06f2fa1935c7af334f846808aeeb789497ae6889bd4dd08b045` |

The [complete validation receipt](tests/research/iceberg/personal_concepts_validation.json)
retains all 48 raw and ranked word scores, both projections, private fields,
actual dream text and count snapshots, test/source/runtime hashes and checkpoint
parity. Its SHA256 is
`b9fe65efc4430b1d14377780fa757c0e5cbef0db6b9288cee4e5781a9432b9f0`.

```sh
make test-contact-concept-numerical test-contact-concept-experience test-iceberg-concepts
python3 tests/compare_concept_profiles.py
```

## 2026-10-15 — Several known words around one concept

Merged [#21](https://github.com/ariannamethod/haiku.aml/pull/21),
`702ae6ae5098d1002b362abb01b00eaf51f6ff28`, supplies the current organism.
The next representation experiment compares an isolated anchor with
`unit(sum(unit(hidden(word))))` over its existing reviewed English spellings.
The same twenty-one teachers represent water, tree, light and music in three
separate spaces: A alone, B alone, and the physical .5 parameter mixture.

The [protocol](tests/research/iceberg/concept_basis_protocol.json) was published
before the first new model run in commit
[`54aa7e5`](https://github.com/ariannamethod/haiku.aml/commit/54aa7e5763f05100a007adb41d859c83c7eb7fcf).
Its SHA256 is `729a2d56ac782e7adcd06e44dc5c0ed7c43eef3faeb9f47803074bdca834c811`.
An independent reviewer selected twenty-four new words, six per concept, and
eight unrelated/invented controls before model access. They are absent from all
130 English static bindings and from the preceding fifty-row probe. The old
sixteen-word regression group remains separate. The primary endpoint is the
.5 mixture's paired top-one agreement out of 24; methods, order, teachers,
labels and metrics are fixed in the protocol.

The primary result is **6/24 → 8/24**: four centroid-only successes, two
isolated-only successes, four shared successes and fourteen shared failures.
A-only moves **11/24 → 8/24**, B-only **7/24 → 8/24**. On the separate old
sixteen-word set, the .5 mixture moves **2/16 → 5/16**, losing both previous
successes while gaining five others. All fifty historical baseline rows match
their original raw cosine values within `1e-6` and retain their winners.
The new invented control `vexlorn` exceeds the correct `illumination` in both
centroid cosine and margin. These measured differences remain explicit in
the [full scores](tests/research/iceberg/concept_basis_probe.json).

Interpreter and scalar-compiled executions each complete **273 forwards**:
ninety exact texts and one full-hidden repeat in each space, 546 in total.
Their complete stdout bytes agree at SHA256
`8e91b5baf3845264aab507ef98012f84eb807c3ad857d5a7239c9e476a5b338a`.
Measured times are 22.616s and 22.508s, with empty execution stderr. Token
lengths range from one to seven. All 448 repeated hidden values agree in each
space; missing/missing returns quietly without representation arrays.
The first launcher attempt stopped on an overlong generated AML array line
before any forward. Bounded individual assignments fixed that emission;
the registered inputs and numerical method stayed unchanged.

Review found and fixed an omitted entrypoint re-hash in the checker. The
captured `run.aml` still matches its pre-run SHA256
`53d389b96dde63304edcba8ae3201aea894ac0eeceda29b7eb1ae2e3096d3282`.
The measured checker and its strict follow-up remain separate commits; the
original result is unchanged. The [post-run verification](tests/research/iceberg/concept_basis_validation.json)
records that sole observer-code change and zero additional forwards.

The scalar build retains AML `2e17a2887449c89fdd8c066ebdcdaa1e448346cf` and
NoTorch `f6504cf8e8799c6a01b26a75ab90f004a6a3d804`, whose complete tree equals
the established merged `ca3e17a24852ad1db08a6e6b908fb37036011d2d` pin. Runner
SHA256 is `5e235dd06d2aba519a2904d4dc4e5866ecd6d7304f3d3830ae55da7a6ecf5981`;
`libnotorch.a` is `d1c786c3b0c61b6e3758aa39fb5bb355e6de6345d0b327f773b35bb32daa98d3`.
The [concept-basis receipt](docs/research/iceberg/CONCEPT_BASIS.md) holds the
complete protocol, measurements and reproduction commands.

Oleg also specified the next form experiment: one line, two lines, a haiku, or
a Brodsky-style tercet, chosen from the conjunction of the organism's changing
metrics and remembered encounters. The exact direction is now in
[TODO](TODO.md) and [FAMILY](docs/FAMILY.md#form-grows-from-lived-state).
Subjectivity and Brodsky's remote heads still match their pinned family audit.
The planned port includes form-aware candidate assessment and journal/restart
state alongside generation; Brodsky contributes the ABA → BCB rhyme lineage.

## 2026-10-15 — A structure crosses the dream

Iceberg v5 carries a compound's ordered parents with an actual native dream
line to its peer recipient. Each offer uses the sender's exact turn-start
catalogue and spelling associations. At the end of the dream, after local
consolidation, the receiver resolves the missing parent closure atomically
into its own IDs. The first real delivery stays attached to every adopted
node; duplicate canonical structures reuse the existing identity.

Local and adopted compounds share the same sixteen-node alphabet and .25 total
prediction budget. Adopted nodes have zero local proofs. Their next-turn
recognition can support later local recursion: fourteen fresh eligible
encounters with `C → music` and a later dream can grow `(C+music)` around a
received `C`. Existing observations keep their frozen paths. The source
mechanism is Actually.life's recursive `resolve_sym`, pinned at
`2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8`; the full Haiku contract lives in
[SYMBOL_EXCHANGE](docs/SYMBOL_EXCHANGE.md).

Contact v4 adds four exchange leaves to its earlier symbol organ, reaching
**55 fields**. Iceberg v5 has **253 saved fields** and a **531-field** detached
completion. New lives enable exchange; an existing v4 life needs explicit
`upgrade_exchange=1`. Each saved enabled/off mode remains immutable. Publication
derives both directions from the exact predecessors and actual dream events,
then compares the reconstructed symbol and exchange leaves before save/swap.

Base: merged Haiku [#20](https://github.com/ariannamethod/haiku.aml/pull/20),
`09ebd0d071045ddf6739051d8e283438172e969b`. AML remains
`2e17a2887449c89fdd8c066ebdcdaa1e448346cf`; NoTorch's tested library tree remains
`ca3e17a24852ad1db08a6e6b908fb37036011d2d`. NoTorch's newer
`2ed0cb03654b445df76168ada21477ab7282b70f` adds Chuck/SPA experiments without
changing the library, PEN7, harness, ABI or library build rules. This stage
keeps the established dependency pin.

The restored scalar runtime first passed a genuine dream turn on the merged
v4 source: all **245** saved fields and **515** detached fields validated;
serial/worker states and interpreter/compiled checkpoint bytes matched, with
empty stderr. The checkpoint was 801,146 bytes, SHA256
`3e986d5003a5925dd5315cfdddc94302fdea445ff437154ba0bc4115e00d717a`.

The frozen model audit rehashed both 21,308,576-byte GGUF files:
`haiku1.gguf` is `50f159a2c34952bd9e6349880b4b20f8bbc0ac4b1f1dc2c1f6e4768732548b66`,
and `haiku2.gguf` is `16909b8590bd7fbbf15ab11b2dbf3743d612da8c617aca2dda03d640fef13e29`.
Both retain canonical tokenizer identity
`664afe692a006a54fc00406209042f6d452748d64a883ecfab9ef66d41f8bdf2`, computed again
from their actual 1,792 merge pairs. This audit adds no model forward.

Compatibility compares this stage with an isolated copy of merged `09ebd0d`.
Versions 1, 3 and 4 each complete two actual one-line dreams, with seeds
575/57, `rain` and `moon`, UTC days 20740/20741 and unavailable model handles.
All **six individual responses**, **six complete checkpoints** and **three
combined outputs** match byte for byte; stderr is empty. The saved schemas
remain 185/237/245 fields. The candidate run took **64.42 s** on this host.
Its source snapshot used `iceberg_session.aml` SHA256
`91c2062e319632f002b37f3ca9ad5c3ada06273a894a2a222221a4cfa76d68d0`;
the subsequent saved-off sender correction affects only the new exchange path,
which these old profiles never invoke.
`python3 tests/compare_exchange_profiles.py` reproduces the comparison using
that pinned baseline and the current sources. Its three generated AML
scenarios match the executed audit's scenarios exactly; it retains both source
snapshots, outputs, checkpoints and a SHA receipt.

Independent review caught an AML record-assignment copy that left adoption in
a temporary receiver; the dream tail now applies it directly to the owned
contact. Review also removed comparisons between different owners' event and
dream counters. Source numbers remain identity metadata; prior acquisition
turns determine availability, and the enclosing life binds them to actual
dream history. An explicit saved-off sender also yields no structural offer.

The actual launcher was also run unchanged in two isolated directories with
absent model paths. `rain` then `quit` created a v5 life with **253 fields** at
turn 1; `sky` then `quit` resumed the golden v4 checkpoint at turn 3 with all
**245 fields** and its saved profile. Both processes exited successfully with
empty stderr. Launcher SHA256
`d4b139a330ef452620a14080d072e12ec1f246bad338433775102a294b74861b`, all imported
sources and the runtime stayed unchanged. Their complete checkpoints were
818,983 and 804,986 bytes.

The integration factory combines fourteen genuine standalone contact
observations with fourteen actual two-line core dreams in one controlled
predecessor. With seeds 575/57, the known next dream uses the previously
witnessed aliases `magnify intensify` and `arise guide`. The first setup stopped
before teaching because `other` already has a shared binding; the fixture now
checks its actual unknown aliases before teaching. The measured transition
starts from this controlled predecessor and generates its next dream unchanged.
Both owners retain their own root at ID88 and fourteen birth proofs, adopt the
peer's distinct root at ID89 with zero local proofs, and recognize that received
parent on the following waking contact with `C → music`.

The focused integration gate passes serial/worker equality, detached and
idempotent upgrade, exact restart and all **240 non-wrapper fields** of the
disabled v5/v4 comparison. Interpreter and compiled stdout and complete
checkpoint bytes match, SHA256
`febfa24e0da467bb84f806eb2509cafe3d085640a530ff0c752b97033704c814`.
The compiled run recreates a removed checkpoint at its retained source origin
while running from a separate, empty working directory. A separate interpreted
dual-GGUF dream passes serial/worker equality, both adoptions, one model contact
and one lexical revision per owner, and checkpoint round-trip. Both model
hashes remain unchanged. Preparation and the remaining lanes completed in
segments using the same prepared checkpoint and final source hashes.

The new C publication gate passed **13 refusals**, preserving all **253 owner
fields**, the candidate envelope and the preceding checkpoint bytes. Two
alternative histories first pass the whole-life validator: an invented extra
delivery and a consumed delivery that withholds its available structure. Exact
predecessor replay rejects both. Sender-tree, origin, actual-line, dream-boundary,
profile and stale-result changes are also refused. The **531-field** detached
completion, failed-save refusal and one-time save/swap pass with actual adoption
in both directions.

The standalone lifecycle passed in both execution modes: all **twelve saved
record pairs** match byte for byte, including the packet and delivered event.
The ten contact states cover nested closure and private IDs, canonical reuse,
later re-export, the thirteen/fourteen-witness boundary, restart, .25 prediction
mass and +.0375 rank bound, atomic capacity refusal and the exact old-profile
off projection. The later native child and its restarted counterpart share
SHA256 `2699443b91f63556e8870b2f399289214264ea81cdad16879a88363aa7de8a6f`.
The run then stopped in C after **21 rejected deliveries** and **18 rejected
corrupt records**: changing the first receipt event from 100 to 99 was accepted.
The original failure and the unexecuted remaining sixteen cases were retained;
elapsed time was **1,070.24 s**, with empty lifecycle stderr and unchanged hashes.

Both modes reused the same five prepared records, produced through genuine
contact APIs at exchange source SHA256
`026765d63b4762acc8c8e9228fc05bbe6bf1685b3d254406fec8da2393a021f3`.
Preparation precedes the saved-off sender guard and does not query an off
sender. The complete lifecycle, including that guard, ran at the source hashes
below.

The iceberg graph at this execution stage has **30 modules**, **9,115 physical
source lines** and **262 user functions**: 279 slots including AML's builtins.
These production SHA256 values stayed fixed through the long lifecycle and
initial integration gates:

| Source | SHA256 |
| --- | --- |
| `src/contact_exchange.aml` | `89fbe32b8d3faed7409760f28b3f4ea0aba7f77330c4cb64a311909449e0b589` |
| `src/contact_symbols.aml` | `d598ecbdebfb28537b77783e14fc932bc910eac5286fe4c58fdd2979034ec3dd` |
| `src/parliament_contact.aml` | `21491e66fffdf40e3ffc92a38ab77b13d79d6c420511893f7d1afdb308c8e1ef` |
| `src/iceberg_session.aml` | `91c2062e319632f002b37f3ca9ad5c3ada06273a894a2a222221a4cfa76d68d0` |
| `iceberg.aml` | `d4b139a330ef452620a14080d072e12ec1f246bad338433775102a294b74861b` |

The receipt-clock defect above is fixed by seven validator lines. A retained
receipt at the current delivery revision must match the exact current event,
turn and dream; an older revision must precede the latest peer event. The
100-to-99 mutation reproduces acceptance before the fix and explicit rejection
after it. The transition code is unchanged. Corrected `contact_exchange.aml`
SHA256 is `d07ab5c5d555b8028b30e69a57ebdf2a5c2ae4a41f8cb01746e726323f538dba`;
the final import graph has **9,122 lines**, with the same module/function counts.

The focused integration C gate then passed all **13 refusals** again on this
corrected validator, including positive two-owner adoption and exact save/swap.
The extra-delivery alternative changes both the delivery counter and its latest
receipt revision to two, so it still passes the whole-life validator before
exact predecessor replay rejects it. The altered latest event now fails at the
new contact invariant. These final assertions live in
`tests/iceberg_exchange_publication.c`, SHA256
`d1de5d1732016f634b57f05c88699e3c36d0199ff13d16b99fba2b9400af9f1d`.

The final standalone focused gate passed in **33.66 s**: **21 invalid
deliveries** and **36 corrupt records** were rejected without mutation, exact
receive/replay preserved the checkpoint, and all **fifteen genuine contact
snapshots** loaded in both interpreter and compiled execution. These are the ten
retained lifecycle states and five prepared states. Both paths also repeated
the actual positive receive and replay. All twelve earlier record pairs and
their hashes remained unchanged. The added thirty-sixth corruption covers an
older receipt revision pretending to name the latest event; the original
100-to-99 case covers the other direction. Two surplus EOF blank lines were
removed from the fixture and C test before this final focused run.

The long lifecycle and model lanes above remain their recorded executions;
the final validator was checked against their retained states and positive
transitions. Reproduce the complete suites from fresh preparation with:

```sh
make test-contact-exchange test-iceberg-exchange
python3 tests/compare_exchange_profiles.py
```

## 2026-10-14 — A dream gives a familiar path its own name

Each iceberg presence now has a private recursive-symbol organ. Fourteen
distinct external turns can support an ordered pair; the end of an actual
dream preserves at most one new structure per owner. Later encounters can
recognize that compound and earn another level from their own subsequent
history. The two voices keep separate IDs, observations and birth proofs.

Haiku's evidence comes from native human input; the friend's comes from Haiku
dream lines actually delivered to it. Paths freeze before the encounter's word
learning. Unknown words retain barriers, and more than 64 tokens quietly yield
an unavailable path with exact text retained. Repeated occurrences in one turn
and replay add no support; the same relation on a new external turn does.

The bound is 16 immutable nodes, depth four and sixteen expanded anchors.
The rolling ring holds 128 observations; each birth preserves fourteen complete
witnesses outside that ring. Later left-parent suffixes anticipate right-parent
anchors with shared total mass .25. Contact features receive that projection;
lexical ranking receives at most +.0375 beyond existing field pressure. The
organ adds no model forward or random draw. Structural peer adoption remains a
separate TODO item. [DREAM_SYMBOLS](docs/DREAM_SYMBOLS.md) holds the full contract.

Contact v3 has **51 fields**; iceberg v4 has **245 saved fields** and **515**
detached completion fields. New word-learning lives enable symbols; old lives
keep their saved profile until an explicit upgrade. An inert v4 control retains
its saved off mode. Publication reconstructs all accepted source clocks and
eligible external paths, then the actual final dream boundary. Every symbol
leaf must equal that independently reconstructed transition.

The [Actually.life follow-up](docs/research/iceberg/GLYPHS.md) pins
`2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8` and executes external ingestion through
the real dream threshold: 13 turns leave no compound, 14 produce `fire+water`;
fourteen later whole-symbol encounters produce `(fire+water)+tree`. Dreams leave
external co-occurrence totals unchanged. The original source's plaintext router
does not recognize the born compound; Haiku's frozen path compression supplies
that explicit recognition step.

AML [#41](https://github.com/ariannamethod/ariannamethod.ai/pull/41),
`2e17a2887449c89fdd8c066ebdcdaa1e448346cf`, expands shared capacities to 512 function
slots, 1024 record fields and 16384 source lines. Its exact-boundary, worker,
compiled and checkpoint checks retain old wire bytes; C hosts rebuild for the
larger execution context. NoTorch stays at merged
[#171](https://github.com/ariannamethod/notorch/pull/171),
`ca3e17a24852ad1db08a6e6b908fb37036011d2d`. The source weights stay frozen.
The final iceberg import graph contains **29 modules**, **8,324 physical
source lines** and **250 user functions**: 267 slots including AML's builtins.

Compatibility against Haiku #18's `7d4d2d9856f0a7bd22f46d35a5636d8a71e7785d`
used seeds **575/57**, `rain` on UTC day **20740**, then `moon` on **20741**,
both actual mixed GGUF bodies and a dream on each turn. The complete old v3
checkpoints—401,110 and 406,989 bytes—and displayed outputs matched exactly;
stderr was empty. The later #19 merge changes only README's title, which this
branch preserves.

An isolated launcher run with original defaults and absent model paths saved
a new v4 life with 245 fields and one human symbol observation. Another process
resumed a seeded v3 life under those same defaults, retaining all 237 fields
and its version. Both saved turn 1, exited through `quit` and produced empty
stderr. The launcher and every imported source kept the same hashes throughout.

Independent review reproduced loss of the human observation ring in an
otherwise accepted checkpoint; exact ring occupancy now rejects it. The
enclosing iceberg also requires the friend's complete accepted-delivery ring.
Permanent birth proofs retain strictly increasing events, revisions and turns,
including after the original observations expire. Processed dream ordinals and
birth turns match the voice's exact dream log. Publication replays all source
clocks, including contacts that supply no symbol evidence.

Typed whole-array equality verifies every empty cell and path-padding value
against a reconstructed array. A fourteen-contact, birth and pressure control
took **35.886 s before / 26.038 s after** this validation change on this host;
the output and complete checkpoint bytes matched. These are single-run gate
timings. The original array, schema and evidence rules are preserved.

The complete-life gate exercised fourteen native human turns, a real dream,
symbol birth, later contact pressure and restart. Interpreter and compiled
scalar execution produced identical output and checkpoint bytes. One-line
dreams, paused deeper life and the saved off control passed. Its model lane
then found missing provider metadata in a reconstructed mixed-model event.
Replay now carries the independently verified receipt's availability, effective
alpha and canonical identity; this adds no forward or random draw.

Verification continued in segments after that model-only correction. The
actual dual-GGUF fixture passed interpreter and compiled scalar execution,
with identical output and complete checkpoints and unchanged weight hashes.
The C publication gate passed **14** source/birth/receipt/profile/stale
refusals, preserving all **245** live fields and preceding checkpoint bytes.
The **515**-field detached result, failed-save refusal and one-time save/swap
passed against the retained fourteen-turn checkpoint. All nine gated source
and test hashes stayed unchanged during these final two lanes. Reproduction:
`bash tests/run_iceberg_symbols.sh` runs the three lanes in sequence.

`bash tests/run_contact_symbols.sh` passed the complete standalone lifecycle
through interpreter and compiled scalar execution. Output and all **nine**
checkpoints matched byte for byte, including nested birth, private canonical
identities, frozen learned paths, 128-observation expiry, sixteen-node capacity
and exact disabled parity with the old 47-field contact. The threshold,
depth-four and sixteen-anchor limits, unknown barriers and bounded pressure
passed. The C gate rejected **six** invalid calls and **37** malformed records
without changing owned state or preceding checkpoint bytes; replay stayed
an exact no-op. All six gated source/test hashes remained unchanged.

The implementation and assertions were tested at
`0d70f370df3411a82b3c40ddc846fd5c542024fa`. The follow-up records these completed
gates and closes the local-birth TODO. It also corrects the standalone fixture's
final reporting line from `print(...)` to AML's `PRINT`: the former produced
empty output, while a minimal interpreter/compiled control of the corrected
line emitted the same expected success marker. This sole fixture-line change
leaves all assertions and production sources intact; the long gate above was
run before that reporting correction. Structural delivery and adoption between
owners remain a separate step.

Automatic review questioned the complete-life harness's checkpoint path after
launching compiled code from `foreign`. AML resolves relative file operations
against the retained source directory: `amlc` embeds the absolute source path,
and `aml_resolve_value_path` joins the filename to that context. The existing
comparison therefore reads the compiled writer's correct file. Both retained
compiled programs name their work-directory sources; `foreign` is empty and
the checkpoint timestamps follow the compiled builds. An additional control
removed the destination before each interpreter/compiled run from `foreign`.
Both recreated only the work-directory file, identical to the real 245-field
checkpoint, SHA256
`0016ec57ee6ce626857a5a417055600a3ce031b522339256caa9a009b65a95f2`.
The path finding is a false positive; the harness remains unchanged.

## 2026-10-14 — A word acquires a remembered neighbour

The experimental iceberg now keeps private word-to-anchor associations for
Haiku and the friend. Haiku learns from accepted native human encounters; the
friend learns from Haiku dream lines actually delivered to it. Three distinct
retained local contexts can establish an association when at least 75% agree
and the leading anchor is two contexts ahead. Conflicting encounters can suspend
it. Fixed reviewed seed spellings supply the teachers; learned words retain
their own evidence and cannot teach each other.

Each owner has room for 32 English candidates and six contexts per word. Exact
source text, event ancestry and activation clocks accompany the counts. Local
window signatures use sorted sets of normalized words, so case, punctuation,
order and multiplicity changes preserving those sets add no vote. Reviewed
bindings retain priority. Learned strength is at most .5, giving at most ±.375
lexical pressure through the existing bounded field.

Contact v2 adds five leaves, growing from 42 to **47** fields. Iceberg v3 has
**237** saved fields and **499** detached completion fields. New iceberg lives
enable the layer; saved v1/v2 lives keep their profile until explicit upgrade.
Publication reconstructs accepted source events, retained contact receipts and
all five learned leaves. Native features are checked at the moment before each
learning step. This layer adds no model forward or random draw. The full
contract is in [LEARNED_WORDS](docs/LEARNED_WORDS.md).

The [four-anchor PEN7 experiment](docs/research/iceberg/WORD_ANCHOR_PROBE.md)
retains all 50 samples and 200 measured cosines, reproduced within `1e-6`.
Raw hidden similarity recovered **2/16** held-out associations and **2/8**
reviewed aliases; `tax→tree` exceeded both correct held-out cases in cosine
and margin. External context supplies the present learner's evidence. The
existing bounded model-contact path continues; model-attributed word evidence
and recursive symbol birth remain separate experiments.

Base: Haiku [#17](https://github.com/ariannamethod/haiku.aml/pull/17),
`f1ac1480359effdc0775e8b8d38704d063b1bdec`. Tested dependency contents match
merged AML [#40](https://github.com/ariannamethod/ariannamethod.ai/pull/40),
`b05e845d9d6c5c74959c2a22cc56f7b31b8cdc77`, and NoTorch
[#171](https://github.com/ariannamethod/notorch/pull/171),
`ca3e17a24852ad1db08a6e6b908fb37036011d2d`.

`bash tests/run_contact_learning.sh` passed interpreted and compiled lifecycle,
pressure, conflict, owner isolation, bounded retention and restart fixtures.
Their outputs and checkpoints matched exactly. Four rejected teaching
transitions and **16** malformed records preserved the owner and preceding
checkpoint, including contradictory shared-event text/revisions and an invalid
activation boundary. Replay remained an exact no-op.

`bash tests/run_iceberg_learning.sh` passed the complete weightless life and
actual dual-GGUF fixtures through interpreter and compiled scalar execution.
Serial/worker results, restart continuations and full checkpoint bytes matched,
including a **16-line dream** and later missing-body fallback. Its C publication
gate rejected **14** forged transition/source/receipt/profile/stale results,
preserving all 237 live fields and the previous checkpoint. The 499-field
detached result, failed-save refusal and one-time save/swap passed. Both GGUF
hashes and all seven gated source/test files remained unchanged.

Compatibility comparison against that Haiku base used seeds **575/57**,
`rain` on day **20740**, then `moon` on **20741**, both actual mixed GGUF
bodies and a dream on each turn. All four complete checkpoints matched byte
for byte: v1 retained 185 fields, v2 retained 227. Their displayed output also
matched and stderr was empty. The existing weightless Janus fixture passed.

An isolated launcher run with unchanged defaults and absent model files saved
a new v3 life with 237 fields after `rain`. A separate process resumed a seeded
v2 life using those same defaults and retained its 227-field profile after the
exchange. Both saved turn 1, exited through `quit` and produced empty stderr.

Independent review reproduced two corrected cases: permutations of one local
context remain unresolved, and a confirming encounter cannot replace its
frozen features with the association it just learned. Shared-event rows must
also agree on exact text, turn and revision. The final graph uses **254/256**
function slots, **6868/8192** module lines and **28/64** imports.

## 2026-10-14 — Give the little cloud a shorter front door

Oleg asked for the Method's README/log split. The README now keeps character,
actual dialogue, the two dependencies, launch commands and the essential state
of the organism: **147 lines / 724 words**, from 515 lines / 3,410 words.
The detailed engineering and verification account below was
moved from README at `a6b1a6fa036590f3692b1884423f04fcc330f7b9`.
Historical counts retain their original scope and dates; this documentation
move does not claim a new full-suite run.

## 2026-10-14 — A completed Janus transition carries its evidence

[PR #17](https://github.com/ariannamethod/haiku.aml/pull/17),
`a6b1a6fa036590f3692b1884423f04fcc330f7b9`, fixes the P2 identified after
PR #16: a schema-valid replacement of a nonleading score, debt or history row
could pass publication and alter a later witness's balance. Publication now
reconstructs both Janus and lexical owners using the exact predecessor,
frozen experience, accepted dream deliveries and immutable A/B handles.
Execution and verification share one pure context/lineage planner.

`bash tests/run_iceberg_janus.sh` passes interpreted and compiled real-model
continuations with identical checkpoints. All **27** refusal cases preserve
**227** owner fields and the previous file. They include valid-but-forged score
pools, debt/history, ancestry and wrong providers. Two real mixed-model v1 dream
turns produce byte-identical checkpoints before and after this fix. Both GGUF
hashes remain unchanged. Verification adds at most two forwards per turn;
learners, dreams, prototypes, admission and random draws are not repeated.
The saved/result schemas remain **227/479**. See
[publication](docs/ICEBERG_SESSION.md#one-publication-one-checkpoint).

## 2026-10-14 — Two calendars, two expectation histories

[PR #16](https://github.com/ariannamethod/haiku.aml/pull/16),
`abb52a6390967763db1e71c1248d0c9672ec5536`, adds a private Janus organ to
each presence. The v2 iceberg retains both 20-field histories alongside the
original voice, lexical trajectories and contact organs. A captured UTC civil
day travels with the foreground request through workers and publication.
V1's 185 fields retain their fixed-profile continuation until explicit upgrade.

The full reviewed 48-word score pool waits for a later external encounter.
Haiku meets native human input; the friend meets an actual Haiku dream delivery.
A matched witness updates its own expectation debt, which joins calendar and
personal dissonance in the next all-parameter coefficient. The model pair stays
frozen. Fixed-alpha and endpoint controls use the same forward. The C calendar
comparison covers **180** exact fixtures. The module's equations and boundaries
are in [JANUS_BALANCE](docs/JANUS_BALANCE.md).

A real two-process launcher run accepted `rain remembers the window`, then
`the tree keeps a little light` after restart. At turn 2 / UTC day 20740,
Haiku's debt was `0.0706241131`; alpha moved from `0.415521741` to `0.40139693`.
The saved life had 227 fields. This is the recorded run, with its exact inputs.

Dependencies: [AML #39](https://github.com/ariannamethod/ariannamethod.ai/pull/39)
`90684ad3784726f54a5c4d19cdadc93b00a224b0` supplies `utc_day()`;
[NoTorch #171](https://github.com/ariannamethod/notorch/pull/171)
`f6504cf8e8799c6a01b26a75ab90f004a6a3d804` preserves allocation-failure status.
[AML #40](https://github.com/ariannamethod/ariannamethod.ai/pull/40)
`f88dc60449122e67e0c0a15ace38c47dd6848ca0` fixes the opened-import race and two
worker-fixture buffer findings. Their detailed upstream proofs live in AMLLOG
and NOTORCHLOG.

## 2026-10-14 — Frozen words meet private parliaments

The separate `iceberg.aml` conversation connects two immutable PEN7 handles,
four retained contexts, all-parameter mixtures, private parliaments and
bounded word admission to actual human, cloud, inner and dream events. Each
source is a 21,308,576-byte GGUF with Q8_0 matrices and F32 auxiliary tensors;
NoTorch expands the matrices once to frozen F32. The source revisions,
licenses, conversion and hash receipts live in [weights](weights/README.md).

An excursion scores a reviewed 48-word pool and returns its two leading
proposals plus a 448-value contextual representation. Four prototype words
supply bounded concept features. Each contact organ owns its own adapters,
immune history, rewards, receipts and prototype cache. Admission adds at most
two reviewed words to the future cloud/vocabulary/syllable cache; it adds no
fabricated human observation or trigram.

The first complete iceberg schema has 185 fields. Its full predecessor,
result and experience share a detached worker completion. Join, validate,
save, swap and display form one publication boundary. Missing model files are
quiet; the weightless life continues. Contracts and focused gate commands:
[lexical sources](docs/LEXICAL_SOURCES.md),
[contact](docs/PARLIAMENT_CONTACT.md), [iceberg](docs/ICEBERG_SESSION.md).

## 2026-10-07–14 — The Python organism comes home to AML

The following organ account and measured gates were retained from the earlier
README. Pinned source revisions and implementation order are in
[MIGRATION](docs/MIGRATION.md); the Python implementation remains the behavioral
and artistic reference.

### Implemented organs and continuation

Haiku now listens, answers, learns, reflects, dreams, and remembers across AML conversations.
Its organs run in pure AML modules. Pulse,
dissonance, temperatures, and transition scores preserve the Python formulas.
The text organ counts Unicode-delimited words, preserves line contents, and
computes the original RAE three-line coherence feature. The lexical organ
collects words, builds rolling triples from token lists, and feeds their exact
overlaps into Harmonix.

The cloud now remembers each word's weight, frequency, last-use clock, and
origin. Repeated words gain weight; dormant words fade. Observer trigrams and
the generator's Markov transitions have separate counts. The last ten triples
form a copied snapshot, ready for the next observation.

The English generator now gives that memory three lines: **5–7–5 by the original
English estimator**. It keeps
the Python voice's Markov choices and its peculiar first-word leap, with the
original syllable rules carried into AML. Every accepted word leaves room to
finish the line. NoTorch owns the weighted draw; each voice keeps its own
replayable random state. Seventeen syllables, and a little room for chance.

MathBrain and RAE now give those lines a learned preference. Each has its own
57 parameters: five features, eight hidden neurons, one answer. MathBrain
scores a candidate; RAE passes its candidates through the original three-step
recurrence and chooses a voice. Both learn through canonical NoTorch arithmetic,
with their features, gradients, clamping, and experience order composed in AML.
Small networks. Actual changing weights. The octagonal wheel has opinions.

The foreground now joins these organs: input → tokens → living memory → five
candidates → RAE's choice → a response → quality → MathBrain learning.
The default preserves Python chat's fixed RAE selector; set `train_rae = 1`
in `haiku.aml` before starting a new state file to let that selector learn from
the same chosen experience, too.
Each learner and random stream has an explicit owner.

Then the conversation continues inside. MetaHaiku keeps eight short fragments
and answers with an internal haiku at temperature 0.7. Echo, drift, and meta
produce five, seven, and three candidate trigrams; coherent ones enter the
observer's memory. Their counts remain separate from the speaking chain.
The inner voice and rings share the voice's random stream, in Python's event
order. Thinking leaves its place in the next draw.

Phase4 remembers named states and the transitions between them: similarity,
quality change, and the climate of each encounter. After the rings, a dream
can open a four-part conversation with the friend. All its voices are generated
before their fragments return to memory; then the speaking chain changes.
The ordinary ten-turn cooldown gives that conversation room to settle.

Every accepted exchange keeps its own journal entry: the words on both sides,
the contexts that were actually observed, the features the learners saw, the
quality signal, and the state revision. Seventeen syllables above the water.
A whole remembered encounter underneath.

Close the terminal. Come back. The cloud remembers.

Each completed exchange saves one versioned state: cloud, ordered transitions,
recent memory, both learners and their statistics, both random streams, and
the latest bridge event, reflection history, the latest three-ring trace,
Phase4's transition history, dream dialogues/fragments, and exchange journal.
The next launch resumes `haiku.state` beside the
entrypoint. Its tokenizer identity and logical clock come with it. Set
`state_path = ""` for a fresh, unsaved conversation, or choose another path
for another voice. One running process owns each state file.

New conversations use version 3, with the complete inner life. Existing
version-1 and version-2 checkpoints retain their exact continuation. Set
`upgrade_dreams = 1` in `haiku.aml` to grow Phase4, dreams and the journal from
their next exchange; their earlier memory and random streams carry through
unchanged. `upgrade_inner = 1` still enables reflection/rings alone for v1.
For new conversations, `dream_life = 0` selects v2 and `inner_life = 0` selects
the foreground-only v1 profile.

### Module contracts

The fixtures use native `IMPORT`, UTF-8 values, string lists, and numeric maps.
See [numerical parity](docs/NUMERICAL_PARITY.md) and
[text parity](docs/TEXT_PARITY.md),
[lexical parity](docs/LEXICAL_PARITY.md) and
[cloud parity](docs/CLOUD_PARITY.md), plus
[English form](docs/FORM_PARITY.md) and
[generator parity](docs/GENERATOR_PARITY.md),
[MathBrain](docs/MATHBRAIN.md), [RAE](docs/RAE.md),
[tokenization](docs/TOKENIZER.md), [foreground exchanges](docs/FOREGROUND.md),
[MetaHaiku](docs/METAHAIKU.md), [the three rings](docs/OVERTHINKG.md),
[Phase4](docs/PHASE4.md), [dreams](docs/DREAM.md), [the journal](docs/JOURNAL.md)
and [continuity](docs/STATE.md), for exact boundaries and Python
reference reproduction.

### Recorded verification

Reference checks completed on 2026-10-07–08; dream-life follow-up on 2026-10-13:

- Python HAiKU: **161 tests passed**.
- Haiku AML: **343 cases / 6,130 reference results** pass in the interpreter and
  compiled scalar executable. The new cloud/memory suite adds 5,055 numerical
  fields and 247 ordered-list snapshots to the earlier 828 results, including
  the complete 587-entry seed corpus. Counts, strings, and list order compare
  exactly; float tolerance is 0.000002. Six malformed triple lists and 38
  invalid state updates fail explicitly. Host inspection verifies that all
  eleven state containers retain their contents after each rejected update.
- English form adds **1,966 direct Python reference results**, six accepted
  boundary checks, and ten invalid-input rejections through both execution paths.
- Generator checks cover **22 scripted cases**: 16 preserved Python draw paths
  and six explicit repair baselines. Native checks cover replay, independent
  state, batch order, the complete seed corpus, temperatures, and exact text
  limits. **36 invalid generations** fail with model and random state inspected.
- MathBrain adds **3,717 Python reference values**, including every gradient
  and parameter in 24 learning steps, unclipped negative predictions, loss
  statistics, and full parameter clamps. Twenty-two rejected observations
  preserve model, statistics, memory, context, and random state.
- RAE adds **1,657 reference fields**: Unicode features, every refinement pass,
  stable rule choices, ten complete gradient/parameter updates, and the native
  generator-to-learning trace. Twenty-three rejected operations preserve all
  inspected owners. Both learner suites pass interpreted and compiled.
- Tokenization adds **6,034 exact reference fields** from 84 Python cases in
  both modes, plus nine rejected operations. Both execution paths preserve
  native pieces, cleaned tokens, rolling triples, and source-relative loading.
- Foreground checks replay **seven Python turns** and the explicit punctuation
  repair, comparing candidates, complete memory, draw cursors, losses, and
  learner parameters. Real interpreted/compiled conversations agree; blank,
  quit, EOF, and rejected input preserve the inspected owners. Shared RAE
  experience has its own checked update.
- Continuity compares **five seven-turn variants** with a fresh-process restart:
  regex/SentencePiece, fixed/learning RAE, native/scripted draws, and a supplied
  syllable cache. Responses and complete checkpoint bytes agree through the
  interpreter and compiled executable. **36 invalid foreground states** preserve every
  live owner and the previous file; failed staged generation and failed saving
  do the same. The actual CLI resumes saved configuration and withholds an
  unpublished response after a save failure.
- Inner life adds exact Python bootstrap and reflection traces, the original
  rings' sampling/admission paths, and **ten seven-turn restart variants**:
  fresh version 2 and explicit version-1 migration, in both tokenizer and
  training modes. Full checkpoint bytes agree across execution paths. A real
  launcher check covers saved-profile authority and the recorded inner dialogue.
- Phase4 adds 16 exact binary32 state IDs, 12 shared-key cosine cases, 26
  Python/SQLite events and eight suggestion queries. Thirty malformed states
  and nine rejected calls preserve their owners; the 10,000-event boundary
  and metric histories beyond one loop's budget are exercised.
- Dreams retain 12 trigger boundaries, five original dialogue lengths,
  bootstrap/fragment decay and shared native draw order. Twenty invalid states
  and staged history/capacity/generation failures preserve complete owners.
- The journal compares three real Python shards/metric rows and 30 frozen
  feature values, exact Unicode context framing, independent getters and
  checkpoint round trips; 28 malformed contexts/states fail in both paths.
- V3 continuity covers fifteen fresh/migrated restart configurations and
  31 semantic save/restore failures. All 55 leaves and the previous checkpoint
  survive rejected publication, including exhaustion inside a dream.
- The actual v3 CLI completes 24 native turns, dreams on turn 14 and resumes
  from turn 13 with identical responses and checkpoint bytes. Both old-profile
  upgrades, blank/quit/EOF and the recorded dream dialogue pass in interpreter
  and compiled execution. Fifteen frozen v1/v2 configurations also retain
  their exact pre-upgrade responses and checkpoint bytes.
- The owned-tail gate compares 24 serial, worker and compiled turns, complete
  checkpoint bytes and restart before the first dream. Controlled completion,
  parent mutation after launch, stale/duplicate results, failed learning-tail
  execution and failed saving preserve the published owner. Old profiles,
  SentencePiece and coupled learner mode retain their continuation.
- The standalone parliament gate compares interpreted and compiled life/memory
  fixtures, then inspects all owned state through replay, rejected calls,
  thirteen malformed records and exact checkpoint continuation through delayed
  reward. Its pressure, lifecycle and immune memory run without model assets.
- The 2026-10-14 lexical/contact gates exercise both real frozen bodies, complete
  parameter mixing, private contexts, delivered/withheld fragments, named glyph
  aliases, representation pressure and later human reward. Interpreter and
  compiled continuations retain their exact checkpoint bytes.
- The complete iceberg gate compares serial and worker lives, four retained
  contexts, vocabulary admission and restart. Publication rejects another
  private life, altered receipts and stale completions while preserving all
  185 live fields and the previous file. Both GGUF hashes remain unchanged.
- The Janus gates compare the owned C calendar, fixed/endpoints and witnessed
  dynamic mixtures through the same real 48-word pool. Version-2 checks cover
  227-field restart, a frozen day across worker scheduling and midnight, and
  rejected 479-field completions. The complete contracts record the controls
  and their measured score changes.
- Earlier AML runtime baseline: **550 tests passed**; the 5.11 worker and
  parliament additions have their own upstream gates in PR #36.
- AML compiler: scope, source origins, nested failures, worker branches and
  errors, scalar builds, and literal program arguments are covered by regressions.
- NoTorch: native tanh/backward and SGD are merged in
  [PR #159](https://github.com/ariannamethod/notorch/pull/159); **5,122 numerical
  checks** and the full CPU suite pass.
