# First organs — the owl has a pulse

2026-10-07. Governed by the [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md).

Four functions now run as pure AML. Their behavioral reference is
[`harmonix/haiku` at `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`](https://github.com/ariannamethod/harmonix/tree/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku).
The bridge source is `phase4_bridges.py:HaikuStateTransition.score`.

| AML function | Input boundary | Result |
|---|---|---|
| `haiku_pulse(user_count, system_count, word_overlap, unique_words)` | Trigram list lengths, word Jaccard overlap, unique word count | `[novelty, arousal, entropy]` |
| `haiku_observe(user_count, system_count, intersection, union, matching_trigrams)` | List lengths, word-set intersection/union sizes, exact trigram-set intersection size | `[dissonance, novelty, arousal, entropy]` |
| `haiku_temperatures(dissonance)` | Supplied dissonance | `[haiku_temperature, observer_temperature]` |
| `haiku_bridge_score(similarity, quality_delta, overwhelm, boredom, stuck)` | Aggregated transition metrics | Composite transition score |

Counts are nonnegative integer-valued scalars. List lengths include duplicates;
word and trigram intersection sizes use sets. The text organ will supply these
counts. Scalar arithmetic and fixed array returns run in AML; the learners'
NoTorch binding follows in the numerical-library work.

The original behavior is retained: empty observations return neutral dissonance
and zero pulse; direct empty `haiku_pulse` calls still compute their own pulse.
Arousal can exceed one. Entropy/arousal/novelty use strict `>` thresholds.
The exact-trigram discount happens after all boosts, followed by the final
dissonance clamp. Temperature is the direct affine mapping. Bridge quality and
penalty inputs are clamped individually; similarity and the final score retain
their original range.

## Run it

Build the sibling AML v5.2.0 runner, scalar `libaml.a`, and `amlc` with native
`IMPORT` and UTF-8 values, then:

Toolchain pin: [AML PR #27](https://github.com/ariannamethod/ariannamethod.ai/pull/27),
`d7e695fdcfd3b5fe0ec02c5776d8f2560f94b995` (v5.2.0). Rebuild the library and
runner together against that header.

```sh
make -C ../ariannamethod.ai all
make test
```

Override locations with `AML_ROOT`, or with `AML`, `AMLC`, and `AML_LIB`.
`tests/fixtures/numerical.aml` imports `src/harmonix.aml` and `src/bridges.aml`
directly. `IMPORT` shares their function definitions in one prepared program.
`tests/run_numerical.sh` runs that fixture through the interpreter and through
`amlc --scalar`; the imported AML files remain runtime inputs of the executable.
It stages the selected `libaml.a` in a temporary installation prefix, compares
both outputs with the success receipt, and deletes generated files on exit.
The numerical modules and ordinary test command execute AML throughout.

## Verification receipt

Linux x86_64; CPython 3.12 reference calls; AML float32 arithmetic, absolute
tolerance **0.000002** against Python's recorded values.

- **49 cases, 130 scalar reference values** pass in both the interpreter and
  the compiled `--scalar` executable.
- Coverage: both/one side empty, empty word union, exact matches, reordered
  words, disjoint words, repeated trigrams, thresholds below/at/above 0.7 and
  0.6, arousal above one, all boosts before discount, final clamping, temperature
  mapping inside and outside `[0,1]`, bridge bounds and combined penalties.
- Four organism functions plus one fixture helper, loaded through native
  modules. The shell gate checks the root fixture budget; AML validates the
  expanded source and rejects overlong executable lines.
- Interpreter and compiled success output agree exactly:

```text
PASS: 130 Python reference values in AML interpreter and compiled --scalar
```

## Reproduce the reference values

`tests/reference/numerical_inputs.json` contains the exact original trigram
lists and transition inputs. `tests/fixtures/numerical.aml` contains values
obtained by calling Python `Harmonix`, `PulseSnapshot`, and
`HaikuStateTransition` at the pinned commit. Python is used for this development
reference check; the repository's executable organism is AML.

From this repository, in the original Haiku's Python reference environment:

```sh
python3 - ../harmonix <<'PY'
import json, re, subprocess, sys
from pathlib import Path
source = Path(sys.argv[1]).resolve()
data = json.loads(Path('tests/reference/numerical_inputs.json').read_text())
assert subprocess.check_output(['git', '-C', str(source), 'rev-parse', 'HEAD'], text=True).strip() == data['source_commit']
subprocess.run(['git', '-C', str(source), 'diff', '--exit-code', 'HEAD', '--', 'haiku/harmonix.py', 'haiku/phase4_bridges.py'], check=True)
sys.path.insert(0, str(source / 'haiku'))
from harmonix import Harmonix, PulseSnapshot
from phase4_bridges import HaikuStateTransition
fixture = Path('tests/fixtures/numerical.aml').read_text()
h, checked = Harmonix(':memory:'), 0
try:
    for c in data['cases']:
        kind = c['kind']
        if kind in ('observe', 'pulse'):
            u, s = map(lambda rows: list(map(tuple, rows)), (c['user'], c['system']))
            uw, sw = ({w for t in rows for w in t} for rows in (u, s))
            if kind == 'observe':
                d, p = h.compute_dissonance(u, s)
                values = [d, p.novelty, p.arousal, p.entropy]
                args = [len(u), len(s), len(uw & sw), len(uw | sw), len(set(u) & set(s))]
            else:
                p = PulseSnapshot.from_interaction(u, s, c['overlap'])
                values = [p.novelty, p.arousal, p.entropy]
                args = [len(u), len(s), c['overlap'], len(uw | sw)]
            name = 'haiku_' + kind
        elif kind == 'temperature':
            args = [c['dissonance']]
            values, name = h.adjust_temperature(*args), 'haiku_temperatures'
        else:
            args = c['args']
            values = [HaikuStateTransition('a', 'b', 1, *args).score]
            name = 'haiku_bridge_score'
        block = fixture.split('# ' + c['name'] + '\n', 1)[1].split('\n# ', 1)[0]
        call = name + '(' + ', '.join(format(float(x), '.17g') for x in args) + ')'
        assert 'actual = ' + call + '\n' in block, c['name']
        stored = [float(x) for x in re.findall(r'haiku_check\(actual(?:\[\d+\])?, ([^)]+)\)', block)]
        assert stored == list(map(float, values)), c['name']
        checked += len(stored)
finally:
    h.close()
print(f'PASS: {checked} fixture values match direct pinned Python calls')
PY
```

The [text boundary module](TEXT_PARITY.md) now covers words, lines, and RAE
coherence. Set extraction, state storage, asynchronous scheduling, MathBrain,
and the rest of RAE remain the next organs in the [migration map](MIGRATION.md). The
[RAE experiments](RAE_RESEARCH.md) keep their separate research status.
