# RAE research — give the owl a useful second thought

2026-10-07. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
Companion to the [migration map](MIGRATION.md). Research and experiment designs;
the AML organism is being implemented separately.

## What the Python organism actually does

Inspected source: [`harmonix` at `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`](https://github.com/ariannamethod/harmonix/tree/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku).

| Organ | Observed behavior |
|---|---|
| `haiku.py:Neuron`, `MLP`, `HaikuGenerator.observe` | MathBrain has 5→8→1 layers, tanh in both layers, 57 parameters, online MSE/SGD, and parameter clamping to ±5. |
| `rae_recursive.py:extract_features` | Inputs are `[0.5, unique_ratio, context_word_overlap, unique_ratio, line_length_coherence]`. The first feature is constant; the second and fourth are equal. |
| `rae_recursive.py:select_recursive` | The appended sixth feedback feature is removed by `[:5]`. Each network call receives the original five features. A separate score recurrence mixes 70% network output with 30% normalized previous score. |
| `rae.py:reason`, `chat.py:main` | Default selection uses the recursive network immediately. The exchange loop trains MathBrain but never calls `rae.observe()`. The RAE observation method exists and has isolated unit tests. |
| `chat.py:main` | The quality target is `0.5 + 0.2*moderate_dissonance + 0.15*moderate_entropy + 0.15*moderate_novelty`. All inputs are computed before candidate generation; every candidate in that exchange has the same target under this formula. |
| `metahaiku.py:reflect` | An eight-snippet buffer builds `reflection_seed`; generation does not consume it. `update_cloud_bias()` ends in `pass`. |
| `dream_haiku.py:run_dream_dialog` | Friend and response seeds are computed but unused. Four exchanges are recorded, alternating temperatures 1.2/0.9. The saved cooldown counter increments by one instead of recording the foreground turn. |
| `harmonix.py:morph_cloud`, `chat.py:main` | Active word weights multiply by 1.1; dormant weights by 0.99. Recorded dream haikus feed fragments and the generator's trigram chain. These pathways are the existing substrate for consolidation. |

### Measured recurrence receipt

Let `b[i]` be a candidate's unchanged MLP output and `N` min–max normalization.
The current selector computes:

```text
s[0] = b
s[t+1] = 0.7*b + 0.3*N(s[t])
```

With finite scores and a nonzero range, `s[1]` is a positive affine transform of
`b`. Therefore `N(s[1]) = N(b)`: every later pass repeats `s[1]` in exact
arithmetic, and the winning candidate is unchanged. Equal-score candidates stay
tied and selection takes the first one.

Ran the original `RecursiveRAESelector`, without training or saved weights,
under CPython 3.12 with `random.seed(0)` through `random.seed(31)`. Each seed used
the five candidates below and three contexts: `None`, one user trigram
`('words', 'dance', 'cloud')`, and one `('rain', 'glass', 'night')`.

```python
candidates = [
    'words dance in cloud\nresonance finds its own path\nconstraint births form',
    'cloud cloud cloud\ncloud cloud cloud cloud\ncloud cloud cloud',
    'rain under glass\nthe room remembers footsteps\nnight folds its wings',
    'one two three\nfour five six\nseven eight nine',
    'words\nwords dance in the quiet cloud\nresonance',
]
```

For each seed/context pair, the same network ran with `refinement_steps` equal
to 0, 1, 3, and 5. Across **96 cases: zero winner changes**. The maximum change
in returned score between 1, 3, and 5 refinement passes was **0.0**. These are
selector probe inputs; generated dialogue retains its own form checks.

## Three primary sources worth bringing home

1. **Jolicoeur-Martineau, TRM (2025), Figure 3 and §4.1–4.3.**
   [Paper, pinned v1](https://arxiv.org/html/2510.04871v1),
   [authors' repository](https://github.com/SamsungSAILMontreal/TinyRecursiveModels).
   TRM feeds the current answer and latent state into repeated evaluations of a
   shared network. Training differentiates through a complete recursion block;
   deep supervision carries detached states between blocks. The useful design
   for Haiku is learned feedback with intermediate supervision. Its tiny
   selector can express that idea directly through AML and NoTorch.
2. **Chaudhry et al., On Tiny Episodic Memories (2019), Algorithm 1 and §4–5.**
   [Paper, pinned v4](https://arxiv.org/pdf/1902.10486v4),
   [authors' implementation](https://github.com/facebookresearch/agem).
   One update mixes new observations with sampled stored observations. The
   paper compares reservoir sampling and balanced buffers; balanced retention
   helps at the smallest memory budgets in its classification experiments.
   This suggests a compact replay organ for Haiku's two online learners.
3. **Miconi, Stanley, Clune, Differentiable Plasticity (2018), equations 1–3.**
   [Paper](https://proceedings.mlr.press/v80/miconi18a/miconi18a.pdf),
   [authors' implementation](https://github.com/uber-research/differentiable-plasticity).
   Each effective connection combines a base weight and a changing Hebbian
   trace. The paper uses both decaying pre/post activity traces and Oja updates;
   plasticity coefficients can themselves be learned. Haiku's small network
   permits an inexpensive output-layer trace experiment with fixed coefficients.

## Experiments, in implementation order

These are proposed Haiku adaptations. First preserve the Python forward values,
gradients, weight updates, and foreground event order as fixtures. Then connect
both learners to one recorded exchange containing the exact scoring features,
quality, source, and state revision. Keep the original quality formula as the
climate target. Candidate preference experiments use separately recorded
candidate-specific labels and retain their label source.

### 1. Put feedback inside RAE's existing 57 parameters

Keep the 5→8→1 topology. Reuse the fourth input, currently a duplicate of the
second, for a candidate's previous score:

```text
p[0] = unique_ratio
s[t] = MLP(0.5, unique_ratio, resonance, p[t], coherence)
p[t+1] = (s[t] + 1) / 2
loss = mean_t((s[t] - target)^2)
```

The first evaluation exactly reproduces the current input vector. Later
evaluations receive feedback in the same `[0,1]` input range. Backpropagate
through the complete small unroll with shared parameters. Version the feature
schema and persist the chosen depth with learner state.

Compare the preserved selector against this recurrence at depths **1, 3, 5**,
using the same initial parameters, candidate sets, event order, and update
budget. Report per-pass scores, winner changes, target loss, candidate
preference agreement, form validity, distinct trigrams, and elapsed CPU time.
Use an initial synthetic feature/target fixture to verify recurrent gradients,
then a fixed corpus of original Haiku exchanges with labels held out by
conversation. Repeated versions of one exchange stay in the same split.

Acceptance: the recurrent input changes later network evaluations; gradients
match finite differences; additional passes improve held-out preference
agreement under a declared compute budget. Keep the one-pass arm as a useful
baseline. The original 57 MathBrain parameters remain unchanged by this design.

### 2. Make remembered exchanges feed tomorrow's learners

Run **no replay / 32 records / 128 records**. Each record stores frozen features
for both learners, target and target source, turn, language, and state revision.
Start with one current observation plus one replay observation per SGD update;
average their losses before stepping. Give the no-replay control the same
number of example evaluations. Start with uniform reservoir sampling, then
compare a fixed allocation across low/medium/high dissonance and languages.

Evaluate a chronological **A → B → A** stream of distinct vocabulary/context
regimes. Hold out probe exchanges from each regime and check them after every
phase. Measure old-regime loss/preference agreement, new-regime adaptation,
retained vocabulary, repeated-output rate, and storage bytes. Save and reload
halfway through; require identical next replay selections and learner updates
from the saved RNG/state. This is also a concrete test of asynchronous ownership.

The worker performs complete updates and publishes one revision. Speaking,
reflection, and dreams read a stable revision for their whole operation.

### 3. Add eight fast Hebbian traces, consolidate through replay

After experiment 2, place one trace on each of RAE's eight hidden→output
connections. Keep the 57 learned base parameters; `alpha` and `eta` begin as
fixed configuration values, and the eight traces are persistent state:

```text
s = tanh(sum_j((w[j] + alpha*H[j]) * hidden[j]) + bias)
H[j] <- (1-eta)*H[j] + eta*hidden[j]*s
```

Initialize traces to zero. Score all five candidates against one trace snapshot,
then update traces once from the selected exchange. This makes selection
independent of candidate evaluation order. Sweep `alpha = 0, 0.05, 0.1` and
`eta = 0.01, 0.05`; with tanh activities and `eta` in `[0,1]`, each trace stays
in `[-1,1]`. Continue slow base-weight learning from recorded experience.

Compare replay alone against replay plus traces on the same A→B→A stream.
Measure adaptation after 1/5/20 exchanges, recovery of earlier motifs, trace
norms, candidate preference agreement, and repeated-output rate. For the dream
extension, replay recorded exchanges during dream time and log newly generated
dream fragments with their origin. Test real-exchange trace updates first, then
add dream updates as a separate arm with its own update count.

## What the stack needs

| Layer | Required capability |
|---|---|
| AML | Records/collections, float-array events, explicit worker ownership, versioned state, saved PRNG state |
| NoTorch | Linear+bias, tanh with backward, scalar arithmetic, shared-parameter gradients across an unroll, SGD, parameter clamps |
| Haiku | Feature-schema version, learner/replay records, original and recurrent selectors, explicit reflection/dream feedback events |

All three experiments fit the two-dependency design. The organism keeps its
word cloud, Markov generation, three-line voice, inner voice, rings, and dreams.
The research strengthens how experience reaches the next choice.
