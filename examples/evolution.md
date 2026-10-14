# Twenty-four encounters, one little cloud

[The fixture](evolution.aml) records one fresh conversation through the current
owned-worker session path. [The transcript](evolution.txt) preserves its complete
standard output: all human inputs, spoken replies, internal replies, and both
four-part dream conversations. Inputs are hand-written; generated lines are
left exactly as emitted.

Run from the repository root with the current AML + NoTorch build:

```sh
../ariannamethod.ai/runner/aml-notorch examples/evolution.aml
```

The fixture uses the entrypoint's ordinary settings: a version-3 state, native
regex tokenization, seeds `575` and `57`, `train_rae = 0`, reflection, three rings,
Phase4, dreams, and the exchange journal. It starts fresh and writes no state
file. Each owned worker is joined and its result validated before the example
reads the completed voices. The ten-turn dream cooldown, probability `0.25`,
and four-part dialogue remain unchanged.

The transcript was recorded on 2026-10-14; a second fresh run matched it byte
for byte, with no stderr output from either run. Dreams opened at turns **10**
and **23**. Their first HAiKU entry repeats the spoken haiku that opens the dream;
the following friend/self/friend entries are the generated continuation.
Every recorded dream fragment then returns to memory, following `src/dream.aml`.

At turn 10 the friend says `grasp acknowledge path and`. That exact line appears
in the spoken response to `what is love` at turn 23. The same question opened
the conversation with a different reply. These are excerpts from a single
continuing state.

This session exercises the stable AML organism. The GGUF bodies, experimental
glyph parliament, and proposed injection paths have separate integration work;
they did not generate this transcript.
