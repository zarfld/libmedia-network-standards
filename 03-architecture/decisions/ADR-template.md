---
specType: architecture
standard: 42010
phase: 03-architecture
version: 1.0.0
author: Architecture Team
date: "2025-10-03"
status: approved
traceability:
  requirements:
    - REQ-F-001
    - REQ-NF-001
---

# ADR-TEMPLATE: [Decision Title]

> Use this template for each significant architectural decision. Based on Michael Nygard ADR format and ISO/IEC/IEEE 42010 rationale requirements.

## Metadata
```yaml
adrId: ADR-XXX
status: proposed # proposed | accepted | deprecated | superseded
relatedRequirements:
  - REQ-F-001
  - REQ-NF-P-001
relatedComponents:
  - ARC-C-001
supersedes: []
supersededBy: null
author: [Name]
date: YYYY-MM-DD
reviewers: []
```

## Context
Describe the architectural concern, drivers (quality attributes, constraints), and forces in conflict. Reference stakeholder concerns and specific requirements (ISO/IEC/IEEE 29148 linkage).

## Decision
Clearly state the decision in active voice ("We will ...").

## Status
Rationale for current status; if deprecated or superseded, explain why.

## Rationale
Explain why this option best addresses the forces. Include trade-off analysis (performance vs. complexity, etc.).

## Considered Alternatives
| Alternative | Summary | Pros | Cons | Reason Not Chosen |
|------------|---------|------|------|-------------------|
| Option A | ... | ... | ... | ... |
| Option B | ... | ... | ... | ... |

## Consequences
### Positive
- Benefit 1
- Benefit 2

### Negative / Liabilities
- Drawback 1 (mitigation strategy)
- Drawback 2

### Neutral / Follow-ups
- Operational change required
- Additional monitoring needed

## Quality Attribute Impact Matrix
| Quality Attribute | Impact (+/−/0) | Notes |
|-------------------|----------------|-------|
| Performance | + | Reduces latency by caching layer |
| Scalability | + | Enables horizontal scaling |
| Security | 0 | No change |
| Maintainability | − | Added complexity via abstraction |

## Risks
| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|-----------|
| Added latency through proxy | Medium | Medium | Performance testing & tuning |

## Compliance Mapping
| Standard Clause | How Addressed |
|-----------------|---------------|
| ISO 42010 §5.8 (Rationale) | Rationale section documents reasons |
| IEEE 1016 (Design Decisions) | Decision + Alternatives sections |

## Implementation Notes
High-level steps, migration approach, feature flags, rollout & rollback strategy.

## Validation Plan
Describe how to verify the decision delivers intended outcomes (benchmarks, canary release, chaos test, etc.).

## References
- Links to related ADRs
- External references / benchmarks
- Issue tracker discussion
