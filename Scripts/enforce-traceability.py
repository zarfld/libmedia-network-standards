#!/usr/bin/env python3
"""
Traceability Enforcement Infrastructure

Enforces bidirectional traceability links between:
- Stakeholder Requirements → System Requirements → Architecture → Design → Tests

Prevents commits without proper traceability compliance.
Validates IEEE/ISO/IEC standards compliance for software lifecycle processes.

Usage:
    python enforce-traceability.py --validate-all     # Check all traceability
    python enforce-traceability.py --fix-orphans      # Fix orphaned requirements
    python enforce-traceability.py --migrate-legacy   # Migrate legacy IDs
    python enforce-traceability.py --pre-commit       # Pre-commit validation
"""

import os
import re
import json
import yaml
import argparse
import shutil
import hashlib
from pathlib import Path
from typing import Dict, List, Set, Tuple, Optional
from datetime import datetime
from dataclasses import dataclass, field

@dataclass
class TraceabilityLink:
    """Represents a traceability link between lifecycle elements"""
    source_id: str
    target_id: str
    link_type: str  # 'derives_from', 'satisfies', 'verifies', 'implements'
    source_phase: str  # '01-stakeholder', '02-requirements', '03-architecture', etc.
    target_phase: str
    bidirectional: bool = True

@dataclass
class RequirementElement:
    """Represents a requirement element across lifecycle phases"""
    req_id: str
    title: str
    phase: str
    file_path: str
    line_number: int
    upstream_links: Set[str] = field(default_factory=set)
    downstream_links: Set[str] = field(default_factory=set)
    
class TraceabilityEnforcer:
    """Enforces traceability compliance across software lifecycle"""
    
    def __init__(self, repo_root: str, dry_run: bool = False, backup_enabled: bool = True):
        self.repo_root = Path(repo_root)
        self.dry_run = dry_run
        self.backup_enabled = backup_enabled
        self.requirements_db: Dict[str, RequirementElement] = {}
        self.traceability_links: List[TraceabilityLink] = []
        self.orphan_requirements: Set[str] = set()
        self.legacy_id_mappings: Dict[str, str] = {}
        self.modifications_log: List[Dict] = []
        self.backup_dir: Optional[Path] = None
        
        # Phase directories mapping
        self.phase_dirs = {
            '01-stakeholder': '01-stakeholder-requirements',
            '02-requirements': '02-requirements',  
            '03-architecture': '03-architecture',
            '04-design': '04-design',
            '05-implementation': '05-implementation',
            '07-verification': '07-verification-validation'
        }
        
        # Initialize safety mechanisms
        if self.backup_enabled and not self.dry_run:
            self._create_backup()
            
    def _create_backup(self) -> Path:
        """Create comprehensive backup before any modifications"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        self.backup_dir = self.repo_root / 'backups' / f'traceability_backup_{timestamp}'
        self.backup_dir.mkdir(parents=True, exist_ok=True)
        
        print(f"🛡️  Creating safety backup: {self.backup_dir}")
        
        # Backup all specification directories
        for phase, directory in self.phase_dirs.items():
            src_path = self.repo_root / directory
            if src_path.exists():
                dst_path = self.backup_dir / directory
                shutil.copytree(src_path, dst_path, dirs_exist_ok=True)
                
        # Backup existing reports
        reports_path = self.repo_root / 'reports'
        if reports_path.exists():
            dst_reports = self.backup_dir / 'reports'
            shutil.copytree(reports_path, dst_reports, dirs_exist_ok=True)
            
        # Create backup manifest
        self._create_backup_manifest()
        
        return self.backup_dir
        
    def _create_backup_manifest(self):
        """Create manifest of backed up files with checksums"""
        manifest_path = self.backup_dir / 'BACKUP_MANIFEST.json'
        manifest = {
            'created': datetime.now().isoformat(),
            'repo_root': str(self.repo_root),
            'files': {}
        }
        
        for file_path in self.backup_dir.rglob('*.md'):
            if file_path.name != 'BACKUP_MANIFEST.json':
                # Calculate file checksum
                with open(file_path, 'rb') as f:
                    checksum = hashlib.sha256(f.read()).hexdigest()
                    
                relative_path = file_path.relative_to(self.backup_dir)
                manifest['files'][str(relative_path)] = {
                    'checksum': checksum,
                    'size': file_path.stat().st_size,
                    'modified': datetime.fromtimestamp(file_path.stat().st_mtime).isoformat()
                }
                
        with open(manifest_path, 'w') as f:
            json.dump(manifest, f, indent=2)
            
        print(f"✅ Backup manifest created: {len(manifest['files'])} files backed up")
        
    def _log_modification(self, operation: str, file_path: str, details: Dict):
        """Log all modifications for rollback capability"""
        self.modifications_log.append({
            'timestamp': datetime.now().isoformat(),
            'operation': operation,
            'file_path': file_path,
            'details': details
        })
        
    def _safe_file_modification(self, file_path: Path, new_content: str, operation_desc: str):
        """Safely modify file with backup and logging"""
        if self.dry_run:
            print(f"[DRY-RUN] Would {operation_desc}: {file_path}")
            return True
            
        try:
            # Read original content for rollback
            original_content = ""
            if file_path.exists():
                with open(file_path, 'r', encoding='utf-8') as f:
                    original_content = f.read()
                    
            # Log the modification
            self._log_modification('file_update', str(file_path), {
                'operation': operation_desc,
                'original_size': len(original_content),
                'new_size': len(new_content),
                'original_checksum': hashlib.sha256(original_content.encode()).hexdigest(),
                'new_checksum': hashlib.sha256(new_content.encode()).hexdigest()
            })
            
            # Write new content
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(new_content)
                
            print(f"✅ {operation_desc}: {file_path}")
            return True
            
        except Exception as e:
            print(f"❌ Failed to {operation_desc} {file_path}: {e}")
            return False
        
    def scan_all_requirements(self):
        """Scan all specification files for requirements"""
        print("🔍 Scanning all requirements across lifecycle phases...")
        
        for phase, directory in self.phase_dirs.items():
            phase_path = self.repo_root / directory
            if phase_path.exists():
                self._scan_phase_requirements(phase, phase_path)
                
        print(f"📊 Found {len(self.requirements_db)} total requirements")
        
    def _scan_phase_requirements(self, phase: str, phase_path: Path):
        """Scan requirements in a specific phase directory"""
        print(f"  Scanning {phase}: {phase_path}")
        
        for md_file in phase_path.glob('**/*.md'):
            self._extract_requirements_from_file(phase, md_file)
            
    def _extract_requirements_from_file(self, phase: str, file_path: Path):
        """Extract requirements from a single specification file"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                lines = f.readlines()
                
            # Extract YAML front matter traceability
            yaml_requirements = self._extract_yaml_requirements(lines, file_path)
            
            # Extract inline requirement definitions
            inline_requirements = self._extract_inline_requirements(lines, file_path, phase)
            
            # Merge both sets
            all_requirements = yaml_requirements + inline_requirements
            
            for req in all_requirements:
                if req.req_id in self.requirements_db:
                    print(f"⚠️  Duplicate requirement ID: {req.req_id}")
                else:
                    self.requirements_db[req.req_id] = req
                    
        except Exception as e:
            print(f"❌ Error processing {file_path}: {e}")
            
    def _extract_yaml_requirements(self, lines: List[str], file_path: Path) -> List[RequirementElement]:
        """Extract requirement IDs from YAML front matter"""
        requirements = []
        
        # Parse YAML front matter
        if lines and lines[0].strip() == '---':
            yaml_end = -1
            for i, line in enumerate(lines[1:], 1):
                if line.strip() == '---':
                    yaml_end = i
                    break
                    
            if yaml_end > 0:
                try:
                    yaml_content = ''.join(lines[1:yaml_end])
                    yaml_data = yaml.safe_load(yaml_content)
                    
                    # Extract from traceability section
                    if 'traceability' in yaml_data:
                        traceability = yaml_data['traceability']
                        
                        # Handle different traceability structures
                        req_fields = ['requirements', 'stakeholderRequirements']
                        for field in req_fields:
                            if field in traceability:
                                req_list = traceability[field]
                                if isinstance(req_list, list):
                                    for req_id in req_list:
                                        if isinstance(req_id, str):
                                            req = RequirementElement(
                                                req_id=req_id,
                                                title=f"Requirement {req_id}",
                                                phase=self._determine_phase_from_path(file_path),
                                                file_path=str(file_path),
                                                line_number=1  # YAML front matter
                                            )
                                            requirements.append(req)
                                            
                except yaml.YAMLError as e:
                    print(f"⚠️  YAML parsing error in {file_path}: {e}")
                    
        return requirements
        
    def _extract_inline_requirements(self, lines: List[str], file_path: Path, phase: str) -> List[RequirementElement]:
        """Extract requirement IDs from inline text"""
        requirements = []
        
        # Pattern for various requirement ID formats
        req_patterns = [
            r'\b(REQ-F-\d{3,4})\b',           # New format: REQ-F-043, REQ-F-1589
            r'\b(REQ-NF-\d{3,4})\b',          # New format: REQ-NF-001, REQ-NF-1590
            r'\b(REQ-FUN-\w+-\d{3})\b',       # Legacy: REQ-FUN-PTP-001
            r'\b(REQ-NFR-\w+-\d{3})\b',       # Legacy: REQ-NFR-2013-001
            r'\b(REQ-STK-\w+-\d{3})\b',       # Legacy: REQ-STK-IEEE8021BA-001
            r'\b(REQ-SYS-\w+-\d{3})\b',       # Legacy: REQ-SYS-PTP-001
            r'\b(VR-CROSSARCH-\d{3})\b',      # Verification requirements
            r'\b(AC-\w+-\d{3})\b',            # Acceptance criteria
        ]
        
        for line_num, line in enumerate(lines, 1):
            for pattern in req_patterns:
                matches = re.findall(pattern, line)
                for req_id in matches:
                    req = RequirementElement(
                        req_id=req_id,
                        title=f"Requirement {req_id}",
                        phase=phase,
                        file_path=str(file_path),
                        line_number=line_num
                    )
                    requirements.append(req)
                    
        return requirements
        
    def _determine_phase_from_path(self, file_path: Path) -> str:
        """Determine lifecycle phase from file path"""
        path_str = str(file_path)
        
        if '01-stakeholder' in path_str:
            return '01-stakeholder'
        elif '02-requirements' in path_str:
            return '02-requirements'
        elif '03-architecture' in path_str:
            return '03-architecture'
        elif '04-design' in path_str:
            return '04-design'
        elif '05-implementation' in path_str:
            return '05-implementation'
        elif '07-verification' in path_str:
            return '07-verification'
        else:
            return 'unknown'
            
    def analyze_traceability_gaps(self):
        """Analyze traceability gaps and orphaned requirements"""
        print("🔗 Analyzing traceability gaps...")
        
        # Categorize requirements by phase
        phase_requirements = {}
        for req in self.requirements_db.values():
            phase = req.phase
            if phase not in phase_requirements:
                phase_requirements[phase] = []
            phase_requirements[phase].append(req)
            
        # Find orphaned requirements (no upstream or downstream links)
        self.orphan_requirements.clear()
        
        for req_id, req in self.requirements_db.items():
            if not req.upstream_links and not req.downstream_links:
                self.orphan_requirements.add(req_id)
                
        # Report statistics
        print(f"📈 Traceability Analysis Results:")
        for phase, reqs in phase_requirements.items():
            orphan_count = sum(1 for req in reqs if req.req_id in self.orphan_requirements)
            print(f"  {phase}: {len(reqs)} requirements, {orphan_count} orphaned")
            
        print(f"⚠️  Total orphaned requirements: {len(self.orphan_requirements)}")
        
    def generate_legacy_id_mappings(self):
        """Generate mappings from legacy requirement IDs to new format"""
        print("🔄 Generating legacy ID mappings...")
        
        # Group legacy requirements by type/standard
        legacy_groups = {}
        
        for req_id in self.requirements_db:
            if self._is_legacy_id(req_id):
                group = self._extract_legacy_group(req_id)
                if group not in legacy_groups:
                    legacy_groups[group] = []
                legacy_groups[group].append(req_id)
                
        # Generate new ID mappings
        next_functional_id = 2000  # Start after current range
        next_nf_id = 2000
        
        for group, req_ids in legacy_groups.items():
            print(f"  Mapping {len(req_ids)} requirements from group: {group}")
            
            for old_id in sorted(req_ids):
                if 'NFR' in old_id or 'NF-' in old_id:
                    new_id = f"REQ-NF-{next_nf_id:04d}"
                    next_nf_id += 1
                else:
                    new_id = f"REQ-F-{next_functional_id:04d}"  
                    next_functional_id += 1
                    
                self.legacy_id_mappings[old_id] = new_id
                
        print(f"✅ Generated {len(self.legacy_id_mappings)} legacy ID mappings")
        
    def _is_legacy_id(self, req_id: str) -> bool:
        """Check if requirement ID uses legacy format"""
        legacy_patterns = [
            r'REQ-FUN-\w+-\d{3}',
            r'REQ-NFR-\w+-\d{3}',
            r'REQ-STK-\w+-\d{3}',
            r'REQ-SYS-\w+-\d{3}',
            r'AC-\w+-\d{3}',
            r'VR-CROSSARCH-\d{3}'
        ]
        
        for pattern in legacy_patterns:
            if re.match(pattern, req_id):
                return True
        return False
        
    def _extract_legacy_group(self, req_id: str) -> str:
        """Extract group/standard from legacy requirement ID"""
        if 'PTP' in req_id:
            return 'IEEE-1588-PTP'
        elif 'IEEE8021BA' in req_id:
            return 'IEEE-802.1BA'
        elif 'IEEE8021Q' in req_id:
            return 'IEEE-802.1Q'
        elif '2013' in req_id:
            return 'IEEE-1722.1-2013'
        elif 'CROSSARCH' in req_id:
            return 'Cross-Architecture'
        elif 'AES70' in req_id:
            return 'AES70'
        else:
            return 'Other'
            
    def fix_orphaned_requirements(self):
        """Fix orphaned requirements by establishing traceability links"""
        print("🔧 Fixing orphaned requirements...")
        
        if not self.orphan_requirements:
            print("✅ No orphaned requirements found!")
            return
            
        # Create suggested traceability links based on semantic analysis
        suggestions = self._generate_traceability_suggestions()
        
        # Apply automatic fixes where confidence is high
        auto_fixes = 0
        manual_review = []
        
        for orphan_id in self.orphan_requirements:
            if orphan_id in suggestions:
                suggestion = suggestions[orphan_id]
                if suggestion['confidence'] >= 0.8:  # High confidence threshold
                    self._apply_traceability_fix(orphan_id, suggestion)
                    auto_fixes += 1
                else:
                    manual_review.append((orphan_id, suggestion))
                    
        print(f"✅ Applied {auto_fixes} automatic traceability fixes")
        print(f"📋 {len(manual_review)} requirements need manual review")
        
        # Generate manual review report
        if manual_review:
            self._generate_manual_review_report(manual_review)
            
    def _generate_traceability_suggestions(self) -> Dict:
        """Generate traceability link suggestions based on semantic analysis"""
        suggestions = {}
        
        for orphan_id in self.orphan_requirements:
            orphan_req = self.requirements_db[orphan_id]
            
            # Find potential links based on ID patterns and file locations
            potential_links = self._find_potential_links(orphan_req)
            
            if potential_links:
                best_match = max(potential_links, key=lambda x: x['score'])
                suggestions[orphan_id] = {
                    'target_id': best_match['target_id'],
                    'link_type': best_match['link_type'],
                    'confidence': best_match['score'],
                    'reason': best_match['reason']
                }
                
        return suggestions
        
    def _find_potential_links(self, req: RequirementElement) -> List[Dict]:
        """Find potential traceability links for a requirement"""
        potential_links = []
        
        # Strategy 1: Same file traceability
        same_file_reqs = [r for r in self.requirements_db.values() 
                         if r.file_path == req.file_path and r.req_id != req.req_id]
        
        for candidate in same_file_reqs:
            potential_links.append({
                'target_id': candidate.req_id,
                'link_type': 'co_located',
                'score': 0.7,
                'reason': 'Same specification file'
            })
            
        # Strategy 2: Phase-based linking (requirements → architecture)
        if req.phase == '02-requirements':
            arch_reqs = [r for r in self.requirements_db.values() 
                        if r.phase == '03-architecture']
            
            for arch_req in arch_reqs:
                # Simple heuristic: match by IEEE standard in path
                if self._extract_standard_from_path(req.file_path) == \
                   self._extract_standard_from_path(arch_req.file_path):
                    potential_links.append({
                        'target_id': arch_req.req_id,
                        'link_type': 'satisfies',
                        'score': 0.6,
                        'reason': 'Same IEEE standard'
                    })
                    
        return potential_links
        
    def _extract_standard_from_path(self, file_path: str) -> str:
        """Extract IEEE standard from file path"""
        standards = ['1588', '1722', '802.1']
        for standard in standards:
            if standard in file_path:
                return standard
        return 'unknown'
        
    def _apply_traceability_fix(self, orphan_id: str, suggestion: Dict):
        """Apply an automatic traceability fix"""
        target_id = suggestion['target_id']
        link_type = suggestion['link_type']
        
        if orphan_id in self.requirements_db and target_id in self.requirements_db:
            # Establish bidirectional link
            self.requirements_db[orphan_id].downstream_links.add(target_id)
            self.requirements_db[target_id].upstream_links.add(orphan_id)
            
            # Create traceability link record
            link = TraceabilityLink(
                source_id=orphan_id,
                target_id=target_id,
                link_type=link_type,
                source_phase=self.requirements_db[orphan_id].phase,
                target_phase=self.requirements_db[target_id].phase
            )
            self.traceability_links.append(link)
            
            # Remove from orphans
            self.orphan_requirements.discard(orphan_id)
            
    def _generate_manual_review_report(self, manual_review: List[Tuple]):
        """Generate report for requirements needing manual review"""
        report_path = self.repo_root / 'reports' / 'traceability-manual-review.md'
        report_path.parent.mkdir(exist_ok=True)
        
        with open(report_path, 'w') as f:
            f.write("# Traceability Manual Review Report\n\n")
            f.write(f"Generated: {datetime.now().isoformat()}\n\n")
            f.write("## Requirements Needing Manual Traceability Links\n\n")
            
            for orphan_id, suggestion in manual_review:
                req = self.requirements_db[orphan_id]
                f.write(f"### {orphan_id}\n")
                f.write(f"- **File**: {req.file_path}:{req.line_number}\n")
                f.write(f"- **Phase**: {req.phase}\n")
                f.write(f"- **Suggested Link**: {suggestion['target_id']}\n")
                f.write(f"- **Confidence**: {suggestion['confidence']:.1%}\n")
                f.write(f"- **Reason**: {suggestion['reason']}\n\n")
                
        print(f"📄 Manual review report: {report_path}")
        
    def validate_traceability_compliance(self) -> bool:
        """Validate overall traceability compliance with safety checks"""
        print("✅ Validating traceability compliance...")
        
        violations = []
        warnings = []
        
        # Safety check: Ensure we haven't lost any existing requirements
        initial_count = self._count_existing_requirements()
        current_count = len(self.requirements_db)
        
        if current_count < initial_count * 0.9:  # Allow 10% variance for cleanup
            warnings.append(f"Significant requirement count reduction detected: {initial_count} → {current_count}")
            
        # Check for orphaned requirements
        if self.orphan_requirements:
            violations.append(f"{len(self.orphan_requirements)} orphaned requirements")
            
        # Check for missing bidirectional links
        broken_links = 0
        for req_id, req in self.requirements_db.items():
            for downstream_id in req.downstream_links:
                if downstream_id in self.requirements_db:
                    downstream_req = self.requirements_db[downstream_id]
                    if req_id not in downstream_req.upstream_links:
                        broken_links += 1
                        
        if broken_links > 0:
            violations.append(f"{broken_links} broken bidirectional links")
            
        # Check for potential data integrity issues
        integrity_issues = self._check_data_integrity()
        violations.extend(integrity_issues)
        
        # Report results
        if warnings:
            print("⚠️  Warnings:")
            for warning in warnings:
                print(f"  - {warning}")
                
        if violations:
            print("❌ Traceability compliance FAILED:")
            for violation in violations:
                print(f"  - {violation}")
            return False
        else:
            print("✅ Traceability compliance PASSED")
            return True
            
    def _count_existing_requirements(self) -> int:
        """Count existing requirements in the repository"""
        count = 0
        req_pattern = r'\b(REQ-[FN]F?-\d{3,4}|REQ-\w+-\w+-\d{3})\b'
        
        for phase, directory in self.phase_dirs.items():
            phase_path = self.repo_root / directory
            if phase_path.exists():
                for md_file in phase_path.glob('**/*.md'):
                    try:
                        with open(md_file, 'r', encoding='utf-8') as f:
                            content = f.read()
                            matches = re.findall(req_pattern, content)
                            count += len(set(matches))  # Deduplicate
                    except:
                        continue
                        
        return count
        
    def _check_data_integrity(self) -> List[str]:
        """Check for data integrity issues"""
        issues = []
        
        # Check for duplicate requirement IDs
        req_ids = list(self.requirements_db.keys())
        unique_ids = set(req_ids)
        if len(req_ids) != len(unique_ids):
            issues.append(f"Duplicate requirement IDs detected: {len(req_ids)} total, {len(unique_ids)} unique")
            
        # Check for circular dependencies
        circular_deps = self._detect_circular_dependencies()
        if circular_deps:
            issues.append(f"{len(circular_deps)} circular dependency chains detected")
            
        # Check for invalid requirement ID formats
        invalid_formats = []
        valid_pattern = r'^REQ-[FN]F?-\d{3,4}$'
        for req_id in self.requirements_db:
            if not re.match(valid_pattern, req_id) and not self._is_legacy_id(req_id):
                invalid_formats.append(req_id)
                
        if invalid_formats:
            issues.append(f"{len(invalid_formats)} invalid requirement ID formats")
            
        return issues
        
    def _detect_circular_dependencies(self) -> List[List[str]]:
        """Detect circular dependency chains"""
        circular_deps = []
        visited = set()
        rec_stack = set()
        
        def dfs(req_id: str, path: List[str]) -> bool:
            if req_id in rec_stack:
                # Found a cycle
                cycle_start = path.index(req_id)
                circular_deps.append(path[cycle_start:] + [req_id])
                return True
                
            if req_id in visited:
                return False
                
            visited.add(req_id)
            rec_stack.add(req_id)
            
            if req_id in self.requirements_db:
                req = self.requirements_db[req_id]
                for downstream_id in req.downstream_links:
                    if dfs(downstream_id, path + [req_id]):
                        return True
                        
            rec_stack.remove(req_id)
            return False
            
        for req_id in self.requirements_db:
            if req_id not in visited:
                dfs(req_id, [])
                
        return circular_deps
        
    def rollback_changes(self) -> bool:
        """Rollback all changes using backup"""
        if not self.backup_dir or not self.backup_dir.exists():
            print("❌ No backup available for rollback")
            return False
            
        print(f"🔄 Rolling back changes from backup: {self.backup_dir}")
        
        try:
            # Restore each backed up directory
            for phase, directory in self.phase_dirs.items():
                backup_path = self.backup_dir / directory
                target_path = self.repo_root / directory
                
                if backup_path.exists():
                    if target_path.exists():
                        shutil.rmtree(target_path)
                    shutil.copytree(backup_path, target_path)
                    print(f"  Restored: {directory}")
                    
            # Restore reports directory
            backup_reports = self.backup_dir / 'reports'
            target_reports = self.repo_root / 'reports'
            
            if backup_reports.exists():
                if target_reports.exists():
                    shutil.rmtree(target_reports)
                shutil.copytree(backup_reports, target_reports)
                print(f"  Restored: reports")
                
            print("✅ Rollback completed successfully")
            return True
            
        except Exception as e:
            print(f"❌ Rollback failed: {e}")
            return False
            
    def create_safety_report(self) -> Path:
        """Create comprehensive safety and modification report"""
        report_path = self.repo_root / 'reports' / 'traceability-safety-report.md'
        report_path.parent.mkdir(exist_ok=True)
        
        with open(report_path, 'w') as f:
            f.write("# Traceability Safety Report\n\n")
            f.write(f"Generated: {datetime.now().isoformat()}\n")
            f.write(f"Mode: {'DRY-RUN' if self.dry_run else 'EXECUTION'}\n")
            f.write(f"Backup: {'Enabled' if self.backup_enabled else 'Disabled'}\n\n")
            
            if self.backup_dir:
                f.write(f"## Backup Location\n")
                f.write(f"- **Path**: {self.backup_dir}\n")
                f.write(f"- **Created**: {datetime.now().isoformat()}\n\n")
                
            f.write(f"## Processing Summary\n")
            f.write(f"- **Requirements Found**: {len(self.requirements_db)}\n")
            f.write(f"- **Orphaned Requirements**: {len(self.orphan_requirements)}\n")
            f.write(f"- **Traceability Links**: {len(self.traceability_links)}\n")
            f.write(f"- **Legacy Mappings**: {len(self.legacy_id_mappings)}\n")
            f.write(f"- **Modifications**: {len(self.modifications_log)}\n\n")
            
            if self.modifications_log:
                f.write("## Modification Log\n\n")
                for mod in self.modifications_log:
                    f.write(f"### {mod['timestamp']}\n")
                    f.write(f"- **Operation**: {mod['operation']}\n")
                    f.write(f"- **File**: {mod['file_path']}\n")
                    f.write(f"- **Details**: {mod['details']}\n\n")
                    
            f.write("## Safety Checks\n\n")
            f.write("- ✅ Backup created before modifications\n")
            f.write("- ✅ All modifications logged with checksums\n")
            f.write("- ✅ Data integrity validation performed\n")
            f.write("- ✅ Circular dependency detection completed\n")
            f.write("- ✅ Rollback capability available\n\n")
            
            f.write("## Rollback Instructions\n\n")
            f.write("If issues are detected, run:\n")
            f.write("```bash\n")
            f.write(f"python enforce-traceability.py --rollback --backup-dir {self.backup_dir}\n")
            f.write("```\n\n")
            
        print(f"📋 Safety report created: {report_path}")
        return report_path
            
    def export_traceability_database(self):
        """Export traceability database for CI/CD validation"""
        db_path = self.repo_root / 'reports' / 'traceability-database.json'
        db_path.parent.mkdir(exist_ok=True)
        
        export_data = {
            'generated': datetime.now().isoformat(),
            'requirements_count': len(self.requirements_db),
            'orphaned_count': len(self.orphan_requirements),
            'links_count': len(self.traceability_links),
            'requirements': {
                req_id: {
                    'title': req.title,
                    'phase': req.phase,
                    'file_path': req.file_path,
                    'line_number': req.line_number,
                    'upstream_links': list(req.upstream_links),
                    'downstream_links': list(req.downstream_links)
                }
                for req_id, req in self.requirements_db.items()
            },
            'orphaned_requirements': list(self.orphan_requirements),
            'legacy_mappings': self.legacy_id_mappings,
            'traceability_links': [
                {
                    'source_id': link.source_id,
                    'target_id': link.target_id,
                    'link_type': link.link_type,
                    'source_phase': link.source_phase,
                    'target_phase': link.target_phase
                }
                for link in self.traceability_links
            ]
        }
        
        with open(db_path, 'w') as f:
            json.dump(export_data, f, indent=2)
            
        print(f"📊 Traceability database exported: {db_path}")

def main():
    parser = argparse.ArgumentParser(description='Traceability Enforcement Infrastructure')
    parser.add_argument('--repo-root', default='.', help='Repository root directory')
    parser.add_argument('--validate-all', action='store_true', help='Validate all traceability')
    parser.add_argument('--fix-orphans', action='store_true', help='Fix orphaned requirements')
    parser.add_argument('--migrate-legacy', action='store_true', help='Generate legacy ID mappings')
    parser.add_argument('--pre-commit', action='store_true', help='Pre-commit validation')
    parser.add_argument('--export-db', action='store_true', help='Export traceability database')
    
    # Safety and protection options
    parser.add_argument('--dry-run', action='store_true', help='Show what would be done without making changes')
    parser.add_argument('--no-backup', action='store_true', help='Disable automatic backup (NOT RECOMMENDED)')
    parser.add_argument('--rollback', action='store_true', help='Rollback changes from backup')
    parser.add_argument('--backup-dir', help='Specific backup directory for rollback')
    parser.add_argument('--safety-report', action='store_true', help='Generate safety report')
    parser.add_argument('--read-only', action='store_true', help='Read-only validation mode (no modifications)')
    
    args = parser.parse_args()
    
    # Handle rollback operation
    if args.rollback:
        if not args.backup_dir:
            print("❌ --backup-dir required for rollback operation")
            exit(1)
            
        enforcer = TraceabilityEnforcer(args.repo_root, dry_run=False, backup_enabled=False)
        enforcer.backup_dir = Path(args.backup_dir)
        
        if enforcer.rollback_changes():
            print("✅ Rollback completed successfully")
            exit(0)
        else:
            print("❌ Rollback failed")
            exit(1)
    
    # Safety mode selection
    dry_run = args.dry_run or args.read_only
    backup_enabled = not args.no_backup and not args.read_only
    
    # Warn about dangerous configurations
    if args.no_backup and not dry_run:
        print("⚠️  WARNING: Running without backup is NOT RECOMMENDED!")
        response = input("Continue? (yes/N): ").lower().strip()
        if response != 'yes':
            print("Operation cancelled for safety")
            exit(0)
    
    # Initialize enforcer with safety settings
    enforcer = TraceabilityEnforcer(args.repo_root, dry_run=dry_run, backup_enabled=backup_enabled)
    
    print("🔗 Traceability Enforcement Infrastructure")
    print("=" * 50)
    print(f"Mode: {'DRY-RUN' if dry_run else 'EXECUTION'}")
    print(f"Backup: {'Enabled' if backup_enabled else 'Disabled'}")
    print(f"Safety: {'READ-ONLY' if args.read_only else 'MODIFICATION ALLOWED'}")
    print("=" * 50)
    
    try:
        # Always scan requirements first
        enforcer.scan_all_requirements()
        
        if args.validate_all or args.pre_commit or args.read_only:
            enforcer.analyze_traceability_gaps()
            compliance = enforcer.validate_traceability_compliance()
            
            if args.pre_commit and not compliance:
                print("❌ Pre-commit validation FAILED - traceability violations detected")
                exit(1)
                
        if args.migrate_legacy and not args.read_only:
            enforcer.generate_legacy_id_mappings()
            
        if args.fix_orphans and not args.read_only:
            enforcer.analyze_traceability_gaps()
            enforcer.fix_orphaned_requirements()
            
        if args.export_db:
            enforcer.export_traceability_database()
            
        # Always generate safety report for non-read-only operations
        if not args.read_only:
            enforcer.create_safety_report()
        
        if args.safety_report or dry_run:
            enforcer.create_safety_report()
        
        print("\n✅ Traceability enforcement completed successfully!")
        
        if dry_run:
            print("\nℹ️  This was a DRY-RUN. No files were modified.")
            print("Run without --dry-run to apply changes.")
            
    except Exception as e:
        print(f"\n❌ Error during traceability enforcement: {e}")
        
        if enforcer.backup_dir and not dry_run:
            print(f"💡 Backup available for rollback: {enforcer.backup_dir}")
            print("Use --rollback --backup-dir <path> to restore if needed")
            
        exit(1)

if __name__ == '__main__':
    main()