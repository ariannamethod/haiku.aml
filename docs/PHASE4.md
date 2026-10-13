# The islands remember their bridges

`src/phase4.aml` carries Python's island memory into the same checkpoint as
the cloud, learners, and inner voices. Each completed exchange leaves its
climate, its quality change, and a bridge from the previous named state.

The reference is `harmonix/haiku/phase4_bridges.py` and its caller in
`chat.py`, revision `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`.

## What a bridge remembers

The chat records Phase4 before MathBrain learns. Its two metric maps contain
the same current dissonance, entropy, novelty, and arousal. Quality changes
from 0.5 to the exchange's computed quality. The edge's similarity is the
cosine of those **two current maps**. The previous state's ID names the
edge's starting island; its older snapshot does not enter this cosine.

Cosine uses shared keys only, including the norms. Empty intersections and
zero norms produce zero; negative similarity remains negative. Each shared
vector is scaled by its largest absolute value before three canonical
NoTorch `nt_linear` calls compute the dot products. This keeps finite
binary32 extremes and subnormals usable without squaring them into overflow
or zero. AML composes the norms and the original transition score.

Each activation appends its exact before/after maps, flags, clock, and IDs.
Its state's snapshot keeps the latest after-map and total activations.
An explicit nonempty previous ID adds to the matching edge's count, sum of
similarities, quality changes, and overwhelm/boredom/stuck flags. The previous
ID may name an island without a snapshot, as in Python. An empty previous ID
records the activation without adding an edge.

Suggestions follow the source's two-stage selection:

1. Filter outgoing edges by minimum count; retain at most twice the requested
   result count, ordered by count descending.
2. Sort that shortlist by the original composite score and return the requested
   number. Equal counts and equal scores retain edge insertion order.

A high-scoring edge outside the count shortlist remains outside the result.
The source prints suggestions only in debug output. Generation, learning,
reflection, and dreams retain their existing inputs; querying the bridges
consumes no random draws.

## Names in the language's precision

`haiku_bridge_id(dissonance, entropy, quality)` uses AML's `text_fixed` to
produce `d0.7_e0.5_q0.6`. Each component rounds its actual binary32 value to
one decimal with ties to even, preserving negative zero. Python's formatting
of the **same exact binary32 inputs** is the text reference.

The port's numeric representation changes four useful decimal boundaries:

| Decimal input | Python binary64 ID component | AML binary32 ID component |
|---|---|---|
| 0.15 | 0.1 | 0.2 |
| 0.45 | 0.5 | 0.4 |
| 0.65 | 0.7 | 0.6 |
| 0.85 | 0.8 | 0.9 |

The reference receipt records both spellings and exact stored inputs. At
exact binary ties, 0.25 becomes 0.2 and 0.75 becomes 0.8. State naming uses
the same general formatter as turn IDs; it has no metric-specific rounding
table.

## Nine leaves, one owner

The whole-life schema supplies `phase4_started_turn`. This organ supplies:

| Leaf | Type | Contents |
|---|---|---|
| `phase4_prev` | Text | Latest recorded state ID; empty before the first event |
| `phase4_log` | String list | Repeated state ID, previous ID, turn ID |
| `phase4_log_rows` | Vector | Clock; before offset/count; after offset/count; overwhelm, boredom, stuck |
| `phase4_metric_names` | String list | Unique metric names in first-use order |
| `phase4_metric_rows` | Vector | Repeated name-index/value pairs, retaining each map's key order |
| `phase4_states` | String list | State IDs in first-activation order |
| `phase4_snapshots` | Vector | Repeated activation count and latest log index |
| `phase4_edges` | String list | Repeated from/to IDs in first-transition order |
| `phase4_totals` | Vector | Count and sums of similarity, quality change, overwhelm, boredom, stuck |

Metric offsets count pairs. All indices start at zero. Empty numeric vectors
use `[0]`; empty lists use `[]`. Arbitrary map keys, missing quality, empty
maps, Unicode identifiers, and negative similarities retain their behavior.
Missing quality contributes the source default 0.5.

The organ retains up to 10,000 events, 10,000 unique metric names, 10,000 keys
per individual map, and 1,000,000 metric scalars. It rejects the next event
when a bound is exhausted. No older event is discarded. Numeric copies and
validation use bounded inner loops, so histories above 10,000 scalar entries
continue to work. Identifiers have a 10,000-codepoint limit; every metric,
clock, quality change, and accumulated sum must remain finite.

`haiku_phase4_check` reconstructs snapshots and transition totals from the
retained log. It verifies contiguous offsets, unique map keys, dictionary and
edge order, exact aggregate values, and the latest previous-state pointer.
The whole-life checker additionally ties chat rows to their turn sequence,
reflection climate, and latest bridge event.

Python keeps its SQLite history on disk but resets `prev_state_id` when chat
restarts. AML persists that pointer with the rest of the life: the first new
exchange continues the bridge from the last completed exchange. Existing
profiles begin recording through an explicit migration boundary; old turns
are never invented or replayed. [Continuity](STATE.md) defines the upgrade.

## AML interfaces

| Call | Result |
|---|---|
| `haiku_phase4_init(state)` | Installs nine empty leaves in the supplied owner |
| `haiku_phase4_check(state)` | Validates all leaves and returns the event count |
| `haiku_phase4_turn(state, event)` | Records the foreground bridge event and returns its state ID |
| `haiku_phase4_record(state, current, before, after, previous, turn_id, clock, flags)` | Records arbitrary before/after maps; flags are overwhelm/boredom/stuck |
| `haiku_phase4_metrics(state, index, after)` | Returns a detached metric map; `after` is 0 or 1 |
| `haiku_phase4_entry(state, index)` | Returns the event's IDs, clock, maps, and flags in a flat record |
| `haiku_phase4_snapshot(state, id)` | Returns latest metrics, clock, and activation count; missing state gives an empty record |
| `haiku_phase4_suggest(state, id, min_count, max_results)` | Returns from/to lists and a numeric statistics vector |
| `haiku_phase4_history(state, id, limit)` | Returns `count` and a vector of log `indices` |

Suggestion statistics repeat count, average similarity, average quality
change, overwhelm rate, boredom rate, stuck rate, and score. History sorts
clock descending, retaining insertion order for equal clocks. Negative
history limits return all matching events. Negative suggestion limits retain
Python's unbounded SQL shortlist followed by a negative slice.

Mutation receives a detached turn. Entry, metric, and snapshot accessors read
an already validated owner and return independent values. The session owns
whole-record validation, saving, and publication; an unfinished candidate
cannot publish its bridge memory.

Within event admission, assigning `record_get(state, "phase4_metric_names")`
to the local `names` list copies its container under AML's list-assignment
rules. New dictionary entries remain local until `record_set` publishes the
completed event. If a second unseen key exceeds the dictionary limit after
the first was staged, the owner's original dictionary and history remain intact.

## Receipts

`bash tests/run_phase4.sh` runs without Python:

- 16 state-ID boundaries and 12 shared-key cosine cases, including large
  finite values, subnormals, opposite vectors, Unicode keys, and zero norms.
- 26 directly recorded Python SQLite events, every before/after map,
  snapshot, and aggregate, timestamp ties, and eight suggestion queries.
- Interpreted and compiled execution, isolated owners, frozen accessor
  results, unchanged random streams, and a 40,000-scalar metric history.
- Thirty malformed states and thirteen rejected calls, with C host inspection
  confirming that every inspected leaf retains its exact bytes.
- A valid 10,000-event history and rejection of its next admission.
- Dictionary overflow from 9,999 entries, including new keys split between
  before/after maps, preserves all nine leaves and byte-identical checkpoints.
  A shared new key fills the last slot once; existing keys remain usable at
  full capacity. A direct-owner insertion mutation makes this regression fail.

Strings, counts, map contents, and ordering compare exactly. Derived cosine
and transition statistics use absolute tolerance 0.00001. The committed
Python oracle calls the pinned source directly and also checks its NumPy and
standard-library cosine branches. Regenerate or verify its receipts with:

```sh
PYTHONDONTWRITEBYTECODE=1 PYTHONPATH=../reference-deps \
  python tests/reference/phase4_oracle.py --python-haiku ../harmonix/haiku --check
```
