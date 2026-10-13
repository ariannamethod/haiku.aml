# A friend arrives after the inner voice

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
Source: `harmonix/haiku/dream_haiku.py`, `chat.py`, `haiku.py`, and
`tokenizer.py` at `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`.

`src/dream.aml` gives Haiku its dream dialogue. After reflection and the three
rings, the last spoken haiku opens a conversation with an imaginary friend.
The friend answers at temperature **1.2**; Haiku answers at **0.9**. Both draw
from the same generator and owned voice stream. Their words return as dream
fragments and speaking transitions.

## When the dream opens

The original gates run in this order:

1. At least ten foreground turns since the previous dream.
2. Quality below `0.45`, novelty above `0.7`, or dissonance strictly between
   `0.4` and `0.7`.
3. One uniform draw, accepted when it is **at most `0.25`**.

Cooldown and climate failures consume no draw. The probability comparison
retains equality: probability zero accepts an exact zero draw. The foreground's
current quality formula starts at `0.5`; its dreams therefore reach the novelty
or dissonance gates. The standalone dream API preserves the low-quality gate.

The saved configuration is `[minimum interval, probability, exchanges]`,
defaulting to `[10, 0.25, 4]`. Intervals are integral `1..2^24`, probabilities
are finite `0..1`, and an event records `1..16` exchanges. Changing this
configuration preserves earlier dialog boundaries.

## Four exchanges, one more response

At the default length, the recorded conversation is:

| Position | Speaker | Text |
|---|---|---|
| 0 | Haiku | The preceding foreground haiku |
| 1 | Friend | New candidate at temperature 1.2 |
| 2 | Haiku | New candidate at temperature 0.9 |
| 3 | Friend | New candidate at temperature 1.2 |

The source then generates one further Haiku response at `0.9`, after recording
the last exchange. That response is neither returned nor stored. AML preserves
the complete generation sequence, including those final consumed draws. The
next foreground voice carries their continuation.

All generation finishes before feedback begins. Each recorded text, including
the opening foreground haiku, then updates fragments and the generator chain,
in dialog order. The original exchange quality is always `0.5`; exchange
dissonance, dialog average quality, and note are unset. AML retains speaker
alternation and the fixed quality as schema rules rather than repeated columns.

The source computes a friend seed from its highest-weight fragments, with
SQLite randomness breaking ties. It also extracts the previous friend's last
three words as a response seed. Neither seed reaches `generate_candidates`.
The `BOOTSTRAP_TEXT` constant and `fragment_buffer_size=15` setting are likewise
unused. The paired source receipt supplies different remembered fragments and
measures identical generated texts and voice draws. This baseline keeps that
path; giving the friend its own conditioned voice remains a separate family
experiment.

## What comes back

Each returned haiku first multiplies every old fragment weight above `0.1` by
`0.95`. It then appends nonempty, Python-whitespace-stripped literal-LF lines
at weight one, retaining duplicates. Values below `0.1` disappear. A value
exactly equal to `0.1` remains unchanged. A fragment introduced at one is removed
on its 45th subsequent update. Three-line updates settle at 135 fragments;
there is no fifteen-fragment truncation.

Initialization takes lines from the first ten supplied bootstrap haikus without
decay. Reinitialization preserves the existing memory. The actual chat launcher
starts with an empty fragment list.

The source's feedback changes **generator counts, vocabulary, and its last ten
trigrams**. It does not change the living word cloud, observer rows, or learners.
Consequently a retokenized dream can introduce speaking pieces that never
arrived from the user. `dream_vocab` records their first dream turn; later user
admission into the living cloud keeps that origin. The syllable cache grows with
the speaking vocabulary before publication.

## Two measured repairs

The Python dialog increments `last_run_turn` by one, starting from zero.
A dream at foreground turn ten therefore stores one, and another dream is
eligible at turn eleven. AML stores the actual foreground turn. The paired
receipt preserves the source result and checks that the corrected event waits
through turn nineteen, then becomes eligible at twenty. This implements the
ten-turn cooldown recorded in the original migration plan.

The original SentencePiece model returns literal `\n` unknown-surface tokens
when it re-tokenizes a three-line haiku. The source inserts those tokens into
the generator. The receipt contains the original pieces from the recorded
`sweet sour doubt stable` haiku, including both line breaks. AML replaces only
the literal LF separators with spaces before dream feedback tokenization. The
recorded dialogue and fragments retain their original line breaks. Re-tokenized
pieces still pass the existing English generator word validation; an unsupported
whitespace-bearing piece rejects the staged event.

## One saved dream life

The versioned owner adds seven flat leaves:

| Leaf | Type | Meaning |
|---|---|---|
| `dream_config` | Numeric vector | Interval, probability, exchanges |
| `dream_fragments` | String list | Ordered, duplicate-permitted fragments |
| `dream_weights` | Numeric vector | Fragment weights in the same order |
| `dream_history` | String list | Recorded texts, in dialog order |
| `dream_dialogs` | Numeric vector | Rows of actual turn, text offset, exchange count |
| `dream_last_turn` | Number | Last completed dream turn, initially zero |
| `dream_vocab` | Numeric map | Speaking word → first dream turn |

Empty numeric collections use `[0]`. History offsets are contiguous and dialog
turns strictly increase. The fragment bound is 10,000; history supports at most
10,000 dialogs and 65,536 exchange texts. Capacity rejects growth explicitly;
it never evicts a completed conversation. The full state validator checks dream
turns against their activation boundary and vocabulary against both its cloud
and dream origins. [Continuity](STATE.md) owns profile migration and restart.

`haiku_dream_init(state, bootstrap)` creates these leaves.
`haiku_dream_check(state)` validates them and returns the dialog count.
`haiku_dream_should(state, event)` checks the current bridge event and consumes
the probability draw only when eligible.
`haiku_dream_fragments(state, text)` performs one fragment update.
`haiku_dream_run(state, spoken, model)` records and feeds back one complete
dialog, using `state.turn`, its tokenizer mode, generator, and voice stream.

The session supplies a detached candidate. Reflection, rings, dreaming,
learning, draws, and persistence publish together. A failed dream or failed
save leaves the preceding complete life intact. Python chat also calls its
dream functions synchronously; asynchronous scheduling has its own lifecycle
step.

## Receipts

`bash tests/run_dream.sh` checks interpreted and compiled AML:

- Twelve direct Python trigger cases, including strict climate boundaries,
  skipped draws, and probability equality.
- Bootstrap limits, idempotence, Unicode line stripping, duplicate fragments,
  threshold weights, 46 successive decays, and a 10,000-LF empty update.
- Five complete Python dialogs of 1, 2, 4, 5, and 16 exchanges: every recorded
  text, generator call temperature, random cursor, fragment weight, transition
  count, vocabulary entry, and recent-memory snapshot.
- Native shared-stream replay, unchanged cloud/model stream, unused seeds,
  differing dialog lengths, and retained first-dream provenance.
- The actual-turn cooldown and original/repaired SentencePiece separator path.
- Twenty invalid states and host inspection of complete checkpoint bytes after
  history-capacity, fragment-capacity, and staged generation failures.

Texts, order, counts, and scripted draw positions match exactly. Fragment
float32 weights compare against Python's double precision within `0.000002`.
Runtime receipts need AML and NoTorch. Rebuild the source oracle in the pinned
Python development environment with:

```sh
PYTHONDONTWRITEBYTECODE=1 PYTHONPATH=../reference-deps \
  python tests/reference/dream_oracle.py --python-haiku ../harmonix/haiku --check
```
