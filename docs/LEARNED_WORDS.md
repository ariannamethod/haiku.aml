# A word acquires a remembered neighbour

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

The iceberg's version-3 profile gives each presence a private learned layer
between English words and the 88 shared contact anchors. Repeated external
encounters can give a previously unbound word an attraction; conflicting
encounters can suspend it. The source text and the encounters that taught it
remain in that presence's checkpoint.

## What teaches a word

Haiku learns from an accepted native human encounter. The friend learns from
a Haiku dream line actually delivered to it. The latter must retain Haiku as
both root and immediate parent, source 7 at both levels, and equal delivery,
parent and origin event IDs. An undelivered line teaches the friend nothing.

The normal learner accepts only an exact, already-published contact receipt.
The contact feature vector is computed before this learning step, so an
encounter cannot retrospectively acquire the association it just taught.
Later contact and lexical ranking can use the new association.

The first vocabulary is English: normalized alphabetic words of 3–64 letters,
outside the reviewed bindings and the learner's explicit stoplist. Each
candidate examines its local window of at most three tokens on either side.
At least two distinct reviewed spellings for the same anchor must occur in
that window. Only the fixed seed table supplies teachers. Private manual
bindings and learned words cannot teach another word.

A unique strongest teacher gives the context a label. Competing teachers can
leave ambiguous evidence, which counts against confidence. One word receives
at most one evidence row per accepted turn. Each qualifying local window keeps
its sorted set of normalized words; the encounter keeps the sorted set of those
windows. Equal signatures do not vote again while retained. Case, punctuation,
word order and repeated occurrences of the same words do not create another
context when those local sets stay the same. The original text is retained.
Source/event clocks prevent a replay or renumbered same-turn delivery from
adding another observation. Activation captures the current boundary so old
contact receipts cannot teach a newly enabled learner.

Each word retains six distinct contexts. Its 88 counts are recomputed from
those rows. A leading anchor becomes available when:

- At least three retained contexts support it.
- It holds at least 75% of the informative contexts, including ambiguous ones.
- Its lead over the next anchor is at least two contexts.

The window continues to change with new encounters. A formerly available
association becomes unresolved when these conditions cease to hold. The
capacity is 32 candidates per presence; a full learner refuses new candidates
and retains its existing words and evidence.

## What the attraction changes

Reviewed seed/manual bindings keep their existing authority and strength 1.
An available learned association has strength `0.5 * confidence`, at most 0.5.
It adds that strength to its anchor's contact count before the existing
`n / (1 + n)` activation. A sole fully supported learned word therefore gives
activation `1/3`, while one reviewed word gives `1/2`.

Lexical ranking adds `.15 * strength * field[anchor]` to the word's base
score. With the existing field bound of ±5, learned pressure is at most ±.375.
This changes future contact and pressure where that word is present. Human
input may already have admitted the word to the cloud; learning its anchor
is a separate event from admitting it to the vocabulary.

The shared table contains 88 anchor identities. The reviewed English subset
currently gives **16** anchors the multiple spellings needed to teach by this
rule. The remaining anchors continue to work through their named seeds and
explicit reviewed bindings. French and Russian seed aliases retain their
current lookup behavior; automatic learning here is English.

## Five owned leaves

`haiku_contact_enable_learning(contact)` upgrades a version-1 contact to a
detached version-2 contact with 47 fields. Already-enabled v2/v3 contacts retain
their saved profile, including v3's symbols. A contact kept at version 1 retains
its 42-field behavior. These five leaves carry the word-learning organ:

| Leaf | Shape | Contents |
|---|---|---|
| `learn_clock` | 5 values | Activation event/turn, latest event/turn, revision |
| `learn_words` | 64 texts | Language and word for each of 32 candidates |
| `learn_counts` | 32 × 88 | Counts reproduced from retained contexts |
| `learn_rows` | 32 × 6 × 12 | Event ancestry, turn, window location and outcome |
| `learn_text` | 32 × 6 × 4 texts | Original text, normalized window, root and parent owners |

Rows for different candidates from the same encounter must agree on its exact
text, turn and learning revision. Each revision names one external event.
The checker reproduces every local label and count from the retained text.

`haiku_contact_learning_lookup(contact, language, word)` returns the resolved
anchor and strength, honoring reviewed bindings first. The ordinary glyph and
rank paths use it. `haiku_contact_learn` requires the published encounter;
`haiku_contact_learning_step` is the deterministic replay kernel used by the
iceberg publisher. The replay kernel changes only these five leaves.

The enclosing version-3 life has **237 fields** and its detached worker result
has **499**, fitting the original 512-field record bound. Publication begins
with the exact predecessor and independently reconstructs accepted human and
dream deliveries. It replays learning from those events and compares every
learned leaf. A different but internally consistent set of counts or contexts
cannot substitute for that transition. Contact source clocks and retained
receipt order/text/ancestry are also checked. Native contact features are
reconstructed before each learning step, preserving what the confirming
encounter actually knew. This adds no model forward or random draw.

## Activation and controls

New iceberg lives enable word learning with `word_learning = 1` after Janus
activation. Existing profiles retain their behavior until `upgrade_words = 1`
is selected. `haiku_iceberg_enable_words(life, tokenizer)` requires an existing Janus profile;
an old v1 life can explicitly enable both upgrades together. Version 3 retains
the original 55-field speaking voice and both immutable source identities.

Zero available model bodies still allow external word learning. Disabling
`deeper_life` leaves the contact owners unchanged. Internal cloud/self/model
events and repeated deliveries cannot teach an association.

## The model experiment behind this boundary

The [PEN7 word-anchor probe](research/iceberg/WORD_ANCHOR_PROBE.md) ran both
actual GGUF bodies at alpha .5 with four reference anchors. Nearest hidden
representation recovered **2/16** held-out meanings and **2/8** existing
aliases. An unrelated word had a stronger margin than either correct held-out
match; changing a phrase's known prefix sent the same unknown spelling to four
different anchors. All 50 measured samples and the executable probe are kept
with the report.

Those results choose the present learning boundary: repeated external context
supplies the evidence. Existing bounded model features still participate in
contact, while automatic word bindings use the reviewed local teachers above.
Attributed model evidence remains in [TODO](../TODO.md). The later version-4
[dream-symbol organ](DREAM_SYMBOLS.md) uses frozen paths before each word-learning
step and adds its own recurrence rule; it does not change this learner's policy.

Focused gates: `make test-contact-learning test-iceberg-learning`. The standalone
fixtures cover evidence, conflict, pressure, ownership and restart; the complete
life gate covers publication, old profiles, real model bodies and worker parity.
Measured results and source revisions are recorded in [HAIKULOG](../HAIKULOG.md).
