# Frozen PEN7 contribution to word→anchor proposals

Measured 2026-10-14 against the source pins below. The original probe ran in scratch without changing contact records. This receipt, the portable [AML probe](../../../tests/research/iceberg/word_anchor_probe.aml), and [all 50 measured samples](../../../tests/research/iceberg/word_anchor_probe.json) preserve that experiment.

## Fixed identities and primary code

- Haiku: `cbb78c76110db18a29249fb0d1ac6ccf4b9245ac`.
- AML: `90684ad3784726f54a5c4d19cdadc93b00a224b0`.
- NoTorch: `f6504cf8e8799c6a01b26a75ab90f004a6a3d804`.
- A: `50f159a2c34952bd9e6349880b4b20f8bbc0ac4b1f1dc2c1f6e4768732548b66`.
- B: `16909b8590bd7fbbf15ab11b2dbf3743d612da8c617aca2dda03d640fef13e29`.
- Tokenizer: `664afe692a006a54fc00406209042f6d452748d64a883ecfab9ef66d41f8bdf2`.
- Alpha `.5`; availability mask `6` throughout all successful calls.

Primary implementation: `src/parliament_contact.aml` functions `haiku_contact_enrich`, `haiku_contact_projection` and `haiku_contact_prototype_ids`; `src/lexical_sources.aml` function `haiku_lexical_forward`; AML `core/aml_notorch.c` model provider; NoTorch `pen7.h` and `pen7.c`.

NoTorch returns the final last-token RMS-normalized hidden vector, width448, consumed by its output head. Each forward begins at learned position zero. Parameter mixture happens before inference and the two source bodies remain immutable. The current reference cache holds four vectors for exact isolated words `water`, `tree`, `light`, `music`, identified by provider pair, tokenizer, source and effective alpha.

## Protocol

Four independent one-word forwards built the prototypes. Fifty subsequent sample calls covered four anchors, eight seed aliases, sixteen words absent from the English binding map, four unrelated controls, three ambiguous isolated words, eight word-final phrases, two orthographic controls, four artificial anchor-prefix controls, and one exact repeat. Total: **54 successful forwards**, with identical provider pair and alpha. Text inputs used 1–7 BPE tokens. No context trajectory or parliament publication participated.

For each sample, encode its exact text and call `haiku_lexical_forward(A,B,.5,ids)` once. Cosine is `dot(hidden, prototype) / (norm(hidden)*norm(prototype))`; the table records raw cosines, including negative values. The proposed label is argmax across four anchors, and margin is largest minus second-largest raw cosine. Clamping negative values to zero, as current contact projection does, leaves all recorded top choices unchanged.

`stream` was forwarded twice; all 448 hidden values matched exactly. The missing/missing helper returned `ok=0` without a provider call; stderr was empty. Model SHA-256 values after the probe matched the identities captured at load.

## Measured result

The sixteen unbound words were grouped before execution into water: `stream wave tide droplet`; tree: `oak birch branch bark`; light: `glow lamp photon radiance`; music: `rhythm violin harmony concert`. Argmax agreed with these intended four-way associations for **2/16** (`photon`, `radiance`). Of the eight already reviewed seed aliases, only **2/8** (`sun`, `bright`) returned their existing anchor. The four exact reference names returned themselves at cosine approximately1.

| Exact forward text | Water | Tree | Light | Music | Top | Margin |
|---|---:|---:|---:|---:|---|---:|
| `river` | 0.057788 | 0.233173 | 0.238756 | 0.121978 | light | 0.005583 |
| `rain` | 0.144931 | 0.242562 | 0.420020 | 0.089443 | light | 0.177458 |
| `forest` | 0.239940 | 0.107673 | 0.359011 | 0.160250 | light | 0.119071 |
| `sun` | 0.237755 | 0.187860 | 0.458030 | 0.222176 | light | 0.220275 |
| `bright` | 0.152538 | 0.121450 | 0.614102 | 0.135409 | light | 0.461564 |
| `song` | 0.186665 | 0.169895 | 0.233354 | 0.124840 | light | 0.046688 |
| `stream` | 0.197131 | 0.209533 | 0.329060 | 0.206168 | light | 0.119527 |
| `wave` | 0.231075 | 0.423631 | 0.333133 | 0.191451 | tree | 0.090497 |
| `oak` | 0.223237 | 0.364399 | 0.411764 | 0.078801 | light | 0.047365 |
| `glow` | 0.140116 | 0.113274 | 0.072853 | 0.144487 | music | 0.004371 |
| `photon` | 0.214993 | 0.231174 | 0.261351 | 0.131125 | light | 0.030177 |
| `radiance` | 0.169039 | 0.338005 | 0.376360 | 0.084004 | light | 0.038355 |
| `rhythm` | 0.211084 | 0.261872 | 0.434058 | 0.299813 | light | 0.134245 |
| `tax` | 0.156542 | 0.518929 | 0.188809 | 0.097094 | tree | 0.330120 |
| `quorvex` | 0.063480 | 0.084590 | 0.248097 | 0.088537 | light | 0.159560 |
| `tree bark` | 0.195801 | 0.323896 | 0.244456 | 0.026674 | tree | 0.079441 |
| `dogs bark` | 0.239493 | 0.157665 | 0.187059 | 0.007548 | water | 0.052434 |
| `musical pitch` | 0.247912 | 0.204870 | 0.306411 | 0.396772 | music | 0.090361 |
| `dark pitch` | 0.142751 | 0.218162 | 0.303345 | 0.180312 | light | 0.085183 |
| `water quorvex` | 0.282245 | 0.060559 | 0.257017 | 0.131856 | water | 0.025227 |
| `tree quorvex` | 0.096534 | 0.259587 | 0.210691 | 0.059202 | tree | 0.048896 |
| `light quorvex` | 0.118143 | 0.115451 | 0.351095 | 0.091289 | light | 0.232952 |
| `music quorvex` | 0.179202 | 0.125571 | 0.259970 | 0.332422 | music | 0.072453 |

**A minimum cosine and minimum margin cannot separate the correct held-out cases from the unrelated `tax` control in this sample.** `tax→tree` has cosine `.518928945`, margin `.330119669`, exceeding both corresponding values for `photon→light` (`.261350572`, `.030177072`) and `radiance→light` (`.376359552`, `.038354516`). Any such lower-bound gate admitting either successful held-out case also admits `tax`.

The artificial spelling `quorvex` chooses light in isolation. Prefixing the same spelling with each reference word makes its final hidden vector choose water, tree, light, or music respectively. Attributing the whole phrase representation to the spelling would create four contradictory names for the same unknown. The word-final contexts `tree bark`/`dogs bark` and `musical pitch`/`dark pitch` also change the top anchor. Context can be retained as a phrase-level proposal with its exact token sequence; it cannot serve as an isolated lexical identity.

## Recommended boundary for the first learned overlay

Keep promotion governed by accepted, independent external evidence. Use fixed reviewed seed bindings as its teachers, preserve language and owner, require repeated distinct encounters/turns and actual local-window evidence, and keep an unknown tentative or conflicted until that requirement is met. Replays, derived model events, own outputs and echoes add no external hearing count. Previously learned words should not bootstrap each other into a self-confirming chain. The seed/manual table remains authoritative at lookup; learned overlay entries retain their own confidence and source history.

The measured four-reference raw-hidden score contributes **no promotion vote, no accelerated hearing count, and no veto** in that first path. Its weak performance here would otherwise either create unrelated bindings or suppress legitimate repeated evidence. Current bounded phrase-level model pressure can continue under its existing provenance; that is separate from naming a new word.

For a future explicitly enabled proposal receipt, one isolated-word call is the smallest attributable unit:

1. The word must be an exact normalized unknown token in the accepted external receipt, with English language and 1–16 ASCII letters. Encode that word alone, at most16 BPE tokens. Preserve the exact spelling and token IDs.
2. Capture owner, original event/turn and parent/root ancestry. The result keeps model source2/3/4 and names the external event as its parent; it never becomes a native source0 witness.
3. Capture provider identities, tokenizer, availability and effective alpha. Single A is alpha1, single B alpha0. Use a matching immutable four-prototype cache; a mismatch requires four explicitly budgeted rebuild forwards or defers the proposal.
4. Permit at most one new-word forward per accepted external event, selected deterministically after eligibility checks. Replays and missing sources perform zero forwards, consume no RNG and leave evidence counts/checkpoints untouched. A valid existing cache costs zero rebuild calls; cold or changed alpha costs four.
5. Retain four raw similarities, proposed anchor, runner-up margin, input mode `isolated-word-v1`, and identities as a noncommitting observation. Current probe supplies no accepted threshold for promoting that observation.

No new runtime implementation is proposed in this audit. The existing forward/projection helpers supply the numerical path; a future thin adapter could fit one or two functions if a later measured representation earns admission authority.

## Reproduce

From the Haiku repository root, with its matching AML/NoTorch build and both
checked-in weights:

```sh
haiku_probe_work=$(mktemp -d)
"${HAIKU_AML:-../ariannamethod.ai/runner/aml-notorch}" \
  tests/research/iceberg/word_anchor_probe.aml > "$haiku_probe_work/word-anchor.txt"
python3 tests/research/iceberg/check_word_anchor_probe.py \
  "$haiku_probe_work/word-anchor.txt"
rm -rf "$haiku_probe_work"
```

The checker verifies both body/tokenizer identities, sample spellings, seed
membership, token lengths and all 200 cosine values against the recorded JSON
with absolute tolerance `1e-6`. It also reports the two measured agreement
counts. The full-float hidden repeat and quiet-absence assertions run inside
AML. This research probe is explicit and separate from the weightless learner
gate.
