---
specType: architecture
standard: 42010
phase: 03-architecture
version: 1.0.0
author: Architecture Team
date: "2025-10-12"
status: accepted
traceability:
  requirements:
    - REQ-F-STANDARDS-LAYERING-001
    - REQ-NF-INTEROP-001
---

# ADR-002: IEEE Media Networking Standards Layering Architecture

> Decision on implementing IEEE standards dependency hierarchy for media networking protocols

## Metadata

```yaml
adrId: ADR-002
status: accepted
relatedRequirements:
  - REQ-F-STANDARDS-LAYERING-001  # Standards shall follow IEEE protocol layering
  - REQ-NF-INTEROP-001  # Cross-standard interoperability required
relatedComponents:
  - IEEE::_802_1::AS::_2021  # gPTP timing foundation
  - IEEE::_1722::_2016       # AVTP transport layer
  - IEEE::_1722_1::_2021     # AVDECC application layer
relatedADRs:
  - ADR-001  # Hardware Abstraction Interfaces
supersedes: []
supersededBy: null
author: Architecture Team
date: 2025-10-12
reviewers: []
```

## Context

### Architectural Concern

IEEE media networking standards have **explicit dependencies** defined in specifications:
- **IEEE 1722.1 (AVDECC)** depends on IEEE 1722 (AVTP) for transport
- **IEEE 1722 (AVTP)** depends on IEEE 802.1AS (gPTP) for synchronized timing  
- **IEEE 802.1AS (gPTP)** provides timing foundation for all media networking

### Stakeholder Concerns

- **Standards Bodies**: Ensure implementation reflects official IEEE specification dependencies
- **Certification Authorities**: Verify standards compliance across all protocol layers
- **System Integrators**: Need predictable, testable integration between protocol layers
- **Audio/Video Engineers**: Require synchronized, low-latency media transport with device control

### Forces in Conflict

1. **Standards Correctness vs Implementation Simplicity**: Proper layering requires complex coordination but ensures IEEE compliance
2. **Performance vs Modularity**: Separate layers add abstraction overhead but enable independent testing/certification
3. **Timing Precision vs Flexibility**: Tight coordination ensures <100ns accuracy but constrains architectural choices
4. **Backward Compatibility vs Evolution**: Support legacy IEEE versions while enabling migration to current specifications

### Quality Attributes (ISO/IEC 25010)

- **Functional Correctness**: Implementation must match IEEE specification behavior exactly
- **Interoperability**: Different vendors' implementations must achieve certified interoperability  
- **Performance**: Media networking requires <1ms latency with <100ns timing accuracy
- **Maintainability**: Protocol updates must not break dependent layers

## Decision

**We will implement IEEE media networking standards following strict protocol layering hierarchy**:

```
Application Layer:  IEEE 1722.1-2021 (AVDECC) - Device Discovery & Control
                          ↓ depends on ↓
Transport Layer:    IEEE 1722-2016 (AVTP) - Audio/Video Stream Transport  
                          ↓ depends on ↓
Timing Layer:       IEEE 802.1AS-2021 (gPTP) - Precision Time Protocol
                          ↓ depends on ↓ 
Network Layer:      IEEE 802.1Q-2018 (VLAN/QoS) - Network Foundation
```

### Dependency Rules

1. **Upward Dependencies ALLOWED**: Higher layers may depend on lower layers
   - ✅ IEEE 1722.1 CAN use IEEE 1722 and IEEE 802.1AS
   - ✅ IEEE 1722 CAN use IEEE 802.1AS

2. **Downward Dependencies FORBIDDEN**: Lower layers cannot depend on higher layers  
   - ❌ IEEE 802.1AS CANNOT use IEEE 1722 or IEEE 1722.1
   - ❌ IEEE 1722 CANNOT use IEEE 1722.1

3. **Cross-Layer Communication**: Via interfaces only, no direct coupling

## Status

**ACCEPTED** - This layering architecture ensures IEEE specification compliance while enabling modular development and testing.

## Rationale

### Why This Option Best Addresses Forces

1. **IEEE Compliance Guaranteed**: Architecture mirrors official IEEE specifications exactly
2. **Certification Achievable**: Each layer can be certified independently and in combination  
3. **Performance Maintained**: Direct interface calls avoid significant overhead while preserving timing
4. **Evolution Supported**: New IEEE versions can be added without breaking existing layers

### Standards-Based Design Decisions

#### Timing Coordination (Critical for Media)
```cpp
// IEEE 802.1AS provides synchronized time to all upper layers
namespace IEEE::_802_1::AS::_2021 {
    class GPTPDomain {
        uint64_t get_synchronized_time_ns();  // Foundation timing service
    };
}

// IEEE 1722 uses gPTP time for presentation timestamps
namespace IEEE::_1722::_2016::avtp {
    class StreamDataHeader {
        void set_presentation_time(uint64_t gptp_time);  // Uses 802.1AS time
    };
}

// IEEE 1722.1 coordinates timing across device control
namespace IEEE::_1722_1::_2021::aem {
    class StreamDescriptor {
        void set_stream_timing(const TimingSyncInterface& gptp);  // Coordinates via interface
    };
}
```

#### Transport Independence
```cpp
// IEEE 1722.1 uses IEEE 1722 for transport without tight coupling
namespace IEEE::_1722_1::_2021::aecp {
    class AEMCommand {
        // Uses transport interface, not direct IEEE 1722 calls
        void send_command(const TransportInterface& avtp_transport);
    };
}
```

## Considered Alternatives

| Alternative | Summary | Pros | Cons | Reason Not Chosen |
|------------|---------|------|------|-------------------|
| Monolithic Implementation | Single library with all IEEE standards | Simple build, maximum performance | Impossible to certify, violates IEEE architecture | Cannot achieve standards compliance |
| Flat Architecture | All standards as peers | Simple dependencies | No timing coordination, breaks IEEE specs | Violates IEEE layering requirements |
| Microservices | Each standard as separate service | Ultimate isolation | IPC overhead kills timing, complexity | Cannot meet <100ns timing requirements |
| Plugin Architecture | Runtime-loaded standard modules | Maximum flexibility | Complex timing coordination | Over-engineering for deterministic protocols |
| Direct Linking | Standards call each other directly | Maximum performance | Tight coupling, untestable | Violates dependency inversion principle |

## Implementation Guidelines (IEEE 1016-2009 Format)

### Cross-Layer Interface Design

```cpp
// TimingSyncInterface - 802.1AS provides to 1722/1722.1
class TimingSyncInterface {
public:
    virtual uint64_t get_synchronized_time_ns() = 0;
    virtual int register_timing_client(TimingClient* client) = 0;
    virtual bool is_synchronized() const = 0;
    virtual uint32_t get_timing_accuracy_ns() const = 0;
};

// TransportInterface - 1722 provides to 1722.1  
class TransportInterface {
public:
    virtual int send_avtp_packet(const void* packet, size_t length) = 0;
    virtual int register_stream_handler(uint64_t stream_id, StreamHandler* handler) = 0;
    virtual bool is_stream_connected(uint64_t stream_id) const = 0;
};
```

### Cross-Layer Coordination Service

```cpp
namespace Common::integration {
    class StandardsCoordinator {
        // Enforces IEEE layering rules at runtime
        bool validate_dependency(StandardId dependent, StandardId dependency) {
            // Prevent architectural violations
            if (dependency == IEEE_1722_1 && dependent == IEEE_802_1AS) {
                return false;  // ❌ Forbidden downward dependency
            }
            return true;  // ✅ Valid dependency
        }
        
        // Cross-layer message passing with timing guarantees  
        int forward_message(const StandardsMessage& msg, uint64_t deadline_ns);
    };
}
```

## Verification Criteria (IEEE 1012-2016)

### Architecture Compliance Tests

1. **Dependency Direction Test**: Verify no lower layer imports upper layer headers
2. **Interface Isolation Test**: All cross-layer communication via interfaces only  
3. **Timing Coordination Test**: Verify <100ns accuracy across all layers
4. **Standards Certification Test**: Each layer passes relevant IEEE certification independently

### Integration Test Scenarios

```cpp
// Scenario 1: AVDECC Device Discovery with gPTP Sync
TEST(LayerIntegration, AVDECCDiscoveryWithGPTPSync) {
    // 1. Start IEEE 802.1AS gPTP synchronization
    gptp_domain.start_synchronization();
    EXPECT_TRUE(gptp_domain.wait_for_sync(timeout_5s));
    
    // 2. Start IEEE 1722 AVTP transport (depends on gPTP)
    avtp_engine.initialize(gptp_domain.get_timing_interface());
    
    // 3. Start IEEE 1722.1 AVDECC (depends on both)
    avdecc_controller.initialize(
        gptp_domain.get_timing_interface(),
        avtp_engine.get_transport_interface()
    );
    
    // 4. Verify full-stack device discovery
    auto devices = avdecc_controller.discover_devices();
    EXPECT_GT(devices.size(), 0);
}
```

### Success Metrics

- ✅ Each layer compiles independently  
- ✅ Cross-layer interfaces provide required functionality
- ✅ Timing accuracy <100ns maintained across all layers
- ✅ IEEE certification tests pass for each individual standard
- ✅ Full-stack integration achieves Milan interoperability requirements

## Dependencies

### Upstream Dependencies
- **ADR-001**: Hardware Abstraction Interfaces (provides foundation for all layers)

### Downstream Dependencies  
- **IEEE 802.1AS Implementation**: Must provide TimingSyncInterface
- **IEEE 1722 Implementation**: Must use TimingSyncInterface, provide TransportInterface
- **IEEE 1722.1 Implementation**: Must use TimingSyncInterface and TransportInterface

### Standards References
- **IEEE 802.1AS-2021**: Generalized Precision Time Protocol specification  
- **IEEE 1722-2016**: Audio Video Transport Protocol specification
- **IEEE 1722.1-2021**: Audio Video Device Control and Configuration specification
- **Milan Specification v1.2**: AVnu Alliance professional interoperability requirements

## Notes

This layering architecture is **mandatory for IEEE compliance** and cannot be compromised for performance or convenience. The interface overhead (~1-2ns per call) is negligible compared to network operations (~1μs) while providing essential benefits:

- **Standards Certification**: Each layer certifiable independently
- **Vendor Interoperability**: Different vendors can implement different layers  
- **Evolution Path**: New IEEE versions deployable without breaking existing layers
- **Testing Isolation**: Protocol logic testable without full stack integration

The architecture directly implements the IEEE specifications' own layering requirements, ensuring both compliance and practical deployability.