# Phase 01: Stakeholder Requirements Definition

**Standards**: ISO/IEC/IEEE 29148:2018 (Stakeholder Requirements), ISO/IEC/IEEE 12207:2017  
**XP Integration**: Planning Game, Customer Involvement, User Stories Foundation

## 🎯 Phase Objectives

1. Identify and document all stakeholders and their concerns
2. Elicit stakeholder needs and expectations
3. Define business context and constraints
4. Establish success criteria and acceptance measures
5. Create foundation for system requirements

## 📂 Working Directory Context

```yaml
applyTo:
  - "01-stakeholder-requirements/**/*.md"
  - "01-stakeholder-requirements/**/stakeholders/**"
  - "01-stakeholder-requirements/**/business-context/**"
  - "01-stakeholder-requirements/**/templates/**"
```

## 📋 ISO/IEC/IEEE 29148:2018 Compliance

### Stakeholder Requirements Process Activities

1. **Stakeholder Identification**
   - Define all stakeholder classes
   - Identify representatives from each class
   - Document stakeholder roles and responsibilities
   - Map stakeholder concerns and interests

2. **Requirements Elicitation**
   - Conduct stakeholder interviews
   - Facilitate workshops and brainstorming sessions
   - Review existing documentation
   - Analyze competitor solutions
   - Document user pain points and needs

3. **Requirements Analysis**
   - Identify conflicting requirements
   - Prioritize stakeholder needs
   - Analyze feasibility and constraints
   - Define acceptance criteria

4. **Requirements Documentation**
   - Create Stakeholder Requirements Specification (StRS)
   - Document business context
   - Define scope and boundaries
   - Document assumptions and constraints

## 🎨 XP Practices for This Phase

### Planning Game
- **Exploration Phase**: Understand business value
- **Commitment Phase**: Establish iteration scope
- **Steering Phase**: Track progress and adjust

### Customer Involvement
- Embed customer representative in team
- Daily customer feedback
- Customer-defined acceptance tests
- Customer prioritization of features

### User Story Foundation
Start capturing requirements as user stories:
```markdown
As a [stakeholder role]
I want [goal/desire]
So that [benefit/value]
```

## 📝 Required Deliverables

### 1. Stakeholder Register
**Location**: `stakeholders/stakeholder-register.md`

```markdown
# Stakeholder Register

## Stakeholder Classes

### [Class Name] (e.g., End Users, Operations Team, Executives)
- **Description**: [Role description]
- **Representatives**: [Names/Roles]
- **Concerns**: [Key interests and concerns]
- **Influence**: [High/Medium/Low]
- **Interest Level**: [High/Medium/Low]
- **Communication Needs**: [How and when to engage]
```

### 2. Business Context Document
**Location**: `business-context/business-context.md`

Must include (per ISO/IEC/IEEE 29148):
- Business opportunity or problem
- Business goals and objectives
- Success criteria and measures
- Market analysis
- Competitive landscape
- Business constraints
- Regulatory requirements
- Standards compliance needs

### 3. Stakeholder Requirements Specification (StRS)
**Location**: `stakeholder-requirements-specification.md`

Structure per ISO/IEC/IEEE 29148:

```markdown
# Stakeholder Requirements Specification

## 1. Introduction
### 1.1 Purpose
### 1.2 Scope
### 1.3 Definitions, Acronyms, Abbreviations
### 1.4 References
### 1.5 Overview

## 2. Stakeholder Requirements
### 2.1 [Requirement Category 1]
#### StR-001: [Requirement Title]
- **Source**: [Stakeholder class]
- **Priority**: Critical/High/Medium/Low
- **Rationale**: [Why this requirement exists]
- **Acceptance Criteria**: [How to verify]
- **Dependencies**: [Related requirements]
- **Constraints**: [Limitations]

### 2.2 [Requirement Category 2]
...

## 3. Business Context
### 3.1 Business Opportunity
### 3.2 Business Goals
### 3.3 Success Criteria

## 4. Assumptions and Constraints
### 4.1 Assumptions
### 4.2 Constraints
### 4.3 Dependencies

## 5. Acceptance Criteria
```

### 4. Initial User Stories
**Location**: `user-stories/`

Create high-level epic stories:
```markdown
# Epic: [Epic Name]

## User Story
As a [stakeholder role]
I want [goal]
So that [business value]

## Business Value
[Why this matters - ROI, competitive advantage, etc.]

## Acceptance Criteria
Given [context]
When [action]
Then [expected result]

## Constraints
- [Technical constraints]
- [Business constraints]

## Priority: [Critical/High/Medium/Low]
## Estimated Value Points: [1-100]
```

## 🚨 Critical Requirements for This Phase

### Always Do
✅ Interview all stakeholder classes  
✅ Document conflicting requirements  
✅ Prioritize with customer involvement  
✅ Define measurable acceptance criteria  
✅ Validate requirements with stakeholders  
✅ Document assumptions explicitly  
✅ Identify constraints early  
✅ Create traceable requirement IDs (StR-XXX)

### Never Do
❌ Proceed without stakeholder validation  
❌ Mix stakeholder requirements with system requirements  
❌ Skip non-functional requirements  
❌ Ignore conflicting requirements  
❌ Assume implicit requirements  
❌ Define technical solutions (that comes later)  
❌ Skip business context documentation

## 🔍 Clarifying Questions to Ask

When gathering stakeholder requirements, always ask:

### About the Business
1. What business problem are we solving?
2. What are the measurable business goals?
3. What is the expected ROI or business value?
4. What happens if we don't build this?
5. Who are the competitors, and how do they solve this?

### About Users/Stakeholders
1. Who are all the stakeholders (not just users)?
2. What are each stakeholder's goals and pain points?
3. What are the most critical needs vs. nice-to-haves?
4. Are there conflicting stakeholder needs?
5. Who makes final priority decisions?

### About Constraints
1. What is the budget and timeline?
2. Are there regulatory or compliance requirements?
3. What are the technical platform constraints?
4. Are there integration requirements with existing systems?
5. What are the security and privacy requirements?

### About Success
1. How will we measure success?
2. What are the acceptance criteria for the project?
3. What would make stakeholders consider this a failure?
4. What are the key performance indicators (KPIs)?

### Example Clarification Request
```markdown
## Clarification Needed: [Topic]

**Context**: You mentioned that "[stakeholder statement]"

**Questions**:
1. Can you elaborate on the specific goals or outcomes you expect?
2. Are there any constraints or limitations we should know about?
3. How will we know when this requirement is satisfied?
4. What is the priority of this requirement compared to others?
5. Are there regulatory or compliance aspects to consider?

**Why This Matters**: This information will help us:
- Define clear, testable requirements
- Prioritize development work
- Ensure compliance with standards
- Avoid rework later in the lifecycle
```

## 📊 Phase Entry Criteria

✅ Project charter approved  
✅ Initial stakeholders identified  
✅ Resources allocated  
✅ Business opportunity defined

## 📊 Phase Exit Criteria

✅ All stakeholder classes identified and documented  
✅ Stakeholder Requirements Specification (StRS) completed  
✅ Business context documented  
✅ Requirements reviewed and approved by stakeholders  
✅ Conflicts resolved or documented  
✅ Priorities established  
✅ Acceptance criteria defined  
✅ Traceability IDs assigned (StR-XXX format)  
✅ Baseline established for requirements

## 🔗 Traceability

Establish forward traceability:
```
StR-XXX (Stakeholder Requirement)
  ↓
[Next Phase: System Requirements - REQ-XXX]
```

## 📚 Standards References

- **ISO/IEC/IEEE 29148:2018** - Section 5.2 (Stakeholder Requirements Process)
- **ISO/IEC/IEEE 12207:2017** - Section 6.4.1 (Stakeholder Requirements Definition)
- **XP Practices** - Planning Game, Customer Involvement

## 🎯 Next Phase

Once this phase is complete, proceed to:
**Phase 02: Requirements Analysis & Specification** (`02-requirements/`)

---

**Remember**: Stakeholder requirements describe WHAT stakeholders need and WHY. They should be solution-independent. Technical HOW comes in later phases.
