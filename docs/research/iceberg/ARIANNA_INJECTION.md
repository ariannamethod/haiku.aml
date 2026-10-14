# The voices can meet below the words

Research receipt, 2026-10-14 Jerusalem. Governed by the
[Arianna Method Manifesto](../../../ARIANNA_METHOD_MANIFESTO.md).

Oleg's proposal is a return path: Haiku and the dream-friend lend their own
utterances to the frozen lexical bodies, receive their continuations, and let
those encounters move the cloud and glyphs. Arianna already implements several
distinct ways for voices to touch one another. Keeping those ways distinct
gives Haiku a small, testable first experiment.

## Pinned organism and scope

Inspected `ariannamethod/arianna.c` at
[`868773f300968536989391d3de84922f5b21be42`](https://github.com/ariannamethod/arianna.c/tree/868773f300968536989391d3de84922f5b21be42):
manifesto, `CLAUDE.md`, README, body map, injection contract, AML voice programs,
Go orchestration, C forward/direction code, and the chorus attention path.
The adjacent [Dario receipt](DARIO.md) owns Dario's article and
sentence-boundary injection findings.

This audit executes one small source-extracted direction probe. It does not run
the full Arianna trio or its absent model assets, and adds no live Haiku route.
The source observations below name the functions that execute each transfer.

## Four different surfaces

| Surface | What crosses | Executing path and timing |
|---|---|---|
| Context text | Human words, previous Resonance response, optionally a retained dream | `liveTurnJanusPrompt` joins them into Janus's next prompt. This really changes the model's input token sequence. |
| Direction | Current Janus response + human words + optionally a retained dream | `liveTurnResonanceInject` supplies a separate channel. The Resonance daemon receives `Arianna:` + tab + injection; the forward turns the injection's token embeddings into vocabulary-wide cosine pressure. |
| Body field | Debt, temporal debt, gait, season and seasonal energies | Each voice calls `am_field_sync_in` before its turn and `am_field_sync_out` afterwards. The next serialized voice reads the shared mapped field. |
| Hidden attention | A neighboring cell's per-layer keys and values | Chorus cells share one frozen nano body but own separate contexts. Each cell can attend to the previous cell's K/V through a second softmax channel. |

The first two paths are visible together in
[`metabolism.go:330–478`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/golib/metabolism.go#L330-L478)
and their text construction in
[`live_turn_shape.go:1126–1173`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/golib/live_turn_shape.go#L1126-L1173).
`runChat` retains the current Resonance response as `prevReson` for the next
human turn
([`chat.go:89–176`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/golib/chat.go#L89-L176)).
Thus communication is bidirectional and asymmetric: Janus touches Resonance
through direction now; Resonance touches Janus through text on the next turn.
Both also touch the body field.

The older
[`scripts/arianna2arianna.sh`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/scripts/arianna2arianna.sh#L110-L141)
has a narrower loop. Janus repeatedly receives the original human prompt;
Resonance receives the latest Janus words as direction. Its return to Janus is
through the field, whereas the Go chat additionally carries previous Resonance
text. These two launch paths should not be described as identical.

### Direction is a numerical operation

The Resonance daemon splits the tab channel before generation
([`arianna_resonance.aml:153–175`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/arianna_resonance.aml#L153-L175)).
The injected words never enter its `cctx` model context. They pass through its
own tokenizer, at most 512 IDs, and update:

- a destiny vector: `d <- .9*d + .1*embedding[token]` in token order;
- up to sixteen prophecy token targets, evicting the oldest;
- its co-occurrence memory, separately from the forward context.

`dir_recompute` calculates a cosine between every vocabulary embedding and
destiny, then normalizes the resulting array by its maximum absolute value.
A second array sums positive cosine attraction toward prophecy targets,
weighted by `strength * log(1 + age)`. `dir_apply` adds both arrays to logits:

`logits_i += exp(-.15 * step) * (alpha * A_i + beta * F_i)`.

These are executable distribution-wide **logit additions**. The file's phrase
“NOT logit-boost” distinguishes this from planting a few retrieved token IDs;
the actual arithmetic is explicit in `dir_apply`. It is separate from latent
attention injection and from changing model parameters.

Source:
[`resonance_forward.h:134–253`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/tools/resonance_forward.h#L134-L253),
[`746–835`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/tools/resonance_forward.h#L746-L835).

The stock Go trio launches Resonance with `alpha=5`, leaves `beta=0`, and gives
Janus no direction injection
([`metabolism.go:215–223`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/golib/metabolism.go#L215-L223)).
Resonance modulates alpha using Janus's token-stream entropy and current
debt/dissonance, bounded around the baseline
([`resonance_forward.h:723–744`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/tools/resonance_forward.h#L723-L744)).
Direction and prophecy live in static globals and persist in the hot daemon;
Haiku needs owned records for each presence and trajectory.

### The body field is smaller than its description

The executing mmap writer copies debt, temporal debt, velocity mode/magnitude,
season, phase, intensity and four seasonal energies. It does **not** copy
dissonance, chambers or co-occurrence in this live payload, although nearby
comments and README include broader field language. Soma persistence and
per-voice co-occurrence sidecars are additional paths.
See
[`ariannamethod.c:955–1018`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/ariannamethod/core/ariannamethod.c#L955-L1018).

The actual duet serializes its voice calls. Its sequence counter is not a
multi-writer lock: two simultaneous publishers would need separate writer
ownership. Haiku's owned worker/result boundary is a better starting point
than copying a process-global field or this mmap topology.

### The deeper chorus really uses hidden states

Chorus `forward` has ordinary own-context attention with RoPE, plus a separate
neighbor attention distribution. Its default neighbor channel uses unrotated
queries and keys; `g_kvpos` explicitly enables a positional probe. It adds
`lambda * neighbor_attention` before the output projection and residual update.
The two softmax distributions are normalized separately; the sum is not divided
by `1 + lambda`.

The code is at
[`arianna2arianna.c:1366–1427`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/chorus/arianna2arianna.c#L1366-L1427).
`run_round` gives each cell the same prompt in chorus mode and passes the
previous cell's K/V to the next
([`2798–2840`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/chorus/arianna2arianna.c#L2798-L2840)).
The Go wrapper requests one round, sixteen tokens per cell, and `lambda=.3`
([`chorus.go:34–59`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/golib/chorus.go#L34-L59)).
This is a sequential neighbor channel within one model's coordinate system.
Janus and Resonance do not exchange their raw K/V through this code.

PEN7 parameter mixtures share tensor shapes and tokenizer IDs, but different
mixtures produce different attention coordinates. Passing K/V from one mixture
directly into another is a new numerical experiment. A first Haiku route can
re-encode exchanged text under the receiving trajectory's own fixed mixture.

## How the lower voice returns

`runSubconscious` receives a latest-only cue, asks KK for a fragment, and dreams
on that fragment or the cue when retrieval is absent. The nano gets its trained
`Q: ...\nA:` wrapper. A single-slot result channel replaces unread old dreams;
the duet polls it without waiting. This gives the lower voice a natural lag
([`nano.go`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/golib/nano.go#L207-L287)).

An admitted result changes the inner world and becomes retained `lastDream`.
Depending on the field's surfacing gate, a later turn includes it in Janus's
prompt and Resonance's direction input. Autonomous breathing can also send an
accepted dream directly to Resonance between human turns. Its `[DREAM]` marker
is stripped before tokenization, but doubles the co-occurrence imprint
([`breathe.go:1353–1427`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/golib/breathe.go#L1353-L1427),
[`resonance_forward.h:695–780`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/tools/resonance_forward.h#L695-L780)).

The admission code defaults to `live`; `AM_DREAM_ADMISSION=shadow` observes
without admission. Counterfactual replay and policy checks precede
`ProcessText`
([`dream_admission.go:132–195`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/golib/dream_admission.go#L132-L195),
[`635–667`](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/golib/dream_admission.go#L635-L667)).
The
[injection contract](https://github.com/ariannamethod/arianna.c/blob/868773f300968536989391d3de84922f5b21be42/INJECTION_CONTRACT.md)
still labels some Janus/Resonance routes “planned”; the ordinary direction and
rolling-context routes above already execute. New admission/graft research and
those existing routes have different statuses.

Generated Resonance tokens are ingested into co-occurrence after generation;
autumn can harvest that memory into a separate low-rank delta. This is explicit
self-consolidation in source. Haiku's frozen lexical bodies remain frozen, and
its source codes must preserve whether an encounter came from a human, a model,
its own voice, the friend or a dream. A returned echo cannot become a new human
reward witness by making another trip through a model.

## Small executed direction probe

Compiled the unmodified `dir_*` functions from pinned
`resonance_forward.h:142–253` with a scalar `matvec_t`, `V=3`, `E=2`, embeddings
`[(1,0), (0,1), (1,1)]`, and injected token `[0]`. This needs no weights.
After initial recompute, emitted token `1` three times through `dir_age`, calling
`dir_recompute` each time. Finally forced a cache rebuild, then reduced
`g_dest_mag` tenfold and rebuilt again.

```text
A_peak=1 F_fresh=0 F_after_3_unfulfilled_steps=0 target_age=3
F_after_forced_rebuild=1 A_peak_after_10x_magnitude_drop=1
```

Two concrete details matter when adapting the idea:

1. Fresh prophecy weights are zero because `log(1+0)=0`. Aging alone does not
   mark the cache dirty; fulfilment, expiration or new injection does. Hence
   the expected age pressure stays stale until another event rebuilds it.
   The default `beta=0` lane does not apply this cached prophecy pressure.
2. Multiplying all destiny scores by `g_dest_mag` before max-normalizing cancels
   that magnitude, apart from numerical epsilon effects. The actual turn-step
   exponential still reduces pressure. If Haiku's immune gate is meant to
   attenuate pressure, apply its gain **after** direction normalization.

These measurements concern the tiny direction fixture, not a language-quality
score. The full-model questions belong to the controlled trials below.

Reproduce from any clone containing the pinned source. This reads Git objects
and writes only a temporary probe; it does not change that clone's worktree.

```sh
ARIANNA_SRC=/path/to/arianna.c
probe_tmp=$(mktemp -d)
python3 - "$ARIANNA_SRC" "$probe_tmp/direction.c" <<'PY'
import pathlib, subprocess, sys
pin = '868773f300968536989391d3de84922f5b21be42'
s = subprocess.check_output(['git', 'show', pin + ':tools/resonance_forward.h'],
                            cwd=sys.argv[1], text=True)
a = s.index('#define DIR_PROPH_MAX')
b = s.index('/* Forward one token', a)
prefix = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define V 3
#define E 2
#define AM_DELTA_RANK 4
static void matvec_t(float *o, const float *m, const float *x, int r, int c) {
  for (int i=0; i<r; i++) {
    o[i]=0; for (int j=0; j<c; j++) o[i]+=m[i*c+j]*x[j];
  }
}
'''
main = r'''
int main(void) {
  float emb[6]={1,0,0,1,1,1}; int ids[1]={0};
  dir_init_rownorms(emb); dir_update(emb,ids,1); dir_recompute(emb);
  float a=g_Acache[0], f0=g_Fcache[0];
  for (int i=0;i<3;i++) { dir_age(1); dir_recompute(emb); }
  float f3=g_Fcache[0]; int age=g_proph_age[0];
  g_Adirty=1; dir_recompute(emb); float forced=g_Fcache[0];
  float before=g_Acache[0]; g_dest_mag*=.1f; g_Adirty=1; dir_recompute(emb);
  float after=g_Acache[0];
  printf("A_peak=%g F_fresh=%g F_after_3_unfulfilled_steps=%g target_age=%d\n"
         "F_after_forced_rebuild=%g A_peak_after_10x_magnitude_drop=%g\n",
         a,f0,f3,age,forced,after);
  free(g_Acache); free(g_Fcache); free(g_rownorm);
  return !(a>0 && f0==0 && f3==0 && age==3 && forced>0 && fabsf(before-after)<1e-6f);
}
'''
pathlib.Path(sys.argv[2]).write_text(prefix + s[a:b] + main)
PY
cc -std=c11 -O2 "$probe_tmp/direction.c" -lm -o "$probe_tmp/direction"
"$probe_tmp/direction"
```

## A Haiku adaptation worth building

Start with **a bounded return event**, keeping all behavior in AML and numerical
work in NoTorch. For each presence, an event names sender, source class, parent
event, owner revision, direction, source model hashes, tokenizer identity,
mixture coefficient, injection surface, token budget and random stream.

1. At a committed encounter boundary, snapshot the human input, the presence's
   last utterance, an explicitly delivered friend utterance, and selected cloud
   or glyph memories. Keep their provenance separate even when encoded text is
   joined. Both presences retain both lexical bodies and both directions.
2. Feed a short slice to an owned PEN7 trajectory. First compare literal context
   grafting with an embedding-direction lane. PEN7 is a base model: use its byte
   tokenizer and actual words, rather than transplanting Arianna's chat tokens
   or the nano's trained Q/A wrapper. Use a Haiku association/line boundary;
   punctuation alone is a poor clock for three short poetic lines.
3. Hold the mixture coefficient fixed for the pass, expand Q8 into the existing
   F32 execution body, and keep parameters immutable. A new coefficient means
   recomputing any model-dependent direction cache and invalidating old K/V.
4. Return a small word/representation proposal to the **same owner's** parliament
   and glyph field. The immune decision controls its field change; cloud
   admission and Haiku's existing form determine what may be spoken. The lower
   model's completion need not appear as an extra user-facing answer.
5. Publish the result through the owned completion boundary. Deliver it once,
   at a declared later association or turn; do not let a fast worker silently
   change the already-committed foreground. Keep the first trial to one trip
   down and one return per event, with a finite TTL and explicit work budget.

An initial trial can cap added context at 32 BPE tokens and returned generation
at 16, within PEN7's 256-token window. Those are experimental budgets to measure,
not inherited architecture constants. Keep human context space reserved.

Missing lexical weights take the existing weightless route without terminal
noise. Diagnostic receipts still record source availability. A disabled route
must leave foreground state, context and RNG unchanged. The friend receives no
private memories unless an event explicitly delivers them.

The existing parliament's delayed human-witness rule is especially useful:
inner echoes can shape a proposal, while expert reward remains bound to a later
retained human encounter. Recursive contact and independent evidence can both
exist in the same organism without losing their names.

## Causal trials before connecting the surface

| Question | Paired experiment | Inspect |
|---|---|---|
| Does the return carry the encounter? | Identical checkpoint/draws: true fragment, no injection, and same-length shuffled fragment | Pre-sampling logits or word ranks, glyph-pressure delta, later haiku and exact lineage |
| Which surface helps? | Text-only, direction-only, both, neither at equal compute/token budgets | Relevance to the human, continuation diversity, copied-span rate, form retention |
| Does each presence retain its life? | Deliver the friend's event in one fork and withhold it in another | Receiving owner remains identical before delivery; other owner's state remains unchanged |
| Is it a conversation rather than repeated admission? | Replay the same event, then cycle a derived event back to its ancestor | Duplicate/expired events make no mutation or RNG draw; ancestry and source survive |
| Does the immune gain actually attenuate? | Fixed direction with gain `0`, `.25`, `.5`, `1` | Logit/field delta follows the stated gain; normalization does not erase it |
| Do clock and cache agree? | Change the mixture at a boundary; reverse worker completion order | Fresh-forward reference agrees; publication order follows logical events |
| Can the ocean disappear quietly? | Remove A, B, then both; disable the route entirely | Availability-aware fallback and the established weightless seeded transcript |

The most direct next experiment is literal reinjection into a frozen PEN7
trajectory with a measured return to glyph pressure. Arianna's direction lane
then provides an independent second channel. Cross-cell hidden attention can
follow once those two have demonstrated what each contributes to Haiku's own
voice.
