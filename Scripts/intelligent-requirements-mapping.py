#!/usr/bin/env python3
"""
Intelligent Requirement Mapping for Architecture Files
Maps architecture files to appropriate requirements based on content analysis.
"""

import os
import re
import yaml
from pathlib import Path
import json

def extract_yaml_frontmatter(file_path):
    """Extract YAML frontmatter from markdown file."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        if not content.startswith('---'):
            return None, content
            
        # Find end of frontmatter
        end_match = re.search(r'\n---\n', content)
        if not end_match:
            return None, content
            
        yaml_content = content[4:end_match.start()]
        body_content = content[end_match.end():]
        
        return yaml.safe_load(yaml_content), body_content
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
        return None, ""

def analyze_content_for_requirements(file_path, body_content):
    """Analyze file content to determine appropriate requirements."""
    content_lower = body_content.lower()
    filename_lower = os.path.basename(file_path).lower()
    
    # IEEE 1588 PTP related
    if any(term in content_lower or term in filename_lower for term in 
           ['1588', 'ptp', 'precision time', 'clock sync']):
        return ['REQ-F-001', 'REQ-NF-009']  # Multi-standard integration + sync
    
    # IEEE 1722.1 AVDECC related  
    if any(term in content_lower or term in filename_lower for term in
           ['1722.1', 'avdecc', 'entity control', 'device control']):
        return ['REQ-F-002', 'REQ-NF-008']  # Device control + recovery
    
    # IEEE 1722 AVTP related
    if any(term in content_lower or term in filename_lower for term in
           ['1722', 'avtp', 'audio video transport']):
        return ['REQ-NF-001', 'REQ-NF-004']  # Latency + audio channels
        
    # IEEE 802.1AS gPTP related
    if any(term in content_lower or term in filename_lower for term in
           ['802.1as', 'gptp', 'generalized precision']):
        return ['REQ-NF-009', 'REQ-NF-012']  # Sync + PTP domains
    
    # Security related
    if any(term in content_lower or term in filename_lower for term in
           ['security', 'authentication', 'encryption']):
        return ['REQ-NF-015', 'REQ-NF-008']  # Security + recovery
        
    # Performance/Management related
    if any(term in content_lower or term in filename_lower for term in
           ['management', 'performance', 'architecture']):
        return ['REQ-F-001', 'REQ-NF-010']  # Integration + scalability
    
    # Default fallback
    return ['REQ-F-001']

def fix_architecture_traceability():
    """Fix all architecture files with intelligent requirement mapping."""
    
    repo_root = Path(__file__).parent.parent
    architecture_dir = repo_root / "03-architecture"
    
    if not architecture_dir.exists():
        print(f"Architecture directory not found: {architecture_dir}")
        return False
    
    fixed_count = 0
    failed_count = 0
    
    # Find all architecture markdown files
    for md_file in architecture_dir.rglob("*.md"):
        try:
            frontmatter, body = extract_yaml_frontmatter(md_file)
            
            if not frontmatter:
                continue
                
            # Only process architecture specs
            if frontmatter.get('specType') != 'architecture':
                continue
            
            # Check if traceability needs fixing
            traceability = frontmatter.get('traceability', {})
            requirements = traceability.get('requirements', [])
            
            if not requirements or requirements == []:
                print(f"🛠️  Fixing traceability for {md_file.relative_to(repo_root)}")
                
                # Analyze content to determine appropriate requirements
                mapped_requirements = analyze_content_for_requirements(str(md_file), body)
                
                # Update frontmatter
                frontmatter['traceability'] = {
                    'requirements': mapped_requirements
                }
                
                # Write back to file
                with open(md_file, 'r', encoding='utf-8') as f:
                    content = f.read()
                
                # Replace YAML frontmatter
                yaml_str = yaml.dump(frontmatter, default_flow_style=False, sort_keys=False)
                new_content = f"---\n{yaml_str}---\n" + body
                
                with open(md_file, 'w', encoding='utf-8') as f:
                    f.write(new_content)
                
                print(f"   ✅ Added requirements: {', '.join(mapped_requirements)}")
                fixed_count += 1
            
        except Exception as e:
            print(f"   ❌ Error fixing {md_file}: {e}")
            failed_count += 1
    
    print(f"\n📊 INTELLIGENT TRACEABILITY MAPPING COMPLETE:")
    print(f"   ✅ Fixed: {fixed_count} files")
    print(f"   ❌ Failed: {failed_count} files")
    
    return failed_count == 0

if __name__ == "__main__":
    success = fix_architecture_traceability()
    exit(0 if success else 1)