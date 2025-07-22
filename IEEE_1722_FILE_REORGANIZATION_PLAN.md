# IEEE 1722 Standards File Reorganization Plan

## Current State Analysis

### File Categories Identified

#### IEEE 1722.1-2013 Standard Files
```
✅ CONSISTENT NAMING:
- ieee_1722_1_2013.h/.cpp
- ieee_1722_1_2013_complete.h/.cpp  
- ieee_1722_1_2013_complete_test.cpp
- ieee_1722_1_2013_simple_test.cpp
- ieee_1722_1_2013_test.cpp

❌ INCONSISTENT NAMING:
- 1722_1-2013.h/.cpp (should be ieee_1722_1_2013_legacy.h/.cpp)

✅ CORRECT NAMESPACES:
- IEEE::_1722_1::_2013::ADP
- IEEE::_1722_1::_2013::ACMP  
- IEEE::_1722_1::_2013::AECP
- IEEE::_1722_1::_2013::AEM
```

#### IEEE 1722.1-2021 Standard Files
```
✅ CONSISTENT NAMING:
- ieee_1722_1_2021_core.h/.cpp
- ieee_1722_1_2021_library.h/.cpp
- ieee_1722_1_2021_complete_entity.h/.cpp
- ieee_1722_1_2021_state_machines_internal.h/.cpp
- ieee_1722_1_2021_intel_hal_bridge.h/.cpp
- ieee_1722_1_2021_test.cpp
- ieee_1722_1_2021_controller_example.cpp

❌ INCONSISTENT NAMING:
- 1722_1-2021.h/.cpp (should be ieee_1722_1_2021_base.h/.cpp)
- 1722_1-2021_clean.h/.cpp (should be ieee_1722_1_2021_clean.h/.cpp)
- 1722_1-2021_impl.cpp (should be ieee_1722_1_2021_impl.cpp)
- 1722_1_state_machines.h/.cpp (should be ieee_1722_1_2021_state_machines.h/.cpp)
- 1722_1_state_machines_refactored.h/.cpp (should be ieee_1722_1_2021_state_machines_refactored.h/.cpp)

❌ REDUNDANT IMPLEMENTATIONS:
- Multiple AECP implementations: 
  * ieee_1722_1_2021_aecp_implementation.cpp
  * ieee_1722_1_2021_aecp_corrected.cpp
  * ieee_1722_1_2021_aecp_simple.cpp
  * ieee_1722_1_2021_aecp_windows_compatible.cpp
- Multiple ACMP implementations:
  * ieee_1722_1_2021_acmp_implementation.cpp

✅ CORRECT NAMESPACES:
- IEEE::_1722_1::_2021::ADP
- IEEE::_1722_1::_2021::ACMP  
- IEEE::_1722_1::_2021::AECP
- IEEE::_1722_1::_2021::AEM
```

#### IEEE 1722-2016 Standard Files
```
❌ INCONSISTENT NAMING:
- 1722-2016.h/.cpp (should be ieee_1722_2016.h/.cpp)
- 1722-2016_impl.cpp (should be ieee_1722_2016_impl.cpp)

✅ PARTIALLY CONSISTENT:
- ieee_1722_2016_streaming.h/.cpp
- ieee_1722_2016_phase4_tests.h
- ieee_1722_2016_test.cpp

✅ CORRECT NAMESPACES:
- IEEE::_1722::_2016
```

#### AVnu Milan Standard Files
```
✅ CONSISTENT NAMING:
- AVnu_Milan_1_2-2023.h
- AVnu_Milan_1_2-2023_complete.h/.cpp
- AVnu_Milan_2_0a-2023.h

✅ CORRECT NAMESPACES:
- AVnu::Milan::v1_2::_2023
- AVnu::Milan::v2_0a::_2023
```

## File Dependencies Analysis

### Cross-References Found

#### 1722.1-2021 Dependencies:
```cpp
// State machines reference core files
#include "ieee_1722_1_2021_core.h"      // ✅ CORRECT
#include "1722_1-2021.h"                 // ❌ INCONSISTENT - should be ieee_1722_1_2021_base.h

// Library references internal state machines  
#include "ieee_1722_1_2021_state_machines_internal.h"  // ✅ CORRECT
#include "1722_1_state_machines.h"                      // ❌ INCONSISTENT

// Milan references 1722-2016
#include "1722-2016.h"                   // ❌ INCONSISTENT - should be ieee_1722_2016.h
```

#### 1722.1-2013 Dependencies:
```cpp
// Complete implementation is self-contained
#include "ieee_1722_1_2013_complete.h"  // ✅ CORRECT
// Legacy files have no clear dependency pattern
#include "1722_1-2013.h"                 // ❌ INCONSISTENT
```

## Reorganization Plan

### Phase 1: Standardize Naming Convention

#### Rename Files (with dependency updates):

1. **IEEE 1722.1-2021 Files:**
   ```
   1722_1-2021.h → ieee_1722_1_2021_base.h
   1722_1-2021.cpp → ieee_1722_1_2021_base.cpp
   1722_1-2021_clean.h → ieee_1722_1_2021_clean.h  
   1722_1-2021_clean.cpp → ieee_1722_1_2021_clean.cpp
   1722_1-2021_impl.cpp → ieee_1722_1_2021_impl.cpp
   1722_1_state_machines.h → ieee_1722_1_2021_state_machines.h
   1722_1_state_machines.cpp → ieee_1722_1_2021_state_machines.cpp
   1722_1_state_machines_refactored.h → ieee_1722_1_2021_state_machines_refactored.h
   1722_1_state_machines_refactored.cpp → ieee_1722_1_2021_state_machines_refactored.cpp
   ```

2. **IEEE 1722.1-2013 Files:**
   ```
   1722_1-2013.h → ieee_1722_1_2013_legacy.h
   1722_1-2013.cpp → ieee_1722_1_2013_legacy.cpp
   ```

3. **IEEE 1722-2016 Files:**
   ```
   1722-2016.h → ieee_1722_2016.h
   1722-2016.cpp → ieee_1722_2016.cpp  
   1722-2016_impl.cpp → ieee_1722_2016_impl.cpp
   ```

### Phase 2: Consolidate Redundant Implementations

#### AECP Implementation Consolidation:
```
KEEP: ieee_1722_1_2021_aecp_implementation.cpp (most complete)
MERGE FEATURES FROM:
- ieee_1722_1_2021_aecp_windows_compatible.cpp (Windows-specific fixes)
- ieee_1722_1_2021_aecp_simple.cpp (simplified interface)
ARCHIVE: ieee_1722_1_2021_aecp_corrected.cpp (outdated)
```

#### ACMP Implementation Status:
```
KEEP: ieee_1722_1_2021_acmp_implementation.cpp (only implementation)
```

### Phase 3: Update All File References

#### Files Requiring Include Updates:
1. **CMakeLists.txt files** - Update all target source lists
2. **Header files** - Update #include statements  
3. **Test files** - Update include paths
4. **Examples** - Update include paths
5. **Integration files** - Update include paths

### Phase 4: Namespace Consistency Verification

#### Current Namespace Patterns (KEEP):
```cpp
// IEEE 1722-2016
namespace IEEE { namespace _1722 { namespace _2016 { ... }}}

// IEEE 1722.1-2013  
namespace IEEE { namespace _1722_1 { namespace _2013 { ... }}}

// IEEE 1722.1-2021
namespace IEEE { namespace _1722_1 { namespace _2021 { ... }}}

// AVnu Milan
namespace AVnu { namespace Milan { namespace v1_2 { namespace _2023 { ... }}}}
namespace AVnu { namespace Milan { namespace v2_0a { namespace _2023 { ... }}}}
```

## Implementation Strategy

### Step 1: Create Backup
- All file renames will preserve original content
- Create comprehensive mapping of old→new names

### Step 2: Feature Parity Analysis  
- Document all features in redundant implementations
- Ensure no functionality is lost during consolidation

### Step 3: Dependency Mapping
- Map all #include statements across the codebase
- Update CMakeLists.txt and Makefiles
- Update documentation references

### Step 4: Validation Testing
- All existing tests must pass after reorganization
- No build breakage allowed
- No API changes for external consumers

## Expected Benefits

1. **Consistent Naming**: All files follow ieee_[standard]_[version]_[component].h pattern
2. **Clear Dependencies**: Include relationships are obvious from filenames  
3. **Reduced Confusion**: No more mixing of 2013/2021 implementations
4. **Easier Maintenance**: Related files are grouped by naming convention
5. **Better Documentation**: File purposes are clear from standardized names

## Risks and Mitigation

### Risk: Breaking External References
**Mitigation**: Phase the rename over multiple commits, provide compatibility headers initially

### Risk: Lost Functionality in Consolidation  
**Mitigation**: Comprehensive feature audit before merging redundant files

### Risk: Build System Complexity
**Mitigation**: Update all CMakeLists.txt files atomically in same commit as renames

## Success Criteria

- [ ] All files follow consistent ieee_[standard]_[version] naming pattern
- [ ] No redundant implementations remain
- [ ] All builds pass on Windows and Linux  
- [ ] All existing tests continue to pass
- [ ] Clear separation between 2013, 2016, and 2021 standard implementations
- [ ] Documentation accurately reflects new file organization
