#!/usr/bin/env python3
"""
Architecture Decision Record (ADR) Format Validator
Validates ADR structure per ISO/IEC/IEEE 42010:2011 standards
Ensures architectural decisions are properly documented and traceable
"""

import sys
import pathlib
import re
import yaml
from typing import Dict, List, Tuple

class ADRFormatValidator:
    """Validates Architecture Decision Record format and content"""
    
    def __init__(self):
        self.required_sections = [
            "## Context",
            "## Decision", 
            "## Status",
            "## Rationale",
            "## Considered Alternatives"
        ]
        
        self.optional_sections = [
            "## Metadata",
            "## Implementation Guidelines",
            "## Verification Criteria",
            "## Dependencies",
            "## Notes"
        ]
        
        self.required_metadata_fields = [
            "adrId",
            "status",
            "author",
            "date"
        ]
        
        self.valid_statuses = [
            "proposed", "accepted", "deprecated", 
            "superseded", "rejected", "draft"
        ]
    
    def validate_adr(self, adr_file: pathlib.Path) -> Dict:
        """Validate a single ADR file for IEEE 42010:2011 compliance"""
        
        try:
            content = adr_file.read_text(encoding='utf-8')
        except Exception as e:
            return {
                "valid": False,
                "errors": [f"Cannot read file: {e}"],
                "warnings": [],
                "suggestions": []
            }
        
        errors = []
        warnings = []
        suggestions = []
        
        # Check YAML front matter
        yaml_errors, yaml_warnings = self._validate_yaml_frontmatter(content)
        errors.extend(yaml_errors)
        warnings.extend(yaml_warnings)
        
        # Check ADR structure
        structure_errors, structure_warnings = self._validate_adr_structure(content)
        errors.extend(structure_errors)
        warnings.extend(structure_warnings)
        
        # Check content quality
        quality_warnings, quality_suggestions = self._validate_content_quality(content)
        warnings.extend(quality_warnings)
        suggestions.extend(quality_suggestions)
        
        # Check traceability
        trace_warnings = self._validate_traceability(content)
        warnings.extend(trace_warnings)
        
        # Generate suggestions for fixes
        if errors or warnings:
            fix_suggestions = self._generate_fix_suggestions(errors, warnings)
            suggestions.extend(fix_suggestions)
        
        return {
            "valid": len(errors) == 0,
            "errors": errors,
            "warnings": warnings, 
            "suggestions": suggestions
        }
    
    def _validate_yaml_frontmatter(self, content: str) -> Tuple[List[str], List[str]]:
        """Validate YAML front matter per specification standards"""
        errors = []
        warnings = []
        
        # Check for YAML front matter
        if not content.strip().startswith('---'):
            errors.append("Missing YAML front matter (should start with ---)")
            return errors, warnings
        
        try:
            # Extract YAML front matter
            parts = content.split('---', 2)
            if len(parts) < 3:
                errors.append("YAML front matter not properly closed with ---")
                return errors, warnings
            
            yaml_content = parts[1].strip()
            metadata = yaml.safe_load(yaml_content)
            
            if not isinstance(metadata, dict):
                errors.append("YAML front matter must be a dictionary")
                return errors, warnings
            
            # Check required fields
            for field in self.required_metadata_fields:
                if field not in metadata:
                    errors.append(f"Missing required metadata field: {field}")
            
            # Validate specific fields
            if 'specType' in metadata and metadata['specType'] != 'architecture':
                errors.append("ADR files must have specType: architecture")
            
            if 'standard' in metadata and 'ISO/IEC/IEEE 42010:2011' not in metadata['standard']:
                warnings.append("ADR should reference ISO/IEC/IEEE 42010:2011 standard")
            
            if 'status' in metadata and metadata['status'] not in self.valid_statuses:
                errors.append(f"Invalid status '{metadata['status']}'. Valid: {', '.join(self.valid_statuses)}")
                
        except yaml.YAMLError as e:
            errors.append(f"Invalid YAML front matter: {e}")
        
        return errors, warnings
    
    def _validate_adr_structure(self, content: str) -> Tuple[List[str], List[str]]:
        """Validate ADR section structure per IEEE 42010:2011"""
        errors = []
        warnings = []
        
        # Check required sections
        for section in self.required_sections:
            if section not in content:
                errors.append(f"Missing required section: {section}")
        
        # Check section order
        section_positions = []
        for section in self.required_sections:
            match = re.search(rf'^{re.escape(section)}', content, re.MULTILINE)
            if match:
                section_positions.append((section, match.start()))
        
        # Verify sections are in logical order
        if len(section_positions) > 1:
            sorted_positions = sorted(section_positions, key=lambda x: x[1])
            expected_order = [
                "## Context", "## Decision", "## Status", 
                "## Rationale", "## Considered Alternatives"
            ]
            
            actual_order = [pos[0] for pos in sorted_positions if pos[0] in expected_order]
            expected_present = [sec for sec in expected_order if sec in actual_order]
            
            if actual_order != expected_present:
                warnings.append("Sections not in recommended order. Expected: Context → Decision → Status → Rationale → Alternatives")
        
        # Check for ADR ID in title
        title_match = re.search(r'^#\s+ADR-\d+:', content, re.MULTILINE)
        if not title_match:
            warnings.append("ADR title should include ID (e.g., # ADR-001: Decision Title)")
        
        return errors, warnings
    
    def _validate_content_quality(self, content: str) -> Tuple[List[str], List[str]]:
        """Validate content quality and completeness"""
        warnings = []
        suggestions = []
        
        # Check section content length (avoid empty sections)
        for section in self.required_sections:
            section_match = re.search(rf'^{re.escape(section)}(.*?)(?=^##|\Z)', content, re.MULTILINE | re.DOTALL)
            if section_match:
                section_content = section_match.group(1).strip()
                if len(section_content) < 50:  # Arbitrary minimum for meaningful content
                    warnings.append(f"Section '{section}' appears to have minimal content")
        
        # Check for architectural viewpoint references
        architectural_terms = [
            "stakeholder", "concern", "viewpoint", "view", 
            "architectural", "quality attribute", "constraint"
        ]
        
        has_arch_terms = any(term in content.lower() for term in architectural_terms)
        if not has_arch_terms:
            suggestions.append("Consider adding architectural viewpoint and stakeholder concerns per ISO/IEC/IEEE 42010:2011")
        
        # Check for trade-off analysis
        trade_off_indicators = ["trade-off", "tradeoff", "pros and cons", "advantages", "disadvantages"]
        has_trade_off = any(indicator in content.lower() for indicator in trade_off_indicators)
        if not has_trade_off:
            suggestions.append("Consider adding explicit trade-off analysis in rationale section")
        
        # Check for implementation guidance
        if "## Implementation Guidelines" not in content:
            suggestions.append("Consider adding Implementation Guidelines section with concrete examples")
        
        return warnings, suggestions
    
    def _validate_traceability(self, content: str) -> List[str]:
        """Validate traceability links to requirements and other ADRs"""
        warnings = []
        
        # Check for requirement references
        req_pattern = r'REQ-[A-Z]+-[A-Z0-9]+-\d+'
        has_req_refs = re.search(req_pattern, content)
        if not has_req_refs:
            warnings.append("No requirement traceability found (REQ-*-*-* pattern)")
        
        # Check for related ADR references  
        adr_pattern = r'ADR-\d+'
        adr_refs = re.findall(adr_pattern, content)
        if len(adr_refs) <= 1:  # Only self-reference
            warnings.append("Consider referencing related ADRs for architectural coherence")
        
        return warnings
    
    def _generate_fix_suggestions(self, errors: List[str], warnings: List[str]) -> List[str]:
        """Generate actionable fix suggestions"""
        suggestions = []
        
        if any("Missing YAML front matter" in error for error in errors):
            suggestions.extend([
                "",
                "🔧 Fix YAML front matter:",
                "Add this header to your ADR:",
                "---",
                "specType: architecture", 
                "standard: ISO/IEC/IEEE 42010:2011",
                "phase: 03-architecture",
                "version: 1.0.0",
                "author: Your Name",
                "date: \"2024-01-01\"",
                "status: proposed",
                "---",
            ])
        
        if any("Missing required section" in error for error in errors):
            suggestions.extend([
                "",
                "📋 Add missing sections:",
                "Use this template structure:",
                "## Context",
                "Describe the architectural problem and forces",
                "",
                "## Decision", 
                "State the architectural decision clearly",
                "",
                "## Status",
                "Current status: proposed/accepted/deprecated",
                "",
                "## Rationale",
                "Explain why this decision addresses the forces",
                "",
                "## Considered Alternatives",
                "List and analyze rejected alternatives",
            ])
        
        if suggestions:
            suggestions.insert(0, "")
            suggestions.insert(1, "💡 Quick fixes:")
        
        return suggestions

def main():
    """Main entry point for ADR format validation"""
    import argparse
    
    parser = argparse.ArgumentParser(
        description='Validate Architecture Decision Record format per ISO/IEC/IEEE 42010:2011'
    )
    parser.add_argument('files', nargs='*', 
                       help='ADR files to validate')
    parser.add_argument('--all', action='store_true',
                       help='Validate all ADR files in project')
    parser.add_argument('--project-root', default='.',
                       help='Project root directory')
    
    args = parser.parse_args()
    
    validator = ADRFormatValidator()
    
    if args.all:
        # Find all ADR files
        adr_dir = pathlib.Path(args.project_root) / "03-architecture" / "decisions"
        if adr_dir.exists():
            adr_files = list(adr_dir.glob("*.md"))
        else:
            adr_files = []
    else:
        adr_files = [pathlib.Path(f) for f in args.files]
    
    if not adr_files:
        print("ℹ️ No ADR files found to validate")
        return 0
    
    print("🏗️ ADR Format Validation (ISO/IEC/IEEE 42010:2011)")
    print("=" * 60)
    
    all_valid = True
    
    for adr_file in adr_files:
        print(f"\n📄 Validating: {adr_file}")
        
        result = validator.validate_adr(adr_file)
        
        if result["valid"]:
            print("✅ Valid ADR format")
        else:
            print("❌ ADR format issues found")
            all_valid = False
        
        if result["errors"]:
            print("\n🚫 Errors:")
            for error in result["errors"]:
                print(f"  • {error}")
        
        if result["warnings"]:
            print("\n⚠️ Warnings:")
            for warning in result["warnings"]:
                print(f"  • {warning}")
        
        if result["suggestions"]:
            print("\n💡 Suggestions:")
            for suggestion in result["suggestions"]:
                print(f"  {suggestion}")
    
    if all_valid:
        print(f"\n✅ All ADRs follow ISO/IEC/IEEE 42010:2011 format!")
        return 0
    else:
        print(f"\n❌ Some ADRs need format corrections")
        print(f"📚 Reference: ISO/IEC/IEEE 42010:2011 Architecture description practices")
        return 1

if __name__ == '__main__':
    sys.exit(main())