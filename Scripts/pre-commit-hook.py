#!/usr/bin/env python3
"""Pre-commit hook for spec validation.

This hook validates that all committed spec files:
- Have valid YAML front matter
- Pass JSON schema validation
- Have proper ID formatting
- Follow naming conventions

Install:
  Copy to .git/hooks/pre-commit (or use with pre-commit framework)
  chmod +x .git/hooks/pre-commit

Usage with pre-commit framework:
  Add to .pre-commit-config.yaml:
    - repo: local
      hooks:
        - id: validate-specs
          name: Validate Specification Files
          entry: python scripts/pre-commit-hook.py
          language: python
          files: \\.(md)$
          exclude: (README|template|TEMPLATE)
"""
from __future__ import annotations
import sys
import subprocess
from pathlib import Path

# Import validation logic from existing script
ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'scripts'))

try:
    from validate_spec_structure import validate_spec, ValidationIssue, ROOT as VALIDATE_ROOT
except ImportError:
    print("❌ Could not import validation module", file=sys.stderr)
    sys.exit(1)


def get_staged_spec_files() -> list[Path]:
    """Get list of staged markdown files that might be specs."""
    try:
        result = subprocess.run(
            ['git', 'diff', '--cached', '--name-only', '--diff-filter=ACM'],
            capture_output=True,
            text=True,
            check=True
        )
        
        staged_files = result.stdout.strip().split('\n')
        
        # Filter for markdown files in phase directories
        spec_files = []
        for file_str in staged_files:
            if not file_str:
                continue
            
            file_path = ROOT / file_str
            
            # Skip if not markdown
            if not file_path.suffix == '.md':
                continue
            
            # Skip READMEs and templates
            name_lower = file_path.name.lower()
            if 'readme' in name_lower or 'template' in name_lower:
                continue
            
            # Check if in phase directory
            parts = file_path.parts
            if any(p.startswith(('01-', '02-', '03-', '04-', '05-', '06-', '07-', '08-', '09-')) for p in parts):
                spec_files.append(file_path)
        
        return spec_files
    
    except subprocess.CalledProcessError as e:
        print(f"❌ Failed to get staged files: {e}", file=sys.stderr)
        return []


def main() -> int:
    """Validate staged spec files."""
    print("🔍 Validating staged specification files...")
    
    staged_files = get_staged_spec_files()
    
    if not staged_files:
        print("✅ No spec files to validate")
        return 0
    
    print(f"📋 Found {len(staged_files)} spec file(s) to validate")
    
    all_issues: list[ValidationIssue] = []
    validated_count = 0
    
    for file_path in staged_files:
        if not file_path.exists():
            continue
        
        print(f"  Checking: {file_path.relative_to(ROOT)}")
        
        try:
            issues, warnings = validate_spec(file_path)
            
            # Print warnings
            for warning in warnings:
                print(f"    ⚠️  {warning}")
            
            # Collect errors
            if issues:
                for issue in issues:
                    print(f"    ❌ {issue.message}")
                all_issues.extend(issues)
            else:
                print(f"    ✅ Valid")
                validated_count += 1
        
        except Exception as e:
            print(f"    ❌ Validation error: {e}")
            all_issues.append(ValidationIssue(file_path, str(e)))
    
    print()
    print("=" * 60)
    
    if all_issues:
        print(f"❌ Validation failed: {len(all_issues)} issue(s) found")
        print()
        print("Fix the issues above and try committing again.")
        print("To bypass validation (not recommended): git commit --no-verify")
        return 1
    
    print(f"✅ All {validated_count} spec file(s) validated successfully")
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
