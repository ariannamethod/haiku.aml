# Third-party source attribution

## English syllable estimation

`src/form.aml` and `src/english_rules.aml` adapt the estimator and rule data from
**python-syllables 1.1.5**, authored by **David L. Day** and published under the
**GNU General Public License, version 3**.

- Upstream: <https://github.com/prosegrinder/python-syllables>
- Source: `syllables/__init__.py` from the 1.1.5 distribution.
- Source SHA-256: `10cffa48878a801c3b950dea7986f4077a86001f3a65025555687a86c4fb8a2a`.
- AML adaptation by Arianna Method contributors, **2026-10-07**: replace Python
  regular expressions and case conversion with native AML rule lists and
  Unicode-aware predicates, retain the estimator's behavior, and add line/form
  operations plus explicit AML capacity checks.

The adaptation is distributed under GNU GPL v3 with the rest of HAiKU AML.
The complete license, including the absence of warranty, is in [LICENSE](LICENSE).
The package is used to reproduce development references; it is not a runtime
or build dependency of HAiKU AML.

The behavioral reference for the surrounding word and line operations is
Python HAiKU at `ariannamethod/harmonix` commit
`abb878c52d763b73e5ad7a4d6a68a9ea7a248a39`.
