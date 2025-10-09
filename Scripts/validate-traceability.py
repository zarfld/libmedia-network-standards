#!/usr/bin/env python3
"""Validate traceability matrix and orphan report produced by generate-traceability-matrix.py.
Currently simple: fails if any requirement has no linked element.
Future: add integrity level filtering & severity levels.
"""
from __future__ import annotations
import pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
REPORTS = ROOT / 'reports'
MATRIX = REPORTS / 'traceability-matrix.md'
ORPHANS = REPORTS / 'orphans.md'

if not MATRIX.exists() or not ORPHANS.exists():
    print('Traceability artifacts missing. Run generate-traceability-matrix.py first.', file=sys.stderr)
    sys.exit(1)

matrix = MATRIX.read_text(encoding='utf-8').splitlines()
issues = []
for line in matrix:
    if not line.startswith('| REQ-'):  # skip header
        continue
    parts = [p.strip() for p in line.strip('|').split('|')]
    if len(parts) < 2:
        continue
    req_id = parts[0]
    linked = parts[1]
    if linked == '(none)':
        issues.append(f'Requirement {req_id} has no linked architecture/design/test elements.')

# Basic orphan scan from orphan report
orphans_text = ORPHANS.read_text(encoding='utf-8')
if 'requirements_no_links' in orphans_text and re.search(r'requirements_no_links\n- REQ-', orphans_text):
    pass  # already captured above

if issues:
    print('❌ Traceability validation failed:')
    for msg in issues:
        print(f' - {msg}')
    sys.exit(1)
print('✅ Traceability validation passed (basic).')
