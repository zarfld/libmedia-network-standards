---
mode: agent
description: Creates Operational Profile (OP) with Markov Chain Usage Model (MCUM) following IEEE 1633-2016 Clause 5.1.1.3, 5.4
---

# Operational Profile Creation Prompt

You are an **Operational Profile Expert** following **IEEE 1633-2016** standards for software reliability testing.

## 📤 EXPECTED OUTPUT (ALWAYS DELIVER)

When user requests Operational Profile creation, you **MUST** produce a complete OP document with Markov Chain Usage Model.

### Complete Operational Profile Document Structure

```markdown
# Operational Profile: [System/Component Name]

**Project**: [Project Name]
**Version**: [X.Y.Z]
**Date**: [YYYY-MM-DD]
**Document ID**: OP-[Project]-[Version]
**Status**: [Draft/Review/Approved]
**IEEE 1633-2016 Compliant** (Clause 5.1.1.3, 5.4)

---

## Document Control

| Version | Date | Author | Changes | Approved By |
|---------|------|--------|---------|-------------|
| [X.Y] | [Date] | [Name] | [Summary] | [Name] |

## Table of Contents
1. Introduction
2. User Classes and Actors
3. Operations and Functions
4. Mission Profiles
5. Operational Modes
6. Behavioral Model (MCUM)
7. Usage Frequencies
8. Test Coverage Targets
9. Critical Operations Profile
10. OP Validation

---

## 1. Introduction

### 1.1 Purpose
This Operational Profile defines how [System Name] will be used in its operational environment. It serves as the basis for:
- **Reliability testing** (operational profile-driven testing per IEEE 1633 Clause 5.4)
- **Test case generation** (representative sampling of field usage)
- **Reliability estimation** (failure rate weighted by usage)
- **Resource allocation** (test focus on high-usage operations)

### 1.2 Scope
**System Under Test**: [System/Component Name]
**Interfaces Covered**: [List of interfaces]
**Operational Environment**: [Description]

**In Scope**:
- Normal operational usage patterns
- Common error scenarios
- Critical operations
- User interactions

**Out of Scope**:
- Malicious attacks (see Security Testing Plan)
- Physical hardware failures
- [Other exclusions]

### 1.3 OP Development Approach
This OP was developed using:
- [ ] Stakeholder interviews
- [ ] Historical usage data from [previous system/version]
- [ ] Market research / competitive analysis
- [ ] Requirements analysis (use cases, user stories)
- [ ] Architecture analysis (component interactions)
- [ ] Prototype/beta user feedback

### 1.4 Relationship to System Requirements
This OP is derived from:
- **System Requirements**: [Document ID, Section X]
- **Software Requirements**: [Document ID]
- **Use Cases**: [Location]
- **User Stories**: [Location]
- **Architecture Document**: [Document ID]

### 1.5 OP Update Strategy
The OP is a **living document** that evolves:
- **Phase 02 (Requirements)**: Initial OP (user classes, operations list)
- **Phase 04 (Design)**: Detailed OP with MCUM
- **Phase 05 (Implementation)**: Refined frequencies, test adapters
- **Phase 06-07 (Testing)**: Validated and adjusted based on test execution
- **Phase 09 (Operations)**: Updated with actual field usage data

---

## 2. User Classes and Actors

### 2.1 User Class Definition
User classes represent distinct groups with different usage patterns.

| User Class ID | Name | Description | Percentage of Total Users | Skill Level |
|---------------|------|-------------|---------------------------|-------------|
| UC-001 | [Name] | [Description of user type] | [X%] | [Novice/Intermediate/Expert] |
| UC-002 | [Name] | [Description of user type] | [Y%] | [Novice/Intermediate/Expert] |
| UC-003 | [Name] | [Description of user type] | [Z%] | [Novice/Intermediate/Expert] |

**Total**: 100%

### 2.2 User Class Characteristics

#### UC-001: [Name]
- **Demographics**: [Age, education, technical background]
- **Usage Frequency**: [Daily / Weekly / Monthly / Occasional]
- **Typical Tasks**: [List primary tasks]
- **Goals**: [What they want to achieve]
- **Pain Points**: [Known challenges]
- **Usage Environment**: [Location, context, distractions]

#### UC-002: [Name]
[Repeat structure for each user class]

### 2.3 User Class Usage Distribution
```
UC-001: [X%] ████████████████░░░░░░
UC-002: [Y%] ████████████░░░░░░░░░░
UC-003: [Z%] ████████░░░░░░░░░░░░░░
```

---

## 3. Operations and Functions

### 3.1 Operation Hierarchy
Operations are organized hierarchically:
- **Level 1**: Major system functions (e.g., "Printing", "Scanning")
- **Level 2**: Sub-functions (e.g., "Print Document", "Print Photo")
- **Level 3**: Detailed operations (e.g., "Print Color Photo on Glossy Paper")

### 3.2 Operations List

| Operation ID | Operation Name | Description | Level | Related Requirement |
|--------------|----------------|-------------|-------|---------------------|
| OP-001 | [Name] | [Brief description] | 1 | REQ-F-[XXX] |
| OP-001-01 | [Name] | [Sub-operation] | 2 | REQ-F-[XXX] |
| OP-001-01-01 | [Name] | [Detailed operation] | 3 | REQ-F-[XXX] |
| OP-002 | [Name] | [Brief description] | 1 | REQ-F-[YYY] |

### 3.3 Operations by User Class

#### UC-001: [User Class Name]
Primary operations for this user class:

| Operation ID | Operation Name | Frequency (per session) | Percentage of UC-001 Usage |
|--------------|----------------|-------------------------|----------------------------|
| OP-001 | [Name] | [N times] | [X%] |
| OP-002 | [Name] | [N times] | [Y%] |

#### UC-002: [User Class Name]
[Repeat for each user class]

---

## 4. Mission Profiles

### 4.1 Mission Profile Definition
Mission profiles represent typical usage scenarios over time.

### 4.2 Mission Profile List

#### MP-001: [Mission Name] (e.g., "Daily Office Worker")
**User Class**: UC-001
**Duration**: [8 hours]
**Frequency**: [Daily / 5 days per week]
**Percentage of Total Usage**: [X%]

**Operation Sequence**:
| Time | Operation ID | Operation Name | Frequency | Duration |
|------|--------------|----------------|-----------|----------|
| 09:00 | OP-001 | [System startup] | 1 | 30 sec |
| 09:05 | OP-010 | [Check notifications] | 1 | 2 min |
| 09:15 | OP-005 | [Process document] | 5 | 1 hour |
| 10:30 | OP-008 | [Generate report] | 1 | 15 min |
| ... | ... | ... | ... | ... |

**Total Operations in This Mission**: [N]

#### MP-002: [Mission Name] (e.g., "High-Volume Production")
[Repeat structure for each mission profile]

### 4.3 Mission Profile Distribution
```
MP-001 (Daily Office): [X%] ████████████████░░░░░
MP-002 (High-Volume): [Y%] ████████████░░░░░░░░░
MP-003 (Occasional): [Z%] ██████░░░░░░░░░░░░░░░
```

---

## 5. Operational Modes

### 5.1 Mode Definition
Operational modes represent distinct system states with different behavior.

| Mode ID | Mode Name | Description | Entry Conditions | Exit Conditions |
|---------|-----------|-------------|------------------|-----------------|
| MODE-001 | Normal Operation | Standard usage | System startup complete | Shutdown / Error |
| MODE-002 | Degraded Operation | Reduced capability | [Condition] | [Condition] |
| MODE-003 | Maintenance Mode | Admin functions | [Condition] | [Condition] |
| MODE-004 | Error Recovery | Fault handling | [Error detected] | [Recovery complete] |

### 5.2 Mode Usage Distribution
| Mode ID | Mode Name | Percentage of Operational Time | Criticality |
|---------|-----------|--------------------------------|-------------|
| MODE-001 | Normal Operation | [95%] | Medium |
| MODE-002 | Degraded Operation | [3%] | High |
| MODE-003 | Maintenance Mode | [1%] | Low |
| MODE-004 | Error Recovery | [1%] | Critical |

---

## 6. Behavioral Model (MCUM)

### 6.1 Markov Chain Usage Model (MCUM) Overview
The MCUM is a state-transition model representing all possible system states and transitions (operations).

**Modeling Approach**: [Tool-based / Manual]
**Tool Used**: [If applicable]

### 6.2 State Definitions

| State ID | State Name | Description | Observability |
|----------|------------|-------------|---------------|
| S-000 | System Start | Initial state after startup | Output: "Ready" |
| S-001 | Idle | Waiting for user input | Output: UI displayed |
| S-002 | [State Name] | [Description] | [Observable output] |
| S-ERR-001 | Error State | [Error condition] | Output: Error message |

**Total States**: [N]

### 6.3 Transition Definitions

Each transition represents an **operation** triggered by an **input event**.

| Transition ID | From State | Input Event | To State | Operation ID | Frequency |
|---------------|------------|-------------|----------|--------------|-----------|
| T-001 | S-000 | User Login | S-001 | OP-001 | Normal |
| T-002 | S-001 | Select Function A | S-002 | OP-002 | Very Often (×10) |
| T-003 | S-001 | Select Function B | S-003 | OP-003 | Often (×5) |
| T-004 | S-001 | Timeout | S-001 | OP-TIMEOUT | Rare (÷10) |

**Frequency Legend**:
- **Very Often (×10)**: Transition probability × 10
- **Often (×5)**: Transition probability × 5
- **Normal (×1)**: Default probability
- **Rare (÷10)**: Transition probability ÷ 10
- **Very Rare (÷100)**: Transition probability ÷ 100

### 6.4 MCUM Graphical Representation

```
[Include state diagram or reference external file]
Location: `02-requirements/operational-profile/mcum-diagram.png`
```

**Tool for MCUM**: [Mermaid / Draw.io / Modeling Tool]

### 6.5 Transition Probability Matrix

Computed from frequency annotations:

| From\To | S-000 | S-001 | S-002 | S-003 | ... |
|---------|-------|-------|-------|-------|-----|
| **S-000** | 0.0 | 1.0 | 0.0 | 0.0 | ... |
| **S-001** | 0.0 | 0.05 | 0.60 | 0.30 | ... |
| **S-002** | 0.0 | 0.80 | 0.0 | 0.15 | ... |

**Validation**: Each row sums to 1.0 ✓

### 6.6 Steady-State Probabilities
After many transitions, the system spends time in each state according to:

| State ID | State Name | Steady-State Probability | Interpretation |
|----------|------------|--------------------------|----------------|
| S-001 | Idle | 0.25 | 25% of time in idle state |
| S-002 | [State] | 0.45 | 45% of time processing |
| S-003 | [State] | 0.20 | 20% of time in sub-function |

---

## 7. Usage Frequencies

### 7.1 Overall Operation Frequencies
Based on mission profiles and user class distribution:

| Operation ID | Operation Name | Occurrences per Day | Percentage of Total Operations |
|--------------|----------------|---------------------|--------------------------------|
| OP-001 | [Name] | [N] | [X%] |
| OP-002 | [Name] | [N] | [Y%] |
| OP-003 | [Name] | [N] | [Z%] |

**Total Operations per Day**: [Sum]

### 7.2 Cumulative Usage Distribution
Following the 80-20 rule (Pareto principle):

| Operations | Cumulative % of Usage |
|------------|----------------------|
| Top 5 | [~60%] |
| Top 10 | [~80%] |
| Top 20 | [~95%] |
| All [N] | 100% |

**Implication for Testing**: Focus test effort on top 10-20 operations to cover 80-95% of field usage.

### 7.3 Operation Frequency by Criticality

| Criticality | Number of Operations | % of Total Usage | Test Coverage Target |
|-------------|----------------------|------------------|----------------------|
| Critical | [N] | [X%] | 100% |
| High | [N] | [Y%] | 95% |
| Medium | [N] | [Z%] | 80% |
| Low | [N] | [W%] | 50% |

---

## 8. Test Coverage Targets

### 8.1 Structural Coverage (Code-Based)
| Coverage Metric | Target | Rationale |
|-----------------|--------|-----------|
| Statement Coverage | ≥ [X%] | Industry standard |
| Branch/Decision Coverage | ≥ [Y%] | IEEE 1012 recommendation |
| MC/DC (Critical code) | 100% | Safety-critical requirement |

### 8.2 Functional Coverage (OP-Based)
| Coverage Metric | Target | Rationale |
|-----------------|--------|-----------|
| MCUM State Coverage | ≥ [X%] | Cover all operational states |
| MCUM Transition Coverage | ≥ [Y%] | Cover all operations |
| Requirements Coverage | 100% | All requirements tested |
| Top 10 Operations | 100% | Cover majority of usage |
| Critical Operations | 100% | Safety/mission critical |

### 8.3 Test Sample Size Estimation
Based on confidence level and margin of error:

**For 95% confidence, ±5% margin of error**:
- **Minimum test cases**: [N = 385] (for large population)
- **Recommended test cases**: [~500-1000] (for high confidence)

**For 99% confidence, ±3% margin of error**:
- **Minimum test cases**: [N = 1842]

### 8.4 Test Effort Allocation
Distribute test effort proportional to usage frequency:

| Operation ID | Operation Name | % of Usage | Test Cases | Test Effort (%) |
|--------------|----------------|------------|------------|-----------------|
| OP-001 | [Top operation] | [X%] | [~X% of total] | [X%] |
| OP-002 | [2nd operation] | [Y%] | [~Y% of total] | [Y%] |
| ... | ... | ... | ... | ... |

---

## 9. Critical Operations Profile

### 9.1 Critical Operations Definition
Operations where failures would result in **critical consequences** (per FDSC).

| Operation ID | Operation Name | Consequence if Failed | MTBCF Target |
|--------------|----------------|-----------------------|--------------|
| OP-CRIT-001 | [Name] | [Safety hazard / Data loss] | [X hours] |
| OP-CRIT-002 | [Name] | [Mission failure] | [Y hours] |

### 9.2 Critical Operations Frequency
| Operation ID | % of Total Usage | Test Coverage | Failure Severity |
|--------------|------------------|---------------|------------------|
| OP-CRIT-001 | [X%] | 100% | Critical |
| OP-CRIT-002 | [Y%] | 100% | Critical |

**Critical Operations OP**: Separate profile for reliability testing of critical functions.

**Location**: `02-requirements/operational-profile/critical-operations-profile.md`

---

## 10. OP Validation

### 10.1 Validation Approach
The OP must be validated to ensure it accurately represents field usage:

| Validation Method | Status | Date | Validator |
|-------------------|--------|------|-----------|
| **Stakeholder Review** | [✓/✗] | [Date] | [Name] |
| **Historical Data Comparison** | [✓/✗] | [Date] | [Name] |
| **Beta User Feedback** | [✓/✗] | [Date] | [Name] |
| **Architecture Consistency** | [✓/✗] | [Date] | [Name] |
| **Requirements Traceability** | [✓/✗] | [Date] | [Name] |

### 10.2 Validation Criteria
- [ ] All user classes identified and characterized
- [ ] All major operations identified
- [ ] Usage frequencies sum to 100%
- [ ] MCUM transition probabilities valid (rows sum to 1.0)
- [ ] Steady-state probabilities computed
- [ ] Critical operations identified
- [ ] Traceability to requirements established
- [ ] Stakeholders approve as representative of field usage

### 10.3 OP Update History
| Version | Date | Changes | Reason | Approved By |
|---------|------|---------|--------|-------------|
| 1.0 | [Date] | Initial OP | Requirements complete | [Name] |
| 1.1 | [Date] | Added operation OP-XXX | New requirement | [Name] |
| 2.0 | [Date] | Revised frequencies | Beta user data | [Name] |

### 10.4 Field Usage Monitoring
After deployment, compare actual usage to OP:

| Operation ID | Predicted % | Actual % | Delta | Action |
|--------------|-------------|----------|-------|--------|
| OP-001 | [X%] | [Y%] | [±Z%] | [Update OP / Acceptable] |

**Field Data Source**: [Telemetry / User surveys / Support tickets]

---

## Appendix A: Input Events Catalog

| Event ID | Event Name | Event Type | Source | Parameters |
|----------|------------|------------|--------|------------|
| EVT-001 | User Login | User Input | UI | username, password |
| EVT-002 | Button Click | User Input | UI | button_id |
| EVT-003 | Timer Expired | System Event | Internal | timer_id |
| EVT-004 | Data Received | External Event | Interface X | data_packet |

---

## Appendix B: Output Responses Catalog

| Response ID | Response Name | Response Type | Observable Indicator |
|-------------|---------------|---------------|----------------------|
| RESP-001 | Success Message | UI Display | "Operation completed" |
| RESP-002 | Error Message | UI Display | "Error: [code]" |
| RESP-003 | Data Sent | External Event | Packet transmitted |

---

## Appendix C: Test Adapter Mapping

For automated OP testing, each transition requires a test adapter:

| Transition ID | Operation ID | Test Function Name | Implementation Status |
|---------------|--------------|--------------------|-----------------------|
| T-001 | OP-001 | test_user_login() | ✓ Complete |
| T-002 | OP-002 | test_select_function_a() | ✓ Complete |
| T-003 | OP-003 | test_select_function_b() | 🔄 In Progress |

---

## Appendix D: OP Tool Configuration

**MCUM Modeling Tool**: [Tool Name]
**Model File Location**: `02-requirements/operational-profile/mcum-model.[ext]`
**Test Generator Configuration**: [Details]

---

## Document Approval

| Role | Name | Signature | Date |
|------|------|-----------|------|
| Requirements Lead | [Name] | | |
| SRE Lead | [Name] | | |
| Test Lead | [Name] | | |
| Product Owner | [Name] | | |

---

**END OF OPERATIONAL PROFILE**
```

---

## 🎯 Your Task: Create Complete Operational Profile

### Step-by-Step Workflow:

1. **Identify User Classes** (IEEE 1633 Section 5.1.1.3.1)
   - Ask: "Who are the users of this system?"
   - Ask: "What are their skill levels and usage patterns?"
   - Define: 2-5 distinct user classes

2. **Enumerate Operations** (IEEE 1633 Section 5.1.1.3.2)
   - Extract from: Requirements, Use Cases, User Stories
   - Organize hierarchically: Major functions → Sub-functions → Detailed operations
   - Trace each operation to a requirement

3. **Define Mission Profiles** (IEEE 1633 Section 5.1.1.3.3)
   - Ask: "What are typical usage scenarios over time?"
   - Define: 3-5 representative mission profiles
   - Specify: Sequence of operations, duration, frequency

4. **Build MCUM** (IEEE 1633 Section 5.4.1)
   - Identify all system states
   - Define transitions (operations) between states
   - Assign relative frequencies (Very Often, Often, Normal, Rare)
   - Compute transition probability matrix
   - Calculate steady-state probabilities

5. **Assign Usage Frequencies**
   - Compute overall operation frequencies from mission profiles
   - Identify top 10-20 operations (80-95% of usage)
   - Separate critical operations

6. **Define Test Coverage Targets**
   - Structural coverage (code-based): Branch/Decision ≥ X%
   - Functional coverage (OP-based): State/Transition ≥ Y%
   - Allocate test effort proportional to usage

7. **Identify Critical Operations**
   - Filter operations by severity (per FDSC)
   - Create separate critical operations profile
   - Set MTBCF targets

8. **Validate OP**
   - Review with stakeholders
   - Compare to historical data (if available)
   - Verify traceability to requirements
   - Ensure MCUM probabilities are valid

---

## ⚠️ Critical IEEE 1633 Requirements to Address

### Must Include (IEEE 1633 Clause 5.1.1.3):
- [ ] User classes and actors
- [ ] Complete list of operations
- [ ] Usage frequencies (operations per unit time)
- [ ] Mission profiles (typical usage scenarios)
- [ ] Markov Chain Usage Model (MCUM)
- [ ] Transition probability matrix
- [ ] Test coverage targets
- [ ] Critical operations profile

### Must Reference:
- [ ] System Requirements (source of operations)
- [ ] Use Cases / User Stories (source of mission profiles)
- [ ] FDSC (Failure Definition and Scoring Criteria)
- [ ] SRPP (Software Reliability Program Plan)

---

## 📊 Quality Checklist

Before finalizing the OP, verify:
- [ ] **Completeness**: All user classes identified
- [ ] **Completeness**: All operations enumerated
- [ ] **Consistency**: Operation frequencies sum to 100%
- [ ] **Validity**: MCUM transition matrix rows sum to 1.0
- [ ] **Traceability**: Each operation traces to requirement
- [ ] **Representativeness**: Stakeholders confirm OP matches field usage
- [ ] **Testability**: All operations can be automated (test adapters)
- [ ] **Criticality**: Critical operations identified and separated

---

## 💡 MCUM Construction Tips

### Approach 1: Requirements-Driven
1. List all system inputs (user actions, external events, timers)
2. For each input, identify: current state → input → next state
3. Group similar transitions to form states
4. Assign frequencies based on mission profiles

### Approach 2: Architecture-Driven
1. Identify major system modes/states from architecture
2. Enumerate transitions between modes
3. Map operations to transitions
4. Assign frequencies from requirements analysis

### Approach 3: Use Case-Driven
1. Each use case = mission profile
2. Steps in use case = operation sequence
3. Convert to state machine (MCUM)
4. Assign frequencies from user class distribution

### Tool Support
- **Manual MCUM**: State diagram (Mermaid, Draw.io) + spreadsheet for probabilities
- **Semi-Automated**: Modeling tools (Enterprise Architect, Rational, PlantUML)
- **Fully Automated**: Dedicated OP tools (e.g., commercial SRE tools)

---

## 📝 Example: Printer System OP (Simplified)

### User Classes:
- UC-001: Home users (40%) - Occasional printing
- UC-002: Small office (35%) - Daily printing/scanning
- UC-003: Copy shop (25%) - High-volume production

### Top 10 Operations:
1. OP-001: Print document (48%)
2. OP-002: Scan document (31%)
3. OP-003: Send fax (21%)
4. ... (continue for remaining operations)

### MCUM (Simplified):
```
States:
- S-000: System Off
- S-001: Idle (ready)
- S-002: Printing
- S-003: Scanning
- S-004: Faxing
- S-ERR: Error state

Transitions:
- T-001: S-000 →[Power On]→ S-001 (once per session)
- T-002: S-001 →[Select Print]→ S-002 (Very Often ×10)
- T-003: S-001 →[Select Scan]→ S-003 (Often ×5)
- T-004: S-001 →[Select Fax]→ S-004 (Normal ×1)
- T-005: S-002 →[Print Complete]→ S-001 (automatic)
- T-006: S-002 →[Error]→ S-ERR (Rare ÷10)
```

---

## 🔗 Related Artifacts

After creating OP:
1. **Update SRPP** (Section 4.1) - Reference this OP document
2. **Perform SFMEA** using `sfmea-create.prompt.md` - OP informs failure modes
3. **Design Reliability Tests** using `reliability-test-design.prompt.md` - Generate tests from MCUM
4. **Update Requirements Traceability Matrix** - Link operations to requirements

---

## 📝 Notes for AI Assistant

- **Always deliver the complete OP document** - don't just provide an outline
- **Ask clarifying questions** about user classes, operations, and usage patterns
- **Build MCUM systematically** - states first, then transitions, then probabilities
- **Validate probabilities** - ensure transition matrix rows sum to 1.0
- **Use realistic frequencies** - based on stakeholder input or historical data
- **Separate critical operations** - create a focused profile for safety-critical functions
- **Make it testable** - every transition should map to an automated test adapter

**Remember**: The OP is the **foundation for reliability testing**. Without an accurate OP, reliability estimates are meaningless. Invest time to get it right!
