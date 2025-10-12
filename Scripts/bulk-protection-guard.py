#!/usr/bin/env python3
"""
BULK MODIFICATION PREVENTION GUARD
==================================

This script prevents accidental bulk modifications by:
1. Detecting when >5 files would be modified
2. Requiring explicit confirmation for bulk operations
3. Providing detailed impact analysis before any changes
4. Creating safeguards against "fix everything" mistakes

Usage:
    python Scripts/bulk-protection-guard.py <command_to_protect>
    
Example:
    python Scripts/bulk-protection-guard.py python Scripts/autofix-spec-compliance.py --all
"""

import os
import sys
import subprocess
from pathlib import Path
from typing import List, Tuple
import argparse

class BulkModificationGuard:
    """Prevents accidental bulk modifications."""
    
    def __init__(self, repo_root: Path):
        self.repo_root = repo_root
        self.MAX_AUTO_MODIFY = 3  # Never auto-modify more than 3 files
        
    def analyze_potential_changes(self, command: List[str]) -> Tuple[List[Path], bool]:
        """
        Analyze what files would be changed by a command.
        
        Returns:
            (list_of_files_to_modify, is_safe_to_run)
        """
        # Get current git status
        result = subprocess.run(['git', 'status', '--porcelain'], 
                               capture_output=True, text=True, cwd=self.repo_root)
        
        if result.returncode != 0:
            return [], False
        
        initial_modified = set(line[3:] for line in result.stdout.strip().split('\n') if line.strip())
        
        # Run command in dry-run mode (add --check-only if supported)
        test_command = command.copy()
        if any('autofix' in arg for arg in command):
            test_command.append('--check-only')
        
        # Execute test command
        result = subprocess.run(test_command, capture_output=True, text=True, cwd=self.repo_root)
        
        # Check git status after
        result = subprocess.run(['git', 'status', '--porcelain'], 
                               capture_output=True, text=True, cwd=self.repo_root)
        
        if result.returncode != 0:
            return [], False
        
        final_modified = set(line[3:] for line in result.stdout.strip().split('\n') if line.strip())
        
        # Calculate files that would be modified
        newly_modified = final_modified - initial_modified
        modified_files = [Path(f) for f in newly_modified]
        
        is_safe = len(modified_files) <= self.MAX_AUTO_MODIFY
        
        return modified_files, is_safe
    
    def get_failing_files_only(self) -> List[Path]:
        """Get list of files that are actually failing validation."""
        failing_files = []
        
        # Run targeted validator to find actual failures
        result = subprocess.run([
            sys.executable, 'Scripts/targeted-spec-validator.py', '--ci-failures-only'
        ], capture_output=True, text=True, cwd=self.repo_root)
        
        if result.returncode != 0 and result.stdout:
            # Parse output to find failing files
            lines = result.stdout.split('\n')
            for line in lines:
                if '📁 Checking:' in line:
                    file_path = line.split('📁 Checking:')[1].strip()
                    failing_files.append(Path(file_path))
        
        return failing_files
    
    def interactive_confirmation(self, files_to_modify: List[Path]) -> bool:
        """Get user confirmation for bulk modifications."""
        print(f"\n🚨 BULK MODIFICATION WARNING!")
        print(f"   Command would modify {len(files_to_modify)} files:")
        print(f"   Maximum safe limit: {self.MAX_AUTO_MODIFY} files")
        
        print(f"\n📋 FILES TO BE MODIFIED:")
        for i, file_path in enumerate(files_to_modify[:10], 1):
            print(f"   {i}. {file_path}")
        
        if len(files_to_modify) > 10:
            print(f"   ... and {len(files_to_modify) - 10} more files")
        
        print(f"\n🎯 RECOMMENDED APPROACH:")
        failing_files = self.get_failing_files_only()
        if failing_files:
            print(f"   Only {len(failing_files)} files are actually failing:")
            for file_path in failing_files:
                print(f"   - {file_path}")
            print(f"\n   💡 Consider fixing only the failing files:")
            print(f"      python Scripts/targeted-spec-validator.py --ci-failures-only --fix")
        else:
            print(f"   ✅ No files are currently failing validation!")
            print(f"   ❓ Why are you running bulk modifications?")
        
        print(f"\n⚠️  BULK MODIFICATIONS CAN:")
        print(f"   - Create unnecessary git diff noise")
        print(f"   - Modify working files that don't need changes")  
        print(f"   - Make code review difficult")
        print(f"   - Introduce unintended side effects")
        
        response = input(f"\n🤔 Do you REALLY want to modify {len(files_to_modify)} files? [yes/NO]: ")
        
        return response.lower() in ['yes', 'y']
    
    def guard_command(self, command: List[str]) -> int:
        """
        Guard a command against bulk modifications.
        
        Returns:
            Exit code (0 = success, 1 = blocked/failed)
        """
        print(f"🛡️  BULK MODIFICATION GUARD")
        print(f"   Command: {' '.join(command)}")
        
        # Check if this is a potentially dangerous bulk operation
        dangerous_patterns = ['--all', '*', '**/*', 'autofix-spec-compliance.py']
        
        is_bulk_operation = any(pattern in ' '.join(command) for pattern in dangerous_patterns)
        
        if not is_bulk_operation:
            print(f"   ✅ Safe command - executing normally")
            return subprocess.run(command, cwd=self.repo_root).returncode
        
        print(f"   🚨 Detected potential bulk operation!")
        
        # Analyze impact
        files_to_modify, is_safe = self.analyze_potential_changes(command)
        
        if is_safe:
            print(f"   ✅ Safe bulk operation ({len(files_to_modify)} files)")
            return subprocess.run(command, cwd=self.repo_root).returncode
        
        # Require confirmation for dangerous bulk operations
        if not self.interactive_confirmation(files_to_modify):
            print(f"\n❌ BULK OPERATION CANCELLED BY USER")
            print(f"💡 Use targeted validation instead:")
            print(f"   python Scripts/targeted-spec-validator.py --ci-failures-only --fix")
            return 1
        
        print(f"\n⚠️  USER CONFIRMED - PROCEEDING WITH BULK OPERATION")
        return subprocess.run(command, cwd=self.repo_root).returncode

def main(argv: List[str]) -> int:
    """Main entry point."""
    if len(argv) < 2:
        print("❌ Usage: python Scripts/bulk-protection-guard.py <command>")
        return 1
    
    # Find repository root
    repo_root = Path.cwd()
    while not (repo_root / '.git').exists() and repo_root.parent != repo_root:
        repo_root = repo_root.parent
    
    if not (repo_root / '.git').exists():
        print("❌ Not in a git repository")
        return 1
    
    guard = BulkModificationGuard(repo_root)
    command = argv[1:]
    
    return guard.guard_command(command)

if __name__ == '__main__':
    sys.exit(main(sys.argv))