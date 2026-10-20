# Asynchronous life — one owner, an ordered conversation

2026-10-14. Governed by the
[Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

Both launchers now persist the accepted foreground, display its reply and run
the complete learning and inner-life tail in an owned worker. The worker
validates and atomically saves the complete life while the foreground reads
input. Before processing that input, quit or EOF, the foreground joins and
adopts the saved result. Python's inner event order, including Phase4 and dreams,
is preserved. `background_life=0` retains the earlier join-before-display path
described below; `owned_tail=0` selects its sequential control.

## The Python path

Reference: `harmonix/haiku` at
`abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`.
[`chat.py`](https://github.com/ariannamethod/harmonix/blob/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku/chat.py)
imports ordinary `Harmonix` and runs a synchronous `main`. Its runner calls
that function directly. After choosing and displaying a response, chat runs:

1. Phase4 transition recording.
2. MathBrain learning and the previous-state-ID update.
3. MetaHaiku reflection.
4. Echo, drift and meta rings.
5. Dream eligibility, dialogue, fragment feedback and generator updates.
6. Exchange shard creation and metrics.
7. The next input.

These are direct calls in `chat.py:206–300`. The comments call the later organs
“background”; each call finishes before the next input. This order fixes when
learning, new trigrams and shared random draws enter the next exchange.

[`async_harmonix.py`](https://github.com/ariannamethod/harmonix/blob/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku/async_harmonix.py)
provides a separate asynchronous observer. Each instance owns one
`asyncio.Lock`. Dissonance, cloud morphing, shard creation and metric writes
hold it across their awaited database operations; cloud/statistics reads use
the same lock. Shard directory creation and NumPy saving use awaited
`asyncio.to_thread` calls while that lock is held. Concurrent callers are
exercised with `asyncio.gather` in its tests.

Its dissonance method also stores `user_trigrams + system_trigrams` under the
lock (`async_harmonix.py:217–220`). Ordinary `Harmonix.compute_dissonance`
performs no such write, and chat already stores user trigrams separately.
The AML chat retains the ordinary observer's counts. The asynchronous port
carries the field-ownership discipline without adding those extra observations.

The pinned asynchronous test module passes **31 tests**:

```sh
cd ../harmonix/haiku
PYTHONDONTWRITEBYTECODE=1 python -m pytest tests/test_async_harmonix.py -q -p no:cacheprovider
```

This command uses the Python reference environment described in
[MIGRATION](MIGRATION.md). The organism's runtime dependencies remain AML and
NoTorch.

## The owned worker boundary

This profile requires [AML 5.11 / PR #36](https://github.com/ariannamethod/ariannamethod.ai/pull/36).
Full completion receipts also require
[AML PR #37](https://github.com/ariannamethod/ariannamethod.ai/pull/37), which adds
exact flat-record equality and validates `AWAIT ... INTO` before joining.
Its [worker contract](https://github.com/ariannamethod/ariannamethod.ai/blob/8a5d449195ed8d45eecf08279001802aee12673a/docs/WORKERS.md)
supplies the generic language work:

- `SPAWN` snapshots prepared functions and their source origins, mutable
  globals, and the visible caller-local frame. Helpers retain their ordinary
  scope; a captured local does not become a helper's global.
- Records, arrays, lists, maps and owned RNG values become worker-owned copies.
  Immutable text and native tokenizer models retain references.
- `AWAIT name INTO value` transfers a typed result beyond worker teardown.
  Joined slots can be reused, and program teardown drains owned children.
- Worker failures reach the awaiting program with source diagnostics. Channel
  creation, lookup, close and blocked operations have synchronized ownership
  and explicit failures.

Haiku's learners use caller-owned numerical arrays and random streams without
a numerical tape. This worker path uses those owned operations. The language's
legacy global field and autograd interfaces have their own separate contracts.

[`src/session.aml`](../src/session.aml) now splits the accepted exchange into
three boundaries:

| Function | Work and owner |
|---|---|
| `haiku_session_prepare` | Mutates a detached candidate through cloud observation, candidate generation and RAE selection. Freezes input, response, contexts, features, climate, quality and base revision in an experience record. |
| `haiku_session_tail` | Applies Phase4, learner updates, reflection, rings, dreams and journaling in the existing order, then validates the candidate. Returns a transient envelope carrying the state and full frozen experience. Both serial and worker schedules call this same function. |
| `haiku_session_result_check` | Validates the envelope and compares every frozen experience field with the supplied receipt. Checks base/completed revisions, profile, learner mode and tokenizer identity, then returns an independently owned state for publication. |

`haiku_session_worker_turn` launches one `haiku_tail` worker with the detached
candidate and experience, then joins its typed record. A launch-time snapshot
keeps later parent mutations out of that work. Worker code never opens the live
checkpoint. The serial `haiku_session_turn` remains available as a comparison
schedule.

The flat envelope keeps `state:*` and `experience:*` in separate namespaces,
with their ordered field lists, format and version. `record_equal` checks the
receipt's keys, value types, string/list bytes, array shapes and every float32
bit in scalars, arrays and maps. Same revision and same spoken text therefore
cannot substitute for the quality, observation, features and bridge actually
consumed by that job. Only the unwrapped state can reach the checkpoint;
the v1/v2/v3 schemas remain exactly 29/36/55 fields.

The earlier completion check accepted a v1 `moon` result with a same-base
`rain` receipt. Inner profiles checked the input and response but did not bind
the other experience fields. The full receipt closes both paths before any
owner is replaced.

The [`haiku.aml`](../haiku.aml) entrypoint passes its own detached `next_state`
to that call, saves the complete result, swaps it into the live owner without
allocation, then prints the response. Failed saving leaves the old live record
and checkpoint together and withholds the response. Native checkpoint status 2
means replacement succeeded but storage could not confirm durability; the CLI
publishes that written state and reports the status. See [STATE](STATE.md).

With `background_life=0`, `owned_tail = 1` selects the earlier joined worker.
Set `owned_tail` to `0` for serial scheduling. This
setting changes scheduling without changing stable v1/v2/v3 checkpoint schemas
or their draw order. Each accepted turn is joined before display or the next
input. Blank input, quit and EOF therefore have no outstanding turn to drain in
this profile.

## Historical findings, now fixed in the worker path

The original audit ran on 2026-10-13 against AML
`0d8a28304cb528d8a714899dea66efc3ebef03b5`. These measured defects motivated the
language update; they are retained here as observations of that pinned source.

| Probe | Pinned source result | AML 5.11 contract |
|---|---|---|
| Seventeen sequential spawn/join pairs | Launch 17 failed because joined slots remained occupied | Joined slots are reusable; repeated jobs are covered beyond sixteen launches |
| Worker calls a parent-defined function | Returned `0`; the worker lacked the function table | Prepared functions and source origins are inherited |
| Worker reads its launching function's argument | Returned `0`; only globals were copied | The visible local frame is copied with distinct local scope |
| Parent finishes with a worker blocked on a channel | Runner exited before the worker finished | Teardown joins owned children and propagates failure |
| Write to a nonexistent channel | Failure was discarded | The AML command reports the channel error |
| Read from a nonexistent channel | Previous destination survived without an error | The AML command reports the error and preserves the destination |

Channel queues still carry floats. Complete records travel through typed
`AWAIT`, so the organism does not encode a state into a queue of scalar values.
The original source's retained spawn slots, absent function/local snapshots
and unreported channel errors are no longer the boundaries of this profile.

AML's resumable C API still yields between top-level statements. A `while` or
function call completes inside one step. The existing REPL therefore needs an
explicit input/worker schedule for the next stage.

## Executed organism gates

```sh
make test-owned-tail
```

[`tests/run_owned_tail.sh`](../tests/run_owned_tail.sh) compares **24 native
turns** across serial, worker and compiled execution. Responses and complete
checkpoint bytes agree, including the first dream and restart from turn 13.
Separate variants cover old v1/v2 profiles, native SentencePiece, coupled RAE
learning, and blank/quit/EOF.

[`owned_tail_schedule.aml`](../tests/fixtures/owned_tail_schedule.aml) blocks
the real tail, mutates parent launch values, releases the worker, and delays
joining an already completed result. The published owner stays unchanged;
the final record matches the serial dream, learner and checkpoint exactly.

[`owned_tail_state.c`](../tests/owned_tail_state.c) inspects all 29/36/55 live
leaves in the three profiles. Each rejects 13 receipt/envelope mismatches,
including a same-base result from another input, changed quality/observation/
bridge with identical input and response, a one-ULP feature change and signed
zero. Every rejection preserves its complete owner and checkpoint bytes.
Envelope mutation after validation leaves the unwrapped state independent.
The v3 checks also cover stale bases, duplicate completion, an exhausted draw
tape during the inner tail and failed saving. A successful worker after the
failed job verifies slot recovery. The returned record survives thread exit.

The language's own worker gates cover repeated jobs, typed result kinds,
nested ownership, scope, imported source origins, shutdown, channels and
allocation/thread-launch failures. These complement the actual Haiku schedule.

## The current publication rhythm

`src/background.aml` and `src/iceberg_background.aml` retain the exact complete
predecessor, prepared voice and frozen experience; iceberg also retains the
captured day and model request. Their closed pending envelope temporarily
occupies the same checkpoint path. The complete-life schemas stay unchanged.
Replay checks the whole prepared foreground before continuing the ordinary tail.

The root launcher saves pending before display. From SPAWN until AWAIT, the
worker alone can replace that file: it computes, validates and saves completed
state before returning its owned result. Main reads input during that interval
and joins before any subsequent action. Saving fails before rename with the
pending exchange still available; status 2 means the replacement committed.
Startup finishes a pending exchange before applying requested upgrades, without
printing or teaching its accepted response again.

Iceberg recovery uses the captured calendar and provider availability. A source
absent at acceptance stays absent for that exchange if its asset later appears.
A previously available source must still have its original identity; an absent
or changed required asset leaves the pending checkpoint for later recovery.
After completion, the next exchange uses the currently available frozen bodies.
