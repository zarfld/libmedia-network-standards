#!/usr/bin/env python3
"""
Pre-Commit Traceability Validation Hook

Prevents commits that violate traceability requirements.
Enforces IEEE/ISO/IEC standards compliance for software lifecycle processes.

Usage:
    python pre-commit-traceability.py [files...]
    
Exit Codes:
    0 - All traceability checks passed
    1 - Traceability violations found, commit blocked
"""

import sys
import subprocess
from pathlib import Path
from typing import List, Set
import re
import yaml

class PreCommitTraceabilityValidator:
    """Pre-commit hook for traceability validation"""
    
    def __init__(self):
        self.repo_root = Path(__file__).parent.parent
        self.violations: List[str] = []
        
    def validate_modified_files(self, files: List[str]) -> bool:
        """Validate traceability for modified files"""
        print("🔍 Pre-commit traceability validation...")
        
        # Filter for specification files
        spec_files = [f for f in files if self._is_spec_file(f)]
        
        if not spec_files:
            print("ℹ️  No specification files modified, skipping traceability check")
            return True
            
        print(f"📄 Checking {len(spec_files)} specification files...")
        
        # Run comprehensive validation
        success = True
        
        for file_path in spec_files:
            if not self._validate_file_traceability(file_path):
                success = False
                
        # Check for newly introduced orphaned requirements
        if not self._check_orphaned_requirements(spec_files):
            success = False
            
        # Report results
        if success:
            print("✅ All traceability checks passed")
        else:
            print("❌ Traceability violations detected:")
            for violation in self.violations:
                print(f"  - {violation}")
            print("\nℹ️  Fix violations or use --no-verify to bypass (not recommended)")
            
        return success
        
    def _is_spec_file(self, file_path: str) -> bool:
        """Check if file is a specification file requiring traceability"""
        spec_patterns = [
            r'01-stakeholder.*\.md$',
            r'02-requirements.*\.md$',
            r'03-architecture.*\.md$',
            r'04-design.*\.md$',
            r'05-implementation.*\.md$',
            r'07-verification.*\.md$',
        ]
        
        for pattern in spec_patterns:
            if re.search(pattern, file_path):
                return True
        return False
        
    def _validate_file_traceability(self, file_path: str) -> bool:
        """Validate traceability for a single file"""
        abs_path = self.repo_root / file_path
        
        if not abs_path.exists():
            self.violations.append(f"File not found: {file_path}")
            return False
            
        try:
            with open(abs_path, 'r', encoding='utf-8') as f:
                content = f.read()
                lines = content.split('\n')
                
            # Validate YAML front matter traceability
            yaml_valid = self._validate_yaml_traceability(lines, file_path)
            
            # Validate inline requirement references
            inline_valid = self._validate_inline_references(lines, file_path)
            
            # Check for required traceability sections
            sections_valid = self._validate_traceability_sections(content, file_path)
            
            return yaml_valid and inline_valid and sections_valid
            
        except Exception as e:
            self.violations.append(f"Error processing {file_path}: {e}")
            return False
            
    def _validate_yaml_traceability(self, lines: List[str], file_path: str) -> bool:
        """Validate YAML front matter traceability"""
        if not lines or lines[0].strip() != '---':
            self.violations.append(f"{file_path}: Missing YAML front matter")
            return False
            
        # Find YAML section
        yaml_end = -1
        for i, line in enumerate(lines[1:], 1):
            if line.strip() == '---':
                yaml_end = i
                break
                
        if yaml_end == -1:
            self.violations.append(f"{file_path}: Malformed YAML front matter")
            return False
            
        try:
            yaml_content = '\n'.join(lines[1:yaml_end])
            yaml_data = yaml.safe_load(yaml_content)
            
            # Check for required traceability section
            if 'traceability' not in yaml_data:
                self.violations.append(f"{file_path}: Missing traceability section in YAML")
                return False
                
            traceability = yaml_data['traceability']
            
            # Validate traceability structure based on file type
            return self._validate_traceability_structure(traceability, file_path)
            
        except yaml.YAMLError as e:
            self.violations.append(f"{file_path}: YAML parsing error - {e}")
            return False
            
    def _validate_traceability_structure(self, traceability: dict, file_path: str) -> bool:
        """Validate traceability structure based on lifecycle phase"""
        valid = True
        
        # Determine expected fields based on file location
        if '02-requirements' in file_path:
            # Requirements should trace to stakeholder requirements
            if 'stakeholderRequirements' not in traceability:
                self.violations.append(f"{file_path}: Missing stakeholderRequirements traceability")
                valid = False
            else:
                stakeholder_reqs = traceability['stakeholderRequirements']
                if not isinstance(stakeholder_reqs, list) or not stakeholder_reqs:
                    self.violations.append(f"{file_path}: stakeholderRequirements must be non-empty list")
                    valid = False
                    
        elif '03-architecture' in file_path:
            # Architecture should trace to requirements
            if 'requirements' not in traceability:
                self.violations.append(f"{file_path}: Missing requirements traceability")
                valid = False
            else:
                requirements = traceability['requirements']
                if not isinstance(requirements, list) or not requirements:
                    self.violations.append(f"{file_path}: requirements must be non-empty list")
                    valid = False
                    
        elif '04-design' in file_path:
            # Design should trace to architecture
            expected_fields = ['architectureDecisions', 'requirements']
            for field in expected_fields:
                if field not in traceability:
                    self.violations.append(f"{file_path}: Missing {field} traceability")
                    valid = False
                    
        elif '07-verification' in file_path:
            # Tests should trace to requirements and design
            expected_fields = ['requirements', 'designElements']
            for field in expected_fields:
                if field not in traceability:
                    self.violations.append(f"{file_path}: Missing {field} traceability")
                    valid = False
                    
        return valid
        
    def _validate_inline_references(self, lines: List[str], file_path: str) -> bool:
        """Validate inline requirement references"""
        valid = True
        
        # Find requirement ID references
        req_pattern = r'\b(REQ-[FN]F?-\d{3,4})\b'
        
        for line_num, line in enumerate(lines, 1):
            matches = re.findall(req_pattern, line)
            for req_id in matches:
                # Validate requirement ID format
                if not self._is_valid_req_id_format(req_id):
                    self.violations.append(f"{file_path}:{line_num}: Invalid requirement ID format: {req_id}")
                    valid = False
                    
                # Check if requirement exists (simplified check)
                if not self._requirement_exists(req_id):
                    self.violations.append(f"{file_path}:{line_num}: Undefined requirement reference: {req_id}")
                    valid = False
                    
        return valid
        
    def _is_valid_req_id_format(self, req_id: str) -> bool:
        """Check if requirement ID follows correct format"""
        valid_patterns = [
            r'^REQ-F-\d{3,4}$',    # Functional: REQ-F-001, REQ-F-1234
            r'^REQ-NF-\d{3,4}$',   # Non-functional: REQ-NF-001, REQ-NF-1234
        ]
        
        for pattern in valid_patterns:
            if re.match(pattern, req_id):
                return True
        return False
        
    def _requirement_exists(self, req_id: str) -> bool:
        """Check if requirement is defined somewhere in the repository"""
        # Simplified existence check - in production this would use the 
        # traceability database or scan specification files
        
        # Check common requirement files
        req_files = [
            '02-requirements/functional/',
            '02-requirements/non-functional/',
            '01-stakeholder-requirements/',
        ]
        
        for req_dir in req_files:
            req_path = self.repo_root / req_dir
            if req_path.exists():
                for md_file in req_path.glob('**/*.md'):
                    try:
                        with open(md_file, 'r', encoding='utf-8') as f:
                            content = f.read()
                            if req_id in content:
                                return True
                    except:
                        continue
                        
        return False
        
    def _validate_traceability_sections(self, content: str, file_path: str) -> bool:
        """Validate presence of required traceability sections"""
        valid = True
        
        # Check for traceability section in content
        if '## Traceability' not in content and '# Traceability' not in content:
            if '03-architecture' in file_path or '04-design' in file_path:
                self.violations.append(f"{file_path}: Missing Traceability section")
                valid = False
                
        return valid
        
    def _check_orphaned_requirements(self, modified_files: List[str]) -> bool:
        """Check for newly introduced orphaned requirements"""
        print("🔍 Checking for orphaned requirements...")
        
        try:
            # Run quick orphan check using the main traceability enforcer
            result = subprocess.run([
                sys.executable, 
                str(self.repo_root / 'Scripts' / 'enforce-traceability.py'),
                '--validate-all',
                '--repo-root', str(self.repo_root)
            ], capture_output=True, text=True)
            
            if result.returncode != 0:
                # Parse output for orphan count
                if 'orphaned requirements' in result.stdout:
                    self.violations.append("New orphaned requirements detected")
                    return False
                    
        except Exception as e:
            print(f"⚠️  Could not run orphan check: {e}")
            # Don't fail commit for infrastructure issues
            
        return True

def main():
    """Main pre-commit validation function"""
    validator = PreCommitTraceabilityValidator()
    
    # Get list of files from git or command line
    if len(sys.argv) > 1:
        # Files passed as arguments
        files = sys.argv[1:]
    else:
        # Get staged files from git
        try:
            result = subprocess.run(['git', 'diff', '--cached', '--name-only'], 
                                  capture_output=True, text=True, check=True)
            files = result.stdout.strip().split('\n') if result.stdout.strip() else []
        except subprocess.CalledProcessError:
            print("⚠️  Could not get staged files from git")
            files = []
            
    if not files:
        print("ℹ️  No files to validate")
        return 0
        
    # Run validation
    success = validator.validate_modified_files(files)
    
    return 0 if success else 1

if __name__ == '__main__':
    exit(main())