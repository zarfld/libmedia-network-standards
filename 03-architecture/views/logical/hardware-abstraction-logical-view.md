---
specType: architecture
standard: "42010"
phase: "03-architecture"
version: "1.0.0"
author: "Architecture Team"
date: "2025-10-12"
status: "approved"
traceability:
  requirements:
    - "REQ-NF-PORTABILITY-001"
    - "REQ-F-CROSS-STANDARD-001"
  decisions:
    - "ADR-001"
    - "ADR-002"
---

# Logical View: Hardware-Agnostic Standards Architecture

## View Overview

This logical view describes the conceptual organization of the IEEE media networking standards implementation, emphasizing **complete hardware independence** through dependency injection and interface-based design.

## Architectural Concerns

- **Standards Purity**: IEEE protocol implementations must contain zero hardware-specific code
- **Multi-Vendor Support**: Same standards code must work across Intel, Broadcom, Marvell, and other hardware platforms
- **Testability**: Complete protocol validation without physical hardware via mock implementations
- **Maintainability**: Hardware changes must not require standards code modifications

## Logical Components

### Standards Layer (Hardware-Agnostic Core)
```
┌─────────────────────────────────────────────────────┐
│                Standards Layer                      │
│            (Zero Hardware Dependencies)             │
├─────────────────────────────────────────────────────┤
│  IEEE::1722_1::2021    │  AVnu::Milan::v1_2         │
│  ┌─────────────────┐   │  ┌─────────────────────┐   │
│  │ AVDECC Entity   │   │  │ Milan Discovery     │   │
│  │ Model           │   │  │ Extensions          │   │
│  └─────────────────┘   │  └─────────────────────┘   │
├─────────────────────────────────────────────────────┤
│  IEEE::1722::2016                                   │
│  ┌─────────────────┐   ┌─────────────────────────┐  │
│  │ AVTP Stream     │   │ Audio Format Handler    │  │
│  │ Manager         │   │ (AAF/CRF)              │  │
│  └─────────────────┘   └─────────────────────────┘  │
├─────────────────────────────────────────────────────┤
│  IEEE::802_1::AS::2021                              │
│  ┌─────────────────┐   ┌─────────────────────────┐  │
│  │ gPTP State      │   │ Best Master Clock       │  │
│  │ Machine         │   │ Algorithm               │  │
│  └─────────────────┘   └─────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

### Hardware Abstraction Interface Layer
```
┌─────────────────────────────────────────────────────┐
│           Common::interfaces:: (DI Layer)           │
├─────────────────────────────────────────────────────┤
│ NetworkInterface    │ TimerInterface               │
│ ┌─────────────────┐ │ ┌─────────────────────────┐  │
│ │ send_packet()   │ │ │ get_time_ns()           │  │
│ │ receive_packet()│ │ │ set_timer()             │  │
│ │ get_capabilities│ │ │ get_resolution()        │  │
│ └─────────────────┘ │ └─────────────────────────┘  │
├─────────────────────────────────────────────────────┤
│ ClockInterface      │ TransportInterface           │
│ ┌─────────────────┐ │ ┌─────────────────────────┐  │
│ │ sync_clock()    │ │ │ send_message()          │  │
│ │ get_offset()    │ │ │ register_handler()      │  │
│ │ set_discipline()│ │ │ get_status()            │  │
│ └─────────────────┘ │ └─────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

### Service Layer (Hardware-Specific Integration)
```
┌─────────────────────────────────────────────────────┐
│              Service Layer Implementations          │
│            (Hardware-Specific Adapters)             │
├─────────────────────────────────────────────────────┤
│ Intel Service       │ Broadcom Service             │
│ ┌─────────────────┐ │ ┌─────────────────────────┐  │
│ │ IntelNetwork    │ │ │ BroadcomNetwork         │  │
│ │ Service         │ │ │ Service                 │  │
│ │ implements      │ │ │ implements              │  │
│ │ NetworkInterface│ │ │ NetworkInterface        │  │
│ └─────────────────┘ │ └─────────────────────────┘  │
├─────────────────────────────────────────────────────┤
│ Generic Service     │ Mock Service (Testing)       │
│ ┌─────────────────┐ │ ┌─────────────────────────┐  │
│ │ LinuxGeneric    │ │ │ MockNetwork             │  │
│ │ Service         │ │ │ Service                 │  │
│ │ (OS sockets)    │ │ │ (Test Harness)          │  │
│ └─────────────────┘ │ └─────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

## Dependency Injection Pattern

### Standards Initialization
```cpp
// Standards receive interfaces via constructor injection
auto network = std::make_unique<IntelNetworkService>();
auto timer = std::make_unique<IntelTimerService>();
auto clock = std::make_unique<IntelClockService>();

// IEEE standards are completely hardware-agnostic
IEEE::_802_1::AS::_2021::GPTPStateMachine gptp(
    network.get(),  // Dependency injection
    timer.get(),
    clock.get()
);

IEEE::_1722::_2016::AVTPStreamManager avtp(
    network.get(),  // Reuses same interfaces
    &gptp          // Layer dependency (timing from gPTP)
);
```

### Testing with Mocks
```cpp
// Complete testing without hardware
auto mockNetwork = std::make_unique<MockNetworkService>();
auto mockTimer = std::make_unique<MockTimerService>();

// Same standards code, different service implementations
IEEE::_802_1::AS::_2021::GPTPStateMachine gptp(
    mockNetwork.get(),  // Mock for testing
    mockTimer.get(),
    mockClock.get()
);

// Verify protocol behavior without hardware
EXPECT_TRUE(gptp.process_sync_message(test_packet));
```

## Interface Contracts

All hardware abstraction interfaces follow strict contracts:

1. **Hardware Independence**: No hardware-specific types, constants, or behaviors
2. **Error Code Standardization**: Common error codes across all implementations  
3. **Capability Discovery**: Runtime detection of optional hardware features
4. **Performance Guarantees**: Timing requirements specified and enforced
5. **Thread Safety**: All interfaces safe for concurrent access

## Quality Attributes

### Portability
- **Metric**: Standards compile and run identically across all supported hardware platforms
- **Test**: Automated CI testing across Intel, Broadcom, and Mock implementations

### Testability  
- **Metric**: 100% protocol logic testable via mock implementations
- **Test**: Complete IEEE conformance validation without physical hardware

### Maintainability
- **Metric**: Hardware changes require zero modifications to standards code
- **Test**: Hardware vendor changes cause no standards layer recompilation

### Performance
- **Metric**: Hardware abstraction overhead <0.1% of total protocol processing time  
- **Test**: Timing analysis demonstrates negligible impact on IEEE timing requirements

## Standards Compliance

This logical architecture **directly implements IEEE specifications' own layering requirements**:

- **IEEE 802.1AS**: Timing synchronization foundation layer
- **IEEE 1722**: Audio/video transport over synchronized network
- **IEEE 1722.1**: Device control over AVTP transport
- **AVnu Milan**: Professional interoperability over IEEE stack

Each standards implementation contains **only** the protocol logic defined in its respective IEEE specification, with **zero** hardware integration code.

## Notes

The hardware abstraction logical architecture is **mandatory for IEEE standards compliance** and enables:

- **Multi-vendor interoperability** through vendor-neutral interfaces
- **Complete testability** through mock implementation support  
- **Standards certification** independent of hardware platform
- **Future-proof evolution** through stable interface boundaries

This architecture ensures that **standards remain pure** while enabling **practical hardware deployment** through the service layer.