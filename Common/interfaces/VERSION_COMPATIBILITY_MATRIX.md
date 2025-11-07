# Standards Version Compatibility Matrix

## Overview

This document defines which versions of IEEE standards are compatible with each other through the abstract interface contracts defined in `Common/interfaces/`.

## Version Selection Strategy

Each standard can be built with different versions of its dependencies:

```cmake
# Example: Building IEEE 802.1AS-2021 with different IEEE 1588 versions
cmake -DIEEE1588_VERSION=2019 ..  # Use IEEE 1588-2019
cmake -DIEEE1588_VERSION=2008 ..  # Use IEEE 1588-2008
cmake -DIEEE1588_VERSION=latest .. # Use most recent (currently 2019)
```

## Compatibility Matrix

### IEEE 802.1AS Dependencies

| IEEE 802.1AS Version | IEEE 1588 Versions | IEEE 802.1Q Versions | Notes |
|---------------------|-------------------|---------------------|-------|
| **2021** | 2019 ✅<br>2008 ✅<br>2002 ⚠️ | 2020 ✅<br>2014 ✅ | 2002 lacks peer-delay |
| **2020** | 2019 ✅<br>2008 ✅<br>2002 ⚠️ | 2018 ✅<br>2014 ✅ | Legacy support |
| **2011** | 2008 ✅<br>2002 ✅ | 2011 ✅<br>2005 ✅ | Original gPTP |

### IEEE 1722 (AVTP) Dependencies

| IEEE 1722 Version | IEEE 802.1AS Versions | IEEE 802.1Q Versions | IEEE 1588 (indirect) |
|------------------|----------------------|---------------------|---------------------|
| **2016** | 2021 ✅<br>2020 ✅<br>2011 ✅ | 2020 ✅<br>2014 ✅ | 2019/2008 via 802.1AS |
| **2011** | 2011 ✅ | 2011 ✅<br>2005 ✅ | 2008 via 802.1AS |

### IEEE 1722.1 (AVDECC) Dependencies

| IEEE 1722.1 Version | IEEE 1722 Versions | IEEE 802.1AS Versions | IEEE 802.1Q Versions |
|--------------------|-------------------|----------------------|---------------------|
| **2021** | 2016 ✅<br>2011 ⚠️ | 2021 ✅<br>2020 ✅ | 2020 ✅<br>2014 ✅ |
| **2013** | 2011 ✅ | 2011 ✅<br>2020 ✅ | 2011 ✅<br>2014 ✅ |

### IEEE 802.1BA (AVB Systems) Dependencies

| IEEE 802.1BA Version | IEEE 802.1AS | IEEE 802.1Q | IEEE 1722 | IEEE 1722.1 |
|---------------------|--------------|-------------|-----------|-------------|
| **2016** | 2011+ ✅ | 2011+ ✅ | 2011+ ✅ | 2013+ ✅ |
| **2011** | 2011 ✅ | 2011 ✅ | 2011 ✅ | 2013 ✅ |

### AVnu Milan Dependencies

| Milan Version | IEEE 1722.1 | IEEE 1722 | IEEE 802.1AS | IEEE 802.1Q |
|--------------|-------------|-----------|--------------|-------------|
| **v1.2 (2023)** | 2021 ✅ | 2016 ✅ | 2021 ✅ | 2020 ✅ |
| **v1.1** | 2013 ✅ | 2016 ✅ | 2020 ✅ | 2014 ✅ |

## Legend

- ✅ **Fully Compatible**: All features work, tested and validated
- ⚠️ **Limited Compatibility**: Works with feature subset, may have limitations
- ❌ **Incompatible**: Known issues, not recommended
- **latest**: Alias for most recent version (currently 2019 for IEEE 1588)

## Capability Requirements by Standard

### IEEE 802.1AS Requirements from IEEE 1588

**Minimum Required Capabilities:**
```cpp
IIEEE1588Contract::CAP_BASIC_SYNC
IIEEE1588Contract::CAP_DELAY_MECHANISM
IIEEE1588Contract::CAP_BEST_MASTER_CLOCK
IIEEE1588Contract::CAP_FOLLOW_UP
```

**Optional (Enhanced Features):**
```cpp
IIEEE1588Contract::CAP_PEER_DELAY        // P2P delay mechanism
IIEEE1588Contract::CAP_TRANSPARENT_CLOCK // TC support
IIEEE1588Contract::CAP_BOUNDARY_CLOCK    // BC support
```

### IEEE 1722 Requirements from IEEE 802.1AS

**Minimum Required:**
- Synchronized time access (`getCurrentTime()`)
- Clock quality assessment (`getClockQuality()`)
- Synchronization state (`getSyncState()`)

**Optional:**
- Enhanced accuracy for low-latency streams
- Multi-domain support for stream isolation

### IEEE 1722.1 Requirements

**From IEEE 1722:**
- AVTP stream format support
- Presentation time handling

**From IEEE 802.1AS:**
- Time synchronization
- gPTP domain awareness

**From IEEE 802.1Q:**
- VLAN tagging
- Stream reservation (SRP)

## Testing Matrix

### Recommended Test Configurations

| Configuration ID | IEEE 1588 | IEEE 802.1AS | IEEE 802.1Q | IEEE 1722 | IEEE 1722.1 | Status |
|-----------------|-----------|--------------|-------------|-----------|-------------|--------|
| **Modern-Latest** | 2019 | 2021 | 2020 | 2016 | 2021 | ✅ Primary |
| **Modern-Stable** | 2008 | 2021 | 2020 | 2016 | 2021 | ✅ Validated |
| **Legacy-2013** | 2008 | 2011 | 2014 | 2011 | 2013 | ✅ Legacy Support |
| **Mixed-Modern** | 2019 | 2020 | 2020 | 2016 | 2021 | ✅ Compatibility |

### CI/CD Test Strategy

Each commit should test:
1. **Modern-Latest**: Latest version of all standards
2. **Modern-Stable**: Latest with IEEE 1588-2008 (broader hardware support)
3. **Legacy-2013**: Full legacy stack for backward compatibility

## Version Selection Guidelines

### For New Projects

```cmake
# Recommended: Use latest versions for new development
set(IEEE1588_VERSION "2019" CACHE STRING "IEEE 1588 version")
set(IEEE802_1AS_VERSION "2021" CACHE STRING "IEEE 802.1AS version")
set(IEEE802_1Q_VERSION "2020" CACHE STRING "IEEE 802.1Q version")
set(IEEE1722_VERSION "2016" CACHE STRING "IEEE 1722 version")
set(IEEE1722_1_VERSION "2021" CACHE STRING "IEEE 1722.1 version")
```

### For Legacy Hardware Support

```cmake
# Use 2008 for broader hardware compatibility
set(IEEE1588_VERSION "2008" CACHE STRING "IEEE 1588 version")
set(IEEE802_1AS_VERSION "2011" CACHE STRING "IEEE 802.1AS version")
```

### For Milan Certification

```cmake
# Milan v1.2 requires specific versions
set(IEEE1588_VERSION "2019" CACHE STRING "Required by Milan")
set(IEEE802_1AS_VERSION "2021" CACHE STRING "Required by Milan")
set(IEEE1722_VERSION "2016" CACHE STRING "Required by Milan")
set(IEEE1722_1_VERSION "2021" CACHE STRING "Required by Milan")
```

## Dependency Resolution Algorithm

When building with version selection:

1. **Check capability requirements**
   ```cpp
   if (!ieee1588->hasCapability(required_caps)) {
       CMake_Error("Selected IEEE 1588 version lacks required capabilities");
   }
   ```

2. **Verify interface contract version**
   ```cpp
   static_assert(IIEEE1588Contract::VERSION >= 1, 
                 "Interface contract too old");
   ```

3. **Link appropriate implementation**
   ```cmake
   if(IEEE1588_VERSION STREQUAL "2019")
       target_link_libraries(ieee_802_1as PRIVATE ieee_1588_2019)
   elseif(IEEE1588_VERSION STREQUAL "2008")
       target_link_libraries(ieee_802_1as PRIVATE ieee_1588_2008)
   endif()
   ```

## Breaking Changes and Migration

### Interface Contract Versioning

Interface contracts use semantic versioning:

- **Major version**: Breaking changes (new interface required)
- **Minor version**: Backward-compatible additions (optional methods)
- **Patch version**: Documentation or bug fixes

Example:
```cpp
namespace Standards {
namespace Interfaces {
    // Version 1.0: Initial contract
    class IIEEE1588Contract { /* ... */ };
    
    // Version 2.0: Breaking changes (if needed)
    class IIEEE1588ContractV2 { /* ... */ };
}
}
```

### Migration Path for Breaking Changes

1. **Introduce new interface version** (e.g., `V2`)
2. **Keep old interface** for 2 major releases
3. **Provide adapter** from old to new interface
4. **Deprecate old interface** with clear migration guide
5. **Remove old interface** after deprecation period

## Future Version Support

### Planned Additions

- **IEEE 1588-202X**: Next revision (when published)
- **IEEE 802.1AS-202X**: Next gPTP revision
- **IEEE 1722-202X**: Next AVTP revision

### Deprecation Policy

- Legacy versions supported for **3 years** after new version release
- Security fixes continue for **5 years**
- Critical bugs fixed for supported versions only

---

**Maintenance**: Update this matrix when:
- New standard version added
- Compatibility testing reveals issues
- Interface contracts change
- CI/CD test configurations updated
