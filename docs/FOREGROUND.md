# Foreground — the cloud can answer

2026-10-08. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
Python reference: `harmonix/haiku` at
`abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`, especially `chat.py`.

Run `../ariannamethod.ai/runner/aml-notorch haiku.aml` and write to Haiku.
The [entrypoint](../haiku.aml) owns its state explicitly; the six
[foreground helpers](../src/foreground.aml) carry input gates, cache growth,
quality, and the bridge handoff. All organism behavior is AML.

## One encounter, in order

1. Read a line, strip Python's 29 Unicode whitespace characters, and recognize
   `quit`, `exit`, or `q` after lowercase. Blank input and EOF leave the organism
   unchanged. Validate token/context bounds before accepting a turn.
2. Tokenize; construct rolling triples. Morph cloud weights/frequencies; update
   observer trigrams; update generator transitions and the recent-ten snapshot.
3. Read that **updated** recent snapshot. Compute dissonance, pulse, and the two
   temperatures. The first complete input therefore observes its own triples.
4. Extend the syllable cache, generate five candidates, and select through the
   original three RAE refinements. Publish the selected three lines.
5. Compute quality with the original three open intervals. Record the bridge
   handoff, then train MathBrain. Optional shared experience trains RAE next.

Generated candidates and the selected response leave cloud, transition counts,
and recent memory at the state established by the incoming message. Reflection,
rings, and dream feedback have their own later stages.

The quality signal begins at 0.5. It adds 0.2 for `0.3 < dissonance < 0.7`,
0.15 for `0.4 < entropy < 0.8`, and 0.15 for `0.3 < novelty < 0.7`.
Pulse arousal retains values above one. One- and two-token inputs grow the word
cloud while leaving generator vocabulary unchanged; their empty trigram context
gives dissonance 0.5, temperature 0.9, and quality 0.7.

## Owners and configuration

Each fresh execution owns cloud columns, observer rows/counts/resonance,
generator rows/counts/vocabulary/recent, syllable cache, two learner arrays and
their statistics, sampling state, and the last bridge event. The initial cloud
has the original **587 seed entries / 576 unique words**. Seed transitions belong
to the generator; observer and recent memories begin empty.

Configuration is at the top of `haiku.aml`:

| Setting | Default | Behavior |
|---|---|---|
| `tokenizer_mode` | `"regex"` | Historical root-launcher word boundaries; `"sentencepiece"` loads the bundled model relative to the source file. |
| `voice_rng` | `rng_new(575)` | One owned native sampling stream. |
| `model_rng` | `rng_new(57)` | Initializes MathBrain and then RAE; each model retains its own 57 parameters. |
| `train_rae` | `0` | Python chat's MathBrain-only training; `1` gives both learners the selected text, context, and quality once per exchange. |

MathBrain learns the quality signal; the default selector reads RAE weights.
The shared-experience option connects that signal to RAE's subsequent choices.
Initialization and native draws follow NoTorch's owned streams. Python numerical
parity uses explicit weights and draw tapes.

The CLI uses **logical-turn clocks**: seeds have clock zero, accepted turn `n`
uses clock `n`. Clock values occupy `last_used`; boost/decay still occurs once
per active exchange. Native persistence will supply elapsed-time clocks.

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
durable logs remain in the migration queue. The response is visible before
learning; a learning failure ends execution after that output. Storage,
MetaHaiku, Overthinkg, dreams, and background scheduling retain their separate
planned work. A fresh process starts fresh state.

## Receipts

[foreground.input](../examples/foreground.input) and
[foreground.txt](../examples/foreground.txt) record an unedited native three-turn
conversation using the defaults above. Replay it with:

```sh
../ariannamethod.ai/runner/aml-notorch haiku.aml < examples/foreground.input
make test-foreground
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
