# Architecture Views Overview

This folder contains architecture views conforming to defined viewpoints (ISO/IEC/IEEE 42010). Use multiple, consistent views to address stakeholder concerns and enable traceability.

## Viewpoint → View Mapping
| Viewpoint | Purpose / Concerns | Notation | Expected Artifacts |
|-----------|--------------------|----------|--------------------|
| Context | External actors, systems, boundaries | C4 Level 1, UML Context | `context.md`, `context.mmd` |
| Logical | Functional decomposition, major components | C4 Level 2/3, UML Component/Class | `logical.md` |
| Process / Runtime | Concurrency, interactions, sequencing | Sequence, Activity, State | `process.md` |
| Development | Module/package/layer organization | UML Package, Layer diagrams | `development.md` |
| Data / Information | Data entities, flows, storage, lifecycle | ERD, DFD, UML Class | `data.md` |
| Deployment / Physical | Infrastructure nodes, network topology | C4 Deployment, UML Deployment | `deployment.md` |
| Security | Trust boundaries, controls, threat mitigations | Data Flow, Abuse case | `security.md` |
| Operations / Observability | Monitoring, logging, metrics, alerts | Tables, Flow, Sequence | `operations.md` |
| Quality Attribute Scenarios | Concrete measurable QA scenarios | Scenario template | `quality-scenarios.md` |

## Minimum Required Set (Definition of Done)
- Context
- Logical
- Deployment
- Data (if persistent data)
- At least 3 quality attribute scenarios (performance, availability, security)

## View Structure Template
Each view file should contain:
```markdown
# [View Name] View

## Purpose
Why this view exists and which concerns it addresses.

## Primary Stakeholders
| Stakeholder | Concern |
|------------|---------|
| Developer | Maintainability |
| Operations | Deployability |

## Model
(Mermaid / PlantUML diagram + description)

## Elements & Responsibilities
| ID | Element | Responsibility | Interfaces | Related Requirements |
|----|---------|---------------|-----------|----------------------|
| ARC-C-001 | UserService | Manage users | IUserService | REQ-F-001 |

## Design Decisions Referenced
- ADR-001 User service pattern

## Quality Attribute Impact
Brief explanation of how this view contributes to key QAs (e.g., performance via caching component). 
```

## Consistency Rules
- IDs: Use prefixes (ARC-C-, ARC-P-, INT-, DATA-, DEP-) for traceability.
- Every element listed must appear in at least one other view (except context-level external actors).
- Each interface must map to an implementing component.
- No ambiguous element names (append suffix Service/Component/Repo as needed).

## Traceability Guidance
Maintain a matrix (see `../architecture-quality-scenarios.md`) linking:
```
Requirement → ADR(s) → Component(s)/Element(s) → View(s) → Test(s)
```

## Validation Checklist
- [ ] All mandatory views present
- [ ] Diagrams render successfully
- [ ] Elements have unique IDs
- [ ] Cross-view consistency verified
- [ ] References to ADRs valid
- [ ] Quality attribute scenarios referenced
