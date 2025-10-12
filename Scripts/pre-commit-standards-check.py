#!/usr/bin/env python3
"""
Pre-commit hook for standards compliance enforcement
Validates all spec files before commit per ISO/IEC/IEEE standards
"""

import sys
import subprocess
import pathlib

def run_validation():
    """Run the existing validation script with enhanced output"""
    try:
        result = subprocess.run([
            sys.executable, 
            'Scripts/validate-spec-structure.py'
        ], capture_output=True, text=True, cwd=pathlib.Path(__file__).parent.parent)
        
        if result.returncode != 0:
            print("🚫 PRE-COMMIT BLOCKED: Standards compliance issues found!")
            print("\n" + "="*60)
            print("STANDARDS COMPLIANCE VALIDATION FAILED")
            print("="*60)
            print(result.stdout)
            print(result.stderr)
            print("\n💡 FIX GUIDANCE:")
            print("1. Review the validation errors above")
            print("2. Fix YAML front matter in flagged files")
            print("3. Ensure specType, standard, phase, version, date fields are correct")
            print("4. Add required traceability sections")
            print("5. Run validation locally: python Scripts/validate-spec-structure.py")
            print("\n📋 STANDARDS REQUIRED:")
            print("• ISO/IEC/IEEE 12207:2017 - Software lifecycle processes")
            print("• ISO/IEC/IEEE 29148:2018 - Requirements engineering")  
            print("• IEEE 1016-2009 - Software design descriptions")
            print("• ISO/IEC/IEEE 42010:2011 - Architecture description")
            print("• XP Practices - TDD, User Stories, Continuous Integration")
            return False
        else:
            print("✅ Standards compliance validated - commit allowed")
            print(result.stdout)
            return True
            
    except Exception as e:
        print(f"❌ Pre-commit validation failed: {e}")
        return False

if __name__ == '__main__':
    if not run_validation():
        sys.exit(1)
    print("🎯 Commit approved - standards compliance maintained!")