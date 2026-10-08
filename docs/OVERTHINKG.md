# Three rings after the inner voice

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
Python source: `harmonix/haiku/overthinkg.py` and `chat.py` at
`abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`.

`src/overthinkg.aml` runs echo, drift, and meta in that order: **5, 7, and 3
candidate trigrams**. MetaHaiku speaks first. The rings then continue the same
owned voice stream. A completed inner event retains every candidate and its
two kinds of coherence for the future interface.

## The exact path

The Python engine reads the word cloud and observer trigrams once. All three
rings use those snapshots and the current user's triples. When that context
is nonempty, each candidate first draws a probability: below `0.6`, choose a
recent triple, choose one of its three positions, then choose a cloud word to
replace it. Otherwise sample three different cloud words. Empty context skips
the probability draw and goes directly to sampling.

Sampling preserves CPython 3.12's two paths for `random.sample(words, 3)`:
through 21 words it draws from a shrinking pool and fills the selected slot
with the last remaining item; above 21 it redraws previously selected indices.
Scripted draws replay the original Python calls. Native events use NoTorch's
owned stream, with the resulting state saved beside the organism.

The source's two SQL queries have no `ORDER BY`. The reference receipt on
SQLite **3.53.1** records both query plans: the word query scans its covering
unique index; the triple query scans its covering three-column unique index.
Consequently word sampling sees ascending SQLite BINARY order. AML makes that
order explicit with `list_sorted(map_keys(weights))`, retaining the cloud's
separate insertion order. Triple order leaves these summed coherence values
unchanged. The source's optional `expand(None)` uses the last ten triples of
its sorted query; the AML chat API receives its context explicitly, matching
`chat.py`'s `expand(recent_trigrams=user_trigrams)` call.

For triples `t` and observer context `E`, admission coherence is:

```text
sum(len(set(t) & set(e)) for e in E) / max(1, 3 * len(E))
```

Admission requires **strictly greater than 0.4**. A ring's reported coherence
uses all candidate/context pairs, divides by their maximum possible overlap,
multiplies by ten, and caps at one. With no recent context it is `0.5`.
Reported ring coherence and per-triple admission are separate values.
The source's temperature and semantic configuration entries are unused by
generation; AML preserves the actual draw path.

## What the rings change

Each accepted new observer triple enters with count one and its admission
coherence. Existing triples keep their counts and resonance: this is the
source's `INSERT OR IGNORE`, including repeated admissions in later rings.
All fifteen admission checks use the observer snapshot taken before echo.

The frozen-snapshot receipt starts with `a b c` at count seven. Echo admits
`a b d` five times at `2/3`; drift proposes `a d d` seven times at `1/3` and
rejects them; meta repeats the accepted echo. The final new triple still has
count one. Recomputing against the growing observer would give `a d d` a
score of `0.5` and change the result; the fixture fixes the source order.

Accepted words use the cloud's existing insert-or-ignore convention: initial
weight is admission coherence, frequency zero, current logical-turn clock,
and origin `overthinking:echo`, `overthinking:drift`, or `overthinking:meta`.
Chat's recent words already belong to its cloud, so a normal ring event leaves
those word weights, frequencies, origins, and clocks intact. A direct API
fixture supplies an external context word and checks its actual insertion.

Observer memory and the generator remain distinct. Rings retain cloud-token
identity, including an observer-only token such as `a b`. They leave generator
rows, counts, vocabulary, recent memory, syllable cache, both learners, and the
model initialization stream unchanged. This also preserves short SentencePiece
input whose cloud token contains whitespace before it forms a generator triple.

## API and continuity

`haiku_overthinkg(state, recent)` operates on the session's detached candidate
and returns a record containing `trigrams`, `coherence`, and `admission`.
The session stores those as `ring_trigrams`, `ring_coherence`, and
`ring_admission` in its version-two state. Their completed shapes are 45 strings,
three numbers, and fifteen numbers. An initial or skipped event has an empty
list and two `[0]` sentinels. Fewer than three cloud words skip every draw.

`haiku_rings_check(state)` checks those shapes, vector dimensions, bounded
finite scores, cloud identities, and presence of admitted observer triples.
The full state validator checks their relation to the completed inner history.
The session publishes the complete staged event after saving it; its failure
path preserves the prior live state and file.

## Verification

On 2026-10-08, **18 pinned Python cases** produced 51 rings, 255 candidate
triples, and 932 ordered random events. The committed fixtures compare exact
text, draw cursors, ordered cloud/observer rows, counts, origins, and clocks;
coherence values use tolerance `0.000002`. They cover both sample algorithms,
collision retries, Unicode SQL order, empty contexts, too few words, repeated
tokens, strict-threshold equality, unsaturated aggregate coherence, snapshot
timing, duplicate admissions, all three inserted-word origin labels, and
observer-only whitespace tokens.

The nineteenth fixture checks native stream replay, actual observer growth,
unchanged cloud values, and independent generator/learner ownership.
All nineteen pass in interpreted AML and compiled scalar executables.

```sh
bash tests/run_overthinkg.sh
PYTHONDONTWRITEBYTECODE=1 PYTHONPATH=../reference-deps \
  python tests/reference/rings_oracle.py --python-haiku ../harmonix/haiku --check
```

The Python command reproduces development receipts. Haiku runs on AML and
NoTorch. [Continuity](STATE.md) describes the versioned owner and restart checks.
