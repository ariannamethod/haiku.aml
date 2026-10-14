# HAiKU — things still growing

Work follows the [Arianna Method Manifesto](ARIANNA_METHOD_MANIFESTO.md).
The implementation order is in [MIGRATION.md](docs/MIGRATION.md).

## Learning and continuity

- [x] Port MathBrain and RAE's original features, forward/reverse passes,
  score selection, and online parameter updates through canonical NoTorch.
- [x] Connect input, cloud/transition memory, five candidates, RAE selection,
  quality, and MathBrain learning in an AML foreground session. Keep additional
  RAE learning an explicit option; Python chat trains MathBrain alone.
- [x] Give foreground and background work one versioned experience record with
  source, scoring features, quality, state revision, and publication order.
- [x] Save and restore learner parameters, statistics, and owned random
  streams together with the cloud and conversation state.
- [x] Connect MetaHaiku reflection and echo/drift/meta rings after foreground
  learning; retain their owners, shared draw order, and restart continuity.
- [x] Complete Phase4 state-ID aggregation beyond the current metric handoff;
  connect dream events after reflection and rings.
- [x] Carry the complete exchange tail into a reusable AML worker with typed
  results, one state publisher and revision checks. Join before save, swap and
  display; preserve serial output and checkpoint bytes under controlled
  schedules. Quit/EOF has no outstanding turn in this profile. See
  [ASYNC](docs/ASYNC.md).
- [ ] Add early response display and overlap the next input with the owned
  tail. First persist a pending experience, then implement exact restart replay,
  next-input/quit/EOF drain and single publication of the completed job.

## Family experiments

- [x] Audit Penelope, NanoJanus, Actually.life, Leo and caveLLMan; verify both
  PEN7 assets through existing NoTorch operations. Preserve source pins,
  executable receipts and the proposed [iceberg](docs/ICEBERG.md).
- [x] Convert and retain the two immutable sources as Q8_0/F32 GGUF in
  [weights](weights/README.md), with source revisions, licenses, hashes and
  numerical conversion/forward receipts. NoTorch owns the converter and native
  forward; matrices expand once to frozen F32.
- [x] Audit DoE and LoRAGrad and implement an isolated AML
  [parliament](docs/PARLIAMENT.md): D96, rank four, four-to-sixteen private
  experts, 64 combined immune memories, 32 receipts, delayed human-witness
  reward, owned cursors, lifecycle and complete restart state.
- [x] Audit [Dario v2](docs/research/iceberg/DARIO.md) and
  [Arianna's voice exchange](docs/research/iceberg/ARIANNA_INJECTION.md), with
  pinned source paths and bounded probes. Separate context continuation,
  embedding direction and hidden attention; record the return-path experiment.
- [x] Connect the experimental parliament to real cloud, human, inner-voice,
  friend and model features. Give each presence its own persistent organ;
  measure pressure and word-admission changes in the separate experimental
  [iceberg conversation](docs/ICEBERG_SESSION.md).
- [x] Add AML immutable PEN7 handles and optional lexical sources with quiet
  missing-weight fallback, explicit word admission and immutable shared models.
- [x] Give both Haiku and the dream-friend their own retrospective/prospective
  contexts and fixed-per-pass parameter mixtures. Persist four histories,
  separate balance preferences and explicit exchanges while sharing immutable
  weight handles. Keep readiness, hashes and effective one-source coefficients
  explicit at the AML/NoTorch boundary.
- [ ] Evolve each presence's Gamma/Janus balance, calendar and operative debt
  through its history; compare dynamic mixing with the fixed-alpha controls.
- [ ] Add a conceptual field beneath the words: persistent glyph attraction,
  learned word bindings, then recursive symbols born during dream consolidation.
  Keep external observations, peer exchange and internal replay distinct.
- [ ] Let model representations contribute to glyph formation and learned
  concept bindings; let that field influence later parameter mixing and words.
  Use NoTorch for numerical work, retain shared concept identities and each
  presence's own experience, and compare with the working weightless field.
- [x] Let each presence send a selected human/self/friend fragment into
  its owned lexical trajectory and receive word/glyph proposals at a declared
  boundary. Preserve source ancestry, frozen bodies, private histories and
  exact disabled behavior. The first bounded text-context excursion runs in
  `iceberg.aml`, with explicit self/peer graft controls and real return receipts.
- [ ] Compare context injection and embedding-direction pressure independently
  at fixed parameter balance. Measure relevance, copying, glyph pressure and
  form; apply immune gain after normalization and invalidate dependent caches.
- [ ] After the baseline organs work, run the controlled extensions in
  [FAMILY.md](docs/FAMILY.md): reflection preserves foreground state, episodic
  recall, state-at-contact word memory, sampling mixtures, variable form,
  local reading, Haiku/friend coupling, and stable rendering.
- [ ] Run the ranked [Q experiments](docs/Q.md): accepted experience and
  bounded expectations with consolidation, memory of changing regimes, then
  calibrated untrained geometry. Record gate activations and foreground-state
  changes against the working baseline.

## Future interface

- [x] Replace the README's Python dialogue examples with recorded conversations
  from the new AML organism once its speaking and dream paths are running.
- [ ] Give Haiku and its inner dream-friend a visible conversation: two
  characters, alternating lines, pauses, and distinct movement. Render the
  actual dream-dialogue events so their exchange can be watched as it happens.
- [ ] Design Haiku and the friend when the interface work begins. Retire the
  owl idea: Oleg noticed the Duolingo association. Find an original, spare
  character whose listening, hesitation, and dreaming grow from real events.
  Both appearances and their visual relationship remain open.

Recorded from Oleg's direction on 2026-10-07. Core language and organism work
come first; this is a later interface task.
