# Two frozen currents beneath the words

Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

[`src/lexical_sources.aml`](../src/lexical_sources.aml) lets a small fragment
enter the two PEN7 bodies and return as word attraction and a contextual
representation. One short excursion. Forty-eight possible words. Haiku keeps
its own mouth.

AML owns the context, whole-word policy, two private trajectories, source
lineage and publication. The immutable model value in AML calls NoTorch for
native BPE, fresh forward inference and complete parameter interpolation.
The shared models are the [two approved GGUF assets](../weights/README.md).
Their parameters stay frozen throughout this path.

## The call boundary

| Call | Result |
|---|---|
| `haiku_lexical_new(owner, alpha)` | One restartable, eighteen-field presence |
| `haiku_lexical_request(owner, fragment, source, event, turn, direction, alpha, enabled)` | One explicit source encounter |
| `haiku_lexical_graft(request, fragment, source, event)` | Add the own-voice slot (`5`) or an explicitly delivered friend slot (`6`), once each |
| `haiku_lexical_lineage(request, parent_source, parent_event, origin_source, origin_event)` | Retain an earlier ancestor when a returned fragment becomes another input |
| `haiku_lexical_pass(owner, A, B, request)` | Publish a checked private continuation and return a proposal receipt |
| `haiku_lexical_save` / `haiku_lexical_load` | Validate around the native checkpoint boundary |

`A` and `B` are `model_try_load` values, held outside conversational records.
Each presence receives both shared values. It owns its own two contexts and
history. Direction `0` is the retrospective slot; direction `1` is the
prospective slot. The caller chooses the actual remembered or incoming fragment
at that boundary. Both slots run the same causal PEN7 architecture.

The request captures the owner's alpha. With both bodies available, every
parameter participates in `alpha * A + (1 - alpha) * B`. With A alone the
effective alpha is `1`; with B alone it is `0`. A receipt separates this
effective `alpha` from `requested_alpha`. Each pass starts fresh at learned
position zero, so retained text can continue across a later balance change
without stale keys or values.

The `haiku_lexical_model(A, B)` and
`haiku_lexical_forward(A, B, alpha, tokens)` helpers expose the same source
selection to the representation-to-glyph bridge. Forward returns the final
448-coordinate RMS-normalized contextual vector and 2,048 next-token logits.
NoTorch uses bounded call-local working buffers; the lexical record keeps
neither model tensors nor inference caches.

## A whole word has to fit through the door

The reviewed pool is `reviewed-en-48-v1`: water, tree, light, music, rain,
river, stone, root, leaf, forest, shadow, silence, wind, cloud, moon, morning,
night, sun, sea, shore, bird, feather, song, voice, heart, warmth, home, dream,
memory, path, garden, earth, seed, flower, snow, ember, flame, breath, hand,
door, window, branch, bridge, echo, language, wonder, moss and dawn.

Each has an explicit syllable count checked against Haiku's existing English
estimator. The receipt carries `language="en"`, all 48 `words`, their
`syllables`, and their `scores`. A word's score is the sum of its BPE-piece
next-token logits divided by its piece count, following the Penelope/NanoJanus
word-scoring rule. Every word is scored from the same contextual forward.

Decoded tokenizer pieces cannot extend this pool. The private trajectory
retains its leading two distinct whole-word proposals, with stable pool order
breaking equal scores. The complete pool reaches the caller so parliament
pressure can rank the actual cloud admissions. This module inserts no cloud
word, generator transition or synthetic trigram.

## One declared text excursion

The surface identity is `text-context-v1`. At most 64 BPE tokens from the
chosen private trajectory precede the new text. The new text joins these
bounded fragments, in this order:

| Fragment | Original-text suffix | Source retained |
|---|---:|---|
| Own previous voice | 32 codepoints | Self event ID |
| Explicitly delivered friend voice | 32 codepoints | Friend event ID |
| Primary encounter | 64 codepoints | Actual source, event, parent and origin |

Two spaces separate those slots. For this English organ, ASCII is lowercased
and other codepoints become spaces. Original Unicode suffixes remain in the
history and receipt. The ordinary Haiku tokenizer is unchanged.

The added text is at most **130 ASCII bytes / BPE tokens**, and the complete
forward is at most **194 BPE tokens**, inside PEN7's 256-position window. The
last 64 tokens of that context plus the two private proposals become this
direction's next context. No second forward or autoregressive text generation
occurs inside the pass. An all-blank encoded encounter takes the quiet empty
route.

Each receipt retains `input_source`, `origin_source/event`,
`parent_source/event`, `self_event`, `friend_event`, both bounded original
grafts, and the actual `context_tokens`. Its returning provider source is
`2` for A, `3` for B, or `4` for the parameter mixture. A human-origin fragment
therefore returns as a model proposal while preserving its human ancestor.
Passing through a model does not create another human witness for expert
reward. A request explicitly labeled human must name that same direct human
event as its parent and origin.

## Quiet absence, explicit ownership

Availability bits are A=`2`, B=`4`; the mask is `0`, `2`, `4` or `6`.
Ready source hashes and the tokenizer hash remain visible in receipts.
On the first committed pass, the owner binds its loaded identities. A source
that was absent can later join with the same tokenizer. A bound source's
replacement hash or changed tokenizer rejects publication. Missing sources
can disappear again without rewriting the owner's history.

Disabled, unavailable, empty and replayed requests return `committed=0`.
Failed native forward also returns an uncommitted diagnostic receipt. None
changes the owner or makes any random draw. The disabled branch does not even
inspect model handles; callers can pass `0, 0` there. This organ owns no RNG
and never takes the foreground's RNG as an argument.

Sixteen clocks distinguish eight source classes in each direction. An old
event or explicit replay is a no-op. A later event cannot move the owner's
logical turn backward. Each successful call works on a detached record,
checks the complete candidate, and publishes once. The surrounding session
can place that call inside its own larger save-and-swap transaction.

The eighteen checkpoint fields contain format/policy identities, owner,
revision, clock, alpha, both source hashes, tokenizer hash, the sixteen clocks,
two 64-token contexts with their lengths, and a sixteen-entry round-robin
history with cursor/count. Each history row retains the event, direction,
sender, parent, origin, effective alpha, availability, graft event IDs and
actual token counts. Five text cells hold the three original fragments and
the two private proposals. Overwritten history stays bounded; the current
contexts and source clocks retain their continuation.

## Executed gate

Build AML with immutable model values and NoTorch's immutable PEN7 provider,
then run:

```sh
bash tests/run_lexical_sources.sh
```

The real checked-in Q8/F32 pair runs through interpreted and compiled AML.
The gate covers zero/one/two sources, explicit effective balances, the exact
mean-BPE word scores, the reviewed syllable/word boundary, separate presence
and direction ownership, delivered-versus-withheld friend context, original
and returned ancestry, bounded input, duplicate and explicit replay, source
disappearance after lived context, and exact checkpoint continuation.
Both execution paths produce the same checkpoint bytes; asset SHA-256 values
are unchanged after all forwards. Invalid alpha, corrupted retained revision,
human relabeling of a model return, model replacement and tokenizer-fragment
admission are rejected.

The [Dario](research/iceberg/DARIO.md) and
[Arianna](research/iceberg/ARIANNA_INJECTION.md) receipts carry the source
research for this text return. The larger trajectories, glyph feedback and
live-session boundary are mapped in [the iceberg](ICEBERG.md).
