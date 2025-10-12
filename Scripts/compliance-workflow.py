#!/usr/bin/env python3
"""
Standards-compliant development workflow integration
Integrates validation, testing, and compliance checking into developer workflow
Following XP practices and continuous integration principles
"""

import sys
import subprocess
import pathlib
import json
from datetime import datetime

class ComplianceWorkflowManager:
    """Manages standards compliance throughout development lifecycle"""
    
    def __init__(self, project_root="."):
        self.project_root = pathlib.Path(project_root)
        self.compliance_report = {
            "timestamp": datetime.now().isoformat(),
            "checks": [],
            "summary": {"passed": 0, "failed": 0, "warnings": 0}
        }
    
    def run_spec_validation(self):
        """Run specification structure validation per ISO/IEC/IEEE standards"""
        print("🔍 Running specification validation (ISO/IEC/IEEE compliance)...")
        
        # Find all spec files
        spec_patterns = ["**/*.md"]
        spec_dirs = [
            "01-stakeholder-requirements", "02-requirements", "03-architecture",
            "04-design", "05-implementation/docs", "07-verification-validation"
        ]
        
        failed_files = []
        
        for spec_dir in spec_dirs:
            dir_path = self.project_root / spec_dir
            if not dir_path.exists():
                continue
                
            for pattern in spec_patterns:
                for spec_file in dir_path.glob(pattern):
                    if self._should_validate_spec(spec_file):
                        result = self._validate_single_spec(spec_file)
                        if not result["passed"]:
                            failed_files.append(result)
        
        check_result = {
            "check": "Specification Validation",
            "standard": "ISO/IEC/IEEE 29148:2018, 42010:2011",
            "passed": len(failed_files) == 0,
            "details": failed_files if failed_files else "All specs comply with standards",
            "fixes": self._get_spec_validation_fixes() if failed_files else None
        }
        
        self.compliance_report["checks"].append(check_result)
        if check_result["passed"]:
            self.compliance_report["summary"]["passed"] += 1
        else:
            self.compliance_report["summary"]["failed"] += 1
            
        return check_result["passed"]
    
    def run_tdd_validation(self):
        """Validate Test-Driven Development compliance per XP practices"""
        print("🧪 Validating TDD compliance (XP practices)...")
        
        # Check for test files
        test_patterns = ["test_*.cpp", "test_*.c", "*_test.cpp", "*_test.c"]
        impl_patterns = ["*.cpp", "*.c"]
        
        test_coverage = self._analyze_test_coverage(test_patterns, impl_patterns)
        
        # TDD compliance criteria
        min_coverage_percent = 80
        has_unit_tests = test_coverage["unit_tests"] > 0
        has_integration_tests = test_coverage["integration_tests"] > 0
        coverage_adequate = test_coverage["coverage_percent"] >= min_coverage_percent
        
        check_result = {
            "check": "TDD Compliance",
            "standard": "XP Test-Driven Development",
            "passed": has_unit_tests and coverage_adequate,
            "details": {
                "unit_tests": test_coverage["unit_tests"],
                "integration_tests": test_coverage["integration_tests"],
                "coverage_percent": test_coverage["coverage_percent"],
                "missing_tests": test_coverage["missing_tests"]
            },
            "fixes": self._get_tdd_fixes(test_coverage) if not (has_unit_tests and coverage_adequate) else None
        }
        
        self.compliance_report["checks"].append(check_result)
        if check_result["passed"]:
            self.compliance_report["summary"]["passed"] += 1
        else:
            self.compliance_report["summary"]["failed"] += 1
            
        return check_result["passed"]
    
    def run_architecture_compliance(self):
        """Validate architecture compliance per IEEE 1016-2009"""
        print("🏗️ Validating architecture compliance (IEEE 1016-2009)...")
        
        # Check for required architecture documentation
        arch_requirements = [
            "03-architecture/views/context-view.md",
            "03-architecture/views/logical-view.md", 
            "03-architecture/views/process-view.md",
            "03-architecture/decisions"
        ]
        
        missing_docs = []
        for req in arch_requirements:
            path = self.project_root / req
            if not path.exists():
                missing_docs.append(req)
        
        # Check ADR format compliance
        adr_compliance = self._check_adr_compliance()
        
        check_result = {
            "check": "Architecture Compliance",
            "standard": "IEEE 1016-2009",
            "passed": len(missing_docs) == 0 and adr_compliance["compliant"],
            "details": {
                "missing_documentation": missing_docs,
                "adr_compliance": adr_compliance
            },
            "fixes": self._get_architecture_fixes(missing_docs, adr_compliance)
        }
        
        self.compliance_report["checks"].append(check_result)
        if check_result["passed"]:
            self.compliance_report["summary"]["passed"] += 1
        else:
            self.compliance_report["summary"]["failed"] += 1
            
        return check_result["passed"]
    
    def run_ci_integration_check(self):
        """Check CI integration per XP continuous integration practices"""
        print("🔄 Validating CI integration (XP Continuous Integration)...")
        
        ci_files = [
            ".github/workflows/ci-standards-compliance.yml",
            "Scripts/validate-spec-structure.py",
            "Scripts/pre-commit-standards-check.py"
        ]
        
        missing_ci = []
        for ci_file in ci_files:
            if not (self.project_root / ci_file).exists():
                missing_ci.append(ci_file)
        
        # Check if pre-commit is configured
        precommit_configured = (self.project_root / ".pre-commit-config.yaml").exists()
        
        check_result = {
            "check": "CI Integration",
            "standard": "XP Continuous Integration",
            "passed": len(missing_ci) == 0 and precommit_configured,
            "details": {
                "missing_ci_files": missing_ci,
                "precommit_configured": precommit_configured
            },
            "fixes": self._get_ci_integration_fixes(missing_ci, precommit_configured)
        }
        
        self.compliance_report["checks"].append(check_result)
        if check_result["passed"]:
            self.compliance_report["summary"]["passed"] += 1
        else:
            self.compliance_report["summary"]["failed"] += 1
            
        return check_result["passed"]
    
    def generate_compliance_report(self, output_path=None):
        """Generate comprehensive compliance report"""
        if output_path is None:
            output_path = self.project_root / "compliance-report.json"
        
        with open(output_path, 'w') as f:
            json.dump(self.compliance_report, f, indent=2)
        
        # Generate human-readable summary
        summary_path = output_path.with_suffix('.md')
        self._generate_summary_report(summary_path)
        
        return output_path, summary_path
    
    def _should_validate_spec(self, spec_file):
        """Check if file should be validated as a specification"""
        try:
            content = spec_file.read_text(encoding='utf-8')
            return content.strip().startswith('---')  # Has YAML front matter
        except:
            return False
    
    def _validate_single_spec(self, spec_file):
        """Validate a single specification file"""
        try:
            result = subprocess.run([
                sys.executable, "Scripts/validate-spec-structure.py", str(spec_file)
            ], capture_output=True, text=True, cwd=self.project_root)
            
            return {
                "file": str(spec_file),
                "passed": result.returncode == 0,
                "output": result.stdout + result.stderr
            }
        except Exception as e:
            return {
                "file": str(spec_file),
                "passed": False,
                "output": f"Validation error: {e}"
            }
    
    def _get_spec_validation_fixes(self):
        """Get fixes for spec validation issues"""
        return [
            "Run: py Scripts/validate-spec-structure.py <file> for detailed errors",
            "Check YAML front matter matches schema requirements",
            "Ensure specType matches allowed values: requirements, architecture, design",
            "Verify standard references are correct (ISO/IEC/IEEE format)",
            "Add missing required fields: version, date, traceability"
        ]
    
    def _analyze_test_coverage(self, test_patterns, impl_patterns):
        """Analyze test coverage for TDD compliance"""
        
        # Find implementation files
        impl_files = []
        for pattern in impl_patterns:
            impl_files.extend(self.project_root.glob(f"05-implementation/src/**/{pattern}"))
        
        # Find test files  
        test_files = []
        for pattern in test_patterns:
            test_files.extend(self.project_root.glob(f"05-implementation/tests/**/{pattern}"))
            test_files.extend(self.project_root.glob(f"07-verification-validation/**/{pattern}"))
        
        # Simple coverage analysis
        unit_tests = len([f for f in test_files if 'unit' in str(f) or 'test_' in f.name])
        integration_tests = len([f for f in test_files if 'integration' in str(f) or 'integration' in f.parent.name])
        
        # Calculate rough coverage percentage
        if len(impl_files) > 0:
            coverage_percent = min(100, (len(test_files) / len(impl_files)) * 100)
        else:
            coverage_percent = 100 if len(test_files) > 0 else 0
        
        # Find implementation files without corresponding tests
        missing_tests = []
        for impl_file in impl_files:
            test_exists = any(
                impl_file.stem in test_file.name for test_file in test_files
            )
            if not test_exists:
                missing_tests.append(str(impl_file))
        
        return {
            "unit_tests": unit_tests,
            "integration_tests": integration_tests,
            "coverage_percent": round(coverage_percent, 1),
            "missing_tests": missing_tests
        }
    
    def _get_tdd_fixes(self, test_coverage):
        """Get fixes for TDD compliance issues"""
        fixes = []
        
        if test_coverage["unit_tests"] == 0:
            fixes.append("Create unit tests following TDD Red-Green-Refactor cycle")
            fixes.append("Use: py Scripts/generate-compliant-spec.py requirements 'Unit Test Plan'")
        
        if test_coverage["coverage_percent"] < 80:
            fixes.append(f"Increase test coverage from {test_coverage['coverage_percent']}% to 80%")
            fixes.append("Add tests for files in missing_tests list")
        
        if test_coverage["missing_tests"]:
            fixes.append("Create tests for implementation files without test coverage")
            for missing in test_coverage["missing_tests"][:3]:  # Show first 3
                test_name = pathlib.Path(missing).stem
                fixes.append(f"  • Create test_{test_name}.cpp")
        
        return fixes
    
    def _check_adr_compliance(self):
        """Check Architecture Decision Record compliance"""
        adr_dir = self.project_root / "03-architecture" / "decisions"
        
        if not adr_dir.exists():
            return {"compliant": False, "missing_adr_directory": True}
        
        adr_files = list(adr_dir.glob("*.md"))
        compliant_adrs = 0
        
        for adr_file in adr_files:
            if self._validate_adr_format(adr_file):
                compliant_adrs += 1
        
        return {
            "compliant": len(adr_files) > 0 and compliant_adrs == len(adr_files),
            "total_adrs": len(adr_files),
            "compliant_adrs": compliant_adrs,
            "missing_adr_directory": False
        }
    
    def _validate_adr_format(self, adr_file):
        """Validate ADR format per IEEE 42010:2011"""
        try:
            content = adr_file.read_text(encoding='utf-8')
            required_sections = [
                "## Context", "## Decision", "## Status", 
                "## Rationale", "## Considered Alternatives"
            ]
            return all(section in content for section in required_sections)
        except:
            return False
    
    def _get_architecture_fixes(self, missing_docs, adr_compliance):
        """Get fixes for architecture compliance issues"""
        fixes = []
        
        if missing_docs:
            fixes.append("Create missing architecture documentation per IEEE 1016-2009:")
            for doc in missing_docs:
                fixes.append(f"  • {doc}")
        
        if not adr_compliance["compliant"]:
            if adr_compliance.get("missing_adr_directory"):
                fixes.append("Create 03-architecture/decisions directory")
                fixes.append("Use: py Scripts/generate-compliant-spec.py architecture 'Architecture Decision'")
            else:
                fixes.append(f"Fix ADR format compliance: {adr_compliance['compliant_adrs']}/{adr_compliance['total_adrs']} compliant")
                fixes.append("Ensure ADRs include required sections: Context, Decision, Status, Rationale, Alternatives")
        
        return fixes
    
    def _get_ci_integration_fixes(self, missing_ci, precommit_configured):
        """Get fixes for CI integration issues"""
        fixes = []
        
        if missing_ci:
            fixes.append("Set up missing CI integration files:")
            for ci_file in missing_ci:
                fixes.append(f"  • {ci_file}")
        
        if not precommit_configured:
            fixes.append("Configure pre-commit hooks:")
            fixes.append("  • Create .pre-commit-config.yaml")
            fixes.append("  • Run: pre-commit install")
        
        fixes.append("Integrate with development workflow:")
        fixes.append("  • Configure IDE to run validation on save")
        fixes.append("  • Set up commit hooks for automatic compliance checking")
        
        return fixes
    
    def _generate_summary_report(self, summary_path):
        """Generate human-readable compliance summary"""
        summary = self.compliance_report["summary"]
        
        content = f"""# Standards Compliance Report

Generated: {self.compliance_report['timestamp']}

## Summary
- ✅ **Passed**: {summary['passed']} checks
- ❌ **Failed**: {summary['failed']} checks  
- ⚠️ **Warnings**: {summary['warnings']} checks

## Compliance Status
"""
        
        for check in self.compliance_report["checks"]:
            status = "✅" if check["passed"] else "❌"
            content += f"\n### {status} {check['check']}\n"
            content += f"**Standard**: {check['standard']}\n\n"
            
            if isinstance(check["details"], dict):
                for key, value in check["details"].items():
                    content += f"- **{key.replace('_', ' ').title()}**: {value}\n"
            else:
                content += f"**Details**: {check['details']}\n"
            
            if check.get("fixes"):
                content += f"\n**Recommended Fixes**:\n"
                for fix in check["fixes"]:
                    content += f"- {fix}\n"
            
            content += "\n"
        
        content += f"""
## Next Steps

1. **Address Failed Checks**: Focus on failed compliance checks first
2. **Implement Fixes**: Follow recommended fixes for each failed check  
3. **Validate Changes**: Re-run compliance check after implementing fixes
4. **Update Workflow**: Integrate compliance checking into development workflow

## ISO/IEC/IEEE Standards Compliance

This report validates compliance with:
- **ISO/IEC/IEEE 12207:2017**: Software life cycle processes
- **ISO/IEC/IEEE 29148:2018**: Requirements engineering  
- **IEEE 1016-2009**: Software design descriptions
- **ISO/IEC/IEEE 42010:2011**: Architecture description
- **IEEE 1012-2016**: Verification and validation

## XP Practices Integration

Validates integration of Extreme Programming practices:
- Test-Driven Development (TDD)
- Continuous Integration
- Simple Design principles
- User Stories and Planning Game
"""
        
        summary_path.write_text(content, encoding='utf-8')

def main():
    """Main entry point for compliance workflow"""
    import argparse
    
    parser = argparse.ArgumentParser(description='Standards compliance workflow manager')
    parser.add_argument('--project-root', default='.', 
                       help='Project root directory')
    parser.add_argument('--output', default='compliance-report',
                       help='Output file base name (without extension)')
    parser.add_argument('--check', choices=['spec', 'tdd', 'architecture', 'ci', 'all'],
                       default='all', help='Type of check to run')
    
    args = parser.parse_args()
    
    workflow = ComplianceWorkflowManager(args.project_root)
    
    print("🎯 Standards Compliance Workflow")
    print("=" * 50)
    
    all_passed = True
    
    if args.check in ['spec', 'all']:
        passed = workflow.run_spec_validation()
        all_passed = all_passed and passed
    
    if args.check in ['tdd', 'all']:
        passed = workflow.run_tdd_validation()
        all_passed = all_passed and passed
        
    if args.check in ['architecture', 'all']:
        passed = workflow.run_architecture_compliance()
        all_passed = all_passed and passed
        
    if args.check in ['ci', 'all']:
        passed = workflow.run_ci_integration_check()
        all_passed = all_passed and passed
    
    # Generate reports
    report_json, report_md = workflow.generate_compliance_report(
        pathlib.Path(args.output + '.json')
    )
    
    print(f"\n📊 Compliance Report Generated:")
    print(f"   📄 JSON: {report_json}")  
    print(f"   📋 Summary: {report_md}")
    
    if all_passed:
        print(f"\n✅ All compliance checks passed!")
        sys.exit(0)
    else:
        print(f"\n❌ Some compliance checks failed. See report for details.")
        sys.exit(1)

if __name__ == '__main__':
    main()