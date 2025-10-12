#!/usr/bin/env python3
"""
Bulk Fix Validation Failures Script

Uses the specification-document-template-generator.py to fix all validation failures
by regenerating YAML front matter with correct schema compliance.

PRINCIPLE: First-attempt-correct using KNOWN data, not manual editing!
"""

import subprocess
import sys
from pathlib import Path
import json
import yaml
from typing import List, Dict, Any

def run_validation() -> Dict[str, List[str]]:
    """Run validation and capture which files are failing."""
    print("🔍 Running validation to identify failing files...")
    
    cmd = [sys.executable, "Scripts/validate-spec-structure.py"]
    # Do NOT use ALLOW_EMPTY_SPECS - we want to see real failures
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8', errors='replace')
        failing_files = {}
        
        if result.stdout:
            for line in result.stdout.split('\n'):
                if '[FAIL]' in line:
                    # Extract filename and error
                    parts = line.split(': ', 2)
                    if len(parts) >= 3:
                        file_path = parts[0].replace('[FAIL] ', '').strip()
                        error = parts[1] + ': ' + parts[2] if len(parts) > 2 else parts[1]
                        
                        if file_path not in failing_files:
                            failing_files[file_path] = []
                        failing_files[file_path].append(error)
        
        return failing_files
    except Exception as e:
        print(f"❌ Failed to run validation: {e}")
        return {}

def detect_spec_type(file_path: Path) -> str:
    """Detect specification type from file path and content."""
    path_str = str(file_path).lower()
    
    if 'adr' in path_str or 'decision' in path_str:
        return 'architecture'  # ADRs are architecture decisions per validation schema
    elif 'architecture' in path_str or '03-architecture' in path_str:
        return 'architecture' 
    elif 'requirements' in path_str or '02-requirements' in path_str:
        return 'requirements'
    elif 'design' in path_str or '04-design' in path_str:
        return 'design'
    else:
        return 'architecture'  # Default fallback

def extract_id_from_file(file_path: Path) -> str:
    """Extract the correct ID from filename (e.g., ADR-003 from ADR-003-ieee-1588.md)."""
    filename = file_path.name
    
    # ADR pattern: ADR-XXX
    if 'ADR-' in filename.upper():
        import re
        match = re.search(r'ADR-(\d{3})', filename.upper())
        if match:
            return f"ADR-{match.group(1)}"
    
    # Architecture pattern: ARCH-XXX  
    if 'ARCH-' in filename.upper():
        import re
        match = re.search(r'ARCH-([A-Z0-9-]+)', filename.upper())
        if match:
            return f"ARCH-{match.group(1)}"
    
    # Requirements pattern: REQ-F-XXX or REQ-NF-XXX
    if 'REQ-' in filename.upper():
        import re
        match = re.search(r'REQ-(F|NF)-(\d{3,4})', filename.upper())
        if match:
            return f"REQ-{match.group(1)}-{match.group(2)}"
    
    # Design pattern: DES-XXX
    if 'DES-' in filename.upper():
        import re
        match = re.search(r'DES-([A-Z0-9-]+)', filename.upper())
        if match:
            return f"DES-{match.group(1)}"
    
    # Default fallback - use filename without extension
    return file_path.stem.upper().replace('-', '_')

def extract_title_from_file(file_path: Path) -> str:
    """Extract title from existing file content."""
    try:
        content = file_path.read_text(encoding='utf-8')
        
        # Try to get title from YAML front matter
        if content.startswith('---'):
            parts = content.split('---', 2)
            if len(parts) >= 2:
                yaml_content = yaml.safe_load(parts[1])
                if isinstance(yaml_content, dict) and 'title' in yaml_content:
                    return yaml_content['title'].strip('"')
        
        # Try to get title from first H1 heading
        for line in content.split('\n'):
            if line.startswith('# '):
                return line[2:].strip()
        
        # Fallback to filename
        return file_path.stem.replace('-', ' ').replace('_', ' ').title()
        
    except Exception:
        return file_path.stem.replace('-', ' ').replace('_', ' ').title()

def fix_file_with_generator(file_path: Path, spec_type: str, title: str) -> bool:
    """Fix a file using the specification template generator."""
    print(f"🛠️  Fixing {file_path} as {spec_type}: '{title}'")
    
    try:
        # Generate correct YAML front matter and content using the generator
        # Pass file path so generator can extract correct ID
        cmd = [
            sys.executable,
            "Scripts/specification-document-template-generator.py",
            spec_type,
            title,
            str(file_path)  # Pass file path for ID extraction
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8', errors='replace')
        if result.returncode != 0:
            print(f"❌ Generator failed for {file_path}: {result.stderr}")
            return False
        
        generated_content = result.stdout
        if not generated_content:
            print(f"❌ Generator produced no output for {file_path}")
            return False
        
        # Extract just the YAML front matter from generated content
        if generated_content.startswith('---'):
            parts = generated_content.split('---', 2)
            if len(parts) >= 3:
                new_yaml_str = parts[1]
                
                # Parse the generated YAML to add the correct ID field
                try:
                    new_yaml_dict = yaml.safe_load(new_yaml_str)
                    
                    # CRITICAL: Add the correct ID field based on filename
                    correct_id = extract_id_from_file(file_path)
                    new_yaml_dict['id'] = correct_id
                    
                    print(f"   📋 Setting id: {correct_id}")
                    
                    # Convert back to YAML string
                    new_yaml_str = yaml.dump(new_yaml_dict, default_flow_style=False, sort_keys=False)
                    
                except Exception as e:
                    print(f"⚠️  Failed to parse/modify YAML for {file_path}: {e}")
                    # Continue with original YAML if parsing fails
                
                # Read current file content
                current_content = file_path.read_text(encoding='utf-8')
                
                # Replace YAML front matter
                if current_content.startswith('---'):
                    current_parts = current_content.split('---', 2)
                    if len(current_parts) >= 3:
                        # Keep original content, replace only YAML
                        fixed_content = f"---\n{new_yaml_str}---{current_parts[2]}"
                    else:
                        fixed_content = f"---\n{new_yaml_str}---\n\n{current_content}"
                else:
                    # Add YAML front matter to file without it
                    fixed_content = f"---\n{new_yaml_str}---\n\n{current_content}"
                
                # Write fixed content back
                file_path.write_text(fixed_content, encoding='utf-8')
                print(f"✅ Fixed {file_path}")
                return True
        
        print(f"❌ Could not extract YAML from generator output for {file_path}")
        return False
        
    except Exception as e:
        print(f"❌ Failed to fix {file_path}: {e}")
        return False

def main():
    """Main function to bulk fix all validation failures."""
    print("🚀 BULK FIX VALIDATION FAILURES - First-Attempt-Correct Approach")
    print("=" * 60)
    
    # Change to repository root
    repo_root = Path.cwd()
    while not (repo_root / '.git').exists() and repo_root.parent != repo_root:
        repo_root = repo_root.parent
    
    if not (repo_root / '.git').exists():
        print("❌ Not in a git repository")
        return 1
    
    # Get failing files
    failing_files = run_validation()
    
    if not failing_files:
        print("✅ No validation failures found!")
        return 0
    
    print(f"📋 Found {len(failing_files)} failing files with {sum(len(errors) for errors in failing_files.values())} total issues")
    
    # Fix each failing file
    fixed_count = 0
    for file_path_str, errors in failing_files.items():
        file_path = Path(file_path_str)
        
        if not file_path.exists():
            print(f"⚠️  File not found: {file_path}")
            continue
        
        # Detect spec type and extract title and ID
        spec_type = detect_spec_type(file_path)
        title = extract_title_from_file(file_path)
        expected_id = extract_id_from_file(file_path)
        
        print(f"📋 Processing: {file_path}")
        print(f"   Type: {spec_type}")
        print(f"   Title: {title}")  
        print(f"   Expected ID: {expected_id}")
        print(f"   Errors: {errors[:2]}...")  # Show first 2 errors
        
        # Fix the file using the generator
        if fix_file_with_generator(file_path, spec_type, title):
            fixed_count += 1
        else:
            print(f"❌ Failed to fix {file_path}")
            print(f"   Errors were: {errors[:3]}...")  # Show first 3 errors
    
    print()
    print(f"📊 BULK FIX SUMMARY:")
    print(f"   ✅ Fixed: {fixed_count} files")
    print(f"   ❌ Failed: {len(failing_files) - fixed_count} files")
    
    # Run validation again to confirm fixes
    print("\n🔍 Re-running validation to confirm fixes...")
    remaining_failures = run_validation()
    
    if not remaining_failures:
        print("🎉 ALL VALIDATION FAILURES FIXED!")
        return 0
    else:
        print(f"⚠️  {len(remaining_failures)} files still failing:")
        for file_path, errors in remaining_failures.items():
            print(f"   - {file_path}: {errors[0]}")
        return 1

if __name__ == '__main__':
    sys.exit(main())