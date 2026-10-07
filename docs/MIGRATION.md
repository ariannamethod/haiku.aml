# HAiKU → AML: the first engineering map

2026-10-07. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
This document records the inspected sources, current behavior, and implementation
order. The first numerical functions are implemented; the status below marks
the boundary between those functions and the remaining organism.

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

The manifesto is copied byte-for-byte from the pinned NoTorch source.
SHA-256: `c8288157c8e2a63cec307510534a3da5415a5d44f3c0a089c8d7f6bbde07b906`.

Follow-up bases: AML `feaeb724fc1fccd2cefae1f5f3fd712658c5ee0a` includes merged
PR #25. NoTorch `c92e915f2e533372073cc95df9c22bdaf014188b` includes merged
[PR #159](https://github.com/ariannamethod/notorch/pull/159), adding native tanh
and plain SGD. The original audit and manifesto pins above remain their sources.

## Implemented first organs

`src/harmonix.aml` implements pulse, count-based dissonance, and the two
temperatures. `src/bridges.aml` implements the aggregate transition score.
Text extraction, cloud storage, cosine/state aggregation, and learning remain
separate work. [NUMERICAL_PARITY.md](NUMERICAL_PARITY.md) records **49 cases /
130 scalar values** obtained by direct calls to the pinned Python implementation.
Both interpreted and compiled AML pass with tolerance 0.000002.

The test assembles one source explicitly while native shared module exports
are pending. No Python or BLOOD C implements the organism in this repository.

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

The current AML runtime has scalar/float-array variables, functions, return
values, numeric operations, field persistence, threads, and float channels.
Haiku needs:

1. UTF-8 strings, token lists, dictionary/set operations, integer counts and
   indices, and predictable ownership.
2. Module imports that share exported definitions. Current `INCLUDE` calls
   `am_exec_file()` in a separate context; module export semantics need work.
3. Text/array event payloads and worker lifetime handling. Existing channels
   carry floats; the default slot limits are 16 spawns and 16 channels.
4. General file/record storage and a small local HTTP/event interface.
5. A NoTorch binding with a defined training owner. Both inspected runtimes
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

### RAE development

[RAE_RESEARCH.md](RAE_RESEARCH.md) records a 96-case probe of the original
selector: additional refinement passes do not change the winner. It contains
three separately testable developments — recurrent feedback within the existing
57 parameters, replay memory, and output-layer Hebbian traces — with controls,
metrics, and explicit asynchronous publication order.

## Implementation order

1. Finish native text/collections/module contracts in AML, building on the
   compiler and scalar-linking repairs. Exercise Cyrillic, Hebrew, accents, and empty input.
2. Bind the merged NoTorch primitives into AML. Compare MathBrain/RAE
   forward values, gradients, and parameter trajectories to Python fixtures.
3. Port the English cloud, tokenizer, generator, Harmonix, and foreground
   exchange. Pin tokenizer mode, word ordering, both Python RNG streams,
   initial state, and event order when producing reference fixtures.
4. Port learning, bridges, reflection, rings, and dreams; connect the
   integration tasks above. Verify save/restart continuity and worker shutdown.
5. Add Klaus-inspired language packs with haiku-specific vocabulary and
   syllable behavior; preserve each language's own cloud.
6. Add the local owl interface, conversation, and event-driven animation.
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
