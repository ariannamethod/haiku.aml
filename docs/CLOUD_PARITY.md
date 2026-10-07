# The cloud remembers an occurrence

2026-10-07. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

`src/cloud.aml` and `src/memory.aml` carry the in-memory word cloud,
observer trigram storage, Markov transitions, generator vocabulary, and recent
memory. The reference is Python HAiKU at
[`abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`](https://github.com/ariannamethod/harmonix/tree/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku).
The tested toolchain is [AML PR #29](https://github.com/ariannamethod/ariannamethod.ai/pull/29),
`7411864d699f88e3dff3367f84611b0ff5d85133`: ordered numeric maps,
structural list keys, assertions, and finite scalar `floor`.

## State and ownership

Every column below is a distinct caller-owned container. AML assignment copies
containers; passing the same variable as two mutable columns violates this
contract. One execution owner applies an operation before another owner reads
the resulting state. Concurrent scheduling and durable snapshots are later
organs.

| State | Representation |
|---|---|
| Words | `weights`, `frequency`, `last_used`: maps from exact UTF-8 word to float; `sources`: strings aligned with insertion order of `map_keys(weights)` |
| Observer trigrams | Unique flat `rows` list, three strings per row; `counts` and `resonance` maps keyed by the structural encoding of each triple |
| Generator transitions | A separate unique flat `rows` list and `counts` map, plus a unique `vocab` list |
| Recent memory | A separate flat list containing the last ten ingested generator triples, retaining duplicates |

The columns preserve Python's `added_by` strings: seed words stay `seed`
when used; new user words start as `user`. An inserted ring word can retain
`overthinking:echo`. Observer resonance starts at 0.5 and survives repeated
observations unchanged.

Keys use AML's `list_key(list_slice(triples, offset, offset + 3))`. The
encoding records the element count and every UTF-8 **byte length**, followed
by the exact bytes. Spaces, pipes, colons, quotes, empty strings, newlines,
combining marks, Cyrillic, Hebrew, and emoji remain distinct. The raw triples
are also retained for successor queries and future rings.

Word and edge insertion order is explicit. Oracle SQL snapshots use
`ORDER BY id`; generator successor order comes from the original nested
dictionary. The AML vocabulary uses first occurrence order, while Python
stores a set. Fixtures compare exact vocabulary membership against that set
and assert the chosen AML order separately. Seeded random sampling remains
part of the generator migration.

## Operations

| Function | Contract and return |
|---|---|
| `haiku_cloud_add(weights, freq, used, sources, word, weight, clock, source)` | Insert if absent, frequency 0; return 1 if inserted, otherwise 0 |
| `haiku_cloud_seed(weights, freq, used, sources, seeds, clock)` | Seed only an empty cloud; ignore duplicates; return inserted word count |
| `haiku_cloud_morph(weights, freq, used, sources, active, clock)` | Process every occurrence, then decay every dormant word once; return cloud size |
| `haiku_store_trigrams(rows, counts, resonance, triples)` | Increment observer counts per occurrence; return number of new unique rows |
| `haiku_chain_learn(rows, counts, vocab, triples)` | Update only generator transitions and vocabulary; return total unique edge count |
| `haiku_chain_seed(rows, counts, vocab, seeds)` | Initialize fresh generator state from adjacent seed triples; retain even one- or two-word seed vocabularies; return edge count |
| `haiku_chain_update(rows, counts, vocab, recent, triples)` | Learn transitions and return a new last-ten-triple list; caller retains the returned list |
| `haiku_chain_count(counts, first, second, third)` | Exact transition count, or 0 when absent |
| `haiku_chain_next(rows, first, second)` | Successor strings in their original insertion order |

The other eight functions are validation/staging helpers. The two
`*_publish` helpers accept a validated continuation of the current state:
existing keys retain their order, values may change, and new rows append.
They are internal operations.

For an existing word, each occurrence multiplies weight by 1.1 and increments
frequency. The first occurrence of a new word starts at weight 1 and frequency
1. Thus a seeded `a` receiving `a a` becomes 1.21/frequency 2; a new word
received twice becomes 1.1/frequency 2. Each inactive word decays by 0.99 once
per nonempty event. There is no weight clamp.

Seed transitions do not populate recent memory or observer storage. A user
event with fewer than three supplied tokens updates the word cloud but adds
neither transitions nor generator vocabulary. Dream ingestion will use its
own explicit call to the generator operation.

## Validation and source decisions

Complete input triples and supplied record shapes are checked before
publication. Counter increments are staged across the whole batch, including
duplicates; cloud boosts and decay are staged too. The finite-map check rejects
weight overflow before any caller-owned container changes.

Counts are integer-valued floats in 0..2^24; stored transition counts start at
1. An increment beyond 2^24 fails. Clouds, generator vocabularies, unique edge
tables, and token/triple batches have an explicit 10,000-record bound matching
AML's loop budget. A flat triple batch therefore has at most 30,000 strings.
AML's structural-key bound is 1 MiB.

The caller supplies `clock` in finite, nonnegative relative seconds, at most
2^24. Integer ticks through that bound are exact; fractional seconds retain
float32 precision. The same event clock is stored for every active word.
The host-to-wall-clock mapping belongs to persistence; Unix epoch seconds do
not fit this contract.

Publication writes several independent maps/lists. An allocation failure
during publication can leave a partial update; these containers provide no
whole-record transaction or rollback. The regression host measures unchanged
state for the 38 rejected validation cases below.

Four source differences are explicit:

- `harmonix.py:193–219` fails on `morph_cloud([])`: the generated SQL has
  zero placeholders but receives one parameter. AML adopts the no-op in
  `async_harmonix.py:245–279`.
- `async_harmonix.py:218` saves user and system triples during observation.
  Synchronous Harmonix has a pure observer and a separate update operation;
  the AML operations preserve that separation.
- `chat.py:130–144` updates the chain before reading recent triples.
  `test_haiku_session.py:91–106` reads and generates before updating.
  The foreground fixture preserves the actual chat order and also measures
  the alternative observation before the update. A first `a b c` event
  gives dissonance 0 after ingestion and 0.5 before it. A later disjoint
  `x y z` gives 0.4025 after ingestion and 1 before it.
- `haiku.py:421–435,622–624` exposes the live recent list, extends it, then
  replaces it with a slice. A previously returned ten-triple list therefore
  grows to eleven. AML returns an independent snapshot and leaves the supplied
  recent list unchanged. The chat's later dream can no longer change the
  already-recorded observation context through this alias.

## Run and measured coverage

From the repository:

~~~sh
make -C ../ariannamethod.ai all
make test-cloud
../ariannamethod.ai/runner/aml examples/cloud.aml
~~~

The example runs three real memory updates. After the first event, `rain`
has weight 1.21 and its transition count is 2. The two-token second event
leaves five cloud words and three generator words. The third event creates a
second recent triple while the earlier snapshot retains its original triple.
Its observation is approximately `[0.322, 0.4, 1, 0.25]`.

`tests/reference/cloud_inputs.json` records nine cloud sequences, ten memory
sequences, and one four-event foreground trace. The full 587-entry corpus is
included verbatim. It yields 576 unique words, 584 bigram keys, 585 unique
transitions with total count 585, and empty recent memory on initialization.
Fixtures check complete ordered states after each operation, preserved
provenance/resonance, empty input, repeats, token-key collisions, all recent
window boundaries, separate observer/generator updates, and exact counter
increments up to 2^24.

Each expected numeric field comes from the pinned Python method and SQLite
state. Float tolerance is absolute 0.000002 with NaN rejection; integer
counter differences therefore fail. Lists compare every string exactly.
The compact English fixture chunks contain only known whitespace-free words.
All 20 main fixtures remain below 500 physical lines and use native imports;
the runtime also checks its 1,024-line expanded source bound.

The negative fixtures cover malformed columns/triples, provenance and clock
types, fractional/overflowing counts, duplicated increments, weight overflow
after an earlier new word, and record/batch bounds. A temporary C host invokes
the same AML programs and compares all eleven original containers bit for bit
after rejection. Haiku's operations and setup remain AML; C only inspects the
runtime-owned results. The ordinary suite runs no Python.

Verified with the interpreter, `amlc --scalar` from another working directory,
and the persistent host inspection:

~~~text
PASS: 5055 numerical and 247 ordered-list reference results in AML interpreter and compiled --scalar
PASS: 38 invalid updates rejected in both paths; all 11 state containers unchanged in host inspection
~~~

The script accepts `HAIKU_AML`, `HAIKU_AMLC`, `HAIKU_AML_LIB`,
`HAIKU_AML_INCLUDE`, and `CC` for a matching external toolchain.

## Reproduce the Python reference

Use the pinned checkout and its original Python dependencies in a separate
reference environment. Run the following from `haiku.aml`. It replays every
recorded operation through the original classes, including the actual chat
initializer and both synchronous/asynchronous empty-input behavior. Temporary
SQLite files and an absent MathBrain state path isolate each run.

~~~sh
PYTHONPATH=../reference-deps python - <<'PY'
import asyncio, contextlib, io, json, subprocess, sys, tempfile
from pathlib import Path
from unittest.mock import patch
source = Path("../harmonix").resolve()
data = json.loads(Path("tests/reference/cloud_inputs.json").read_text())
assert subprocess.check_output(["git", "-C", str(source), "rev-parse", "HEAD"], text=True).strip() == data["source"]
sys.path.insert(0, str(source / "haiku"))
import chat
from haiku import HaikuGenerator, SEED_WORDS
from harmonix import Harmonix
from async_harmonix import AsyncHarmonix
from overthinkg import Overthinkg

def rolling(words):
    return [tuple(words[i:i+3]) for i in range(len(words)-2)]
def cloud(h):
    return [list(r) for r in h.conn.execute(
        "SELECT word,weight,frequency,last_used,added_by FROM words ORDER BY id")]
def seed(directory, words, clock):
    with patch.object(chat, "STATE_DIR", Path(directory)), patch.object(chat, "SEED_WORDS", words):
        with patch("time.time", return_value=clock), contextlib.redirect_stdout(io.StringIO()):
            chat.init_database()
async def empty(path):
    async with AsyncHarmonix(path) as h:
        await h.morph_cloud([])
def memory(h, g, order, vocab):
    assert set(vocab) == g.vocab
    assert set(order) == {(a,b,c) for (a,b), nxt in g.markov_chain.items() for c in nxt}
    return dict(observer=[list(r) for r in h.conn.execute(
        "SELECT word1,word2,word3,count,resonance FROM trigrams ORDER BY id")],
        chain=[list(t)+[g.markov_chain[t[:2]][t[2]]] for t in order],
        vocab=vocab, python_vocab_sorted=sorted(g.vocab),
        recent=[list(t) for t in g.get_recent_trigrams()],
        successors=[dict(pair=list(k),words=list(v)) for k,v in g.markov_chain.items() if len(v)>1])

assert next(c for c in data["cloud_cases"] if c["name"]=="all_seeds")["seeds"] == SEED_WORDS
assert next(c for c in data["memory_cases"] if c["name"]=="all_seeds")["seeds"] == SEED_WORDS
for case in data["cloud_cases"]:
    with tempfile.TemporaryDirectory() as directory:
        seed(directory, case["seeds"], case["clock"])
        path = str(Path(directory)/"cloud.db")
        h, over = Harmonix(path), Overthinkg(path)
        for word,weight,count,clock in case["overrides"]:
            h.conn.execute("UPDATE words SET weight=?,frequency=?,last_used=? WHERE word=?",
                           (weight,count,clock,word))
        h.conn.commit()
        assert cloud(h) == case["snapshots"][0], case["name"]
        for event, expected in zip(case["events"], case["snapshots"][1:], strict=True):
            with patch("time.time", return_value=event["clock"]):
                if event["kind"]=="morph":
                    if event["words"]: h.morph_cloud(event["words"])
                    else: asyncio.run(empty(path))
                elif event["kind"]=="seed": seed(directory,event["words"],event["clock"])
                else: over._add_word(event["word"],event["source"],event["weight"])
            assert cloud(h) == expected, case["name"]
        before = cloud(h)
        try: h.morph_cloud([])
        except Exception as error:
            assert "Incorrect number of bindings" in str(error)
        else: raise AssertionError("Expected pinned synchronous empty-input defect")
        assert cloud(h) == before
        over.close(); h.close()

for case in data["memory_cases"]:
    with tempfile.TemporaryDirectory() as directory:
        g=HaikuGenerator(case["seeds"],state_path=str(Path(directory)/"absent.json"),db_path=":memory:")
        h=Harmonix(":memory:")
        order=list(dict.fromkeys(rolling(case["seeds"])))
        vocab=list(dict.fromkeys(case["seeds"]))
        assert memory(h,g,order,vocab) == case["snapshots"][0], case["name"]
        for event,expected in zip(case["events"],case["snapshots"][1:],strict=True):
            mode=event.get("mode","both")
            triples=[tuple(t) for t in event.get("triples",[])]
            if mode in ("count","resonance"):
                field="count" if mode=="count" else "resonance"
                t=tuple(event["triple"])
                h.conn.execute("UPDATE trigrams SET "+field+"=? WHERE word1=? AND word2=? AND word3=?",
                               (event["value"],*t)); h.conn.commit()
                if mode=="count": g.markov_chain[t[:2]][t[2]]=event["value"]
            else:
                if mode in ("both","observer"): h.update_trigrams(triples)
                if mode in ("both","chain"):
                    g.update_chain(triples)
                    order=list(dict.fromkeys(order+triples))
                    vocab=list(dict.fromkeys(vocab+[w for t in triples for w in t]))
            assert memory(h,g,order,vocab) == expected, case["name"]
        g.db_conn.close(); h.close()

with tempfile.TemporaryDirectory() as directory:
    g=HaikuGenerator([],state_path=str(Path(directory)/"absent.json"),db_path=":memory:")
    h=Harmonix(":memory:"); order=[]; vocab=[]
    for event in data["foreground"]:
        triples=rolling(event["words"])
        d,p=h.compute_dissonance(triples,g.get_recent_trigrams())
        assert [float(d),p.novelty,p.arousal,p.entropy] == event["before"]
        with patch("time.time",return_value=event["clock"]): h.morph_cloud(event["words"])
        h.update_trigrams(triples); g.update_chain(triples)
        order=list(dict.fromkeys(order+triples))
        vocab=list(dict.fromkeys(vocab+[w for t in triples for w in t]))
        d,p=h.compute_dissonance(triples,g.get_recent_trigrams())
        assert [float(d),p.novelty,p.arousal,p.entropy] == event["after"]
        assert cloud(h) == event["cloud"]
        assert memory(h,g,order,vocab) == event["memory"]
    g.db_conn.close(); h.close()
assert sum(f["numeric"] for f in data["fixtures"]) == 5055
assert sum(f["lists"] for f in data["fixtures"]) == 247
print("PASS: all 20 recorded sequences match direct pinned Python calls")
PY
~~~
