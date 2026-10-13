# caveLLMan: glyph substrate for the Haiku iceberg

Read-only audit, 2026-10-13. Repository: `ariannamethod/caveLLMan`.
Pinned commit: `471aed21abe9b7b882a464f07bc66b8ef3d5c411`.
Checkout: `<research-workspace>/caveLLMan`.
No AGENTS.md, CLAUDE.md, or manifesto file is tracked in this checkout. README and source were read. Repository files were not changed; probes live beside the checkout.

## The useful contribution

caveLLMan makes concept compression and pressure concrete: text becomes a short chain of shared glyph IDs; a persistent co-occurrence surface remembers which concepts arrive together; surprise accumulates into excitement and dissonance; a threshold controls when the inner process speaks. The compressor and surface operate without a transformer. This is a good small internal organ for Haiku, where a handful of spoken lines can carry the effect of a much larger, persistent conceptual life.

For the proposed Haiku extension, the concrete boundary should be: original words remain in the cloud and spoken generator, while a glyph trace supplies attraction, expectation, and admission pressure. The new substrate can guide Penelope/Janus word proposals and dreams while Haiku's existing form selector keeps the visible compression. This is an integration proposal, not current caveLLMan/Haiku behavior.

## File map

| File | Relevant implementation |
| --- | --- |
| `ariannamethod/semantic_tokenizer.h:23–52` | Canonical 88 base glyphs and stable IDs |
| `ariannamethod/semantic_tokenizer.h:58–169` | Literal synonym map and stop words |
| `ariannamethod/semantic_tokenizer.h:173–228` | Word lookup and text compressor |
| `cavellman.c:98–122,201–239` | Symmetric co-occurrence counts, normalization, decay |
| `cavellman.c:251–325,345–382` | Emerged pair metadata, survival, use counters |
| `cavellman.c:388–477,527–617,642–680` | Loaded transformer, zero-initialized Hebbian factors, inference and online updates |
| `cavellman.c:740–785,2082–2108,2238–2265` | Pressure field, hearing, gate, decay, maturity |
| `cavellman.c:853–908` | Passive sentence ingestion into every cave |
| `cavellman.c:2272–2321` | Runtime autoregressive generation and feedback |
| `cavellman.c:3034–3059` | Persistent spore header |
| `ariannamethod/train_cavellman.c` | From-scratch autoregressive/CPT training |
| `ariannamethod/train_diffusion.c` | Separate experimental diffusion trainer |
| `42/` | Larger gravitational-dispatch sibling; not needed for minimal Haiku glyph organ |

Pinned source root: <https://github.com/ariannamethod/caveLLMan/tree/471aed21abe9b7b882a464f07bc66b8ef3d5c411>.

## Executed semantic compressor

The actual C map contains **514 entries / 506 unique alias strings**, plus direct lookup of the 88 canonical names. Eight alias entries conflict with earlier mappings; a direct canonical glyph wins first, otherwise the first matching alias wins. There is no suffix/morphology branch in `semtok_word` or `semtok_line`.

Processing order:

1. Copy at most 4,095 bytes.
2. ASCII-lowercase A–Z; replace everything except a–z, digits, space, apostrophe and hyphen with spaces.
3. Skip the stop list, then exact canonical/alias lookup; skip unknown words.
4. Suppress consecutive mapped duplicate IDs. Skipped words do not reset the previous ID.
5. Stop at the caller's token budget; engine feed uses at most 127 IDs.

The following rows are direct output of `<research-workspace>/cavellman-iceberg-probe` compiled against the pinned header:

| Input | Actual glyph output |
| --- | --- |
| `water river ocean dream rain rain` | `water dream water` |
| `the sun rose and the birds started singing` | `light and music` |
| `Count Dracula stood in the dark castle` | `dark stone` |
| `she wrote code all night and found the bug` | `woman write dark and see` |
| `the old dog stretched by the fireplace and fell asleep` | `old animal and down` |
| `trees dreamed working walked loved walking` | `go` |
| `music song miss wish street little decide way` | `music hear miss want outside child think path` |
| `love unknown unknown love` | `love` |
| `вода огонь любовь silence` | empty |

The current README's 2,000+ entries/morphological fallback description and illustrative phrase outputs describe a richer compressor than this C header. The header itself calls its map a subset of `semantic_tokenizer.js`. This matters for importing the intended coverage: use the actual.life successor or deliberately extend the map instead of assuming the C subset has those branches.

`AI` and `BE` are uppercase canonical names, while `semtok_line` lowercases input: direct uppercase spelling is lost in that path. `computer`/`machine` map to `AI`, and `is`/`being` map to `BE`. The ring's interactive `tokenize_prompt` path is separate: it accepts exact glyph names without semantic compression (`cavellman.c:713–723`). Every shipped `.vocab` matches all 88 canonical IDs exactly.

## Surface memory and symbol birth

`cooccur_update` counts both directions for all pairs within distance three, increments an interaction count, then divides every count by the global maximum count (minimum denominator one). It is a symmetric association surface, not a direction-sensitive history. Repeated identical glyphs also increase their diagonal twice for a pair.

`cooccur_decay` multiplies each strength by a factor and stochastically decrements nonzero raw counts with probability `1−factor`. Runtime active generation calls it with `.999`; passive feed updates do not invoke that decay branch. The two directions are decremented independently by the decay loop, so they can differ until subsequent updates/normalization.

`try_emerge_symbol` selects the strongest unused unordered **base-glyph** pair. The threshold is

`max(.40, .75 × (1 − .5 × exp(−interactions/200)))`.

It appends one name and metadata record (`parents`, birth strength, birth interaction, uses, alive, depth). Limits are 128 emerged entries and 256 total vocabulary slots. `DISCIPLINE_WINDOW=50` is declared, but no current runtime condition reads it. The birth probe produced `fire+fear` on the **first** interaction. Because both candidate parents come from `0..base_size−1`, this implementation's depth calculation remains one; it cannot construct recursive composites through that candidate loop.

At age 500, an active symbol survives if current parent strength is at least `.75×.7=.525`, or its usage count is at least five. Otherwise `alive` becomes zero. The source contains a depth-five freeze branch, which the current base-only candidate loop does not reach.

Executed probe:

```text
first pair count=1 strength=1.000000 interactions=1
first birth id=88 bos=88 mask=89 n_emerged=1 depth=1
at 501 interactions alive=1 depth=1 strength=1.000000
after strength=.52 alive=0
```

The symbol metadata is a useful record of reinforced concepts. To turn a composite into a real token space, the Haiku port should use the improved actually.life mechanism described by that parallel audit: separate IDs after reserved tokens, recursive parents, and explicit embedding/head integration when weights are present.

## Pressure measured without any weights

`field_hear` reads the existing pair strength, using `.5` for the first token. Surprise is `1−strength`; novelty is one for base glyphs or 1.5 for IDs≥88. Excitement receives `.35×surprise×novelty` and clips at 2.5. For strength below `.15`, dissonance receives `.15−strength` and clips at one. Novelty/resonance mass counters also grow. The listener updates symmetric raw pair counts; normalization waits for `cooccur_update`.

The gate is strict: dissonance `>.40` or excitement `>coherence_floor`. After speech, excitement becomes zero, dissonance halves, and speech count increments. Per tick, excitement decays by `.94`, dissonance by `.90`. Once total count reaches 40, a speaking ratio above `.70` raises the gate by `.005` and a ratio below `.20` lowers it; clamp is baseline±.30.

Probe on an empty field with floor `.60`:

| Heard glyph | Excitement | Dissonance | Gate |
| --- | ---: | ---: | --- |
| before | .175 | 0 | closed |
| dream | .525 | .15 | closed |
| after | .875 | .30 | open |
| fear | 1.225 | .45 | open |

After speaking and one decay, excitement was zero and dissonance `.2025`. Immediately after hearing, raw first-pair count was one while normalized strength stayed zero; after `cooccur_update`, count became two and strength one. This ordering is observable and should be explicit in any port.

For Haiku, this can be pressure on *internal* word admission or dream/symbol activity, fed by its existing dissonance/novelty/quality event. A tick-based autonomous colony and forced public silence are not prerequisites for retaining this useful mechanism.

## Weights and online updates

The transformer is loaded from trained NoTorch-format files. Both founders use four layers, E=96, eight heads, FFN=384, context128. Parsed binary dimensions agree with their JSON receipts:

| File | File bytes | Float parameters | Tensors | Embedding/head |
| --- | ---: | ---: | ---: | --- |
| `cavellman_A.bin` | 1,890,836 | 472,608 | 36 | `[89,96]` |
| `cavellman_B.bin` | 1,890,836 | 472,608 | 36 | `[89,96]` |
| `cavellman_medium.bin` | 3,307,412 | 826,752 | 36 | `[89,128]` |

Weights A SHA256: `7ff49654be4b76faea612c5c610ed38d59bf759feb111c32fe02ed316049758c`.
Weights B SHA256: `bcd549c6e560c60aaa051f28408ab9f5ac9dd1614a66dbcc600785a522013a40`.
The files' 89 rows represent 88 glyphs plus BOS. The engine also defines MASK as ID89, but there is no trained MASK row in those files. First emerged ID88 coincides with BOS; later emerged IDs have no trained rows. `model_forward` allocates zero-filled logits beyond the actual head, while generation samples `vocab_size+2` IDs. Thus importing the current emerging-vocabulary array verbatim would import this ID/layout behavior; choose separate internal symbol IDs in Haiku.

The live ring generates autoregressively with KV cache, RMSNorm, Q/K/V, SiLU FFN and top-p selection. The diffusion experiment is a separate trainer/file and is not the ring's active generator.

Hebbian adapters are rank four, applied to Q and V. The active path updates Q and V; passive reading scales signal by `.3` and updates V only. However, **both factors are allocated with calloc** (`cavellman.c:388–392`), and the vendored update is multiplicative in the other factor (`ariannamethod/notorch.c:2683+`). An executed 100-step probe starting from zero factors left their combined L1 norm exactly zero. The separate co-occurrence field does change; mass-threshold CPT also updates the full loaded weights. Any optional Haiku adapter should initialize a nonzero factor or use the appropriate working canonical NoTorch update. No upstream changes were made in this research task.

## Minimal recommendation to the parent integration

1. Keep the base English compressor/alias table inspectable in AML; retain original lexical tokens and their provenance alongside compressed IDs.
2. Use the stronger actual.life directional field and recursive symbol allocation as the concrete next-generation implementation. caveLLMan supplies a compact example of how accumulated surprise becomes internal pressure.
3. Let Penelope provide optional lexical material; let Janus traverse alternative concept directions; use glyph attraction/pressure to rank or admit those proposals into Haiku's existing cloud and dreaming organs. This keeps the outer 5–7–5 selector authoritative.
4. Persist glyph state, symbol definitions, admission events, and any independent random stream through Haiku's existing checkpoint transaction. Missing optional lexical weights can leave the cloud/glyph process active.
5. For the first implementation, test the effect of the concept field on actual word admission and later haiku, with a trace of the compressed path and admissions. The fixed number of internal steps should belong to the explicit Penelope/Janus design, not to a property of caveLLMan: this engine has no twelve-step glyph constraint.

The actual.life comparison above comes from the parallel agent's source audit at `2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8`; the parent should use that audit for its exact references.

## Reproduction

Scratch probe sources:

- `<research-workspace>/cavellman-iceberg-probe.c`
- `<research-workspace>/cavellman-surface-probe.c`

Commands from the shared workspace:

```sh
cc -std=c11 -Wall -Wextra -O2 cavellman-iceberg-probe.c -o cavellman-iceberg-probe
./cavellman-iceberg-probe
cc -std=c11 -D_GNU_SOURCE -O1 -ffunction-sections -fdata-sections -IcaveLLMan/ariannamethod cavellman-surface-probe.c caveLLMan/ariannamethod/notorch.c -Wl,--gc-sections -lm -pthread -o cavellman-surface-probe
./cavellman-surface-probe
```

Both probes exited zero. Compilation emitted warnings from the included original C source; no full colony, autonomous learner, or long training run was launched. No test suite is tracked in this repository. `git status --short` remained empty.


This subaudit preceded Oleg's further choice of parameter mixing inside each
presence. For the combined design and that refinement,
see [The Haiku iceberg](../../ICEBERG.md). Portable probes are in
[the research bundle](../../../tests/research/iceberg/README.md).
