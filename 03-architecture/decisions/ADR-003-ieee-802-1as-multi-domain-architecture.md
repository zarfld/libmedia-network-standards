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
    - "REQ-F-001"
    - "REQ-NF-001"
---

# ADR-003: IEEE 802.1AS-2021 Multi-Domain Architecture

## Status
Accepted

## Context
IEEE 802.1AS-2021 introduces enhanced multi-domain support for complex network topologies. Professional media installations require multiple timing domains for different applications (audio, video, control) while maintaining precise synchronization relationships between domains.

### Requirements Driving This Decision
- **REQ-SYS-IEEE8021AS-001**: Enhanced Multi-Domain Support
- **REQ-FUN-IEEE8021AS-001**: Domain Configuration  
- **REQ-FUN-IEEE8021AS-002**: Cross-Domain Synchronization
- **REQ-NFR-IEEE8021AS-002**: Scalability Requirements (minimum 16 domains)

### Technical Constraints
- Domain numbers limited to 0-127 per IEEE specification
- Each domain requires independent BMCA operation
- Cross-domain synchronization introduces timing complexity
- Hardware must support per-domain timestamp capabilities

## Decision
We will implement a **Domain-Centric Architecture** with the following components:

### 1. Domain Manager
```cpp
class DomainManager {
public:
    // Domain lifecycle management
    int create_domain(uint8_t domain_number, const domain_config_t* config);
    int destroy_domain(uint8_t domain_number);
    int configure_domain(uint8_t domain_number, const domain_config_t* config);
    
    // Cross-domain relationships
    int establish_domain_relationship(uint8_t source_domain, uint8_t target_domain);
    int configure_domain_priority(uint8_t domain_number, uint8_t priority);
    
    // Hardware abstraction for multi-domain
    int register_domain_hardware(uint8_t domain_number, 
                                const domain_hardware_interface_t* hw_if);
};
```

### 2. Per-Domain State Isolation
- **Independent BMCA**: Each domain runs separate Best Master Clock Algorithm
- **Isolated Message Processing**: Domain-specific message queues and processing
- **Separate Timing Loops**: Independent synchronization control for each domain
- **Domain-Specific Configuration**: Per-domain timing parameters and policies

### 3. Cross-Domain Synchronization Framework
```cpp
typedef struct {
    uint8_t source_domain;
    uint8_t target_domain;
    sync_relationship_type_t relationship_type;
    int64_t offset_correction_ns;
    double rate_correction_ppb;
} cross_domain_sync_t;

class CrossDomainSynchronizer {
public:
    int configure_sync_relationship(const cross_domain_sync_t* sync_config);
    int monitor_sync_quality(uint8_t source_domain, uint8_t target_domain);
    int handle_domain_failover(uint8_t failed_domain, uint8_t backup_domain);
};
```

### 4. Hardware Abstraction for Multi-Domain
```cpp
// Generic interface supporting multiple timing domains
typedef struct {
    // Per-domain timing operations
    int (*set_domain_time)(uint8_t domain, const timespec_t* time);
    int (*get_domain_time)(uint8_t domain, timespec_t* time);
    int (*adjust_domain_frequency)(uint8_t domain, double ppb_adjustment);
    
    // Domain-specific timestamping
    int (*timestamp_domain_packet)(uint8_t domain, const packet_t* packet, 
                                   timestamp_t* tx_timestamp);
    int (*extract_domain_timestamp)(uint8_t domain, const packet_t* packet,
                                    timestamp_t* rx_timestamp);
    
    // Cross-domain capabilities
    bool (*supports_cross_domain_sync)(void);
    int (*configure_domain_relationship)(uint8_t source, uint8_t target);
} multi_domain_hardware_interface_t;
```

## Rationale

### **Advantages**
1. **Scalability**: Supports complex installations with multiple timing requirements
2. **Isolation**: Domain failures don't affect other domains
3. **Flexibility**: Different timing parameters and priorities per domain
4. **Professional Requirements**: Meets broadcast and production facility needs
5. **Hardware Independence**: Generic interface works across vendor hardware

### **Implementation Benefits**
- **Audio Domain**: Optimized for sub-microsecond audio production timing
- **Video Domain**: Frame-accurate video production synchronization  
- **Control Domain**: Network management and control plane timing
- **Backup Domains**: Redundant timing sources for reliability

### **Challenges Addressed**
- **Complex Topologies**: Multi-domain support handles complex network designs
- **Application Isolation**: Different applications don't interfere with each other
- **Vendor Interoperability**: Hardware abstraction enables multi-vendor domains

## Consequences

### **Positive Impacts**
✅ **Scalability**: Enables complex professional media installations with 16+ domains  
✅ **Reliability**: Domain isolation prevents cascading failures  
✅ **Performance**: Per-domain optimization for specific application requirements  
✅ **Standards Compliance**: Full IEEE 802.1AS-2021 multi-domain support  
✅ **Vendor Independence**: Generic hardware interface across all timing vendors  

### **Negative Impacts**  
❌ **Complexity**: Significantly more complex than single-domain implementation  
❌ **Resource Usage**: Higher CPU and memory usage for multiple domain state machines  
❌ **Testing Complexity**: Cross-domain interactions require extensive validation  
❌ **Hardware Requirements**: Hardware must support multi-domain timestamping  

### **Risk Mitigation**
- **Performance Overhead**: Limit to essential domains, optimize state machine efficiency
- **Hardware Compatibility**: Provide fallback to single-domain for limited hardware
- **Configuration Complexity**: Provide domain templates for common use cases
- **Cross-Domain Sync Quality**: Implement monitoring and alerting for sync degradation

### **Implementation Requirements**
- Domain Manager with lifecycle management and cross-domain coordination
- Per-domain BMCA engines with independent operation
- Hardware abstraction layer supporting multi-domain timestamping
- Configuration management for domain relationships and priorities
- Monitoring and diagnostics for multi-domain timing quality

### **Testing Strategy**
- Unit tests for domain isolation and independence
- Integration tests for cross-domain synchronization accuracy
- Performance tests for scalability with 16+ active domains
- Conformance tests against IEEE 802.1AS-2021 multi-domain requirements
- Interoperability tests with major TSN vendor hardware

This architecture provides the foundation for complex professional media installations while maintaining the hardware abstraction principles essential for vendor independence and deployment flexibility.