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
    - "REQ-F-001"
    - "REQ-F-002"
    - "REQ-NF-001"
---

# ADR-005: IEEE 1722-2016 AVTP Transport Architecture Pattern

## Status
Accepted

## Context

We need to implement IEEE 1722-2016 Audio Video Transport Protocol (AVTP) as the media transport layer for professional networking systems. AVTP provides standardized streaming for time-sensitive audio and video applications over Ethernet networks.

### Key Requirements:
- **Real-time Performance**: Sub-5ms audio latency, sub-16ms video latency
- **Professional Quality**: Uncompressed multi-channel audio, 4K+ video support
- **Hardware Independence**: Support for Intel, Broadcom, Marvell platforms
- **Standards Compliance**: Full IEEE 1722-2016 protocol implementation
- **Scalability**: 100+ concurrent streams, 512+ audio channels
- **Integration**: Seamless integration with IEEE 802.1AS gPTP timing

### Technical Challenges:
- Complex packet formats (AAF audio, CVF/RVF video)
- Real-time timing synchronization requirements
- Hardware-specific optimizations vs. portability
- Multi-format media processing complexity
- Quality of Service coordination

## Decision

We will implement IEEE 1722-2016 AVTP using a **Layered Architecture with Hardware Abstraction Pattern**:

### Core Architecture Pattern:
1. **Protocol Layer** (Hardware Agnostic)
   - Pure IEEE 1722-2016 protocol implementation
   - Format-specific processors (AAF, CVF, RVF)
   - Stream management and QoS coordination
   - Integration with IEEE 802.1AS timing foundation

2. **Hardware Abstraction Layer**
   - C-based interface definitions for network, timing, memory operations
   - Runtime capability discovery and optimization selection
   - Vendor-independent API surface for protocol layer

3. **Hardware Bridge Layer** (Vendor-Specific)
   - Intel-specific implementations
   - Broadcom-specific implementations  
   - Marvell-specific implementations
   - Generic fallback implementations

### Key Architectural Components:

#### AVTP Core Engine
- **Packet Processor**: AVTPDU validation, parsing, and construction
- **Stream Controller**: Stream lifecycle management and coordination
- **Timestamp Manager**: gPTP integration and presentation time calculation
- **QoS Manager**: Traffic shaping and priority queue coordination

#### Format Processors
- **AAF Handler**: Professional audio format processing
- **CVF Handler**: Compressed video format processing (H.264/H.265/JPEG-XS)
- **RVF Handler**: Raw video format processing (4K/8K uncompressed)

#### Stream Processing
- **Assembly Engine**: Packet sequencing and fragment reassembly
- **Disassembly Engine**: Packet fragmentation and sequence management
- **Buffer Manager**: Zero-copy buffer pools and memory optimization

### Hardware Abstraction Interface:
```c
typedef struct {
    // Network Operations
    int (*send_packet)(const void* packet, size_t length, uint64_t timestamp);
    int (*receive_packet)(void* buffer, size_t* length, uint64_t* timestamp);
    int (*get_network_capabilities)(network_capabilities_t* caps);
    
    // Timing Operations  
    uint64_t (*get_current_time)(void);
    int (*schedule_event)(uint32_t delay_ns, event_callback_t callback);
    int (*get_timing_accuracy)(timing_accuracy_t* accuracy);
    
    // Memory Operations
    void* (*allocate_buffer)(size_t size, uint32_t alignment);
    int (*free_buffer)(void* buffer);
    int (*map_dma_buffer)(void* buffer, dma_mapping_t* mapping);
} avtp_hardware_interface_t;
```

## Rationale

### Pros:
- **Standards Purity**: Protocol layer maintains complete IEEE 1722-2016 compliance
- **Hardware Independence**: Single protocol implementation supports all platforms
- **Performance Optimization**: Hardware-specific optimizations through abstraction
- **Maintainability**: Clean separation of concerns, isolated vendor code
- **Testability**: Mock hardware interfaces enable comprehensive unit testing
- **Scalability**: Efficient resource management and parallel processing
- **Real-time Determinism**: Predictable timing behavior across platforms
- **Professional Quality**: Optimized for broadcast and production requirements

### Cons:
- **Complexity**: Additional abstraction layer adds architectural complexity
- **Performance Overhead**: Function pointer indirection in critical paths
- **Integration Effort**: Multiple hardware bridge implementations required
- **Testing Scope**: Comprehensive testing across all supported platforms

### Risk Mitigation:
- **Performance**: Inline functions and compiler optimizations minimize overhead
- **Complexity**: Clear interface definitions and comprehensive documentation
- **Integration**: Incremental hardware platform enablement
- **Testing**: Automated testing framework with hardware simulation

## Consequences

### Implementation Requirements:
1. **Protocol Layer Development**
   - Pure C/C++ implementation with no hardware dependencies
   - Comprehensive IEEE 1722-2016 protocol compliance
   - Integration with IEEE 802.1AS gPTP timing foundation
   - Format-specific optimizations for audio and video processing

2. **Hardware Abstraction Development**
   - C interface definitions for all hardware operations
   - Runtime capability discovery and optimization selection
   - Performance-critical path optimizations
   - Comprehensive error handling and diagnostics

3. **Hardware Bridge Development**
   - Intel platform integration (network adapters, timing hardware)
   - Broadcom platform integration (switch chips, embedded systems)
   - Marvell platform integration (automotive Ethernet, TSN controllers)
   - Generic platform fallback for unsupported hardware

### Testing Strategy:
- **Unit Testing**: Mock hardware interfaces for protocol validation
- **Integration Testing**: Real hardware validation on each platform
- **Performance Testing**: Latency and throughput validation
- **Interoperability Testing**: Multi-vendor professional equipment validation

### Documentation Requirements:
- **IEEE 1722-2016 Implementation Guide**: Protocol compliance documentation
- **Hardware Abstraction API Reference**: HAL interface specifications
- **Platform Integration Guide**: Hardware bridge implementation guidelines
- **Performance Optimization Guide**: Platform-specific tuning recommendations

### Migration Path:
1. **Phase 1**: Core protocol implementation with software timing
2. **Phase 2**: Hardware abstraction layer and Intel platform bridge
3. **Phase 3**: Broadcom and Marvell platform bridges
4. **Phase 4**: Performance optimization and professional validation

This architecture ensures that IEEE 1722-2016 AVTP implementation maintains the highest standards compliance while providing the flexibility and performance required for professional media networking applications.

---

**Decision Date**: January 27, 2025  
**Status**: Accepted  
**Supersedes**: None  
**Referenced By**: ADR-006 (Hardware Abstraction Pattern)