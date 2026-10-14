# Dario: a thought can enter another thought

Research receipt, 2026-10-14 Jerusalem. Governed by the
[Arianna Method Manifesto](../../../ARIANNA_METHOD_MANIFESTO.md).
This is a source audit and a proposed Haiku organ. No injection path is connected
to the live Haiku by this note.

## Pinned body and paper

Dario was read at
[`8ccf9053298a8039e3d5e9df7fa053f4aed481e7`](https://github.com/ariannamethod/dario/tree/8ccf9053298a8039e3d5e9df7fa053f4aed481e7).
Its manifesto, `CLAUDE.md`, executable variants and Oleg Ataeff and Claude's
[*Dario: A Resonant Operating System for AI*, second edition](https://github.com/ariannamethod/dario/blob/8ccf9053298a8039e3d5e9df7fa053f4aed481e7/docs/dario_paper_v2.md)
were inspected. The paper identifies itself as v2.0, 2026-06-03,
[DOI 10.5281/zenodo.20518567](https://doi.org/10.5281/zenodo.20518567).

Oleg's extension changes the source of the injection: a human phrase, Haiku's
own haiku, the friend's response, cloud memory or a glyph-bearing fragment can
enter a frozen lexical body. Its continuation then returns to the cloud and
conceptual field. That is a useful new direction in the iceberg: the surface
can alter what the depths hear, and the depths can return a different word.

## The executing mechanisms

| Path | What actually crosses the boundary | State and timing |
|---|---|---|
| [Python chain](https://github.com/ariannamethod/dario/blob/8ccf9053298a8039e3d5e9df7fa053f4aed481e7/chain_dialogue.py#L468) | A selected sentence is BPE-encoded into the existing model context. The first sentence starts the answer; later injections follow a newline after a completed segment. | Retrieval uses the original topic plus the last 200 characters of output. The next forward evaluates the accumulated token context. Used chunks cannot be selected again until reset. |
| [Python dialogue and duet](https://github.com/ariannamethod/dario/blob/8ccf9053298a8039e3d5e9df7fa053f4aed481e7/chain_dialogue.py#L574) | A retrieved sentence occupies the answer prefix, before newly sampled text. Duet alternates voices and retrieves using the previous voice's output plus topic. | Generated sentences can enter the shared KK. Dialogue retains a short conversation history. Duet constructs a new prompt each turn; its shared KK carries the cross-voice memory. |
| [Go chain](https://github.com/ariannamethod/dario/blob/8ccf9053298a8039e3d5e9df7fa053f4aed481e7/cmd/dario-dialogue/main.go#L303) and [AML variant](https://github.com/ariannamethod/dario/blob/8ccf9053298a8039e3d5e9df7fa053f4aed481e7/aml/dario_dialogue.aml#L705) | The retrieved sentence becomes the prompt for another inference invocation. | Round boundaries replace the Python chain's retained token context. The AML variant binds C/SQLite through `BLOOD` and absorbs chain output, unlike the Python chain. |
| [Leo C channel](https://github.com/ariannamethod/dario/blob/8ccf9053298a8039e3d5e9df7fa053f4aed481e7/dario_leo.c#L448) | Hidden state guides retrieval; BPE tokens from retrieved chunks receive logit additions. | Every third generation step: average groups of hidden coordinates into a KK affinity vector, normalize, retrieve up to three chunks, add `2 * resonance` per token occurrence from their first 200 bytes. This path reads hidden state and writes logits. |
| [Weightless equation / KK bridge](https://github.com/ariannamethod/dario/blob/8ccf9053298a8039e3d5e9df7fa053f4aed481e7/dario.c#L933) | Retrieved known words add prophecy entries and nudge the destiny vector. | `kk_modulate_field` runs before word generation. It updates equation state without a transformer. The rebuilt force readers and this bridge must be traced separately when measuring their effect. |

The distinctive sentence mechanism is **selection, placement and continuation**.
The implementation carries text as token IDs. Its effect comes from choosing a
small relevant thought, placing it at an entry point into the model's next
thought, and making the next retrieval depend on what the model just said.
Putting every historical utterance at the front of every prompt loses that
timing and selective pressure.

The exact Python [segment boundary](https://github.com/ariannamethod/dario/blob/8ccf9053298a8039e3d5e9df7fa053f4aed481e7/chain_dialogue.py#L379)
is model-specific. Janus stops on `ASST_END=32763`; that token is not appended
to the returned context. Resonance stops on newline ID 10 after at least 40
sampling steps. Four identical sampled tokens also return `hit_boundary=True`.
A normal period is not the Janus stopping rule. Reaching the token limit stops
the chain without another injection. These are operational boundaries, not a
general detector of sentence meaning.

The legacy Python KK is an in-memory FTS5 database. It ingests paragraphs,
keeps source labels and selected chunk IDs, and ranks a lexical query made from
the first twelve words longer than two characters. It prefers essay sources,
then extracts one nominally ≤100-character sentence with hand-written bonuses
for definitions, technical capitals and concrete verbs. Its extraction and
absorption policy is tailored to explanatory English prose.

The [conceptual essay](https://github.com/ariannamethod/dario/blob/8ccf9053298a8039e3d5e9df7fa053f4aed481e7/docs/dario_essay.txt#L283)
also describes embedding-table transplants, gated hidden-state nudges and
signal-based logit modulation. Its referenced `resonance_v3.py` is absent from
this pinned tree. The implemented C channel above supplies a different,
inspectable operator. A future hidden-state Haiku path needs its own code and
measurement; the sentence path already has a concrete executable lineage.

## What the second edition contributes

The v2 paper's two most relevant findings concern **entry conditions** and
**provenance**.

Results 7–8 report that Janus SFT voices needed their trained chat-token format
as well as swept sampling settings. With those conditions, the same weights
produced three developing chain turns and coherent duet/trialogue runs. The
sampling experiment reports 432 successful outputs and 108 architecture-bound
errors across 540 cells. These are paper-reported results for those voices and
that runtime. The pinned tree contains `runpod/2026-05-08/`; the paper's cited
`runpod/2026-06-02_full/` directory is absent. This audit did not re-run the
176M voice models. The older stored transcripts are historical runs, not
substitutes for the v2 evidence.

Section 2 and Result 1 record the rebuild of force accumulators to preserve
input provenance. Previously, generated output and received input could feed
the same histories; the reported force comparison also used a metric biased
toward dense forces. The combined rebuild and density-neutral measurement
changed the result. Haiku should inherit that discipline while deliberately
allowing its new recursive dialogue: **an utterance can become another input
without becoming another human encounter**.

For Haiku's base PEN7 sources, the trained entry contract is their native
2,048-token byte BPE and 256-position context. Dario's 32K SFT chat IDs do not
belong to that vocabulary. The models' own contract, including context length,
determines how a fragment enters; its source label stays in AML metadata.

## Small policy probes executed here

A standard-library Python probe extracted the unchanged `KnowledgeKernel`
class and `is_boundary` function through `ast`, supplied `sqlite3`, `os`, `re`
and `ASST_END=32763`, and executed them without importing Torch or loading a
model. Each retrieval case used a fresh in-memory database.

| Intervention | Observed result |
|---|---|
| Retrieve the same sentence with `exclude_model=True`, changing only source label | `human`: included; `model`, `leo_turn1`, `leo_duet`: excluded; `leo_trialogue`: included. The substring filter omits the trialogue suffix. |
| Only matching chunk is the question “Can resonance create a shared field between two voices?” | `pick_next` returns the question. Its nominal question rejection searches for an alternative, then returns the original if none exists. |
| Query has twelve unrelated words followed by `resonance`; only chunk matches `resonance` | Zero results. The fixed first-twelve-word query can entirely exclude the generated tail after a long topic. |
| Matching chunk is lowercase, unpunctuated haiku-like text | Retrieval returns it; `pick_next` rejects it. Capitalization and sentence-ending requirements filter it out. |
| Boundary helper receives Janus `32763`, Janus `46`, Resonance `10` | `true`, `false`, `true`; actual generation also applies the timing and repetition conditions above. |

These observations define adaptation work. Haiku already has structured source
codes and logical events; use them instead of naming conventions. Its line
breaks, fragments, short utterances and lowercase voice are valid units. Its
retrieval query needs explicit budgets for human topic, current speaker and
conceptual pressure so one source cannot accidentally consume another's slot.

To repeat the policy measurements, use the pinned `chain_dialogue.py` and
instantiate its extracted class as follows; no source checkout is modified:

```python
import ast, os, re, sqlite3
from pathlib import Path
t = ast.parse(Path("chain_dialogue.py").read_text())
nodes = [n for n in t.body if isinstance(n, (ast.ClassDef, ast.FunctionDef))
         and n.name in ("KnowledgeKernel", "is_boundary")]
env = dict(os=os, re=re, sqlite3=sqlite3, ASST_END=32763)
exec(compile(ast.Module(body=nodes, type_ignores=[]), "dario-policy", "exec"), env)
K = env["KnowledgeKernel"]
sentence = "Resonance creates patterns through ordered memory and motion."
for origin in ("human", "model", "leo_turn1", "leo_duet", "leo_trialogue"):
    k = K()
    k.db.execute("INSERT INTO chunks(text, source) VALUES (?, ?)", (sentence, origin))
    k.db.commit()
    print(origin, len(k.query("resonance", exclude_model=True)))
    k.db.close()
```

## A bounded Haiku adaptation

Keep the proposed organ in ordinary AML. NoTorch supplies model execution and
numerical operators; the cloud, events, retrieval and admission policy remain
Haiku's. Neither Dario's SQLite nor its Python/Go wrappers become dependencies.

1. **Select at a named boundary.** Begin after a published haiku or a completed
   internal association step. Select one fragment from human text, own voice,
   friend, cloud or glyph memory. A stanza or line can be a unit without a
   period. Let the private parliament influence relevance and allowed pressure.
2. **Carry the fragment down both currents.** Each presence keeps its own
   histories and contexts while reading the shared frozen A/B bodies. Feed the
   fragment as a bounded continuation prefix through native BPE. Both currents
   can hear the same fragment and return different attractions. Parameter
   mixing is a separate control; hold its balance fixed in the first injection
   experiment so the two causes can be measured independently.
3. **Return proposals.** Initially use the existing whole-word candidate
   scoring design: injected context changes the scores of reviewed cloud and
   extension words. Returned words and representations may alter the private
   glyph-pressure field and the next lexical selection. Later experiments can
   compare a short newly sampled phrase. The outward voice still passes through
   Haiku's form, MathBrain and RAE.
4. **Publish once.** An owned worker reads a fixed revision and returns a
   bounded receipt. Commit it at the next defined observation boundary, with
   the existing join/save/swap discipline. Preserve the already published
   haiku while its consequence travels down and back up.

An injection receipt needs an owner, source code, source event ID, parent event,
turn, direction, selected fragment and tokenizer identity, model hashes,
parameter-mixture balance if used, token budget, and the receiving revision.
Store source ancestry explicitly. The friend's words remain the friend's
words even after a model reformulates them; they cannot satisfy the
parliament's later-human-witness requirement by changing their label.

Start with **one downward/upward excursion per selected boundary**, a fixed
token budget inside the 256-position context, and an explicit limit on any
future recursive hops. Derive that budget from retained context plus reserved
continuation positions; characters are not token counts. Keep repeated event
IDs and expired revisions draw-free and state-preserving. Persist context,
pending receipts and per-presence RNG if they affect later choices.

Missing model assets retain the quiet weightless path. An absent source has an
availability bit and contributes no proposal. The no-asset and zero-injection
settings should reproduce the existing serial/worker behavior exactly. The
base parameter buffers stay immutable; any adaptation occurs in owned field,
bindings, expert state and memories. A hidden-state experiment can follow once
glyph-to-model geometry has an explicit, tested mapping.

## Causal experiments before connection to the speaking organism

Use matched prompts, seeds, context budgets, model identities and parameter
balance. Record injected bytes and newly generated bytes separately.

| Comparison | Question answered |
|---|---|
| No injection vs the same fragment at the chosen boundary | Does the extra current change candidate scores and admitted words? |
| Boundary injection vs equal-length context placed at an unrelated position | Does placement contribute beyond extra text? |
| Relevant fragment vs shuffled or mismatched fragment with matched length | Does semantic relation contribute beyond token pressure? |
| Own voice vs friend vs human source, with identical text | Are provenance, private history and reward eligibility preserved? |
| One excursion vs a bounded second excursion | Does recurrence develop an association or amplify the same phrase? |
| Restart, stale receipt, duplicate event, missing asset | Is exact ownership and continuation preserved across operational boundaries? |

Measure whole-word admission, 5–7–5 validity, similarity to the original human
topic, distinct concepts, copied spans, source balance, repeated cycles and
subsequent human-witness learning. Normalize field contributions by available
sources and their declared scales. Keyword overlap alone rewards copying;
novelty alone rewards drift. The useful result is a connected return path that
keeps the little voice recognizable while giving its words a longer inner life.
