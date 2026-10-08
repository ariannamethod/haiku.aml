# Q / PostGPT — coherence from the tokenized field

2026-10-07. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).
Source audit and proposed experiments; these extensions are not implemented in Haiku.

**Q is the active PostGPT line.** Its README names PostGPT as its direct ancestor
and places active development in Q. The inherited path is corpus → BPE tokens
→ MetaWeights → continuation: tokenization fixes the identities, and bigram and
trigram structure gives the field its local coherence. Haiku brings this
principle into its cloud and transition memory while preserving its own
Python SentencePiece and regex contracts.

Inspected [Q at `f5d00a36ecfcdb5e655e1576770f03d06d900e04`](https://github.com/ariannamethod/q/tree/f5d00a36ecfcdb5e655e1576770f03d06d900e04).
The direct ancestor was inspected at
[PostGPT `b711595defb135231dda9858677a4a66578d0d9b`](https://github.com/ariannamethod/postgpt/tree/b711595defb135231dda9858677a4a66578d0d9b).
The C engine is the execution reference below; measurements call the original
Python mechanisms. The README presents **θ = ε + γ + αδ**: transformer substrate,
living statistical field, and an adapting parliament. It publishes attention
losses of 2.41 versus 2.86 and a 2M-versus-100M comparison. The measurements in
this audit are the contract tests and small mechanism probes recorded below.

Q's useful kinship is concrete: experience changes the field while speech is
happening. Haiku already has a living cloud, transition memory, dissonance,
dreams, and two tiny learners. Q supplies ways to give those organs different
memory timescales and a record of expectations that have not yet resolved.

## The units underneath coherence

Original PostGPT's `BPETokenizer` learns ordered merges over 256 UTF-8 byte
roots. `MetaWeights.build` collects token frequencies, bigrams, trigrams,
positional affinities, and co-occurrences. Its `generate_meta` first tries the
exact last two tokens, then a bigram, then unigram fallback. Association boosts,
repetition penalties, top-15 filtering, and temperature operate on those
corpus-backed continuations.

Q keeps that identity through its transition field. Its `coherence_score`
combines mean bigram strength, 0.5 times adjacent Hebbian density, 0.8 times
mean trigram strength, and a length bonus. Surface boundary scoring supplies
another signal during candidate selection. Prophecy, phase memory, and SPA
are additional mechanisms on this substrate.

The shipped `q.merges` has 1,024 merges and 1,280 entries. Of these, 117 entries
span multiple whitespace-separated units. `The cloud remembers.` becomes
`["The ", "clou", "d ", "remem", "ber", "s", "."]`; lowercase `the ` has a
different identity. There is no whole-word reconstruction before Q's transition
memory. For multilingual byte BPE, concatenate token bytes before UTF-8 decoding:
decoding each incomplete byte piece separately loses characters.

Haiku's exact tokens feed its cloud, observer, generator, and learners. Its
SentencePiece path lowercases, normalizes with the model, encodes Unigram pieces,
removes every `▁`, and drops empty pieces; regex mode retains lowercase word
runs. Keep the source text, selected mode and model, ordered tokens, and rolling
triples explicit. A future reconstructed-word view needs its own named contract
and checks. The PostGPT connection makes preserving these boundaries more
important: changing token identity changes the field being learned.

## What actually runs

1. **Build the field.** Load BPE merges and corpus; build unigram frequencies,
   normalized bigrams/trigrams, and distance-weighted Hebbian associations.
   Classify words through nearby chamber anchors. Load trained weights if
   supplied; otherwise create zero token embeddings and zero transformer weights.
2. **Restore continuity.** Load SQLite state, or the binary fallback, then blend
   the slower spore residue. Initialize four fresh rank-4 parliament experts.
3. **Enter a chain.** Ingest the prompt, update chambers/soma/scar, choose a
   velocity profile, and soften debt. Twelve steps follow the calendar/debt
   backward/pivot/forward schedule. Each step selects a document and chunk,
   then a prompt from the user, interference, or corpus boundaries.
4. **Generate each candidate.** Run transformer forward and its magnitude gate;
   inject parliament output; update experts from the previous prediction debt;
   mix Hebbian, prophecy, destiny, bigram/trigram, prompt, and document signals.
   Apply frequency, repetition, and surface adjustments; select the next token;
   age/fulfill prophecies and capture the transition into memory.
5. **Choose and feel.** Try three candidates, or five for the first two weightless
   steps, with early exit. Score continuity and surface form, optionally jump
   through a wormhole, print, ingest the selected text, update phase memory,
   record events, and cross-couple chambers. SPA then checks the twelve sentences.
6. **Consolidate and save.** Decay Hebbian strength after each chain. On exit,
   convert event residue into field/association changes and save SQLite, a binary
   shard, and a spore.

Loci: [`main`](https://github.com/ariannamethod/q/blob/f5d00a36ecfcdb5e655e1576770f03d06d900e04/postgpt_q.c#L1930),
[`gen_chain`](https://github.com/ariannamethod/q/blob/f5d00a36ecfcdb5e655e1576770f03d06d900e04/postgpt_q.c#L1518),
[`gen_sent`](https://github.com/ariannamethod/q/blob/f5d00a36ecfcdb5e655e1576770f03d06d900e04/postgpt_q.c#L1279).

Actual decoding at this pin is **top-15, then nucleus**. Weightless generation
uses ten greedy tokens followed by temperature 0.35 / top-p 0.55. The trained
path uses two greedy tokens, three colder top-p 0.60 tokens, then top-p 0.85.
The README's four-token greedy description belongs to an earlier schedule.

## Organs worth studying

| Q mechanism and source | Exact behavior | Useful Haiku experiment |
| --- | --- | --- |
| [Active prophecy, C:220–280](https://github.com/ariannamethod/q/blob/f5d00a36ecfcdb5e655e1576770f03d06d900e04/postgpt_q.c#L220) | Up to 32 `(target, strength, age)` records. Unfulfilled records decay by 0.995, contribute `strength*log(1+age)`, expire at age 50, and disappear on fulfillment. | Carry a small set of unresolved words/motifs between turns and dreams; log fulfillment and expiration. |
| [Experience consolidation, C:449](https://github.com/ariannamethod/q/blob/f5d00a36ecfcdb5e655e1576770f03d06d900e04/postgpt_q.c#L449) | Summarized events strengthen current prophecies, co-attract their targets, and reinforce chamber anchors. | Give accepted exchange and dream events explicit origins, then consolidate their summaries at a revision boundary. |
| [Fast traces, C:863–892](https://github.com/ariannamethod/q/blob/f5d00a36ecfcdb5e655e1576770f03d06d900e04/postgpt_q.c#L863) | Hebbian delta immediately changes `A`; a 0.96/0.04 trace also accumulates. Mass ≥0.002 enables a second normalized trace update; `B` decays by 0.999. | Compare gradual traces with thresholded consolidation in the existing eight hidden→output connections described in [RAE research](RAE_RESEARCH.md). |
| [Chambers and phase memory, C:378–416, 535–604](https://github.com/ariannamethod/q/blob/f5d00a36ecfcdb5e655e1576770f03d06d900e04/postgpt_q.c#L378) | Six bounded chamber activations decay and cross-couple from one prior-state snapshot. Slower soma, coherence, phase lock, and threshold bias change coefficients and gates. | Add declared slow state behind Haiku's existing pulse, retaining the current pulse and temperature formulas as the baseline. |
| [Chunk resonance, C:674–817](https://github.com/ariannamethod/q/blob/f5d00a36ecfcdb5e655e1576770f03d06d900e04/postgpt_q.c#L674) | Choose a document, then a chunk using lexical, chamber, and prophecy signals; heavy tokens bias the continuation. | Recall a bounded remembered fragment or dream by overlap plus unresolved motif, with source and score visible in events. |
| [SPA geometry, C:1470–1514](https://github.com/ariannamethod/q/blob/f5d00a36ecfcdb5e655e1576770f03d06d900e04/postgpt_q.c#L1470) | Fresh random 32-dimensional token vectors per chain; tail-weighted mean with α=0.85, normalization, then exponentiated pair affinity plus distance bias. | Compare stable untrained token geometry with exact word/trigram overlap for Haiku's three lines. Calibrate the gate before connecting it to revision. |
| [Sampling, C:70–92, 1396–1428](https://github.com/ariannamethod/q/blob/f5d00a36ecfcdb5e655e1576770f03d06d900e04/postgpt_q.c#L70) | Restricted candidate mass, staged randomness, distance-weighted repetition factors, and a word-boundary adjustment. | Apply sampling only after legal syllable continuations are identified; measure form, repetition, and diversity together. |

AML already has the related vocabulary and concrete field operations: prophecy,
destiny, debt, velocity, and Schumann state. Its
[`spa_embed` / `spa_connectedness`](https://github.com/ariannamethod/ariannamethod.ai/blob/947ae6737ac68b0c27a705ff947f986ddd388c48/core/ariannamethod.c#L6413)
implement the same weighted-mean and pair-affinity geometry. This is a practical
bridge for an untrained architecture experiment. Haiku's update laws belong in
`.aml`; general missing math, seeded randomness, and persistence capabilities
belong in AML or NoTorch. Q's C inference embeds its own numerical routines;
the vendored NoTorch code serves its separate trainer.

## Receipts and consequences for the port

**28 original Python contract tests passed** with
`PYTHONDONTWRITEBYTECODE=1 python3 -m unittest tests.test_contract`.
Small probes used the same source, standard Python only, no loaded checkpoint
and no training run.

| Probe | Measured result |
| --- | --- |
| Four rank-4 experts, seed 0, zero input/debt, 64 updates | Logit injection L1 = 0; maximum `A` change = 0; trace mass = 0; consolidations = 0. |
| One prophecy of strength 0.6, five unfulfilled updates | Pressure rises from 0 to 0.2621116782; fulfillment removes the record. |
| Bigram score 1.0, one `ingest_ids([1,2], 0.02)` | Score becomes 1.02: online scores are additive strengths after the initial normalization. |
| SPA, 32 seeds × three twelve-sentence cases | Minimum/mean score ratios range 0.8124723868–0.9873070150; zero reseeds at the most permissive threshold 0.70. |
| Prophecy mask with byte 1 and token 257 | Unseen token 257 is suppressed after byte 1; an already-seen 257 remains eligible. |
| SQLite restore into an already-built corpus field | A stored trigram score 0.9 is appended behind the corpus score 0.1; lookup returns 0.1 and the entry count becomes two. |

The weightless path's token embeddings are zero. Therefore its embedding-based
destiny direction, expert input, and expert prediction-debt vector are zero.
The measured parliament result follows directly. Statistical memory, lexical
interference, surface rules, and chamber coefficient modulation carry that path.
Haiku can bring those mechanisms home with its cloud and transition generator.

**The current SPA reseed gate is unreachable for finite normalized inputs.**
Let `m` be the minimum connectedness score. Each vector has norm ≤1, so the
pair exponent lies between `−1/√32 + 0.1/12` and `1/√32 + 0.05`.
The other eleven sentences have 110 directed edges between them; the chosen
sentence's edges contribute `2m` to the total. Consequently:

```text
m / mean(score) >= 12 / (2 + 10 * exp(2/sqrt(32) + 0.05 - 0.1/12))
                = 0.7122883913
```

The actual threshold is `0.52 + 0.18*(1-phase_gate)`, at most 0.70.
That bound explains the zero activations. A future geometry change needs an
activation fixture before a generation-quality experiment. The subsequent
acceptance rule also permits `new_score > 0.7*old_score` **or** greater length;
it accepts some lower-scoring replacements.

Other execution details determine the adaptation:

- **Candidate experience is shared.** `gen_sent` changes MetaWeights, prophecies,
  and experts for every trial. Best-of-N restores only the initial destiny;
  after selection the surviving destiny belongs to the last trial, not
  necessarily the selected trial. Scores are evaluated after each trial's own
  memory updates. Freeze one revision when comparing Haiku candidates, then
  publish deliberately chosen experience.
- **Persistence has distinct bodies.** C calls an external `sqlite3` executable.
  SQLite appends loaded trigram/Hebb rows to the corpus field; trigram lookup
  returns the first match. The binary fallback omits active prophecy records;
  spores carry at most sixteen. Expert matrices/traces and RNG state are not
  serialized. Haiku needs one versioned state contract and idempotent restore.
- **Use exact token identity.** The prophecy seen-mask indexes `target % 256`
  while only recording context IDs below 256. Haiku's native string lists can
  represent exact word identity; its multilingual domain must keep that identity.
- **Sampling arithmetic matters.** Multiplying a negative logit by a factor
  below one increases it. Q's repetition factors therefore have opposite effects
  across the sign boundary. Test positive, zero, and negative scores explicitly.
  Q's C periodic/somatic word scanner uses bytewise `isalpha` and English seeds;
  multilingual Haiku needs its own language packs and Unicode word boundaries.

## Three experiments, in order

1. **Accepted experience, then slower consolidation.** After preserving the
   original cloud/generator, compare no prophecy, a 16-record prophecy buffer,
   and the same buffer plus dream-time consolidation. Give every record a
   target word, strength, age, language, source, and revision. Update once per
   accepted exchange; candidate trials read a frozen revision. On a fixed
   A→B→A motif stream, measure motif return after 1/5/20 turns, fulfillment and
   expiration rates, repeated trigrams, 5–7–5 validity, and memory bytes.
   Check a midpoint save/reload for identical next selections and updates, and
   check that restoring the same snapshot twice does not duplicate entries.
   This extends the replay design in [RAE research](RAE_RESEARCH.md).
2. **Let the field remember a regime.** Compare the original dissonance→temperature
   mapping with a versioned coherence/phase-lock/threshold-bias recurrence.
   Initially drive it from Haiku's own overlap, novelty, entropy, and observed
   form quality; record each coefficient's effect. Use alternating calm/novel
   blocks, single-turn spikes, and a vocabulary shift. Measure response lag,
   recovery, temperature variance, regime switches, diversity, and form validity.
   Verify bounded state and explicit input treatment for arousal greater than one.
   Keep the existing 57-parameter learners and their feature schema intact.
3. **Make geometry earn a revision.** Compare exact word/trigram overlap with
   stable seeded 32-dimensional token vectors and tail-weighted line embeddings.
   Use three-line fixtures with one deliberately unrelated line, then fixed
   generated candidates. Calibrate a similarity scale/quantile threshold on one
   fixture split and report gate recall and false activation on another. Compare
   original sampling against a short greedy start plus top-k/top-p over only
   syllable-legal continuations. Log seeds, eligible sets, chosen probabilities,
   repeated trigrams, form validity, and elapsed time. Accept a revised line
   only against the same frozen scoring state and an explicit improvement rule.

## Reproduce the SPA and zero-input receipts

Run from the pinned Q checkout. The three SPA cases are a rolling token sequence,
eleven repeated sentences plus an outlier, and random sentences of length 20.

```sh
PYTHONDONTWRITEBYTECODE=1 python3 - <<'PY'
import math, random
import postgpt_q as q
ratios = []
for seed in range(32):
    random.seed(seed)
    s = q.SPACtx(); q.spa_init(s, 64)
    cases = [[[i, (i+1)%64, (i+2)%64] for i in range(12)],
             [[1,2,3]] * 11 + [[50,51,52]],
             [[random.randrange(64) for _ in range(20)] for _ in range(12)]]
    for sentences in cases:
        e = [q.spa_embed_sentence(s, x, len(x)) for x in sentences]
        scores = q.spa_cross_attend(s, e, 12)
        ratios.append(min(scores)/(sum(scores)/12))
print(len(ratios), min(ratios), max(ratios), sum(x < .7 for x in ratios))
print(12/(2+10*math.exp(2/math.sqrt(32)+.05-.1/12)))
random.seed(0)
p = q.Parliament(); q.parl_init(p, 4, 4)
before = [list(e.A) for e in p.ex]
logits = [0.] * 12
q.parl_inject(p, logits, [0.]*4, 12)
for _ in range(64): q.parl_notorch(p, [0.]*4, [0.]*4, 4)
print(sum(map(abs, logits)),
      max(abs(x-y) for e,a in zip(p.ex,before) for x,y in zip(e.A,a)),
      sum(e.plasticity_mass for e in p.ex), sum(e.consolidations for e in p.ex))
PY
```
