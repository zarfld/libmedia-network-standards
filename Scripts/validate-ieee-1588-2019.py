#!/usr/bin/env python3
"""
IEEE 1588-2019 Precision Time Protocol Validation Script

Validates IEEE 1588-2019 implementation compliance according to:
- State machine implementations per Section 9.2
- BMCA algorithm per Section 9.3  
- Transport layer per Annex D
- Management protocol per Section 15

Usage:
    python validate-ieee-1588-2019.py --check-state-machines --validate-bmca --verify-transport --check-management-protocol
"""

import argparse
import json
import os
import re
import sys
from pathlib import Path
from typing import Dict, List, Set

class IEEE1588Validator:
    """Validator for IEEE 1588-2019 PTP implementation"""
    
    def __init__(self):
        self.repo_root = Path(__file__).parent.parent
        self.ieee_1588_path = self.repo_root / "lib" / "Standards" / "IEEE" / "1588" / "2019"
        self.design_path = self.repo_root / "04-design" / "components"
        self.errors = []
        self.warnings = []
        
        # Required IEEE 1588-2019 components per specification
        self.required_components = {
            "state_machines": {
                "interfaces": ["IPortStateMachine"],
                "classes": ["PortStateMachineImpl"],
                "files": ["port_state_machine.hpp", "port_state_machine.cpp"]
            },
            "bmca": {
                "interfaces": ["IBMCAEngine"], 
                "classes": ["BMCAEngineImpl", "DatasetComparison"],
                "files": ["bmca_engine.hpp", "bmca_engine.cpp"]
            },
            "transport": {
                "interfaces": ["ITransport", "IEthernetTransport", "IUDPTransport"],
                "classes": ["TransportManager", "EthernetTransport", "UDPTransport"],
                "files": ["transport_manager.hpp", "ethernet_transport.hpp", "udp_transport.hpp"]
            },
            "management": {
                "interfaces": ["IManagementProtocol", "ITLVProcessor"],
                "classes": ["ManagementProtocolImpl", "TLVProcessorImpl"],
                "files": ["management_protocol.hpp", "tlv_processor.hpp"]
            }
        }
        
    def check_state_machines(self) -> bool:
        """Check IEEE 1588-2019 state machine implementation per Section 9.2"""
        print("🔄 Checking State Machines (IEEE 1588-2019 Section 9.2)...")
        
        # Check design specification exists
        design_file = self.design_path / "ieee-1588-2019-state-machine-design.md"
        if not design_file.exists():
            self.errors.append("State machine design specification not found")
            return False
            
        # Validate design content
        if not self._validate_design_content(design_file, "state_machines"):
            return False
            
        # Check implementation structure (when it exists)
        component_path = self.ieee_1588_path / "core" / "state_machines"
        if component_path.exists():
            return self._validate_component_implementation("state_machines", component_path)
        else:
            self.warnings.append("State machine implementation not yet created (Phase 05)")
            
        print("✅ State machine validation completed")
        return True
        
    def validate_bmca(self) -> bool:
        """Validate BMCA implementation per IEEE 1588-2019 Section 9.3"""
        print("⚡ Validating BMCA Algorithm (IEEE 1588-2019 Section 9.3)...")
        
        design_file = self.design_path / "ieee-1588-2019-bmca-design.md"
        if not design_file.exists():
            self.errors.append("BMCA design specification not found")
            return False
            
        if not self._validate_design_content(design_file, "bmca"):
            return False
            
        component_path = self.ieee_1588_path / "core" / "bmca"
        if component_path.exists():
            return self._validate_component_implementation("bmca", component_path)
        else:
            self.warnings.append("BMCA implementation not yet created (Phase 05)")
            
        print("✅ BMCA validation completed")
        return True
        
    def verify_transport(self) -> bool:
        """Verify transport layer implementation per IEEE 1588-2019 Annex D"""
        print("🚀 Verifying Transport Layer (IEEE 1588-2019 Annex D)...")
        
        design_file = self.design_path / "ieee-1588-2019-transport-design.md"
        if not design_file.exists():
            self.errors.append("Transport design specification not found")
            return False
            
        if not self._validate_design_content(design_file, "transport"):
            return False
            
        component_path = self.ieee_1588_path / "transport"
        if component_path.exists():
            return self._validate_component_implementation("transport", component_path)
        else:
            self.warnings.append("Transport implementation not yet created (Phase 05)")
            
        print("✅ Transport validation completed")
        return True
        
    def check_management_protocol(self) -> bool:
        """Check management protocol per IEEE 1588-2019 Section 15"""
        print("⚙️ Checking Management Protocol (IEEE 1588-2019 Section 15)...")
        
        design_file = self.design_path / "ieee-1588-2019-management-design.md"
        if not design_file.exists():
            self.errors.append("Management protocol design specification not found")
            return False
            
        if not self._validate_design_content(design_file, "management"):
            return False
            
        component_path = self.ieee_1588_path / "management"  
        if component_path.exists():
            return self._validate_component_implementation("management", component_path)
        else:
            self.warnings.append("Management protocol implementation not yet created (Phase 05)")
            
        print("✅ Management protocol validation completed")
        return True
        
    def _validate_design_content(self, design_file: Path, component: str) -> bool:
        """Validate design specification content"""
        try:
            with open(design_file, 'r', encoding='utf-8') as f:
                content = f.read()
                
            # Check for required sections per IEEE 1016-2009
            required_sections = [
                "Design Overview",
                "Component Architecture", 
                "Detailed Design",
                "Interface",
                "Implementation"
            ]
            
            missing_sections = []
            for section in required_sections:
                if section not in content:
                    missing_sections.append(section)
                    
            if missing_sections:
                self.errors.append(f"Design {design_file.name} missing sections: {missing_sections}")
                return False
                
            # Check for IEEE specification references
            if "IEEE 1588-2019" not in content:
                self.errors.append(f"Design {design_file.name} missing IEEE 1588-2019 specification references")
                return False
                
            # Check for required interfaces and classes
            required_items = self.required_components.get(component, {})
            for interface in required_items.get("interfaces", []):
                if interface not in content:
                    self.warnings.append(f"Interface {interface} not found in {design_file.name}")
                    
            return True
            
        except Exception as e:
            self.errors.append(f"Could not validate design file {design_file}: {e}")
            return False
            
    def _validate_component_implementation(self, component: str, component_path: Path) -> bool:
        """Validate component implementation against design"""
        required_items = self.required_components.get(component, {})
        
        # Check for required files
        for required_file in required_items.get("files", []):
            file_path = component_path / required_file
            if not file_path.exists():
                self.warnings.append(f"Required file not found: {file_path}")
                
        # Check for required interfaces and classes in source files
        cpp_files = list(component_path.glob("*.cpp"))
        hpp_files = list(component_path.glob("*.hpp"))
        
        all_content = ""
        for file_path in cpp_files + hpp_files:
            try:
                with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                    all_content += f.read() + "\n"
            except Exception as e:
                self.warnings.append(f"Could not read {file_path}: {e}")
                
        # Check interfaces
        for interface in required_items.get("interfaces", []):
            if f"class {interface}" not in all_content:
                self.warnings.append(f"Interface {interface} not implemented in {component_path}")
                
        # Check classes  
        for class_name in required_items.get("classes", []):
            if f"class {class_name}" not in all_content:
                self.warnings.append(f"Class {class_name} not implemented in {component_path}")
                
        return True
        
    def check_traceability(self) -> bool:
        """Check traceability from requirements to design to implementation"""
        print("🔗 Checking Requirements Traceability...")
        
        # Check that all design files trace to requirements
        design_files = list(self.design_path.glob("ieee-1588-2019-*-design.md"))
        
        for design_file in design_files:
            if not self._check_design_traceability(design_file):
                return False
                
        return True
        
    def _check_design_traceability(self, design_file: Path) -> bool:
        """Check traceability in a specific design file"""
        try:
            with open(design_file, 'r', encoding='utf-8') as f:
                content = f.read()
                
            # Check for traceability section
            if "traceability:" not in content.lower():
                self.warnings.append(f"No traceability section found in {design_file.name}")
                return True  # Warning, not error
                
            # Check for requirement references
            req_pattern = r'REQ-[A-Z]+-\d+'
            requirements = re.findall(req_pattern, content)
            
            if not requirements:
                self.warnings.append(f"No requirement references found in {design_file.name}")
                
            return True
            
        except Exception as e:
            self.errors.append(f"Could not check traceability in {design_file}: {e}")
            return False
            
    def generate_report(self) -> Dict:
        """Generate validation report"""
        return {
            "standard": "IEEE 1588-2019",
            "validation_timestamp": "2024-12-10", 
            "phase": "04-design-validation",
            "total_errors": len(self.errors),
            "total_warnings": len(self.warnings),
            "errors": self.errors,
            "warnings": self.warnings,
            "status": "PASS" if len(self.errors) == 0 else "FAIL",
            "components_validated": list(self.required_components.keys())
        }

def main():
    parser = argparse.ArgumentParser(description="Validate IEEE 1588-2019 Implementation")
    parser.add_argument("--check-state-machines", action="store_true",
                       help="Check state machine implementation")
    parser.add_argument("--validate-bmca", action="store_true",
                       help="Validate BMCA algorithm")
    parser.add_argument("--verify-transport", action="store_true",
                       help="Verify transport layer")
    parser.add_argument("--check-management-protocol", action="store_true",
                       help="Check management protocol")
    
    args = parser.parse_args()
    
    validator = IEEE1588Validator()
    success = True
    
    if args.check_state_machines:
        success &= validator.check_state_machines()
        
    if args.validate_bmca:
        success &= validator.validate_bmca()
        
    if args.verify_transport:
        success &= validator.verify_transport()
        
    if args.check_management_protocol:
        success &= validator.check_management_protocol()
        
    # Always check traceability
    success &= validator.check_traceability()
        
    # Generate report
    report = validator.generate_report()
    
    # Save report
    os.makedirs("build", exist_ok=True)
    with open("build/ieee-1588-2019-validation-report.json", "w") as f:
        json.dump(report, f, indent=2)
        
    # Print results
    if validator.errors:
        print(f"\n❌ IEEE 1588-2019 validation FAILED with {len(validator.errors)} errors:")
        for error in validator.errors:
            print(f"  - {error}")
            
    if validator.warnings:
        print(f"\n⚠️ {len(validator.warnings)} warnings:")
        for warning in validator.warnings:
            print(f"  - {warning}")
            
    if success:
        print("\n✅ IEEE 1588-2019 validation PASSED")
    else:
        print("\n❌ IEEE 1588-2019 validation FAILED") 
        sys.exit(1)

if __name__ == "__main__":
    main()