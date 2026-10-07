# More birds in the family

2026-10-07. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

Oleg brought two more relatives into the workshop: **Subjectivity** and
**Brodsky**. Both have organs worth studying. Python Haiku remains this port's
behavioral and artistic reference; the family contributes explicit experiments
after those organs are working in AML.

## Source bodies

| Repository | Inspected revision | Source read |
|---|---|---|
| [Python Haiku](https://github.com/ariannamethod/harmonix/tree/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku) | `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39` | `harmonix.py`, generator/MathBrain in `haiku.py`, `rae_recursive.py`, `metahaiku.py`, dream and chat orchestration |
| [Subjectivity](https://github.com/ariannamethod/subjectivity/tree/221157935d84d5037768f34c4661b11ca6865f91) | `221157935d84d5037768f34c4661b11ca6865f91` | Complete `subjectivity.c`, manifesto, `CLAUDE.md`, README, `PROJECT_LOG.md` |
| [Brodsky](https://github.com/ariannamethod/brodsky/tree/501d617d5da8f9f00ff194ce6765d6749647bda8) | `501d617d5da8f9f00ff194ce6765d6749647bda8` | Vocabulary loading, perception, selection, generation, learning, memory, persistence and presentation in `brodsky.c`; complete `kk.h`; README and Makefile |

This is a source audit. Experiment rows below describe future measurements.
Runtime claims from the older READMEs and project logs are kept with their own
history; the mechanisms below come from the pinned code.

## Subjectivity — a body chooses its shape

All function names in this section are in
[`subjectivity.c`](https://github.com/ariannamethod/subjectivity/blob/221157935d84d5037768f34c4661b11ca6865f91/subjectivity.c).

| Organ | Current mechanism | Connection to Python Haiku |
|---|---|---|
| Contact and heat — `inhale` | Unknown-token fraction sets dissonance; known-word affinities excite six chambers. Temperature is `(0.3 + 1.2*d) * (1 + 0.4*flow)`, clamped to `[0.3,1.8]` | Retains the direction of alienness → heat. Python uses word-set Jaccard, pulse adjustments and exact-trigram discount before its direct `0.3 + 1.2*d` mapping |
| Choice — `score_words`, `parliament`, `best_pos` | Affinity cosine, mutable word mass and repetition shape scores. Self/shadow/ghost votes choose the hidden draft; visible lines use part-of-speech slots with temperature-scaled jitter | Python generates five Markov candidates and selects through RAE; its MathBrain learns whole-candidate taste |
| Inner hearing — `settle` | Generates a draft, re-ingests it, and blends 15% of the resulting chamber change. Restores the external input's exact temperature and dissonance | Gives a concrete feedback path for MetaHaiku's reflection. In the Python source, `reflection_seed` is assembled and `update_cloud_bias` ends in `pass` |
| Form — `resonance`, `gen_line`, `render` | Dominant-chamber strength and separation from the mean select fragment / three-line haiku / couplet at thresholds `0.34` and `0.66`; accumulated shame contracts that value | Supplies a later form-policy experiment. Python's ordinary candidate loop always generates three lines with 5–7–5 targets |
| Consolidation — `inhale`, `morph` | A new word receives the body's affinity snapshot at contact. Used content words gain ×1.1 once per turn; others decay ×0.99; weights clamp to `[0.05,3]` | Adds state-at-contact to word memory. Python Harmonix stores usage/frequency and boosts each occurrence of an active input word; its generator also updates the order-2 chain |
| Memory — `save_scar`, `load_scar` | Versioned live-word/shame snapshot, temporary file + rename, restore by word text onto the fresh seed | Word-keyed restoration is useful when language packs evolve; Haiku's own snapshot must also carry its learners, chain, rings, dreams and RNG streams |

The visible grammar chooses among five short frames. It nudges the body 18%
toward each content word it emits. `gen_line` pads toward a syllable target;
its initial frame has no upper-budget admission check. The rhyme helper compares
an orthographic tail beginning at the last vowel after dropping a final silent-e
pattern. Haiku's future form tests therefore need the actual emitted words,
their syllable counts, and language-pack rhyme data.

Subjectivity also carries a shared sediment field in `carrier_bend`: read the
standing field, bend the body with coefficient `0.30`, then write
`0.90 * field + 0.35 * own_scar`. The current carrier is **one seven-float field**;
the two-slot design in `PROJECT_LOG.md` records an earlier stage. This suggests
an experiment for the owl and friend once both have distinct state and explicit
exchange events.

Its learning lives in mutable word weights, contact affinities, shame and the
shared field. Haiku AML keeps the original MathBrain/RAE learning path alongside
any separately tested addition from this body.

## Brodsky — memory, rhythm, and an unfinished rhyme

The main source is
[`brodsky.c`](https://github.com/ariannamethod/brodsky/blob/501d617d5da8f9f00ff194ce6765d6749647bda8/brodsky.c);
the reading organ is
[`kk.h`](https://github.com/ariannamethod/brodsky/blob/501d617d5da8f9f00ff194ce6765d6749647bda8/kk.h).

| Organ | Current mechanism | Useful Haiku direction |
|---|---|---|
| Two sampling voices — `sample_word` | Separately normalizes `score^(1/0.4)` and `score^(1/1.5)`, then mixes them. Cold share is `clamp(0.5 + (1.2-tau)*0.4, 0.15, 0.85)` | An optional sampler over Haiku's existing transition weights, compared against its single-temperature baseline |
| Rhyme reservation — `pick_rhyme_word`, `generate_line`, `generate_haiku` | Picks a closing word first and reserves its syllables; carries the middle ending forward for ABA → BCB. An 8% branch leaves rhyme unreserved | A form module can reserve constraints before sampling the prefix, with explicit fallback when the vocabulary has no fitting ending |
| Language packs — `load_raw_array`, `generate_haiku` | `inhale/{en,ru,he,fr,es}.h` stores words with language, syllables, mass and emotion. Homographs survive across languages. Line two can admit one foreign ghost word | Keep word identities language-qualified and form data local to each language; measure optional code-switching separately |
| Memory Sea — `sea_record`, `sea_decay`, `sea_resurface` | Stores up to 64 poems with eight key words, coda, chamber state, Julia and depth. A 25% gate admits recall; state similarity, depth and small noise choose a memory; recall strengthens it | A compact episodic memory for reflection and dreams, with explicit retention and repetition measurements |
| Online association — `hebbian_record`, `hebbian_decay`, `line_place_word` | Reinforces co-occurrences, decays/evicts entries in the 64-slot online table, and updates corpus bigram/Hebbian tables from emitted words | Add a measured associative trace while retaining Haiku's original chain and learner updates |
| Reading — `kk_ingest_file`, `kk_choose_doc`, `kk_heavy_score`, `kk_colloc_score` | Known words in local texts create ranked attractions, nearby-word pairs and a chamber profile. A state-selected document influences the next cycle | A small local reading shelf can feed the cloud and dream fragments through recorded source events |
| Self-hearing — `generate_cycle` | Re-ingests emitted text while restoring the user's language and the external dissonance EMA | Reflection can change internal state while retaining the identity of the foreground exchange |
| Rhythm — `punctuate_line` | Uses a local RNG seeded from line word IDs, rather than the generation RNG, to place punctuation | Terminal, browser, replay and internal hearing should share one stable rendering of an utterance |

Memory Sea recall enters the live path: `generate_cycle` applies remembered
words to chambers and installs remembered words/coda as prophecy targets before
generation. At the end, the cycle deposits another episode and strengthens
scars on sufficiently heavy words. The reading shelf similarly contributes
directly to `score_word` through the active document's words and pairs.

Form is a sequence as well as a shape: current cycles stop between **two and
seven tercets**, then add a coda. The stop probability reads accumulated mass,
saturation, planetary dissonance and Julia. Prophecy debt grows Julia between
tercets; Julia stretches destiny distances and changes sampling mixture weight.
Those are Brodsky's own organs; a Haiku experiment should name which one it adds
and record its effect on the original organism.

The current spore saves associations, chamber phases, prophecy, parliament,
Memory Sea, scars and cycle counts. `spore_save` does not serialize the RNG or
destiny vector. Haiku's continuity tests will compare the next exchange after
restart, so its state format needs those fields and explicit vocabulary IDs.

## Experiments, in implementation order

| Experiment | Control and measurement |
|---|---|
| **Reflection keeps the world's trace** | Record foreground dissonance, temperature, language and turn ID; run inner hearing with the same snapshot; require those recorded values to stay fixed while checking that reflection changes the intended next-generation state |
| **A small sea for the owl** | Compare no replay, uniform replay and state-matched replay at equal capacity/update count. Use repeated A → B → A contexts; measure recovered word associations, candidate scores, repeated phrases and save/restart continuity |
| **Words remember their arrival** | Store the pulse/state at first contact with each new word. Compare the same later input after different recorded introductions; freeze the learner and RNG when isolating this trace, then measure its interaction with learning |
| **Two temperatures in one breath** | Compare the original categorical sampler and a cold/hot mixture on the same transition counts. Check exact normalization, candidate entropy, repeated words and form completion; include a single-temperature control matched for entropy |
| **Form grows from state** | Keep fixed 5–7–5 as the baseline and add a separately enabled policy. Log the state and chosen form before generation; test boundaries, exact emitted syllables, rhyme availability and the distribution of forms across a fixed conversation set |
| **A reading shelf in each tongue** | Give the same initial cloud a small pinned document, shuffled document and empty shelf. Track admitted words, associations, recall and later output. Keep language, normalization, syllables and rhyme keys in the pack |
| **Two characters, one sediment** | After distinct owl/friend states exist, compare uncoupled, live-coupled and frozen/shuffled-field runs with identical exchange order. Record which speaker's state changed and which later words followed that change |
| **Rendering preserves the next word** | Render one completed utterance through terminal/browser/replay repeatedly. Require identical text and unchanged learner, memory and generation RNG state |

All experimental application logic belongs in AML. General text, collections,
events and storage belong in the language; numerical learning stays in NoTorch.
The [migration map](MIGRATION.md) and [RAE research](RAE_RESEARCH.md) retain the
main implementation order. New README conversations will come from the AML
organism's own speaking loop.
