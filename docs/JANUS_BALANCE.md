# Two faces, two private clocks

`src/janus_balance.aml` gives Haiku and its friend separate Gamma/Janus
balance records. Each presence remembers an expectation, meets a later external
witness, and carries that encounter into its next frozen mixture. Both retain
the same immutable A/B bodies described in [LEXICAL_SOURCES.md](LEXICAL_SOURCES.md).

The source equation is pinned to
[`ariannamethod/janus` at `5a945a9d7c1dc9abfa217f685f0a81455f9ac493`](https://github.com/ariannamethod/janus/blob/5a945a9d7c1dc9abfa217f685f0a81455f9ac493/janus.c).
Calendar arithmetic follows `calendar_cumulative_drift()` and
`calendar_dissonance()`; the coefficient follows `dual_blend()`:

```text
alpha = clamp01(.5 + .3*(calendarD - .5) - .2*debt + .1*personalD)
W_effective = alpha*A + (1-alpha)*B
personalD = clamp01(abs(current_drift - birth_drift)/33)
```

`alpha=1` selects A; `alpha=0` selects B. AML's separate `JANUS_BLEND`
orientation is `1-alpha`. The owned module calls no process-global Gamma/Janus
directives. Mode `1` uses the equation; mode `0` keeps the owner's explicit
`fixed_alpha`, including endpoint controls. Both modes retain the same
expectation and witness machinery.

## One captured calendar day

The input is a UTC civil ordinal: `floor(POSIX seconds / 86400)`, with
1970-01-01 equal to zero. The foreground captures it once in `request.day`;
workers and the balance module use that value. `birth_day` is the day this
organ is activated, including an explicit upgrade of an older life.

The pinned C Janus epoch is October 3, 2024 at noon. Haiku gives that epoch an
explicit UTC identity and interprets a civil ordinal at midnight:

```text
elapsed = trunc_toward_zero(day - 19999.5)
years = elapsed / 365.25
cycles = trunc_toward_zero(years / 19)
year_in_cycle = trunc_toward_zero(fmod(years, 19)) + 1
corrections = cycles*7*30 + 30*count([3,6,8,11,14,17,19] <= year_in_cycle)
drift = years*11.25 - corrections
calendarD = abs(fmod(drift, 33))/33
```

This retains C's signed truncation and remainder. The module accepts integral
days from −1,000,000 through 1,000,000 and requires new encounters to move
forward in day and turn. Pure snapshots may inspect a later day without
advancing either owned clock. Replays return before chronology checks.

| UTC ordinal | Elapsed Janus days | Drift | Calendar dissonance |
|---:|---:|---:|---:|
| 19,634 | −365 | −11.242299 | 0.34067574 |
| 19,999 | 0 | 0 | 0 |
| 20,000 | 0 | 0 | 0 |
| 20,740 | 740 | −7.2073927 | 0.21840584 |

The C Janus use of `mktime` is replaced by this explicit UTC conversion.
Python's modulo and the audit HTML's negative-cycle convention are separate
calendar conventions; their negative-date values are not these fixtures.

## A forecast must meet someone later

A completed lexical pass supplies all 48 reviewed whole-word scores, their
ordered pool, both available model identities, tokenizer identity, frozen
requested/effective alpha, direction, and context ancestry. The owner copies
those scores into one pending expectation. An uncompleted pass changes
nothing. A later completed pass can supersede an unresolved expectation;
its receipt records that replacement.

The next eligible external event consumes that expectation exactly once:

- Haiku hears a native human contact, source `0`.
- The friend hears an actual Haiku dream line delivered as source `6`.
  Its parent and origin are both native Haiku source `7`, and delivery,
  parent, and origin event numbers must agree.
- The witness turn must be strictly later than the forecast turn. Its source
  event must be newer than the owner's last accepted event in that lane.

Own utterances, model returns, model-derived peer deliveries, explicit replay,
and a renumbered old peer line leave this record unchanged. The friend's
forecast made after a dream therefore waits for a later dream's delivery.

The witness text is tokenized into unique whole words. For English words
present in the reviewed pool:

```text
observed = mean(saved_score[word] for unique matching witness words)
diff = max(0, max(all 48 saved scores) - observed)
gap = diff / (1 + diff)
debt_next = .9*debt + .1*gap
```

The bounded gap follows pinned C Janus's max-logit contrast and denominator.
Using a later external witness's mean reviewed-word score is Haiku's explicit
adaptation of that mechanism. A top-scored encounter has gap zero and decays
debt. A witness with no reviewed overlap consumes the forecast as
`assessed=0`, preserving debt. Repeated spelling counts once. Each receipt
retains the observed score, full-pool maximum, exact matching words, gap,
debt before/after, and expectation lineage.

The pure snapshot chooses alpha after witness settlement. The session stages
that alpha in a detached lexical owner and adopts it only when a pass
commits. Missing models can still leave an older expectation available for a
legitimate later witness. Disabled deeper life leaves both balance records
unchanged. A successful single-source pass records its actual endpoint alpha
alongside the requested dynamic preference.

## API and checkpoint

| Function | Result |
|---|---|
| `haiku_janus_new(owner, mode, fixed_alpha, birth_day, witness_source)` | New owned record: `haiku`/`0` or `friend`/`6` |
| `haiku_janus_check(state)` | Strict shape, domain, chronology and receipt validation |
| `haiku_janus_calendar(day)` | `[elapsed, drift, calendarD]` |
| `haiku_janus_snapshot(state, day)` | Pure `owner, revision, day, elapsed, drift, calendar_dissonance, personal_dissonance, debt, alpha, janus_blend` |
| `haiku_janus_expect(state, receipt, day)` | `1` when a completed forecast commits; `0` for absent/replayed completion |
| `haiku_janus_witness(state, event, day)` | `committed, assessed, consumed, matched, gap, debt_before, debt_after` |
| `haiku_janus_save(state, path)` / `haiku_janus_load(path)` | Validated exact checkpoint |

Each owner has exactly 20 flat fields:

| Fields | Contents |
|---|---|
| `format`, `version`, `owner`, `calendar` | Record and calendar identities |
| `mode`, `fixed_alpha`, `birth_day`, `witness_source` | Private configuration |
| `clock`, `last_day`, `debt`, `revision` | Current owned continuity |
| `seen[24]` | Witness and directional model-source dedupe cursors |
| `pending_scores[48]`, `pending_meta[16]`, `pending_identity[3]` | One complete expectation, timing/ancestry, A/B/tokenizer hashes |
| `history_rows[384]`, `history_text[64]` | Sixteen receipts, each with 24 numbers and four strings |
| `history_cursor`, `history_count` | Bounded receipt position |

`pending_meta` stores active, event, turn, provider source, direction,
effective alpha, requested alpha, day, origin source/event, parent
source/event, creation revision, leading word index, availability, and a
reserved zero. Consuming an expectation clears its active bit and retains its
evidence. History strings contain a bounded context excerpt, leading word,
exact matched words, and sender. Records contain no model handles or RNG.

The experimental session explicitly upgrades its composite to v2; v1 lives
keep their existing continuation until that upgrade is requested. The
session-level contract is in [ICEBERG_SESSION.md](ICEBERG_SESSION.md).

## Reproducible controls

Run `tests/run_janus_balance.sh`. It runs the AML lifecycle and actual frozen
GGUF controls interpreted and compiled, compares their output and checkpoint
bytes, checks an independent C calendar oracle, and verifies both model files'
SHA-256 before and after.

The numerical fixture exercises exact debt arithmetic, top-word fulfilment,
unknown-word consumption, owner isolation, same-turn refusal, peer ancestry,
replay, sixteen-receipt wraparound, and pending restart. The host gate compares
every owned leaf after ten rejected calls and sixteen corrupt-state checks.
Its 180 civil-day fixtures match the pinned C float equation exactly.

The real experiment uses `water tree music`, the same cold lexical context,
and the same 48-word pool in five conditions: A endpoint, B endpoint, fixed
half mixture, initial dynamic mixture, and dynamic mixture after a controlled
later human witness. That witness supplies the initial dynamic pass's
lowest-scored reviewed word. Each condition performs exactly one PEN7 forward;
all five use the shared immutable sources. The control excludes parliament
prototype refreshes: a changed alpha in the live session also rebuilds its
four named reference representations.

Measured with the checked-in bodies on UTC ordinal 20,740, with activation on
that same day:

| Condition | Frozen alpha | Leading reviewed word | Mean BPE logit |
|---|---:|---|---:|
| A endpoint | 1 | root | 3.60891271 |
| B endpoint | 0 | snow | 3.16484928 |
| Fixed half | 0.5 | morning | 2.83230281 |
| Initial dynamic | 0.415521741 | root | 2.74421048 |
| Dynamic after witness | 0.398964405 | root | 2.73326755 |

The controlled later witness was `light`, producing gap `0.827866793` and
debt `0.0827866793`. Maximum absolute changes across the 48 scores were
`1.84042859` for A versus B, `1.78339064` for fixed half versus initial
dynamic, and `0.780918956` after debt changed the next mixture. Both execution
paths produced identical output and checkpoint bytes. Both model hashes
remained unchanged.
