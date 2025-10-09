# Architecture Review Checklist

Use before exiting Phase 03. Based on ISO/IEC/IEEE 42010, IEEE 1016, and quality attribute scenario best practices (ATAM influences).

## Metadata
```yaml
checklistId: ARCH-CHK-001
version: 1.0.0
date: YYYY-MM-DD
reviewers:
  - Architect
  - Developer Lead
  - Ops Lead
  - Security Lead
```

## Core Compliance
| Item | Criteria | Status | Notes |
|------|----------|--------|-------|
| Stakeholders Identified | List of all stakeholders & concerns |  |  |
| Viewpoints Defined | Each concern mapped to viewpoint |  |  |
| Views Produced | Context, Logical, Deployment + required others |  |  |
| Decision Records | All significant decisions captured as ADRs |  |  |
| Rationale Documented | ADRs include rationale & alternatives |  |  |
| Traceability Established | Requirements ↔ Components ↔ ADRs ↔ Scenarios |  |  |

## Quality Attributes
| Item | Criteria | Status | Notes |
|------|----------|--------|-------|
| QA Scenarios Defined | At least 1 per prioritized attribute |  |  |
| Measurable Metrics | Quantitative response measures |  |  |
| Validation Methods | Benchmark / test / inspection documented |  |  |
| Coverage Adequate | No critical attribute missing scenarios |  |  |

## Risks & Tradeoffs
| Item | Criteria | Status | Notes |
|------|----------|--------|-------|
| Risks Identified | Risk log created |  |  |
| Mitigations Planned | Each high risk has mitigation |  |  |
| Tradeoffs Explicit | Performance vs. cost, etc., documented |  |  |
| Sensitivity Points | Key parameters identified |  |  |

## Cross-Cutting Concerns
| Item | Criteria | Status | Notes |
|------|----------|--------|-------|
| Security Architecture | AuthN/Z, threat modeling basics |  |  |
| Performance Architecture | Caching, scaling strategy |  |  |
| Observability | Metrics, logging, tracing approach |  |  |
| Deployment Model | Environments & topology defined |  |  |
| Data Strategy | Schema, retention, consistency |  |  |

## XP Alignment
| Item | Criteria | Status | Notes |
|------|----------|--------|-------|
| Simplicity | No unnecessary components |  |  |
| Testability | Components support unit/integration tests |  |  |
| Refactorability | Low coupling, clear boundaries |  |  |
| Evolution | Architecture supports incremental delivery |  |  |

## Outcomes
```yaml
approved: false
blockers: []
actionItems:
  - id: ACT-001
    description: Add missing security scenario
    owner: Security Lead
    due: YYYY-MM-DD
```

## Sign-off
| Role | Name | Signature | Date |
|------|------|----------|------|
| Architect |  |  |  |
| Product Owner |  |  |  |
| Dev Lead |  |  |  |
| Security Lead |  |  |  |
