#!/usr/bin/env python3
"""Generate a simple traceability matrix skeleton and orphan report.
Parses markdown files for ID patterns and produces:
 - reports/traceability-matrix.md (basic table placeholders)
 - reports/orphans.md (lists missing link elements)

Patterns recognized (with optional 4-char category prefix for complex projects):
  StR-[ABCD-]?\d{3}              (e.g., StR-001 or StR-CORE-001)
  REQ-(ABCD-)?(F|NF)-[A-Z+-]?\d{3}  (e.g., REQ-F-001 or REQ-AUTH-F-001)
  ADR-[ABCD-]?\d{3}              (e.g., ADR-001 or ADR-INFRA-001)
  ARC-C-[ABCD-]?\d{3}            (e.g., ARC-C-001 or ARC-C-CORE-001)
  QA-SC-[ABCD-]?\d{3}            (e.g., QA-SC-001 or QA-SC-PERF-001)
  TEST-[ABCD-]?[A-Z0-9-]+        (e.g., TEST-LOGIN-001 or TEST-AUTH-LOGIN-001)

Heuristics (improve later):
 - If a REQ appears in architecture spec or ADR file, we assume linkage
 - Components appear if referenced in architecture spec or views
 - Scenarios appear if in architecture-quality-scenarios file

Exit code 0 even if orphans exist (validation script enforces later).
"""
from __future__ import annotations
import re, pathlib, os, yaml
from collections import defaultdict

ROOT = pathlib.Path(__file__).resolve().parent.parent
REPORTS = ROOT / 'reports'
REPORTS.mkdir(exist_ok=True)

PATTERNS = {
    # Support optional 4-char category: StR-ABCD-001 or StR-001
    'stakeholder': re.compile(r'StR-(?:[A-Z]{4}-)?(?:[A-Z]+-)??\d{3}'),
    # REQ-AUTH-F-001 or REQ-F-001, REQ-AUTH-NF-001 or REQ-NF-001
    'requirement': re.compile(r'REQ-(?:[A-Z]{4}-)?(?:F|NF)-(?:[A-Z]+-)??\d{3}'),
    # ADR-AUTH-001 or ADR-001
    'adr': re.compile(r'ADR-(?:[A-Z]{4}-)??\d{3}'),
    # ARC-C-CORE-001 or ARC-C-001
    'component': re.compile(r'ARC-C-(?:[A-Z]{4}-)??\d{3}'),
    # QA-SC-PERF-001 or QA-SC-001
    'scenario': re.compile(r'QA-SC-(?:[A-Z]{4}-)??\d{3}'),
    # TEST-AUTH-LOGIN-001 or TEST-LOGIN-001 (flexible: alphanumeric + dash)
    'test': re.compile(r'TEST-(?:[A-Z]{4}-)?[A-Z0-9-]+'),
}

def is_guidance(path: pathlib.Path, text: str) -> bool:
    if any(seg in path.as_posix() for seg in ['.github/prompts']):
        return True
    if 'copilot-instructions.md' in path.name.lower():
        return True
    if 'ADR-template.md' in path.name:
        return True
    # front matter specType: guidance
    if text.startswith('---'):
        try:
            fm = text.split('---', 2)[1]
            meta = yaml.safe_load(fm) or {}
            if meta.get('specType') == 'guidance':
                return True
        except Exception:
            pass
    return False

files = []
for p in ROOT.rglob('*.md'):
    if 'node_modules' in p.parts or 'reports' in p.parts:
        continue
    try:
        txt = p.read_text(encoding='utf-8', errors='ignore')
    except Exception:
        continue
    if is_guidance(p, txt):
        continue
    files.append(p)
index: dict[str, set[str]] = {k: set() for k in PATTERNS}
occurrence: dict[str, dict[str, set[pathlib.Path]]] = {k: defaultdict(set) for k in PATTERNS}

for path in files:
    try:
        text = path.read_text(encoding='utf-8', errors='ignore')
    except Exception:
        continue
    # Skip placeholder example IDs (e.g., REQ-F-000, ADR-XXX) from counting
    text = re.sub(r'ADR-XXX', '', text)
    text = re.sub(r'REQ-(F|NF)-000', '', text)
    for key, pat in PATTERNS.items():
        for match in pat.findall(text):
            index[key].add(match)
            occurrence[key][match].add(path)

# Simple linkage inference
req_links = defaultdict(set)  # requirement -> linked identifiers (adr/component/scenario/test)

# Collect all requirement IDs from both legacy and new patterns
all_requirements = set()
requirement_patterns = ['requirement_legacy', 'stakeholder', 'system', 'functional', 'nonfunctional', 'interface', 'verification', 'acceptance']
for pattern in requirement_patterns:
    if pattern in index:
        all_requirements.update(index[pattern])

for adr in index['adr']:
    # parse requirements referenced in ADR file names or contents
    for req in all_requirements:
        for path in occurrence['adr'][adr]:
            if req in path.read_text(encoding='utf-8', errors='ignore'):
                req_links[req].add(adr)
for scen in index['scenario']:
    scen_text_files = occurrence['scenario'][scen]
    text = '\n'.join(p.read_text(encoding='utf-8', errors='ignore') for p in scen_text_files)
    for req in all_requirements:
        if req in text:
            req_links[req].add(scen)
for comp in index['component']:
    comp_in_files = occurrence['component'][comp]
    text = '\n'.join(p.read_text(encoding='utf-8', errors='ignore') for p in comp_in_files)
    for req in all_requirements:
        if req in text:
            req_links[req].add(comp)
for test in index['test']:
    # naive: if test id includes requirement id substring (rare) skip, else can't infer; placeholder
    pass

# Orphan detection
orphans = {
    'requirements_no_links': sorted([r for r in all_requirements if not req_links.get(r)]),
    'scenarios_no_req': sorted([
        s for s in index['scenario']
        if not any(s in links for links in req_links.values())
    ]),
    'components_no_req': sorted([
        c for c in index['component']
        if not any(c in links for links in req_links.values())
    ]),
    'adrs_no_req': sorted([
        a for a in index['adr']
        if not any(a in links for links in req_links.values())
    ]),
}

# Allow empty repository without producing noise
if not any(index.values()) and os.environ.get('ALLOW_EMPTY_SPECS'):
    (REPORTS / 'traceability-matrix.md').write_text('# Traceability Matrix\n\n_No governed spec items found (empty scaffold mode)._', encoding='utf-8')
    (REPORTS / 'orphans.md').write_text('# Orphan Analysis\n\n_No governed spec items found (empty scaffold mode)._', encoding='utf-8')
    print('Empty scaffold: generated placeholder traceability outputs.')
    raise SystemExit(0)

matrix_lines = [
    '# Traceability Matrix (Heuristic Draft)',
    '',
    '| Requirement | Linked Elements (ADR / Component / Scenario / Test) |',
    '|-------------|----------------------------------------------------|',
]
for req in sorted(all_requirements):
    linked = ', '.join(sorted(req_links.get(req, []))) or '(none)'
    matrix_lines.append(f'| {req} | {linked} |')

(REPORTS / 'traceability-matrix.md').write_text('\n'.join(matrix_lines), encoding='utf-8')

orphan_lines = ['# Orphan Analysis', '']
for category, items in orphans.items():
    orphan_lines.append(f'## {category}')
    if not items:
        orphan_lines.append('- None')
    else:
        for item in items:
            orphan_lines.append(f'- {item}')
    orphan_lines.append('')
(REPORTS / 'orphans.md').write_text('\n'.join(orphan_lines), encoding='utf-8')

print('Generated reports/traceability-matrix.md and reports/orphans.md')
