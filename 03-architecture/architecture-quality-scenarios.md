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

# Architecture Quality Attribute Scenarios

Quality Attribute (QA) scenarios make non-functional requirements concrete and testable. Each scenario follows the structured form recommended in architecture evaluation methods (e.g., ATAM) and ties directly to requirements.

## Scenario Template
```yaml
id: QA-SC-XXX
qualityAttribute: Performance | Availability | Security | Scalability | Maintainability | Reliability | Usability | Portability
source: [Actor/Event triggering the stimulus]
stimulus: [Condition precipitating the response]
stimulusEnvironment: [Normal | Peak Load | Degraded | Failure Mode]
artifact: [System | Component | Data Store | Interface]
response: [Desired architectural response]
responseMeasure: [Quantified metric / success criteria]
relatedRequirements:
  - REQ-NF-P-001
relatedADRs:
  - ADR-001
relatedViews:
  - logical
validationMethod: benchmark | test | inspection | simulation
status: draft | verified | at-risk
```

## Example Scenarios

### QA-SC-001 Performance - API Latency
```yaml
id: QA-SC-001
qualityAttribute: Performance
source: User via Web UI
stimulus: Submits a standard data retrieval request
stimulusEnvironment: Peak Load (1000 concurrent users)
artifact: API Gateway + Application Service
response: Returns requested data
responseMeasure: p95 < 200ms, p99 < 500ms
relatedRequirements:
  - REQ-NF-P-001
relatedADRs:
  - ADR-002
relatedViews:
  - logical
  - process
validationMethod: benchmark
status: draft
```

### QA-SC-002 Availability - Primary Database Failure
```yaml
id: QA-SC-002
qualityAttribute: Availability
source: Hardware failure in primary DB node
stimulus: Primary database becomes unreachable
stimulusEnvironment: Normal Operation
artifact: Data Persistence Layer
response: Automatic failover to standby
responseMeasure: RTO < 60s, RPO = 0
relatedRequirements:
  - REQ-NF-R-002
relatedADRs:
  - ADR-003
relatedViews:
  - deployment
  - data
validationMethod: chaos test
status: draft
```

### QA-SC-003 Security - Credential Stuffing Attack
```yaml
id: QA-SC-003
qualityAttribute: Security
source: Malicious actor
stimulus: Automated login attempts (500 req/s distributed across IPs)
stimulusEnvironment: Normal Operation
artifact: Authentication subsystem
response: Rate limiting, anomaly detection triggers, suspicious IPs blocked
responseMeasure: > 95% attacks blocked, < 1% false positives, no user data compromised
relatedRequirements:
  - REQ-NF-S-001
relatedADRs:
  - ADR-004
relatedViews:
  - security
validationMethod: security test
status: draft
```

## Coverage Matrix
| Scenario ID | Quality Attribute | Requirements | ADRs | Views | Validation Method | Status |
|-------------|-------------------|--------------|------|-------|-------------------|--------|
| QA-SC-001 | Performance | REQ-NF-P-001 | ADR-002 | logical, process | benchmark | draft |
| QA-SC-002 | Availability | REQ-NF-R-002 | ADR-003 | deployment, data | chaos test | draft |
| QA-SC-003 | Security | REQ-NF-S-001 | ADR-004 | security | security test | draft |

## Definition of Done
- At least one scenario per prioritized quality attribute
- Each scenario traces to at least one requirement
- Each scenario traces to at least one architecture view and ADR
- Each response measure is objectively testable
- Validation method defined
- Gaps identified for missing attributes (mark as TODO)

## Review Checklist
- [ ] Scenarios follow structured template
- [ ] Metrics are quantifiable
- [ ] No ambiguous adjectives ("fast", "secure") without metrics
- [ ] All critical quality attributes covered
- [ ] Traceability complete (Requirement ↔ Scenario ↔ ADR ↔ View)
