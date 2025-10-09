#!/usr/bin/env python3
"""Trace Unlinked Requirements Helper

Purpose:
  Identify requirement (REQ-*) identifiers that currently have NO linkage to any
  Architecture Decision Record (ADR-*) according to the same symmetric
  (forward + backward) inference model used by build_trace_json.py.

Usage:
  python scripts/trace_unlinked_requirements.py [--json | --markdown]

Behavior:
  - Consumes build/traceability.json if present (preferred)
  - Falls back to build/spec-index.json and reconstructs the minimal linkage
    model (forward + backward) for ADR relationships only.
  - Emits a human readable summary by default.
  - With --json outputs machine readable JSON list of unlinked requirement IDs.
  - With --markdown outputs a markdown table (ID | Source Path | Title).

Exit Codes:
  0 success (will still be 0 even if all requirements are linked)
  1 missing prerequisite files and cannot proceed

This script is intentionally read-only; it does not modify any specs. It is a
pre-step before judiciously adding a single justified ADR linkage to raise ADR
coverage without artificial inflation.
"""
from __future__ import annotations
import json, argparse, sys
from pathlib import Path
from typing import Dict, List

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / 'build'
TRACE_FILE = BUILD / 'traceability.json'
INDEX_FILE = BUILD / 'spec-index.json'


def load_trace_details() -> Dict[str, dict]:
    if TRACE_FILE.exists():
        data = json.loads(TRACE_FILE.read_text(encoding='utf-8'))
        m = data.get('metrics', {}).get('requirement_to_ADR', {})
        details = m.get('details') or {}
        if details:
            # details format: rid -> {forward_refs: [...], reverse_refs: [...]}
            return details
    # Fallback: reconstruct from spec-index.json
    if not INDEX_FILE.exists():
        raise FileNotFoundError("Neither traceability.json nor spec-index.json exists; run spec_parser + build_trace_json first.")
    idx = json.loads(INDEX_FILE.read_text(encoding='utf-8'))
    items = idx.get('items', [])
    forward = {i['id']: i.get('references', []) for i in items}
    # Build reverse map requirement <- ADR
    adr_to_reqs = {}
    for itm in items:
        iid = itm['id']
        if iid.startswith('ADR'):
            for ref in itm.get('references', []):
                if ref.startswith('REQ'):
                    adr_to_reqs.setdefault(ref, set()).add(iid)
    details: Dict[str, dict] = {}
    for itm in items:
        rid = itm['id']
        if not rid.startswith('REQ'):
            continue
        fwd = [r for r in forward.get(rid, []) if r.startswith('ADR')]
        rev = sorted(list(adr_to_reqs.get(rid, set())))
        details[rid] = {'forward_refs': fwd, 'reverse_refs': rev}
    return details


def enrich_with_source_metadata(req_ids: List[str]) -> Dict[str, dict]:
    """Return mapping id -> {path, title} using spec-index.json metadata."""
    meta = {}
    if INDEX_FILE.exists():
        idx = json.loads(INDEX_FILE.read_text(encoding='utf-8'))
        for itm in idx.get('items', []):
            if itm['id'] in req_ids:
                meta[itm['id']] = {'path': itm.get('path'), 'title': itm.get('title')}
    return meta


def main(argv: List[str]) -> int:
    ap = argparse.ArgumentParser()
    fmt = ap.add_mutually_exclusive_group()
    fmt.add_argument('--json', action='store_true', help='Emit raw JSON list of unlinked requirements with metadata')
    fmt.add_argument('--markdown', action='store_true', help='Emit markdown table output')
    args = ap.parse_args(argv)

    try:
        details = load_trace_details()
    except FileNotFoundError as e:
        print(f"ERROR: {e}", file=sys.stderr)
        return 1

    unlinked = [rid for rid, d in details.items() if not d['forward_refs'] and not d['reverse_refs']]
    unlinked.sort()
    meta = enrich_with_source_metadata(unlinked)

    if args.json:
        payload = []
        for rid in unlinked:
            info = meta.get(rid, {})
            payload.append({'id': rid, **info})
        print(json.dumps({'unlinked_requirements': payload, 'count': len(unlinked)}, indent=2))
        return 0

    if args.markdown:
        print('| Requirement ID | Title | Source Path |')
        print('|---------------|-------|-------------|')
        for rid in unlinked:
            info = meta.get(rid, {})
            title = (info.get('title') or '').replace('|','\\|')
            path = (info.get('path') or '').replace('|','\\|')
            print(f'| {rid} | {title} | {path} |')
        print(f"\nTotal unlinked requirements: {len(unlinked)}")
        return 0

    # Default human readable summary
    print('Unlinked Requirements (no ADR forward or backward references found):')
    if not unlinked:
        print('  (none)')
    else:
        for rid in unlinked:
            info = meta.get(rid, {})
            print(f"  - {rid}  ({info.get('title','?')})  [{info.get('path','?')}]")
    print(f"Count: {len(unlinked)}")
    return 0


if __name__ == '__main__':
    raise SystemExit(main(sys.argv[1:]))
