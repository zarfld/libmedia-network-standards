---
specType: stakeholder-requirements
phase: 01-stakeholder-requirements
version: 0.1.0
author: core-team
date: 2025-11-07
status: draft
standard: 29148
---
# Stakeholder Requirements (ISO/IEC/IEEE 29148)

## Purpose
Capture stakeholder needs for the multi‑standard, platform‑agnostic media networking library to enable traceability into formal system/software requirements (Phase 02), design (Phases 03–04), implementation and V&V.

## Stakeholder Summary
| ID | Stakeholder Type | Role / Description | Primary Needs | Open Questions / Missing Info |
|----|------------------|--------------------|---------------|--------------------------------|
| StR-001 | Makers & Developers | Integrate library in diverse apps (embedded + desktop) | Simple, stable, version‑abstracted APIs; clear docs; runnable examples; low friction build (CMake) | Typical baseline skill level? Toolchain diversity (ARM GCC vs MSVC vs clang)? |
| StR-002 | Audio Equipment Manufacturers | Ship standards‑compliant products | Conformance evidence, deterministic behavior, stable LTS branch, reproducible builds | Which manufacturers intend to adopt first? Required certification scope? |
| StR-003 | System Integrators | Build multi‑vendor AV systems | Multi‑standard interoperability (IEEE 1722/1722.1/802.1AS + AES67 + Milan), version coexistence, minimal integration glue | Priority order of standards for MVP? |
| StR-004 | QA / Test Engineers | Validate conformity & regression | Automated conformance + unit test suite, deterministic mocks, coverage reports | Access to certified test vectors? External lab integration? |
| StR-005 | Standards Bodies / Compliance Reviewers | Assess spec adherence | Traceable implementation (requirements→design→code→tests), section references (no copyrighted text), conformance matrix | Any formal submission pathways planned? |
| StR-006 | Project Maintainers | Sustain project long term (volunteer run) | Clean architecture, low duplication, clear module boundaries, contribution guidelines, CI reliability | Governance model & release cadence? |
| StR-007 | Potential Sponsors | Decide on funding/cert sponsorship | Clear roadmap, impact metrics, list of gaps blocking certification, transparency on resource needs | Which certifications unlock most value? |

## High-Level Needs & Rationales

1. Platform Agnostic Core (StR-001, StR-002, StR-003): No vendor / OS specific code in standards layer to ensure portability and reuse.
2. Deterministic & RT-Friendly (StR-002, StR-003): Avoid dynamic allocation in hot paths; non-blocking APIs; bounded latency.
3. Version Abstraction Layer (StR-001, StR-003, StR-006): Allow concurrent support for multiple revisions (e.g., IEEE 1722.1-2013 & 2021) under stable façade.
4. Evidence of Compliance (StR-002, StR-004, StR-005): Conformance test harness + coverage + per‑requirement test mapping.
5. Extensible HAL Boundary (StR-001, StR-003): Inject platform services (network send/recv, clock, timers) via C function pointer tables.
6. Resource Transparency (StR-006, StR-007): Document memory, timing, and CPU profiles for representative scenarios.
7. Volunteer Friendly Contribution (StR-006, StR-007): Clear onboarding docs, small focused modules, automated lint/test.
8. Certification Path Enablement (StR-002, StR-007): Gap analysis documents to attract sponsors for official test suites.

## Stakeholder Requirement Statements

- StR-001: The library SHALL expose a minimal, stable, version‑abstracted API surface for each supported standard enabling integration without knowledge of underlying version specifics.
- StR-002: The implementation SHALL avoid unbounded dynamic memory allocation during steady-state streaming or control message processing.
- StR-003: The system SHALL allow simultaneous enablement of at least two versions of the same standard (e.g., IEEE 1722.1-2013 and 2021) selectable at compile time and optionally at runtime.
- StR-004: Every normative protocol feature SHALL have at least one automated conformance test referencing its requirement ID and standard clause.
- StR-005: Each public API element SHALL trace back to at least one stakeholder requirement ID (StR-xxx) and one system requirement (REQ-xxx).
- StR-006: The build system SHALL succeed with no external package manager dependency (self-contained) except the listed standards submodules.
- StR-007: HAL interfaces SHALL be pure C function pointer structures returning integer status codes (no exceptions) to remain toolchain neutral for embedded targets.
- StR-008: All time synchronization consumers SHALL obtain monotonic synchronized time via injected timing interface (no direct OS calls in standards layer).
- StR-009: The project SHALL produce a sponsor briefing document enumerating certification cost blockers prior to first stable release.
- StR-010: Critical path functions (gPTP sync handling, AVTP timestamping, AECP/ACMP state transitions) SHALL execute within bounded worst-case latency documented in performance notes.

## Assumptions

- A1: Contributors have access to CMake ≥3.20 and a C++17 capable compiler if/when C++ is used (core may remain C for maximal portability).
- A2: Real-time target ARM Cortex-M7 provides representative embedded constraints; Windows x86-64 provides desktop test environment.
- A3: Official certification suites (e.g., Milan, AES67) are not presently licensed; internal conformance tests approximate behavior until sponsorship.

## Risks

| Risk ID | Description | Impact | Mitigation |
|---------|-------------|--------|-----------|
| R-001 | Volunteer bandwidth fluctuations | Slower delivery | Modular backlog, automation, clear priorities |
| R-002 | Lack of official test suites | Interop uncertainty | Layered conformance harness + sponsor outreach |
| R-003 | API instability from refactors | Integrator churn | Versioned façade + semantic versioning policy |
| R-004 | Performance regressions | Real-time failures | Continuous benchmarking & budget thresholds |

## Acceptance Criteria

- All StR IDs mapped to at least one forthcoming system requirement (Phase 02) with traceability file.
- No platform / vendor includes in standards layer code.
- Prototype HAL integrated with at least one mock network & timing provider in tests.
- Conformance test harness skeleton executing in CI with placeholder cases referencing ≥3 StR IDs.

## Traceability Seed

Will be extended in Phase 02 requirement spec:

```text
StR-001 -> REQ-API-VERSIONING
StR-002 -> REQ-RT-NO-UNBOUNDED-ALLOC
StR-003 -> REQ-MULTI-VERSION-COEXIST
StR-004 -> REQ-CONFORMANCE-COVERAGE
StR-007 -> REQ-HAL-C-FPTR
```

## Next Actions

1. Approve stakeholder set & refine open questions.
2. Generate Phase 02 system requirements referencing StR IDs.
3. Establish initial HAL header + mock implementations.
4. Draft sponsor outreach placeholder doc.
