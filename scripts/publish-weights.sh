#!/usr/bin/env bash
# Reproduce the two approved Q8_0 assets, then publish one file per commit/push.
# CI publication is restricted to the named repository and topic branch.
# --offline SOURCE_ROOT OUTPUT_DIR builds/verifies without git writes or network.
set -euo pipefail
export GIT_TERMINAL_PROMPT=0

haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_branch=codex/haiku-gguf-parliament
haiku_repository=ariannamethod/haiku.aml
haiku_mode=publish
haiku_source_root=
haiku_output=
case ${1:-} in
    '') [ "$#" -eq 0 ] || exit 2 ;;
    --offline)
        [ "$#" -eq 3 ] || { printf 'Usage: %s --offline SOURCE_ROOT OUTPUT_DIR\n' "$0" >&2; exit 2; }
        haiku_mode=offline
        haiku_source_root=$(cd "$2" && pwd)
        haiku_output=$3
        ;;
    *) printf 'Usage: %s [--offline SOURCE_ROOT OUTPUT_DIR]\n' "$0" >&2; exit 2 ;;
esac

die() { printf 'Weight publication: %s\n' "$*" >&2; exit 1; }

if [ "$haiku_mode" = publish ]; then
    [ "${GITHUB_ACTIONS:-}" = true ] || die 'publication runs only in the authorized GitHub Actions job'
    [ "${GITHUB_REPOSITORY:-}" = "$haiku_repository" ] || die 'wrong repository'
    [ "${GITHUB_REF:-}" = "refs/heads/$haiku_branch" ] || die 'wrong branch'
    [ "${GITHUB_EVENT_NAME:-}" = push ] || die 'expected the topic-branch push event'
    [[ ${GITHUB_SHA:-} =~ ^[0-9a-f]{40}$ ]] || die 'invalid triggering commit'
    [[ ${NOTORCH_REVISION:-} =~ ^[0-9a-f]{40}$ ]] || die 'an exact published NoTorch revision is required'
    [ "$(git -C "$haiku_root" rev-parse --show-toplevel)" = "$haiku_root" ] || die 'unexpected repository root'
    [ "$(git -C "$haiku_root" branch --show-current)" = "$haiku_branch" ] || die 'checkout is not the topic branch'
    haiku_origin=$(git -C "$haiku_root" remote get-url origin)
    case "$haiku_origin" in
        "https://github.com/$haiku_repository"|"https://github.com/$haiku_repository.git") ;;
        *) die 'origin is not the authorized GitHub repository' ;;
    esac
    [ -z "$(git -C "$haiku_root" status --porcelain --untracked-files=all)" ] || die 'checkout must be clean'
    git -C "$haiku_root" merge-base --is-ancestor "$GITHUB_SHA" HEAD || die 'topic branch no longer contains this workflow run'
    # A retry may follow the first successful asset commit. Only those two asset
    # paths may have changed after the event; concurrent source work ends this run.
    while IFS= read -r haiku_changed; do
        case "$haiku_changed" in weights/haiku1.gguf|weights/haiku2.gguf|'') ;;
            *) die 'new source work arrived after the triggering commit' ;;
        esac
    done < <(git -C "$haiku_root" diff --name-only "$GITHUB_SHA" HEAD)
fi

haiku_tmp=$(mktemp -d "${TMPDIR:-/tmp}/haiku-publish-weights.XXXXXXXX")
trap 'rm -rf -- "$haiku_tmp"' EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

# Read only the approved two-file manifest. All shell fields are constrained
# before they leave the JSON reader; no manifest value is evaluated as code.
python3 -I - "$haiku_root/weights/manifest.json" > "$haiku_tmp/assets.tsv" <<'PY'
import json, re, sys
with open(sys.argv[1], encoding="utf-8") as f:
    manifest = json.load(f)
assert manifest["schema"] == 1
assert manifest["architecture"] == "penelope"
assert manifest["gguf_version"] == 3
assert manifest["storage"]["matrix_dtype"] == "Q8_0"
approved = [
    ("haiku1.gguf", "ariannamethod/1984", "a0d20019587ff14e5516eb050ba0876d6edd0baa", "weights/penelope.bin"),
    ("haiku2.gguf", "ariannamethod/janus", "5a945a9d7c1dc9abfa217f685f0a81455f9ac493", "weights/nanojanus.bin"),
]
assert len(manifest["assets"]) == len(approved)
for asset, expected in zip(manifest["assets"], approved):
    assert tuple(asset[k] for k in ("file", "source_repository", "source_revision", "source_file")) == expected
    assert asset["bytes"] == 21308576 and asset["source_bytes"] == 78477152
    assert re.fullmatch(r"[0-9a-f]{64}", asset["sha256"])
    assert re.fullmatch(r"[0-9a-f]{64}", asset["source_sha256"])
    print("\t".join(str(asset[k]) for k in (
        "file", "source_repository", "source_revision", "source_file",
        "source_bytes", "source_sha256", "bytes", "sha256")))
PY

verify_file() {
    local haiku_file=$1 haiku_bytes=$2 haiku_sha=$3 haiku_actual
    [ -f "$haiku_file" ] && [ ! -L "$haiku_file" ] || die 'expected a regular non-symlink asset'
    [ "$(wc -c < "$haiku_file")" -eq "$haiku_bytes" ] || die 'asset byte count differs from the manifest'
    haiku_actual=$(sha256sum < "$haiku_file")
    [ "${haiku_actual%% *}" = "$haiku_sha" ] || die 'asset SHA256 differs from the manifest'
}

if [ "$haiku_mode" = publish ]; then
    haiku_notorch="$haiku_tmp/notorch"
    git init --quiet "$haiku_notorch"
    git -C "$haiku_notorch" remote add origin https://github.com/ariannamethod/notorch.git
    git -C "$haiku_notorch" fetch --quiet --depth=1 origin "$NOTORCH_REVISION"
    git -C "$haiku_notorch" checkout --quiet --detach FETCH_HEAD
    [ "$(git -C "$haiku_notorch" rev-parse HEAD)" = "$NOTORCH_REVISION" ] || die 'NoTorch revision mismatch'
    haiku_output="$haiku_tmp/converted"
else
    haiku_notorch="$haiku_source_root/notorch"
fi

haiku_output=$(python3 -I -c 'import os,sys; print(os.path.realpath(sys.argv[1]))' "$haiku_output")
case "$haiku_output" in
    "$haiku_root"|"$haiku_root"/*) die 'conversion output must stay outside the repository' ;;
esac
haiku_parent=$haiku_output
while [ ! -e "$haiku_parent" ]; do haiku_parent=$(dirname "$haiku_parent"); done
[ -d "$haiku_parent" ] || die 'conversion output parent is not a directory'
if git -C "$haiku_parent" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    die 'conversion output must stay outside every Git worktree'
fi
mkdir -p -- "$haiku_output"
for haiku_name in haiku1.gguf haiku2.gguf; do
    [ ! -e "$haiku_output/$haiku_name" ] && [ ! -L "$haiku_output/$haiku_name" ] || die 'conversion output already exists'
done

cc -O2 -std=gnu11 -ffp-contract=off -pthread -I"$haiku_notorch" \
    "$haiku_notorch/tools/pen7_to_gguf.c" "$haiku_notorch/gguf.c" \
    "$haiku_notorch/sha256.c" "$haiku_notorch/notorch.c" -lm \
    -o "$haiku_tmp/pen7_to_gguf"

while IFS=$'\t' read -r haiku_name haiku_repo haiku_revision haiku_source_file \
    haiku_source_bytes haiku_source_sha haiku_bytes haiku_sha; do
    if [ "$haiku_mode" = publish ]; then
        haiku_input="$haiku_tmp/${haiku_name%.gguf}.bin"
        curl --fail --location --silent --show-error --proto '=https' --proto-redir '=https' \
            --connect-timeout 20 --max-time 300 --retry 3 \
            "https://raw.githubusercontent.com/$haiku_repo/$haiku_revision/$haiku_source_file" \
            --output "$haiku_input"
    else
        haiku_input="$haiku_source_root/${haiku_repo#*/}/$haiku_source_file"
    fi
    verify_file "$haiku_input" "$haiku_source_bytes" "$haiku_source_sha"
    "$haiku_tmp/pen7_to_gguf" --q8_0 "$haiku_input" "$haiku_output/$haiku_name"
    verify_file "$haiku_output/$haiku_name" "$haiku_bytes" "$haiku_sha"
done < "$haiku_tmp/assets.tsv"

if [ "$haiku_mode" = offline ]; then
    printf 'Verified both Q8_0 assets offline; no repository files or refs were changed.\n'
    exit 0
fi

[ -d "$haiku_root/weights" ] && [ ! -L "$haiku_root/weights" ] || die 'weights must be a regular directory'
while IFS=$'\t' read -r haiku_name haiku_repo haiku_revision haiku_source_file \
    haiku_source_bytes haiku_source_sha haiku_bytes haiku_sha; do
    haiku_relative="weights/$haiku_name"
    haiku_target="$haiku_root/$haiku_relative"
    # Each push is an ordinary fast-forward. New concurrent work is preserved
    # by refusing to publish on top of a branch tip this job has not inspected.
    git -C "$haiku_root" fetch --quiet origin "$haiku_branch"
    [ "$(git -C "$haiku_root" rev-parse FETCH_HEAD)" = "$(git -C "$haiku_root" rev-parse HEAD)" ] || die 'remote topic branch changed during conversion'
    if [ -e "$haiku_target" ] || [ -L "$haiku_target" ]; then
        verify_file "$haiku_target" "$haiku_bytes" "$haiku_sha"
        git -C "$haiku_root" ls-files --error-unmatch -- "$haiku_relative" >/dev/null || die 'existing asset is not tracked'
        printf 'Already published and verified: %s\n' "$haiku_relative"
        continue
    fi
    [ -z "$(git -C "$haiku_root" status --porcelain --untracked-files=all)" ] || die 'checkout changed before staging an asset'
    cp -- "$haiku_output/$haiku_name" "$haiku_target"
    chmod 644 "$haiku_target"
    verify_file "$haiku_target" "$haiku_bytes" "$haiku_sha"
    git -C "$haiku_root" add -- "$haiku_relative"
    [ "$(git -C "$haiku_root" diff --cached --name-only)" = "$haiku_relative" ] || die 'commit would include an unexpected path'
    if [ "$haiku_name" = haiku1.gguf ]; then
        haiku_quote='One field receives its first stored body.'
    else
        haiku_quote='The second body keeps another direction open.'
    fi
    {
        printf 'Add verified %s Q8_0 body\n\n' "$haiku_name"
        printf 'Source: %s@%s / %s\n' "$haiku_repo" "$haiku_revision" "$haiku_source_file"
        printf 'NoTorch converter: %s\n' "$NOTORCH_REVISION"
        printf 'SHA256: %s\n\n' "$haiku_sha"
        printf 'Quote: "%s" — Astra\n' "$haiku_quote"
        printf 'Method: Reproduce pinned PEN7 bytes through canonical NoTorch Q8_0 and verify the complete manifest before publication.\n'
    } > "$haiku_tmp/commit-message.txt"
    git -C "$haiku_root" -c user.name='github-actions[bot]' \
        -c user.email='41898282+github-actions[bot]@users.noreply.github.com' \
        commit --quiet --file "$haiku_tmp/commit-message.txt"
    [ "$(git -C "$haiku_root" diff-tree --no-commit-id --name-only -r HEAD)" = "$haiku_relative" ] || die 'new commit contains an unexpected path'
    git -C "$haiku_root" push origin "HEAD:refs/heads/$haiku_branch"
    printf 'Published and verified: %s\n' "$haiku_relative"
done < "$haiku_tmp/assets.tsv"
