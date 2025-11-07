---
specType: requirements
standard: 29148
phase: 02-requirements
version: 0.1.0
author: core-team
date: 2025-11-07
status: draft
traceability:
  stakeholderRequirements:
    - StR-001
    - StR-002
    - StR-003
    - StR-004
    - StR-005
    - StR-006
    - StR-007
---
# System Requirements Specification (SRS)

This SRS captures functional and non-functional requirements for the standards-aggregation library, enabling traceability to stakeholder statements and later design, implementation, and tests.

## Design Constraints (Platform-Agnostic + RT)

- REQ-PLAT-AGNOSTIC: The standards implementation SHALL NOT include vendor or OS specific headers or APIs; platform specifics live behind HAL adapters only.
- REQ-RT-NO-UNBOUNDED-ALLOC (StR-002): The system SHALL avoid unbounded dynamic memory allocation in steady state paths (gPTP processing, AVTP transmit/receive, AECP/ACMP state machines).
- REQ-RT-NONBLOCKING (StR-002): Public APIs SHALL provide non-blocking or bounded-wait forms suitable for RT contexts.
- REQ-RT-BOUNDS (StR-002, StR-003): Worst-case latencies for critical paths SHALL be documented and tested under CI benchmarks.
- REQ-TIME-SOURCE (StR-008): All timing consumers SHALL obtain synchronized time via injected timing interface; no direct OS time calls in standards layer.

## Version Abstraction & Standards Coexistence

- REQ-API-VERSIONING (StR-001, StR-003): Provide stable façade APIs for IEEE 1722, 1722.1, 802.1AS, AES families; select concrete version backends at compile time with optional runtime registry.
- REQ-MULTI-VERSION-COEXIST (StR-003): Support at least 2 versions side-by-side for a given standard (e.g., IEEE 1722.1-2013 and 2021) without symbol clashes.
- REQ-LAYERING (StR-005): Enforce dependency direction (1722.1 → 1722 → 802.1AS); no reverse dependencies.

## HAL & Error Handling

- REQ-HAL-C-FPTR (StR-007): Define C function pointer based HAL structs for network, clock, timers, memory ops with integer return codes.
- REQ-HAL-ADAPTERS: Provide optional C++ adapters to map HAL to existing C++ interfaces (no exceptions in standards layer).
- REQ-ERROR-CODES: All operations SHALL return explicit integer error codes; no exceptions across the boundary.

## Build & Dependencies

- REQ-BUILD-CMAKE (StR-006): CMake is the authoritative build system; projects MUST build with no external package manager besides included submodules.
- REQ-TEST-FWK (StR-004): Unit testing via Google Test and Unity; conformance harness integrated under 07-verification-validation.

## Optional Features

- REQ-FEATURE-COMPILE (StR-001): Optional features SHALL be toggled at compile time; runtime toggles are allowed only when they do not increase steady-state latency or memory fragmentation.

## Performance Priorities

- REQ-PERF-PRIO (StR-002): Optimize for Jitter > Latency > Throughput.
- REQ-PROFILING (StR-006): Provide built-in lightweight profiling hooks; allow integration with SystemView or similar where license permits (in service layer, not standards layer).

## CPU Targets

- REQ-CPU-EMBEDDED (StR-001): Maintain suitability for ARM Cortex-M7 (no reliance on heavy runtime, avoid exceptions/RTTI).
- REQ-CPU-DESKTOP (StR-001): Provide x86-64 Windows build to support development and verification.

## Interoperability

- REQ-INTEROP (StR-003): Ensure cross-standard flows strictly reuse lower-layer implementations (e.g., AVDECC over AVTP with time from 802.1AS).

## Conformance & Traceability

- REQ-CONFORMANCE-COVERAGE (StR-004, StR-005): For each requirement, at least one automated test SHALL be associated; tests SHALL reference standards clause numbers in comments (section numbers only).

## Acceptance

- All REQ-* items traced to StR IDs.
- CI builds green on Windows x86-64; embedded compilation checks for Cortex-M7 toolchain.
- Initial conformance harness runs and reports per‑requirement mapping.
