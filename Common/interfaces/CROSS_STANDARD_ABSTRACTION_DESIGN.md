# Cross-Standard Abstraction Layer Design

## Problem Statement

With standards now as separate submodules (compiled and tested independently), we need:
1. **Version flexibility**: Support multiple versions of each standard (e.g., IEEE 1588-2019, 2008, 2002)
2. **Contract stability**: Standards referencing others need fixed, well-defined interfaces
3. **Loose coupling**: Changes in one standard shouldn't break dependent standards
4. **Build-time selection**: Choose which version to link against at build time

## Solution: Abstract Interface Contracts

### Architecture Principle

Each standard provides:
1. **Version-specific implementation** (in its submodule)
2. **Abstract interface contract** (in Common/interfaces/)
3. **Version capability reporting** (runtime feature detection)

```
┌─────────────────────────────────────────────────┐
│         Dependent Standard (IEEE 802.1AS)       │
│              Uses Abstract Interface            │
└────────────────┬────────────────────────────────┘
                 │
                 │ depends on
                 ↓
┌─────────────────────────────────────────────────┐
│     Common/interfaces/IIEEE1588Contract.hpp     │
│           (Abstract Interface Contract)          │
│                                                  │
│  • Defines required methods/types               │
│  • Version-agnostic API                         │
│  • Capability flags for version detection       │
└────────────────┬────────────────────────────────┘
                 │
                 │ implemented by
                 ↓
┌─────────────────────────────────────────────────┐
│    IEEE 1588 Version Implementations            │
│  ┌──────────────┐  ┌──────────────┐            │
│  │ IEEE 1588    │  │ IEEE 1588    │            │
│  │ 2019 Impl    │  │ 2008 Impl    │            │
│  └──────────────┘  └──────────────┘            │
└─────────────────────────────────────────────────┘
```

## Implementation Strategy

### 1. Abstract Interface Contracts (Common/interfaces/)

Each standard with cross-references provides an abstract interface:

```cpp
// Common/interfaces/IIEEE1588Contract.hpp
namespace Standards {
namespace Interfaces {

/**
 * @brief Abstract interface contract for IEEE 1588 Precision Time Protocol
 * 
 * This interface defines the minimum contract that any IEEE 1588 implementation
 * must provide to be compatible with dependent standards (IEEE 802.1AS, IEEE 1722, etc.)
 * 
 * Design Principles:
 * - Version-agnostic: Works with IEEE 1588-2019, 2008, 2002
 * - Minimal contract: Only essential operations required by dependents
 * - Capability detection: Runtime query for version-specific features
 * - Zero overhead: Pure virtual functions with no runtime penalty
 */
class IIEEE1588Contract {
public:
    virtual ~IIEEE1588Contract() = default;
    
    // ========== Version and Capability Detection ==========
    
    /**
     * @brief Get the IEEE 1588 version implemented
     * @return Version string (e.g., "2019", "2008", "2002")
     */
    virtual const char* getVersion() const noexcept = 0;
    
    /**
     * @brief Query if specific capability is supported
     * @param capability Capability identifier (see Capability enum)
     * @return true if supported, false otherwise
     */
    virtual bool hasCapability(uint32_t capability) const noexcept = 0;
    
    enum Capability : uint32_t {
        // Core capabilities (all versions)
        CAP_BASIC_SYNC           = 0x0001,  // Basic sync message support
        CAP_DELAY_MECHANISM      = 0x0002,  // Delay request/response
        CAP_BEST_MASTER_CLOCK    = 0x0004,  // BMCA algorithm
        
        // IEEE 1588-2008+ capabilities
        CAP_PEER_DELAY           = 0x0010,  // Peer-to-peer delay mechanism
        CAP_TRANSPARENT_CLOCK    = 0x0020,  // Transparent clock support
        CAP_BOUNDARY_CLOCK       = 0x0040,  // Boundary clock support
        
        // IEEE 1588-2019 capabilities
        CAP_SECURITY             = 0x0100,  // Security mechanisms
        CAP_MULTI_DOMAIN         = 0x0200,  // Multi-domain support
        CAP_ENHANCED_ACCURACY    = 0x0400,  // Enhanced calibration
        CAP_CMLDS                = 0x0800   // Common Mean Link Delay Service
    };
    
    // ========== Core Timing Operations ==========
    
    /**
     * @brief Timestamp structure (common across all versions)
     */
    struct Timestamp {
        uint64_t seconds;        // Seconds since epoch
        uint32_t nanoseconds;    // Nanoseconds (0-999,999,999)
    };
    
    /**
     * @brief Get current PTP time
     * @param[out] timestamp Output timestamp
     * @return 0 on success, error code on failure
     */
    virtual int getCurrentTime(Timestamp& timestamp) const noexcept = 0;
    
    /**
     * @brief Clock identity (8 bytes, standard across versions)
     */
    struct ClockIdentity {
        uint8_t id[8];
    };
    
    /**
     * @brief Get local clock identity
     * @param[out] clock_id Output clock identity
     * @return 0 on success, error code on failure
     */
    virtual int getClockIdentity(ClockIdentity& clock_id) const noexcept = 0;
    
    // ========== Clock Quality Assessment ==========
    
    /**
     * @brief Clock quality structure (compatible across versions)
     */
    struct ClockQuality {
        uint8_t clock_class;         // Clock class (Section 7.6.2.2)
        uint8_t clock_accuracy;      // Clock accuracy (Section 7.6.2.3)
        uint16_t offset_scaled_log_variance;  // Allan variance
    };
    
    /**
     * @brief Get current clock quality metrics
     * @param[out] quality Output clock quality
     * @return 0 on success, error code on failure
     */
    virtual int getClockQuality(ClockQuality& quality) const noexcept = 0;
    
    // ========== Synchronization State ==========
    
    enum SyncState : uint8_t {
        SYNC_STATE_INITIALIZING = 0,
        SYNC_STATE_LISTENING    = 1,
        SYNC_STATE_SLAVE        = 2,
        SYNC_STATE_MASTER       = 3,
        SYNC_STATE_PASSIVE      = 4,
        SYNC_STATE_FAULTY       = 5
    };
    
    /**
     * @brief Get current synchronization state
     * @return Current sync state
     */
    virtual SyncState getSyncState() const noexcept = 0;
    
    /**
     * @brief Get offset from master (if in slave state)
     * @return Offset in nanoseconds (0 if not synchronized)
     */
    virtual int64_t getOffsetFromMaster() const noexcept = 0;
    
    /**
     * @brief Get mean path delay to master
     * @return Path delay in nanoseconds (0 if unknown)
     */
    virtual uint64_t getMeanPathDelay() const noexcept = 0;
};

} // namespace Interfaces
} // namespace Standards
```

### 2. Version-Specific Implementations

Each IEEE 1588 version implements the contract:

```cpp
// IEEE/1588/2019/include/IEEE1588_2019_Implementation.hpp
#include "Common/interfaces/IIEEE1588Contract.hpp"

namespace IEEE {
namespace _1588 {
namespace _2019 {

class IEEE1588_2019_Implementation : public Standards::Interfaces::IIEEE1588Contract {
public:
    // Version identification
    const char* getVersion() const noexcept override {
        return "2019";
    }
    
    bool hasCapability(uint32_t capability) const noexcept override {
        // IEEE 1588-2019 supports all capabilities
        constexpr uint32_t SUPPORTED_CAPS = 
            CAP_BASIC_SYNC | CAP_DELAY_MECHANISM | CAP_BEST_MASTER_CLOCK |
            CAP_PEER_DELAY | CAP_TRANSPARENT_CLOCK | CAP_BOUNDARY_CLOCK |
            CAP_SECURITY | CAP_MULTI_DOMAIN | CAP_ENHANCED_ACCURACY | CAP_CMLDS;
        return (capability & SUPPORTED_CAPS) != 0;
    }
    
    // Implement all contract methods...
    int getCurrentTime(Timestamp& timestamp) const noexcept override;
    int getClockIdentity(ClockIdentity& clock_id) const noexcept override;
    // ... etc
};

} // namespace _2019
} // namespace _1588
} // namespace IEEE
```

```cpp
// IEEE/1588/2008/include/IEEE1588_2008_Implementation.hpp
namespace IEEE {
namespace _1588 {
namespace _2008 {

class IEEE1588_2008_Implementation : public Standards::Interfaces::IIEEE1588Contract {
public:
    const char* getVersion() const noexcept override {
        return "2008";
    }
    
    bool hasCapability(uint32_t capability) const noexcept override {
        // IEEE 1588-2008 doesn't support 2019-specific features
        constexpr uint32_t SUPPORTED_CAPS = 
            CAP_BASIC_SYNC | CAP_DELAY_MECHANISM | CAP_BEST_MASTER_CLOCK |
            CAP_PEER_DELAY | CAP_TRANSPARENT_CLOCK | CAP_BOUNDARY_CLOCK;
        return (capability & SUPPORTED_CAPS) != 0;
    }
    
    // Implement contract methods for 2008 version...
};

} // namespace _2008
} // namespace _1588
} // namespace IEEE
```

### 3. Build-Time Version Selection

Use CMake to select which version to link:

```cmake
# Common/interfaces/CMakeLists.txt
add_library(standards_interfaces INTERFACE)
target_include_directories(standards_interfaces
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

# IEEE/802.1AS/2021/CMakeLists.txt
# Dependent standard links against abstract interface
add_library(ieee_802_1as_2021 STATIC
    src/gptp_implementation.cpp
)

target_link_libraries(ieee_802_1as_2021
    PUBLIC standards_interfaces  # Abstract interface contract
)

# Select IEEE 1588 version at build time
set(IEEE1588_VERSION "2019" CACHE STRING "IEEE 1588 version to use (2019, 2008, 2002)")

if(IEEE1588_VERSION STREQUAL "2019")
    target_link_libraries(ieee_802_1as_2021
        PRIVATE ieee_1588_2019
    )
elseif(IEEE1588_VERSION STREQUAL "2008")
    target_link_libraries(ieee_802_1as_2021
        PRIVATE ieee_1588_2008
    )
elseif(IEEE1588_VERSION STREQUAL "latest")
    # "latest" alias points to most recent version
    target_link_libraries(ieee_802_1as_2021
        PRIVATE ieee_1588_2019
    )
endif()
```

### 4. Usage in Dependent Standards

```cpp
// IEEE/802.1AS/2021/src/gptp_implementation.cpp
#include "Common/interfaces/IIEEE1588Contract.hpp"

namespace IEEE {
namespace _802_1 {
namespace AS {
namespace _2021 {

class GPTPImplementation {
private:
    // Depend on abstract interface, not concrete version
    Standards::Interfaces::IIEEE1588Contract* ptp_provider_;
    
public:
    GPTPImplementation(Standards::Interfaces::IIEEE1588Contract* ptp)
        : ptp_provider_(ptp) {
        
        // Check required capabilities at initialization
        if (!ptp_provider_->hasCapability(
                Standards::Interfaces::IIEEE1588Contract::CAP_BASIC_SYNC)) {
            throw std::runtime_error("PTP provider missing required capability");
        }
        
        // Optional: use version-specific features if available
        if (ptp_provider_->hasCapability(
                Standards::Interfaces::IIEEE1588Contract::CAP_ENHANCED_ACCURACY)) {
            // Use enhanced accuracy features
        }
    }
    
    void synchronize() {
        // Use abstract interface
        Standards::Interfaces::IIEEE1588Contract::Timestamp current_time;
        ptp_provider_->getCurrentTime(current_time);
        
        // Work with PTP time...
    }
};

} // namespace _2021
} // namespace AS
} // namespace _802_1
} // namespace IEEE
```

## Contract Evolution Strategy

### Adding New Capabilities (Backward Compatible)

When a new IEEE 1588 version adds features:

1. **Add new capability flags** to `Capability` enum
2. **Add optional methods** with default implementations returning "not supported"
3. **Existing implementations** continue to work without changes

```cpp
class IIEEE1588Contract {
    // New in IEEE 1588-2025 (hypothetical)
    virtual bool getEnhancedSecurityStatus(SecurityStatus& status) const noexcept {
        // Default implementation: not supported
        return false;
    }
};
```

### Breaking Changes (Major Version)

If contract must break compatibility:

1. **Create new interface version**: `IIEEE1588ContractV2`
2. **Keep old interface** for backward compatibility
3. **Provide adapter** from old to new interface
4. **Migrate dependent standards** over time

## Testing Strategy

### 1. Interface Contract Tests

```cpp
// Common/interfaces/tests/test_ieee1588_contract.cpp
TEST(IEEE1588Contract, BasicContractCompliance) {
    // Test that implementations satisfy contract
    std::unique_ptr<IIEEE1588Contract> ptp_2019 = 
        std::make_unique<IEEE1588_2019_Implementation>();
    
    // Verify version reporting
    EXPECT_STREQ(ptp_2019->getVersion(), "2019");
    
    // Verify capability reporting
    EXPECT_TRUE(ptp_2019->hasCapability(IIEEE1588Contract::CAP_SECURITY));
    
    // Verify core operations
    IIEEE1588Contract::Timestamp ts;
    EXPECT_EQ(ptp_2019->getCurrentTime(ts), 0);
}
```

### 2. Cross-Version Compatibility Tests

```cpp
// Integration/tests/test_cross_version_compat.cpp
TEST(CrossVersionCompat, IEEE802_1AS_Works_With_IEEE1588_2019) {
    auto ptp = std::make_unique<IEEE1588_2019_Implementation>();
    GPTPImplementation gptp(ptp.get());
    
    // Verify gPTP works with IEEE 1588-2019
    EXPECT_NO_THROW(gptp.synchronize());
}

TEST(CrossVersionCompat, IEEE802_1AS_Works_With_IEEE1588_2008) {
    auto ptp = std::make_unique<IEEE1588_2008_Implementation>();
    GPTPImplementation gptp(ptp.get());
    
    // Verify gPTP works with IEEE 1588-2008
    EXPECT_NO_THROW(gptp.synchronize());
}
```

## Benefits

1. **Version Flexibility**: Switch IEEE 1588 versions without recompiling dependent standards
2. **Contract Stability**: Interface changes follow semantic versioning
3. **Independent Compilation**: Each submodule compiles against stable interface
4. **Runtime Capability Detection**: Dependent standards adapt to available features
5. **Testability**: Mock implementations for unit testing dependent standards
6. **Backward Compatibility**: New versions support old interfaces

## Migration Path

### Phase 1: Create Interface Contracts (Current)
- Define `IIEEE1588Contract.hpp` in Common/interfaces/
- Define contracts for IEEE 802.1Q, IEEE 1722, etc.

### Phase 2: Implement Contracts in Existing Standards
- IEEE 1588-2019 implements `IIEEE1588Contract`
- IEEE 802.1Q-2020 implements `IIEEE802_1QContract`

### Phase 3: Migrate Dependent Standards
- IEEE 802.1AS depends on `IIEEE1588Contract` (not concrete IEEE 1588-2019)
- IEEE 1722 depends on `IIEEE802_1ASContract`

### Phase 4: Add Version Selection
- CMake options for version selection
- Build-time linking to selected versions

### Phase 5: Testing and Validation
- Cross-version compatibility tests
- Performance benchmarking
- Continuous integration for all version combinations

## Example: Complete Cross-Standard Stack

```cmake
# Build IEEE 802.1AS-2021 with IEEE 1588-2019
cmake -DIEEE1588_VERSION=2019 ..

# Build IEEE 802.1AS-2021 with IEEE 1588-2008
cmake -DIEEE1588_VERSION=2008 ..

# Build with "latest" (current = 2019)
cmake -DIEEE1588_VERSION=latest ..
```

All configurations produce compatible binaries because dependent standards only use the abstract interface contract.

---

**Next Steps**:
1. Define all interface contracts in Common/interfaces/
2. Create compatibility matrix documenting which versions work together
3. Implement contracts in current standard submodules
4. Add cross-version integration tests
