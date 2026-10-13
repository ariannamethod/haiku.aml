# PEN7 → NoTorch compatibility receipt

Research-only audit, 2026-10-13. No repository files changed; no training or product implementation. The scratch host retains the pinned Penelope C forward as an oracle and runs a second forward through existing NoTorch numerical primitives.

## Source identities

| Body | Git commit | Weight SHA-256 |
|---|---|---|
| Penelope | `a0d20019587ff14e5516eb050ba0876d6edd0baa` | `afff67c780a7c78fdc1f76e521dfda30d65e68b95fe5bd324bb392b84ba3cd38` |
| NanoJanus | `5a945a9d7c1dc9abfa217f685f0a81455f9ac493` | `d675df2ee3a150d63bc73935a84060280d0f6b0aa84283e4b78ab905180b576c` |
| NoTorch | `bfde06669ad34896421b86147e919ca902a0d337` | Source build below |

Both files are exactly **78,477,152 bytes**: 32 header bytes and **19,619,280 float32 parameters**. Header values are `[0x50454E37, 2048, 1984, 448, 896, 7, 8, 256]`: PEN7, BPE vocabulary, fixed word vocabulary, residual width, FFN width, head count, layer count, maximum context. They contain different parameter values with the same layout.

One float payload occupies 74.8416 MiB; both occupy 149.6832 MiB. The files contain the embeddings, eight sequential attention/resonance/SwiGLU layers, and the output head. The BPE merge table lives in source rather than in the PEN7 file.

## What existing loaders do

Measured against both actual files:

- `examples/infer_janus.c` exits 1 and prints `format: DoE (flat)` followed by `DoE param count 1346719287 != expected 26256912`. Its recognized formats are ARIN, the seven-int Janus BPE header, and flat DoE. The PEN7 magic reaches its flat fallback.
- `nt_load` returns NULL: it reads the NoTorch tensor serialization magic, not PEN7.
- `gguf_open` returns NULL and reports `gguf: bad magic (got 0x50454e37)`.
- The harness architecture registry has no Penelope/PEN7 family. Its current `resonance` and `janus` adapters implement their own specified arithmetic and tensor layouts.

Receipts: `pen7_infer_janus_rejection.json`, `pen7_generic_loaders_rejection.json`. This identifies the actual adapter work needed before an AML caller can load these two files.

## Existing arithmetic is sufficient for an inference adapter

PEN7's resonance branch is **a dense `W_r @ h` projection**, blended with attention by a two-scalar softmax. It is not the position-scoring RRPRAM implemented by `nt_rrpram_attention`, nor the low-rank Janus harness recurrence.

| PEN7 requirement | Existing NoTorch primitive used by the probe |
|---|---|
| Row-major dense projections | `nt_blas_matvec` |
| RMSNorm, epsilon `1e-5`, `g*x*inv` order | `harness/runtime.c:rmsnorm(out, x, g, n, eps)` |
| Split-half RoPE, `x0*cos-x1*sin`, `x0*sin+x1*cos` | `harness/runtime.c:rope(..., neox=1)` |
| Multi-head causal attention | `nt_attention` for each gathered head and query's exact causal prefix |
| Embedding+position, gate blend, residuals, SwiGLU | The source architecture's small pointwise loops; source SiLU cutoff at `x <= -20` retained |

The tape-level `nt_seq_rmsnorm` has its own fixed `1e-6` epsilon. The tape-level `nt_rope_split_half_freq` has the documented canonical large-Janus opposite sine convention, and `nt_silu` has no Penelope cutoff. Choosing these by name alone would change PEN7 arithmetic. The existing forward-only harness RMSNorm and RoPE already express PEN7's values directly. None of these differences is a defect in the other architecture's contract.

## Executed forward proof

Machine: Linux x86-64, AMD EPYC 9V74 80-Core Processor. Compiler: Ubuntu GCC 13.3.0. Scalar CPU; `-O2 -std=gnu11 -ffp-contract=off -pthread`; no external BLAS or accelerator.

Prompt: `memory turns through the silence`.

Pinned source BPE IDs: `[1472,116,1383,256,1307,294,342,108,1284]` — nine positions, exercising nonzero RoPE positions and causal attention.

The scratch host maps both weight files read-only, assigns immutable tensor views, and allocates temporary activations per call. It never starts the NoTorch global tape and never allocates Adam moments. The final receipt checks a digest of the complete global tape before/after both forwards: `tape_unchanged=true`, zero optimizer slots, zero active tape and zero tape entries. Model Adam pointers remain zero-initialized; the host never calls `model_init`, training, or source CLI entry points. It compares 2,048 final-position logits per file against the unmodified included Penelope C forward. The NanoJanus checkpoint is tested through the shared PEN7 arithmetic; NanoJanus's own native/Python cross-language probes belong to the sibling audit.

| Checkpoint | Differing float32 logits | Maximum absolute difference | RMS difference | Argmax, both paths |
|---|---:|---:|---:|---:|
| Penelope | 0 / 2,048 | 0 | 0 | 573 |
| NanoJanus | 0 / 2,048 | 0 | 0 | 573 |

All outputs finite. **4,096 / 4,096 logits are byte-identical** at this context. The result's scope is bounded parity: these two pinned checkpoints, this nine-position input, and the scalar build above.

The defect check switches the adapter to adjacent-pair RoPE. It changes all 2,048 logits for each file and the gate exits 1: Penelope max error 2.3361163; NanoJanus 3.9849584, with NanoJanus argmax changing from 573 to 111. This checks that the comparison detects a real architecture mismatch.

Observed one-run resource receipt from the normal pass:

- Before mappings: VmRSS 1,108 KiB.
- Both mappings created, headers read: 2,592 KiB.
- After Penelope's reference and NoTorch forwards: 75,616 KiB.
- After both models' forwards: **148,444 KiB (144.965 MiB)**.
- Kernel `ru_maxrss`: 148,340 KiB; Linux's independently reported current/peak counters differ slightly.
- Penelope reference 138.977 ms, NoTorch 135.169 ms; NanoJanus reference 134.142 ms, NoTorch 139.047 ms.

These are one-run proof timings at context length nine, not a throughput benchmark. The mapped file pages that the short forward never reads need not become resident. Each active provider can share immutable weights across independent calls; temporary context belongs to the call. Full context, many candidate words, phone performance, and provider interleaving are separate future gates.

## Mixing the actual parameters

The follow-up tests Oleg's intended operation directly: **every one of the 19,619,280 parameters** is interpolated with `alpha=0.5`, using `float32(0.5f*A[i] + 0.5f*B[i])`. This includes embeddings, positional embeddings, final norm, output head, all per-layer projection matrices, both norms and the two gate parameters. Both original bodies remain `PROT_READ` mappings. The midpoint is a temporary third body, which the reference C and the NoTorch path both read.

At the same nine-position prompt and scalar build:

- All **2,048 midpoint logits are byte-identical** between C and NoTorch; maximum and RMS error zero.
- All mixed parameters and output logits are finite.
- Global tape digest is unchanged, with zero optimizer slots and no active tape.
- The midpoint forward took 126.168 ms in C and 127.890 ms in NoTorch in this single receipt.

The distinction from mixing outputs is directly visible:

| Computation | Highest-scoring BPE token |
|---|---|
| Forward through body A | 573, `and` |
| Forward through body B | 573, `and` |
| `0.5*logits(A) + 0.5*logits(B)` | 573, `and` |
| Forward through the interpolated parameters | 276, `al` |

All 2,048 midpoint logits differ from the float32 average of endpoint logits. Mean absolute difference is **4.8895556**, maximum **8.2231488**. This is an observation at the BPE-logit layer; Haiku's later whole-word scoring, pressure, form and selection still have their own jobs. Full endpoint and midpoint arrays are saved in `pen7_blend_probe.json`, making this distinction reproducible without rerunning the model.

Naive materialization adds exactly **78,477,152 bytes (74.8416 MiB)** for the temporary PEN7 body. VmRSS after building it was 231,064 KiB, after the forwards **231,544 KiB (226.117 MiB)**; kernel `ru_maxrss` was 231,404 KiB. The mixing loop touches all original weight pages, so this receipt includes both complete source bodies and the materialized midpoint. A future implementation can interpolate a layer/tensor on demand or compute a fused interpolated-weight matvec against the two immutable originals. Its float32 interpolation and accumulation order need a parity gate against this materialized reference. No such optimization or product API was added here.

Three-context source-diversity measurement was not run; the requested parameter-space experiment took priority. The next behavioral comparison is the mixed pair against each endpoint and against a duplicated-one-model pair under the same Haiku pressure and draw stream.

## Minimum product boundary for pure AML Haiku

1. **NoTorch owns a PEN7 family loader and forward.** Validate exact payload length, magic, dimensions, tensor bounds, finite weights, and loaded-byte identity; retain the original bytes without constructing optimizer state. A dedicated forward-only family preserves the blend, learned position embeddings, RoPE convention and source numerical choices. The existing kernels above cover the expensive operations. A common generic model API can expose the family without teaching Haiku C about its organism.
2. **AML gets an immutable opaque model value and backend callbacks.** Its existing immutable `AM_Tokenizer` plus copied callback table is an ownership precedent. The current `AM_NumericalBackend` only exposes linear/tanh/MSE/SGD/normal values and has no weight-model loader or logits callback. The model handle should retain shared immutable bytes; each forward owns scratch. No global autograd tape or integer handle into a mutable process-wide model registry belongs in Haiku state. For Janus, AML owns the interpolation coefficient and its relationship to pressure; the backend receives two compatible immutable bodies and performs actual parameter interpolation. The ordinary endpoint and same-body pair are explicit identities to preserve.
3. **The PEN7 tokenizer contract is explicit.** A model's metadata/identity includes the pinned BPE table. Whole-word BPE scoring, source word admission, two-direction context, pressure and association policy stay in AML. Loading a weight file does not by itself provide its word tokenizer.
4. **Optional providers have a quiet absence path.** An unavailable optional weight file produces no lexical candidates and consumes no voice RNG. The existing cloud and hardcoded words continue. Provider availability, identities and admitted-word provenance can be inspected in state/diagnostics without user-facing fallback noise. A failed partial forward publishes no words.
5. **Checkpoint the life, retain the body.** Keep immutable 75 MiB model data outside the cloned per-turn record. Checkpoint both body identities, the AML-owned interpolation state, small contexts, accepted lexical traces and the source-specific RNG only if it samples. This fits Haiku's staged turn → validation → checkpoint → swap publication contract. On reattachment, compare loaded bytes with the checkpoint's identity before resuming the provider's continuation.

An AML array holds at most 1,048,576 floats (4 MiB); a PEN7 body is 19.6 million floats. Each individual matrix happens to fit, but embedding dozens of mutable matrix arrays in the life record would clone and serialize the body on every transactional turn. The opaque immutable boundary avoids that cost and keeps async ownership tractable.

The existing `1984/penelope.aml` openly packages its engine in `BLOOD COMPILE` C blocks. For Haiku's pure AML organism, reuse its architecture and source behavior through the generic NoTorch/AML bridge, while writing the lexical and association policy as ordinary AML modules.

## Reproduction

`--source-root` names a directory containing the pinned `1984`, `janus`, and `notorch` checkouts. `--output` can point into a separate temporary build directory. The generator and these receipts can live anywhere. A portable invocation was built and executed from a separate temporary directory successfully.

```sh
TASK_SOURCE_ROOT=/path/to/checkouts
TASK_RECEIPT_ROOT=/path/to/receipt
TASK_PROBE_DIR=$(mktemp -d)
python3 "$TASK_RECEIPT_ROOT/pen7_build_probe.py" \
  --source-root "$TASK_SOURCE_ROOT" --output "$TASK_PROBE_DIR/probe.c"
cc -O2 -std=gnu11 -ffp-contract=off -pthread \
  -I "$TASK_SOURCE_ROOT" -I "$TASK_SOURCE_ROOT/notorch" \
  "$TASK_PROBE_DIR/probe.c" "$TASK_SOURCE_ROOT/notorch/notorch.c" \
  "$TASK_SOURCE_ROOT/notorch/gguf.c" "$TASK_SOURCE_ROOT/notorch/harness/runtime.c" \
  -lm -o "$TASK_PROBE_DIR/probe"
"$TASK_PROBE_DIR/probe" \
  "$TASK_SOURCE_ROOT/1984/weights/penelope.bin" \
  "$TASK_SOURCE_ROOT/janus/weights/nanojanus.bin"
PEN7_WRONG_ROPE=1 "$TASK_PROBE_DIR/probe" \
  "$TASK_SOURCE_ROOT/1984/weights/penelope.bin" \
  "$TASK_SOURCE_ROOT/janus/weights/nanojanus.bin"
"$TASK_PROBE_DIR/probe" --loaders "$TASK_SOURCE_ROOT/1984/weights/penelope.bin"
"$TASK_PROBE_DIR/probe" --loaders "$TASK_SOURCE_ROOT/janus/weights/nanojanus.bin"
"$TASK_PROBE_DIR/probe" --blend \
  "$TASK_SOURCE_ROOT/1984/weights/penelope.bin" \
  "$TASK_SOURCE_ROOT/janus/weights/nanojanus.bin"
```

Normal and midpoint forwards exit 0; deliberate wrong-RoPE exits 1; loader probe exit 0 means both existing loaders declined PEN7 as expected. `pen7_build_probe.py` extracts the pinned C scaffold without modifying it and redirects its matrix/norm/RoPE/attention calls. Source loaders' pre-existing ignored-`fread` compiler warnings are preserved in `pen7_build_stderr.txt`; the scratch loader performs its own exact header/length validation and does not call them. Midpoint output includes three 2,048-element logit arrays; redirect it to a JSON file for analysis.

Source anchors: Penelope `penelope.c` lines 2296, 2355–2362, 2491–2570, 2579–2779; NoTorch `notorch.c` lines 3821, 4994, 5406, 5569, 9954; `harness/runtime.c` lines 206–235; `harness/archs.c`; `examples/infer_janus.c` lines 325–412; AML `core/ariannamethod.h` lines 316, 384–412, 463–491; `core/aml_notorch.c`.


See [The Haiku iceberg](../../ICEBERG.md) for the combined design and
[the research bundle](../../../tests/research/iceberg/README.md) for portable commands.
