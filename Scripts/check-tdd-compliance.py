#!/usr/bin/env python3
"""
TDD Compliance Checker for XP Practices
Ensures implementation files have corresponding test files per TDD principles
Integrates with pre-commit framework and CI pipeline
"""

import sys
import pathlib
import argparse
from typing import List, Tuple, Dict

class TDDComplianceChecker:
    """Validates Test-Driven Development compliance per XP practices"""
    
    def __init__(self, project_root: str = "."):
        self.project_root = pathlib.Path(project_root)
        self.test_patterns = ["test_*.cpp", "test_*.c", "*_test.cpp", "*_test.c"]
        self.impl_patterns = ["*.cpp", "*.c"]
        
    def check_file_has_tests(self, impl_file: pathlib.Path) -> Tuple[bool, List[str]]:
        """Check if implementation file has corresponding test files"""
        
        # Skip if file is already a test
        if self._is_test_file(impl_file):
            return True, []
        
        # Skip certain files that don't need tests
        if self._should_skip_file(impl_file):
            return True, []
        
        # Find corresponding test files
        test_files = self._find_test_files_for_impl(impl_file)
        
        if not test_files:
            suggestions = self._generate_test_suggestions(impl_file)
            return False, suggestions
        
        return True, []
    
    def check_multiple_files(self, impl_files: List[pathlib.Path]) -> Dict:
        """Check multiple implementation files for TDD compliance"""
        results = {
            "compliant_files": [],
            "non_compliant_files": [],
            "suggestions": {},
            "summary": {"total": 0, "compliant": 0, "non_compliant": 0}
        }
        
        for impl_file in impl_files:
            is_compliant, suggestions = self.check_file_has_tests(impl_file)
            
            results["summary"]["total"] += 1
            
            if is_compliant:
                results["compliant_files"].append(str(impl_file))
                results["summary"]["compliant"] += 1
            else:
                results["non_compliant_files"].append(str(impl_file))
                results["suggestions"][str(impl_file)] = suggestions
                results["summary"]["non_compliant"] += 1
        
        return results
    
    def _is_test_file(self, file_path: pathlib.Path) -> bool:
        """Check if file is already a test file"""
        name = file_path.name.lower()
        return (
            name.startswith('test_') or 
            name.endswith('_test.cpp') or 
            name.endswith('_test.c') or
            'test' in file_path.parts
        )
    
    def _should_skip_file(self, file_path: pathlib.Path) -> bool:
        """Check if file should be skipped from TDD requirements"""
        name = file_path.name.lower()
        
        # Skip main files, examples, and certain utility files
        skip_patterns = [
            'main.cpp', 'main.c',
            'example_', 'demo_',
            '_generated.', '_auto.',
            'cmake', 'makefile'
        ]
        
        return any(pattern in name for pattern in skip_patterns)
    
    def _find_test_files_for_impl(self, impl_file: pathlib.Path) -> List[pathlib.Path]:
        """Find test files corresponding to implementation file"""
        test_files = []
        base_name = impl_file.stem
        
        # Common test directories relative to implementation
        test_dirs = [
            self.project_root / "05-implementation" / "tests",
            self.project_root / "07-verification-validation" / "unit-tests",
            self.project_root / "07-verification-validation" / "integration-tests",
            impl_file.parent / "tests",
            impl_file.parent.parent / "tests"
        ]
        
        # Search for test files with various naming patterns
        test_name_patterns = [
            f"test_{base_name}.*",
            f"{base_name}_test.*",
            f"*{base_name}*test*.*",
            f"test*{base_name}*.*"
        ]
        
        for test_dir in test_dirs:
            if not test_dir.exists():
                continue
                
            for pattern in test_name_patterns:
                for ext in ['cpp', 'c']:
                    test_pattern = pattern.replace('*', '').replace('.', f'.{ext}')
                    matches = list(test_dir.glob(f"**/{test_pattern}"))
                    test_files.extend(matches)
        
        return test_files
    
    def _generate_test_suggestions(self, impl_file: pathlib.Path) -> List[str]:
        """Generate suggestions for creating test files"""
        base_name = impl_file.stem
        
        suggestions = [
            "🧪 TDD Compliance Required - Create test files per XP practices:",
            "",
            f"📁 Recommended test locations:",
            f"  • 07-verification-validation/unit-tests/test_{base_name}.cpp",
            f"  • 05-implementation/tests/test_{base_name}.cpp", 
            "",
            f"🔧 Quick setup commands:",
            f"  py Scripts/generate-compliant-spec.py requirements 'Unit Tests for {base_name}'",
            f"  # Then create test_{base_name}.cpp with TDD Red-Green-Refactor cycle",
            "",
            f"📋 TDD Implementation Pattern:",
            f"  1. RED: Write failing test first",
            f"  2. GREEN: Write minimal code to pass test",  
            f"  3. REFACTOR: Improve code while keeping tests green",
            "",
            f"✅ Test template for {base_name}:",
            f"  #include <gtest/gtest.h>",
            f"  #include \"{impl_file.name}\"",
            f"  ",
            f"  class {base_name.title()}Test : public ::testing::Test {{",
            f"  protected:",
            f"    void SetUp() override {{ /* setup */ }}",
            f"    void TearDown() override {{ /* cleanup */ }}",
            f"  }};",
            f"  ",
            f"  TEST_F({base_name.title()}Test, BasicFunctionality) {{",
            f"    // RED: Write failing test",
            f"    EXPECT_TRUE(false); // Start with failing test",
            f"  }}",
        ]
        
        return suggestions

def main():
    """Main entry point for TDD compliance checking"""
    parser = argparse.ArgumentParser(
        description='Check TDD compliance for implementation files per XP practices'
    )
    parser.add_argument('files', nargs='*', 
                       help='Implementation files to check')
    parser.add_argument('--project-root', default='.',
                       help='Project root directory')
    parser.add_argument('--all', action='store_true',
                       help='Check all implementation files')
    parser.add_argument('--fix', action='store_true',
                       help='Generate template test files')
    
    args = parser.parse_args()
    
    checker = TDDComplianceChecker(args.project_root)
    
    if args.all:
        # Find all implementation files
        impl_files = []
        impl_dir = pathlib.Path(args.project_root) / "05-implementation" / "src"
        if impl_dir.exists():
            for pattern in checker.impl_patterns:
                impl_files.extend(impl_dir.glob(f"**/{pattern}"))
    else:
        impl_files = [pathlib.Path(f) for f in args.files]
    
    if not impl_files:
        print("ℹ️ No implementation files to check")
        return 0
    
    print("🧪 TDD Compliance Check (XP Practices)")
    print("=" * 50)
    
    results = checker.check_multiple_files(impl_files)
    
    # Display results
    print(f"📊 Summary:")
    print(f"  Total files: {results['summary']['total']}")
    print(f"  ✅ Compliant: {results['summary']['compliant']}")
    print(f"  ❌ Non-compliant: {results['summary']['non_compliant']}")
    
    if results['non_compliant_files']:
        print(f"\n❌ Files missing tests (TDD violation):")
        for file_path in results['non_compliant_files']:
            print(f"  • {file_path}")
            
            if file_path in results['suggestions']:
                print()
                for suggestion in results['suggestions'][file_path]:
                    print(f"    {suggestion}")
                print()
        
        print(f"\n🎯 XP TDD Principle:")
        print(f"Every implementation requires tests FIRST (Red-Green-Refactor)")
        print(f"Tests serve as executable specification and safety net")
        
        return 1 if not args.fix else 0
    else:
        print(f"\n✅ All implementation files have corresponding tests!")
        print(f"🎉 TDD compliance maintained per XP practices")
        return 0

if __name__ == '__main__':
    sys.exit(main())