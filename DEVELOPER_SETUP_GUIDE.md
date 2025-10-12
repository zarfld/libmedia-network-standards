# Developer Standards Compliance Setup Guide

> **Quick Setup for ISO/IEC/IEEE Standards + XP Practices Integration**

This guide helps developers set up automated standards compliance checking to prevent CI failures and enforce quality standards throughout the development lifecycle.

## 🎯 What This Setup Provides

### Standards Coverage
- **ISO/IEC/IEEE 12207:2017**: Software lifecycle processes
- **ISO/IEC/IEEE 29148:2018**: Requirements engineering
- **IEEE 1016-2009**: Software design descriptions  
- **ISO/IEC/IEEE 42010:2011**: Architecture description
- **IEEE 1012-2016**: Verification and validation

### XP Practices Integration
- **Test-Driven Development (TDD)** validation
- **Continuous Integration** checks
- **Simple Design** principle enforcement
- **User Stories** format validation
- **Planning Game** integration

## 🚀 Quick Setup (5 Minutes)

### Step 1: Install Pre-commit Framework
```powershell
# Install pre-commit (if not already installed)
pip install pre-commit

# Install hooks in your repository
pre-commit install

# Test the setup
pre-commit run --all-files
```

### Step 2: Verify Scripts Are Executable
```powershell
# Test spec validation
py Scripts\validate-spec-structure.py --help

# Test compliance workflow
py Scripts\compliance-workflow.py --help

# Test template generator
py Scripts\generate-compliant-spec.py --help
```

### Step 3: Create Your First Compliant Spec
```powershell
# Generate a requirements spec
py Scripts\generate-compliant-spec.py requirements "My New Feature"

# Generate an architecture decision
py Scripts\generate-compliant-spec.py architecture "Database Selection"

# Validate the generated files
py Scripts\validate-spec-structure.py 02-requirements\functional\my-new-feature.md
```

## 📋 Daily Development Workflow

### Before Starting Work
```powershell
# Run full compliance check
py Scripts\compliance-workflow.py

# If issues found, get detailed report
more compliance-report.md
```

### When Creating Specifications
```powershell
# Generate compliant spec template
py Scripts\generate-compliant-spec.py <type> "<title>"

# Edit the generated file
# Validate before committing
py Scripts\validate-spec-structure.py <your-file.md>
```

### When Implementing Code (TDD)
```powershell
# Check TDD compliance
py Scripts\check-tdd-compliance.py 05-implementation\src\my-file.cpp

# Follow Red-Green-Refactor cycle:
# 1. RED: Write failing test first
# 2. GREEN: Write minimal code to pass
# 3. REFACTOR: Improve while keeping tests green
```

### Before Committing
Pre-commit hooks automatically run, but you can test manually:
```powershell
# Test what pre-commit will check
pre-commit run

# Run specific checks
pre-commit run validate-spec-structure
pre-commit run standards-compliance-check
```

## 🛠️ Troubleshooting Common Issues

### Issue: "Unicode encoding error in PowerShell"
**Solution**: The scripts now use Windows-compatible output. If you still see issues:
```powershell
# Set UTF-8 encoding for current session
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

# Or use cmd instead of PowerShell
cmd /c "py Scripts\validate-spec-structure.py <file>"
```

### Issue: "Pre-commit hooks not running"
**Solution**: 
```powershell
# Reinstall hooks
pre-commit uninstall
pre-commit install

# Check if .git/hooks/pre-commit exists
dir .git\hooks\
```

### Issue: "Spec validation failing"
**Solution**: Use the detailed error messages:
```powershell
# Get specific validation errors
py Scripts\validate-spec-structure.py <failing-file.md>

# Common fixes:
# 1. Check YAML front matter format
# 2. Ensure specType is 'requirements' or 'architecture'
# 3. Add missing fields: version, date, traceability
# 4. Fix standard reference format
```

### Issue: "TDD compliance failing"
**Solution**: 
```powershell
# Check what tests are missing
py Scripts\check-tdd-compliance.py --all

# Create test template
# Copy the suggested test structure from output
# Follow TDD Red-Green-Refactor cycle
```

## 🔧 Advanced Configuration

### Customize Pre-commit Checks
Edit `.pre-commit-config.yaml` to:
- Skip certain file types
- Add additional checks
- Modify validation rules

### Integrate with IDE
#### VS Code
1. Install "Run on Save" extension
2. Configure to run validation on spec file save:
```json
{
    "runOnSave.commands": [
        {
            "match": ".*\\.(md)$",
            "command": "py Scripts/validate-spec-structure.py ${file}",
            "runIn": "terminal"
        }
    ]
}
```

#### Other IDEs
Configure external tools to run:
- `py Scripts/validate-spec-structure.py ${file}` for spec validation
- `py Scripts/check-tdd-compliance.py ${file}` for TDD checks

### CI Integration Verification
The `.github/workflows/ci-standards-compliance.yml` automatically runs:
- Spec structure validation
- TDD compliance checks  
- Architecture compliance verification
- Cross-standard integration validation

## 📚 Standards Quick Reference

### Requirements Spec Template
```yaml
---
specType: requirements
standard: ISO/IEC/IEEE 29148:2018
phase: 02-requirements
version: 1.0.0
author: Your Name
date: "2024-01-01"
status: draft
traceability:
  stakeholderRequirements:
    - REQ-STK-TOPIC-001
---
```

### Architecture Spec Template  
```yaml
---
specType: architecture
standard: ISO/IEC/IEEE 42010:2011
phase: 03-architecture
version: 1.0.0
author: Your Name
date: "2024-01-01"
status: draft
traceability:
  requirements:
    - REQ-F-001
---
```

### TDD Test Structure
```cpp
#include <gtest/gtest.h>
#include "my_implementation.h"

class MyImplementationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }
    
    void TearDown() override {
        // Test cleanup
    }
};

TEST_F(MyImplementationTest, BasicFunctionality) {
    // RED: Start with failing test
    EXPECT_TRUE(false); // Replace with real test
    
    // GREEN: Write minimal code to pass
    // REFACTOR: Improve design while keeping tests green
}
```

## 🎯 Success Metrics

Your development workflow is compliant when:

### Pre-commit Checks
- ✅ All spec files pass structure validation
- ✅ TDD compliance maintained (>80% test coverage)
- ✅ Architecture decisions properly documented
- ✅ Code formatting standards applied

### CI Pipeline 
- ✅ All GitHub Actions workflows pass
- ✅ Standards compliance report shows 0 failures
- ✅ Cross-standard integration validated
- ✅ Requirements traceability maintained

### Quality Gates
- ✅ All specifications have proper YAML front matter
- ✅ Implementation files have corresponding test files
- ✅ Architecture decisions reference requirements
- ✅ Documentation follows IEEE standards format

## 🆘 Getting Help

### Check Compliance Status
```powershell
# Full compliance report
py Scripts\compliance-workflow.py

# Specific check types
py Scripts\compliance-workflow.py --check spec
py Scripts\compliance-workflow.py --check tdd
py Scripts\compliance-workflow.py --check architecture
py Scripts\compliance-workflow.py --check ci
```

### Generate Reports
```powershell
# Create compliance report
py Scripts\compliance-workflow.py --output my-compliance-report

# View human-readable summary
more my-compliance-report.md

# View detailed JSON report
more my-compliance-report.json
```

### Contact and Resources
- **Standards References**: See `Documentation/standards-compliance/` directory
- **Template Library**: Use `py Scripts\generate-compliant-spec.py --help`
- **Validation Tools**: Run `py Scripts\validate-spec-structure.py --help`

---

## ⚡ TL;DR - Essential Commands

```powershell
# Initial setup
pre-commit install

# Daily workflow
py Scripts\compliance-workflow.py
py Scripts\generate-compliant-spec.py requirements "Feature Name"
py Scripts\validate-spec-structure.py <file.md>
py Scripts\check-tdd-compliance.py <source-file.cpp>

# Before commit
pre-commit run
```

This setup ensures your development follows ISO/IEC/IEEE standards and XP practices while providing immediate feedback to prevent CI failures and maintain code quality! 🚀