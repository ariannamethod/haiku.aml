# Iceberg research probes

These small, read-only probes reproduce source observations in
[ICEBERG.md](../../../docs/ICEBERG.md). They call the pinned relatives directly;
the external checkouts are research inputs. Haiku's runtime dependencies remain
AML and NoTorch. These probes are run explicitly, outside `make test`.

Use a C compiler, Bash, Node.js, and the checkouts below. The PEN7 probe also uses
Python's standard library to construct its temporary C host. Commands were
executed on Linux with GCC 13.3. Keep binaries, logs, and generated C in a
disposable directory outside the repository.

## Source roots and pins

Run this setup from the Haiku repository root; point `iceberg_sources` at a
directory containing the named external checkouts. Their source must match
these revisions, also recorded in the audit.

```sh
iceberg_sources=/absolute/path/to/checkouts
iceberg_probes="$PWD/tests/research/iceberg"
iceberg_work=$(mktemp -d)

iceberg_pin() {
    test "$(git -C "$iceberg_sources/$1" rev-parse HEAD)" = "$2" || return 1
    git -C "$iceberg_sources/$1" diff --quiet HEAD
}

iceberg_pin actually.life 2bf11b3f2fab36e1a8cd8a917438ca2423f65ee8
iceberg_pin leo b842f6f480da928c666f8685e87cac6f6b899e90
iceberg_pin caveLLMan 471aed21abe9b7b882a464f07bc66b8ef3d5c411
iceberg_pin 1984 a0d20019587ff14e5516eb050ba0876d6edd0baa
iceberg_pin janus 5a945a9d7c1dc9abfa217f685f0a81455f9ac493
iceberg_pin notorch bfde06669ad34896421b86147e919ca902a0d337
iceberg_pin ariannamethod.ai f2d4496790be33505715ba90373fea5225799625
```

Each pin check must exit zero before running its corresponding probe. The
external source trees are read in place; none of the commands edits them.

## Glyphs and pressure

```sh
cc -O2 -std=gnu11 -I "$iceberg_sources/actually.life" \
  "$iceberg_probes/glyph_actual_probe.c" -lm -o "$iceberg_work/glyph-actual"
"$iceberg_work/glyph-actual"

cc -O2 -std=gnu11 -I "$iceberg_sources/leo" \
  "$iceberg_probes/glyph_leo_probe.c" -lm -o "$iceberg_work/glyph-leo"
"$iceberg_work/glyph-leo"

cc -O2 -std=c11 -I "$iceberg_sources/caveLLMan/ariannamethod" \
  "$iceberg_probes/cavellman-iceberg-probe.c" -o "$iceberg_work/cave-tokenizer"
"$iceberg_work/cave-tokenizer" \
  "$iceberg_sources/caveLLMan/weights/cavellman_A.bin.vocab"

cc -O1 -std=gnu11 -D_GNU_SOURCE -ffunction-sections -fdata-sections \
  -I "$iceberg_sources/caveLLMan" -I "$iceberg_sources/caveLLMan/ariannamethod" \
  "$iceberg_probes/cavellman-surface-probe.c" \
  "$iceberg_sources/caveLLMan/ariannamethod/notorch.c" \
  -Wl,--gc-sections -lm -pthread -o "$iceberg_work/cave-surface"
"$iceberg_work/cave-surface"
```

The cave surface probe uses caveLLMan's own pinned NoTorch source to reproduce
that body's update. The PEN7 experiment below uses canonical NoTorch.

| Probe | Measured observations |
|---|---|
| `glyph_actual_probe.c` | English compression; directional transition pull; no birth at 13 pair occurrences, `fire+water` at 14 as ID90; recursive ID91 resolution; `BE fire` changes the internal charge; dreams leave external transition counts at 4 |
| `glyph_leo_probe.c` | Hearing leaves `nebula` unresolved; answering the pending question with `river ocean` binds it to `water`, records one missed guess, and adds .4 to the complex chamber |
| `cavellman-iceberg-probe.c` | 514 aliases / 506 distinct strings; exact compressed examples; all 88 shipped vocabulary IDs match |
| `cavellman-surface-probe.c` | First-pair emergence, ID88/BOS overlap, survival then death below .525, surprise-driven pressure, raw-count/normalization order, zero Hebbian factors after 100 updates |

All four probes exit zero and print their observations. They do not start a
colony, training loop, server, or source CLI. Compiler warnings originating in
the included pinned sources remain visible.

## Penelope's browser field

```sh
node "$iceberg_probes/penelope_html_field_probe.cjs" \
  "$iceberg_sources/1984/penelope.html"
```

The probe evaluates the pinned HTML's field logic in Node's VM with a minimal
DOM, synchronous animation timers, and an explicit LCG32 seed42. A call to the
neural forward throws. The measured field-mode chain contains the seed plus
twelve transitions:

```text
darkness cell ghost fever temple translation distance pilgrimage
tremor ministry iguana corpse skin
```

It also prints the chambers, destiny, trauma, 45 co-occurrence entries, 12
bigrams, and the source UI metric string. This is a reproducible field receipt;
browser-native randomness is replaced explicitly by the probe's seed.

## AML's gamma branch

```sh
cc -O2 -I "$iceberg_sources/ariannamethod.ai/core" \
  "$iceberg_probes/aml-gamma-iceberg-probe.c" \
  "$iceberg_sources/ariannamethod.ai/core/ariannamethod.c" \
  -lm -lpthread -o "$iceberg_work/aml-gamma"
"$iceberg_work/aml-gamma"
```

This source receipt exercises the gamma/GAMMA branch examined in the audit.
Its source pin belongs to the measured language revision above.

## PEN7 through canonical NoTorch

`pen7_build_probe.py` constructs a C oracle/NoTorch comparison from the pinned
Penelope source. Its two file arguments are the original Penelope and NanoJanus
PEN7 weights. Generation and compilation happen in the disposable directory.

```sh
python3 "$iceberg_probes/pen7_build_probe.py" \
  --source-root "$iceberg_sources" --output "$iceberg_work/pen7.c"
cc -O2 -std=gnu11 -ffp-contract=off -pthread \
  -I "$iceberg_sources" -I "$iceberg_sources/notorch" \
  "$iceberg_work/pen7.c" "$iceberg_sources/notorch/notorch.c" \
  "$iceberg_sources/notorch/gguf.c" \
  "$iceberg_sources/notorch/harness/runtime.c" -lm -o "$iceberg_work/pen7"
"$iceberg_work/pen7" \
  "$iceberg_sources/1984/weights/penelope.bin" \
  "$iceberg_sources/janus/weights/nanojanus.bin"
```

Normal execution must exit zero. The deliberate wrong-RoPE control must exit
one; it verifies that the comparison detects the changed architecture:

```sh
if PEN7_WRONG_ROPE=1 "$iceberg_work/pen7" \
    "$iceberg_sources/1984/weights/penelope.bin" \
    "$iceberg_sources/janus/weights/nanojanus.bin"; then
    printf 'wrong-RoPE control unexpectedly passed\n' >&2
    exit 1
else
    test "$?" -eq 1 || exit 1
fi
```

The physical parameter-mixture probe uses the same executable. It maps A/B
read-only, materializes a temporary midpoint, compares C/NoTorch output and
records the difference from averaging endpoint logits:

```sh
"$iceberg_work/pen7" --blend \
  "$iceberg_sources/1984/weights/penelope.bin" \
  "$iceberg_sources/janus/weights/nanojanus.bin"
```

Expected exit is zero. The nine-token source receipt is in
`pen7_blend_probe.json`, including all endpoint and midpoint logits. Timings
and resident memory are observations from the original host, not exact portable
expectations. Endpoint and wrong-RoPE receipts are alongside it.

The loader-only observations have their own successful exit status: zero means
the current native tensor and GGUF loaders both declined PEN7 as expected.

```sh
"$iceberg_work/pen7" --loaders "$iceberg_sources/1984/weights/penelope.bin"
"$iceberg_work/pen7" --loaders "$iceberg_sources/janus/weights/nanojanus.bin"
```

The audit records exact weight identities, forward values, and measured memory.
No weights are copied into Haiku, and these commands publish no model or state.
