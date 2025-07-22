# IEEE 1722 File Reorganization Mapping

## File Renames Executed

### IEEE 1722.1-2021 Standard Files
| Original File | New File | Status | Dependencies Updated |
|--------------|----------|--------|---------------------|
| 1722_1-2021.h | ieee_1722_1_2021_base.h | ✅ COMPLETED | ✅ CMakeLists.txt, *.cpp files |
| 1722_1-2021.cpp | ieee_1722_1_2021_base.cpp | ✅ COMPLETED | ✅ CMakeLists.txt |
| 1722_1-2021_clean.h | ieee_1722_1_2021_clean.h | ✅ COMPLETED | ✅ CMakeLists.txt, *.cpp files |
| 1722_1-2021_clean.cpp | ieee_1722_1_2021_clean.cpp | ✅ COMPLETED | ✅ CMakeLists.txt |
| 1722_1-2021_impl.cpp | ieee_1722_1_2021_impl.cpp | ✅ COMPLETED | ⚠️ Needs header fixes |
| 1722_1_state_machines.h | ieee_1722_1_2021_state_machines.h | ✅ COMPLETED | ⚠️ Type conflicts detected |
| 1722_1_state_machines.cpp | ieee_1722_1_2021_state_machines.cpp | ✅ COMPLETED | ⚠️ Depends on header fixes |
| 1722_1_state_machines_refactored.h | ieee_1722_1_2021_state_machines_refactored.h | ✅ COMPLETED | ✅ example files |
| 1722_1_state_machines_refactored.cpp | ieee_1722_1_2021_state_machines_refactored.cpp | ✅ COMPLETED | ⚠️ Template errors |

### IEEE 1722.1-2013 Standard Files  
| Original File | New File | Status | Dependencies Updated |
|--------------|----------|--------|---------------------|
| 1722_1-2013.h | ieee_1722_1_2013_legacy.h | ✅ COMPLETED | ✅ test files |
| 1722_1-2013.cpp | ieee_1722_1_2013_legacy.cpp | ✅ COMPLETED | ✅ CMakeLists.txt |

### IEEE 1722-2016 Standard Files
| Original File | New File | Status | Dependencies Updated |
|--------------|----------|--------|---------------------|
| 1722-2016.h | ieee_1722_2016.h | ✅ COMPLETED | ✅ Milan files, test files |
| 1722-2016.cpp | ieee_1722_2016.cpp | ✅ COMPLETED | ✅ CMakeLists.txt |
| 1722-2016_impl.cpp | ieee_1722_2016_impl.cpp | ✅ COMPLETED | ✅ CMakeLists.txt |

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
2. ✅ Rename core files and update includes  
3. ✅ Update CMakeLists.txt 
4. ✅ Update test files
5. ✅ Update example files
6. ⏳ Fix compilation errors from header conflicts
7. ⏳ Consolidate redundant implementations
8. ⏳ Verify all builds pass
9. ⏳ Update documentation references

## Next Steps Required

### Phase 2: Fix Compilation Issues

#### Header Structure Problems Identified:
1. **Type Redefinitions**: ieee_1722_1_2021_state_machines.h and ieee_1722_1_2021_clean.h define overlapping types
2. **Missing Includes**: Some implementation files reference types not in their headers
3. **Template Errors**: C++ template compatibility issues in some files
4. **Namespace Conflicts**: AECP namespace conflicts in test files

#### Recommended Actions:
1. **Rationalize Headers**: Decide which header should define which types
2. **Fix Include Dependencies**: Ensure each .cpp includes the right headers
3. **Resolve Template Issues**: Address C++ standard compatibility
4. **Test Compilation**: Build each library target individually

## Validation Checklist

- ✅ All renamed files preserve Git history
- ✅ All include paths updated in modified files
- ✅ CMakeLists.txt references new file names
- ⚠️ Some compilation errors remain (see Phase 2)
- ⏳ No duplicate symbols or naming conflicts (pending fixes)
- ⏳ All tests pass after reorganization (pending compilation fixes)
- ⏳ Documentation reflects new structure 
- ✅ Namespace consistency maintained
