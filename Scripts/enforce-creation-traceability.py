#!/usr/bin/env python3
"""
Traceability Creation Enforcement

Enforces traceability compliance when creating NEW artifacts:
- Requirements (Stakeholder → System → Functional)
- Architecture Decision Records (ADRs)
- Design Documents
- Test-Driven Development (TDD) artifacts
- Implementation specifications

Prevents orphaned artifacts from being created without proper traceability links.

Usage:
    python enforce-creation-traceability.py --validate-new-requirement REQ-F-1234
    python enforce-creation-traceability.py --validate-new-adr ADR-015
    python enforce-creation-traceability.py --pre-commit-hook
    python enforce-creation-traceability.py --create-template requirement --id REQ-F-1235
"""

import os
import re
import json
import yaml
import argparse
from pathlib import Path
from typing import Dict, List, Set, Optional, Tuple
from datetime import datetime
from dataclasses import dataclass

@dataclass
class TraceabilityTemplate:
    """Template for creating traceability-compliant artifacts"""
    artifact_type: str
    template_content: str
    required_links: List[str]
    validation_rules: List[str]

class CreationTraceabilityEnforcer:
    """Enforces traceability for newly created artifacts"""
    
    def __init__(self, repo_root: str):
        self.repo_root = Path(repo_root)
        self.templates = self._load_templates()
        self.existing_requirements = self._scan_existing_requirements()
        self.next_ids = self._calculate_next_available_ids()
        
    def _load_templates(self) -> Dict[str, TraceabilityTemplate]:
        """Load traceability-compliant templates"""
        templates = {}
        
        # Requirement Template
        templates['requirement'] = TraceabilityTemplate(
            artifact_type='requirement',
            template_content=self._get_requirement_template(),
            required_links=['stakeholderRequirements'],
            validation_rules=['must_link_to_stakeholder', 'unique_id', 'proper_format']
        )
        
        # ADR Template
        templates['adr'] = TraceabilityTemplate(
            artifact_type='adr',
            template_content=self._get_adr_template(),
            required_links=['requirements', 'relatedDecisions'],
            validation_rules=['must_link_to_requirements', 'unique_id', 'decision_rationale']
        )
        
        # Design Template
        templates['design'] = TraceabilityTemplate(
            artifact_type='design',
            template_content=self._get_design_template(),
            required_links=['requirements', 'architectureDecisions'],
            validation_rules=['must_link_to_requirements', 'must_link_to_architecture']
        )
        
        # TDD Test Template
        templates['test'] = TraceabilityTemplate(
            artifact_type='test',
            template_content=self._get_test_template(),
            required_links=['requirements', 'designElements'],
            validation_rules=['must_link_to_requirements', 'test_coverage', 'acceptance_criteria']
        )
        
        return templates
    
    def _get_requirement_template(self) -> str:
        """Generate requirement specification template with traceability"""
        return '''---
# Requirement Specification Template - TRACEABILITY ENFORCED
id: "{req_id}"
title: "{title}"
type: "{req_type}"  # functional, non-functional, stakeholder
phase: "{phase}"
priority: "{priority}"  # must-have, should-have, could-have, won't-have
status: "draft"  # draft, approved, implemented, verified

# MANDATORY TRACEABILITY - Cannot be empty
traceability:
  # Links to upstream requirements (REQUIRED)
  stakeholderRequirements:
    - "REQ-STK-XXX-001"  # REPLACE with actual stakeholder requirement IDs
    - "REQ-STK-XXX-002"  # Multiple links encouraged for comprehensive traceability
  
  # Links to related requirements (Optional but recommended)
  relatedRequirements: []
  
  # Links to architecture decisions (Auto-populated by tooling)
  architectureDecisions: []
  
  # Links to design elements (Auto-populated by tooling)  
  designElements: []
  
  # Links to test cases (Auto-populated by tooling)
  testCases: []

# IEEE/ISO Compliance Metadata
standards:
  ieee29148: "requirement_specification"  # IEEE 29148:2018 Requirements Engineering
  iso12207: "stakeholder_requirements"   # ISO/IEC/IEEE 12207:2017 Software Lifecycle
  
# Quality Attributes
validation:
  reviewRequired: true
  approvalRequired: true
  implementationTracking: true
  testCoverage: "required"
---

# {title}

## Description

[Provide clear, unambiguous description of the requirement]

**CRITICAL**: This requirement MUST be linked to stakeholder requirements above. Orphaned requirements violate IEEE 29148:2018 standards.

## Acceptance Criteria

**As a** [stakeholder type]  
**I want** [functionality]  
**So that** [business value]

### Acceptance Tests
- [ ] [Specific testable criterion 1]
- [ ] [Specific testable criterion 2] 
- [ ] [Specific testable criterion 3]

## Constraints

- **Technical**: [Any technical limitations]
- **Business**: [Any business constraints]
- **Regulatory**: [Any compliance requirements]

## Dependencies

### Upstream Dependencies
- Links to stakeholder requirements (see traceability section)
- [Other system dependencies]

### Downstream Dependencies  
- Architecture decisions will reference this requirement
- Design elements will implement this requirement
- Test cases will verify this requirement

## Notes

**Traceability Enforcement**: This requirement cannot be approved without:
1. ✅ Valid stakeholder requirement links
2. ✅ Clear acceptance criteria  
3. ✅ Testable conditions
4. ✅ IEEE 29148:2018 compliance

## Implementation Tracking

- **Created**: {date}
- **Author**: {author}
- **Phase**: {phase}
- **Next Review**: [Schedule review date]

---

**VALIDATION CHECKLIST** (Remove after completion):
- [ ] Requirement ID is unique and follows REQ-[F|NF]-NNNN format
- [ ] Stakeholder requirements are linked and exist
- [ ] Acceptance criteria are testable and measurable
- [ ] Description is clear and unambiguous
- [ ] Priority and status are appropriate
- [ ] IEEE/ISO compliance metadata is correct
'''

    def _get_adr_template(self) -> str:
        """Generate ADR template with traceability enforcement"""
        return '''---
# Architecture Decision Record - TRACEABILITY ENFORCED
id: "{adr_id}"
title: "{title}"
status: "proposed"  # proposed, approved, superseded, deprecated
date: "{date}"
authors: ["{author}"]

# MANDATORY TRACEABILITY - Cannot be empty
traceability:
  # Requirements this decision addresses (REQUIRED)
  requirements:
    - "REQ-F-XXX"  # REPLACE with actual requirement IDs
    - "REQ-NF-XXX"  # Include both functional and non-functional requirements
  
  # Related architecture decisions
  relatedDecisions: []  # Will be populated with related ADR IDs
  
  # Stakeholder requirements (inherited from linked requirements)
  stakeholderRequirements: []  # Auto-populated from requirement links
  
  # Design elements that implement this decision
  designElements: []  # Auto-populated by tooling
  
  # Test cases that validate this decision  
  testCases: []  # Auto-populated by tooling

# IEEE/ISO Compliance
standards:
  ieee42010: "architecture_decision"      # IEEE 42010:2011 Architecture Description  
  iso12207: "architectural_design"       # ISO/IEC/IEEE 12207:2017 Software Lifecycle
  
# Decision Metadata
decision:
  type: "{decision_type}"  # structural, behavioral, quality, technology
  scope: "{scope}"         # system, subsystem, component, interface
  impact: "{impact}"       # high, medium, low
  reversibility: "{reversibility}"  # reversible, difficult, irreversible
---

# {title}

## Context

**Problem Statement**: [Clear description of the problem or need]

**Requirements Context**: 
- This decision directly addresses requirements: {linked_requirements}
- **CRITICAL**: All linked requirements must exist and be approved before this ADR can be approved.

## Decision

**We will** [clear statement of the architectural decision]

**Because** [primary rationale for the decision]

## Rationale

### Options Considered

#### Option 1: [Alternative 1]
- **Pros**: [Benefits]
- **Cons**: [Drawbacks]
- **Requirements Impact**: [How this affects linked requirements]

#### Option 2: [Alternative 2]  
- **Pros**: [Benefits]
- **Cons**: [Drawbacks]
- **Requirements Impact**: [How this affects linked requirements]

#### **Selected Option**: [Chosen Alternative]
- **Rationale**: [Why this option was selected]
- **Requirements Satisfaction**: [How this satisfies all linked requirements]

## Consequences

### Positive Consequences
- [Benefit 1]
- [Benefit 2]

### Negative Consequences  
- [Risk 1 and mitigation strategy]
- [Risk 2 and mitigation strategy]

### Requirements Traceability Impact
- **Requirements Satisfied**: [List how each linked requirement is addressed]
- **New Requirements**: [Any new requirements this decision creates]
- **Changed Requirements**: [Any requirements this decision modifies]

## Implementation

### Design Elements Required
- [Component/Interface 1] - Will implement REQ-F-XXX
- [Component/Interface 2] - Will implement REQ-NF-XXX

### Testing Strategy
- [Test approach 1] - Will verify REQ-F-XXX
- [Test approach 2] - Will verify REQ-NF-XXX

## Compliance

**IEEE 42010:2011 Compliance**:
- ✅ Addresses stakeholder concerns through requirements linkage
- ✅ Provides architectural rationale
- ✅ Documents decision context and alternatives

**Traceability Enforcement**: This ADR cannot be approved without:
1. ✅ Valid requirement links to existing, approved requirements
2. ✅ Clear rationale for architectural choice
3. ✅ Implementation and testing strategy
4. ✅ Impact analysis on linked requirements

---

**VALIDATION CHECKLIST** (Remove after completion):
- [ ] ADR ID follows ADR-NNN format and is unique
- [ ] All linked requirements exist and are approved  
- [ ] Decision rationale is technically sound
- [ ] Implementation strategy is feasible
- [ ] Testing approach validates requirements
- [ ] IEEE 42010 compliance is demonstrated
'''

    def _get_design_template(self) -> str:
        """Generate design document template with traceability"""
        return '''---
# Design Specification - TRACEABILITY ENFORCED  
id: "{design_id}"
title: "{title}"
type: "{design_type}"  # component, interface, data-model, algorithm
status: "draft"  # draft, approved, implemented, verified
version: "1.0"

# MANDATORY TRACEABILITY - Cannot be empty
traceability:
  # Requirements this design implements (REQUIRED)
  requirements:
    - "REQ-F-XXX"  # REPLACE with actual functional requirements
    - "REQ-NF-XXX"  # REPLACE with actual non-functional requirements
  
  # Architecture decisions this design follows (REQUIRED)
  architectureDecisions:
    - "ADR-XXX"  # REPLACE with actual ADR IDs
    
  # Stakeholder requirements (inherited from requirement links)  
  stakeholderRequirements: []  # Auto-populated from requirement links
  
  # Test cases that validate this design
  testCases: []  # Auto-populated by tooling
  
  # Related design elements
  relatedDesigns: []

# IEEE/ISO Compliance
standards:
  ieee1016: "software_design_description"  # IEEE 1016:2009 Software Design
  iso12207: "detailed_design"             # ISO/IEC/IEEE 12207:2017 Software Lifecycle

# Design Metadata  
design:
  complexity: "{complexity}"      # low, medium, high
  criticality: "{criticality}"    # low, medium, high, safety-critical
  reusability: "{reusability}"    # single-use, reusable, framework
---

# {title}

## Purpose

**Design Intent**: [What this design element accomplishes]

**Requirements Traceability**: 
- Implements requirements: {linked_requirements}
- Follows architecture decisions: {linked_adrs}
- **CRITICAL**: All linked requirements and ADRs must exist and be approved.

## Design Overview

### System Context
[How this design fits into the larger system architecture]

### Key Design Principles
- [Principle 1 - derived from ADR-XXX]
- [Principle 2 - derived from REQ-NF-XXX]  
- [Principle 3 - derived from ADR-YYY]

## Detailed Design

### Components

#### Component 1: [Name]
- **Purpose**: [Implements REQ-F-XXX]
- **Interfaces**: [External interfaces]
- **Responsibilities**: [Core functions]
- **Implementation**: [Design approach]

#### Component 2: [Name]  
- **Purpose**: [Implements REQ-NF-XXX]
- **Interfaces**: [External interfaces] 
- **Responsibilities**: [Core functions]
- **Implementation**: [Design approach]

### Interfaces

#### Interface 1: [Name]
- **Contract**: [API specification]
- **Requirements Satisfied**: REQ-F-XXX, REQ-NF-YYY
- **Error Handling**: [Error conditions and responses]

### Data Models

#### Data Structure 1: [Name]
- **Purpose**: [Supports REQ-F-XXX]
- **Schema**: [Data structure definition]
- **Constraints**: [Validation rules from requirements]

## Requirements Satisfaction Analysis

### Functional Requirements
- **REQ-F-XXX**: Satisfied by [Component/Interface] through [mechanism]
- **REQ-F-YYY**: Satisfied by [Component/Interface] through [mechanism]

### Non-Functional Requirements  
- **REQ-NF-XXX**: Achieved through [design approach] with [metrics]
- **REQ-NF-YYY**: Achieved through [design approach] with [metrics]

### Architecture Decision Compliance
- **ADR-XXX**: Implemented through [design elements]
- **ADR-YYY**: Implemented through [design elements]

## Testing Strategy

### Unit Tests
- [Test approach for Component 1] - Validates REQ-F-XXX
- [Test approach for Component 2] - Validates REQ-NF-XXX

### Integration Tests  
- [Test approach for Interface 1] - Validates REQ-F-YYY
- [Cross-component tests] - Validates system-level requirements

### Test-Driven Development (TDD)
```
Given: [Preconditions from requirements]
When: [Action from acceptance criteria]  
Then: [Expected outcome from requirements]
```

## Implementation Notes

### Development Guidelines
- [Guideline 1 - ensures REQ-NF-XXX compliance]
- [Guideline 2 - follows ADR-XXX decision]

### Quality Assurance
- **Code Review**: Focus on requirements satisfaction
- **Design Review**: Validate ADR compliance
- **Requirements Review**: Confirm all linked requirements are addressed

## Compliance Validation

**IEEE 1016:2009 Compliance**:
- ✅ Design description follows standard format
- ✅ Requirements traceability documented
- ✅ Interface specifications complete

**Traceability Enforcement**: This design cannot be approved without:
1. ✅ Valid links to existing, approved requirements
2. ✅ Valid links to existing, approved architecture decisions  
3. ✅ Demonstration of requirements satisfaction
4. ✅ Testable implementation approach
5. ✅ IEEE 1016 compliance

---

**VALIDATION CHECKLIST** (Remove after completion):
- [ ] Design ID follows DES-XXX format and is unique
- [ ] All linked requirements exist and are approved
- [ ] All linked ADRs exist and are approved
- [ ] Design satisfies all linked requirements
- [ ] Test strategy validates requirements
- [ ] Implementation approach is feasible
- [ ] IEEE 1016 compliance is demonstrated
'''

    def _get_test_template(self) -> str:
        """Generate TDD test template with traceability"""
        return '''---
# Test Specification - TRACEABILITY ENFORCED
id: "{test_id}"  
title: "{title}"
type: "{test_type}"  # unit, integration, system, acceptance, performance
status: "draft"  # draft, approved, implemented, passed, failed
framework: "{framework}"  # pytest, junit, cucumber, etc.

# MANDATORY TRACEABILITY - Cannot be empty
traceability:
  # Requirements this test validates (REQUIRED)
  requirements:
    - "REQ-F-XXX"  # REPLACE with actual requirement IDs
    - "REQ-NF-XXX"  # Include acceptance criteria from requirements
  
  # Design elements this test verifies (REQUIRED)  
  designElements:
    - "DES-XXX"  # REPLACE with actual design IDs
    
  # Architecture decisions this test validates
  architectureDecisions:
    - "ADR-XXX"  # ADRs that affect testability
    
  # Stakeholder requirements (inherited)
  stakeholderRequirements: []  # Auto-populated from requirement links

# Test-Driven Development Metadata
tdd:
  redGreenRefactor: true    # Following TDD cycle
  testFirst: true           # Test written before implementation  
  acceptanceCriteria: true  # Based on requirement acceptance criteria
  
# IEEE/ISO Compliance
standards:
  ieee1012: "verification_validation"    # IEEE 1012:2016 V&V
  iso29119: "test_specification"         # ISO/IEC/IEEE 29119 Software Testing
  iso12207: "verification_process"       # ISO/IEC/IEEE 12207:2017 Lifecycle
---

# {title}

## Test Purpose

**Validation Target**: [What requirements/design elements this test validates]

**Requirements Traceability**:
- Validates requirements: {linked_requirements}
- Verifies design elements: {linked_design_elements}
- **CRITICAL**: All linked requirements and design elements must exist and be approved.

## Test-Driven Development (TDD) Approach

### Red Phase (Failing Test)
```python
def test_{test_name}_red():
    """
    Test Case: {test_case_name}
    Requirement: REQ-F-XXX
    Acceptance Criterion: [From requirement specification]
    
    Expected: Test fails because functionality not implemented yet
    """
    # Given: [Preconditions from requirement]
    given_condition = setup_test_condition()
    
    # When: [Action from acceptance criteria]  
    result = system_under_test.perform_action(given_condition)
    
    # Then: [Expected outcome from requirement]
    assert result == expected_outcome_from_REQ_F_XXX
    # This should FAIL initially (Red phase)
```

### Green Phase (Minimal Implementation)
```python  
def test_{test_name}_green():
    """
    Test Case: {test_case_name}
    Requirement: REQ-F-XXX
    Implementation: DES-XXX
    
    Expected: Test passes with minimal implementation
    """
    # Given: [Same preconditions]
    given_condition = setup_test_condition()
    
    # When: [Same action]
    result = system_under_test.perform_action(given_condition)
    
    # Then: [Same expectation, now passing]
    assert result == expected_outcome_from_REQ_F_XXX
    # This should PASS with minimal implementation (Green phase)
```

### Refactor Phase (Improved Implementation)  
```python
def test_{test_name}_refactor():
    """
    Test Case: {test_case_name}
    Requirement: REQ-F-XXX, REQ-NF-XXX  
    Design: DES-XXX
    
    Expected: Test passes with optimized implementation
    """
    # Given: [Enhanced test conditions for non-functional requirements]
    given_condition = setup_enhanced_test_condition()
    
    # When: [Same action, potentially with performance/quality constraints]
    start_time = time.time()
    result = system_under_test.perform_action(given_condition)
    execution_time = time.time() - start_time
    
    # Then: [Functional + non-functional requirements satisfied]
    assert result == expected_outcome_from_REQ_F_XXX  # Functional requirement
    assert execution_time < max_time_from_REQ_NF_XXX  # Non-functional requirement
    # This should PASS with refactored, optimized implementation
```

## Requirements Coverage Analysis

### Functional Requirements Testing

#### REQ-F-XXX: [Requirement Title]
- **Acceptance Criteria**: [From requirement specification]
- **Test Cases**:
  - `test_req_f_xxx_happy_path()` - Normal operation
  - `test_req_f_xxx_error_conditions()` - Error handling
  - `test_req_f_xxx_boundary_values()` - Edge cases
- **Coverage**: 100% of acceptance criteria

#### REQ-F-YYY: [Requirement Title]  
- **Acceptance Criteria**: [From requirement specification]
- **Test Cases**:
  - `test_req_f_yyy_integration()` - Integration scenarios
  - `test_req_f_yyy_data_validation()` - Data constraints
- **Coverage**: 100% of acceptance criteria

### Non-Functional Requirements Testing

#### REQ-NF-XXX: [Performance Requirement]
- **Acceptance Criteria**: [Performance thresholds from requirement]
- **Test Cases**:
  - `test_performance_under_normal_load()` - Expected load
  - `test_performance_under_peak_load()` - Maximum load
  - `test_performance_degradation_graceful()` - Overload behavior
- **Metrics**: Response time, throughput, resource usage

#### REQ-NF-YYY: [Security Requirement]
- **Acceptance Criteria**: [Security controls from requirement]  
- **Test Cases**:
  - `test_authentication_mechanisms()` - Access control
  - `test_data_encryption()` - Data protection
  - `test_input_validation()` - Injection prevention
- **Coverage**: All security controls validated

## Design Element Verification

### Component Testing
- **DES-XXX Component**: Verified through unit tests
  - Interface contracts validated
  - Internal logic verified
  - Error handling confirmed

### Integration Testing  
- **Component Interactions**: Verified through integration tests
  - Data flow validation
  - Protocol compliance
  - System-level behavior

## Test Implementation

### Test Setup
```python
@pytest.fixture
def test_setup():
    """Setup test environment per requirements and design specifications"""
    # Initialize system per DES-XXX design
    system = SystemUnderTest()
    
    # Configure per REQ-F-XXX requirements
    system.configure_per_requirements()
    
    return system
```

### Test Execution Strategy
1. **Automated Execution**: All tests run in CI/CD pipeline
2. **Traceability Validation**: Verify requirement links before execution
3. **Coverage Measurement**: Ensure 100% requirement coverage
4. **Results Tracking**: Link test results back to requirements

### Test Data Management
- **Test Data**: Based on requirement specifications and acceptance criteria
- **Data Privacy**: Complies with REQ-NF-XXX privacy requirements  
- **Data Integrity**: Validates per DES-XXX data model constraints

## Compliance Validation

**IEEE 1012:2016 V&V Compliance**:
- ✅ Test cases derived from requirements
- ✅ Verification activities documented  
- ✅ Validation criteria defined
- ✅ Traceability to requirements maintained

**TDD Best Practices**:
- ✅ Tests written before implementation
- ✅ Red-Green-Refactor cycle followed
- ✅ Acceptance criteria directly tested
- ✅ Requirements traceability maintained

**Traceability Enforcement**: This test specification cannot be approved without:
1. ✅ Valid links to existing, approved requirements
2. ✅ Valid links to existing, approved design elements
3. ✅ Test cases covering 100% of linked requirement acceptance criteria
4. ✅ TDD approach documented and followed
5. ✅ IEEE 1012 V&V compliance demonstrated

---

**VALIDATION CHECKLIST** (Remove after completion):
- [ ] Test ID follows TST-XXX format and is unique
- [ ] All linked requirements exist and are approved
- [ ] All linked design elements exist and are approved  
- [ ] Test cases cover 100% of requirement acceptance criteria
- [ ] TDD Red-Green-Refactor cycle is documented
- [ ] Performance/security/quality tests address non-functional requirements
- [ ] Test implementation is executable and maintainable
- [ ] IEEE 1012 V&V compliance is demonstrated
'''

    def _scan_existing_requirements(self) -> Set[str]:
        """Scan repository for existing requirement IDs"""
        existing = set()
        
        # Scan specification files for requirement IDs
        for phase_dir in ['01-stakeholder-requirements', '02-requirements', '03-architecture']:
            phase_path = self.repo_root / phase_dir
            if phase_path.exists():
                for md_file in phase_path.glob('**/*.md'):
                    try:
                        content = md_file.read_text(encoding='utf-8')
                        # Find requirement IDs in various formats
                        req_ids = re.findall(r'\b(REQ-[FN]F?-\d{3,4}|REQ-\w+-\w+-\d{3}|ADR-\d{3,4}|DES-\d{3,4}|TST-\d{3,4})\b', content)
                        existing.update(req_ids)
                    except:
                        continue
                        
        return existing
    
    def _calculate_next_available_ids(self) -> Dict[str, int]:
        """Calculate next available ID numbers for each artifact type"""
        next_ids = {
            'REQ-F': 2000,    # Functional requirements start at 2000
            'REQ-NF': 2000,   # Non-functional requirements start at 2000
            'REQ-STK': 1000,  # Stakeholder requirements start at 1000
            'ADR': 100,       # Architecture decisions start at 100
            'DES': 1000,      # Design elements start at 1000
            'TST': 1000       # Test cases start at 1000
        }
        
        # Find highest existing IDs and increment
        for existing_id in self.existing_requirements:
            for prefix in next_ids.keys():
                if existing_id.startswith(prefix):
                    try:
                        num_part = existing_id.split('-')[-1]
                        num = int(num_part)
                        if num >= next_ids[prefix]:
                            next_ids[prefix] = num + 1
                    except (ValueError, IndexError):
                        continue
                        
        return next_ids
    
    def validate_new_artifact(self, artifact_type: str, artifact_id: str, file_path: str = None) -> Tuple[bool, List[str]]:
        """Validate a new artifact for traceability compliance"""
        violations = []
        
        # Check ID uniqueness
        if artifact_id in self.existing_requirements:
            violations.append(f"Artifact ID {artifact_id} already exists - IDs must be unique")
        
        # Check ID format
        if not self._validate_id_format(artifact_type, artifact_id):
            violations.append(f"Artifact ID {artifact_id} does not follow required format for {artifact_type}")
        
        # If file path provided, validate file content
        if file_path and Path(file_path).exists():
            content_violations = self._validate_file_content(artifact_type, file_path)
            violations.extend(content_violations)
        
        return len(violations) == 0, violations
    
    def _validate_id_format(self, artifact_type: str, artifact_id: str) -> bool:
        """Validate artifact ID follows correct format"""
        format_patterns = {
            'requirement': r'^REQ-[FN]F?-\d{3,4}$',
            'adr': r'^ADR-\d{3,4}$',
            'design': r'^DES-\d{3,4}$', 
            'test': r'^TST-\d{3,4}$'
        }
        
        pattern = format_patterns.get(artifact_type)
        if not pattern:
            return False
            
        return bool(re.match(pattern, artifact_id))
    
    def _validate_file_content(self, artifact_type: str, file_path: str) -> List[str]:
        """Validate file content for traceability compliance"""
        violations = []
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
                
            # Parse YAML front matter
            if content.startswith('---'):
                yaml_end = content.find('---', 3)
                if yaml_end > 0:
                    yaml_content = content[4:yaml_end]
                    try:
                        yaml_data = yaml.safe_load(yaml_content)
                        
                        # Validate required traceability section
                        if 'traceability' not in yaml_data:
                            violations.append(f"Missing required traceability section in YAML front matter")
                        else:
                            traceability = yaml_data['traceability']
                            
                            # Validate artifact-specific traceability requirements
                            required_links = self.templates[artifact_type].required_links
                            for required_link in required_links:
                                if required_link not in traceability or not traceability[required_link]:
                                    violations.append(f"Missing required traceability link: {required_link}")
                                else:
                                    # Validate linked artifacts exist
                                    linked_ids = traceability[required_link]
                                    if isinstance(linked_ids, list):
                                        for linked_id in linked_ids:
                                            if linked_id not in self.existing_requirements and not linked_id.startswith('REQ-XXX'):
                                                violations.append(f"Linked artifact does not exist: {linked_id}")
                                    
                    except yaml.YAMLError:
                        violations.append("Invalid YAML format in front matter")
            else:
                violations.append("Missing YAML front matter - required for traceability enforcement")
                
        except Exception as e:
            violations.append(f"Error reading file: {e}")
            
        return violations
    
    def create_traceability_compliant_artifact(self, artifact_type: str, **kwargs) -> Tuple[str, str]:
        """Create new artifact with enforced traceability compliance"""
        
        # Generate next available ID
        if artifact_type == 'requirement':
            req_type = kwargs.get('req_type', 'F')  # F or NF
            prefix = f'REQ-{req_type}'
            next_id = f"{prefix}-{self.next_ids[prefix]:04d}"
        elif artifact_type == 'adr':
            next_id = f"ADR-{self.next_ids['ADR']:03d}"
        elif artifact_type == 'design':
            next_id = f"DES-{self.next_ids['DES']:04d}"
        elif artifact_type == 'test':
            next_id = f"TST-{self.next_ids['TST']:04d}"
        else:
            raise ValueError(f"Unsupported artifact type: {artifact_type}")
        
        # Get template and populate with provided data
        template = self.templates[artifact_type]
        content = template.template_content
        
        # Replace template variables
        replacements = {
            'req_id': next_id,
            'adr_id': next_id,
            'design_id': next_id,
            'test_id': next_id,
            'title': kwargs.get('title', f'New {artifact_type.title()}'),
            'req_type': kwargs.get('req_type', 'functional'),
            'decision_type': kwargs.get('decision_type', 'structural'),
            'design_type': kwargs.get('design_type', 'component'),
            'test_type': kwargs.get('test_type', 'unit'),
            'framework': kwargs.get('framework', 'pytest'),
            'phase': kwargs.get('phase', '02-requirements'),
            'priority': kwargs.get('priority', 'must-have'),
            'scope': kwargs.get('scope', 'system'),
            'impact': kwargs.get('impact', 'medium'),
            'reversibility': kwargs.get('reversibility', 'reversible'),
            'complexity': kwargs.get('complexity', 'medium'),
            'criticality': kwargs.get('criticality', 'medium'),
            'reusability': kwargs.get('reusability', 'reusable'),
            'date': datetime.now().strftime('%Y-%m-%d'),
            'author': kwargs.get('author', 'System'),
            'linked_requirements': ', '.join(kwargs.get('linked_requirements', [])),
            'linked_adrs': ', '.join(kwargs.get('linked_adrs', [])),
            'linked_design_elements': ', '.join(kwargs.get('linked_design_elements', [])),
            'test_case_name': kwargs.get('test_case_name', 'test_basic_functionality'),
            'test_name': kwargs.get('test_name', 'basic_functionality')
        }
        
        for key, value in replacements.items():
            content = content.replace(f'{{{key}}}', str(value))
            
        return next_id, content
    
    def validate_pre_commit(self, changed_files: List[str]) -> Tuple[bool, List[str]]:
        """Validate traceability compliance for pre-commit hook"""
        violations = []
        
        for file_path in changed_files:
            if not file_path.endswith('.md'):
                continue
                
            # Determine artifact type from file path and content
            artifact_type = self._determine_artifact_type(file_path)
            if not artifact_type:
                continue
                
            # Extract artifact ID from file content
            artifact_id = self._extract_artifact_id(file_path)
            if not artifact_id:
                violations.append(f"Could not extract valid artifact ID from {file_path}")
                continue
                
            # Validate the artifact
            is_valid, artifact_violations = self.validate_new_artifact(artifact_type, artifact_id, file_path)
            if not is_valid:
                violations.extend([f"{file_path}: {v}" for v in artifact_violations])
        
        return len(violations) == 0, violations
    
    def _determine_artifact_type(self, file_path: str) -> Optional[str]:
        """Determine artifact type from file path"""
        path_lower = file_path.lower()
        
        if '/requirements/' in path_lower or 'req-' in path_lower:
            return 'requirement'
        elif '/architecture/' in path_lower and ('adr-' in path_lower or 'decision' in path_lower):
            return 'adr'
        elif '/design/' in path_lower or 'des-' in path_lower:
            return 'design'
        elif '/test' in path_lower or 'tst-' in path_lower:
            return 'test'
        
        return None
    
    def _extract_artifact_id(self, file_path: str) -> Optional[str]:
        """Extract artifact ID from file content"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
                
            # Look for ID in YAML front matter
            yaml_match = re.search(r'id:\s*["\']?([^"\'\n]+)["\']?', content)
            if yaml_match:
                return yaml_match.group(1).strip()
                
            # Look for ID in filename
            filename = Path(file_path).name
            id_match = re.search(r'(REQ-[FN]F?-\d{3,4}|ADR-\d{3,4}|DES-\d{3,4}|TST-\d{3,4})', filename)
            if id_match:
                return id_match.group(1)
                
        except Exception:
            pass
            
        return None

def main():
    parser = argparse.ArgumentParser(description='Traceability Creation Enforcement')
    parser.add_argument('--repo-root', default='.', help='Repository root directory')
    parser.add_argument('--validate-new-requirement', help='Validate new requirement ID')
    parser.add_argument('--validate-new-adr', help='Validate new ADR ID')
    parser.add_argument('--validate-new-design', help='Validate new design ID')
    parser.add_argument('--validate-new-test', help='Validate new test ID')
    parser.add_argument('--pre-commit-hook', action='store_true', help='Run pre-commit validation')
    parser.add_argument('--create-template', help='Create template for artifact type')
    parser.add_argument('--id', help='Specific ID for template (optional)')
    parser.add_argument('--title', help='Title for new artifact')
    parser.add_argument('--output', help='Output file path for generated template')
    
    args = parser.parse_args()
    
    enforcer = CreationTraceabilityEnforcer(args.repo_root)
    
    print("🔗 Creation Traceability Enforcement")
    print("=" * 40)
    
    success = True
    
    # Handle individual artifact validation
    validations = [
        (args.validate_new_requirement, 'requirement'),
        (args.validate_new_adr, 'adr'),
        (args.validate_new_design, 'design'),
        (args.validate_new_test, 'test')
    ]
    
    for artifact_id, artifact_type in validations:
        if artifact_id:
            is_valid, violations = enforcer.validate_new_artifact(artifact_type, artifact_id)
            if is_valid:
                print(f"✅ {artifact_type.title()} {artifact_id} validation PASSED")
            else:
                print(f"❌ {artifact_type.title()} {artifact_id} validation FAILED:")
                for violation in violations:
                    print(f"  - {violation}")
                success = False
    
    # Handle pre-commit hook
    if args.pre_commit_hook:
        # Get changed files from git
        import subprocess
        try:
            result = subprocess.run(['git', 'diff', '--cached', '--name-only'], 
                                  capture_output=True, text=True, check=True)
            changed_files = result.stdout.strip().split('\n') if result.stdout.strip() else []
            
            is_valid, violations = enforcer.validate_pre_commit(changed_files)
            if is_valid:
                print("✅ Pre-commit traceability validation PASSED")
            else:
                print("❌ Pre-commit traceability validation FAILED:")
                for violation in violations:
                    print(f"  - {violation}")
                success = False
                
        except subprocess.CalledProcessError:
            print("⚠️  Could not get git staged files")
    
    # Handle template creation
    if args.create_template:
        try:
            artifact_id, content = enforcer.create_traceability_compliant_artifact(
                args.create_template,
                title=args.title or f"New {args.create_template.title()}",
                id=args.id
            )
            
            if args.output:
                with open(args.output, 'w', encoding='utf-8') as f:
                    f.write(content)
                print(f"✅ Created {args.create_template} template: {args.output}")
                print(f"   Assigned ID: {artifact_id}")
            else:
                print(f"✅ Generated {args.create_template} template with ID: {artifact_id}")
                print("\nTemplate content:")
                print("=" * 50)
                print(content[:1000] + "..." if len(content) > 1000 else content)
                
        except Exception as e:
            print(f"❌ Template creation failed: {e}")
            success = False
    
    exit(0 if success else 1)

if __name__ == '__main__':
    main()