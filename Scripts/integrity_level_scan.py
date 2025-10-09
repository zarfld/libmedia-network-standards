#!/usr/bin/env python3
"""Integrity Level Scanner

Scans spec files for integrityLevel >= 3 and emits a JSON summary used to
conditionally trigger stricter checks (e.g., additional reviewer, evidence).

Outputs build/integrity-scan.json with structure:
{
  "highIntegrity": true/false,
  "files": [ {"path": str, "level": int} ]
}

Current usage: detection only (no gating logic beyond presence). Future work can
extend to matrix strategies or dynamic job inclusion.
"""
from __future__ import annotations
import json, re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'build' / 'integrity-scan.json'
TARGET_LEVEL = 3

FRONT_MATTER_RE = re.compile(r'^---\n(.*?)\n---\n', re.DOTALL)
LEVEL_RE = re.compile(r'^integrityLevel\s*:\s*(\d+)', re.MULTILINE)

SCAN_DIRS = [
    ROOT / '02-requirements',
    ROOT / '03-architecture',
]

def extract(text: str):
    m = FRONT_MATTER_RE.match(text)
    if not m:
        return None
    block = m.group(1)
    lvl_match = LEVEL_RE.search(block)
    return int(lvl_match.group(1)) if lvl_match else None

def main() -> int:
    results = []
    for base in SCAN_DIRS:
        if not base.exists():
            continue
        for md in base.rglob('*.md'):
            if md.name.startswith('README'): continue
            try:
                text = md.read_text(encoding='utf-8', errors='ignore')
                lvl = extract(text)
                if lvl is not None and lvl >= TARGET_LEVEL:
                    results.append({'path': str(md.relative_to(ROOT)), 'level': lvl})
            except Exception as e:
                print(f"WARN: integrity scan failed for {md}: {e}")
    data = {
        'highIntegrity': any(r['level'] >= TARGET_LEVEL for r in results),
        'files': results,
        'targetLevel': TARGET_LEVEL,
    }
    OUT.parent.mkdir(exist_ok=True)
    OUT.write_text(json.dumps(data, indent=2), encoding='utf-8')
    print(f"Integrity scan complete. High integrity present: {data['highIntegrity']}. Entries: {len(results)}")
    return 0

if __name__ == '__main__':
    import sys
    raise SystemExit(main())
