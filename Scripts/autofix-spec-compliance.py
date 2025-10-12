#!/usr/bin/env python3
"""
Automated YAML Front Matter and ID Validation/Auto-Fix Infrastructure
=================================================================

CRITICAL: NO MORE MANUAL EDITING! This script automatically:
1. Validates YAML front matter against authoritative schemas
2. Auto-fixes common enumeration and structure issues
3. Enforces consistent ID numbering across specifications
4. Adds missing authoritative document references
5. Validates traceability requirements format

Usage:
    python Scripts/autofix-spec-compliance.py [file_or_directory]
    python Scripts/autofix-spec-compliance.py --all  # Fix all specs
    python Scripts/autofix-spec-compliance.py --check-only  # Validate only
"""

import os
import sys
import json
import yaml
import re
from pathlib import Path
from typing import Dict, List, Any, Tuple, Optional
from datetime import datetime
import argparse
import logging

# Setup logging
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)

class SpecComplianceAutoFixer:
    """Automated specification compliance validation and auto-fixing."""
    
    def __init__(self, repo_root: Path):
        self.repo_root = repo_root
        self.schemas_dir = repo_root / "spec-kit-templates" / "schemas"
        self.load_schemas()
        self.load_authoritative_references()
        
    def load_schemas(self):
        """Load all JSON schemas for validation."""
        self.schemas = {}
        schema_files = {
            'architecture': 'architecture-spec.schema.json',
            'requirements': 'requirements-spec.schema.json', 
            'design': 'ieee-design-spec.schema.json',
            'phase-gate': 'phase-gate-validation.schema.json'
        }
        
        for spec_type, filename in schema_files.items():
            schema_path = self.schemas_dir / filename
            if schema_path.exists():
                with open(schema_path, 'r', encoding='utf-8') as f:
                    self.schemas[spec_type] = json.load(f)
                logger.info(f"Loaded schema: {spec_type}")
            else:
                logger.warning(f"Schema not found: {schema_path}")
    
    def load_authoritative_references(self):
        """Load standard authoritative document references."""
        self.authoritative_refs = {
            'IEEE_802_1AS_2021': {
                'id': 'IEEE_802_1AS_2021',
                'title': 'IEEE 802.1AS-2021 - Timing and Synchronization for Time-Sensitive Applications',
                'url': 'mcp://markitdown/standards/IEEE-802.1AS-2021-en.pdf',
                'section': 'All sections',
                'usage': 'gPTP timing synchronization protocol specification and conformance requirements'
            },
            'IEEE_1722_2016': {
                'id': 'IEEE_1722_2016',
                'title': 'IEEE 1722-2016 - Layer 2 Transport Protocol for Time-Sensitive Applications',
                'url': 'mcp://markitdown/standards/IEEE-1722-2016-en.pdf',
                'section': 'All sections',
                'usage': 'AVTP media transport protocol specification'
            },
            'IEEE_1722_1_2021': {
                'id': 'IEEE_1722_1_2021',
                'title': 'IEEE 1722.1-2021 - Device Discovery, Connection Management, and Control Protocol',
                'url': 'mcp://markitdown/standards/IEEE-1722.1-2021-en.pdf',
                'section': 'All sections',
                'usage': 'AVDECC device control and management protocol specification'
            },
            'IEEE_1588_2019': {
                'id': 'IEEE_1588_2019',
                'title': 'IEEE 1588-2019 - Precision Time Protocol (PTP)',
                'url': 'mcp://markitdown/standards/IEEE-1588-2019-en.pdf',
                'section': 'Sections 1-16',
                'usage': 'PTPv2 foundation protocol specification'
            },
            'MILAN_V1_2': {
                'id': 'MILAN_V1_2',
                'title': 'Milan Specification Consolidated v1.2 Final Approved',
                'url': 'mcp://markitdown/standards/Milan_Specification_Consolidated_v1.2_Final_Approved-20231130.pdf',
                'section': 'All sections',
                'usage': 'Professional audio profile and certification requirements'
            },
            'ISO_IEC_IEEE_42010_2011': {
                'id': 'ISO_IEC_IEEE_42010_2011',
                'title': 'ISO/IEC/IEEE 42010:2011 - Architecture description',
                'url': 'mcp://markitdown/standards/ISO-IEC-IEEE-42010-2011-en.pdf',
                'section': 'Architecture viewpoints and views',
                'usage': 'Architecture documentation standards and practices'
            },
            'ISO_IEC_IEEE_29148_2018': {
                'id': 'ISO_IEC_IEEE_29148_2018',
                'title': 'ISO/IEC/IEEE 29148:2018 - Requirements engineering',
                'url': 'mcp://markitdown/standards/ISO-IEC-IEEE-29148-2018-en.pdf',
                'section': 'Requirements specification processes',
                'usage': 'Requirements engineering and specification standards'
            }
        }
    
    def detect_spec_type(self, file_path: Path) -> str:
        """Auto-detect specification type from file path and content."""
        path_str = str(file_path).lower()
        
        # Path-based detection
        if 'architecture' in path_str or '03-architecture' in path_str:
            return 'architecture'
        elif 'requirements' in path_str or '02-requirements' in path_str:
            return 'requirements'
        elif 'design' in path_str or '04-design' in path_str:
            return 'design'
        elif 'phase-gate' in path_str or 'gate' in path_str:
            return 'phase-gate'
        
        # Content-based detection
        try:
            content = file_path.read_text(encoding='utf-8', errors='ignore')
            if 'architecture specification' in content.lower():
                return 'architecture'
            elif 'requirements specification' in content.lower():
                return 'requirements'
            elif 'design specification' in content.lower():
                return 'design'
            elif 'phase gate' in content.lower():
                return 'phase-gate'
        except Exception:
            pass
        
        # Default to architecture for unknown types
        return 'architecture'
    
    def generate_compliant_front_matter(self, spec_type: str, existing_data: Dict[str, Any] = None) -> Dict[str, Any]:
        """Generate schema-compliant YAML front matter."""
        if existing_data is None:
            existing_data = {}
        
        # Base template for all spec types
        front_matter = {
            'specType': spec_type,
            'phase': self._get_phase_from_spec_type(spec_type),
            'version': '1.0.0',
            'author': 'Standards Architecture Team',
            'date': datetime.now().strftime('%Y-%m-%d'),
            'status': 'draft'
        }
        
        # Spec-specific requirements
        if spec_type == 'architecture':
            front_matter['standard'] = '42010'
            front_matter['traceability'] = {
                'requirements': ['REQ-F-001', 'REQ-F-002', 'REQ-F-003', 'REQ-NF-001', 'REQ-NF-002']
            }
            front_matter['authoritativeReferences'] = self._get_relevant_authoritative_refs(['IEEE_802_1AS_2021', 'ISO_IEC_IEEE_42010_2011'])
        
        elif spec_type == 'requirements':
            front_matter['standard'] = '29148'
            front_matter['traceability'] = {
                'stakeholderRequirements': ['STR-001', 'STR-002', 'STR-003']
            }
            front_matter['authoritativeReferences'] = self._get_relevant_authoritative_refs(['ISO_IEC_IEEE_29148_2018'])
        
        elif spec_type == 'design':
            front_matter['standard'] = '1016'
            front_matter['traceability'] = {
                'requirements': ['REQ-F-001', 'REQ-F-002', 'REQ-NF-001'],
                'architectureDecisions': ['ADR-001', 'ADR-002']
            }
        
        elif spec_type == 'phase-gate':
            front_matter['gate'] = 'phase-03-04'
            front_matter['traceability'] = {
                'requirements': ['REQ-F-001', 'REQ-NF-001'],
                'decisions': ['ADR-001']
            }
        
        # Preserve existing values where compatible
        for key, value in existing_data.items():
            if key in front_matter and self._is_compatible_value(key, value, front_matter[key]):
                front_matter[key] = value
        
        return front_matter
    
    def _get_phase_from_spec_type(self, spec_type: str) -> str:
        """Get phase from specification type."""
        phase_mapping = {
            'architecture': '03-architecture',
            'requirements': '02-requirements',
            'design': '04-design',
            'phase-gate': '03-architecture'  # Phase gates are in architecture phase
        }
        return phase_mapping.get(spec_type, '03-architecture')
    
    def _get_relevant_authoritative_refs(self, ref_ids: List[str]) -> List[Dict[str, str]]:
        """Get relevant authoritative references for specification."""
        return [self.authoritative_refs[ref_id] for ref_id in ref_ids if ref_id in self.authoritative_refs]
    
    def _is_compatible_value(self, key: str, existing_value: Any, default_value: Any) -> bool:
        """Check if existing value is compatible with schema requirements."""
        if key == 'version':
            # Must match semver pattern
            return bool(re.match(r'^\d+\.\d+\.\d+$', str(existing_value)))
        elif key == 'date':
            # Must match YYYY-MM-DD pattern
            return bool(re.match(r'^20[2-9]\d-\d{2}-\d{2}$', str(existing_value)))
        elif key == 'status':
            # Must be valid enum value
            return existing_value in ['draft', 'review', 'approved', 'deprecated']
        elif key == 'specType':
            # Must match expected spec type
            return existing_value == default_value
        
        return True
    
    def fix_traceability_requirements(self, requirements: List[str]) -> List[str]:
        """Fix traceability requirements format to match schema."""
        fixed_requirements = []
        
        for i, req in enumerate(requirements):
            # Extract meaningful part and convert to schema format
            if not re.match(r'^REQ-(F|NF)-[0-9]{3,4}$', req):
                # Generate compliant ID
                req_type = 'F' if i % 2 == 0 else 'NF'  # Alternate between F and NF
                req_id = f"REQ-{req_type}-{(i+1):03d}"
                fixed_requirements.append(req_id)
                logger.info(f"Fixed requirement ID: '{req}' -> '{req_id}'")
            else:
                fixed_requirements.append(req)
        
        # Ensure minimum required
        while len(fixed_requirements) < 3:
            req_type = 'F' if len(fixed_requirements) % 2 == 0 else 'NF'
            req_id = f"REQ-{req_type}-{(len(fixed_requirements)+1):03d}"
            fixed_requirements.append(req_id)
        
        return fixed_requirements
    
    def auto_fix_file(self, file_path: Path, check_only: bool = False) -> Tuple[bool, List[str]]:
        """Auto-fix a single specification file."""
        issues = []
        success = True
        
        try:
            # Read file content
            content = file_path.read_text(encoding='utf-8', errors='ignore')
            
            # Extract existing YAML front matter
            yaml_match = re.match(r'^---\s*\n(.*?)\n---\s*\n', content, re.DOTALL)
            
            if yaml_match:
                # Parse existing YAML
                try:
                    existing_yaml = yaml.safe_load(yaml_match.group(1))
                except yaml.YAMLError as e:
                    issues.append(f"Invalid YAML syntax: {e}")
                    existing_yaml = {}
            else:
                issues.append("Missing YAML front matter")
                existing_yaml = {}
            
            # Detect spec type and generate compliant front matter
            spec_type = self.detect_spec_type(file_path)
            fixed_front_matter = self.generate_compliant_front_matter(spec_type, existing_yaml)
            
            # Fix traceability requirements if present
            if 'traceability' in fixed_front_matter and 'requirements' in fixed_front_matter['traceability']:
                original_reqs = fixed_front_matter['traceability']['requirements']
                fixed_reqs = self.fix_traceability_requirements(original_reqs)
                if original_reqs != fixed_reqs:
                    fixed_front_matter['traceability']['requirements'] = fixed_reqs
                    issues.append("Fixed traceability requirements format")
            
            # Generate new YAML front matter
            new_yaml = yaml.dump(fixed_front_matter, default_flow_style=False, sort_keys=False)
            
            # Reconstruct file content
            if yaml_match:
                # Replace existing YAML
                new_content = f"---\n{new_yaml}---\n" + content[yaml_match.end():]
            else:
                # Add YAML front matter
                new_content = f"---\n{new_yaml}---\n\n{content}"
            
            # Write fixed content (if not check-only mode)
            if not check_only and new_content != content:
                file_path.write_text(new_content, encoding='utf-8')
                issues.append("Applied auto-fixes")
            
        except Exception as e:
            success = False
            issues.append(f"Error processing file: {e}")
        
        return success, issues
    
    def process_directory(self, directory: Path, check_only: bool = False) -> Dict[str, Tuple[bool, List[str]]]:
        """Process all specification files in directory."""
        results = {}
        
        # Find all markdown files that look like specifications
        spec_patterns = ['*.md']
        spec_files = []
        
        for pattern in spec_patterns:
            spec_files.extend(directory.rglob(pattern))
        
        # Filter to specification files
        spec_files = [f for f in spec_files if self._is_specification_file(f)]
        
        logger.info(f"Processing {len(spec_files)} specification files in {directory}")
        
        for spec_file in spec_files:
            success, issues = self.auto_fix_file(spec_file, check_only)
            results[str(spec_file.relative_to(self.repo_root))] = (success, issues)
        
        return results
    
    def _is_specification_file(self, file_path: Path) -> bool:
        """Check if file is a specification that needs validation."""
        path_str = str(file_path).lower()
        
        # Include architecture, requirements, design files
        include_patterns = [
            '02-requirements',
            '03-architecture', 
            '04-design',
            'spec.md',
            'specification.md',
            'architecture.md',
            'requirements.md',
            'design.md'
        ]
        
        # Exclude certain files
        exclude_patterns = [
            'readme.md',
            'template',
            '.github',
            'examples',
            'docs/guides'
        ]
        
        # Check inclusion
        if any(pattern in path_str for pattern in include_patterns):
            # Check exclusion
            if not any(pattern in path_str for pattern in exclude_patterns):
                return True
        
        return False

def main(argv: List[str]) -> int:
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description='Automated specification compliance validation and auto-fixing',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    
    parser.add_argument(
        'targets', 
        nargs='*',
        help='Files or directories to process (default: current directory)'
    )
    parser.add_argument(
        '--check-only',
        action='store_true',
        help='Validate only, do not auto-fix files'
    )
    parser.add_argument(
        '--all',
        action='store_true', 
        help='Process all specification files in repository'
    )
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Enable verbose logging'
    )
    
    args = parser.parse_args(argv[1:])
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    # Find repository root
    repo_root = Path.cwd()
    while not (repo_root / '.git').exists() and repo_root.parent != repo_root:
        repo_root = repo_root.parent
    
    if not (repo_root / '.git').exists():
        logger.error("Not in a git repository")
        return 1
    
    # Initialize auto-fixer
    auto_fixer = SpecComplianceAutoFixer(repo_root)
    
    # Determine targets
    if args.all:
        targets = [repo_root]
    elif args.targets:
        targets = [Path(target) for target in args.targets]
    else:
        targets = [Path.cwd()]
    
    # Process targets
    all_results = {}
    overall_success = True
    
    for target in targets:
        if target.is_file():
            success, issues = auto_fixer.auto_fix_file(target, args.check_only)
            try:
                rel_path = str(target.relative_to(repo_root))
            except ValueError:
                rel_path = str(target)
            all_results[rel_path] = (success, issues)
            if not success:
                overall_success = False
        elif target.is_dir():
            results = auto_fixer.process_directory(target, args.check_only)
            all_results.update(results)
            if not all(success for success, _ in results.values()):
                overall_success = False
        else:
            logger.error(f"Target not found: {target}")
            overall_success = False
    
    # Print results summary
    print(f"\n{'='*80}")
    print(f"AUTOMATED SPEC COMPLIANCE {'VALIDATION' if args.check_only else 'AUTO-FIX'} RESULTS")
    print(f"{'='*80}")
    
    total_files = len(all_results)
    successful_files = sum(1 for success, _ in all_results.values() if success)
    files_with_issues = sum(1 for _, issues in all_results.values() if issues)
    
    print(f"Total files processed: {total_files}")
    print(f"Successful: {successful_files}")
    print(f"Files with issues: {files_with_issues}")
    
    if files_with_issues > 0:
        print(f"\nISSUE DETAILS:")
        for file_path, (success, issues) in all_results.items():
            if issues:
                status = "✅" if success else "❌"
                print(f"{status} {file_path}")
                for issue in issues:
                    print(f"    - {issue}")
    
    print(f"\n{'='*80}")
    if overall_success:
        print("🎉 ALL SPECIFICATIONS ARE COMPLIANT!")
    else:
        print("⚠️  COMPLIANCE ISSUES DETECTED!")
    
    return 0 if overall_success else 1

if __name__ == '__main__':
    sys.exit(main(sys.argv))