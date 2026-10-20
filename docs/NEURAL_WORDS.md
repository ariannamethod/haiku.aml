# A word finds its own concept

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

The version-8 iceberg lets a sparse external encounter teach a private word
association through both context and frozen PEN7 representations. Haiku hears
the human; the friend hears Haiku's actual delivered dream lines. Each keeps
its own evidence. The four shared concept identities remain water, tree, light
and music, with the [personal centres](PERSONAL_CONCEPTS.md) already grown
from that presence's earlier encounters.

## What teaches this association

The original [word learner](LEARNED_WORDS.md) needs two distinct reviewed
spellings in a local window. The new layer considers a word with exactly one
reviewed spelling in its window of three tokens on either side. That spelling
must name one of the four supported concepts. Every reviewed spelling counts
when deciding eligibility, including those outside these four concepts.

The target is an unbound English alphabetic word of 3–64 letters outside the
existing stoplist. It must occur exactly once in an encounter of at most 64
tokens. The complete local query retains its original token order. Repeated
target occurrences are ineligible; a second conflicting occurrence cannot be
hidden by selecting the first. Reviewed and manual bindings keep their existing
authority.

Each available frozen body forwards that exact query through NoTorch. Its
hidden vector is compared with that endpoint's four centres, using the owner's
**previous-turn** teacher counts. Equal-centre mode uses zero counts. With both
bodies present, scalar cosine scores mix at a fixed alpha of .5; a single
available body uses its own score. This coefficient is independent of the
Janus parameter mixture used for lexical continuation.

A context supports its reviewed teacher when that concept is the unique top
score, the score is positive, and its lead is at least .01. An available
projection that disagrees remains an ambiguous context. It counts against the
association's confidence and can suspend an earlier association.

Each word keeps six distinct contexts. Promotion requires three supporting
contexts, at least 75% support among all retained contexts, and a lead of two
over the next concept. Its strength is `.25 * support / retained_contexts`.
The existing external learner wins whenever it has a resolved association;
the neural layer is its fallback. Neither learned layer becomes a reviewed
teacher for the other.

Canonical sorted sets of local words identify duplicate contexts. Reordering,
punctuation, case and repeated local spellings cannot mint another vote. One
word earns at most one context per turn, including multiple friend deliveries.
Replay, self speech, model proposals and returned human echoes teach no new
association. Activation begins at the owner's existing event and turn boundary.

## Where the association goes

The effective association participates in ordinary contact features, word
ranking and later symbol paths. A path is frozen before its encounter teaches
anything; a newly confirmed binding can enter a later path. Dream symbols retain
their existing recurrence and source requirements.

The foreground also reads the **prior** Haiku contact. A word whose effective
association comes from this layer receives pressure
`.15 * binding_strength * parliament_field[concept]`, bounded by ±.1875.
For each of the same five generated candidates, pressure is averaged over its
unique words and added to the final recursive RAE score. Repeating a word cannot
amplify it. Stable ties keep the earlier candidate. No extra generation or draw
is introduced; the confirming encounter cannot change its own foreground reply.

The publisher independently regenerates this foreground experience from the
exact predecessor. It then reconstructs every contact and neural observation
from the accepted human input and actual dream deliveries before checking
admission and the complete successor. A substituted score, proof or reply
cannot become the accepted completion.

## Bounded work and continuation

At most four eligible targets are considered per external encounter, in token
order. Each owner has 32 candidate slots and six proofs per candidate. A full
candidate table retains its words and refuses additional candidates; a full
word history replaces its oldest proof. Queries use at most 194 model tokens;
encoding requests an extra token to detect overflow and refuses overlong queries.

A cold bank costs 21 teacher forwards per available endpoint. Each evaluated
local query costs one further forward per endpoint. Duplicate contexts require
no query forward. The source publisher reproduces this work. A resumed life
audits populated available banks and retained query scores against their frozen
providers. No available body means no new neural observation, bank mutation or
random draw; already learned history remains available to the organism.

Contact v6 adds four leaves to v5's 61 fields:

| Leaf | Shape | Contents |
| --- | --- | --- |
| `neural_clock` | 6 values | Mode, activation event/turn, latest event/turn, revision |
| `neural_words` | 64 texts | Language and word for 32 candidates |
| `neural_rows` | 32 × 6 × 242 | Ancestry, verdict, provider mask, fixed mixture, concept snapshot, counts, endpoint scores and exact query tokens |
| `neural_text` | 32 × 6 × 8 texts | Original encounter, canonical context, exact query, ancestry owners and provider/tokenizer identities |

Iceberg v8 has 276 fields, with the same 58-field speaking core and two private
65-field contacts. New complete lives enable `neural_bindings=1`. Set it to zero
for a new inert v8 control. Existing v7 lives retain their profile until
`upgrade_neural=1`; activation is detached and idempotent. Active neural learning
requires active concept centres. New symbols-off or concepts-off lives select
the inert mode automatically.

Direct callers use `haiku_iceberg_enable_neural(life, enabled, tokenizer)` and
`haiku_iceberg_prepare(life, core, source, tokenizer)`. The normal turn API calls
the latter itself. `haiku_iceberg_concepts_audit(life, a, b)` audits both concept
banks and neural proofs for v8, including after provider availability changes.
Older saved profiles preserve their prior behavior and fields.

The [recorded conversation and controlled reply comparison](../examples/neural-words.txt)
retain the actual outputs. Their executable sources are
[`iceberg_neural_models.aml`](../tests/iceberg_neural_models.aml) and
[`neural_foreground.aml`](../tests/neural_foreground.aml).
