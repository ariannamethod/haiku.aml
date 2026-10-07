# HAiKU AML — agent work rules

Read the [Arianna Method Manifesto](ARIANNA_METHOD_MANIFESTO.md) first.
All repository work follows it. Preserve the Method's language, presence,
resonance, recursion, memory, and continuity throughout code and documentation.

## The organism

- The behavioral and artistic reference is
  [Python HAiKU](https://github.com/ariannamethod/harmonix/tree/main/haiku).
  Read its implementation, dialogue examples, and tests before porting a module.
- Preserve its living word cloud, dissonance, haiku voice, inner voice,
  recursive selection, three rings of overthinking, dreams, state transitions,
  and asynchronous learning. Track each part explicitly during the port.
- Write the organism in AML. Its two project dependencies are
  [AML](https://github.com/ariannamethod/ariannamethod.ai) and
  [NoTorch](https://github.com/ariannamethod/notorch).
- Put missing general language/runtime capabilities in AML and missing
  numerical operations in NoTorch. Keep Haiku's behavior in `.aml` modules.
- Account for every removed Python dependency, including persistence and
  tokenization. Data files and browser assets are part of the project.
- Learn from `haiku.c`, `haiku`, `klaus.c`, and `yent.aml` while preserving the
  Python organism's lineage. Multilingual growth follows Klaus's useful ideas;
  each language needs its own text and syllable behavior.
- Build the local HTML interface after the core. Its owl responds to actual
  organism events. Keep presentation and numerical work in their own layers.

## Engineering

- Read the README and current migration notes. Check `git status` before edits.
- Work on a dedicated branch; open a PR and let Oleg merge. Preserve unrelated
  work and refresh the base branch before finalizing changes.
- Pin source revisions in audits. Separate implemented behavior, measured
  results, and planned work with concrete file references.
- Verify ported behavior using fixed seeds, explicit event order, numerical
  tolerances, persistence round trips, and live dialogue runs as applicable.
- Keep asynchronous state ownership explicit: foreground exchanges, learning,
  dreams, and persistence must agree on when an update becomes visible.
- Keep documentation concise and playful. Carry the original README's voice.
- Commit source and small reproducible fixtures. Keep credentials, generated
  binaries, live state, and downloaded model weights out of git.
