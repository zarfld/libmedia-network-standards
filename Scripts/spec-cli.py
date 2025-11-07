#!/usr/bin/env python3
"""CLI tool for spec validation and management.

This comprehensive tool provides:
- Validation of existing specs against schemas
- Creation of new specs with guided prompts
- Bulk operations (validate all, fix common issues)
- Integration with CI/CD pipelines

Usage:
  # Validate all specs
  python scripts/spec-cli.py validate

  # Validate specific file
  python scripts/spec-cli.py validate path/to/spec.md

  # Create new spec interactively
  python scripts/spec-cli.py create requirements

  # Check numbering
  python scripts/spec-cli.py check-numbering

  # Generate report
  python scripts/spec-cli.py report --format markdown

"""
from __future__ import annotations
import sys
import argparse
from pathlib import Path
from typing import List, Optional

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'scripts'))

try:
    from validate_spec_structure import validate_spec, discover_targets, ValidationIssue
    from check_spec_numbering import analyze_numbering, print_report as print_numbering_report
    from create_spec import main as create_spec_main, SPEC_TYPES
except ImportError as e:
    print(f"❌ Missing dependency: {e}", file=sys.stderr)
    print("Ensure scripts are in the scripts/ directory", file=sys.stderr)
    sys.exit(2)


def cmd_validate(args: argparse.Namespace) -> int:
    """Validate spec files."""
    print("🔍 Validating specification files...")
    print("=" * 60)
    
    if args.files:
        targets = [Path(f).resolve() for f in args.files]
    else:
        targets = discover_targets([])
    
    if not targets:
        print("❌ No spec files found to validate")
        return 1
    
    print(f"📋 Found {len(targets)} file(s) to validate\n")
    
    all_issues: List[ValidationIssue] = []
    validated_count = 0
    warning_count = 0
    
    for path in targets:
        if not path.exists():
            print(f"⚠️  File not found: {path}")
            continue
        
        rel_path = path.relative_to(ROOT) if path.is_relative_to(ROOT) else path
        
        try:
            issues, warnings = validate_spec(path)
            
            if warnings:
                warning_count += len(warnings)
                if args.verbose:
                    for warning in warnings:
                        print(warning)
            
            if issues:
                print(f"❌ {rel_path}")
                for issue in issues:
                    print(f"   • {issue.message}")
                all_issues.extend(issues)
            else:
                if args.verbose:
                    print(f"✅ {rel_path}")
                validated_count += 1
        
        except Exception as e:
            print(f"❌ {rel_path}: Validation error: {e}")
            all_issues.append(ValidationIssue(path, str(e)))
    
    print()
    print("=" * 60)
    print(f"📊 Results:")
    print(f"  ✅ Valid: {validated_count}")
    print(f"  ❌ Invalid: {len(all_issues)}")
    if warning_count > 0:
        print(f"  ⚠️  Warnings: {warning_count}")
    
    if all_issues:
        print()
        print("❌ Validation failed")
        return 1
    
    print()
    print("✅ All specs validated successfully")
    return 0


def cmd_create(args: argparse.Namespace) -> int:
    """Create new spec file."""
    # Delegate to create-spec.py
    create_args = [args.spec_type]
    if args.interactive:
        create_args.append('--interactive')
    
    return create_spec_main(create_args)


def cmd_check_numbering(args: argparse.Namespace) -> int:
    """Check ID numbering for gaps and duplicates."""
    from check_spec_numbering import main as check_numbering_main
    
    check_args = []
    if args.strict:
        check_args.append('--strict')
    
    return check_numbering_main(check_args)


def cmd_report(args: argparse.Namespace) -> int:
    """Generate compliance report."""
    print("📊 Generating Specification Compliance Report")
    print("=" * 60)
    print()
    
    # Validate all specs
    targets = discover_targets([])
    
    validated = 0
    failed = 0
    warnings = 0
    
    for path in targets:
        try:
            issues, warns = validate_spec(path)
            if issues:
                failed += 1
            else:
                validated += 1
            warnings += len(warns)
        except Exception:
            failed += 1
    
    # Check numbering
    numbering_analysis = analyze_numbering()
    duplicates = len(numbering_analysis['duplicates'])
    gaps = sum(len(g) for g in numbering_analysis['gaps'].values())
    
    # Generate report
    if args.format == 'markdown':
        print("# Specification Compliance Report")
        print()
        print(f"**Generated**: {Path.cwd()}")
        print()
        print("## Validation Summary")
        print()
        print(f"- Total specs: {validated + failed}")
        print(f"- ✅ Valid: {validated}")
        print(f"- ❌ Invalid: {failed}")
        print(f"- ⚠️  Warnings: {warnings}")
        print()
        print("## ID Numbering")
        print()
        print(f"- Total IDs: {len(numbering_analysis['id_locations'])}")
        print(f"- ❌ Duplicates: {duplicates}")
        print(f"- ⚠️  Gaps: {gaps}")
        print()
        print("## Compliance Score")
        print()
        total = validated + failed
        if total > 0:
            score = (validated / total) * 100
            print(f"**{score:.1f}%** ({validated}/{total} specs compliant)")
        else:
            print("No specs found")
    
    else:  # text format
        print("Validation Summary:")
        print(f"  Total specs: {validated + failed}")
        print(f"  Valid: {validated}")
        print(f"  Invalid: {failed}")
        print(f"  Warnings: {warnings}")
        print()
        print("ID Numbering:")
        print(f"  Total IDs: {len(numbering_analysis['id_locations'])}")
        print(f"  Duplicates: {duplicates}")
        print(f"  Gaps: {gaps}")
    
    return 0


def main(argv: List[str]) -> int:
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description='Specification validation and management CLI',
        epilog='For detailed help on a command: spec-cli.py <command> --help'
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Validate command
    validate_parser = subparsers.add_parser('validate', help='Validate spec files')
    validate_parser.add_argument('files', nargs='*', help='Files to validate (default: all)')
    validate_parser.add_argument('-v', '--verbose', action='store_true', help='Verbose output')
    
    # Create command
    create_parser = subparsers.add_parser('create', help='Create new spec file')
    create_parser.add_argument('spec_type', choices=list(SPEC_TYPES.keys()), help='Type of spec to create')
    create_parser.add_argument('-i', '--interactive', action='store_true', help='Interactive mode')
    
    # Check numbering command
    numbering_parser = subparsers.add_parser('check-numbering', help='Check ID numbering')
    numbering_parser.add_argument('--strict', action='store_true', help='Treat gaps as errors')
    
    # Report command
    report_parser = subparsers.add_parser('report', help='Generate compliance report')
    report_parser.add_argument('--format', choices=['text', 'markdown'], default='text', help='Output format')
    
    args = parser.parse_args(argv)
    
    if not args.command:
        parser.print_help()
        return 0
    
    # Execute command
    if args.command == 'validate':
        return cmd_validate(args)
    elif args.command == 'create':
        return cmd_create(args)
    elif args.command == 'check-numbering':
        return cmd_check_numbering(args)
    elif args.command == 'report':
        return cmd_report(args)
    else:
        print(f"❌ Unknown command: {args.command}", file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main(sys.argv[1:]))
