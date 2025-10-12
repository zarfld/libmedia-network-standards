#!/usr/bin/env python3
"""
Foundation Dependencies Checker

Validates IEEE layering dependencies are satisfied:
- Foundation Layer: IEEE 802.1Q-2022, IEEE 1588-2019 
- Timing Layer: IEEE 802.1AS-2021
- Transport Layer: IEEE 1722-2016
- Control Layer: IEEE 1722.1-2021

Usage:
    python check-foundation-dependencies.py --foundation-layer="IEEE 802.1Q-2022,IEEE 1588-2019" --timing-layer="IEEE 802.1AS-2021"
"""

import argparse
import json
import os
import sys
from pathlib import Path
from typing import Dict, List

class FoundationDependencyChecker:
    """Validates IEEE foundation dependencies per layering requirements"""
    
    def __init__(self):
        self.repo_root = Path(__file__).parent.parent
        self.standards_path = self.repo_root / "lib" / "Standards"
        self.status_tracker = self.repo_root / ".github" / "ieee-standards-status-tracker.md"
        self.errors = []
        self.warnings = []
        
        # IEEE layering dependencies
        self.layer_dependencies = {
            "foundation": [],  # No dependencies
            "timing": ["foundation"], 
            "transport": ["foundation", "timing"],
            "control": ["foundation", "timing", "transport"],
            "pro_audio": ["foundation", "timing", "transport", "control"]
        }
        
    def check_dependencies(self, foundation_layer: List[str], timing_layer: List[str], 
                         transport_layer: List[str], control_layer: List[str]) -> bool:
        """Check that all dependencies are satisfied"""
        print("🏗️ Checking IEEE Foundation Dependencies...")
        
        # Parse status tracker to get current completion status
        status = self._parse_status_tracker()
        
        layers = {
            "foundation": foundation_layer,
            "timing": timing_layer, 
            "transport": transport_layer,
            "control": control_layer
        }
        
        success = True
        
        for layer_name, standards in layers.items():
            if not self._validate_layer_dependencies(layer_name, standards, status):
                success = False
                
        return success
        
    def _parse_status_tracker(self) -> Dict[str, Dict]:
        """Parse IEEE standards status tracker for completion status"""
        if not self.status_tracker.exists():
            self.errors.append(f"Status tracker not found: {self.status_tracker}")
            return {}
            
        status = {}
        
        try:
            with open(self.status_tracker, 'r', encoding='utf-8') as f:
                content = f.read()
                
            # Extract table data (simplified parsing)
            lines = content.split('\n')
            in_table = False
            
            for line in lines:
                if '| Layer ' in line and '| Standard ' in line:
                    in_table = True
                    continue
                    
                if in_table and line.startswith('|') and 'IEEE' in line:
                    parts = [part.strip() for part in line.split('|')[1:-1]]  # Remove empty first/last
                    if len(parts) >= 6:
                        layer = parts[0]
                        standard = parts[1] 
                        priority = parts[2]
                        overall_status = parts[3]
                        phase = parts[4]
                        progress = parts[5]
                        
                        status[standard] = {
                            'layer': layer,
                            'priority': priority,
                            'status': overall_status,
                            'phase': phase,
                            'progress': progress
                        }
                        
                elif in_table and not line.startswith('|'):
                    break
                    
        except Exception as e:
            self.warnings.append(f"Could not parse status tracker: {e}")
            
        return status
        
    def _validate_layer_dependencies(self, layer_name: str, standards: List[str], 
                                   status: Dict[str, Dict]) -> bool:
        """Validate that a layer's dependencies are satisfied"""
        if layer_name not in self.layer_dependencies:
            self.errors.append(f"Unknown layer: {layer_name}")
            return False
            
        required_dependencies = self.layer_dependencies[layer_name]
        
        # Check that all dependency layers are complete
        for dep_layer in required_dependencies:
            if not self._is_layer_complete(dep_layer, status):
                self.errors.append(f"Layer '{layer_name}' requires '{dep_layer}' to be complete first")
                return False
                
        # Check individual standards in this layer
        for standard in standards:
            if standard in status:
                std_status = status[standard]
                
                # Check if trying to work on higher layer before foundation complete
                if layer_name != "foundation":
                    foundation_complete = self._is_layer_complete("foundation", status)
                    if not foundation_complete and std_status['phase'] not in ['00-planning', '01-stakeholder']:
                        self.errors.append(f"Cannot work on {standard} (phase {std_status['phase']}) before foundation layer complete")
                        return False
                        
                print(f"✅ {standard}: {std_status['status']} (Phase: {std_status['phase']})")
            else:
                self.warnings.append(f"Standard {standard} not found in status tracker")
                
        return True
        
    def _is_layer_complete(self, layer_name: str, status: Dict[str, Dict]) -> bool:
        """Check if a layer is complete (all standards at Phase 09)"""
        layer_map = {
            "foundation": ["IEEE 802.1Q-2022", "IEEE 1588-2019"],
            "timing": ["IEEE 802.1AS-2021"],
            "transport": ["IEEE 1722-2016"],
            "control": ["IEEE 1722.1-2021"]
        }
        
        if layer_name not in layer_map:
            return False
            
        layer_standards = layer_map[layer_name]
        
        for standard in layer_standards:
            if standard not in status:
                return False
                
            std_status = status[standard]
            
            # Consider complete if Phase 08+ or status contains "Complete"
            phase = std_status.get('phase', '00-planning')
            overall_status = std_status.get('status', 'Not Started')
            
            if not (phase.startswith('08') or phase.startswith('09') or 'Complete' in overall_status):
                return False
                
        return True
        
    def check_implementation_order(self, status: Dict[str, Dict]) -> bool:
        """Check that implementation follows foundation-first order"""
        print("📋 Checking Implementation Order...")
        
        # Get all standards sorted by priority
        standards_by_priority = []
        for standard, info in status.items():
            try:
                priority = int(info.get('priority', '999'))
                standards_by_priority.append((priority, standard, info))
            except ValueError:
                self.warnings.append(f"Invalid priority for {standard}: {info.get('priority')}")
                
        standards_by_priority.sort(key=lambda x: x[0])
        
        # Check that higher priority standards are not blocked by lower priority
        active_phases = []
        for priority, standard, info in standards_by_priority:
            phase = info.get('phase', '00-planning')
            
            # If this is an implementation phase (05+), check that lower priorities are complete
            if phase.startswith(('05', '06', '07', '08', '09')):
                for other_priority, other_standard, other_info in standards_by_priority:
                    if other_priority < priority:  # Higher priority (lower number)
                        other_phase = other_info.get('phase', '00-planning')
                        if not other_phase.startswith(('08', '09')):
                            self.errors.append(
                                f"{standard} (priority {priority}) is in {phase} but "
                                f"{other_standard} (priority {other_priority}) is only in {other_phase}"
                            )
                            return False
                            
        return True
        
    def generate_dependency_matrix(self, status: Dict[str, Dict]) -> Dict:
        """Generate dependency matrix for visualization"""
        matrix = {
            "layers": {
                "foundation": {"standards": ["IEEE 802.1Q-2022", "IEEE 1588-2019"], "dependencies": []},
                "timing": {"standards": ["IEEE 802.1AS-2021"], "dependencies": ["foundation"]},
                "transport": {"standards": ["IEEE 1722-2016"], "dependencies": ["foundation", "timing"]}, 
                "control": {"standards": ["IEEE 1722.1-2021"], "dependencies": ["foundation", "timing", "transport"]},
                "pro_audio": {"standards": ["AVnu Milan v1.2", "AES67-2018", "AES70-2021"], 
                             "dependencies": ["foundation", "timing", "transport", "control"]}
            },
            "status": status,
            "validation_timestamp": "2024-12-10"
        }
        
        return matrix
        
    def generate_report(self, status: Dict[str, Dict]) -> Dict:
        """Generate validation report"""
        return {
            "validation_type": "foundation_dependencies",
            "validation_timestamp": "2024-12-10",
            "total_errors": len(self.errors),
            "total_warnings": len(self.warnings), 
            "errors": self.errors,
            "warnings": self.warnings,
            "status": "PASS" if len(self.errors) == 0 else "FAIL",
            "dependency_matrix": self.generate_dependency_matrix(status)
        }

def main():
    parser = argparse.ArgumentParser(description="Check IEEE Foundation Dependencies")
    parser.add_argument("--foundation-layer", required=True,
                       help="Comma-separated list of foundation standards")
    parser.add_argument("--timing-layer", required=True,
                       help="Comma-separated list of timing standards")
    parser.add_argument("--transport-layer", required=True,
                       help="Comma-separated list of transport standards") 
    parser.add_argument("--control-layer", required=True,
                       help="Comma-separated list of control standards")
    
    args = parser.parse_args()
    
    foundation_standards = [s.strip() for s in args.foundation_layer.split(',')]
    timing_standards = [s.strip() for s in args.timing_layer.split(',')]
    transport_standards = [s.strip() for s in args.transport_layer.split(',')]
    control_standards = [s.strip() for s in args.control_layer.split(',')]
    
    checker = FoundationDependencyChecker()
    
    # Parse status first
    status = checker._parse_status_tracker()
    
    # Check dependencies
    success = checker.check_dependencies(
        foundation_standards, timing_standards, 
        transport_standards, control_standards
    )
    
    # Check implementation order
    success &= checker.check_implementation_order(status)
    
    # Generate reports
    report = checker.generate_report(status)
    dependency_matrix = checker.generate_dependency_matrix(status)
    
    # Save reports
    os.makedirs("build", exist_ok=True)
    with open("build/foundation-dependency-report.json", "w") as f:
        json.dump(report, f, indent=2)
        
    with open("build/foundation-dependency-matrix.json", "w") as f:
        json.dump(dependency_matrix, f, indent=2)
        
    # Print results
    if checker.errors:
        print(f"\n❌ Foundation dependency check FAILED with {len(checker.errors)} errors:")
        for error in checker.errors:
            print(f"  - {error}")
            
    if checker.warnings:
        print(f"\n⚠️ {len(checker.warnings)} warnings:")
        for warning in checker.warnings:
            print(f"  - {warning}")
            
    if success:
        print("\n✅ Foundation dependencies validation PASSED")
    else:
        print("\n❌ Foundation dependencies validation FAILED")
        sys.exit(1)

if __name__ == "__main__":
    main()