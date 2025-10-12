---
specType: architecture
standard: "ISO/IEC/IEEE 42010:2011"
phase: "03-architecture"
version: "1.0.0"
author: "Architecture Team"
date: "2025-10-12"
status: "draft"
traceability:
  requirements:
    - "REQ-FUN-IEEE8021AS-003"
    - "REQ-FUN-IEEE8021AS-004"
    - "REQ-NFR-IEEE8021AS-001"
---

# ADR-004: IEEE 802.1AS-2021 Enhanced Path Delay Architecture

## Status
Accepted

## Context
IEEE 802.1AS-2021 introduces enhanced path delay mechanisms including both peer-to-peer and end-to-end path delay measurement. Professional media applications require sub-microsecond timing accuracy, which depends on precise path delay measurement and compensation.

### Requirements Driving This Decision
- **REQ-FUN-IEEE8021AS-003**: Peer-to-Peer Path Delay support
- **REQ-FUN-IEEE8021AS-004**: End-to-End Path Delay support (new in 2021)
- **REQ-NFR-IEEE8021AS-001**: Timing accuracy <500 nanoseconds
- **REQ-SYS-IEEE8021AS-002**: Precision timing accuracy requirements

### Technical Constraints
- Hardware timestamping accuracy varies by vendor
- Network asymmetry affects path delay accuracy
- Path delay mechanism selection impacts network performance
- Legacy equipment may only support peer-to-peer mechanism

## Decision
We will implement a **Dual Path Delay Architecture** supporting both mechanisms with automatic selection:

### 1. Path Delay Manager
```cpp
typedef enum {
    PATH_DELAY_P2P,          // Peer-to-peer (traditional)
    PATH_DELAY_E2E,          // End-to-end (IEEE 802.1AS-2021)
    PATH_DELAY_HYBRID        // Adaptive selection
} path_delay_mechanism_t;

class PathDelayManager {
public:
    // Mechanism selection and configuration
    int configure_path_delay_mechanism(path_delay_mechanism_t mechanism);
    int enable_adaptive_mechanism_selection(bool enable);
    
    // Path delay measurement operations
    int initiate_p2p_measurement(port_id_t port);
    int initiate_e2e_measurement(port_id_t port);
    int process_pdelay_message(const pdelay_message_t* message);
    
    // Results and statistics
    int get_path_delay(port_id_t port, int64_t* delay_ns);
    int get_path_delay_statistics(port_id_t port, path_delay_stats_t* stats);
};
```

### 2. Hardware-Agnostic Timestamping Interface
```cpp
typedef struct {
    // Hardware timestamp capabilities
    bool supports_hardware_timestamping;
    uint32_t timestamp_accuracy_ns;
    bool supports_p2p_timestamping;
    bool supports_e2e_timestamping;
    
    // Timestamp operations
    int (*get_tx_timestamp)(const packet_t* packet, timestamp_t* tx_time);
    int (*get_rx_timestamp)(const packet_t* packet, timestamp_t* rx_time);
    int (*measure_hardware_latency)(port_id_t port, uint64_t* latency_ns);
    
    // Path delay specific operations
    int (*timestamp_pdelay_req)(port_id_t port, timestamp_t* tx_time);
    int (*timestamp_pdelay_resp)(port_id_t port, timestamp_t* rx_time, 
                                 timestamp_t* tx_time);
    int (*compensate_asymmetry)(port_id_t port, int64_t* correction_ns);
} path_delay_hardware_interface_t;
```

### 3. Adaptive Mechanism Selection
```cpp
class AdaptiveMechanismSelector {
public:
    struct selection_criteria_t {
        uint32_t network_topology_score;    // P2P better for point-to-point
        uint32_t accuracy_requirements;     // E2E may be more accurate
        uint32_t hardware_capabilities;     // Hardware support availability
        uint32_t legacy_compatibility;      // Backward compatibility needs
    };
    
    path_delay_mechanism_t select_optimal_mechanism(
        const selection_criteria_t* criteria);
    
    int monitor_mechanism_performance(path_delay_mechanism_t mechanism,
                                      performance_metrics_t* metrics);
    
    int trigger_mechanism_switch(path_delay_mechanism_t new_mechanism);
};
```

### 4. Path Delay Processing Engine
```cpp
class PathDelayProcessor {
public:
    // Peer-to-peer path delay processing
    int process_pdelay_req(const pdelay_req_message_t* req_msg,
                          pdelay_resp_message_t* resp_msg);
    
    int process_pdelay_resp(const pdelay_resp_message_t* resp_msg,
                           int64_t* path_delay_ns);
    
    // End-to-end path delay processing  
    int process_delay_req(const delay_req_message_t* req_msg);
    
    int process_delay_resp(const delay_resp_message_t* resp_msg,
                          int64_t* path_delay_ns);
    
    // Path delay filtering and smoothing
    int apply_path_delay_filter(int64_t raw_delay, int64_t* filtered_delay);
    int detect_path_delay_outliers(int64_t delay, bool* is_outlier);
};
```

## Rationale

### **Technical Advantages**

**Peer-to-Peer Mechanism**:
- Lower latency (no master clock interaction required)
- Better for point-to-point links
- Supported by all IEEE 802.1AS implementations
- More frequent measurements possible

**End-to-End Mechanism** (IEEE 802.1AS-2021):
- May provide better accuracy in some network topologies
- Centralized control through master clock
- Better suited for complex network paths
- Enhanced measurement capabilities

**Adaptive Selection**:
- Optimal mechanism based on network conditions
- Automatic fallback for hardware limitations
- Performance monitoring and optimization
- Future-proof for evolving requirements

### **Hardware Abstraction Benefits**
- **Vendor Independence**: Same path delay API across Intel, Broadcom, Marvell hardware
- **Performance Optimization**: Hardware-specific optimizations in implementation layer
- **Capability Detection**: Runtime discovery of hardware timestamping capabilities
- **Graceful Degradation**: Software fallback when hardware timestamping unavailable

## Consequences

### **Positive Impacts**
✅ **Timing Accuracy**: Enhanced path delay mechanisms improve synchronization precision  
✅ **Flexibility**: Support for both P2P and E2E mechanisms based on requirements  
✅ **Hardware Independence**: Generic interface works across all timing hardware vendors  
✅ **Standards Compliance**: Full IEEE 802.1AS-2021 path delay mechanism support  
✅ **Performance Optimization**: Adaptive selection optimizes for specific network conditions  
✅ **Backward Compatibility**: P2P support ensures compatibility with existing equipment  

### **Negative Impacts**
❌ **Implementation Complexity**: Dual mechanism support increases code complexity  
❌ **Hardware Requirements**: E2E mechanism may require enhanced timestamping hardware  
❌ **Configuration Complexity**: Network operators must understand mechanism selection  
❌ **Testing Overhead**: Both mechanisms require extensive validation and testing  

### **Risk Mitigation**
- **Hardware Compatibility**: Provide software timestamping fallback for limited hardware
- **Configuration Simplicity**: Provide automatic mechanism selection with manual override
- **Performance Impact**: Optimize path delay processing for real-time performance requirements
- **Interoperability**: Extensive testing with major TSN vendor equipment

### **Implementation Requirements**
- Path delay manager with support for both P2P and E2E mechanisms
- Hardware abstraction layer for vendor-independent timestamping operations
- Adaptive mechanism selection based on network topology and requirements
- Path delay filtering and outlier detection for robust operation
- Configuration interface for mechanism selection and tuning parameters

### **Testing Strategy**
- Unit tests for path delay calculation algorithms and filtering
- Hardware-in-the-loop tests with actual TSN hardware from multiple vendors
- Network topology tests validating mechanism selection logic
- Precision timing tests verifying sub-microsecond accuracy requirements
- Interoperability tests with existing IEEE 802.1AS-2020/2011 implementations
- Performance benchmarks for path delay measurement overhead and accuracy

This architecture ensures optimal path delay measurement accuracy while maintaining hardware vendor independence and providing flexibility for various network topologies and requirements.