# Repository Cleanup & Consolidation Plan

This plan inventories duplicate/redundant implementations and organizes code into the correct directories, preferring standards-only code under `lib/Standards/` and versioned submodules one level above (do not modify submodule contents).

## Goals

- Remove redundant top-level duplicates of 1722.1/1722 implementations.
- Keep hardware/OS specifics out of standards layer.
- Align with Version Abstraction Layer (VAL) and cross-standard dependency rules.

## Known Redundancies (Detected by filename search)

### AECP (IEEE 1722.1-2021)

- Top-level duplicates:
  - `ieee_1722_1_2021_aecp_implementation.cpp`
  - `ieee_1722_1_2021_aecp_simple.cpp`
  - `ieee_1722_1_2021_aecp_windows_compatible.cpp`
  - `ieee_1722_1_2021_aecp_corrected.cpp`
  - Under `IEEE/1722.1/2021/protocols/aecp/` (likely submodule/adapted):
  - `ieee_1722_1_2021_aecp_implementation.cpp`
  - `ieee_1722_1_2021_aecp_simple.cpp` (+ variants)
  - `ieee_1722_1_2021_aecp_windows_compatible.cpp`
  - `ieee_1722_1_2021_aecp_phase2c_real.cpp`

Action: Freeze top-level files; consolidate into a single adapter-backed implementation under `lib/Standards/IEEE/1722.1/2021/aecp/` (façade/adapters) that delegates to selected version backend. Mark top-level copies as ARCHIVE-CANDIDATE.

### ACMP (IEEE 1722.1-2021)

- Top-level: `ieee_1722_1_2021_acmp_implementation.cpp`
- Subtree: `IEEE/1722.1/2021/protocols/acmp/ieee_1722_1_2021_acmp_implementation.cpp`

Action: Keep one canonical implementation via VAL adapter; archive duplicate.

### State Machines (IEEE 1722.1-2021)

- Top-level: `ieee_1722_1_2021_state_machines*.cpp`
- Subtree: `IEEE/1722.1/2021/state_machines/*`

Action: Keep versioned state machines in submodule space; provide wrappers in `lib/Standards/IEEE/1722.1/2021/aem|aecp|acmp|adp/` as needed; archive top-level duplicates post-adapter migration.

## Target Structure

```text
lib/Standards/
  Common/interfaces/  # HAL (C) and optional C++ adapters
  IEEE/1722.1/2021/   # façade + adapters only (no vendor/OS)
  IEEE/1722/2016/     # façade + adapters only
  IEEE/802.1/AS/2021/ # façade + adapters only
```

Submodules remain under `IEEE/`, `AES/`, `AVnu/` directories and are not edited here.

## Decision Matrix (KEEP/MERGE/ARCHIVE)

- KEEP: Adapter/Facade implementations in `lib/Standards/...` (VAL)
- MERGE: Feature deltas from simple/windows variants into canonical adapter where standard-compliant
- ARCHIVE: Top-level duplicate files once VAL-based path is verified by tests

## Steps

1. Create VAL façade headers for AECP/ACMP/ADP/AEM and AVTP/gPTP (no behavior change).
2. Implement per-version adapters that call into submodules.
3. Update tests to use façade; keep legacy tests temporarily via compatibility shims.
4. Remove top-level duplicates; keep archived copies in `backups/` with mapping reference.

## References

- `IEEE_1722_FILE_REORGANIZATION_PLAN.md`
- `IEEE_1722_REORGANIZATION_MAPPING.md`
- `.github/instructions/copilot-instructions.md`
