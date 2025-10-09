#!/usr/bin/env python3
"""Validate spec YAML front matter against JSON Schemas.

Usage:
  python scripts/validate-spec-structure.py [path ...]
If no paths supplied, scans spec-kit-templates usage patterns & phase folders.

Exit codes:
 0 success
 1 validation errors
 2 internal error
"""
from __future__ import annotations
import sys, re, json, subprocess, pathlib, typing as t, os

try:
    import yaml  # type: ignore
except ImportError:
    print("Missing dependency pyyaml. Install with: pip install pyyaml jsonschema", file=sys.stderr)
    sys.exit(2)
try:
    import jsonschema  # type: ignore
except ImportError:
    print("Missing dependency jsonschema. Install with: pip install jsonschema", file=sys.stderr)
    sys.exit(2)

ROOT = pathlib.Path(__file__).resolve().parent.parent
SCHEMA_DIR = ROOT / 'spec-kit-templates' / 'schemas'
SCHEMA_MAP = {
    'requirements': SCHEMA_DIR / 'requirements-spec.schema.json',
    'architecture': SCHEMA_DIR / 'architecture-spec.schema.json',
    # 'guidance' intentionally has no strict schema – treated leniently
}

FRONT_MATTER_RE = re.compile(r'^---\n(.*?)\n---', re.DOTALL)

class ValidationIssue(t.NamedTuple):
    file: pathlib.Path
    message: str


def extract_front_matter(text: str) -> t.Optional[str]:
    m = FRONT_MATTER_RE.match(text)
    return m.group(1) if m else None


def parse_yaml_block(block: str) -> t.Optional[dict]:
    try:
        return yaml.safe_load(block) or {}
    except Exception as e:
        return None


def load_schema(spec_type: str) -> dict:
    p = SCHEMA_MAP.get(spec_type)
    if not p or not p.exists():
        raise FileNotFoundError(f"No schema for specType={spec_type}")
    return json.loads(p.read_text(encoding='utf-8'))


GUIDANCE_HINTS = [
    'copilot-instructions.md',
    'ADR-template.md',
    'template',
]

def is_guidance(path: pathlib.Path, meta: dict | None) -> bool:
    if meta and meta.get('specType') == 'guidance':
        return True
    lower = path.name.lower()
    return any(h in lower for h in GUIDANCE_HINTS)


def validate_spec(path: pathlib.Path) -> tuple[list[ValidationIssue], list[str]]:
    issues: list[ValidationIssue] = []
    warnings: list[str] = []
    text = path.read_text(encoding='utf-8', errors='ignore')
    fm_raw = extract_front_matter(text)
    if not fm_raw:
        # Guidance files without front matter -> warning, not error
        if any(k in path.as_posix() for k in ['.github/prompts']):
            return [], []  # ignore prompt library entirely
        if is_guidance(path, None):
            warnings.append(f"ℹ️ {path.relative_to(ROOT)}: guidance file missing front matter (ignored)")
            return [], warnings
        issues.append(ValidationIssue(path, 'Missing YAML front matter (--- block)'))
        return issues, warnings
    meta = parse_yaml_block(fm_raw)
    if meta is None:
        if is_guidance(path, None):
            warnings.append(f"ℹ️ {path.relative_to(ROOT)}: invalid YAML in guidance file (ignored)")
            return [], warnings
        issues.append(ValidationIssue(path, 'Invalid YAML front matter'))
        return issues, warnings
    spec_type = meta.get('specType')
    if not spec_type:
        if is_guidance(path, meta):
            warnings.append(f"ℹ️ {path.relative_to(ROOT)}: guidance file missing specType (ignored)")
            return [], warnings
        issues.append(ValidationIssue(path, 'Missing specType in front matter'))
        return issues, warnings
    if spec_type not in SCHEMA_MAP:
        # Non-governed specType -> soft skip
        warnings.append(f"ℹ️ {path.relative_to(ROOT)}: specType '{spec_type}' not governed; skipped")
        return [], warnings
    try:
        schema = load_schema(spec_type)
    except Exception as e:
        issues.append(ValidationIssue(path, f'Schema load error: {e}'))
        return issues, warnings
    validator = jsonschema.Draft7Validator(schema)
    for err in validator.iter_errors(meta):
        issues.append(ValidationIssue(path, f"Schema violation: {'/'.join(map(str, err.path)) or '<root>'}: {err.message}"))

    # Additional cross-field custom checks
    if spec_type == 'requirements':
        # Ensure at least one Functional (REQ-F-) or Non-Functional (REQ-NF-) ID present in body
        if not re.search(r'REQ-(F|NF)-\d{3}', text):
            issues.append(ValidationIssue(path, 'No REQ-* identifiers found in body'))
    if spec_type == 'architecture':
        # Ensure at least one ARC-C- or ADR reference
        if not re.search(r'ADR-\d{3}', text):
            # treat placeholder ADR-XXX as not sufficient
            if 'ADR-XXX' not in text:
                issues.append(ValidationIssue(path, 'No ADR-XXX references found in architecture spec'))
    return issues, warnings


def discover_targets(explicit: list[str]) -> list[pathlib.Path]:
    if explicit:
        return [pathlib.Path(p).resolve() for p in explicit]
    candidates: list[pathlib.Path] = []
    for pattern in [
        '02-requirements/**/*.md',
        '03-architecture/**/*.md',
    ]:
        candidates.extend(ROOT.glob(pattern))
    return [c for c in candidates if c.is_file()]


def main(argv: list[str]) -> int:
    targets = discover_targets(argv[1:])
    if not targets:
        print('No spec files found to validate', file=sys.stderr)
        return 0
    all_issues: list[ValidationIssue] = []
    governed_specs = 0
    for path in targets:
        if path.name.startswith('README'):  # skip readmes
            continue
        issues, warnings = validate_spec(path)
        for w in warnings:
            print(w)
        if issues:
            for issue in issues:
                print(f"❌ {issue.file.relative_to(ROOT)}: {issue.message}")
            all_issues.extend(issues)
        else:
            # Count only governed spec types (requirements / architecture)
            text = path.read_text(encoding='utf-8', errors='ignore')
            fm = extract_front_matter(text)
            meta = parse_yaml_block(fm) if fm else {}
            if meta and meta.get('specType') in ('requirements','architecture'):
                governed_specs += 1
            print(f"✅ {path.relative_to(ROOT)} valid")
    allow_empty = bool(os.environ.get('ALLOW_EMPTY_SPECS'))
    if governed_specs == 0 and allow_empty and not all_issues:
        print('ℹ️ No governed specs found; ALLOW_EMPTY_SPECS set -> passing without enforcement.')
        return 0
    if all_issues:
        print(f"\nFailed: {len(all_issues)} validation issues across {len(set(i.file for i in all_issues))} files.")
        return 1
    print("All specs validated successfully.")
    return 0

if __name__ == '__main__':  # pragma: no cover
    sys.exit(main(sys.argv))
