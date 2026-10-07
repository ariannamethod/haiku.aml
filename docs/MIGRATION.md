# HAiKU → AML: the first engineering map

2026-10-07. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
This document records the inspected sources, current behavior, and implementation
order. Numerical, text, lexical, cloud, and memory organs are implemented; the status below
marks the boundary between those functions and the remaining organism.

## Source bodies

| Source | Inspected commit | Role |
|---|---|---|
| [harmonix/haiku](https://github.com/ariannamethod/harmonix/tree/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku) | `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39` | Behavioral and artistic reference |
| [AML](https://github.com/ariannamethod/ariannamethod.ai/tree/5477997c8ab88024434cb35633793ffd416188e4) | `5477997c8ab88024434cb35633793ffd416188e4` | Language baseline |
| [NoTorch](https://github.com/ariannamethod/notorch/tree/5eb709f12eca7089551d349e93b07daec3c5e2c7) | `5eb709f12eca7089551d349e93b07daec3c5e2c7` | Numerical substrate and manifesto source |
| [haiku.c](https://github.com/ariannamethod/haiku.c/tree/10adb56fc6de563c09a134756b0a008e71dee8cc) | `10adb56fc6de563c09a134756b0a008e71dee8cc` | Five-force Dario generator and local HTTP example |
| [haiku](https://github.com/ariannamethod/haiku/tree/96260893e6ea866d44a93e26ced67d50eeae78c2) | `96260893e6ea866d44a93e26ced67d50eeae78c2` | Parliament, persistent word mass, worker, distinct dream-friend |
| [klaus.c](https://github.com/ariannamethod/klaus.c/tree/6fad88329538f0a820d5266796d456c9133559c6) | `6fad88329538f0a820d5266796d456c9133559c6` | Language packs, incoming/outgoing vocabularies, cross-language affinity |
| [yent.aml](https://github.com/ariannamethod/yent.aml/tree/28baf3199b649a0b8cad6e03045b979cbead1684) | `28baf3199b649a0b8cad6e03045b979cbead1684` | Working AML/BLOOD + libaml + NoTorch integration |

The later [family audit](FAMILY.md) adds two source bodies supplied by Oleg:
Subjectivity `221157935d84d5037768f34c4661b11ca6865f91` and Brodsky
`501d617d5da8f9f00ff194ce6765d6749647bda8`. It maps variable form, preserved
foreground state during reflection, episodic recall, sampling mixtures, rhyme
reservation, and rendering independence to measured future extensions.

[Q](Q.md), inspected at `f5d00a36ecfcdb5e655e1576770f03d06d900e04`, adds
weightless field generation, aging expectations, candidate experience, and
slow Hebbian consolidation to the research queue. Its audit records concrete
activation and persistence findings before proposing Haiku experiments.

The manifesto is copied byte-for-byte from the pinned NoTorch source.
SHA-256: `c8288157c8e2a63cec307510534a3da5415a5d44f3c0a089c8d7f6bbde07b906`.

Follow-up bases: AML `355a0345a432d61c35f7e45327ba3ae0bea0b8e6` includes merged
PRs #25 and #26. NoTorch `c92e915f2e533372073cc95df9c22bdaf014188b` includes merged
[PR #159](https://github.com/ariannamethod/notorch/pull/159), adding native tanh
and plain SGD. The original audit and manifesto pins above remain their sources.

## Implemented first organs

`src/harmonix.aml` implements pulse, count-based dissonance, and the two
temperatures. `src/bridges.aml` implements the aggregate transition score.
Durable cloud storage, cosine/state aggregation, and learning
remain separate work. [NUMERICAL_PARITY.md](NUMERICAL_PARITY.md) records **49 cases /
130 scalar values** obtained by direct calls to the pinned Python implementation.
Both interpreted and compiled AML pass with tolerance 0.000002.

`src/text.aml` now implements the 29-codepoint Python whitespace table, word
counts, literal LF line counts, exact line extraction, and the original RAE
coherence feature. [TEXT_PARITY.md](TEXT_PARITY.md) records **146 cases / 282
reference values**, including Cyrillic, Hebrew, combining marks, and emoji.

The numerical and text suites use native `IMPORT`. Their 412 reference values
pass in the interpreter and scalar executable. The organism modules are all AML.

`src/lexicon.aml` connects these organs: whitespace-delimited word lists,
rolling triples from token lists, unique words, exact tuple intersections, and
the full Harmonix observation. Triples retain their three separate components;
tokens containing spaces or punctuation keep their identity. Duplicate triples
still contribute to arousal through the original list lengths. The
[lexical parity record](LEXICAL_PARITY.md) defines this boundary and its fixtures.
The input to the observer is already tokenized; SentencePiece and the original
lowercasing/regex fallback remain in the tokenizer migration.

`src/cloud.aml` now carries ordered word weights, frequencies, last-use clocks,
and exact origin labels. Each active occurrence boosts its word; dormant words
decay once per nonempty update. `src/memory.aml` keeps observer trigrams,
generator transition counts/vocabulary, and recent-ten snapshots separate.
The [cloud parity record](CLOUD_PARITY.md) specifies event order, the original
seed corpus, float32 bounds, and the explicit empty-input and snapshot repairs.
These states live in memory; native file persistence remains a later step.

## Preserve the organism

| Python source | Observed mechanism | Planned AML home |
|---|---|---|
| `tokenizer.py` | SentencePiece unigram pieces, lowercasing, boundary-marker removal, sliding trigrams; regex fallback | `tokenizer.aml` |
| `haiku.py` | 587 seed entries / 576 unique words; order-2 Markov chain; five candidates; syllable constraints | `generator.aml`, language data |
| `haiku.py` MathBrain | 5→8→1, tanh at both layers, 57 scalar parameters, MSE/SGD, parameter clamp | `mathbrain.aml` through NoTorch |
| `harmonix.py` | Jaccard overlap, pulse adjustments, exact-trigram discount, temperature 0.3 + 1.2d, word boost/decay | `harmonix.aml`, `cloud.aml` |
| `rae.py`, `rae_recursive.py` | Rule fallback and a second 57-parameter MLP; three refinement passes and online observation API | `rae.aml` |
| `metahaiku.py` | Eight-snippet bootstrap buffer; internal candidate at T=0.7; reflection history | `metahaiku.aml` |
| `overthinkg.py` | Echo/drift/meta rings with 5/7/3 candidate trigrams and coherence admission | `overthinkg.aml` |
| `phase4_bridges.py` | Cosine matching, transition counts, quality change, boredom/overwhelm/stuck penalties | `bridges.aml` |
| `dream_haiku.py` | Trigger/cooldown, four recorded exchanges, friend T=1.2 / self T=0.9, fragment decay and chain feedback | `dream.aml` |
| `chat.py`, `async_harmonix.py` | Foreground exchange orchestration; separate asynchronous observer with locks and async storage | `haiku.aml`, `life.aml`, `state.aml` |

Preserve foreground event order as an explicit fixture: tokenize → cloud/trigram
update → generator chain update → recent-trigram read → dissonance → five
candidates → RAE choice → response → quality/state transition → MathBrain
observation → reflection → rings → dream → shard/metrics. In particular,
`update_chain()` updates recent trigrams before dissonance reads them.

## Connections to develop

Source inspection identifies these concrete integration tasks:

- `chat.py` calls `haiku_gen.observe()`; it never calls `rae.observe()`.
  Wire both learners to the same recorded experience and verify both updates.
- `chat.py` invokes reflection, expansion, and dreams synchronously after
  printing. `async_harmonix.py` supplies a separate async observer. The AML
  lifecycle will schedule these organs with explicit snapshot/update ownership.
- `MetaHaiku.reflect()` builds `reflection_seed` but does not pass it to
  generation; `update_cloud_bias()` ends in `pass`. Connect the bootstrap and
  reflection feedback with a test that changes the next generation state.
- The recursive selector appends its prior score, then slices the feature
  vector back to five dimensions. Current recurrence is the final
  `0.7 * score + 0.3 * previous_normalized_score`. Preserve that baseline;
  deeper learned feedback is a separately measured extension.
- Dream code computes friend/response seeds without using them in generation.
  The newer C sibling provides a useful distinct-friend design.
- Dream `last_run_turn` is incremented by one after a dialog. Store the actual
  foreground turn in AML and test the intended ten-turn cooldown.

These changes should carry their own before/after observations so the port's
lineage and its developments stay traceable.

## Two-dependency replacement map

| Existing dependency/service | Work required |
|---|---|
| NumPy transition sampling | Native count-power normalization and categorical draw; explicit PRNG state |
| NumPy clip/dot/norm | Scalar bounds in AML; vector operations through NoTorch |
| NumPy object shards | Versioned exchange records in the native state format |
| SciPy `csgraph/eigsh` | Both observer files import them; neither calls them. No eigensolver is needed for the current path |
| Micrograd classes | NoTorch linear/bias, tanh, squared-error/backward, SGD, and weight clamp; match each 57-parameter network |
| SentencePiece package | Native unigram-model/tokenizer support in NoTorch, including the model's normalization behavior; piece-ID and trigram fixtures before replacement |
| `syllables` | Per-language syllable lexicons and rules carried as project data/code |
| SQLite / aiosqlite | Native snapshot + journal storage covering words, counts, recent trigrams, metrics, bridges, dreams, learners, and lifecycle state |
| Python collections/async/I/O | AML text values, collections, module exports, event transport, files, and host-facing I/O |

NoTorch exposes linear/bias operations, tensor arithmetic, backward, and
Adam/AdamW/Chuck. The initial audit found missing standalone tanh and SGD;
merged PR #159 now supplies `nt_tanh` and `nt_tape_sgd_step`. Its tests compare
forward values, gradients, and 24-step parameter trajectories against an
independent double-precision reference for both Haiku-sized networks.
AML currently implements its own numeric tape, so the NoTorch binding must
explicitly route Haiku's numerical work to the canonical library.

The baseline build uses AML, NoTorch, and system libraries. The inspected
`amlc` originally added OpenBLAS on Linux while `--no-accel` also disabled
runtime auto-linking. The source-origin follow-up adds `--scalar`, which links
the two projects' scalar archives without BLAS. The numerical test uses it.

## Language work comes first

AML v5.2.0 adds UTF-8 values, typed function arguments/returns, `PRINT`, and
shared `IMPORT`; v5.3.0 adds mutable string lists with copied assignment and
worker containers. AML v5.4.0 adds ordered numeric maps, exact composite keys,
assertions, and scalar floor. These extend the scalar/array runtime, field
persistence, threads, and float channels. Haiku still needs:

1. Durable word-cloud and transition records. Token lists, indexed numeric
   lookup, ordered words, frequencies, weights, and in-memory updates now run
   in AML. Their native save/restart format must preserve every state owner.
2. Text/array event payloads and worker lifetime handling. Existing channels
   carry floats; the default slot limits are 16 spawns and 16 channels.
3. General file/record storage and a small local HTTP/event interface.
4. A NoTorch binding with a defined training owner. Both inspected runtimes
   use global tape state; serialize learning and publish completed snapshots.

Yent demonstrates the compiled/library connection through C-bodied BLOOD
blocks. Haiku's application behavior will be expressed in AML; the language
and numerical libraries provide the general primitives underneath it.

### First compiler repair

At the pinned AML commit, this program prints one marker through the interpreter
and both markers after compilation:

```aml
x = 0
if x == 1:
    ECHO WRONG_BRANCH
else:
    ECHO EXPECTED_BRANCH
```

`amlc` strips indentation and emits one `am_exec()` per line.
[AML PR #25](https://github.com/ariannamethod/ariannamethod.ai/pull/25), commit
`380ef8bf78e6a6e1b2d5670e174f14c02faa6811`, retains one ordered, indented runtime
program and propagates runtime errors before C `main()`.

The regression suite compares interpreted/compiled output for branching,
functions, loops, arrays, and async channel delivery. It also checks mixed
BLOOD/AML initialization, runtime failure, and oversized compiler lines.
The test fails against the original compiler and passes after the repair.

### Source origins and asynchronous failures

The second repair is [AML PR #26](https://github.com/ariannamethod/ariannamethod.ai/pull/26),
tested commit `11b54b8efff41310b1a1f9bd578960bd71e438ef`.

Review of merged PR #25 exposed missing source origins in compiled programs
and ignored child execution errors. The follow-up adds `am_exec_source`,
preserves relative/quoted nested includes from other launch directories, and
stops failed parents before C `main()`. It also preserves nested SPAWN indentation
and carries worker diagnostics through AWAIT. `--run` passes literal arguments
and reports the program's status. These repairs are required by Haiku's future
module and background-learning paths.

### Native modules and text

The v5.2.0 language follow-up is [AML PR #27](https://github.com/ariannamethod/ariannamethod.ai/pull/27),
tested source tree published as `d7e695fdcfd3b5fe0ec02c5776d8f2560f94b995`.
It supplies shared module preparation, canonical
file deduplication, cycle/collision/budget diagnostics, and source origins for
imported functions and workers. Resumable and bytecode programs retain their
prepared sources. `amlc` embeds the root; imports are runtime source inputs.

UTF-8 strings now travel through variables, typed parameters and returns,
`PRINT`, and persistent host values. Eight general text intrinsics provide
codepoint operations. Workers receive a snapshot of globals with copied arrays
and atomic string references; their persistent tables belong to each thread.
Haiku's Python whitespace and line-coherence rules live
in `src/text.aml`. Loops retain a 10,000-iteration budget and fail explicitly
instead of silently truncating a longer text scan.

### String collections and lexical observation

AML v5.3.0 adds native string lists, eight typed intrinsics, JSON list output,
and copied containers for assignment, persistent storage, and worker snapshots.
Its tested toolchain is [AML PR #28](https://github.com/ariannamethod/ariannamethod.ai/pull/28),
commit `11bedd5e6cc042cc80bfc85f1d30443df09827d2`.
Function parameters retain shared containers so AML functions can append words.
Persistent replacement is prepared before publication; allocation failure
preserves the old table. These primitives carry Haiku's growing lexical data
without a new dependency.

The new word-splitting fixtures exposed an older AML branch bug: a nested false
`if` could consume its outer `else`. Pairing now checks indentation. Haiku's
lexical suite checks 324 numerical values and 92 ordered lists against Python,
plus six explicit malformed-triple failures. All four organism modules together
pass **323 cases / 828 reference results** interpreted and compiled.

### Numeric maps and living memory

AML v5.4.0, [PR #29](https://github.com/ariannamethod/ariannamethod.ai/pull/29),
tested source `7411864d699f88e3dff3367f84611b0ff5d85133`, adds ordered maps
from UTF-8 strings to finite scalars. Hashed lookup supports growing word
weights and counts. `list_key` preserves complete token tuples with byte-length
prefixes; empty tokens and embedded delimiters remain distinct. `assert` and
`floor` supply typed application preconditions and exact-integer checks.

Haiku keeps four separate state owners: word-cloud columns, observer
trigrams, generator transitions/vocabulary, and the last-ten snapshot. Seeds
initialize the generator without becoming recent interaction memory. Short
inputs can enter the cloud before they form any generator transition. These
boundaries and the copied recent snapshot prepare the foreground/dream handoff.

Cloud and transition updates prevalidate input shape and counts. Word boosts
and dormant decay are staged before publication. The application currently
limits each state/batch to 10,000 records, matching AML's loop budget; its
counter domain is the exact float32 integer interval through 2^24. Last-use
time is supplied as relative seconds. Allocation failure during publication
across separate columns remains an explicit error; durable atomic exchange
records and epoch timestamps belong to the persistence step.

### RAE development

[RAE_RESEARCH.md](RAE_RESEARCH.md) records a 96-case probe of the original
selector: additional refinement passes do not change the winner. It contains
three separately testable developments — recurrent feedback within the existing
57 parameters, replay memory, and output-layer Hebbian traces — with controls,
metrics, and explicit asynchronous publication order.

## Implementation order

1. Connect native tokenization to the working lexical observer, word cloud,
   and transition memory. Unicode boundary fixtures already cover Cyrillic,
   Hebrew, accents, emoji, whitespace, and empty input; tuple fixtures preserve
   exact token identity and duplicate counts.
2. Bind the merged NoTorch primitives into AML. Compare MathBrain/RAE
   forward values, gradients, and parameter trajectories to Python fixtures.
3. Complete the English tokenizer, generator, and foreground exchange around
   the working cloud and Harmonix. Pin tokenizer mode, word ordering, both Python RNG streams,
   initial state, and event order when producing reference fixtures.
4. Port learning, bridges, reflection, rings, and dreams; connect the
   integration tasks above. Verify save/restart continuity and worker shutdown.
5. Add Klaus-inspired language packs with haiku-specific vocabulary and
   syllable behavior; preserve each language's own cloud.
6. Add the local character interface, conversation, and event-driven animation.
   [TODO](../TODO.md) records the future visible conversation with the inner friend.

## Verification receipt

Linux x86_64, GCC 13.3.0, Python 3.12.14. Fresh cloned sources at the commits above.

- From `harmonix/haiku`: `python -m pytest tests -q` → **161 passed**.
  Reference-only packages: NumPy 2.3.5, SciPy 1.17.0, syllables 1.1.5,
  SentencePiece 0.2.2, aiosqlite 0.22.1, pytest 9.1.1, pytest-asyncio 1.4.0.
- From AML: `make -j2 all test` → **550/550 passed** on the baseline.
- Existing `bash tests/test_amlc.sh` passed before and after the repair.
- Repaired `make test-amlc`: all compiler checks passed; the new behavioral
  comparison failed against the original compiler with all five forbidden
  branch markers present.
