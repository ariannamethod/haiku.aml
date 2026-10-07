# RAE — the voice chooses, then learns

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
Implemented in [rae.aml](../src/rae.aml), using the shared
[AML learner and MathBrain](MATHBRAIN.md) over canonical NoTorch operations.
The behavioral source is [`harmonix/haiku` at
`abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`](https://github.com/ariannamethod/harmonix/tree/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku),
specifically `rae.py`, `rae_recursive.py`, and the foreground calls in `chat.py`.

## Five features, three second thoughts

The recursive selector uses its own 57 parameters: five inputs, eight hidden
neurons, one output, with tanh in both layers. Its features preserve the source:

| Slot | Value |
|---|---|
| 0 | Constant `0.5` |
| 1 | Unique lowercased words / all words; zero for empty text |
| 2 | Candidate-word/context-word intersection / unique context words; zero for empty context |
| 3 | Exact duplicate of slot 1 |
| 4 | `1 / (1 + variance)` of three line word counts; zero unless there are exactly three LF-separated lines |

Candidate words use Unicode `lower()`; context words retain their exact case
and punctuation. Splitting the original text before lowering each word preserves
Python's features and accommodates expanding mappings such as `İ → i̇`.
The oracle checks **4,640** sigma and combining-mark cases across Python's
complete 29-character whitespace set. Context arrives as complete flat triples,
with the same representation as the memory organ.

For multiple candidates, pass zero contains their raw network outputs `b[i]`.
Each refinement computes:

```text
normalized[i] = (previous[i] - minimum) / (maximum - minimum)
next[i] = 0.7 * b[i] + 0.3 * normalized[i]
```

Equal scores use `normalized[i] = 0.5`. The original code appends a sixth
feedback feature and slices it away before the network call; this baseline
preserves that behavior. The five network inputs stay constant across passes.
Ties select the first candidate. Confidence is the largest final score; zero
weights give `0.15` after a refinement. A singleton returns confidence `1` and
bypasses model, context, and depth evaluation.

The inherited empty-candidate response is preserved verbatim:

```text
cloud is empty
silence speaks louder than words
wait for resonance
```

Its counts under the original English estimator are **4–8–6**. The generator
produces attainable 5–7–5 candidates before selection.

## Explicit recursive and rule paths

| AML function | Result |
|---|---|
| `haiku_rae_features(text, user)` | Five numerical features |
| `haiku_rae_trace(params, candidates, user, steps)` | Flat pass-zero and refinement rows, each of candidate-count width |
| `haiku_rae_select(params, candidates, user, steps)` | `[candidate_index, confidence]` |
| `haiku_rae_reason(params, candidates, user, steps)` | Selected text, or the inherited empty response |
| `haiku_rae_rule(candidates, params, rows, counts, user, use_scorer)` | The original rule-chain choice |
| `haiku_rae_state()` | A fresh map with `observations=0`, `learning_rate=0.01` |
| `haiku_rae_observe(params, state, selected, quality, user)` | Loss, after publishing one complete parameter update |

The rule path first retains exactly-three-line texts. If that leaves no
candidates, it uses the complete input list. With `use_scorer=1`, MathBrain
scores every retained candidate and a stable descending ranking takes three.
With `use_scorer=0`, their existing order supplies the first three. The final
choice maximizes **case-sensitive** unique-word ratio; ties keep the first.
Empty and singleton rule inputs use the same immediate returns as recursive
mode, before evaluating the scorer or context.

Python enters rule mode when recursive selection raises an exception. AML has
explicit recursive and rule entry points; callers select the path. Invalid
inputs produce checked runtime errors.

## An observation changes the next choice

RAE trains only the selected text. Its quality target is finite and remains
unchanged, including targets outside `[0,1]`. Empty text also trains. The update
uses the raw tanh prediction, squared error, fresh gradients, plain SGD at the
state's learning rate, then clamps every parameter to `[-5,5]`.

The complete step and observation counter are checked before publishing.
`params` is the caller-owned mutable array; the existing state map receives one
observation increment after the parameter copy. Domain, numerical-overflow,
and counter errors preserve both owners. The state has no loss history or
moving average, matching RAE's source state. Original JSON saving every ten
observations is part of the upcoming durable-storage integration.

The original foreground loop trains **MathBrain only**. RAE's observation entry
is available explicitly. [selection.aml](../examples/selection.aml) demonstrates
that entry after generating and selecting real candidates; its call is visible
in the example and remains separate from the future chat orchestration.

With generator seed `575`, temperature `0.9`, five candidates, and the frozen
reference parameters `p[i] = ((17*i mod 31)-15)/32`, it selects:

```text
will one rest warm
they say subtle his could a
maintain protect lose
```

Context triples are `(cloud, words, dance)` and `(rest, warm, dream)`. The
original Python selector gives confidence **0.7526770854**. One RAE observation
at quality `0.85` has loss **0.04133839186**; the next confidence is
**0.7565252080**, with the same selected candidate. The executable AML example
runs native generation, selection, observation, and selection again. The
integration fixture also compares the five actual texts, complete score traces,
and all 57 updated parameters against the source receipts.

Fresh models can use `haiku_learner_new(5, 8, rng_new(seed))`. Pass a distinct
owned random map to each voice or learner when their sequences should advance
independently; the shared learner preserves zero biases and fan-in scaling.

## Bounds and receipts

The AML boundary allows 10,000 source codepoints per candidate, 100 candidates,
and integer refinement depths `0..10`. Flat context lists contain at most
10,000 entries and complete triples, so the largest complete list has 9,999.
Trace inspection requires at least two candidates; `select` handles singleton
confidence and rejects empty input. `reason` supplies the empty response.
An observation count of `16,777,215` can advance to `16,777,216`; another update
is rejected. Learning rates are finite and nonnegative.

`tests/run_rae.sh` checks **1,657 reference fields** through the interpreter and
compiled scalar AML. The corpus includes 15 feature vectors, ten complete score
traces, ten full gradient/parameter updates, twelve rule selections, and the
native generation-to-learning sequence. Numerical tolerance is `0.000005`;
strings, chosen indexes, observation counts, and order compare exactly.
Accepted limits and **23 rejected operations** have dedicated fixtures. A C
inspection host checks the actual parameter bytes, learning map, context, and
candidate list after each rejected operation.

The development-only reference runner calls the original Python methods and
captures their actual score lists, gradients, updates, and choices. Runtime and
normal tests use AML and NoTorch. Reproduce the source receipts or regenerate
their AML assertions with the original Python dependencies available:

```sh
PYTHONDONTWRITEBYTECODE=1 PYTHONPATH=../reference-deps \
  python tests/reference/rae_oracle.py ../harmonix
PYTHONDONTWRITEBYTECODE=1 PYTHONPATH=../reference-deps \
  python tests/reference/rae_oracle.py ../harmonix --emit-aml
tests/run_rae.sh
../ariannamethod.ai/runner/aml-notorch examples/selection.aml
```

The [RAE research](RAE_RESEARCH.md) keeps learned feedback, replay, and Hebbian
traces as the next measured experiments. This module supplies their preserved
baseline.
