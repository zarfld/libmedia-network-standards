#!/usr/bin/env python3
"""Generate Test Skeletons from Spec Index

Consumes build/spec-index.json (produced by spec_parser.py) and creates
placeholder test files for requirements not yet covered by existing tests.

Mapping Rules (simple heuristic):
  - For each REQ-* create tests/test_requirements/test_<id_normalized>.py
  - Insert pytest skeleton with docstring referencing linked design IDs
  - Skip if file already exists (idempotent)

Non-goals: full scenario expansion (left for future enhancement)
"""
from __future__ import annotations
import json, re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
INDEX = ROOT / 'build' / 'spec-index.json'
TEST_DIR = ROOT / '05-implementation' / 'tests' / 'generated'

REQ_PREFIX = 'REQ'

SAFE = re.compile(r'[^a-zA-Z0-9_]')

def normalize(req_id: str) -> str:
    return SAFE.sub('_', req_id.lower())

def main() -> int:
    if not INDEX.exists():
        print("Spec index missing; run spec_parser first")
        return 1
    data = json.loads(INDEX.read_text(encoding='utf-8'))
    items = data.get('items', [])
    TEST_DIR.mkdir(parents=True, exist_ok=True)
    created = 0
    for item in items:
        rid = item['id']
        if not rid.startswith(REQ_PREFIX):
            continue
        fname = TEST_DIR / f"test_{normalize(rid)}.py"
        if fname.exists():
            continue
        refs = ', '.join(item.get('references', [])[:6])
        content = (
            f"# Auto-generated placeholder test for {rid}\n"
            f"# Do NOT edit manually; update requirement or promote to custom test.\n"
            f"import pytest\n\n"
            f"@pytest.mark.requirement('{rid}')\n"
            f"def test_{normalize(rid)}():\n"
            f"    \"\"\"Requirement {rid}: {item['title']}\n"
            f"    References: {refs}\n"
            f"    \"\"\"\n"
            f"    # TODO: Implement test logic derived from requirement specification.\n"
            f"    assert True  # placeholder\n"
        )
        fname.write_text(content, encoding='utf-8')
        created += 1
    print(f"Created {created} new requirement test skeleton(s) in {TEST_DIR}")
    return 0

if __name__ == '__main__':
    import sys
    raise SystemExit(main())
