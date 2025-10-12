#!/usr/bin/env python3
"""
Requirement ID Registry and Management System

Automatically assigns and validates unique requirement IDs across all specifications.
Prevents ID conflicts and maintains traceability standards compliance.

Usage:
    python requirements-id-registry.py --scan          # Scan existing IDs
    python requirements-id-registry.py --next-f 10     # Get next 10 functional IDs
    python requirements-id-registry.py --next-nf 5     # Get next 5 non-functional IDs  
    python requirements-id-registry.py --validate      # Validate all specs
    python requirements-id-registry.py --assign SPEC   # Auto-assign IDs to spec
"""

import os
import re
import json
import yaml
import argparse
from pathlib import Path
from typing import Dict, List, Set, Tuple, Optional
from dataclasses import dataclass, asdict
from datetime import datetime

@dataclass
class RequirementID:
    """Represents a requirement ID with metadata"""
    id: str
    type: str  # 'F' or 'NF'
    number: int
    file_path: str
    line_number: int
    standard: Optional[str] = None
    description: Optional[str] = None

@dataclass
class IDRegistry:
    """Registry tracking all requirement IDs"""
    functional_ids: Dict[int, RequirementID]
    non_functional_ids: Dict[int, RequirementID]
    next_functional: int
    next_non_functional: int
    last_updated: str
    conflicts: List[str]

class RequirementIDManager:
    """Main class for managing requirement IDs across all specifications"""
    
    def __init__(self, repo_root: str):
        self.repo_root = Path(repo_root)
        self.registry_file = self.repo_root / "Scripts" / "requirement-id-registry.json"
        self.registry: IDRegistry = self._load_or_create_registry()
        
    def _load_or_create_registry(self) -> IDRegistry:
        """Load existing registry or create new one"""
        if self.registry_file.exists():
            with open(self.registry_file, 'r') as f:
                data = json.load(f)
                return IDRegistry(
                    functional_ids={int(k): RequirementID(**v) for k, v in data['functional_ids'].items()},
                    non_functional_ids={int(k): RequirementID(**v) for k, v in data['non_functional_ids'].items()},
                    next_functional=data['next_functional'],
                    next_non_functional=data['next_non_functional'],
                    last_updated=data['last_updated'],
                    conflicts=data['conflicts']
                )
        else:
            return IDRegistry(
                functional_ids={},
                non_functional_ids={},
                next_functional=1,
                next_non_functional=1,
                conflicts=[],
                last_updated=datetime.now().isoformat()
            )
    
    def save_registry(self):
        """Save registry to file"""
        self.registry.last_updated = datetime.now().isoformat()
        
        # Convert to serializable format
        data = {
            'functional_ids': {str(k): asdict(v) for k, v in self.registry.functional_ids.items()},
            'non_functional_ids': {str(k): asdict(v) for k, v in self.registry.non_functional_ids.items()},
            'next_functional': self.registry.next_functional,
            'next_non_functional': self.registry.next_non_functional,
            'last_updated': self.registry.last_updated,
            'conflicts': self.registry.conflicts
        }
        
        os.makedirs(self.registry_file.parent, exist_ok=True)
        with open(self.registry_file, 'w') as f:
            json.dump(data, f, indent=2)
    
    def scan_existing_ids(self) -> Tuple[int, int]:
        """Scan all markdown files for existing requirement IDs"""
        print("Scanning existing requirement IDs...")
        
        # Reset registry
        self.registry.functional_ids = {}
        self.registry.non_functional_ids = {}
        self.registry.conflicts = []
        
        # Patterns to match requirement IDs
        f_pattern = re.compile(r'REQ-F-(\d+)')
        nf_pattern = re.compile(r'REQ-NF-(\d+)')
        
        # Scan all markdown files
        for md_file in self.repo_root.rglob('*.md'):
            if '.git' in str(md_file):
                continue
                
            try:
                with open(md_file, 'r', encoding='utf-8') as f:
                    lines = f.readlines()
                    
                for line_num, line in enumerate(lines, 1):
                    # Find functional requirements
                    for match in f_pattern.finditer(line):
                        num = int(match.group(1))
                        req_id = RequirementID(
                            id=f"REQ-F-{num:03d}",
                            type='F',
                            number=num,
                            file_path=str(md_file.relative_to(self.repo_root)),
                            line_number=line_num,
                            description=line.strip()
                        )
                        
                        if num in self.registry.functional_ids:
                            self.registry.conflicts.append(
                                f"Duplicate REQ-F-{num:03d} in {req_id.file_path}:{line_num} and "
                                f"{self.registry.functional_ids[num].file_path}:{self.registry.functional_ids[num].line_number}"
                            )
                        else:
                            self.registry.functional_ids[num] = req_id
                    
                    # Find non-functional requirements  
                    for match in nf_pattern.finditer(line):
                        num = int(match.group(1))
                        req_id = RequirementID(
                            id=f"REQ-NF-{num:03d}",
                            type='NF',
                            number=num,
                            file_path=str(md_file.relative_to(self.repo_root)),
                            line_number=line_num,
                            description=line.strip()
                        )
                        
                        if num in self.registry.non_functional_ids:
                            self.registry.conflicts.append(
                                f"Duplicate REQ-NF-{num:03d} in {req_id.file_path}:{line_num} and "
                                f"{self.registry.non_functional_ids[num].file_path}:{self.registry.non_functional_ids[num].line_number}"
                            )
                        else:
                            self.registry.non_functional_ids[num] = req_id
                            
            except (UnicodeDecodeError, PermissionError):
                continue
        
        # Calculate next available IDs
        if self.registry.functional_ids:
            self.registry.next_functional = max(self.registry.functional_ids.keys()) + 1
        else:
            self.registry.next_functional = 1
            
        if self.registry.non_functional_ids:
            self.registry.next_non_functional = max(self.registry.non_functional_ids.keys()) + 1
        else:
            self.registry.next_non_functional = 1
        
        self.save_registry()
        
        print(f"Found {len(self.registry.functional_ids)} functional requirements")
        print(f"Found {len(self.registry.non_functional_ids)} non-functional requirements")
        print(f"Next functional ID: REQ-F-{self.registry.next_functional:03d}")
        print(f"Next non-functional ID: REQ-NF-{self.registry.next_non_functional:03d}")
        
        if self.registry.conflicts:
            print(f"\n⚠️  Found {len(self.registry.conflicts)} conflicts:")
            for conflict in self.registry.conflicts:
                print(f"  - {conflict}")
        
        return len(self.registry.functional_ids), len(self.registry.non_functional_ids)
    
    def get_next_functional_ids(self, count: int) -> List[str]:
        """Get next available functional requirement IDs"""
        ids = []
        start = self.registry.next_functional
        
        for i in range(count):
            id_num = start + i
            ids.append(f"REQ-F-{id_num:03d}")
            
        # Reserve these IDs
        self.registry.next_functional = start + count
        self.save_registry()
        
        return ids
    
    def get_next_non_functional_ids(self, count: int) -> List[str]:
        """Get next available non-functional requirement IDs"""
        ids = []
        start = self.registry.next_non_functional
        
        for i in range(count):
            id_num = start + i
            ids.append(f"REQ-NF-{id_num:03d}")
            
        # Reserve these IDs
        self.registry.next_non_functional = start + count
        self.save_registry()
        
        return ids
    
    def validate_yaml_front_matter(self, file_path: str) -> List[str]:
        """Validate YAML front matter in a specification file"""
        errors = []
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Extract YAML front matter
            if not content.startswith('---'):
                errors.append("Missing YAML front matter")
                return errors
            
            # Find end of front matter
            end_match = re.search(r'\n---\s*\n', content)
            if not end_match:
                errors.append("Malformed YAML front matter (missing closing ---)")
                return errors
            
            yaml_content = content[3:end_match.start()]
            
            try:
                yaml_data = yaml.safe_load(yaml_content)
            except yaml.YAMLError as e:
                errors.append(f"Invalid YAML syntax: {e}")
                return errors
            
            # Validate required fields
            required_fields = ['specType', 'standard', 'requirements']
            for field in required_fields:
                if field not in yaml_data:
                    errors.append(f"Missing required field: {field}")
            
            # Validate requirement ID format
            if 'requirements' in yaml_data:
                for req_id in yaml_data['requirements']:
                    if not re.match(r'REQ-(F|NF)-\d{3}', req_id):
                        errors.append(f"Invalid requirement ID format: {req_id}")
            
        except Exception as e:
            errors.append(f"Error reading file: {e}")
        
        return errors
    
    def auto_assign_ids_to_spec(self, spec_file: str, functional_count: int, non_functional_count: int = 0):
        """Auto-assign requirement IDs to a specification file"""
        print(f"Auto-assigning IDs to {spec_file}...")
        
        # Get required IDs
        f_ids = self.get_next_functional_ids(functional_count) if functional_count > 0 else []
        nf_ids = self.get_next_non_functional_ids(non_functional_count) if non_functional_count > 0 else []
        
        # Read current file
        with open(spec_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Extract and update YAML front matter
        if content.startswith('---'):
            end_match = re.search(r'\n---\s*\n', content)
            if end_match:
                yaml_content = content[3:end_match.start()]
                rest_content = content[end_match.end():]
                
                try:
                    yaml_data = yaml.safe_load(yaml_content)
                    
                    # Update requirements list
                    all_req_ids = f_ids + nf_ids
                    yaml_data['requirements'] = all_req_ids
                    
                    # Update other metadata
                    yaml_data['version'] = yaml_data.get('version', '1.0.0')
                    yaml_data['date'] = datetime.now().strftime('%Y-%m-%d')
                    yaml_data['status'] = yaml_data.get('status', 'draft')
                    
                    # Write back
                    new_yaml = yaml.dump(yaml_data, default_flow_style=False, sort_keys=False)
                    new_content = f"---\n{new_yaml}---\n{rest_content}"
                    
                    with open(spec_file, 'w', encoding='utf-8') as f:
                        f.write(new_content)
                    
                    print(f"✅ Assigned {len(all_req_ids)} IDs to {spec_file}")
                    print(f"   Functional: {f_ids}")
                    if nf_ids:
                        print(f"   Non-functional: {nf_ids}")
                    
                except yaml.YAMLError as e:
                    print(f"❌ Error updating YAML: {e}")
            else:
                print(f"❌ Malformed YAML front matter in {spec_file}")
        else:
            print(f"❌ No YAML front matter found in {spec_file}")

def main():
    parser = argparse.ArgumentParser(description='Requirement ID Registry Management')
    parser.add_argument('--scan', action='store_true', help='Scan existing requirement IDs')
    parser.add_argument('--next-f', type=int, metavar='COUNT', help='Get next N functional IDs')
    parser.add_argument('--next-nf', type=int, metavar='COUNT', help='Get next N non-functional IDs')
    parser.add_argument('--validate', metavar='FILE', help='Validate YAML front matter in file')
    parser.add_argument('--assign', metavar='FILE', help='Auto-assign IDs to specification file')
    parser.add_argument('--f-count', type=int, default=50, help='Number of functional requirements to assign')
    parser.add_argument('--nf-count', type=int, default=0, help='Number of non-functional requirements to assign')
    parser.add_argument('--repo-root', default='.', help='Repository root directory')
    
    args = parser.parse_args()
    
    manager = RequirementIDManager(args.repo_root)
    
    if args.scan:
        manager.scan_existing_ids()
    elif args.next_f:
        ids = manager.get_next_functional_ids(args.next_f)
        print("Next functional IDs:")
        for id in ids:
            print(f"  - \"{id}\"")
    elif args.next_nf:
        ids = manager.get_next_non_functional_ids(args.next_nf)
        print("Next non-functional IDs:")
        for id in ids:
            print(f"  - \"{id}\"")
    elif args.validate:
        errors = manager.validate_yaml_front_matter(args.validate)
        if errors:
            print(f"❌ Validation errors in {args.validate}:")
            for error in errors:
                print(f"  - {error}")
        else:
            print(f"✅ {args.validate} is valid")
    elif args.assign:
        manager.auto_assign_ids_to_spec(args.assign, args.f_count, args.nf_count)
    else:
        # Show current status
        print("Requirement ID Registry Status:")
        print(f"Functional IDs: {len(manager.registry.functional_ids)} used, next: REQ-F-{manager.registry.next_functional:03d}")
        print(f"Non-functional IDs: {len(manager.registry.non_functional_ids)} used, next: REQ-NF-{manager.registry.next_non_functional:03d}")
        print(f"Conflicts: {len(manager.registry.conflicts)}")
        print(f"Last updated: {manager.registry.last_updated}")
        
        if manager.registry.conflicts:
            print("\n⚠️  Active conflicts:")
            for conflict in manager.registry.conflicts[:5]:  # Show first 5
                print(f"  - {conflict}")
            if len(manager.registry.conflicts) > 5:
                print(f"  ... and {len(manager.registry.conflicts) - 5} more")

if __name__ == '__main__':
    main()