# The voice inside the voice

After speaking and learning, Haiku answers inwardly: one new haiku at
temperature **0.7**, remembered together with the exchange that brought it.
`src/metahaiku.aml` carries the Python inner voice into AML.

The reference is `harmonix/haiku/metahaiku.py` and its caller in `chat.py`,
revision `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`.

## Eight little shards

A nonempty spoken haiku always consumes one diversity draw. Its snippet enters
the bootstrap buffer when dissonance exceeds 0.6, arousal exceeds 0.6, or the
draw is below 0.3. High dissonance and arousal still consume that draw. Empty
spoken text skips the admission draw and proceeds to generate an inner haiku.

The snippet takes the first ten Python-whitespace-delimited words, joins them
with ordinary spaces, then slices the first 100 Unicode codepoints. The slice
can end inside a word or after a combining mark. Nonempty whitespace can leave
an empty snippet. Duplicates remain. The buffer retains its last eight entries.

Reflection and the following rings share `voice_rng` and `draws`. Native draws
use NoTorch's owned stream; scripted draws fix the same event order for source
comparisons. The reflection's generator reads the current Markov vocabulary and
transitions. Its one candidate becomes the inner voice directly.

The source computes `reflection_seed` from the buffer and spoken text, then
calls `generate_candidates(n=1, temp=0.7)` without passing that seed.
`update_cloud_bias()` extracts words and ends in `pass`. A paired source probe
starts with different bootstrap memories and measures identical generated text,
identical draws, and unchanged vocabulary and transition weights. AML preserves
that path. Connecting remembered shards to generation and feeding reflection
back into the cloud remain explicit developments, with before/after receipts.

## A remembered inner exchange

The inner-life state adds three flat leaves to the native checkpoint:

| Leaf | Type | Contents |
|---|---|---|
| `meta_bootstrap` | String list | Last eight admitted snippets |
| `meta_history` | String list | Repeated `user`, `spoken`, `inner` triples |
| `meta_metrics` | Numeric vector | Repeated `turn`, `dissonance`, `novelty`, `arousal`, `entropy` rows |

An empty history has an empty bootstrap and metrics `[0]`. The history preserves
every completed inner exchange up to 10,000 rows; reaching that bound rejects
the next reflection before it consumes a draw. This follows the existing AML
organism's 10,000-record domain. No older reflection is silently discarded.

Python history retains the caller's interaction dictionary by reference. AML
freezes the actual chat context in the row: later caller changes leave its
words and climate intact. The versioned state records the inner-life starting
turn and validates the history's exact relation to subsequent completed turns.
The [state contract](STATE.md) owns migration and restart behavior.

`haiku_meta_reflect(state, user, spoken, observation, turn)` receives a detached
turn, records one reflection, and returns its inner haiku. `observation` contains
the four Harmonix values in dissonance/novelty/arousal/entropy order.
`haiku_meta_check(state)` reads the three leaves and returns their history count.
The session publishes reflection, rings, learning, and their random state
together after its final validation and save.

## Receipts

`bash tests/run_metahaiku.sh` checks the interpreter and compiled scalar runner:

- 23 source bootstrap cases: thresholds, draw order, empty text, whitespace,
  Unicode slicing, ten-word admission, duplicates, and eight-entry eviction.
- Four complete Python reflection traces: every generated line, admission
  buffer, stored context, and consumed draw. A 1:3 weighted transition at draw
  0.1875 distinguishes the required temperature 0.7 from temperature 1.
- Native shared-stream replay, frozen contexts, and unchanged generator/cloud
  owners; different bootstrap memories retain the measured generation path.
- Eleven rejected states covering malformed history, metrics, shape, climate,
  snippet bounds, inner lines, empty-history consistency, and full capacity.

The committed receipts run with AML and NoTorch. To regenerate them using the
original Python dependencies in a development environment:

```sh
PYTHONDONTWRITEBYTECODE=1 PYTHONPATH=../reference-deps \
  python tests/reference/metahaiku_oracle.py --python-haiku ../harmonix/haiku --check
```

The source oracle calls the pinned Python methods and records the two unused
feedback paths alongside its generated texts and draw traces.
