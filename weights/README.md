# Two quiet sources

Penelope and NanoJanus become Haiku's two shared lexical bodies. Each presence
will approach both through its own history and mixing balance. These assets
are immutable; parliamentary learning belongs to the separate glyph field.
The [Arianna Method Manifesto](../ARIANNA_METHOD_MANIFESTO.md) governs the work.

| File | Body | Storage | Bytes |
|---|---|---|---:|
| `haiku1.gguf` | Penelope | Q8_0 matrices, F32 positions/norms/gates | 21,308,576 |
| `haiku2.gguf` | NanoJanus | Q8_0 matrices, F32 positions/norms/gates | 21,308,576 |

NoTorch's `penelope` family expands Q8_0 matrices **once to owned F32** through
`gguf_dequant`, then performs the forward in F32. This also supplies numerical
parameters for the planned per-presence mixtures. Unquantized tensors borrow
the read-only file mapping. No optimizer or tape registers either body.

Each model has 19,619,280 parameters in 92 tensors. Packing reduces each file
from the 78,477,152-byte PEN7 source to 21,308,576 bytes (20.32 MiB). Full F32
parameters still occupy about 74.84 MiB per body; mapped Q8 bytes, sequence
caches and scratch are additional memory. Q8 unpacking preserves the quantized
model's values; it cannot recover the original discarded precision.

The [manifest](manifest.json) records complete source revisions, original and
converted SHA-256 identities, tokenizer contract and measured parameter errors.
Sources are [Penelope](https://github.com/ariannamethod/1984/tree/a0d20019587ff14e5516eb050ba0876d6edd0baa)
and [NanoJanus](https://github.com/ariannamethod/janus/tree/5a945a9d7c1dc9abfa217f685f0a81455f9ac493),
by Oleg Ataeff and Arianna Method contributors. Both source repositories ship
GPLv3; their pinned license files are linked in the manifest. Haiku retains its
[GPLv3 license](../LICENSE).

## Reproduce the bodies

Use NoTorch's native `tools/pen7_to_gguf.c` converter at
[`4d0b807`](https://github.com/ariannamethod/notorch/commit/4d0b807d6d545d8e25b25bebf425377d62833ede)
and the pinned original files. Its output path must not already exist. From
the NoTorch checkout:

```sh
make pen7_to_gguf BLAS_FLAGS= BLAS_LIBS= X86_SIMD=0 ARM_SIMD=0
./pen7_to_gguf --q8_0 ../1984/weights/penelope.bin /tmp/haiku1.gguf
./pen7_to_gguf --q8_0 ../janus/weights/nanojanus.bin /tmp/haiku2.gguf
sha256sum /tmp/haiku1.gguf /tmp/haiku2.gguf
```

Expected identities:

```text
50f159a2c34952bd9e6349880b4b20f8bbc0ac4b1f1dc2c1f6e4768732548b66  haiku1.gguf
16909b8590bd7fbbf15ab11b2dbf3743d612da8c617aca2dda03d640fef13e29  haiku2.gguf
```

Omitting `--q8_0` creates the lossless F32 reference. The converter verifies
every tensor, source size/header, tokenizer and finite values before publishing
the output. NoTorch's independent audit reconstructs all 609,280 Q8 blocks per
body and checks the 26 F32 tensors byte for byte. NoTorch owns quantization,
dequantization, forward operations and the native tokenizer.

The repository's [publication workflow](../.github/workflows/publish-weights.yml)
reproduces these bytes from the pinned sources and publishes each verified
asset in its own commit and push on the integration branch. Its local
`scripts/publish-weights.sh --offline SOURCE_ROOT OUTPUT_DIR` mode performs
the same conversion and verification without changing Git files or refs.

## Executed comparison

On the scalar Linux/GCC reference build, contexts of 1, 9, 33 and 256 tokens
produce **32,768/32,768 identical logits** between NoTorch and the native C
oracle bound to the same expanded Q8 values, covering full and chunked input
for both bodies. Comparing Q8 to the original F32 models is a separate test:

| Body | Maximum absolute logit difference across these contexts | Same next-token argmax | Same fixed greedy continuation |
|---|---:|---:|---:|
| Penelope | 0.224016 | 4/4 | 16/16 tokens |
| NanoJanus | 0.197905 | 4/4 | 16/16 tokens |

These are measured fixtures, not a claim that every later association stays
identical after quantization. The lossless F32 path also matches its native
source exactly. NoTorch's `tests/test_penelope_reference.py` contains the
reproducible forward, tokenizer, chunking and deliberate wrong-RoPE controls.

Both files declare the common Penelope byte tokenizer. NanoJanus's older
wrapper dropped non-ASCII input; this shared contract preserves bytes. GGUF
also retains 49 long token surfaces lost by the original fixed decoder buffer.
Those deliberate wrapper improvements leave token identities and model
geometry intact.

The ordinary Haiku chat does not load these sources yet. The next integration
connects immutable model handles, optional lexical proposals and glyph
representations, with a quiet weightless path when assets are absent. Keeping
model ownership outside conversation snapshots is part of that boundary.
