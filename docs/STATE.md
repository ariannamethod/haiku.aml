# Continuity — the cloud remembers

2026-10-08. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
Version 1 saves the complete foreground organism from [state.aml](../src/state.aml).
With the same runtime and profile, closing the process and reopening its
checkpoint preserves the next response, the next learning update, and both
random streams exactly.

## One owner, one completed turn

[haiku.aml](../haiku.aml) uses `haiku.state` beside its own source file. The path
retains that source origin when compiled. A present checkpoint supplies its
tokenizer mode and `train_rae`; the fresh settings and seeds apply only to a new
life. Set `state_path = ""` for an unsaved session. Each path has one running
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

## The 29 fields

The generic AML checkpoint preserves IEEE-754 binary32 bits, array shapes,
UTF-8 bytes, and record/map/list insertion order. There is no save-time timestamp.
Haiku accepts exactly this versioned schema:

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
texts, the selected response, gradients, and temporary feature arrays are local
to one completed exchange; the next turn recomputes them. Stateless NoTorch SGD
has no optimizer buffer to save.

Validation checks exact fields/types, finite parameter arrays, ordered triple
rows/counts, cloud columns, vocabulary/cache coverage, observer/generator count
relations, recent membership, bounded clocks/counts, learner statistics, both
RNGs, draw cursor/tape, and the reconstructed bridge event. MathBrain observations
equal completed turns; RAE observations can range from zero through that count.
The logical clock resumes at the next integer. Cloud decay still occurs once
per active exchange, independent of elapsed wall time.

Version 1 persists the implemented foreground. Reflection, rings, dreams,
Phase4 transition aggregation and event history, and background workers retain
their later migration stages.

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

The historical Python audit is a development reference:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/reference/persistence_audit.py \
  --python-haiku ../harmonix/haiku --check
```

Its original Python dependencies belong to that reference environment. Running
and resuming Haiku uses AML and NoTorch.
