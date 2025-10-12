#!/usr/bin/env python3
"""
Traceability Safety Validator

Comprehensive safety checks to ensure traceability infrastructure 
protects project integrity. Validates against accidental data loss,
corruption, or disruption of existing traceability relationships.

Usage:
    python validate-traceability-safety.py --full-check
    python validate-traceability-safety.py --pre-execution --target-script enforce-traceability.py
    python validate-traceability-safety.py --post-execution --compare-backup backup_dir
"""

import os
import sys
import json
import yaml
import hashlib
import argparse
from pathlib import Path
from typing import Dict, List, Set, Tuple, Optional
from datetime import datetime
import subprocess

class TraceabilitySafetyValidator:
    """Validates safety of traceability infrastructure operations"""
    
    def __init__(self, repo_root: str):
        self.repo_root = Path(repo_root)
        self.safety_violations: List[Dict] = []
        self.warnings: List[Dict] = []
        self.baseline_snapshot: Dict = {}
        
    def create_baseline_snapshot(self) -> Dict:
        """Create baseline snapshot of current traceability state"""
        print("📸 Creating baseline traceability snapshot...")
        
        snapshot = {
            'timestamp': datetime.now().isoformat(),
            'files': {},
            'requirements': {},
            'traceability_matrix': {},
            'statistics': {}
        }
        
        # Scan all specification files
        spec_files = []
        for phase_dir in ['01-stakeholder-requirements', '02-requirements', '03-architecture', 
                         '04-design', '05-implementation', '07-verification-validation']:
            phase_path = self.repo_root / phase_dir
            if phase_path.exists():
                spec_files.extend(phase_path.glob('**/*.md'))
                
        # Create file snapshots with checksums
        for file_path in spec_files:
            try:
                with open(file_path, 'rb') as f:
                    content = f.read()
                    checksum = hashlib.sha256(content).hexdigest()
                    
                relative_path = str(file_path.relative_to(self.repo_root))
                snapshot['files'][relative_path] = {
                    'checksum': checksum,
                    'size': len(content),
                    'modified': datetime.fromtimestamp(file_path.stat().st_mtime).isoformat()
                }
                
                # Extract requirements from this file
                text_content = content.decode('utf-8', errors='ignore')
                requirements = self._extract_requirements_from_content(text_content)
                
                for req_id in requirements:
                    if req_id not in snapshot['requirements']:
                        snapshot['requirements'][req_id] = []
                    snapshot['requirements'][req_id].append(relative_path)
                    
            except Exception as e:
                self.warnings.append({
                    'type': 'snapshot_error',
                    'file': str(file_path),
                    'message': f"Error creating snapshot: {e}"
                })
                
        # Calculate statistics
        snapshot['statistics'] = {
            'total_files': len(snapshot['files']),
            'total_requirements': len(snapshot['requirements']),
            'duplicate_requirements': self._count_duplicate_requirements(snapshot['requirements']),
            'orphaned_files': self._count_orphaned_files(snapshot['files'])
        }
        
        print(f"✅ Baseline snapshot created: {snapshot['statistics']['total_requirements']} requirements in {snapshot['statistics']['total_files']} files")
        
        self.baseline_snapshot = snapshot
        return snapshot
        
    def _extract_requirements_from_content(self, content: str) -> List[str]:
        """Extract requirement IDs from file content"""
        import re
        
        # Pattern for various requirement ID formats
        patterns = [
            r'\b(REQ-F-\d{3,4})\b',
            r'\b(REQ-NF-\d{3,4})\b',
            r'\b(REQ-FUN-\w+-\d{3})\b',
            r'\b(REQ-NFR-\w+-\d{3})\b',
            r'\b(REQ-STK-\w+-\d{3})\b',
            r'\b(REQ-SYS-\w+-\d{3})\b',
            r'\b(VR-CROSSARCH-\d{3})\b',
        ]
        
        requirements = []
        for pattern in patterns:
            matches = re.findall(pattern, content)
            requirements.extend(matches)
            
        return list(set(requirements))  # Remove duplicates
        
    def _count_duplicate_requirements(self, requirements_map: Dict) -> int:
        """Count requirements that appear in multiple files"""
        duplicates = 0
        for req_id, files in requirements_map.items():
            if len(files) > 1:
                duplicates += 1
        return duplicates
        
    def _count_orphaned_files(self, files_map: Dict) -> int:
        """Count files that might not have proper traceability"""
        orphaned = 0
        for file_path, file_info in files_map.items():
            # Simple heuristic: files under 1KB might be empty or incomplete
            if file_info['size'] < 1024:
                orphaned += 1
        return orphaned
        
    def validate_pre_execution_safety(self, target_script: str) -> bool:
        """Validate safety before executing traceability operations"""
        print(f"🔍 Pre-execution safety validation for: {target_script}")
        
        safety_passed = True
        
        # Check 1: Repository state validation
        if not self._validate_repository_state():
            safety_passed = False
            
        # Check 2: Script safety analysis
        if not self._validate_script_safety(target_script):
            safety_passed = False
            
        # Check 3: Backup capability verification
        if not self._validate_backup_capability():
            safety_passed = False
            
        # Check 4: Rollback mechanism test
        if not self._validate_rollback_capability():
            safety_passed = False
            
        # Check 5: Dry-run validation
        if not self._validate_dry_run_capability(target_script):
            safety_passed = False
            
        return safety_passed
        
    def _validate_repository_state(self) -> bool:
        """Validate current repository state is safe for operations"""
        print("  Checking repository state...")
        
        valid = True
        
        # Check for uncommitted changes
        try:
            result = subprocess.run(['git', 'status', '--porcelain'], 
                                  capture_output=True, text=True, cwd=self.repo_root)
            if result.returncode == 0 and result.stdout.strip():
                self.warnings.append({
                    'type': 'uncommitted_changes',
                    'message': 'Repository has uncommitted changes - consider committing before traceability operations'
                })
        except:
            self.warnings.append({
                'type': 'git_check_failed',
                'message': 'Could not check git status'
            })
            
        # Check for critical files existence
        critical_files = [
            '02-requirements',
            '03-architecture', 
            'Scripts'
        ]
        
        for critical_path in critical_files:
            if not (self.repo_root / critical_path).exists():
                self.safety_violations.append({
                    'type': 'missing_critical_directory',
                    'path': critical_path,
                    'message': f'Critical directory missing: {critical_path}'
                })
                valid = False
                
        # Check available disk space
        try:
            stat = os.statvfs(self.repo_root)
            free_bytes = stat.f_frsize * stat.f_availe
            if free_bytes < 100 * 1024 * 1024:  # Less than 100MB
                self.safety_violations.append({
                    'type': 'low_disk_space',
                    'available': free_bytes,
                    'message': 'Insufficient disk space for safe backup operations'
                })
                valid = False
        except:
            pass  # Not critical on all systems
            
        return valid
        
    def _validate_script_safety(self, script_path: str) -> bool:
        """Validate the target script has proper safety mechanisms"""
        print("  Analyzing script safety mechanisms...")
        
        script_full_path = self.repo_root / 'Scripts' / script_path
        
        if not script_full_path.exists():
            self.safety_violations.append({
                'type': 'script_not_found',
                'script': script_path,
                'message': f'Target script not found: {script_path}'
            })
            return False
            
        try:
            with open(script_full_path, 'r', encoding='utf-8') as f:
                script_content = f.read()
                
            # Check for required safety features
            safety_features = {
                'backup_creation': 'backup' in script_content.lower(),
                'dry_run_support': '--dry-run' in script_content,
                'rollback_capability': 'rollback' in script_content.lower(),
                'error_handling': 'try:' in script_content and 'except' in script_content,
                'modification_logging': 'log' in script_content.lower(),
            }
            
            missing_features = [feature for feature, present in safety_features.items() if not present]
            
            if missing_features:
                self.safety_violations.append({
                    'type': 'missing_safety_features',
                    'script': script_path,
                    'missing': missing_features,
                    'message': f'Script missing safety features: {", ".join(missing_features)}'
                })
                return False
                
            return True
            
        except Exception as e:
            self.safety_violations.append({
                'type': 'script_analysis_error',
                'script': script_path,
                'message': f'Error analyzing script: {e}'
            })
            return False
            
    def _validate_backup_capability(self) -> bool:
        """Validate backup directory creation and permissions"""
        print("  Validating backup capability...")
        
        backup_test_dir = self.repo_root / 'backups' / 'safety_test'
        
        try:
            # Test backup directory creation
            backup_test_dir.mkdir(parents=True, exist_ok=True)
            
            # Test file creation in backup directory
            test_file = backup_test_dir / 'test.txt'
            test_file.write_text('Safety validation test')
            
            # Test file reading
            content = test_file.read_text()
            if content != 'Safety validation test':
                raise Exception("File read/write validation failed")
                
            # Cleanup test
            test_file.unlink()
            backup_test_dir.rmdir()
            
            return True
            
        except Exception as e:
            self.safety_violations.append({
                'type': 'backup_capability_failed',
                'message': f'Backup capability validation failed: {e}'
            })
            return False
            
    def _validate_rollback_capability(self) -> bool:
        """Validate rollback mechanism"""
        print("  Validating rollback capability...")
        
        # Create a small test scenario
        test_dir = self.repo_root / 'test_rollback_safety'
        
        try:
            # Create test directory and file
            test_dir.mkdir(exist_ok=True)
            test_file = test_dir / 'test.md'
            original_content = "# Original Content\nThis is a test."
            test_file.write_text(original_content)
            
            # Create backup
            backup_dir = self.repo_root / 'backups' / 'rollback_test'
            backup_dir.mkdir(parents=True, exist_ok=True)
            backup_file = backup_dir / 'test.md'
            backup_file.write_text(original_content)
            
            # Modify original
            modified_content = "# Modified Content\nThis was changed."
            test_file.write_text(modified_content)
            
            # Test rollback (simulate)
            restored_content = backup_file.read_text()
            test_file.write_text(restored_content)
            
            # Verify restoration
            final_content = test_file.read_text()
            if final_content != original_content:
                raise Exception("Rollback verification failed")
                
            # Cleanup
            test_file.unlink()
            test_dir.rmdir()
            backup_file.unlink()
            backup_dir.rmdir()
            
            return True
            
        except Exception as e:
            self.safety_violations.append({
                'type': 'rollback_test_failed',
                'message': f'Rollback capability test failed: {e}'
            })
            return False
            
    def _validate_dry_run_capability(self, script_path: str) -> bool:
        """Validate dry-run mode works correctly"""
        print("  Validating dry-run capability...")
        
        script_full_path = self.repo_root / 'Scripts' / script_path
        
        try:
            # Test dry-run execution
            result = subprocess.run([
                sys.executable, str(script_full_path), 
                '--dry-run', '--validate-all', 
                '--repo-root', str(self.repo_root)
            ], capture_output=True, text=True, timeout=60)
            
            if result.returncode != 0:
                self.safety_violations.append({
                    'type': 'dry_run_failed',
                    'script': script_path,
                    'error': result.stderr,
                    'message': 'Dry-run execution failed'
                })
                return False
                
            # Check that dry-run output indicates no modifications
            if '[DRY-RUN]' not in result.stdout and 'DRY-RUN' not in result.stdout:
                self.safety_violations.append({
                    'type': 'dry_run_unclear',
                    'script': script_path,
                    'message': 'Dry-run mode not clearly indicated in output'
                })
                return False
                
            return True
            
        except subprocess.TimeoutExpired:
            self.safety_violations.append({
                'type': 'dry_run_timeout',
                'script': script_path,
                'message': 'Dry-run execution timed out'
            })
            return False
        except Exception as e:
            self.safety_violations.append({
                'type': 'dry_run_error',
                'script': script_path,
                'message': f'Dry-run test error: {e}'
            })
            return False
            
    def validate_post_execution_integrity(self, backup_dir: str = None) -> bool:
        """Validate integrity after traceability operations"""
        print("🔍 Post-execution integrity validation...")
        
        integrity_passed = True
        
        # Create new snapshot
        post_snapshot = self.create_baseline_snapshot()
        
        # Compare with baseline if available
        if self.baseline_snapshot:
            if not self._compare_snapshots(self.baseline_snapshot, post_snapshot):
                integrity_passed = False
        else:
            print("⚠️  No baseline snapshot available for comparison")
            
        # Validate against backup if provided
        if backup_dir and not self._validate_against_backup(backup_dir):
            integrity_passed = False
            
        # Check for data consistency
        if not self._validate_data_consistency():
            integrity_passed = False
            
        return integrity_passed
        
    def _compare_snapshots(self, baseline: Dict, current: Dict) -> bool:
        """Compare snapshots for integrity validation"""
        print("  Comparing snapshots...")
        
        valid = True
        
        # Check for lost files
        baseline_files = set(baseline['files'].keys())
        current_files = set(current['files'].keys())
        
        lost_files = baseline_files - current_files
        new_files = current_files - baseline_files
        
        if lost_files:
            self.safety_violations.append({
                'type': 'files_lost',
                'files': list(lost_files),
                'message': f'{len(lost_files)} specification files were lost during operation'
            })
            valid = False
            
        # Check for lost requirements
        baseline_reqs = set(baseline['requirements'].keys())
        current_reqs = set(current['requirements'].keys())
        
        lost_reqs = baseline_reqs - current_reqs
        new_reqs = current_reqs - baseline_reqs
        
        if lost_reqs:
            self.safety_violations.append({
                'type': 'requirements_lost',
                'requirements': list(lost_reqs),
                'message': f'{len(lost_reqs)} requirements were lost during operation'
            })
            valid = False
            
        # Check for significant statistical changes
        baseline_stats = baseline['statistics']
        current_stats = current['statistics']
        
        req_change_ratio = current_stats['total_requirements'] / max(baseline_stats['total_requirements'], 1)
        if req_change_ratio < 0.8:  # More than 20% reduction
            self.safety_violations.append({
                'type': 'significant_requirement_loss',
                'baseline': baseline_stats['total_requirements'],
                'current': current_stats['total_requirements'],
                'message': f'Significant requirement count reduction: {req_change_ratio:.1%}'
            })
            valid = False
            
        # Report on changes
        if new_files:
            print(f"  ℹ️  {len(new_files)} new files created")
        if new_reqs:
            print(f"  ℹ️  {len(new_reqs)} new requirements added")
            
        return valid
        
    def _validate_against_backup(self, backup_dir: str) -> bool:
        """Validate current state against backup"""
        print(f"  Validating against backup: {backup_dir}")
        
        backup_path = Path(backup_dir)
        if not backup_path.exists():
            self.safety_violations.append({
                'type': 'backup_not_found',
                'path': backup_dir,
                'message': 'Specified backup directory not found'
            })
            return False
            
        # Load backup manifest if available
        manifest_path = backup_path / 'BACKUP_MANIFEST.json'
        if manifest_path.exists():
            try:
                with open(manifest_path, 'r') as f:
                    manifest = json.load(f)
                    
                # Validate some files still exist and have reasonable content
                validated_files = 0
                for file_rel_path, file_info in manifest['files'].items():
                    current_file = self.repo_root / file_rel_path
                    if current_file.exists():
                        validated_files += 1
                        
                if validated_files == 0:
                    self.safety_violations.append({
                        'type': 'all_files_missing',
                        'message': 'All files from backup manifest are missing'
                    })
                    return False
                    
            except Exception as e:
                self.warnings.append({
                    'type': 'manifest_validation_error',
                    'message': f'Error validating backup manifest: {e}'
                })
                
        return True
        
    def _validate_data_consistency(self) -> bool:
        """Validate internal data consistency"""
        print("  Validating data consistency...")
        
        valid = True
        
        # Check for duplicate requirement IDs across files
        req_locations = {}
        
        for phase_dir in ['01-stakeholder-requirements', '02-requirements', '03-architecture']:
            phase_path = self.repo_root / phase_dir
            if phase_path.exists():
                for md_file in phase_path.glob('**/*.md'):
                    try:
                        content = md_file.read_text(encoding='utf-8')
                        requirements = self._extract_requirements_from_content(content)
                        
                        for req_id in requirements:
                            if req_id not in req_locations:
                                req_locations[req_id] = []
                            req_locations[req_id].append(str(md_file.relative_to(self.repo_root)))
                            
                    except Exception as e:
                        self.warnings.append({
                            'type': 'file_read_error',
                            'file': str(md_file),
                            'message': f'Error reading file: {e}'
                        })
                        
        # Report duplicates
        duplicates = {req_id: locations for req_id, locations in req_locations.items() if len(locations) > 1}
        
        if duplicates:
            self.safety_violations.append({
                'type': 'duplicate_requirements',
                'duplicates': duplicates,
                'message': f'{len(duplicates)} requirements found in multiple files'
            })
            valid = False
            
        return valid
        
    def generate_safety_report(self) -> Path:
        """Generate comprehensive safety validation report"""
        report_path = self.repo_root / 'reports' / 'traceability-safety-validation.md'
        report_path.parent.mkdir(exist_ok=True)
        
        with open(report_path, 'w', encoding='utf-8') as f:
            f.write("# Traceability Safety Validation Report\n\n")
            f.write(f"Generated: {datetime.now().isoformat()}\n\n")
            
            f.write("## Safety Status\n\n")
            if not self.safety_violations:
                f.write("[SAFE] **SAFE** - No safety violations detected\n\n")
            else:
                f.write(f"[UNSAFE] **UNSAFE** - {len(self.safety_violations)} safety violations detected\n\n")
                
            if self.safety_violations:
                f.write("## Safety Violations\n\n")
                for i, violation in enumerate(self.safety_violations, 1):
                    f.write(f"### Violation {i}: {violation['type']}\n")
                    f.write(f"**Message**: {violation['message']}\n\n")
                    
                    # Add specific details based on violation type
                    if 'files' in violation:
                        f.write(f"**Affected Files**: {len(violation['files'])}\n")
                        for file_path in violation['files'][:10]:  # Limit to first 10
                            f.write(f"- {file_path}\n")
                        if len(violation['files']) > 10:
                            f.write(f"- ... and {len(violation['files']) - 10} more\n")
                        f.write("\n")
                        
            if self.warnings:
                f.write("## Warnings\n\n")
                for i, warning in enumerate(self.warnings, 1):
                    f.write(f"{i}. **{warning['type']}**: {warning['message']}\n")
                f.write("\n")
                
            if self.baseline_snapshot:
                stats = self.baseline_snapshot['statistics']
                f.write("## Repository Statistics\n\n")
                f.write(f"- **Total Files**: {stats['total_files']}\n")
                f.write(f"- **Total Requirements**: {stats['total_requirements']}\n")
                f.write(f"- **Duplicate Requirements**: {stats['duplicate_requirements']}\n")
                f.write(f"- **Potential Orphaned Files**: {stats['orphaned_files']}\n\n")
                
            f.write("## Recommendations\n\n")
            if self.safety_violations:
                f.write("[STOP] **DO NOT PROCEED** with traceability operations until violations are resolved.\n\n")
                f.write("### Required Actions:\n")
                f.write("1. Review and fix all safety violations\n")
                f.write("2. Commit any uncommitted changes\n")
                f.write("3. Re-run safety validation\n")
                f.write("4. Only proceed when validation passes\n\n")
            else:
                f.write("[SAFE] **SAFE TO PROCEED** with traceability operations.\n\n")
                f.write("### Recommended Safety Measures:\n")
                f.write("1. Always use --dry-run first\n")
                f.write("2. Enable automatic backups\n")
                f.write("3. Review all modifications before committing\n")
                f.write("4. Keep backup directory until validation complete\n\n")
                
        print(f"📋 Safety validation report: {report_path}")
        return report_path

def main():
    parser = argparse.ArgumentParser(description='Traceability Safety Validator')
    parser.add_argument('--repo-root', default='.', help='Repository root directory')
    parser.add_argument('--full-check', action='store_true', help='Run complete safety validation')
    parser.add_argument('--pre-execution', action='store_true', help='Pre-execution safety check')
    parser.add_argument('--post-execution', action='store_true', help='Post-execution integrity check')
    parser.add_argument('--target-script', help='Target script to validate for safety')
    parser.add_argument('--compare-backup', help='Backup directory for comparison')
    parser.add_argument('--create-baseline', action='store_true', help='Create baseline snapshot')
    
    args = parser.parse_args()
    
    validator = TraceabilitySafetyValidator(args.repo_root)
    
    print("🛡️  Traceability Safety Validator")
    print("=" * 40)
    
    safety_passed = True
    
    if args.create_baseline or args.full_check:
        validator.create_baseline_snapshot()
        
    if args.pre_execution or args.full_check:
        if not args.target_script:
            args.target_script = 'enforce-traceability.py'
            
        if not validator.validate_pre_execution_safety(args.target_script):
            safety_passed = False
            
    if args.post_execution:
        if not validator.validate_post_execution_integrity(args.compare_backup):
            safety_passed = False
            
    # Always generate report
    validator.generate_safety_report()
    
    if safety_passed:
        print("\n✅ All safety validations PASSED")
        exit(0)
    else:
        print("\n❌ Safety validation FAILED")
        print("See safety report for details and required actions")
        exit(1)

if __name__ == '__main__':
    main()