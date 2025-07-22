# IEEE 1722 File Reorganization Mapping

## File Renames Executed

### IEEE 1722.1-2021 Standard Files
| Original File | New File | Status | Dependencies Updated |
|--------------|----------|--------|---------------------|
| 1722_1-2021.h | ieee_1722_1_2021_base.h | ✅ PLANNED | CMakeLists.txt, *.cpp files |
| 1722_1-2021.cpp | ieee_1722_1_2021_base.cpp | ✅ PLANNED | CMakeLists.txt |
| 1722_1-2021_clean.h | ieee_1722_1_2021_clean.h | ✅ PLANNED | CMakeLists.txt, *.cpp files |
| 1722_1-2021_clean.cpp | ieee_1722_1_2021_clean.cpp | ✅ PLANNED | CMakeLists.txt |
| 1722_1-2021_impl.cpp | ieee_1722_1_2021_impl.cpp | ✅ PLANNED | CMakeLists.txt |
| 1722_1_state_machines.h | ieee_1722_1_2021_state_machines.h | ✅ PLANNED | CMakeLists.txt, example files |
| 1722_1_state_machines.cpp | ieee_1722_1_2021_state_machines.cpp | ✅ PLANNED | CMakeLists.txt |
| 1722_1_state_machines_refactored.h | ieee_1722_1_2021_state_machines_refactored.h | ✅ PLANNED | example files |
| 1722_1_state_machines_refactored.cpp | ieee_1722_1_2021_state_machines_refactored.cpp | ✅ PLANNED | CMakeLists.txt |

### IEEE 1722.1-2013 Standard Files  
| Original File | New File | Status | Dependencies Updated |
|--------------|----------|--------|---------------------|
| 1722_1-2013.h | ieee_1722_1_2013_legacy.h | ✅ PLANNED | test files |
| 1722_1-2013.cpp | ieee_1722_1_2013_legacy.cpp | ✅ PLANNED | CMakeLists.txt |

### IEEE 1722-2016 Standard Files
| Original File | New File | Status | Dependencies Updated |
|--------------|----------|--------|---------------------|
| 1722-2016.h | ieee_1722_2016.h | ✅ PLANNED | Milan files, test files |
| 1722-2016.cpp | ieee_1722_2016.cpp | ✅ PLANNED | CMakeLists.txt |
| 1722-2016_impl.cpp | ieee_1722_2016_impl.cpp | ✅ PLANNED | CMakeLists.txt |

## Include Statement Updates Required

### Files Referencing 1722_1-2021.h:
- ieee_1722_1_2021_acmp_implementation.cpp
- ieee_1722_1_2021_aecp_corrected.cpp  
- ieee_1722_1_2021_aecp_simple.cpp
- ieee_1722_1_2021_aecp_windows_compatible.cpp
- ieee_1722_1_2021_aecp_implementation.cpp
- test_1722_1_2021.cpp
- CMakeLists.txt (multiple references)

### Files Referencing 1722_1-2021_clean.h:
- 1722_1-2021.cpp → ieee_1722_1_2021_base.cpp
- 1722_1-2021_clean.cpp → ieee_1722_1_2021_clean.cpp
- CMakeLists.txt

### Files Referencing 1722_1_state_machines_refactored.h:
- 1722_1_state_machines_refactored.cpp → ieee_1722_1_2021_state_machines_refactored.cpp
- example_avdecc_controller.cpp

### Files Referencing 1722_1-2013.h:
- 1722_1-2013.cpp → ieee_1722_1_2013_legacy.cpp
- test_1722_1_2013.cpp

### Files Referencing 1722-2016.h:
- 1722-2016.cpp → ieee_1722_2016.cpp
- 1722-2016_impl.cpp → ieee_1722_2016_impl.cpp
- test_1722_2016.cpp
- AVnu_Milan_2_0a-2023.h

## CMakeLists.txt Updates Required

### Current CMakeLists.txt references to update:
```cmake
# Line 23: 1722_1-2021_clean.cpp → ieee_1722_1_2021_clean.cpp
# Line 24: 1722_1-2021_clean.h → ieee_1722_1_2021_clean.h
# Line 34: 1722_1-2021.h → ieee_1722_1_2021_base.h
# Line 40: 1722_1-2021.h → ieee_1722_1_2021_base.h
```

## Feature Consolidation Plan

### AECP Implementation Files:
```
PRIMARY: ieee_1722_1_2021_aecp_implementation.cpp (keep)
FEATURES TO MERGE:
- Windows compatibility from ieee_1722_1_2021_aecp_windows_compatible.cpp
- Simplified interface from ieee_1722_1_2021_aecp_simple.cpp
ARCHIVE: ieee_1722_1_2021_aecp_corrected.cpp (outdated)
```

### Legacy File Handling:
```
OBSOLETE_1722_1-2021.h/.cpp → ARCHIVE (marked as obsolete)
OBSOLETE_1722_1_state_machines.h/.cpp → ARCHIVE (marked as obsolete)
```

## Implementation Steps

1. ✅ Create this mapping file
2. ⏳ Rename core files and update includes
3. ⏳ Update CMakeLists.txt 
4. ⏳ Update test files
5. ⏳ Update example files
6. ⏳ Consolidate redundant implementations
7. ⏳ Verify all builds pass
8. ⏳ Update documentation references

## Validation Checklist

- [ ] All renamed files compile successfully
- [ ] All include paths resolve correctly
- [ ] CMakeLists.txt builds all targets
- [ ] No duplicate symbols or naming conflicts
- [ ] All tests pass after reorganization
- [ ] Documentation reflects new structure
- [ ] Namespace consistency maintained
