# Exchange journal — the words keep their moment

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
The [AML journal](../src/journal.aml) replaces Python's NumPy exchange shards
and SQLite metric rows. Every completed full-life turn retains its speaking
context, scoring features and final metrics in the same checkpoint as its organs.

## What Python records

Reference: `harmonix/haiku` at
`abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`.
`Harmonix.create_shard` writes a NumPy object containing a millisecond timestamp,
input, output, dissonance and user/system trigrams. Its SQLite `shards` row keeps
the filename, dissonance and two generation temperatures. `record_metrics`
stores perplexity, entropy, resonance and the observer's word count.

Chat invokes both methods after dreams. Its metric values are the fixed
perplexity `0.5`, the interaction's pulse entropy, `1 - dissonance`, and
`COUNT(words)`. Dream feedback updates the generator and dream-fragment memory;
generator-only words do not increase this observer count.

The native checkpoint replaces the separate filename and millisecond identity
with the existing logical turn. The journal records `clock_kind = "turn"`.
Every archived row belongs to the same committed turn as its learners, dreams
and random streams. [STATE](STATE.md) describes the save/restore boundary.

## Two persistent leaves

Full-life state adds these owners alongside MetaHaiku and Phase4:

| Leaf | Shape | Per-turn contents |
|---|---|---|
| `journal_text` | String list, three entries per turn | Source `foreground`, canonical user-trigram key, canonical system-trigram key. |
| `journal_values` | Numeric vector, thirteen entries per turn | Haiku temperature, observer temperature, final observer word count, five MathBrain features, five RAE features. |

The input and spoken text already live in `meta_history`. Climate and quality
already live in the corresponding Phase4 event. The journal refers to those
actual rows: its zero-based index is `turn - phase4_started_turn - 1`, while
the reflection index is `turn - inner_started_turn - 1`. An explicit upgrade
creates empty journal leaves at its current turn. It creates no earlier
exchange records.

Empty journals use an empty list and numeric `[0]`. History retains at most
10,000 rows: 30,000 strings and 130,000 numeric values. User contexts contain at
most 9,999 flat tokens in complete triples; system contexts contain at most
30. Original order, duplicates, case and UTF-8 bytes survive.

Contexts use AML's `list_key` framing: a decimal item count, then each token's
UTF-8 byte length and exact bytes. `haiku_journal_unpack` decodes those lengths
against codepoint-indexed strings and verifies the canonical representation.
No delimiter can collide with a token. One context occupies one string leaf,
so retaining many exchanges does not consume the string-list bound per token.
AML's ordinary text and checkpoint-size bounds still apply.

## Freeze, finish, publish

`haiku_journal_capture` runs before learning and before dream feedback. It
freezes the chosen response's two feature vectors and the contexts that
actually accompanied generation and scoring. MathBrain's first feature reads
Markov counts; computing it after a dream would measure another chain.

`haiku_journal_append` runs after the complete dream path. It checks the
capture against the current turn's reflection and Phase4 event, adds the
observer's current `map_len(weights)`, and appends the row to the detached
candidate. It does not recompute earlier features or read the generator's
later recent-context list. The normal validate/save/swap transaction publishes
the journal and the complete turn together.

| API | Result |
|---|---|
| `haiku_journal_capture(state, source, spoken, user, system, temperatures, turn)` | Transient packet with frozen contexts and features. |
| `haiku_journal_append(state, captured)` | Appends after Phase4, reflection and dream work; returns the new row count. |
| `haiku_journal_get(state, index)` | Returns an independent, flat versioned experience record. |
| `haiku_journal_check(state)` | Validates types, shapes, contexts, numeric domains, history alignment and final word count; returns the row count. |
| `haiku_journal_unpack(encoded)` | Returns the exact ordered context list. |

The getter's record has format `haiku-experience`, version `1`, source,
input/output, user/system trigrams, dissonance, temperatures, quality, both
feature arrays and a metrics map. It exposes the logical turn/clock and the
serial profile's publication identity: `base_revision = turn - 1`,
`state_revision = turn`, `publication_order = turn`. Mutating a returned
context, feature array or metrics map leaves the journal unchanged.

This completed experience is the handoff for later background learning.
[ASYNC](ASYNC.md) records the worker facilities and next publication protocol.
The current profile finishes the whole turn before publication.

## A measured context repair

Python chat keeps the list returned by `get_recent_trigrams()` in its
`system_trigrams` variable. `update_chain` subsequently does:

```python
self.recent_trigrams.extend(trigrams)
self.recent_trigrams = self.recent_trigrams[-10:]
```

The first dream extends the original list before replacing the generator's
reference. Consequently, the later NumPy shard contains the earlier system
context plus the **first** dream's trigrams. Later dreams change the new list.
The archived list can exceed ten triples and differs from the context used
for the earlier observation.

AML keeps its established copied-context boundary: the journal retains the
actual pre-generation context. The reference receipt records both Python's
first-dream mixture and the frozen context, making this repair reproducible.
Dream feedback still reaches the generator in the original event order.

## Verification

```sh
bash tests/run_journal.sh
PYTHONDONTWRITEBYTECODE=1 python tests/reference/journal_oracle.py \
  --python-haiku ../harmonix/haiku --check
```

The oracle command uses the reference Python environment; ordinary runtime
checks consume committed AML fixtures and need no Python packages.

Three original NumPy shards and SQLite metric rows anchor the receipt. It
includes 30 original feature values, Unicode and empty contexts, distinct
first/later dream feedback, an upgrade after older reflection history, and
generator-only words. The AML checks preserve the frozen features after
Markov counts change and retain the observer's distinct word count.

Interpreted and compiled checks also cover exact Unicode framing, the maximum
9,999-token context, a 10,000-codepoint token, independent getter results and
byte-identical checkpoint round trips. Malformed prefixes, UTF-8 byte cuts,
trailing data, numeric/shape violations and misaligned histories fail before
being accepted as a completed journal.
