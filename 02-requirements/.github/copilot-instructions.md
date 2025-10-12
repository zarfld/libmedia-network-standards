---
title: "Phase 02 Copilot Instructions"
specType: guidance
phase: 02-requirements
version: 1.0.0
status: approved
author: template-system
date: 2025-10-03
description: "Operational guidance for Requirements Analysis & Specification (ISO/IEC/IEEE 29148, 12207)."
---

# Phase 02: Requirements Analysis & Specification

**Standards**: ISO/IEC/IEEE 29148:2018 (System Requirements), ISO/IEC/IEEE 12207:2017  
**XP Integration**: User Stories, Acceptance Tests, YAGNI Principle

## 🎯 Phase Objectives

1. Transform stakeholder requirements into system requirements
2. Define functional and non-functional requirements
3. Create detailed use cases and user stories
4. Establish requirements traceability
5. Define testable acceptance criteria

## 📂 Working Directory Context

```yaml
applyTo:
  - "02-requirements/**/*.md"
  - "02-requirements/functional/**"
  - "02-requirements/non-functional/**"
  - "02-requirements/use-cases/**"
  - "02-requirements/user-stories/**"
```

## ⚠️ MANDATORY: YAML Front Matter Schema Compliance

**CRITICAL**: All requirements specification files MUST use EXACT YAML front matter format defined in authoritative schema:

**Authoritative Schema**: `spec-kit-templates/schemas/requirements-spec.schema.json`

**Required YAML Front Matter Format**:
```yaml
---
specType: requirements
standard: 29148
phase: 02-requirements
version: 1.0.0
author: [Your Name]
date: 2025-MM-DD
status: draft  # draft | review | approved | deprecated
traceability:
  stakeholderRequirements:
    - StR-001
    - REQ-STK-[A-Z0-9]+-001
---
```

**ENFORCEMENT**: 
- Do NOT use full standard names like "ISO/IEC/IEEE 29148:2018" - use "29148" only
- Do NOT modify schema patterns - match them exactly
- Reference authoritative schema file for any questions
- Validation will FAIL if format deviates from schema

## 📋 ISO/IEC/IEEE 29148:2018 Compliance

### System Requirements Process Activities

1. **Requirements Analysis**
   - Decompose stakeholder requirements
   - Identify system boundaries
   - Define interfaces
   - Analyze feasibility
   - Resolve conflicts

2. **System Requirements Specification**
   - Define functional requirements
   - Define non-functional requirements (quality attributes)
   - Specify constraints
   - Define interfaces
   - Create System Requirements Specification (SyRS)

3. **Requirements Validation**
   - Review for completeness
   - Check consistency
   - Verify traceability to stakeholder requirements
   - Validate with stakeholders

## 🎨 XP Practices for This Phase

### User Stories
Transform requirements into user stories:
```markdown
As a [user role]
I want to [action/capability]
So that [business value]

Acceptance Criteria:
- Given [context]
- When [action]
- Then [outcome]
```

### YAGNI (You Aren't Gonna Need It)
- Only specify requirements for current iteration + 1
- Avoid speculative features
- Keep requirements minimal and focused

### Acceptance Test-Driven Development
- Define acceptance tests BEFORE implementation
- Make acceptance criteria executable
- Customer defines acceptance tests

## 📝 Required Deliverables

### 1. System Requirements Specification (SyRS)
**Location**: `system-requirements-specification.md`

```markdown
# System Requirements Specification

## 1. Introduction
### 1.1 Purpose
### 1.2 Scope
### 1.3 Definitions, Acronyms, Abbreviations
### 1.4 References
### 1.5 Overview

## 2. Functional Requirements
### 2.1 [Feature Category 1]
#### REQ-F-001: [Requirement Title]
- **Trace to**: StR-XXX
- **Description**: [What the system shall do]
- **Priority**: Critical/High/Medium/Low
- **Rationale**: [Why needed]
- **Acceptance Criteria**:
  - Given [precondition]
  - When [action]
  - Then [expected result]
- **Dependencies**: REQ-XXX
- **Assumptions**: [If any]

## 3. Non-Functional Requirements
### 3.1 Performance Requirements
#### REQ-NF-001: [Performance Requirement]
- **Trace to**: StR-XXX
- **Metric**: [Measurable metric]
- **Target**: [Specific value]
- **Acceptance Test**: [How to verify]

### 3.2 Security Requirements
### 3.3 Usability Requirements
### 3.4 Reliability Requirements
### 3.5 Maintainability Requirements
### 3.6 Portability Requirements
### 3.7 Scalability Requirements

## 4. System Interfaces
### 4.1 User Interfaces
### 4.2 Hardware Interfaces
### 4.3 Software Interfaces
### 4.4 Communication Interfaces

## 5. Constraints
### 5.1 Design Constraints
### 5.2 Implementation Constraints
### 5.3 Interface Constraints

## 6. Traceability Matrix
| System Req | Stakeholder Req | Priority | Status |
|------------|----------------|----------|--------|
| REQ-F-001  | StR-001        | High     | Draft  |
```

### 2. Use Cases
**Location**: `use-cases/UC-XXX-[name].md`

Follow "Writing Effective Use Cases" (Alistair Cockburn) format:

```markdown
# Use Case: UC-001 [Use Case Name]

## Brief Description
[One paragraph summary]

## Actors
- **Primary Actor**: [User role who initiates]
- **Secondary Actors**: [Supporting actors]
- **Stakeholders and Interests**:
  - [Stakeholder]: [Interest]

## Preconditions
- [State that must be true before use case starts]

## Postconditions
- **Success End Condition**: [System state after success]
- **Failure End Condition**: [System state after failure]

## Main Success Scenario
1. [Actor action]
2. [System response]
3. [Next actor action]
4. [System response]
...

## Extensions (Alternative Flows)
### 3a. [Alternative condition]
- 3a1. [Alternative action]
- 3a2. [System response]

## Special Requirements
- [Non-functional requirements specific to this use case]

## Technology and Data Variations List
- [Variations in implementation]

## Frequency of Occurrence
[How often this use case occurs]

## Trace to Requirements
- REQ-F-001
- REQ-NF-005
```

### 3. User Stories
**Location**: `user-stories/STORY-XXX-[name].md`

```markdown
# User Story: STORY-001 [Story Title]

## Story
As a [user role]
I want to [action/feature]
So that [business benefit]

## Trace to Requirements
- StR-XXX
- REQ-F-XXX

## Acceptance Criteria
### Scenario 1: [Success Path]
Given [initial context]
And [additional context]
When [action taken]
Then [expected outcome]
And [additional outcome]

### Scenario 2: [Alternative Path]
Given [different context]
When [action taken]
Then [expected outcome]

### Scenario 3: [Error Handling]
Given [error condition]
When [action taken]
Then [error handling expected]

## Definition of Done
- [ ] Code implemented
- [ ] Unit tests pass (TDD)
- [ ] Acceptance tests pass
- [ ] Code reviewed
- [ ] Documentation updated
- [ ] Integrated into main branch

## Story Points
[Estimate: 1, 2, 3, 5, 8, 13]

## Priority
[Critical/High/Medium/Low]

## Dependencies
- STORY-XXX (must complete first)

## Technical Notes
[Implementation hints, constraints, risks]

## Questions/Clarifications Needed
- [ ] [Question 1]
- [ ] [Question 2]
```

### 4. Non-Functional Requirements Document
**Location**: `non-functional/nfr-specification.md`

```markdown
# Non-Functional Requirements Specification

## 1. Performance Requirements

### REQ-NF-P-001: Response Time
- **Description**: System response time for user interactions
- **Metric**: 95th percentile response time
- **Target**: < 200ms
- **Test Method**: Load testing with 1000 concurrent users
- **Trace to**: StR-XXX

### REQ-NF-P-002: Throughput
- **Description**: Transaction processing capacity
- **Metric**: Transactions per second (TPS)
- **Target**: 10,000 TPS sustained
- **Test Method**: Performance testing under load

## 2. Security Requirements

### REQ-NF-S-001: Authentication
- **Description**: User authentication mechanism
- **Requirement**: Multi-factor authentication required
- **Standard**: NIST 800-63B compliance
- **Test Method**: Security audit

### REQ-NF-S-002: Data Encryption
- **Description**: Data protection in transit and at rest
- **Requirement**: AES-256 encryption
- **Standard**: FIPS 140-2 compliance

## 3. Usability Requirements

### REQ-NF-U-001: Learnability
- **Description**: Time for new user to become productive
- **Metric**: Task completion time
- **Target**: 80% of users complete core tasks in < 10 minutes
- **Test Method**: Usability testing

## 4. Reliability Requirements

### REQ-NF-R-001: Availability
- **Description**: System uptime
- **Metric**: Percentage uptime
- **Target**: 99.9% (8.76 hours downtime/year max)
- **Test Method**: Availability monitoring

### REQ-NF-R-002: Mean Time Between Failures (MTBF)
- **Target**: > 720 hours

## 5. Maintainability Requirements

### REQ-NF-M-001: Code Quality
- **Metric**: Code coverage
- **Target**: > 80% unit test coverage
- **Standard**: XP TDD practices

### REQ-NF-M-002: Technical Debt
- **Metric**: Maintainability Index
- **Target**: > 75 (Visual Studio metric)

## 6. Scalability Requirements

### REQ-NF-SC-001: Horizontal Scaling
- **Description**: Ability to add capacity by adding nodes
- **Target**: Linear scaling up to 10 nodes

## 7. Compliance Requirements

### REQ-NF-C-001: [Regulatory Requirement]
- **Standard**: [e.g., GDPR, HIPAA, SOC 2]
- **Requirements**: [Specific compliance requirements]
```

## 🚨 Critical Requirements for This Phase

### Always Do
✅ Trace every system requirement to stakeholder requirement(s)  
✅ Define testable acceptance criteria for every requirement  
✅ Include non-functional requirements (not just functional)  
✅ Use consistent requirement IDs (REQ-F-XXX, REQ-NF-XXX)  
✅ Write user stories in Given-When-Then format  
✅ Prioritize with customer involvement  
✅ Document assumptions and dependencies  
✅ Validate requirements with stakeholders  

### Never Do
❌ Create untraceable requirements  
❌ Use ambiguous language ("fast," "user-friendly")  
❌ Mix requirements with design solutions  
❌ Skip non-functional requirements  
❌ Create requirements without acceptance criteria  
❌ Proceed with inconsistent or conflicting requirements  
❌ Specify requirements for "future" features (YAGNI)  

## 🔍 Clarifying Questions to Ask

### About Functional Requirements
1. What exactly should the system do in this scenario?
2. What are all the possible outcomes/results?
3. What happens in error conditions?
4. Are there any special cases or exceptions?
5. What data is needed as input?
6. What data should be produced as output?

### About Non-Functional Requirements
1. What are the performance expectations?
   - Response time?
   - Throughput?
   - Number of concurrent users?
2. What are the security requirements?
   - Authentication method?
   - Authorization levels?
   - Data encryption needs?
3. What are the availability requirements?
   - Uptime percentage?
   - Maintenance windows?
4. What are the usability requirements?
   - Accessibility standards?
   - Browser support?
   - Mobile support?
5. What are the scalability needs?
   - Expected growth?
   - Peak load scenarios?

### About Acceptance Criteria
1. How will we know this requirement is satisfied?
2. What test scenarios must pass?
3. What would constitute a failure?
4. Who defines "done" for this requirement?

### Example Clarification Request
```markdown
## Clarification Needed: REQ-F-042 User Login

**Current Understanding**: "Users should be able to log in securely"

**Questions**:
1. **Authentication Method**: 
   - Username/password only?
   - Multi-factor authentication required?
   - Social login (Google, Microsoft) supported?

2. **Session Management**:
   - Session timeout duration?
   - "Remember me" functionality needed?
   - Concurrent session handling?

3. **Security Requirements**:
   - Password complexity rules?
   - Account lockout after failed attempts?
   - Password reset process?

4. **Error Handling**:
   - What feedback for invalid credentials?
   - How to handle account locked scenarios?

5. **Acceptance Criteria**:
   - Response time requirement?
   - Success/failure scenarios to test?

**Impact**: These details are needed to:
- Create complete, testable requirements
- Design proper security measures
- Write comprehensive acceptance tests
- Estimate implementation effort accurately
```

## 📊 Requirements Quality Checklist

Each requirement must be:
- [ ] **Complete** - Fully describes the capability
- [ ] **Correct** - Accurately represents stakeholder need
- [ ] **Consistent** - No conflicts with other requirements
- [ ] **Unambiguous** - Only one interpretation possible
- [ ] **Verifiable** - Can be tested/verified
- [ ] **Traceable** - Linked to stakeholder requirement
- [ ] **Feasible** - Technically and economically achievable
- [ ] **Necessary** - Required for success (YAGNI)
- [ ] **Prioritized** - Relative importance defined
- [ ] **Atomic** - Single, specific requirement

## 📊 Phase Entry Criteria

✅ Stakeholder Requirements Specification (StRS) approved  
✅ Stakeholders available for clarification  
✅ Business context understood  
✅ Technical feasibility assessed

## 📊 Phase Exit Criteria

✅ System Requirements Specification (SyRS) complete  
✅ All functional requirements documented with acceptance criteria  
✅ All non-functional requirements documented with metrics  
✅ Use cases written for key scenarios  
✅ User stories created with acceptance tests  
✅ Traceability matrix complete (REQ → StR)  
✅ Requirements reviewed and approved by stakeholders  
✅ Requirements baseline established  
✅ No unresolved conflicts or ambiguities  

## 🔗 Traceability

Establish complete traceability chain:
```
StR-XXX (Stakeholder Requirement)
  ↓
REQ-F-XXX (Functional Requirement)
REQ-NF-XXX (Non-Functional Requirement)
  ↓
UC-XXX (Use Case)
STORY-XXX (User Story)
  ↓
[Next Phase: Architecture - ARC-XXX]
```

## 📚 Standards References

- **ISO/IEC/IEEE 29148:2018** - Section 5.3 (System Requirements)
- **ISO/IEC/IEEE 12207:2017** - Section 6.4.2 (System Requirements Analysis)
- **Writing Effective Use Cases** - Alistair Cockburn
- **XP Practices** - User Stories, Acceptance Tests, YAGNI

## 🎯 Next Phase

Once this phase is complete, proceed to:
**Phase 03: Architecture Design** (`03-architecture/`)

---

**Remember**: Requirements describe WHAT the system must do, not HOW it will be implemented. Every requirement must be testable and traceable. When in doubt, ask clarifying questions!
