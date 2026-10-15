#!/usr/bin/env python3
"""Optional audit: python3 tests/compare_exchange_profiles.py

Uses HAIKU_AML or the sibling AML/NoTorch runner, Python stdlib and git.
Retains source snapshots, outputs, checkpoints and SHA receipts in a temp dir.
Replays the two-turn v1/v3/v4 comparison against merged symbol work (#20).
"""
import hashlib
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
import tarfile
import tempfile
import time

BASELINE = '09ebd0d071045ddf6739051d8e283438172e969b'
PROFILES = {1: 185, 3: 237, 4: 245}


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def fixture(source, profile, destination):
    lines = [
        'IMPORT ' + json.dumps(str(source / 'src/iceberg_session.aml')),
        'missing = model_try_load("")',
        'core = haiku_state_enable_dreams(haiku_state_new("regex", 0, 0, 575, 57), 0)',
        'record_set(core, "dream_config", [1, 1, 1])',
        'life = haiku_iceberg_new(core, [1, 0, 0, 0])',
    ]
    if profile >= 3:
        lines += [
            'life = haiku_iceberg_enable_janus(life, 20740, 1, 0)',
            'life = haiku_iceberg_enable_words(life, 0)',
        ]
    if profile >= 4:
        lines += ['life = haiku_iceberg_enable_symbols(life, 1, 0)']
    lines += [f'assert(record_get(life, "version") == {profile}, "Golden profile differs")']
    for turn, (text, day) in enumerate((('rain', 20740), ('moon', 20741)), 1):
        checkpoint = json.dumps(str(destination / f'turn{turn}.state'))
        lines += [
            f'output = haiku_iceberg_turn_at(life, "{text}", 0, missing, missing, 0, {day})',
            f'assert(record_get(life, "voice:turn") == {turn}, "Golden foreground turn differs")',
            f'assert(haiku_dream_check(haiku_iceberg_part(life, "voice")) == {turn}, "Golden requires actual dreams")',
            f'assert(list_len(record_keys(life)) == {PROFILES[profile]}, "Golden checkpoint fields differ")',
            f'checkpoint_save(life, {checkpoint})',
            f'PRINT "GOLDEN_TURN_{turn}_BEGIN"',
            'PRINT output',
            f'PRINT "GOLDEN_TURN_{turn}_END"',
        ]
    return '\n'.join(lines) + '\n'


def main():
    root = Path(__file__).resolve().parents[1]
    runner = Path(os.environ.get('HAIKU_AML', root.parent / 'ariannamethod.ai/runner/aml-notorch')).resolve()
    runner_sha = sha(runner)
    work = Path(tempfile.mkdtemp(prefix='haiku-exchange-profiles-'))
    print(f'Audit files: {work}', flush=True)
    archive = subprocess.check_output(['git', '-C', str(root), 'archive', BASELINE, 'src'])
    with tarfile.open(fileobj=io.BytesIO(archive)) as tree:
        for member in tree.getmembers():
            parts = Path(member.name).parts
            if not parts or parts[0] != 'src' or '..' in parts or not (member.isdir() or member.isfile()):
                raise RuntimeError(f'Unexpected source archive member: {member.name}')
            path = work / 'baseline' / member.name
            if member.isdir():
                path.mkdir(parents=True, exist_ok=True)
            else:
                path.parent.mkdir(parents=True, exist_ok=True)
                with tree.extractfile(member) as content:
                    path.write_bytes(content.read())
    shutil.copytree(root / 'src', work / 'candidate/src')
    receipt = {'baseline_commit': BASELINE, 'runner': str(runner),
               'runner_sha256': runner_sha, 'source_sha256': {}, 'seconds': {}, 'files': []}
    names = ('stdout.txt', 'turn1.output', 'turn2.output', 'turn1.state', 'turn2.state')
    for side in ('baseline', 'candidate'):
        source = work / side
        source_hashes = {str(p.relative_to(source)): sha(p)
                         for p in sorted((source / 'src').rglob('*')) if p.is_file()}
        receipt['source_sha256'][side] = source_hashes
        started = time.monotonic()
        for profile in PROFILES:
            case = source / f'v{profile}'
            case.mkdir()
            script = case / 'golden.aml'
            script.write_text(fixture(source, profile, case))
            with (case / 'stdout.txt').open('wb') as out, (case / 'stderr.txt').open('wb') as err:
                run = subprocess.run([str(runner), str(script)], cwd=case, stdout=out, stderr=err)
            if run.returncode or (case / 'stderr.txt').stat().st_size:
                raise RuntimeError(f'{side} v{profile}: rc={run.returncode}; see {case}/stderr.txt')
            stdout = (case / 'stdout.txt').read_bytes()
            for turn in (1, 2):
                begin = f'GOLDEN_TURN_{turn}_BEGIN\n'.encode()
                end = f'GOLDEN_TURN_{turn}_END\n'.encode()
                if stdout.count(begin) != 1 or stdout.count(end) != 1:
                    raise RuntimeError(f'{side} v{profile}: missing or repeated output marker {turn}')
                (case / f'turn{turn}.output').write_bytes(stdout.split(begin, 1)[1].split(end, 1)[0])
            print(f'{side} v{profile}: two actual dreams, {PROFILES[profile]} fields', flush=True)
        receipt['seconds'][side] = time.monotonic() - started
        if sha(runner) != runner_sha or source_hashes != {
            str(p.relative_to(source)): sha(p)
            for p in sorted((source / 'src').rglob('*')) if p.is_file()
        }:
            raise RuntimeError('Runtime or source snapshot changed during the audit')
    for profile in PROFILES:
        for name in names:
            before, after = (work / side / f'v{profile}' / name for side in ('baseline', 'candidate'))
            receipt['files'].append({'profile': profile, 'file': name,
                                     'equal': before.read_bytes() == after.read_bytes(),
                                     'baseline_sha256': sha(before), 'candidate_sha256': sha(after)})
    receipt['equal'] = all(item['equal'] for item in receipt['files'])
    (work / 'receipt.json').write_text(json.dumps(receipt, indent=2) + '\n')
    if not receipt['equal']:
        for item in receipt['files']:
            if not item['equal']:
                print(f"DIFF v{item['profile']}: {item['file']}")
        raise SystemExit(1)
    print('PASS: 6/6 outputs, 6/6 complete checkpoints and 3/3 combined stdout are byte-identical')
    print(f'Receipt: {work}/receipt.json')


if __name__ == '__main__':
    main()
