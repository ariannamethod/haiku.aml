# Asynchronous life — one owner, an ordered conversation

2026-10-13. Governed by the
[Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

The current organism publishes one completed exchange: a detached candidate
passes validation, its checkpoint is saved, and the live record is swapped.
The next asynchronous stage will carry that ownership across workers. First,
Phase4 and dreams follow the Python chat's complete event order.

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

## What AML already owns

AML source inspected at `0d8a28304cb528d8a714899dea66efc3ebef03b5`:

- `SPAWN` takes a launch-time copy of mutable record, array, list and map
  globals. Immutable strings and tokenizers retain atomic references.
- Workers have their own persistent-global stores and release them on exit.
- Named and bare `AWAIT` propagate worker execution errors to the awaiting
  program, including source diagnostics.
- Haiku's numerical operations use caller-owned arrays and random streams.
  Its learners share no numerical tape.
- Native typed checkpoints and `record_swap` already support a single
  publisher of a complete life. See [STATE](STATE.md).

These contracts are in `core/ariannamethod.c`: the worker entry/launch code,
`symtab_copy_value`, `symtab_snapshot`, and the spawn join functions.

## Measured worker boundaries

The following small programs were run with the AML runner on 2026-10-13.
Source inspection identifies the corresponding execution paths.

| Probe | Observed result | Runtime path |
|---|---|---|
| Seventeen sequential spawn/join pairs | Launch 17 fails with `cannot launch worker` | Joined slots are retained; `g_spawn_count` reaches `AM_MAX_SPAWNS = 16`. |
| A worker calls a function defined by its parent | The expression returns `0` | The worker receives globals and its block text; the parent's function table is absent. |
| A worker inside a function reads that function's argument | The expression returns `0` | Launch copies `ctx->globals`, not the caller's local bindings. |
| Parent finishes while its worker waits on an empty channel | Runner exits before the worker finishes | Runner/program teardown does not await outstanding workers. |
| Write to a nonexistent channel | AML proceeds without an error | The command discards `am_channel_write`'s failure result. |
| Read from a nonexistent channel | Destination keeps its previous value | The command does not propagate `am_channel_read`'s failure. |

For example, the slot limit is reached even though every worker was joined:

```aml
i = 0
while i < 17:
    SPAWN worker:
        result = 1
    AWAIT worker
    i = i + 1
PRINT i
```

This program prints `0`, showing the function-table boundary:

```aml
def twice(x):
    return x * 2
SPAWN calculation:
    PRINT twice(3)
AWAIT calculation
```

The other probes can be reproduced independently:

```aml
# A function-local argument does not enter the worker's global snapshot.
def start(value):
    SPAWN local:
        PRINT value
    AWAIT local
start(7)
```

```aml
# The runner prints PARENT_EXIT and exits while this worker is waiting.
CHANNEL CREATE ready 1
SPAWN worker:
    CHANNEL READ ready signal
    ECHO WORKER_FINISHED
ECHO PARENT_EXIT
```

```aml
# A missing channel currently leaves both command failures unreported.
CHANNEL WRITE absent 7
value = 99
CHANNEL READ absent value
PRINT value
```

Channel payloads are floats, so the current queues cannot return an experience
or a learned state. Worker globals are released on exit. Reusable worker slots,
typed result transport, explicit channel errors and orderly shutdown are the
next language work for this path. Channel creation/individual closure also need
the same registry synchronization as reads and writes. A worker can currently
`INCLUDE` an entry file that imports its own modules; inherited parent functions
are not part of its execution context.

AML's resumable C API yields between top-level statements. A `while` or function
call completes inside one step (`am_program_step` and its source comment).
Haiku's existing REPL loop therefore cannot become an event loop just by placing
that program behind `am_program_step`.

## The next publication boundary

The first worker profile should have one job in flight and one live-state
publisher. A job freezes the accepted turn, source, selected response, contexts,
scoring features, climate, quality and base revision with its detached state.
Its worker executes the same ordered tail as the serial profile. The parent
validates a completed result against its base revision, saves it and swaps it
into the live owner. Workers never write the live checkpoint.

The next foreground exchange, quit and EOF drain the preceding job. Completing
a worker sooner changes waiting time; it does not choose the next experience
or random draw. Reading the next input can overlap with the worker's work.

If a response is displayed before learning and the inner tail finish, a durable
pending-experience intent must precede display. One compact restart boundary
keeps the previous complete state plus the accepted input, selected response and
revision. Restart replays that input from the saved state, checks the same
response, completes the tail and commits it once. Replay remains detached;
partial learning and dream feedback never enter the published owner.

This profile needs its own explicit upgrade. Existing checkpoint profiles keep
their current continuation. The [TODO](../TODO.md) tracks the shared experience
record and later worker integration.

## Gates for that stage

- Fixed inputs and draws produce identical responses and final checkpoint bytes
  through the serial and worker schedules.
- Controlled completion before, during and after the next input preserves the
  same publication order; quit and EOF drain work and surface errors.
- Restart from a pending intent reproduces the selected response, parameters,
  random position and dream feedback exactly once.
- Duplicate and wrong-revision results leave live state and its checkpoint
  unchanged.
- Launch, copying, worker execution, validation and saving failures retain the
  last complete owner and any durable pending intent.
- More than sixteen completed jobs, close while blocked, typed-result ownership
  after worker teardown, and shutdown with a failed worker have direct tests.
- Interpreted, compiled and host execution retain the same contracts.

These are the next worker gates. The current sequential milestone is checked
at its complete-turn boundary.
