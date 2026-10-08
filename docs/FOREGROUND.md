# Foreground — the cloud can answer

2026-10-08. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
Python reference: `harmonix/haiku` at
`abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`, especially `chat.py`.

Run `../ariannamethod.ai/runner/aml-notorch haiku.aml` and write to Haiku.
The [entrypoint](../haiku.aml) owns one versioned state record; the six
[foreground helpers](../src/foreground.aml) carry input gates, cache growth,
quality, and the bridge handoff. [session.aml](../src/session.aml) stages a
complete exchange and [state.aml](../src/state.aml) checks every persistent owner.
All organism behavior is AML.

## One encounter, in order

1. Read a line, strip Python's 29 Unicode whitespace characters, and recognize
   `quit`, `exit`, or `q` after lowercase. Blank input and EOF leave the organism
   unchanged. Validate token/context bounds before accepting a turn.
2. Tokenize; construct rolling triples. Morph cloud weights/frequencies; update
   observer trigrams; update generator transitions and the recent-ten snapshot.
3. Read that **updated** recent snapshot. Compute dissonance, pulse, and the two
   temperatures. The first complete input therefore observes its own triples.
4. Extend the syllable cache, generate five candidates, and select through the
   original three RAE refinements. Keep the selected three lines for publication.
5. Compute quality with the original three open intervals. Record the bridge
   handoff, then train MathBrain. Optional shared experience trains RAE next.
6. Version 2 reflects at temperature 0.7, then runs echo/drift/meta rings against
   one observer snapshot. Store the internal haiku, its context, and ring traces.
   Version 1 retains its foreground-only event order.
7. Validate the complete candidate state. The CLI saves its checkpoint, swaps
   it into the live owner without allocating, then prints the selected three
   lines. This moves display
   after learning and storage; the numerical and lexical event order is unchanged.

Generated candidates and the selected response leave cloud, transition counts,
and recent memory at the state established by the incoming message. Reflection
consumes shared voice draws. Rings can add observer trigrams while leaving
generator transitions and recent memory unchanged. Dream feedback follows in
a later stage.

The quality signal begins at 0.5. It adds 0.2 for `0.3 < dissonance < 0.7`,
0.15 for `0.4 < entropy < 0.8`, and 0.15 for `0.3 < novelty < 0.7`.
Pulse arousal retains values above one. One- and two-token inputs grow the word
cloud while leaving generator vocabulary unchanged; their empty trigram context
gives dissonance 0.5, temperature 0.9, and quality 0.7.

## Owners and configuration

Each session owns cloud columns, observer rows/counts/resonance,
generator rows/counts/vocabulary/recent, syllable cache, two learner arrays and
their statistics, sampling state, and the last bridge event. The initial cloud
has the original **587 seed entries / 576 unique words**. Seed transitions belong
to the generator; observer and recent memories begin empty.

Configuration is at the top of `haiku.aml`:

| Setting | Default | Behavior |
|---|---|---|
| `state_path` | `"haiku.state"` | Source-relative checkpoint; resume when present and save each completed turn. An empty string starts an unsaved session. |
| `tokenizer_mode` | `"regex"` | Historical root-launcher word boundaries; `"sentencepiece"` loads the verified optional model relative to the source file. Applies to new state. |
| Voice seed | `575` | Starts an owned native sampling stream; resumption restores its exact state. |
| Model seed | `57` | Initializes MathBrain and then RAE; resumption restores both 57-parameter arrays and the initialization stream. |
| `train_rae` | `0` | Python chat's MathBrain-only training; `1` gives both learners the selected text, context, and quality once per exchange. |
| `inner_life` | `1` | New lives use the version-2 reflection/rings profile; `0` creates a foreground-only version-1 life. |
| `upgrade_inner` | `0` | Set to `1` to grow an existing version-1 life from its next exchange. Already-enabled lives retain their history. |

Saved tokenizer mode, `train_rae`, and profile are authoritative on resumption.
`upgrade_inner` explicitly changes the profile at a completed-turn boundary. Use a
new path to begin another configuration. Install the optional tokenizer with
`bash scripts/setup-tokenizer.sh`; default regex sessions need no model asset.

MathBrain learns the quality signal; the default selector reads RAE weights.
The shared-experience option connects that signal to RAE's subsequent choices.
Initialization and native draws follow NoTorch's owned streams. Python numerical
parity uses explicit weights and draw tapes.

The CLI uses **logical-turn clocks**: seeds have clock zero, accepted turn `n`
uses clock `n`. Clock values occupy `last_used`; boost/decay still occurs once
per active exchange. Checkpoints preserve this clock exactly across restarts.

Input and lowered text are bounded at 10,000 codepoints; learner context is
bounded at 10,000 flat triple components. SentencePiece's original unknown
pieces can contain internal whitespace. When such a piece would enter the
generator, input preflight rejects it before advancing the turn or changing
memory. Exact token identity is preserved. [TOKENIZER.md](TOKENIZER.md)
records both tokenizer modes.

The bridge handoff contains turn, clock, dissonance, novelty, arousal, entropy,
quality before/after, and boredom/overwhelm/stuck flags. The four climate values
are shared by its before/after metric snapshots; quality changes from 0.5 to
the observed value. Phase4 state-ID formatting, transition aggregation, and
durable event logs remain in the migration queue, along with dreams and
background scheduling. The complete foreground and enabled inner snapshot
now persist; [STATE.md](STATE.md) records their validation
and publication boundaries. One running process owns each checkpoint path.

## Receipts

[foreground.input](../examples/foreground.input) and
[foreground.txt](../examples/foreground.txt) record an unedited native three-turn
conversation starting with fresh version-1 state. `make test-foreground` uses an
isolated, unsaved copy of the real entrypoint with `inner_life = 0`.
`make test-inner-cli` checks the new default, profile resumption, explicit upgrade,
and the recorded [inner dialogue](../examples/inner.txt) through both execution paths:

```sh
make test-foreground
make test-state
make test-inner-cli
```

The Python oracle calls the original tokenizer, cloud, generator, RAE, bridge,
and MathBrain methods. Twelve explicitly ordered seed words, frozen learner
parameters, and dyadic draws make every choice reproducible. Seven successive
turns cover short input, self-observation, repeated triples, recent-ten eviction,
new vocabulary, and arousal 9. An eighth turn records the existing empty-cloud
repair: punctuation-only input reaches the original SQL binding failure;
replaying that update as a no-op yields a neutral response and one learning step.

Four two-turn AML fixtures preserve the sequence through explicit intermediate
snapshots. Every candidate, selected index, draw cursor, token/triple list,
ordered memory, cloud column, learner parameter, loss statistic, and bridge
handoff is checked. Text/order/counts compare exactly; float tolerance is
0.000002. Both interpreted and compiled scalar paths run these fixtures.

The real stdin entrypoint is exercised separately through blank/quit/EOF,
three accepted exchanges, shared RAE experience, punctuation-only input,
context overflow, and whitespace-bearing SentencePiece pieces. A C inspection
host verifies the actual owners after each run. Model paths and the compiled
application are exercised from an unrelated working directory.

Development-only reference reproduction uses the Python organism's dependencies:

```sh
PYTHONPATH=../reference-deps python3 tests/reference/foreground_oracle.py --python-haiku ../harmonix/haiku --check
python3 tests/reference/foreground_fixtures.py --check
```

Normal product and fixture execution uses AML, NoTorch, and system libraries.
