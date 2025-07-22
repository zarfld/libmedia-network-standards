# IEEE 1722.1 File Organization Analysis & Naming Convention Plan

## 📋 Current File Status Analysis

### Current IEEE 1722.1 Files in `lib/Standards/`

| File Name | Purpose | Status | Standard Version | Recommendation |
|-----------|---------|--------|------------------|----------------|
| `1722_1-2013.h/.cpp` | IEEE 1722.1-2013 implementation | ✅ Active | 2013 | **KEEP** - Clear 2013 version |
| `1722_1-2021.h/.cpp` | Early IEEE 1722.1-2021 implementation | ⚠️ Partial | 2021 | **OBSOLETE** - Replace with clean version |
| `1722_1-2021_clean.h/.cpp` | Clean IEEE 1722.1-2021 implementation | ✅ Active | 2021 | **KEEP** - Main 2021 implementation |
| `1722_1_state_machines.h/.cpp` | Original state machine implementation | ⚠️ Complex | 2021 | **OBSOLETE** - Replace with refactored |
| `1722_1_state_machines_refactored.h/.cpp` | Library-style state machines | ✅ Active | 2021 | **KEEP** - Modern library design |
| `1722-2016.h/.cpp` | IEEE 1722-2016 streaming standard | ✅ Active | 2016 | **KEEP** - Different standard |

### Related Files
| File Name | Purpose | Status | Recommendation |
|-----------|---------|--------|----------------|
| `example_avdecc_controller.cpp` | Library usage example | ✅ Active | **KEEP** |
| `test_1722_1_2013.cpp` | 2013 version tests | ✅ Active | **KEEP** |
| `test_1722_1_2021.cpp` | 2021 version tests | ✅ Active | **KEEP** |
| `test_1722_2016.cpp` | 1722-2016 tests | ✅ Active | **KEEP** |

---

## 🎯 Proposed File Naming Convention

### Clear Version Identification

#### IEEE 1722.1-2013 (Legacy Version)
```
ieee_1722_1_2013.h           // Main header
ieee_1722_1_2013.cpp         // Implementation  
ieee_1722_1_2013_test.cpp    // Unit tests
```

#### IEEE 1722.1-2021 (Current Standard)
```
ieee_1722_1_2021_core.h      // Core types and PDU definitions
ieee_1722_1_2021_core.cpp    // Core implementation
ieee_1722_1_2021_library.h   // High-level library interface  
ieee_1722_1_2021_library.cpp // Library implementation
ieee_1722_1_2021_test.cpp    // Unit tests
ieee_1722_1_2021_example.cpp // Usage examples
```

#### IEEE 1722-2016 (Streaming Standard)
```
ieee_1722_2016_streaming.h   // Clear distinction from 1722.1
ieee_1722_2016_streaming.cpp // Implementation
ieee_1722_2016_test.cpp      // Tests
```

---

## 🗂️ File Migration Plan

### Phase 1: Rename Active Files (Immediate)
1. **`1722_1-2013.h/.cpp`** → **`ieee_1722_1_2013.h/.cpp`**
   - Status: Active, well-defined 2013 implementation
   - Action: Rename for consistency

2. **`1722_1-2021_clean.h/.cpp`** → **`ieee_1722_1_2021_core.h/.cpp`**
   - Status: Active, core 2021 implementation  
   - Action: Rename to indicate core functionality

3. **`1722_1_state_machines_refactored.h/.cpp`** → **`ieee_1722_1_2021_library.h/.cpp`**
   - Status: Active, modern library design
   - Action: Rename to indicate high-level library

4. **`1722-2016.h/.cpp`** → **`ieee_1722_2016_streaming.h/.cpp`**
   - Status: Active, different standard
   - Action: Rename for clarity

### Phase 2: Mark Obsolete Files (Immediate)
1. **`1722_1-2021.h/.cpp`** → **`OBSOLETE_1722_1-2021.h/.cpp`**
   - Status: Superseded by clean implementation
   - Action: Mark obsolete, plan for removal

2. **`1722_1_state_machines.h/.cpp`** → **`OBSOLETE_1722_1_state_machines.h/.cpp`**
   - Status: Superseded by refactored library
   - Action: Mark obsolete, plan for removal

### Phase 3: Update Tests and Examples (Follow-up)
1. **`test_1722_1_2013.cpp`** → **`ieee_1722_1_2013_test.cpp`**
2. **`test_1722_1_2021.cpp`** → **`ieee_1722_1_2021_test.cpp`**
3. **`test_1722_2016.cpp`** → **`ieee_1722_2016_test.cpp`**
4. **`example_avdecc_controller.cpp`** → **`ieee_1722_1_2021_controller_example.cpp`**

---

## 📁 Final Directory Structure

```
lib/Standards/
├── IEEE 1722.1-2013 (Legacy)
│   ├── ieee_1722_1_2013.h
│   ├── ieee_1722_1_2013.cpp
│   └── ieee_1722_1_2013_test.cpp
│
├── IEEE 1722.1-2021 (Current)
│   ├── ieee_1722_1_2021_core.h        // Core types, PDUs, descriptors
│   ├── ieee_1722_1_2021_core.cpp      // Core implementation
│   ├── ieee_1722_1_2021_library.h     // High-level library API
│   ├── ieee_1722_1_2021_library.cpp   // Library implementation  
│   ├── ieee_1722_1_2021_test.cpp      // Unit tests
│   └── ieee_1722_1_2021_controller_example.cpp // Usage example
│
├── IEEE 1722-2016 (Streaming)
│   ├── ieee_1722_2016_streaming.h
│   ├── ieee_1722_2016_streaming.cpp
│   └── ieee_1722_2016_test.cpp
│
├── Obsolete (To be removed)
│   ├── OBSOLETE_1722_1-2021.h
│   ├── OBSOLETE_1722_1-2021.cpp
│   ├── OBSOLETE_1722_1_state_machines.h
│   └── OBSOLETE_1722_1_state_machines.cpp
│
└── Documentation
    ├── IEEE_1722_1_2021_LIBRARY_DESIGN_PLAN.md
    ├── IEEE_1722_1_2021_LIBRARY_IMPLEMENTATION_SUMMARY.md
    └── IEEE_1722_1_2021_STATE_MACHINE_FINAL_STATUS.md
```

---

## 🔄 Include Dependencies Update

### After Renaming
Update all `#include` statements:

```cpp
// OLD
#include "1722_1-2021_clean.h"
#include "1722_1_state_machines_refactored.h"

// NEW  
#include "ieee_1722_1_2021_core.h"
#include "ieee_1722_1_2021_library.h"
```

---

## ✅ Benefits of New Naming Convention

1. **Clear Version Identification**: Immediately obvious which IEEE standard version
2. **Obsolete File Management**: Clear marking of deprecated files  
3. **Standard Separation**: 1722.1 vs 1722-2016 clearly distinguished
4. **Functional Clarity**: Core vs Library vs Test clearly identified
5. **Professional Appearance**: Consistent `ieee_` prefix matches industry standards
6. **Future-Proof**: Easy to add new standard versions (e.g., `ieee_1722_1_2025.h`)

---

## 🚀 Implementation Priority

### Immediate (Phase 1)
- [x] Analyze current file status
- [ ] Rename active files with clear version identification
- [ ] Mark obsolete files for removal
- [ ] Update include dependencies

### Follow-up (Phase 2)  
- [ ] Rename test files for consistency
- [ ] Update CMakeLists.txt with new file names
- [ ] Update documentation references
- [ ] Remove obsolete files after verification

### Future (Phase 3)
- [ ] Consider moving obsolete files to `archive/` directory
- [ ] Add file header comments explaining version differences
- [ ] Create migration guide for external users

---

## 📝 Summary

The current file organization has evolved organically and contains obsolete implementations alongside active ones. The proposed naming convention provides:

- **Clear standard version identification** (2013 vs 2021 vs 2016)
- **Functional separation** (core vs library vs examples)  
- **Obsolete file management** (clear marking for removal)
- **Professional consistency** (ieee_ prefix convention)

This organization will prevent confusion and make the codebase more maintainable for both developers and users.
