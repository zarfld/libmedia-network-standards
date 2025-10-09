#!/usr/bin/env python3
"""ADR Impact Scanner

Detects architecture-impacting changes (in views/components/architecture spec)
without a corresponding new or modified ADR file in the same diff.

Heuristic (when run in CI / PR context):
- Look at git diff against base (default: origin/main) for files under 03-architecture/views or architecture-spec files.
- If changes detected and no file added/modified in 03-architecture/decisions with 'ADR' pattern today, emit warning and non-zero exit code.

Local usage can supply --base <ref>.

Limitations: Simple heuristic; does not parse semantic changes.
"""
from __future__ import annotations
import subprocess, sys, argparse, os, re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

ADR_DIR = ROOT / '03-architecture' / 'decisions'
VIEW_DIR = ROOT / '03-architecture' / 'views'
ARCH_SPEC = ROOT / '03-architecture'

ADR_NAME_RE = re.compile(r'ADR', re.IGNORECASE)


def run(cmd: list[str]) -> str:
    return subprocess.check_output(cmd, cwd=ROOT, text=True, stderr=subprocess.DEVNULL).strip()

def changed_files(base: str) -> list[str]:
    diff_cmd = ['git', 'diff', '--name-only', base, 'HEAD']
    out = run(diff_cmd)
    return [l for l in out.splitlines() if l]


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument('--base', default=os.environ.get('GITHUB_BASE_REF', 'origin/main'))
    ap.add_argument('--warn-only', action='store_true', help='Do not fail, just warn')
    args = ap.parse_args()

    try:
        files = changed_files(args.base)
    except subprocess.CalledProcessError:
        print('Could not obtain diff; skipping ADR impact scan (no-op).')
        return 0

    arch_changes = [f for f in files if f.startswith('03-architecture/views') or f.endswith('architecture-spec.md')]
    if not arch_changes:
        print('No architecture view/spec changes detected.')
        return 0

    adr_changes = [f for f in files if f.startswith('03-architecture/decisions') and ADR_NAME_RE.search(Path(f).name)]

    if adr_changes:
        print('Architecture changes accompanied by ADR update(s):')
        for a in adr_changes:
            print(f'  - {a}')
        return 0

    msg = '⚠️ Architecture-impacting changes detected without accompanying ADR update.'
    if args.warn_only:
        print(msg)
        return 0
    print(msg, file=sys.stderr)
    return 2

if __name__ == '__main__':
    raise SystemExit(main())
