# The original ear

`haiku_sp.model` is the unchanged tokenizer data shipped with Python HAiKU:

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
asset from the repository directory.

The training recipe and corpus remain in the pinned source's
[`haiku/scripts/`](https://github.com/ariannamethod/harmonix/tree/abb878c52d763b73e5ad7a4d6a68a9ea7a248a39/haiku/scripts).
Runtime dependencies remain AML and NoTorch.
