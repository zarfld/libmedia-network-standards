---
specType: architecture
standard: "42010"
phase: "03-architecture"
version: "1.0.0"
author: "Architecture Technology Team"
date: "2025-10-12"
status: "approved"
traceability:
  requirements:
    - "REQ-NF-001"
    - "REQ-NF-002"
    - "REQ-NF-003"
---

# IEEE 1722.1-2021 AVDECC Technology Stack Recommendations

**Standards Compliance**: IEEE Std 1722.1-2021 "IEEE Standard for Device Discovery, Connection Management, and Control Protocol for IEEE 802 Based Networks"  
**Architecture Reference**: IEEE 1722.1-2021 AVDECC Architecture Specification  
**Document Version**: 1.0  
**Date**: October 12, 2025  
**Prepared by**: Architecture Technology Team

## Architectural Decisions

This technology stack implements:
- **ADR-001**: Hardware Abstraction Interfaces - Generic AVDECC hardware APIs
- **ADR-002**: IEEE Standards Layering - AVDECC device control above AVTP transport
- **ADR-007**: IEEE 1722.1 AVDECC Entity Model Architecture - Comprehensive entity representation
- **ADR-008**: IEEE 1722.1 Protocol State Machine Architecture - Event-driven protocol implementation
- **ADR-009**: Milan Professional Audio Integration - Milan compatibility layer

## Stakeholder Concerns

- **Audio Professionals**: Require reliable, low-latency device control for professional audio production
- **Equipment Manufacturers**: Need complete IEEE 1722.1-2021 compliance with Milan certification capability
- **Network Engineers**: Require efficient protocol implementation with predictable network behavior
- **Software Developers**: Need maintainable, well-documented AVDECC implementation with comprehensive APIs

## Architectural Viewpoints

- **Protocol Viewpoint**: Complete IEEE 1722.1-2021 protocol implementation with state machine precision
- **Performance Viewpoint**: Real-time device control with sub-millisecond command response requirements
- **Integration Viewpoint**: Seamless integration with IEEE 1722 AVTP and IEEE 802.1AS gPTP layers
- **Professional Viewpoint**: Milan professional audio profile support for broadcast and production environments

## Core Technology Stack

### Programming Language: C++17 with C Interfaces

**Rationale**:
- **Protocol Precision**: Exact control over IEEE 1722.1-2021 state machine implementation
- **Real-time Performance**: Deterministic memory management and zero-overhead abstractions
- **Hardware Integration**: Direct access to network hardware timestamping and control features
- **Professional Ecosystem**: Industry standard for professional audio and networking equipment
- **Standards Compliance**: Precise control over protocol timing and message formatting

**Implementation Pattern**:
```cpp
// C++ implementation for complex protocol logic
namespace IEEE::_1722_1::_2021 {
    class AVDECCEntity {
        // Complex state management and protocol implementation
    };
}

// C interface for cross-platform compatibility
extern "C" {
    avdecc_handle_t avdecc_entity_create(const avdecc_config_t* config);
    int avdecc_send_command(avdecc_handle_t handle, const aem_command_t* command);
    int avdecc_get_entity_info(avdecc_handle_t handle, entity_info_t* info);
}
```

**Alternatives Considered**:
- **Pure C**: Maximum portability but complex for IEEE 1722.1 state machine management
- **Rust**: Modern safety but limited professional audio ecosystem and hardware driver support
- **Go**: Simplified concurrency but garbage collection incompatible with real-time requirements

**Technology Constraints**:
- **No Exceptions**: Real-time determinism requires error codes over exception handling
- **No Dynamic Allocation**: Protocol processing uses pre-allocated memory pools
- **Constexpr**: Compile-time computation for protocol constants and validation
- **RAII**: Resource management for network handles and entity state

### Build System: CMake with Cross-Platform Support

**Rationale**:
- **Cross-platform**: Native support for Linux, Windows, and embedded targets
- **IEEE Standards Integration**: Clean integration with existing IEEE protocol libraries
- **Professional Tooling**: Industry standard for professional audio and networking software
- **Package Management**: Integration with Conan and vcpkg for dependency management

**Configuration Pattern**:
```cmake
# IEEE 1722.1 AVDECC Implementation
add_library(ieee_1722_1_2021 STATIC
    IEEE/1722.1/2021/adp/adp_state_machine.cpp
    IEEE/1722.1/2021/aecp/aecp_state_machine.cpp
    IEEE/1722.1/2021/acmp/acmp_state_machine.cpp
    IEEE/1722.1/2021/aem/entity_model.cpp
)

target_link_libraries(ieee_1722_1_2021
    ieee_1722_2016              # AVTP transport dependency
    ieee_802_1_as_2021          # gPTP timing dependency
    avnu_milan_v12              # Milan extensions
    standards_common            # Hardware abstraction
)

# Professional audio certification testing
add_executable(avdecc_milan_certification_tests
    tests/milan/test_discovery_extensions.cpp
    tests/milan/test_stream_formats.cpp
    tests/milan/test_redundancy_behavior.cpp
)
```

## Device Control Technology Stack

### Entity Model Framework: Template-Based Descriptors

**Rationale**:
- **Type Safety**: Compile-time validation of IEEE 1722.1-2021 descriptor structures
- **Performance**: Zero-overhead abstraction over raw protocol structures  
- **Extensibility**: Clean support for Milan extensions and vendor-specific descriptors
- **Memory Efficiency**: Efficient packing and alignment for professional audio descriptor sets

**Implementation Approach**:
```cpp
// Template-based descriptor system
template<typename DescriptorType>
class Descriptor {
public:
    static constexpr uint16_t descriptor_type = DescriptorType::TYPE_VALUE;
    
    bool validate() const { return static_cast<const DescriptorType*>(this)->validate_impl(); }
    size_t serialize(uint8_t* buffer, size_t max_len) const;
    bool deserialize(const uint8_t* buffer, size_t length);
};

// IEEE 1722.1-2021 entity descriptor
struct EntityDescriptor : public Descriptor<EntityDescriptor> {
    static constexpr uint16_t TYPE_VALUE = 0x0000;
    
    uint64_t entity_id;
    uint64_t entity_model_id;
    uint32_t entity_capabilities;
    // ... other fields per IEEE specification
    
    bool validate_impl() const;
};
```

### Protocol State Machine: Event-Driven Architecture

**Primary Choice**: Custom Event Loop with High-Precision Timers

**Rationale**:
- **IEEE 1722.1 Compliance**: Precise timing control for protocol state machines
- **Real-time Performance**: Deterministic event processing without blocking operations
- **Concurrent Protocols**: ADP, AECP, and ACMP operate simultaneously without interference
- **Professional Audio**: Sub-millisecond command response for broadcast applications

**Architecture Pattern**:
```cpp
class AVDECCProtocolEngine {
private:
    // High-precision event queue
    EventQueue<ProtocolEvent, 1000> event_queue_;
    
    // IEEE 1722.1-2021 protocol state machines
    std::unique_ptr<ADPStateMachine> adp_state_machine_;
    std::unique_ptr<AECPStateMachine> aecp_state_machine_;
    std::unique_ptr<ACMPStateMachine> acmp_state_machine_;
    
    // Hardware abstraction
    NetworkInterface& network_interface_;
    TimerManager& timer_manager_;
    
public:
    void processEvents() {
        while (auto event = event_queue_.tryDequeue()) {
            distributeEvent(*event);
        }
    }
};
```

**Alternative Considerations**:
- **Boost.Statechart**: Feature-rich but higher overhead than required for IEEE protocols
- **QT State Machine**: GUI-focused, not optimized for network protocol timing requirements
- **libev/libevent**: C-based, lacks type safety for IEEE 1722.1 message structures

## Network Abstraction Technology Stack

### Hardware Abstraction Layer: Dependency Injection Pattern

**Interface Design**:
```cpp
// Hardware-agnostic network interface
class NetworkInterface {
public:
    virtual ~NetworkInterface() = default;
    
    // IEEE 1722.1 packet transmission
    virtual Result sendAVDECCPacket(const MacAddress& dest, const std::vector<uint8_t>& payload) = 0;
    virtual Result registerPacketHandler(PacketType type, PacketHandler handler) = 0;
    
    // Hardware timestamping for precise AVDECC timing
    virtual Result getHardwareTimestamp(std::chrono::nanoseconds& timestamp) = 0;
    virtual bool supportsHardwareTimestamping() const = 0;
    
    // Network interface information
    virtual MacAddress getLocalMacAddress() const = 0;
    virtual std::string getInterfaceName() const = 0;
    virtual NetworkCapabilities getCapabilities() const = 0;
};
```

### Primary: Raw Ethernet with Hardware Timestamping

**Rationale**:
- **IEEE 1722.1 Requirement**: AVDECC operates directly over Ethernet (IEEE 802.3)
- **Timing Precision**: Hardware timestamping essential for professional audio synchronization
- **QoS Integration**: Direct integration with IEEE 802.1Q traffic classes for professional audio
- **Multicast Support**: Native support for AVDECC multicast discovery and advertisement

**Implementation Approach**:
```cpp
#ifdef _WIN32
// Windows raw socket implementation
class WindowsRawEthernet : public NetworkInterface {
    SOCKET raw_socket_;
    HANDLE completion_port_;
    // Windows-specific implementation
};
#else
// Linux AF_PACKET implementation  
class LinuxRawEthernet : public NetworkInterface {
    int socket_fd_;
    struct sockaddr_ll socket_address_;
    // Linux-specific implementation using AF_PACKET
};
#endif
```

## Professional Audio Extensions

### Milan Compatibility Framework

**Architecture**:
```cpp
namespace AVnu::Milan {

class MilanExtensionManager {
public:
    // Milan version detection and negotiation
    MilanVersion detectMilanVersion(const EntityDescriptor& entity) const;
    bool validateMilanCompliance(const EntityModel& model) const;
    
    // Professional audio format support
    std::vector<StreamFormat> getMilanSupportedFormats() const;
    bool isRedundancyRequired(const StreamFormat& format) const;
    
    // Milan certification testing
    bool runCertificationTests() const;
    CertificationReport generateCertificationReport() const;
};

} // namespace AVnu::Milan
```

### Real-Time Operating System Integration

#### Primary: FreeRTOS for Embedded Platforms

**Configuration for AVDECC**:
```c
// FreeRTOS configuration for IEEE 1722.1 real-time requirements
#define configTICK_RATE_HZ                1000000  // 1μs precision for AVDECC timing
#define configMAX_PRIORITIES              16
#define configUSE_PREEMPTION              1
#define configUSE_TIME_SLICING            0        // Deterministic scheduling

// AVDECC task priorities
#define AVDECC_ADP_TASK_PRIORITY         (configMAX_PRIORITIES - 1)
#define AVDECC_AECP_TASK_PRIORITY        (configMAX_PRIORITIES - 2)
#define AVDECC_ACMP_TASK_PRIORITY        (configMAX_PRIORITIES - 3)
#define AVDECC_TIMER_TASK_PRIORITY       (configMAX_PRIORITIES - 1)
```

#### Alternative: Linux Real-Time (PREEMPT_RT)

**Configuration**:
```c
// Linux RT configuration for AVDECC
#define AVDECC_THREAD_POLICY             SCHED_FIFO
#define AVDECC_ADP_PRIORITY              99
#define AVDECC_AECP_PRIORITY             98  
#define AVDECC_ACMP_PRIORITY             97
#define AVDECC_TIMER_PRIORITY            99

// Real-time thread creation
pthread_attr_t rt_attr;
struct sched_param rt_param = { .sched_priority = AVDECC_ADP_PRIORITY };
pthread_attr_setschedpolicy(&rt_attr, AVDECC_THREAD_POLICY);
pthread_attr_setschedparam(&rt_attr, &rt_param);
```

## Testing and Validation Framework

### IEEE 1722.1 Compliance Testing

#### Primary: Google Test with IEEE Protocol Vectors

**Rationale**:
- **Professional Standard**: Industry-standard C++ testing framework  
- **IEEE Integration**: Custom matchers for IEEE 1722.1 protocol validation
- **Performance Testing**: Benchmarking support for real-time requirements
- **Mock Support**: Integration with Google Mock for hardware abstraction testing

**Implementation Pattern**:
```cpp
// IEEE 1722.1-2021 compliance test framework
class AVDECC1722_1_ComplianceTest : public ::testing::Test {
protected:
    void SetUp() override {
        entity_model_ = std::make_unique<EntityModel>(test_entity_config_);
        mock_network_ = std::make_shared<MockNetworkInterface>();
        avdecc_engine_ = std::make_unique<AVDECCProtocolEngine>(
            *entity_model_, *mock_network_);
    }
    
    std::unique_ptr<EntityModel> entity_model_;
    std::shared_ptr<MockNetworkInterface> mock_network_;
    std::unique_ptr<AVDECCProtocolEngine> avdecc_engine_;
};

TEST_F(AVDECC1722_1_ComplianceTest, EntityAdvertisementTiming) {
    // Test ADP advertisement timing per IEEE 1722.1-2021 Section 6.2.1.7
    avdecc_engine_->startAdvertising();
    
    EXPECT_CALL(*mock_network_, sendAVDECCPacket(_, _))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(Result::SUCCESS));
    
    // Verify advertisement timing compliance
    auto start_time = std::chrono::steady_clock::now();
    avdecc_engine_->processEvents();
    auto end_time = std::chrono::steady_clock::now();
    
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    EXPECT_GE(elapsed.count(), 2000);  // Minimum 2 second interval
    EXPECT_LE(elapsed.count(), 62000); // Maximum 62 second interval
}
```

### Milan Certification Framework

**Automated Testing**:
```cpp
class MilanCertificationTestSuite {
public:
    struct CertificationResult {
        bool discovery_compliance;
        bool stream_format_compliance;
        bool redundancy_compliance;
        bool timing_compliance;
        std::vector<std::string> failure_details;
    };
    
    CertificationResult runFullCertification(const EntityModel& entity) const;
    
private:
    bool testMilanDiscoveryExtensions(const EntityModel& entity) const;
    bool testMilanStreamFormats(const EntityModel& entity) const;
    bool testMilanRedundancyBehavior(const EntityModel& entity) const;
    bool testMilanTimingRequirements(const EntityModel& entity) const;
};
```

## Performance Optimization Technologies

### Memory Management: Pre-allocated Pools

**Real-time Memory Strategy**:
```cpp
// Pre-allocated memory pools for real-time AVDECC processing
template<typename T, size_t PoolSize>
class RealtimeMemoryPool {
public:
    T* allocate() {
        if (free_list_.empty()) return nullptr;
        T* ptr = free_list_.back();
        free_list_.pop_back();
        return ptr;
    }
    
    void deallocate(T* ptr) {
        if (ptr) free_list_.push_back(ptr);
    }
    
private:
    std::array<T, PoolSize> storage_;
    std::vector<T*> free_list_;
};

// AVDECC message pools
RealtimeMemoryPool<AEMCommand, 100> aem_command_pool_;
RealtimeMemoryPool<ACMPMessage, 50> acmp_message_pool_;
```

### Network Performance: Zero-Copy Packet Processing

**High-Performance Packet Handling**:
```cpp
class ZeroCopyPacketProcessor {
public:
    // Direct hardware buffer access
    void processIncomingPacket(const EthernetFrame& frame) {
        // Parse AVDECC header without copying
        const auto* avdecc_header = reinterpret_cast<const AVDECCHeader*>(
            frame.payload_ptr());
        
        switch (avdecc_header->message_type) {
            case AVDECC_ADP_MESSAGE:
                processADPMessage(frame.payload_ptr(), frame.payload_size());
                break;
            case AVDECC_AECP_MESSAGE:
                processAECPMessage(frame.payload_ptr(), frame.payload_size());
                break;
            // ... other message types
        }
    }
};
```

## Deployment and Operations Technology Stack

### Configuration Management: JSON with Schema Validation

**Primary Choice**: nlohmann::json with JSON Schema Validation

**AVDECC Configuration Format**:
```json
{
  "entity": {
    "entity_id": "0x0001020304050607",
    "entity_model_id": "0x0011223344556677",
    "entity_name": "Professional Audio Mixer",
    "configurations": [
      {
        "configuration_index": 0,
        "configuration_name": "Standard Configuration",
        "stream_inputs": [
          {
            "stream_index": 0,
            "stream_name": "Audio Input 1-8",
            "formats": ["0x00A0020840001000"]
          }
        ]
      }
    ]
  },
  "milan": {
    "enabled": true,
    "version": "1.2",
    "certification_level": 2,
    "redundancy_support": true
  }
}
```

### Monitoring and Diagnostics: Structured Logging

**Implementation**:
```cpp
// Structured logging for AVDECC protocol monitoring
class AVDECCLogger {
public:
    enum class Level { DEBUG, INFO, WARNING, ERROR, CRITICAL };
    
    void logProtocolEvent(Level level, const std::string& protocol,
                         const std::string& event, const json& details) {
        json log_entry = {
            {"timestamp", getCurrentTimestamp()},
            {"level", levelToString(level)},
            {"protocol", protocol},
            {"event", event},
            {"details", details}
        };
        
        writeLogEntry(log_entry);
    }
    
    // IEEE 1722.1-specific logging
    void logADPEvent(const std::string& event, const EntityInfo& entity) {
        logProtocolEvent(Level::INFO, "ADP", event, {
            {"entity_id", entity.entity_id},
            {"entity_name", entity.entity_name}
        });
    }
};
```

## Success Criteria

### Performance Requirements
- **Command Response Time**: AEM commands processed within 100ms (simple) / 500ms (complex)
- **Discovery Performance**: Entity discovery completed within 2 seconds of network connection
- **Connection Establishment**: Stream connections established within 500ms including validation
- **Memory Efficiency**: Memory usage scales linearly with entity count (O(n) complexity)

### Standards Compliance
- **IEEE 1722.1-2021**: 100% compliance with all mandatory protocol requirements
- **Milan Certification**: Pass AVnu Alliance Milan v1.2/v2.0a certification tests
- **Interoperability**: Validated operation with major professional audio equipment vendors
- **Multi-platform**: Consistent behavior across Windows and Linux operating systems

### Quality Metrics
- **Test Coverage**: >90% code coverage with IEEE protocol compliance tests
- **Real-time Performance**: <1ms jitter for 99.9% of protocol operations
- **Memory Safety**: Zero memory leaks or buffer overflows in 24-hour stress testing
- **Concurrent Operation**: Support 200+ simultaneous entities without performance degradation

## References

- **IEEE Std 1722.1-2021**: IEEE Standard for Device Discovery, Connection Management, and Control Protocol
- **AVnu Milan Specification v1.2/v2.0a**: Professional Audio Profile for Time-Sensitive Networking
- **IEEE Std 1722-2016**: Audio Video Transport Protocol (AVTP) dependency
- **IEEE Std 802.1AS-2021**: Generalized Precision Time Protocol (gPTP) timing foundation
- **Professional Audio Standards**: AES67, AES70, Milan professional audio specifications