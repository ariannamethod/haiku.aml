# A concept acquires a personal history

The version-6 iceberg profile lets Haiku and the dream-friend carry different
centres for the same four concepts. A meeting with rain can leave a different
trace from a meeting with the sea. Both presences keep the same frozen bodies
and reviewed word identities; each supplies its own history.

This stage follows merged [#22](https://github.com/ariannamethod/haiku.aml/pull/22),
`fab2a4aad1dbfdf05f7487582b7f6acbe3352271`, and the measured
[concept-basis comparison](research/iceberg/CONCEPT_BASIS.md). That comparison
used separate A, B and physical parameter-mixture spaces. The new live return
uses two endpoint spaces and mixes their scalar concept scores.

## The three saved modes

| Mode | Representation contribution |
| --- | --- |
| 0 | No concept representation contribution; existing lexical encounters and word scores continue |
| 1 | Equal weight for all reviewed words within their concept |
| 2 | Each reviewed word has weight `1 + own_encounter_count` |

Modes 1 and 2 use the existing external symbol-history ring. They require that
organ to be enabled. Mode 0 keeps the new concept state empty and permits a
life with symbols disabled. A profile's selected mode remains part of its
saved state.

The teacher inventory is fixed:

| Concept | Glyph | Words, in accumulation order |
| --- | ---: | --- |
| water | 0 | water, rain, river, sea, ocean, lake |
| tree | 4 | tree, forest, wood, leaf |
| light | 6 | light, sun, sunrise, dawn, bright |
| music | 82 | music, song, melody, guitar, piano, drum |

## What counts as a meeting

Haiku counts accepted native human encounters. The friend counts the actual
delivery of Haiku's dream line, with its original speaker and source ancestry.
The existing 128-entry external ring retains those texts and their events.
An English teacher spelling contributes once per encounter, even if it appears
several times. Distinct teacher words in that encounter each contribute once.
Counts range from zero to 128; a retained meeting's contribution expires when
that meeting leaves the ring.

The count uses these exact reviewed spellings. Learned words and manual
bindings do not add teachers to the inventory. Model proposals, private
speech, replay and cache reconstruction add no external meeting.

Activation derives its initial snapshot from the owner's already retained
history. Throughout a turn, every model contact uses the predecessor's frozen
counts. After the complete owned tail, accepted encounters produce the next
snapshot. Its version advances once when a new eligible external event changes
the history boundary. The same snapshot can therefore span several turns in
which the friend hears no new line.

## Frozen vectors, changing centres

The shared tokenizer encodes each exact teacher word. Each body independently
forwards those token IDs and returns its final 448-value hidden vector. The
bank stores its normalized vector, exact token
IDs, model identity, tokenizer identity and teacher-manifest identity. A and B
share their token inventory but retain separate vector banks.

For body `s`, concept `g` and the selected snapshot:

```text
teacher_s(word) = unit(hidden_s(exact isolated word))
centre_s(g) = unit(sum_in_declared_order(weight(word) * teacher_s(word)))
```

Changing personal counts or alpha changes no bank vector. A live query uses
the exact `context_tokens` of the completed lexical receipt. It is forwarded
through each available endpoint and compared with that endpoint's centres by
raw cosine. For two available bodies:

```text
raw[g] = alpha * cosine_A[g] + (1 - alpha) * cosine_B[g]
representation[g] = clamp(raw[g], 0, 1)
```

The coefficient is captured by the lexical receipt. A-only uses effective
alpha 1; B-only uses 0. The lexical word scores still come from their existing
physical parameter-mixture pass. The concept calculation makes its own
endpoint queries from that pass's exact input tokens.

The existing representation gain is .25. The contribution enters contact
features, the private parliament's field, word ranking and vocabulary
admission. The model event retains its owner, source ancestry, identities and
concept-snapshot boundary. Learned word promotion continues through its
existing external-encounter policy. Janus retains its own later-witness path.

## Ownership and continuation

Contact version 5 adds six leaves: a five-value snapshot clock, 21 counts,
two banks of 9,408 values, a 693-value teacher-token table and the bank identity
list. The clock records mode, snapshot version, last eligible event, its turn
and its symbol-history revision. The enclosing iceberg is version 6.
The complete life has 265 flat fields; its detached completion has 555.

During a detached tail, structural validation permits the frozen snapshot while
new encounters update the ring. At completed iceberg publication and life
validation after loading, counts and the history boundary must match the
retained external witnesses exactly.
The publisher reconstructs the version-6 contact transitions from the exact
predecessor and accepted events, including model features and the parliament's
field. It checks word admission at the same point as the actual tail, before
new dream symbols and exchanges are consolidated. Pre-admission vocabulary is
reconstructed from the predecessor, frozen human trigrams and accepted dream
texts; the full selection ledger and final vocabulary must agree. Voice
learning and generation keep their existing accepted-core boundary.

Saved banks receive a model-aware audit after loading and when a provider
returns. Each available populated endpoint is rebuilt from the frozen body and
compared with its saved unit vectors and token IDs. The audit changes no state
and saves no trusted flag. Missing bodies leave the existing history and bank
intact; they contribute no representation.

| Additional model work, per owner | Both bodies | One body |
| --- | ---: | ---: |
| First bank construction and query | 44 forwards | 22 forwards |
| Query with existing banks | 2 forwards | 1 forward |
| Resume audit of populated banks | 42 forwards | 21 forwards |

The publisher independently verifies the live calculation. Its verification
work is additional to the worker's calls. Replays and the no-body path perform
zero concept forwards. All work uses AML and canonical NoTorch.

## Launch controls

New lives select `personal_concepts = 2` in `iceberg.aml`. When dream symbols
are disabled, the new life uses mode 0. Existing version-1 through version-5
checkpoints retain their profile. An already version-5 life can explicitly
activate concepts through `upgrade_concepts = 1`; its prior history survives.
Upgrading a symbols-off checkpoint requires `personal_concepts = 0`; changing
the launcher's `dream_symbols` value leaves that saved symbol mode intact.

Direct API callers activate a version-5 life with
`haiku_iceberg_enable_concepts(life, mode, tokenizer)`. They call
`haiku_iceberg_concepts_audit(life, body_a, body_b)` after checkpoint loading
and after provider availability changes. It returns the actual number of
audit forwards. The launcher performs it after the resumed life's bodies have
been loaded.

The first fixed-input live comparison, exact continuation checks and refusal
gates are recorded in [HAIKULOG](../HAIKULOG.md#2026-10-15--the-same-concept-two-personal-histories)
and the [complete validation receipt](../tests/research/iceberg/personal_concepts_validation.json).
