---
specType: architecture
standard: "42010"
phase: "03-architecture"
version: "1.0.0"
author: "Architecture Team"
date: "2025-10-12"
status: "draft"
traceability:
  requirements:
    - "REQ-NF-002"
    - "REQ-NF-003"
    - "REQ-NF-004"
---

# IEEE 802.1AS-2021 gPTP Technology Stack Recommendations

## Architectural Decisions

This technology stack implements:
- **ADR-001**: Hardware Abstraction Interfaces - Generic timing hardware APIs
- **ADR-002**: IEEE Standards Layering - gPTP as foundational timing layer
- **ADR-003**: Multi-Domain Architecture - Domain isolation and cross-domain sync
- **ADR-004**: Enhanced Path Delay Architecture - Dual P2P/E2E mechanisms

## Stakeholder Concerns

- **Audio Engineers**: Require deterministic real-time performance with microsecond precision
- **Video Engineers**: Need consistent timing across distributed video production systems  
- **Network Engineers**: Must integrate with existing network infrastructure and management
- **Hardware Vendors**: Need portable APIs that work across different timing hardware
- **System Integrators**: Require predictable performance and diagnostic capabilities

## Architectural Viewpoints

- **Performance Viewpoint**: Real-time constraints and timing precision requirements
- **Portability Viewpoint**: Hardware abstraction enabling multi-vendor support
- **Safety Viewpoint**: Deterministic behavior and fault tolerance requirements
- **Integration Viewpoint**: Compatibility with existing network and audio/video systems

## Core Technology Stack

### Programming Language: C/C++
**Rationale**: Real-time performance requirements, hardware access, existing ecosystem

**Recommended Standards**:
- **C17/C18** for core protocol implementation
- **C++17** for object-oriented architecture components
- **MISRA C** guidelines for safety-critical applications

### Real-Time Operating System

#### **Primary: FreeRTOS**
```c
// FreeRTOS configuration for gPTP
#define configTICK_RATE_HZ              1000000  // 1μs tick for precision
#define configMAX_PRIORITIES            16
#define configUSE_PREEMPTION            1
#define configUSE_TIME_SLICING          0        // Deterministic scheduling
#define configCHECK_FOR_STACK_OVERFLOW  2

// gPTP task priorities
#define GPTP_SYNC_TASK_PRIORITY         (configMAX_PRIORITIES - 1)
#define GPTP_PDELAY_TASK_PRIORITY       (configMAX_PRIORITIES - 2)  
#define GPTP_BMCA_TASK_PRIORITY         (configMAX_PRIORITIES - 3)
```

#### **Alternative: Linux RT (PREEMPT_RT)**
```c
// Linux RT configuration
#define GPTP_THREAD_POLICY              SCHED_FIFO
#define GPTP_SYNC_PRIORITY              99
#define GPTP_PDELAY_PRIORITY            98
#define GPTP_MGMT_PRIORITY              50

// CPU isolation for timing threads
// Boot parameter: isolcpus=2,3 rcu_nocbs=2,3
```

### Hardware Abstraction Layer

#### **Timing Hardware Interface**
```c
typedef struct gptp_timing_hal {
    // Hardware identification
    char vendor_name[32];
    char device_model[64];
    uint32_t api_version;
    
    // Timing capabilities
    struct {
        bool hardware_timestamping;
        bool multi_domain_support;
        uint32_t timestamp_resolution_ns;
        uint32_t frequency_adjustment_ppb_max;
    } capabilities;
    
    // Core timing operations
    int (*get_time)(uint8_t domain, struct timespec* ts);
    int (*set_time)(uint8_t domain, const struct timespec* ts);
    int (*adjust_frequency)(uint8_t domain, int32_t ppb);
    
    // Packet timestamping
    int (*tx_timestamp)(const void* packet, size_t len, struct timespec* ts);
    int (*rx_timestamp)(const void* packet, size_t len, struct timespec* ts);
    
    // Multi-domain operations
    int (*create_domain)(uint8_t domain_id, const domain_config_t* config);
    int (*destroy_domain)(uint8_t domain_id);
} gptp_timing_hal_t;
```

### Network Stack Integration

#### **Primary: Raw Ethernet**
```c
// Direct Ethernet frame processing for minimal latency
typedef struct gptp_eth_interface {
    int socket_fd;                      // Raw socket for gPTP frames
    uint8_t local_mac[6];              // Local MAC address
    uint16_t ethertype;                // 0x88F7 for gPTP
    
    // Hardware timestamping socket options
    struct hwtstamp_config hw_config;
    int timestamping_flags;            // SOF_TIMESTAMPING_*
    
    // Send/receive operations
    ssize_t (*send_frame)(const void* frame, size_t len, struct timespec* tx_ts);
    ssize_t (*recv_frame)(void* frame, size_t max_len, struct timespec* rx_ts);
} gptp_eth_interface_t;
```

#### **Alternative: AF_PACKET with PACKET_MMAP**
```c
// High-performance packet processing for Linux
struct gptp_packet_ring {
    void* ring_buffer;
    uint32_t frame_size;
    uint32_t frame_count;
    uint32_t head;
    uint32_t tail;
};
```

### Configuration Management

#### **Primary: JSON Configuration**
```json
{
    "gptp": {
        "version": "802.1AS-2021",
        "domains": [
            {
                "domain_number": 0,
                "name": "default",
                "priority1": 248,
                "priority2": 248,
                "sync_interval": -3,
                "pdelay_interval": 0,
                "path_delay_mechanism": "P2P"
            },
            {
                "domain_number": 1, 
                "name": "audio",
                "priority1": 200,
                "priority2": 200,
                "sync_interval": -4,
                "pdelay_interval": -1,
                "path_delay_mechanism": "adaptive"
            }
        ],
        "hardware": {
            "vendor": "generic",
            "timestamping": "hardware",
            "fallback_to_software": true
        }
    }
}
```

#### **Management Interface: YANG Model**
```yang
// IEEE 802.1AS-2021 YANG model support
module ieee802-dot1as-gptp {
    namespace "urn:ieee:std:802.1AS:yang:ieee802-dot1as-gptp";
    prefix "gptp";
    
    container gptp-system {
        list domain {
            key "domain-number";
            leaf domain-number {
                type uint8 { range "0..127"; }
            }
            leaf sync-interval {
                type int8 { range "-7..7"; }
            }
            // ... additional configuration parameters
        }
    }
}
```

### Development and Testing Tools

#### **Static Analysis: PC-lint Plus**
```c
// Lint configuration for gPTP
//lint -strong(AXJ)      // Strong type checking
//lint -e534             // Ignoring return value
//lint +fdi              // Start deferred identification
//lint -e715             // Symbol not referenced
```

#### **Dynamic Analysis: Valgrind**
```bash
# Memory leak detection for gPTP daemon
valgrind --tool=memcheck \
         --leak-check=full \
         --track-origins=yes \
         --show-reachable=yes \
         ./gptp-daemon --config=test.json
```

#### **Performance Profiling: perf**
```bash
# Profile gPTP timing performance
perf record -g -e cycles:u,instructions:u ./gptp-daemon
perf report --sort=symbol
```

### Build System: CMake

```cmake
cmake_minimum_required(VERSION 3.16)
project(ieee802_1as_2021_gptp VERSION 1.0.0)

# Hardware abstraction build options
option(ENABLE_INTEL_TSN_HAL "Enable Intel TSN hardware support" OFF)
option(ENABLE_BROADCOM_HAL "Enable Broadcom hardware support" OFF)
option(ENABLE_MARVELL_HAL "Enable Marvell hardware support" OFF)
option(ENABLE_SOFTWARE_FALLBACK "Enable software timestamping fallback" ON)

# Real-time requirements
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE -pthread")

# Hardware abstraction library
add_library(gptp_core STATIC
    src/core/bmca.c
    src/core/multi_domain.c
    src/core/path_delay.c
    src/core/message_processing.c
    src/core/state_machines.c
)

# Platform-specific implementations
if(ENABLE_INTEL_TSN_HAL)
    add_library(gptp_intel_hal SHARED
        src/platform/intel/intel_tsn_hal.c
        src/platform/intel/intel_timing.c
    )
    target_link_libraries(gptp_core gptp_intel_hal)
endif()

# Conformance testing
add_executable(gptp_conformance_tests
    tests/conformance/ieee_802_1as_2021_tests.c
    tests/conformance/multi_domain_tests.c
    tests/conformance/path_delay_tests.c
)
```

### Deployment Architecture

#### **Container-Based Deployment (Docker)**
```dockerfile
FROM ubuntu:22.04

# Install real-time kernel and dependencies
RUN apt-get update && apt-get install -y \
    linux-image-rt-amd64 \
    ethtool \
    ptp4l \
    linuxptp

# gPTP application
COPY gptp-daemon /usr/local/bin/
COPY config/ /etc/gptp/

# Real-time configuration
RUN echo 'kernel.sched_rt_runtime_us = -1' >> /etc/sysctl.conf
RUN echo '* - rtprio 99' >> /etc/security/limits.conf

ENTRYPOINT ["/usr/local/bin/gptp-daemon"]
```

#### **Bare Metal Deployment**
```c
// Embedded systems configuration
#define GPTP_STACK_SIZE_BYTES       8192
#define GPTP_HEAP_SIZE_BYTES        65536
#define GPTP_MAX_DOMAINS            16
#define GPTP_MAX_PORTS              8

// Memory allocation for real-time operation
static uint8_t gptp_memory_pool[GPTP_HEAP_SIZE_BYTES] __attribute__((aligned(8)));
static StaticTask_t gptp_sync_task_buffer;
static StackType_t gptp_sync_stack[GPTP_STACK_SIZE_BYTES];
```

## Vendor Hardware Integration

### Intel Hardware Support
```c
// Intel i210/i225 TSN hardware integration
struct intel_tsn_hal {
    void __iomem* registers;
    uint32_t clock_frequency;
    bool supports_multi_domain;
    
    // Intel-specific operations
    int (*configure_ieee1588_clock)(uint8_t domain);
    int (*enable_hardware_timestamping)(bool enable);
    int (*adjust_system_time)(int64_t offset_ns);
};
```

### Broadcom Hardware Support  
```c
// Broadcom switch hardware integration
struct broadcom_switch_hal {
    uint32_t switch_id;
    uint8_t port_count;
    bool supports_boundary_clock;
    
    // Broadcom-specific operations  
    int (*configure_switch_timing)(const timing_config_t* config);
    int (*enable_port_timestamping)(uint8_t port, bool enable);
    int (*get_switch_time)(struct timespec* ts);
};
```

### Marvell Hardware Support
```c
// Marvell automotive TSN integration
struct marvell_automotive_hal {
    uint32_t device_id;
    automotive_profile_t profile;
    bool supports_automotive_extensions;
    
    // Marvell-specific operations
    int (*configure_automotive_profile)(automotive_profile_t profile);
    int (*enable_safety_features)(bool enable);
    int (*monitor_timing_quality)(timing_quality_t* quality);
};
```

## Performance Characteristics

### Timing Requirements
- **Synchronization Accuracy**: <500 nanoseconds
- **Convergence Time**: <2 seconds from network join
- **Message Processing Latency**: <10 microseconds
- **Path Delay Measurement**: <1 microsecond resolution
- **CPU Usage**: <5% on 1GHz ARM processor
- **Memory Footprint**: <2MB RAM, <512KB code

### Scalability Metrics  
- **Maximum Domains**: 16 simultaneous
- **Maximum Network Hops**: 20 bridge hops
- **Message Rate**: 1000+ messages/second/port
- **Network Size**: 1000+ nodes per domain
- **Failover Time**: <100 milliseconds

This technology stack provides a comprehensive foundation for implementing IEEE 802.1AS-2021 gPTP while maintaining hardware vendor independence and meeting professional media timing requirements.