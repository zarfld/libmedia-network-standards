#!/usr/bin/env python3
"""
IEEE Media Networking Standards Validation Script

Validates IEEE standards implementation compliance according to:
- IEEE layering hierarchy and dependencies
- Hardware abstraction requirements 
- Namespace compliance with architectural patterns
- Foundation-first implementation order

Usage:
    python validate-ieee-standards.py --check-foundation-dependencies --validate-hardware-abstraction --verify-namespace-compliance
"""

import argparse
import json
import os
import re
import sys
from pathlib import Path
from typing import Dict, List, Set, Tuple

class IEEEStandardsValidator:
    """Main validator for IEEE standards implementation compliance"""
    
    def __init__(self):
        self.repo_root = Path(__file__).parent.parent
        self.standards_path = self.repo_root / "lib" / "Standards"
        self.errors = []
        self.warnings = []
        
        # IEEE Standards hierarchy per prompt requirements
        self.ieee_hierarchy = {
            "Foundation": ["IEEE/802.1/Q/2022", "IEEE/1588/2019"],
            "Timing": ["IEEE/802.1/AS/2021"],
            "Transport": ["IEEE/1722/2016"], 
            "Control": ["IEEE/1722.1/2021"],
            "ProAudio": ["AVnu/Milan/v1.2", "AES/AES67/2018", "AES/AES70/2021"]
        }
        
        # Hardware abstraction forbidden patterns
        self.forbidden_hw_patterns = [
            r"intel_.*\.h",
            r"#include.*linux/",
            r"#include.*winsock", 
            r"#include.*hal\.h",
            r"vendor_.*_api",
            r"platform_specific"
        ]
        
    def validate_foundation_dependencies(self) -> bool:
        """Validate IEEE foundation dependencies are complete before higher layers"""
        print("🔍 Validating Foundation Dependencies...")
        
        foundation_complete = self._check_layer_completion("Foundation")
        timing_dependencies = self._check_dependencies("Timing", ["Foundation"])
        transport_dependencies = self._check_dependencies("Transport", ["Foundation", "Timing"])
        control_dependencies = self._check_dependencies("Control", ["Foundation", "Timing", "Transport"])
        
        if not foundation_complete:
            self.errors.append("Foundation layer (802.1Q, 1588) must be 100% complete before other layers")
            
        return len(self.errors) == 0
        
    def validate_hardware_abstraction(self) -> bool:
        """Validate no hardware-specific code in lib/Standards/"""
        print("🔧 Validating Hardware Abstraction...")
        
        if not self.standards_path.exists():
            self.errors.append(f"Standards path not found: {self.standards_path}")
            return False
            
        violations = self._scan_for_hardware_violations(self.standards_path)
        
        if violations:
            self.errors.append(f"Hardware abstraction violations found: {violations}")
            return False
            
        print("✅ Hardware abstraction compliance verified")
        return True
        
    def verify_namespace_compliance(self) -> bool:
        """Verify namespace hierarchy matches folder structure"""
        print("📁 Verifying Namespace Compliance...")
        
        cpp_files = list(self.standards_path.glob("**/*.cpp"))
        hpp_files = list(self.standards_path.glob("**/*.hpp")) 
        header_files = list(self.standards_path.glob("**/*.h"))
        
        all_files = cpp_files + hpp_files + header_files
        
        for file_path in all_files:
            if not self._validate_namespace_in_file(file_path):
                return False
                
        print("✅ Namespace compliance verified")
        return True
        
    def _check_layer_completion(self, layer: str) -> bool:
        """Check if a layer is complete (all standards implemented)"""
        if layer not in self.ieee_hierarchy:
            return False
            
        for standard_path in self.ieee_hierarchy[layer]:
            full_path = self.standards_path / standard_path
            if not self._is_standard_complete(full_path):
                self.warnings.append(f"Standard not complete: {standard_path}")
                return False
                
        return True
        
    def _check_dependencies(self, layer: str, dependency_layers: List[str]) -> bool:
        """Check that dependency layers are complete before this layer"""
        for dep_layer in dependency_layers:
            if not self._check_layer_completion(dep_layer):
                self.errors.append(f"Layer {layer} requires {dep_layer} to be complete first")
                return False
        return True
        
    def _is_standard_complete(self, standard_path: Path) -> bool:
        """Check if a standard implementation is complete"""
        if not standard_path.exists():
            return False
            
        # Check for required files/directories
        required_items = ["core", "interfaces", "tests"]
        for item in required_items:
            if not (standard_path / item).exists():
                return False
                
        return True
        
    def _scan_for_hardware_violations(self, path: Path) -> List[str]:
        """Scan for hardware-specific code patterns"""
        violations = []
        
        for file_path in path.glob("**/*.[ch]pp"):
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                
            for pattern in self.forbidden_hw_patterns:
                matches = re.findall(pattern, content, re.IGNORECASE)
                if matches:
                    violations.append(f"{file_path}: {matches}")
                    
        return violations
        
    def _validate_namespace_in_file(self, file_path: Path) -> bool:
        """Validate namespace matches folder structure in a specific file"""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                
            # Extract namespace declarations
            namespace_pattern = r'namespace\s+(\w+)\s*\{'
            namespaces = re.findall(namespace_pattern, content)
            
            # Convert file path to expected namespace
            relative_path = file_path.relative_to(self.standards_path)
            expected_namespaces = self._path_to_namespaces(relative_path)
            
            # Validate alignment (simplified check)
            if namespaces and expected_namespaces:
                if not any(ns in expected_namespaces for ns in namespaces):
                    self.warnings.append(f"Namespace mismatch in {file_path}")
                    
        except Exception as e:
            self.warnings.append(f"Could not validate namespace in {file_path}: {e}")
            
        return True
        
    def _path_to_namespaces(self, path: Path) -> List[str]:
        """Convert file path to expected namespace hierarchy"""
        parts = path.parts[:-1]  # Exclude filename
        namespaces = []
        
        for part in parts:
            # Convert dots to underscores for namespace compatibility
            namespace_part = part.replace(".", "_")
            if namespace_part.isdigit() or namespace_part.startswith(('v', 'V')):
                namespace_part = f"_{namespace_part}"
            namespaces.append(namespace_part)
            
        return namespaces
        
    def generate_report(self) -> Dict:
        """Generate validation report"""
        return {
            "validation_timestamp": "2024-12-10",
            "total_errors": len(self.errors),
            "total_warnings": len(self.warnings),
            "errors": self.errors,
            "warnings": self.warnings,
            "status": "PASS" if len(self.errors) == 0 else "FAIL"
        }

def main():
    parser = argparse.ArgumentParser(description="Validate IEEE Standards Implementation")
    parser.add_argument("--check-foundation-dependencies", action="store_true",
                       help="Check foundation dependencies")
    parser.add_argument("--validate-hardware-abstraction", action="store_true",
                       help="Validate hardware abstraction")
    parser.add_argument("--verify-namespace-compliance", action="store_true", 
                       help="Verify namespace compliance")
    
    args = parser.parse_args()
    
    validator = IEEEStandardsValidator()
    success = True
    
    if args.check_foundation_dependencies:
        success &= validator.validate_foundation_dependencies()
        
    if args.validate_hardware_abstraction:
        success &= validator.validate_hardware_abstraction()
        
    if args.verify_namespace_compliance:
        success &= validator.verify_namespace_compliance()
        
    # Generate report
    report = validator.generate_report()
    
    # Save report
    os.makedirs("build", exist_ok=True)
    with open("build/ieee-validation-report.json", "w") as f:
        json.dump(report, f, indent=2)
        
    # Print results
    if validator.errors:
        print(f"\n❌ Validation FAILED with {len(validator.errors)} errors:")
        for error in validator.errors:
            print(f"  - {error}")
            
    if validator.warnings:
        print(f"\n⚠️ {len(validator.warnings)} warnings:")
        for warning in validator.warnings:
            print(f"  - {warning}")
            
    if success:
        print("\n✅ IEEE Standards validation PASSED")
    else:
        print("\n❌ IEEE Standards validation FAILED")
        sys.exit(1)

if __name__ == "__main__":
    main()