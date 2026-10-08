# The original ear

The optional `haiku_sp.model` is acquired locally from Python HAiKU.
Set it up once, from any directory:

```bash
bash /path/to/haiku.aml/scripts/setup-tokenizer.sh
```

Or copy an existing pinned source asset:

```bash
bash scripts/setup-tokenizer.sh --from ../harmonix/haiku/models/haiku_sp.model
bash scripts/setup-tokenizer.sh --check
```

The setup script downloads over HTTPS using `curl` or `wget`, or copies the
supplied file. It verifies size and SHA-256 before an atomic rename into
`models/haiku_sp.model`. Failed acquisition or verification preserves the
installed file. `--check` verifies without network access; the default command
also reuses an already verified file. The model and temporary downloads stay
outside Git. Default regex conversations work without this asset.

Pinned asset:

- Source: [`harmonix/haiku/models/haiku_sp.model`](https://github.com/ariannamethod/harmonix/blob/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku/models/haiku_sp.model).
- Source revision: `abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`.
- License: source repository GPL-3.0; see this repository's [LICENSE](../LICENSE).
- Size: 256,534 bytes.
- SHA-256: `b8fb56e049977498be0f59586534e67b43b23fd1d7a2efd06cc6c62d9213b942`.

It carries 650 Unigram pieces: 644 normal, three control, one unknown, and
the user-defined `<haiku>` / `<line>`. The embedded `nfkc_cf` normalizer has
247,028 bytes and preserves its original Unicode mappings. Binary float32
piece scores determine segmentation, including ties.

AML lowercases the input, asks NoTorch for pieces, removes every `▁`, and
discards empty pieces. Unknown runs keep their normalized surface. Tabs,
newlines and punctuation can remain tokens.

The default mode is `regex`, matching Python's root launcher. The original
`haiku_run.py` left the working directory unchanged, so its relative
`models/haiku_sp.model` lookup missed the model from the Harmonix root.
Running inside `haiku/` activated SentencePiece. AML chooses the mode
explicitly; model-load failures are errors. The chat launcher resolves this
asset relative to its AML source file. `make test-tokenizer` and the native
foreground checks require the exact verified asset; their diagnostics give
the setup command when it is missing or changed.

The training recipe and corpus remain in the pinned source's
[`haiku/scripts/`](https://github.com/ariannamethod/harmonix/tree/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku/scripts).
Runtime dependencies remain AML and NoTorch.
