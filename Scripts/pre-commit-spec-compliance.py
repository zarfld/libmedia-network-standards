#!/usr/bin/env python3
"""
Pre-commit Hook for Automated Specification Compliance
====================================================

Automatically validates and fixes specification files before commit.
Prevents non-compliant YAML front matter from being committed.

Installation:
    cp Scripts/pre-commit-spec-compliance.py .git/hooks/pre-commit
    chmod +x .git/hooks/pre-commit

Or use with pre-commit framework:
    pip install pre-commit
    pre-commit install
"""

import sys
import subprocess
from pathlib import Path

def main():
    """Pre-commit hook main function."""
    
    # Get list of staged files
    result = subprocess.run(['git', 'diff', '--cached', '--name-only'], 
                           capture_output=True, text=True)
    
    if result.returncode != 0:
        print("❌ Failed to get staged files")
        return 1
    
    staged_files = result.stdout.strip().split('\n')
    if not staged_files or staged_files == ['']:
        return 0  # No files staged
    
    # Filter specification files
    spec_files = []
    for file_path in staged_files:
        path = Path(file_path)
        if (path.suffix == '.md' and 
            any(pattern in str(path).lower() for pattern in [
                '02-requirements', '03-architecture', '04-design', 
                'spec.md', 'specification.md', 'architecture.md'
            ]) and
            not any(pattern in str(path).lower() for pattern in [
                'readme.md', 'template', '.github', 'examples'
            ])):
            spec_files.append(file_path)
    
    if not spec_files:
        return 0  # No specification files staged
    
    print(f"🔍 Validating {len(spec_files)} specification files...")
    
    # Run compliance validation and auto-fix
    cmd = [sys.executable, 'Scripts/autofix-spec-compliance.py'] + spec_files
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode != 0:
        print("❌ SPECIFICATION COMPLIANCE VALIDATION FAILED!")
        print(result.stdout)
        print(result.stderr)
        print("\n💡 Run this command to auto-fix issues:")
        print(f"   python Scripts/autofix-spec-compliance.py {' '.join(spec_files)}")
        return 1
    
    # Check if any files were modified by auto-fix
    result = subprocess.run(['git', 'diff', '--name-only'] + spec_files,
                           capture_output=True, text=True)
    
    if result.stdout.strip():
        modified_files = result.stdout.strip().split('\n')
        print(f"✅ Auto-fixed {len(modified_files)} specification files")
        print("📝 Files modified:")
        for file_path in modified_files:
            print(f"   - {file_path}")
        
        # Add modified files back to staging
        subprocess.run(['git', 'add'] + modified_files)
        print("🚀 Auto-fixes staged for commit")
    
    print("✅ All specification files are compliant!")
    return 0

if __name__ == '__main__':
    sys.exit(main())