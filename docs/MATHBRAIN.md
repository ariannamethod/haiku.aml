# MathBrain: experience reaches the weights

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
Behavioral source: `harmonix/haiku/haiku.py` at
`abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`.

`src/mathbrain.aml` carries the original scoring features, clipped public score,
online observation, and learning statistics. `src/learner.aml` supplies the
shared two-layer learner also used by [RAE](RAE.md). The organism composes
forward and reverse passes in AML; its numerical kernels execute in NoTorch.

## The shared learner

The original networks have five inputs, eight hidden neurons, one output, and
tanh at both layers. Their **57 parameters** have this exact order:

| Indices | Contents |
|---|---|
| `6*j` through `6*j+4`, for `j=0..7` | Hidden neuron `j`'s five weights |
| `6*j+5` | Hidden neuron `j`'s bias |
| `48..55` | Output neuron's eight weights |
| `56` | Output bias |

The generic helper accepts input and hidden dimensions from 1 through 64, with
one output. Weights and biases retain the same neuron-major order. Forward
unpacks linear weights/biases and applies two tanh activations. Reverse applies
MSE, output tanh, output linear, hidden tanh, then hidden linear derivatives,
and packs every weight/bias gradient back into that order.

NoTorch provides `nt_linear`, `nt_linear_vjp`, `nt_tanh`, `nt_tanh_vjp`,
`nt_mse_grad`, and `nt_sgd`. Each operation returns fresh storage. The shared
helper uses these stateless operations, so different models have no shared tape
or optimizer state. Haiku applies its original `[-5,5]` clamp after SGD.

| AML function | Result |
|---|---|
| `haiku_learner_new(inputs, hidden, rng)` | Fresh parameters from the supplied owned RNG |
| `haiku_learner_check(params, inputs, hidden)` | Validated parameter count |
| `haiku_learner_forward(params, x, inputs, hidden)` | Raw tanh scalar |
| `haiku_learner_gradient(params, x, inputs, hidden, target)` | `[prediction, loss, all gradients]` |
| `haiku_learner_step(params, x, inputs, hidden, target, lr)` | `[prediction, loss, all updated parameters]` |
| `haiku_learner_publish(params, staged)` | Copies the validated update into its owner; returns loss |
| `haiku_learner_part(values, start, count)` | Independent numeric copy of the requested part |

Gradient and step calls leave their input parameters unchanged. Targets must be
finite; the generic learner accepts targets outside `[0,1]`. Learning rate is
finite and nonnegative. Parameters must be finite, including an explicit model
supplied before its first update. Numerical overflow fails before publication.

Fresh initialization consumes `inputs*hidden + hidden` owned standard-normal
samples, scales hidden weights by `sqrt(2/inputs)` and output weights by
`sqrt(2/hidden)`, and leaves biases zero. NoTorch supplies `rng_normal` over its
owned PCG stream. Python used `random.gauss` over its process-wide stream, so
initialization replay belongs to the new owned stream. Cross-language numerical
parity supplies explicit parameters and features to both implementations.

## MathBrain's five features

Word splitting preserves exact case and punctuation. Consecutive triples span
line breaks, as in the Python scorer. For at least three words:

1. Sum each stored generator-trigram count, using `0.1` for an unknown triple;
   divide by the number of triples, divide by ten, and cap at one.
2. Divide the unique word count by the total word count.
3. Divide the number of unique context words present in the candidate by the
   context's unique word count; empty context contributes zero.
4. Divide the original English estimator's total syllables by seventeen and
   cap at one.
5. Repeat the unique-word ratio, preserving the original feature schema.

`haiku_mathbrain_features(text, rows, counts, user_triples)` returns these five
values. It reads the existing ordered generator rows/counts and complete flat
context triples. Text is bounded to 10,000 codepoints; context is bounded to
10,000 flat tokens, therefore at most 3,333 complete triples. Existing generator
memory validation keeps its 10,000-row and exact count bounds.

`haiku_mathbrain_score(params, text, rows, counts, user_triples)` returns zero
for fewer than three words. Otherwise it clips the raw network output to
`[0,1]`. Learning uses the **raw output**: a raw prediction of `-0.4621171573`
displays as zero but gives loss `1.5929397187` against target `0.8`.

## One observation, one owner

`haiku_mathbrain_state()` creates the four-field numeric map:
`lr=0.01`, `observations=0`, `last_loss=0`, `running_loss=0`.

`haiku_mathbrain_observe(params, state, text, quality, rows, counts, user_triples)`:

1. Validates the statistics. Nonfinite quality or fewer than three words returns
   the existing last loss, leaving all owners unchanged.
2. Clamps finite quality to `[0,1]`, extracts features, and stages the complete
   MSE/SGD/clamped parameter update.
3. Stages the next count, last loss, and exponential moving average:
   `running_loss += 0.05 * (loss - running_loss)`.
4. Validates the full update and publishes its parameters and statistics.

The counter is an exact float32 integer through `2^24`; an observation at a full
counter fails before learning. Existing statistics keys and their key strings
are prepared before publication. The final parameter-copy loop and replacement
of those existing map entries require no new containers or strings.

Function parameters share their owner arrays/maps. Use `haiku_learner_part`
and `map_clone` for independent foreground/learner snapshots. The fixture keeps
two models and random streams isolated and checks that a completed update
leaves an earlier snapshot unchanged. Worker scheduling and durable save/restart
records remain the lifecycle and persistence milestones.

## Measured source repairs

Direct calls to the pinned Python implementation exposed four concrete paths:

- A NaN first weight made `score_haiku("rain finds stone")` return `1.0`.
  AML rejects the nonfinite model before scoring or learning.
- Python's nonfinite-loss recovery returned zero, reset the model/count/EMA,
  and retained a previous `last_loss=0.375`. AML rejects the invalid update
  while preserving the current model and statistics.
- Saving `lr=0.075` and loading retained the constructor's `0.01`.
- A malformed serialized parameter at index three left an earlier parameter
  changed to `2.125` before loading aborted.

The last two findings belong to the durable-state work: its record must restore
the learning rate and validate the entire model before replacing live state.
Python's `scoring_history` is initialized and never used; no history is created
by these APIs.

## Verification

`bash tests/run_mathbrain.sh` executes **3,717 reference values** in the AML
interpreter and compiled scalar executable. This includes ten feature cases,
24 complete online updates, five boundary cases, and three additional network
shapes. Every online step checks all 57 gradients, all 57 new parameters,
prediction, loss, and all learning statistics. Absolute tolerance is `2e-6`.

The direct Python oracle independently checks its micrograd gradients against
an analytic two-layer chain rule: maximum discrepancy across the 24-step
trajectory is zero. Native fixtures check owned Gaussian initialization,
staging, separate models, copied state, skips, and the exact counter boundary.
Twenty-two invalid-operation cases run through both execution paths; a C test
host verifies that parameters, statistics, generator memory, context, and RNG
remain byte-for-byte unchanged after rejection.

The reference-only reproduction uses the pinned Python source and its original
dependencies. Haiku's generation, learning, and product checks run without Python:

```sh
PYTHONDONTWRITEBYTECODE=1 python tests/reference/mathbrain_oracle.py \
  --python-haiku ../harmonix/haiku --check
bash tests/run_mathbrain.sh
```

`tests/reference/mathbrain_inputs.json` fixes the source checksum, explicit
weights, input texts, contexts, transitions, targets, and boundary models.
