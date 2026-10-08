# Continuity — the cloud remembers

2026-10-08. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
Version 1 saves the foreground; version 2 adds MetaHaiku and the three rings.
Both schemas live in [state.aml](../src/state.aml).
With the same runtime and profile, closing the process and reopening its
checkpoint preserves the next response, the next learning update, and both
random streams exactly.

## One owner, one completed turn

[haiku.aml](../haiku.aml) uses `haiku.state` beside its own source file. The path
retains that source origin when compiled. A present checkpoint supplies its
tokenizer mode, `train_rae`, and profile; fresh settings and seeds apply only to
a new life. Set `state_path = ""` for an unsaved session. Each path has one running
writer; concurrent writers require a later ownership protocol.

The CLI clones the live record, completes an exchange in that candidate, and
saves it. AML writes a temporary sibling, syncs it, atomically replaces the old
file, and syncs the directory. Only then does the CLI swap the candidate into
the live owner and print Haiku's response. A failure before file replacement
leaves the live owner and previous checkpoint intact. Save status `2` means the
replacement committed but directory durability could not be confirmed; the CLI
publishes that committed state and reports this status. The final record swap
allocates nothing.

Blank input, quit, EOF, rejected input, and failed staged learning leave the
published owner unchanged. Resumption loads a detached record, verifies every
field and cross-organ relation, then publishes it. Restoration replaces the
whole owner; it never adds seeds or replays learned text. Restoring the same
checkpoint twice is idempotent.

`haiku_session_turn` itself stages and replaces an in-memory owner. Applications
that persist a turn use the CLI's outer candidate/save/swap boundary.
`haiku_state_save`, `haiku_state_load`, and `haiku_state_restore` are convenience
wrappers whose relative paths follow **src/state.aml**. The CLI calls generic
checkpoint operations at its own source origin. Absolute paths work in either
interface.

## The 29 foreground fields

The generic AML checkpoint preserves IEEE-754 binary32 bits, array shapes,
UTF-8 bytes, and record/map/list insertion order. There is no save-time timestamp.
Version 1 accepts exactly these fields. Version 2 retains all of them, with
`version = 2` and `profile = "english-inner-v2"`:

| Fields | Type | Meaning |
|---|---|---|
| `format`, `version`, `profile` | string, float, string | `haiku.aml`, `1`, `english-foreground-v1`; identify current foreground rules. |
| `clock_kind`, `turn`, `clock` | string, float, float | `turn`; equal integer counters from 0 through 2²⁴. |
| `tokenizer_mode`, `tokenizer_identity`, `train_rae` | string, string, float | `regex` with empty identity, or `sentencepiece` with the SHA256 of exact loaded bytes; training flag 0/1. |
| `weights`, `frequencies`, `last_used`, `origins` | three maps, list | All cloud columns; origins follow weight-key order. |
| `observer_rows`, `observer_counts`, `observer_resonance` | list, two maps | User observer memory, separate from seeded generator memory. |
| `rows`, `counts`, `vocab`, `recent`, `syllables` | list, map, two lists, map | Ordered generator edges and vocabulary, last ten triples, exact syllable cache. |
| `mathbrain`, `mathbrain_state` | array, map | 57 parameters; learning rate, observation count, last/running loss. |
| `rae`, `rae_state` | array, map | 57 parameters; learning rate and observation count. |
| `voice_rng`, `model_rng`, `draws` | two maps, array | Owned algorithm/state limbs; native `[-1]` or the entire explicit draw tape and cursor. |
| `bridge_event` | map | Last completed handoff: turn/clock, four climate values, before/after quality and three flags. |

The cache is stored, because supplied syllable counts control generation.
The initialization stream is stored after **both** learner initializations.
The tokenizer handle is reopened and its exact byte identity verified. Candidate
batches, gradients, and temporary feature arrays are local to one completed
exchange. Version 1 also keeps its selected response local; version 2 retains
user, selected, and internal text in `meta_history`. Stateless NoTorch SGD has
no optimizer buffer to save.

Validation checks exact fields/types, finite parameter arrays, ordered triple
rows/counts, cloud columns, vocabulary/cache coverage, observer/generator count
relations, recent membership, bounded clocks/counts, learner statistics, both
RNGs, draw cursor/tape, and the reconstructed bridge event. MathBrain observations
equal completed turns. The persisted `train_rae` setting governs the whole
session: RAE observations equal `turn * train_rae`, so fixed RAE has zero and
learning RAE has one observation per completed turn.
The logical clock resumes at the next integer. Cloud decay still occurs once
per active exchange, independent of elapsed wall time.

Version 1 keeps its observer-to-generator count relation. Version 2 validates
observer rows, counts, and resonance independently: rings insert observer-only
triples, preserving their exact cloud token identities. Generator vocabulary
and rows keep their own word/form checks.

## Seven more fields for an inner life

Version 2 requires exactly 36 fields. Its seven additional owners are:

| Fields | Type | Meaning |
|---|---|---|
| `inner_started_turn` | float | Completed-turn boundary at which inner life was enabled. |
| `meta_bootstrap` | list | Last eight admitted snippets, each at most ten words and 100 codepoints. |
| `meta_history` | list | Ordered flat rows of user text, spoken haiku, and internal haiku. |
| `meta_metrics` | array | Five values per history row: turn, dissonance, novelty, arousal, entropy. |
| `ring_trigrams` | list | Latest echo/drift/meta candidates: five, seven, and three triples, in that order. |
| `ring_coherence` | array | Three ring coherence values. |
| `ring_admission` | array | Fifteen per-triple admission coherence values. |

Fresh inner leaves have empty lists and numeric `[0]` sentinels. Every completed
inner turn appends one reflection row and replaces the latest ring receipt.
Custom clouds with fewer than three words retain an empty receipt, following
the rings' skip rule; growth to three words enables the full receipt.
The reflection count equals `turn - inner_started_turn`; its metric turns are
contiguous from that boundary, and its latest climate equals the bridge event.
History retains up to 10,000 complete reflection rows. Reaching that limit
rejects the next turn before publication, retaining the existing checkpoint.

The foreground, bootstrap admission, internal generation, and rings consume
the same owned `voice_rng` or explicit draw tape in event order. Reflection and
ring updates belong to the detached turn candidate. Saving commits all 36
owners together, including observer-only ring insertions.

## Explicit migration

`haiku_state_new` continues to construct version 1. Calling
`haiku_state_enable_inner(state, model)` validates that owner and returns a
detached version 2 clone. It retains every foreground value, sets the boundary
to the current completed turn, initializes the six inner leaves, and consumes
no random values. Calling it on version 2 returns an equal detached clone.
The caller chooses when to save and publish that result.

The CLI starts a fresh life with `inner_life = 1`; set it to `0` to begin with
the foreground profile. Existing v1 checkpoints continue their existing voice
until `upgrade_inner = 1` explicitly enables inner life. Migration starts at
that checkpoint's current turn, retaining its learned foreground memory.
Existing v2 checkpoints resume their own profile regardless of fresh settings.

Dreams, Phase4 transition aggregation and event history, and background workers
retain their later migration stages.

## Python lineage and measured repairs

Reference: `harmonix/haiku` at
`abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`. Its SQLite cloud and observer rows,
generator edges, recent triples, MathBrain JSON, RAE JSON, and NumPy shard files
have separate write boundaries. Generator reload has no explicit SQL ordering
for edges, and its vocabulary starts as a set. Random states and the foreground
turn counter are absent from the saved data. Cloud timestamps use `time.time()`;
weight decay remains interaction-based. MathBrain saves on explicit save/close;
RAE saves every ten observations, while the original chat never observes RAE.

The [audit receipt](../tests/reference/persistence_audit.json) measures the
original load methods: MathBrain saves learning rate `0.075` and reloads `0.01`;
an invalid fourth weight leaves the first three already changed; a NaN weight
loads. RAE accepts two weights while retaining 55 previous values, accepts a
string weight, and changes its counter before a failed `None` weight load.
Two shards created in the same millisecond produce two SQL rows pointing at
one overwritten file. Version 1 validates a complete detached state and commits
one checkpoint for a completed foreground exchange.

## Reproduce

`make test-state` compares an uninterrupted seven-turn conversation with three
turns, save, a fresh process, restore, and four more turns. It covers regex and
the optional verified SentencePiece model, both RAE training modes, native RNGs
and a scripted draw tape, interpreted and compiled execution. Responses and the
complete final checkpoint bytes must match. It also exercises repeated restore,
model mismatch, malformed and semantically invalid snapshots, unchanged live
owners after failed restore/staged generation, and preservation of an existing
checkpoint when validation rejects a save.

`make test-inner-state` repeats the seven-turn comparison for ten variants:
five fresh v2 lives and five v1 lives explicitly upgraded after turn three.
It covers the same tokenizer/training/draw choices, including a supplied
syllable cache. The complete 36-field checkpoint and response text must agree
across interpreted and compiled fresh-process continuations. Host inspection
checks detached migration, repeated migration, rejected restores, exact
observer-only token identities, exhaustion inside MetaHaiku and rings, and
failed file publication with every live leaf and previous file preserved.
A one-word custom cloud also continues across a two-word checkpoint, then
starts its three rings as the third word arrives.

The historical Python audit is a development reference:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/reference/persistence_audit.py \
  --python-haiku ../harmonix/haiku --check
```

Its original Python dependencies belong to that reference environment. Running
and resuming Haiku uses AML and NoTorch.
