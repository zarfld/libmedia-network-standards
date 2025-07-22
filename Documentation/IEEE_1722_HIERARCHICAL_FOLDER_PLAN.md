# IEEE 1722 Standards Hierarchical Folder Structure Plan

## Proposed Folder Structure

Based on the current file analysis, I propose the following hierarchical organization:

```
lib/Standards/
├── IEEE/
│   ├── 1722/
│   │   └── 2016/
│   │       ├── core/
│   │       │   ├── ieee_1722_2016.h
│   │       │   ├── ieee_1722_2016.cpp
│   │       │   └── ieee_1722_2016_impl.cpp
│   │       ├── streaming/
│   │       │   ├── ieee_1722_2016_streaming.h
│   │       │   ├── ieee_1722_2016_streaming.cpp
│   │       │   ├── ieee_1722_2016_streaming_clean.cpp
│   │       │   ├── ieee_1722_2016_streaming_fixed.cpp
│   │       │   └── ieee_1722_2016_streaming_simple.cpp
│   │       ├── tests/
│   │       │   ├── ieee_1722_2016_test.cpp
│   │       │   └── ieee_1722_2016_phase4_tests.h
│   │       └── CMakeLists.txt
│   │
│   └── 1722.1/
│       ├── 2013/
│       │   ├── core/
│       │   │   ├── ieee_1722_1_2013_legacy.h
│       │   │   ├── ieee_1722_1_2013_legacy.cpp
│       │   │   ├── ieee_1722_1_2013.h
│       │   │   ├── ieee_1722_1_2013.cpp
│       │   │   ├── ieee_1722_1_2013_complete.h
│       │   │   └── ieee_1722_1_2013_complete.cpp
│       │   ├── tests/
│       │   │   ├── ieee_1722_1_2013_test.cpp
│       │   │   ├── ieee_1722_1_2013_complete_test.cpp
│       │   │   ├── ieee_1722_1_2013_simple_test.cpp
│       │   │   └── test_1722_1_2013.cpp
│       │   └── CMakeLists.txt
│       │
│       └── 2021/
│           ├── core/
│           │   ├── ieee_1722_1_2021_base.h
│           │   ├── ieee_1722_1_2021_base.cpp
│           │   ├── ieee_1722_1_2021_clean.h
│           │   ├── ieee_1722_1_2021_clean.cpp
│           │   ├── ieee_1722_1_2021_core.h
│           │   ├── ieee_1722_1_2021_core.cpp
│           │   └── ieee_1722_1_2021_impl.cpp
│           ├── protocols/
│           │   ├── aecp/
│           │   │   ├── ieee_1722_1_2021_aecp_implementation.cpp
│           │   │   ├── ieee_1722_1_2021_aecp_windows_compatible.cpp
│           │   │   ├── ieee_1722_1_2021_aecp_simple.cpp
│           │   │   └── ieee_1722_1_2021_aecp_corrected.cpp
│           │   └── acmp/
│           │       └── ieee_1722_1_2021_acmp_implementation.cpp
│           ├── state_machines/
│           │   ├── ieee_1722_1_2021_state_machines.h
│           │   ├── ieee_1722_1_2021_state_machines.cpp
│           │   ├── ieee_1722_1_2021_state_machines_refactored.h
│           │   ├── ieee_1722_1_2021_state_machines_refactored.cpp
│           │   ├── ieee_1722_1_2021_state_machines_internal.h
│           │   └── ieee_1722_1_2021_state_machines_internal.cpp
│           ├── library/
│           │   ├── ieee_1722_1_2021_library.h
│           │   ├── ieee_1722_1_2021_library.cpp
│           │   ├── ieee_1722_1_2021_complete_entity.h
│           │   └── ieee_1722_1_2021_complete_entity.cpp
│           ├── hardware/
│           │   ├── ieee_1722_1_2021_intel_hal_bridge.h
│           │   ├── ieee_1722_1_2021_intel_hal_bridge.cpp
│           │   └── ieee_1722_1_2021_intel_hal_bridge_new.h
│           ├── tests/
│           │   ├── ieee_1722_1_2021_test.cpp
│           │   ├── test_1722_1_2021.cpp
│           │   ├── test_aecp_implementation.cpp
│           │   └── test_core_descriptors.cpp
│           ├── examples/
│           │   ├── ieee_1722_1_2021_controller_example.cpp
│           │   └── example_avdecc_controller.cpp
│           ├── documentation/
│           │   ├── IEEE_1722_1_2021_GAP_ANALYSIS.md
│           │   ├── IEEE_1722_1_2021_LIBRARY_DESIGN_PLAN.md
│           │   ├── IEEE_1722_1_2021_LIBRARY_IMPLEMENTATION_SUMMARY.md
│           │   ├── IEEE_1722_1_2021_STATE_MACHINES_IMPLEMENTATION_STATUS.md
│           │   └── IEEE_1722_1_2021_STATE_MACHINE_FINAL_STATUS.md
│           └── CMakeLists.txt
│
├── AVnu/
│   └── Milan/
│       ├── 1.2-2023/
│       │   ├── AVnu_Milan_1_2-2023.h
│       │   ├── AVnu_Milan_1_2-2023_complete.h
│       │   ├── AVnu_Milan_1_2-2023_complete.cpp
│       │   └── CMakeLists.txt
│       └── 2.0a-2023/
│           ├── AVnu_Milan_2_0a-2023.h
│           └── CMakeLists.txt
│
├── Integration/
│   ├── avdecc_integration.h
│   ├── example_integration.cpp
│   └── hive_hardware_test.cpp
│
├── Common/
│   ├── industrial_protocols.h
│   ├── security_cryptography.h
│   └── CMakeLists.txt
│
├── Documentation/
│   ├── IEEE_1722_FILE_ORGANIZATION_ANALYSIS.md
│   ├── IEEE_1722_FILE_ORGANIZATION_COMPLETE.md
│   ├── IEEE_1722_FILE_REORGANIZATION_PLAN.md
│   ├── IEEE_1722_REORGANIZATION_MAPPING.md
│   ├── IEEE_1722_REORGANIZATION_PHASE1_COMPLETE.md
│   ├── IMPLEMENTATION_COMPLETION_REPORT.md
│   ├── INTEL_AVB_AVDECC_EXTENSION_SUCCESS.md
│   ├── DESCRIPTOR_IMPLEMENTATION_STATUS.md
│   ├── CMAKE_UPDATE_SUMMARY.md
│   ├── avdecc_packet_analysis.md
│   └── README.md
│
├── Scripts/
│   ├── verify_build_success.ps1
│   ├── verify_complete_coverage.ps1
│   ├── verify_domains.ps1
│   └── verify_full_implementation.ps1
│
├── Archive/
│   ├── OBSOLETE_1722_1-2021.h
│   ├── OBSOLETE_1722_1-2021.cpp
│   ├── OBSOLETE_1722_1_state_machines.h
│   ├── OBSOLETE_1722_1_state_machines.cpp
│   ├── CMakeLists_ieee_1722_1_2013.txt
│   ├── CMakeLists_minimal.txt
│   └── CMakeLists_original.txt
│
└── CMakeLists.txt (root)
```

## Benefits of Hierarchical Structure

### 🎯 Standards Organization Benefits
1. **IEEE 1722-2016 (AVTP)**: Clear separation of core protocol from streaming applications
2. **IEEE 1722.1-2013**: Legacy implementation isolated but accessible
3. **IEEE 1722.1-2021**: Logical grouping by functional area (core, protocols, state machines, library)
4. **AVnu Milan**: Industry extensions properly separated

### 🎯 Development Benefits
1. **Modular Development**: Work on specific protocol areas without interference
2. **Clear Dependencies**: Folder structure shows component relationships
3. **Testing Organization**: Tests grouped with their implementation
4. **Documentation**: Related docs grouped with implementations

### 🎯 Build System Benefits
1. **Granular CMake**: Each folder can have its own CMakeLists.txt
2. **Selective Building**: Build only needed components
3. **Dependency Management**: Clear include paths
4. **Parallel Development**: Teams can work on different folders independently

## Detailed Folder Purpose

### IEEE/1722/2016/ (AVTP Core)
- **core/**: Basic AVTP protocol implementation
- **streaming/**: Audio/video streaming applications
- **tests/**: AVTP-specific tests

### IEEE/1722.1/2013/ (Legacy AVDECC)
- **core/**: Complete 2013 implementation (standalone)
- **tests/**: 2013-specific tests

### IEEE/1722.1/2021/ (Modern AVDECC)
- **core/**: Base protocol definitions and utilities
- **protocols/aecp/**: AECP protocol implementations (multiple variants)
- **protocols/acmp/**: ACMP protocol implementation
- **state_machines/**: Protocol state machine implementations
- **library/**: High-level API and complete entity implementations
- **hardware/**: Intel HAL integration
- **tests/**: 2021-specific tests
- **examples/**: Usage examples and demos
- **documentation/**: Implementation-specific documentation

### AVnu/Milan/ (Industry Extensions)
- **1.2-2023/**: Milan 1.2 implementation
- **2.0a-2023/**: Milan 2.0a implementation

### Integration/ (Cross-Standard)
- Files that integrate multiple standards

### Common/ (Shared Utilities)
- Protocols and utilities used across multiple standards

## Include Path Strategy

With the new structure, include paths would become:

```cpp
// IEEE 1722-2016 AVTP
#include "IEEE/1722/2016/core/ieee_1722_2016.h"
#include "IEEE/1722/2016/streaming/ieee_1722_2016_streaming.h"

// IEEE 1722.1-2013 Legacy AVDECC  
#include "IEEE/1722.1/2013/core/ieee_1722_1_2013_complete.h"

// IEEE 1722.1-2021 Modern AVDECC
#include "IEEE/1722.1/2021/core/ieee_1722_1_2021_clean.h"
#include "IEEE/1722.1/2021/library/ieee_1722_1_2021_library.h"
#include "IEEE/1722.1/2021/protocols/aecp/ieee_1722_1_2021_aecp_implementation.h"

// AVnu Milan
#include "AVnu/Milan/1.2-2023/AVnu_Milan_1_2-2023.h"
#include "AVnu/Milan/2.0a-2023/AVnu_Milan_2_0a-2023.h"

// Integration
#include "Integration/avdecc_integration.h"

// Common utilities
#include "Common/industrial_protocols.h"
```

## CMake Structure

Each major folder would have its own CMakeLists.txt:

```cmake
# IEEE/1722/2016/CMakeLists.txt
add_library(ieee_1722_2016_core STATIC
    core/ieee_1722_2016.cpp
    core/ieee_1722_2016_impl.cpp
)

add_library(ieee_1722_2016_streaming STATIC
    streaming/ieee_1722_2016_streaming.cpp
)

# IEEE/1722.1/2021/CMakeLists.txt
add_library(ieee_1722_1_2021_core STATIC
    core/ieee_1722_1_2021_clean.cpp
    core/ieee_1722_1_2021_base.cpp
)

add_library(ieee_1722_1_2021_aecp STATIC
    protocols/aecp/ieee_1722_1_2021_aecp_implementation.cpp
)

add_library(ieee_1722_1_2021_library STATIC
    library/ieee_1722_1_2021_library.cpp
    library/ieee_1722_1_2021_complete_entity.cpp
)
```

## Migration Strategy

### Phase 2A: Create Folder Structure
1. Create all directories
2. Move files to appropriate folders
3. Update all include paths
4. Update CMakeLists.txt files

### Phase 2B: Validate Structure
1. Verify all builds work
2. Test all functionality
3. Update documentation
4. Update external project references

## Backwards Compatibility

For external projects, we could provide compatibility headers:

```cpp
// lib/Standards/ieee_1722_1_2021_clean.h (compatibility)
#pragma once
#include "IEEE/1722.1/2021/core/ieee_1722_1_2021_clean.h"
```

This would allow existing projects to continue working while new projects adopt the hierarchical structure.

## Implementation Recommendation

✅ **RECOMMEND PROCEEDING** with hierarchical folder structure because:

1. **Scalability**: Easy to add new standards (IEEE 1722-2022, Milan 3.0, etc.)
2. **Clarity**: Obvious where each file belongs
3. **Maintainability**: Logical separation of concerns
4. **Professional**: Industry-standard organization pattern
5. **Future-Proof**: Accommodates growth and evolution

The hierarchical structure would transform the Standards library from a flat collection of files into a well-organized, professional standards implementation library.
