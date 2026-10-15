#!/usr/bin/env bash
set -euo pipefail
haiku_root=$(cd "$(dirname "$0")/.." && pwd)
haiku_aml=${HAIKU_AML:-"$haiku_root/../ariannamethod.ai/runner/aml-notorch"}
haiku_amlc=${HAIKU_AMLC:-"$haiku_root/../ariannamethod.ai/tools/amlc"}
haiku_lib=${HAIKU_AML_LIB:-"$haiku_root/../ariannamethod.ai/libaml.a"}
haiku_bridge=${HAIKU_AML_BRIDGE_LIB:-"$haiku_root/../ariannamethod.ai/libaml_notorch.a"}
haiku_notorch=${HAIKU_NOTORCH_LIB:-"$haiku_root/../notorch/libnotorch.a"}
haiku_work=$(mktemp -d "${TMPDIR:-/tmp}/haiku-concept-numerical.XXXXXX")
trap 'haiku_status=$?; if [ "$haiku_status" -ne 0 ]; then cat "$haiku_work"/*.err >&2 2>/dev/null || true; fi; rm -rf "$haiku_work"' EXIT
mkdir -p "$haiku_work/prefix/lib" "$haiku_work/foreign"
ln -s "$haiku_lib" "$haiku_work/prefix/lib/libaml.a"
ln -s "$haiku_bridge" "$haiku_work/prefix/lib/libaml_notorch.a"
ln -s "$haiku_notorch" "$haiku_work/prefix/lib/libnotorch.a"
sha256sum "$haiku_root/weights/haiku1.gguf" "$haiku_root/weights/haiku2.gguf" > "$haiku_work/weights.before"
printf 'concept_checkpoint = "%s/cache.state"\nIMPORT "%s/tests/contact_concept_numerical.aml"\n' "$haiku_work" "$haiku_root" > "$haiku_work/run.aml"
"$haiku_aml" "$haiku_work/run.aml" > "$haiku_work/interpreted.out" 2> "$haiku_work/interpreted.err"
test ! -s "$haiku_work/interpreted.err"
cp "$haiku_work/cache.state" "$haiku_work/interpreted.state"
if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/run.aml" --scalar -o "$haiku_work/probe" > "$haiku_work/compile.out" 2> "$haiku_work/compile.err"; then
    cat "$haiku_work/compile.out" "$haiku_work/compile.err" >&2
    exit 1
fi
(cd "$haiku_work/foreign" && ../probe) > "$haiku_work/compiled.out" 2> "$haiku_work/compiled.err"
test ! -s "$haiku_work/compiled.err"
cmp "$haiku_work/interpreted.out" "$haiku_work/compiled.out"
cmp "$haiku_work/interpreted.state" "$haiku_work/cache.state"
for haiku_case in 0 1; do
    printf 'concept_checkpoint = "%s/cache.state"\nconcept_invalid_case = %s\nIMPORT "%s/tests/contact_concept_invalid.aml"\n' "$haiku_work" "$haiku_case" "$haiku_root" > "$haiku_work/invalid.aml"
    if "$haiku_aml" "$haiku_work/invalid.aml" > "$haiku_work/invalid.out" 2> "$haiku_work/invalid.err"; then
        printf '%s\n' "Concept corrupt cache $haiku_case was accepted by interpreter" >&2
        exit 1
    fi
    haiku_diagnostic='Concept audit teacher input differs'
    if [ "$haiku_case" -eq 1 ]; then
        haiku_diagnostic='Concept audited teacher vector differs'
    fi
    case "$(cat "$haiku_work/invalid.err")" in
        *"$haiku_diagnostic"*) ;;
        *) cat "$haiku_work/invalid.err" >&2; exit 1 ;;
    esac
    if ! AML_PREFIX="$haiku_work/prefix" "$haiku_amlc" "$haiku_work/invalid.aml" --scalar -o "$haiku_work/invalid" > "$haiku_work/invalid-compile.out" 2> "$haiku_work/invalid-compile.err"; then
        cat "$haiku_work/invalid-compile.out" "$haiku_work/invalid-compile.err" >&2
        exit 1
    fi
    if (cd "$haiku_work/foreign" && ../invalid) > "$haiku_work/invalid-compiled.out" 2> "$haiku_work/invalid-compiled.err"; then
        printf '%s\n' "Concept corrupt cache $haiku_case was accepted by compiled runner" >&2
        exit 1
    fi
    case "$(cat "$haiku_work/invalid-compiled.err")" in
        *"$haiku_diagnostic"*) ;;
        *) cat "$haiku_work/invalid-compiled.err" >&2; exit 1 ;;
    esac
done
sha256sum "$haiku_root/weights/haiku1.gguf" "$haiku_root/weights/haiku2.gguf" > "$haiku_work/weights.after"
cmp "$haiku_work/weights.before" "$haiku_work/weights.after"
cat "$haiku_work/interpreted.out"
printf '%s\n' 'HAIKU_CONTACT_CONCEPT_PARITY_AND_IMMUTABLE_WEIGHTS_OK'
printf '%s\n' 'HAIKU_CONTACT_CONCEPT_BODY_AUDIT_REJECTIONS_2_OK'
