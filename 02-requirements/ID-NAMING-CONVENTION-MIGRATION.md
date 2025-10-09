---
specType: requirements
standard: "29148"
phase: "02-requirements"
version: "1.0.0"
author: "Requirements Engineering Team"
date: "2025-10-10"
status: "approved"
traceability:
  stakeholderRequirements:
    - "REQ-STK-NAMING-001"
    - "REQ-STK-NAMING-002"
    - "REQ-STK-NAMING-003"
---

# Requirements ID Naming Convention and Migration Plan

## Overview

This document defines the new standard-specific ID naming convention to resolve traceability conflicts and ensure unique identifiers across all requirements documents in the libmedia-network-standards repository.

## Problem Statement

Current requirements documents use generic IDs like:
- `SR-001`, `SR-002`, etc.
- `REQ-F-001`, `REQ-F-002`, etc.
- `SYS-001`, `SYS-002`, etc.

This creates massive ID conflicts when the CI pipeline analyzes traceability across all standards, making it impossible to distinguish between requirements from different standards.

## New Naming Convention

### Standard Abbreviations

| Standard | Abbreviation | Example |
|----------|-------------|---------|
| IEEE 1588-2019 PTP v2.1 | `IEEE1588PTP` | `SR-IEEE1588PTP-001` |
| IEEE 802.1Q-2020 Bridging | `IEEE8021Q` | `SR-IEEE8021Q-001` |
| IEEE 802.1Q-2022 Enhanced | `IEEE8021Q2022` | `SR-IEEE8021Q2022-001` |
| IEEE 802.1BA-2016 AVB Systems | `IEEE8021BA` | `SR-IEEE8021BA-001` |
| IEEE 1722.1-2013 Legacy AVDECC | `IEEE17221LEG` | `SR-IEEE17221LEG-001` |
| IEEE 1722.1-2021 AVDECC | `IEEE17221` | `SR-IEEE17221-001` |
| IEEE 1722-2016 AVTP | `IEEE1722` | `SR-IEEE1722-001` |
| IEEE 802.1AS-2020 gPTP | `IEEE8021AS` | `SR-IEEE8021AS-001` |
| IEEE 802.1AB-2016 LLDP | `IEEE8021AB` | `SR-IEEE8021AB-001` |
| IEEE 802.1AX-2020 Link Aggregation | `IEEE8021AX` | `SR-IEEE8021AX-001` |
| IEEE 802.1X-2020 Authentication | `IEEE8021X` | `SR-IEEE8021X-001` |
| AES67-2018 Audio-over-IP | `AES67` | `SR-AES67-001` |
| AES70-2018 Open Control Architecture | `AES70` | `SR-AES70-001` |
| AES5-2018 Preferred Sampling Frequencies | `AES5` | `SR-AES5-001` |
| AES3-2009 Digital Audio Interface | `AES3` | `SR-AES3-001` |
| AES60id-2020 Object-Based Audio | `AES60ID` | `SR-AES60ID-001` |
| Cross-Standards Architecture | `CROSSARCH` | `SR-CROSSARCH-001` |
| Milan Professional Audio | `MILAN` | `SR-MILAN-001` |

### ID Format Template

```
[TYPE]-[STANDARD]-[NUMBER]

Where:
- TYPE: SR, REQ-F, REQ-NF, SYS, IR, VR, AC, etc.
- STANDARD: Abbreviation from table above
- NUMBER: 3-digit zero-padded number (001, 002, etc.)
```

### Examples

**Before:**
```
SR-001: Unified System Architecture
REQ-F-001: Multi-Standard Integration
SYS-001: Integration Framework
```

**After:**
```
SR-CROSSARCH-001: Unified System Architecture
REQ-F-CROSSARCH-001: Multi-Standard Integration  
SYS-CROSSARCH-001: Integration Framework
```

## Migration Process

### Phase 1: Convention Definition ✅
- [x] Define standard abbreviations
- [x] Create ID format template
- [x] Document migration process

### Phase 2: Update Requirements Documents
For each requirements document:

1. **Identify Current IDs**
   - Scan for all `SR-XXX`, `REQ-F-XXX`, `SYS-XXX`, etc.
   - Document current ID usage

2. **Generate New IDs**
   - Apply standard abbreviation
   - Maintain sequential numbering within each type
   - Ensure uniqueness across entire repository

3. **Update Cross-References**
   - Find all references to old IDs
   - Replace with new IDs
   - Verify reference accuracy

4. **Update Interface Requirements**
   - Update `IR-XXX` to `IR-[STANDARD]-XXX`
   - Maintain interface relationship mappings

5. **Update Verification and Acceptance Criteria**
   - Update `VR-XXX` to `VR-[STANDARD]-XXX`
   - Update `AC-XXX` to `AC-[STANDARD]-XXX`

### Phase 3: Validation
1. **Spec Parser Validation**
   - Run `spec_parser.py` to check for duplicate IDs
   - Verify all IDs follow new convention

2. **Traceability Validation**
   - Run traceability matrix generation
   - Verify cross-references work correctly
   - Check CI pipeline passes

3. **Documentation Update**
   - Update copilot instructions
   - Create ID mapping documentation

## File-by-File Migration Plan

### 1. IEEE 1588-2019 PTP Requirements
**File:** `ieee-1588-2019-ptp-requirements.md`
**Standard:** `IEEE1588PTP`

**ID Mappings:**
- `SR-001` → `SR-IEEE1588PTP-001`
- `SR-002` → `SR-IEEE1588PTP-002`
- `REQ-F-001` → `REQ-F-IEEE1588PTP-001`
- `SYS-001` → `SYS-IEEE1588PTP-001`

### 2. IEEE 802.1Q Bridging Requirements  
**Files:** 
- `ieee-802-1q-2020-bridging-requirements.md`
- `ieee-802-1q-2022-bridges-bridged-networks-requirements.md`

**Standards:** `IEEE8021Q`, `IEEE8021Q2022`

### 3. IEEE 802.1BA AVB Systems Requirements
**File:** `ieee-802-1ba-2016-avb-systems-requirements.md`
**Standard:** `IEEE8021BA`

### 4. IEEE 1722.1 Legacy AVDECC Requirements
**File:** `ieee-1722-1-2013-legacy-avdecc-requirements.md`  
**Standard:** `IEEE17221LEG`

### 5. AES67 Audio-over-IP Requirements
**File:** `aes67-2018-audio-over-ip-requirements.md`
**Standard:** `AES67`

### 6. AES70 Open Control Architecture Requirements
**File:** `aes70-2018-open-control-architecture-requirements.md`
**Standard:** `AES70`

### 7. AES5 Sampling Frequencies Requirements
**File:** `aes5-2018-preferred-sampling-frequencies-requirements.md`
**Standard:** `AES5`

### 8. AES3 Digital Audio Interface Requirements
**File:** `aes3-2009-digital-audio-interface-requirements.md`
**Standard:** `AES3`

### 9. IEEE 802.1AS gPTP Timing Requirements
**File:** `ieee-802-1as-2020-gptp-timing-requirements.md`
**Standard:** `IEEE8021AS`

### 10. IEEE 802.1AB LLDP Discovery Requirements
**File:** `ieee-802-1ab-2016-lldp-discovery-requirements.md`
**Standard:** `IEEE8021AB`

### 11. IEEE 802.1AX Link Aggregation Requirements
**File:** `ieee-802-1ax-2020-link-aggregation-requirements.md`
**Standard:** `IEEE8021AX`

### 12. IEEE 802.1X Authentication Requirements
**File:** `ieee-802-1x-2020-authentication-requirements.md`
**Standard:** `IEEE8021X`

### 13. AES60id Object-Based Audio Requirements
**File:** `aes60id-2020-object-based-audio-requirements.md`
**Standard:** `AES60ID`

### 14. Cross-Standards Architecture Integration Requirements
**File:** `cross-standards-architecture-integration-requirements.md`
**Standard:** `CROSSARCH`

## Tools and Scripts Impact

### Spec Parser Updates
The `spec_parser.py` script will need to handle the new ID format:

**Current Pattern:**
```python
ID_PATTERN = re.compile(r'^(?P<id>(StR|REQ|ARC|ADR|QA|TEST)-[A-Z0-9][A-Z0-9\-]*)\b')
```

**Updated Pattern (Compatible):**
```python  
ID_PATTERN = re.compile(r'^(?P<id>(StR|REQ|ARC|ADR|QA|TEST)-[A-Z0-9][A-Z0-9\-]*)\b')
```
*(Pattern remains compatible as it already handles arbitrary alphanumeric sequences)*

### Traceability Scripts
All traceability validation scripts should continue to work as they use the same regex patterns for ID detection.

## Quality Assurance

### Automated Checks
1. **No Duplicate IDs**: Verify uniqueness across all documents
2. **Consistent Format**: Ensure all IDs follow `[TYPE]-[STANDARD]-[NUMBER]` format
3. **Valid References**: All cross-references point to existing IDs
4. **Complete Coverage**: All requirement types have appropriate IDs

### Manual Review
1. **Semantic Consistency**: IDs match document content
2. **Logical Grouping**: Related requirements have sequential numbers
3. **Cross-Document Links**: Inter-standard references are appropriate

## Benefits

### Improved Traceability
- **Unique Identification**: No ID conflicts across standards
- **Clear Origin**: Standard source immediately visible from ID
- **Better Navigation**: Easy to locate requirements by standard

### Enhanced CI/CD
- **Automated Validation**: CI can validate traceability without conflicts
- **Comprehensive Coverage**: Full system traceability becomes possible
- **Quality Gates**: Stricter validation of requirements completeness

### Better Documentation
- **Self-Documenting**: IDs indicate which standard they belong to
- **Easier Maintenance**: Clear relationship between IDs and standards
- **Professional Presentation**: Industry-standard approach to requirements management

## Implementation Timeline

| Phase | Duration | Activities |
|-------|----------|------------|
| **Phase 1** | Completed | Convention definition and planning |
| **Phase 2** | 2-3 days | Update all 14+ requirements documents |
| **Phase 3** | 1 day | Validation and CI verification |
| **Phase 4** | 0.5 day | Documentation and instruction updates |

**Total Estimated Effort:** 3.5-4.5 days

## Success Criteria

✅ **Zero ID Conflicts**: No duplicate IDs across entire repository  
✅ **CI Pipeline Success**: All traceability checks pass  
✅ **Complete Coverage**: All requirements have unique, traceable IDs  
✅ **Standard Compliance**: Follows industry best practices for requirements management  
✅ **Future Scalability**: Convention supports addition of new standards

---

**Next Steps:**
1. ✅ Complete this convention definition
2. 🔄 Begin systematic update of requirements documents
3. 🔄 Validate with spec parser and traceability tools
4. 🔄 Update development guidelines and instructions

**Document Status:** Complete and Ready for Implementation  
**Date:** 2025-10-09  
**Version:** 1.0