#!/usr/bin/env python3
"""
YAML Front Matter Validator for Standards Specifications

Validates and fixes YAML front matter across all specification files
to ensure compliance with JSON schemas and traceability requirements.

Usage:
    python validate-yaml-frontmatter.py --validate-all    # Validate all specs
    python validate-yaml-frontmatter.py --fix-all         # Fix all specs  
    python validate-yaml-frontmatter.py --check FILE      # Check specific file
    python validate-yaml-frontmatter.py --fix FILE        # Fix specific file
"""

import os
import re
import json
import yaml
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional, Tuple
from datetime import datetime

class YAMLFrontMatterValidator:
    """Validates and fixes YAML front matter in specification files"""
    
    def __init__(self, repo_root: str):
        self.repo_root = Path(repo_root)
        self.schema_dir = self.repo_root / "spec-kit-templates" / "schemas"
        self.schemas = self._load_schemas()
        
    def _load_schemas(self) -> Dict[str, Dict]:
        """Load all JSON schemas"""
        schemas = {}
        
        schema_files = {
            'requirements': 'requirements-spec.schema.json',
            'architecture': 'architecture-spec.schema.json', 
            'design': 'ieee-design-spec.schema.json',
            'phase-gate': 'phase-gate-validation.schema.json'
        }
        
        for schema_type, schema_file in schema_files.items():
            schema_path = self.schema_dir / schema_file
            if schema_path.exists():
                try:
                    with open(schema_path, 'r') as f:
                        schemas[schema_type] = json.load(f)
                except json.JSONError as e:
                    print(f"⚠️  Error loading schema {schema_file}: {e}")
            else:
                print(f"⚠️  Schema file not found: {schema_path}")
        
        return schemas
    
    def validate_file(self, file_path: Path) -> Tuple[bool, List[str], Dict[str, Any]]:
        """Validate YAML front matter in a single file"""
        errors = []
        yaml_data = {}
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Check for YAML front matter
            if not content.startswith('---'):
                errors.append("Missing YAML front matter")
                return False, errors, yaml_data
            
            # Extract YAML content
            end_match = re.search(r'\n---\s*\n', content)
            if not end_match:
                errors.append("Malformed YAML front matter (missing closing ---)")
                return False, errors, yaml_data
            
            yaml_content = content[3:end_match.start()]
            
            # Parse YAML
            try:
                yaml_data = yaml.safe_load(yaml_content)
                if yaml_data is None:
                    yaml_data = {}
            except yaml.YAMLError as e:
                errors.append(f"Invalid YAML syntax: {e}")
                return False, errors, yaml_data
            
            # Determine spec type
            spec_type = yaml_data.get('specType', 'unknown')
            
            # Validate against appropriate schema
            if spec_type in self.schemas:
                schema_errors = self._validate_against_schema(yaml_data, self.schemas[spec_type])
                errors.extend(schema_errors)
            else:
                errors.append(f"Unknown spec type: {spec_type}")
            
            # Additional validation rules
            validation_errors = self._additional_validation(yaml_data, file_path)
            errors.extend(validation_errors)
            
        except Exception as e:
            errors.append(f"Error reading file: {e}")
            return False, errors, yaml_data
        
        return len(errors) == 0, errors, yaml_data
    
    def _validate_against_schema(self, data: Dict[str, Any], schema: Dict[str, Any]) -> List[str]:
        """Validate data against JSON schema (simplified validation)"""
        errors = []
        
        # Check required properties
        required = schema.get('properties', {})
        for prop, prop_schema in required.items():
            if prop not in data:
                if prop_schema.get('required', False):
                    errors.append(f"Missing required property: {prop}")
            else:
                # Basic type checking
                expected_type = prop_schema.get('type')
                if expected_type:
                    actual_value = data[prop]
                    
                    if expected_type == 'string' and not isinstance(actual_value, str):
                        errors.append(f"Property '{prop}' must be a string")
                    elif expected_type == 'array' and not isinstance(actual_value, list):
                        errors.append(f"Property '{prop}' must be an array")
                    elif expected_type == 'object' and not isinstance(actual_value, dict):
                        errors.append(f"Property '{prop}' must be an object")
        
        return errors
    
    def _additional_validation(self, data: Dict[str, Any], file_path: Path) -> List[str]:
        """Additional validation rules beyond schema"""
        errors = []
        
        # Validate requirement ID format
        if 'requirements' in data:
            requirements = data['requirements']
            if isinstance(requirements, list):
                for req_id in requirements:
                    if isinstance(req_id, str):
                        if not re.match(r'^REQ-(F|NF)-\d{3,4}$', req_id):
                            errors.append(f"Invalid requirement ID format: {req_id}")
            elif isinstance(requirements, dict) and 'requirements' in requirements:
                # Handle nested requirements structure
                nested_reqs = requirements['requirements']
                if isinstance(nested_reqs, list):
                    for req_id in nested_reqs:
                        if isinstance(req_id, str):
                            if not re.match(r'^REQ-(F|NF)-\d{3,4}$', req_id):
                                errors.append(f"Invalid requirement ID format: {req_id}")
        
        # Validate standard field for architecture specs
        if data.get('specType') == 'architecture':
            if data.get('standard') != '42010':
                errors.append("Architecture specs must use standard: '42010' (ISO/IEC/IEEE 42010)")
        
        # Validate date format
        if 'date' in data:
            date_str = data['date']
            try:
                datetime.strptime(date_str, '%Y-%m-%d')
            except ValueError:
                errors.append(f"Invalid date format: {date_str} (expected YYYY-MM-DD)")
        
        # Validate status values
        valid_statuses = ['draft', 'review', 'approved', 'published', 'deprecated']
        if 'status' in data:
            status = data['status']
            if status not in valid_statuses:
                errors.append(f"Invalid status: {status} (must be one of: {', '.join(valid_statuses)})")
        
        return errors
    
    def fix_file(self, file_path: Path) -> bool:
        """Fix YAML front matter in a file"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Extract YAML and content
            if not content.startswith('---'):
                print(f"❌ {file_path}: No YAML front matter to fix")
                return False
            
            end_match = re.search(r'\n---\s*\n', content)
            if not end_match:
                print(f"❌ {file_path}: Malformed YAML front matter")
                return False
            
            yaml_content = content[3:end_match.start()]
            rest_content = content[end_match.end():]
            
            # Parse and fix YAML
            try:
                yaml_data = yaml.safe_load(yaml_content) or {}
            except yaml.YAMLError:
                print(f"❌ {file_path}: Cannot parse YAML")
                return False
            
            # Apply fixes
            fixed_data = self._apply_fixes(yaml_data, file_path)
            
            # Write back
            new_yaml = yaml.dump(fixed_data, default_flow_style=False, sort_keys=False)
            new_content = f"---\n{new_yaml}---\n{rest_content}"
            
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(new_content)
            
            print(f"✅ {file_path}: Fixed YAML front matter")
            return True
            
        except Exception as e:
            print(f"❌ {file_path}: Error fixing YAML - {e}")
            return False
    
    def _apply_fixes(self, data: Dict[str, Any], file_path: Path) -> Dict[str, Any]:
        """Apply automatic fixes to YAML data"""
        fixed_data = data.copy()
        
        # Fix spec type for architecture files
        if 'architecture' in str(file_path):
            fixed_data['specType'] = 'architecture'
            fixed_data['standard'] = '42010'
        elif 'requirements' in str(file_path):
            fixed_data['specType'] = 'requirements'
            fixed_data['standard'] = '29148'
        elif 'design' in str(file_path):
            fixed_data['specType'] = 'design'
            fixed_data['standard'] = '1016'
        
        # Fix date format
        if 'date' not in fixed_data or not fixed_data['date']:
            fixed_data['date'] = datetime.now().strftime('%Y-%m-%d')
        
        # Fix status if missing
        if 'status' not in fixed_data:
            fixed_data['status'] = 'draft'
        
        # Fix version if missing
        if 'version' not in fixed_data:
            fixed_data['version'] = '1.0.0'
        
        # Fix requirements format if needed
        if 'requirements' in fixed_data:
            reqs = fixed_data['requirements']
            if isinstance(reqs, dict) and 'requirements' in reqs:
                # Flatten nested requirements
                fixed_data['requirements'] = reqs['requirements']
        
        return fixed_data
    
    def validate_all(self) -> Tuple[int, int]:
        """Validate all specification files"""
        valid_count = 0
        total_count = 0
        
        print("Validating all specification files...")
        
        # Find all markdown files in key directories
        spec_dirs = ['02-requirements', '03-architecture', '04-design']
        
        for spec_dir in spec_dirs:
            dir_path = self.repo_root / spec_dir
            if not dir_path.exists():
                continue
                
            for md_file in dir_path.rglob('*.md'):
                if md_file.name.startswith('.') or 'README' in md_file.name:
                    continue
                    
                total_count += 1
                is_valid, errors, yaml_data = self.validate_file(md_file)
                
                relative_path = md_file.relative_to(self.repo_root)
                
                if is_valid:
                    print(f"✅ {relative_path}")
                    valid_count += 1
                else:
                    print(f"❌ {relative_path}")
                    for error in errors[:3]:  # Show first 3 errors
                        print(f"   - {error}")
                    if len(errors) > 3:
                        print(f"   ... and {len(errors) - 3} more errors")
        
        print(f"\nValidation Summary: {valid_count}/{total_count} files valid")
        return valid_count, total_count
    
    def fix_all(self) -> Tuple[int, int]:
        """Fix all specification files"""
        fixed_count = 0
        total_count = 0
        
        print("Fixing all specification files...")
        
        # Find all markdown files in key directories  
        spec_dirs = ['02-requirements', '03-architecture', '04-design']
        
        for spec_dir in spec_dirs:
            dir_path = self.repo_root / spec_dir
            if not dir_path.exists():
                continue
                
            for md_file in dir_path.rglob('*.md'):
                if md_file.name.startswith('.') or 'README' in md_file.name:
                    continue
                    
                total_count += 1
                if self.fix_file(md_file):
                    fixed_count += 1
        
        print(f"\nFix Summary: {fixed_count}/{total_count} files fixed")
        return fixed_count, total_count

def main():
    parser = argparse.ArgumentParser(description='YAML Front Matter Validator')
    parser.add_argument('--validate-all', action='store_true', help='Validate all specification files')
    parser.add_argument('--fix-all', action='store_true', help='Fix all specification files')
    parser.add_argument('--check', metavar='FILE', help='Check specific file')
    parser.add_argument('--fix', metavar='FILE', help='Fix specific file')
    parser.add_argument('--repo-root', default='.', help='Repository root directory')
    
    args = parser.parse_args()
    
    validator = YAMLFrontMatterValidator(args.repo_root)
    
    if args.validate_all:
        valid_count, total_count = validator.validate_all()
        if valid_count < total_count:
            exit(1)
    elif args.fix_all:
        validator.fix_all()
    elif args.check:
        file_path = Path(args.repo_root) / args.check
        is_valid, errors, yaml_data = validator.validate_file(file_path)
        
        if is_valid:
            print(f"✅ {file_path} is valid")
        else:
            print(f"❌ {file_path} has errors:")
            for error in errors:
                print(f"  - {error}")
            exit(1)
    elif args.fix:
        file_path = Path(args.repo_root) / args.fix
        success = validator.fix_file(file_path)
        if not success:
            exit(1)
    else:
        print("No action specified. Use --help for usage information.")

if __name__ == '__main__':
    main()