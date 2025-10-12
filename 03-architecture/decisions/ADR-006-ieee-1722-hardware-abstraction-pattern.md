# ADR-006: Hardware Abstraction Pattern for Multi-vendor Support

## Status

Accepted

## Context

Professional media networking requires support for multiple hardware platforms from different vendors (Intel, Broadcom, Marvell) while maintaining consistent IEEE 1722-2016 AVTP protocol behavior. Each vendor provides different APIs, capabilities, and optimization opportunities.

### Hardware Platform Requirements

- **Intel Platforms**: Network adapters, embedded systems, server platforms
- **Broadcom Platforms**: Switch chips, network processors, embedded controllers  
- **Marvell Platforms**: Automotive Ethernet, industrial controllers, TSN switches
- **Generic Platforms**: Software fallback for unsupported hardware

### Technical Challenges

- **API Diversity**: Each vendor has different programming interfaces
- **Capability Variance**: Hardware capabilities vary significantly between platforms
- **Performance Optimization**: Platform-specific optimizations required for real-time performance
- **Maintenance Burden**: Multiple codepaths increase complexity and testing requirements
- **Real-time Constraints**: Hardware abstraction cannot compromise timing determinism

### Standards Requirements

- **IEEE 1722-2016 Compliance**: Protocol behavior must be consistent across all platforms
- **Professional Quality**: Sub-5ms audio latency, sub-16ms video latency requirements
- **Interoperability**: Multi-vendor networks must operate seamlessly

## Decision

We will implement a **Dependency Injection Hardware Abstraction Pattern** with runtime capability discovery:

### Architecture Pattern

```
┌─────────────────────────────────────┐
│        Protocol Layer               │ ← Pure IEEE 1722-2016 implementation
│     (Hardware Agnostic)             │
├─────────────────────────────────────┤
│     Hardware Abstraction Layer      │ ← C interface definitions
│      (Capability-based APIs)        │
├─────────────────────────────────────┤
│       Hardware Bridge Layer        │ ← Vendor-specific implementations
│    (Intel|Broadcom|Marvell|Generic) │
├─────────────────────────────────────┤
│       Vendor Hardware APIs          │ ← Platform-specific SDKs
│     (Intel HAL|Broadcom SDK|etc.)   │
└─────────────────────────────────────┘
```

### Key Components

#### 1. Hardware Abstraction Interface (C-based)

```c
// Network operations with capability-based selection
typedef struct {
    // Core network operations
    int (*send_packet)(const void* packet, size_t length, uint64_t timestamp);
    int (*receive_packet)(void* buffer, size_t* length, uint64_t* timestamp);
    
    // Capability discovery
    int (*get_capabilities)(network_capabilities_t* caps);
    
    // Performance optimization hooks
    int (*enable_hardware_timestamping)(void);
    int (*configure_traffic_shaping)(const qos_config_t* config);
    int (*allocate_zero_copy_buffer)(size_t size, void** buffer);
} network_interface_t;

// Timing operations with precision selection
typedef struct {
    // Time access
    uint64_t (*get_current_time)(void);
    int (*get_time_accuracy)(timing_accuracy_t* accuracy);
    
    // Event scheduling
    int (*schedule_event)(uint32_t delay_ns, event_callback_t callback);
    int (*cancel_event)(event_handle_t handle);
    
    // Hardware sync capabilities
    int (*sync_to_hardware_clock)(void);
    int (*get_sync_status)(sync_status_t* status);
} timing_interface_t;

// Memory operations with performance optimization
typedef struct {
    // Buffer management
    void* (*allocate_buffer)(size_t size, uint32_t alignment);
    int (*free_buffer)(void* buffer);
    
    // DMA operations (if supported)
    int (*map_dma_buffer)(void* buffer, dma_mapping_t* mapping);
    int (*unmap_dma_buffer)(dma_mapping_t* mapping);
    
    // Cache management
    int (*flush_cache)(void* buffer, size_t size);
    int (*invalidate_cache)(void* buffer, size_t size);
} memory_interface_t;
```

#### 2. Capability-based Runtime Selection

```c
// Hardware capability discovery
typedef struct {
    // Network capabilities
    bool hardware_timestamping_available;
    bool zero_copy_transmit_available;
    bool traffic_shaping_available;
    uint32_t max_packet_size;
    uint32_t tx_queue_count;
    uint32_t rx_queue_count;
    
    // Timing capabilities  
    bool hardware_ptp_available;
    uint32_t timing_resolution_ns;
    uint32_t timing_accuracy_ns;
    
    // Memory capabilities
    bool dma_available;
    uint32_t dma_alignment_requirement;
    size_t cache_line_size;
} hardware_capabilities_t;
```

#### 3. Hardware Bridge Implementation Pattern

```c
// Intel Hardware Bridge Implementation
static int intel_send_packet(const void* packet, size_t length, uint64_t timestamp) {
    // Use Intel-specific APIs for optimal performance
    return intel_hal_send_packet_with_timestamp(packet, length, timestamp);
}

static int intel_get_capabilities(network_capabilities_t* caps) {
    caps->hardware_timestamping_available = true;  // Intel supports HW timestamping
    caps->zero_copy_transmit_available = true;     // Intel DMA support
    caps->timing_accuracy_ns = 80;                 // Intel achieves ±80ns accuracy
    return 0;
}

static const network_interface_t intel_network_interface = {
    .send_packet = intel_send_packet,
    .receive_packet = intel_receive_packet,
    .get_capabilities = intel_get_capabilities,
    .enable_hardware_timestamping = intel_enable_hw_timestamping,
    .configure_traffic_shaping = intel_configure_qos,
    .allocate_zero_copy_buffer = intel_allocate_dma_buffer
};

// Broadcom Hardware Bridge Implementation  
static int broadcom_send_packet(const void* packet, size_t length, uint64_t timestamp) {
    // Use Broadcom SDK for switch fabric optimization
    return broadcom_sdk_transmit_packet(packet, length, timestamp);
}

static const network_interface_t broadcom_network_interface = {
    .send_packet = broadcom_send_packet,
    .receive_packet = broadcom_receive_packet,
    .get_capabilities = broadcom_get_capabilities,
    .enable_hardware_timestamping = broadcom_enable_hw_timestamping,
    .configure_traffic_shaping = broadcom_configure_switch_qos,
    .allocate_zero_copy_buffer = broadcom_allocate_switch_buffer
};

// Generic Software Fallback
static int generic_send_packet(const void* packet, size_t length, uint64_t timestamp) {
    // Use standard socket APIs with software timestamping
    return socket_send_with_software_timestamp(packet, length, timestamp);
}
```

#### 4. Runtime Platform Detection and Selection

```c
typedef enum {
    HARDWARE_PLATFORM_INTEL,
    HARDWARE_PLATFORM_BROADCOM, 
    HARDWARE_PLATFORM_MARVELL,
    HARDWARE_PLATFORM_GENERIC
} hardware_platform_t;

// Runtime hardware detection
hardware_platform_t detect_hardware_platform(void) {
    // Check for Intel hardware
    if (intel_hardware_detected()) {
        return HARDWARE_PLATFORM_INTEL;
    }
    
    // Check for Broadcom hardware
    if (broadcom_hardware_detected()) {
        return HARDWARE_PLATFORM_BROADCOM;
    }
    
    // Check for Marvell hardware
    if (marvell_hardware_detected()) {
        return HARDWARE_PLATFORM_MARVELL;
    }
    
    // Fallback to generic implementation
    return HARDWARE_PLATFORM_GENERIC;
}

// Interface selection based on detected platform
int avtp_initialize_hardware_interfaces(avtp_hardware_interfaces_t* interfaces) {
    hardware_platform_t platform = detect_hardware_platform();
    
    switch (platform) {
        case HARDWARE_PLATFORM_INTEL:
            interfaces->network = &intel_network_interface;
            interfaces->timing = &intel_timing_interface;
            interfaces->memory = &intel_memory_interface;
            break;
            
        case HARDWARE_PLATFORM_BROADCOM:
            interfaces->network = &broadcom_network_interface;
            interfaces->timing = &broadcom_timing_interface;
            interfaces->memory = &broadcom_memory_interface;
            break;
            
        case HARDWARE_PLATFORM_MARVELL:
            interfaces->network = &marvell_network_interface;
            interfaces->timing = &marvell_timing_interface;
            interfaces->memory = &marvell_memory_interface;
            break;
            
        default:
            interfaces->network = &generic_network_interface;
            interfaces->timing = &generic_timing_interface;
            interfaces->memory = &generic_memory_interface;
            break;
    }
    
    return 0;
}
```

## Rationale

### Pros

- **Hardware Independence**: Protocol layer completely isolated from hardware details
- **Performance Optimization**: Platform-specific optimizations without compromising portability  
- **Capability-based Selection**: Runtime optimization selection based on actual hardware capabilities
- **Maintainability**: Clean separation between standards and hardware implementation
- **Testability**: Mock interfaces enable comprehensive unit testing without hardware
- **Extensibility**: New hardware platforms can be added without protocol changes
- **Standards Compliance**: IEEE 1722-2016 behavior consistent across all platforms

### Cons

- **Complexity**: Additional abstraction layer increases architecture complexity
- **Performance Overhead**: Function pointer indirection adds minimal latency
- **Development Effort**: Multiple hardware bridge implementations required
- **Testing Scope**: Comprehensive validation across all supported platforms

### Risk Mitigation

- **Performance**: Compiler optimizations and careful interface design minimize overhead
- **Complexity**: Clear documentation and interface specifications
- **Development**: Incremental platform enablement, starting with most critical platforms
- **Testing**: Automated test frameworks with hardware simulation capabilities

## Consequences

### Implementation Impact

1. **Protocol Layer Benefits**
   - Single IEEE 1722-2016 implementation for all platforms
   - No hardware-specific code contamination
   - Comprehensive unit testing with mock interfaces
   - Consistent behavior across all supported hardware

2. **Hardware Bridge Requirements**
   - Intel bridge: Network adapter integration, hardware timestamping
   - Broadcom bridge: Switch fabric optimization, TSN QoS integration
   - Marvell bridge: Automotive Ethernet support, TSN controller integration
   - Generic bridge: Software fallback for unsupported platforms

3. **Testing Strategy**
   - **Unit Testing**: Mock hardware interfaces for protocol validation
   - **Integration Testing**: Real hardware validation on each supported platform
   - **Performance Testing**: Platform-specific optimization validation
   - **Interoperability Testing**: Multi-vendor network validation

### Development Guidelines

1. **Protocol Layer**
   - Never include hardware-specific headers or APIs
   - All hardware access through abstraction interfaces
   - Capability-based feature selection at runtime
   - Comprehensive error handling for all hardware operations

2. **Hardware Bridges**
   - Implement all required interface functions
   - Provide accurate capability reporting
   - Optimize for platform-specific performance characteristics
   - Handle hardware errors gracefully with fallback behavior

3. **Testing Requirements**
   - Mock interface implementations for unit testing
   - Platform-specific integration test suites
   - Performance benchmarking for each platform
   - Stress testing with multiple concurrent streams

### Migration Strategy

1. **Phase 1**: Hardware abstraction interface definitions and generic implementation
2. **Phase 2**: Intel platform bridge implementation and validation
3. **Phase 3**: Broadcom platform bridge implementation and validation  
4. **Phase 4**: Marvell platform bridge implementation and validation
5. **Phase 5**: Performance optimization and professional equipment validation

This hardware abstraction pattern ensures that IEEE 1722-2016 AVTP implementation achieves optimal performance on each supported platform while maintaining complete standards compliance and hardware independence.

---

**Decision Date**: January 27, 2025  
**Status**: Accepted  
**Supersedes**: None  
**References**: ADR-005 (IEEE 1722 AVTP Transport Architecture)