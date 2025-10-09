#!/usr/bin/env python3
"""Generate a simple traceability matrix skeleton and orphan report.
Parses markdown files for ID patterns and produces:
 - reports/traceability-matrix.md (basic table placeholders)
 - reports/orphans.md (lists missing link elements)

Patterns recognized:
  StR-\d{3}
  REQ-(F|NF)-\d{3}
  ADR-\d{3}
  ARC-C-\d{3}
  QA-SC-\d{3}
  TEST-[A-Z0-9-]+ (any test id with prefix TEST-)

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
    # Legacy patterns (deprecated - to be migrated)
    'stakeholder_legacy': re.compile(r'StR-\d{3}'),
    'requirement_legacy': re.compile(r'REQ-(?:F|NF)-\d{3}'),
    
    # NEW NAMING CONVENTION PATTERNS (Standards-Specific)
    # Stakeholder Requirements: REQ-STK-{STANDARD}-{NNN}
    'stakeholder': re.compile(r'REQ-STK-[A-Z0-9]+(?:[A-Z0-9]+)*-\d{3}'),
    
    # System Requirements: REQ-SYS-{STANDARD}-{NNN}  
    'system': re.compile(r'REQ-SYS-[A-Z0-9]+(?:[A-Z0-9]+)*-\d{3}'),
    
    # Functional Requirements: REQ-FUN-{STANDARD}-{NNN}
    'functional': re.compile(r'REQ-FUN-[A-Z0-9]+(?:[A-Z0-9]+)*-\d{3}'),
    
    # Non-Functional Requirements: REQ-NFR-{STANDARD}-{NNN}
    'nonfunctional': re.compile(r'REQ-NFR-[A-Z0-9]+(?:[A-Z0-9]+)*-\d{3}'),
    
    # Interface Requirements: IR-{STANDARD}-{NNN}
    'interface': re.compile(r'IR-[A-Z0-9]+(?:[A-Z0-9]+)*-\d{3}'),
    
    # Verification Requirements: VR-{STANDARD}-{NNN}  
    'verification': re.compile(r'VR-[A-Z0-9]+(?:[A-Z0-9]+)*-\d{3}'),
    
    # Acceptance Criteria: AC-{STANDARD}-{NNN}
    'acceptance': re.compile(r'AC-[A-Z0-9]+(?:[A-Z0-9]+)*-\d{3}'),
    
    # Use Cases: UC-{STANDARD}-{NNN}
    'usecase': re.compile(r'UC-[A-Z0-9]+(?:[A-Z0-9]+)*-\d{3}'),
    
    # User Stories: {STANDARD}-US-{NNN}
    'userstory': re.compile(r'[A-Z0-9]+(?:[A-Z0-9]+)*-US-\d{3}'),
    
    # Architecture and other patterns (unchanged)
    'adr': re.compile(r'ADR-\d{3}'),
    'component': re.compile(r'ARC-C-\d{3}'),
    'scenario': re.compile(r'QA-SC-\d{3}'),
    'test': re.compile(r'TEST-[A-Z0-9-]+'),
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
