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

# ADR-007: IEEE 1722.1 AVDECC Entity Model Architecture

## Status

Accepted

## Context

IEEE 1722.1-2021 AVDECC requires a comprehensive entity model to represent device capabilities, configurations, and state. The entity model serves as the foundation for device control and must support complex professional audio devices with multiple configurations and dynamic capabilities.

### Entity Model Requirements

- **Device Representation**: Complete description of device capabilities and features
- **Configuration Management**: Support for multiple device configurations with dynamic switching  
- **Descriptor Hierarchy**: Hierarchical organization of device components and controls
- **State Synchronization**: Consistent state management across network and local operations
- **Milan Extensions**: Support for Milan professional audio profile extensions

### Technical Challenges

- **Complex Hierarchies**: Professional audio devices have complex interconnected components
- **Dynamic Configuration**: Runtime configuration changes must maintain model consistency
- **Memory Efficiency**: Large descriptor sets must be managed efficiently
- **Validation Requirements**: Entity model must be validated for IEEE 1722.1 compliance
- **Extension Support**: Architecture must accommodate Milan and vendor-specific extensions

## Stakeholder Concerns

- **Audio Engineers**: Need comprehensive device representation for complex professional audio workflows
- **Equipment Manufacturers**: Require flexible entity model supporting diverse device architectures
- **Software Developers**: Need predictable, well-structured API for device interaction
- **Network Administrators**: Require consistent device representation across multi-vendor networks

## Architectural Viewpoints

- **Data Model Viewpoint**: Hierarchical descriptor organization and relationships
- **Extensibility Viewpoint**: Support for Milan extensions and vendor-specific capabilities
- **Performance Viewpoint**: Efficient memory usage and fast descriptor access
- **Compliance Viewpoint**: IEEE 1722.1-2021 specification adherence and validation

## Decision

We will implement a **Layered Entity Model Architecture** with separation of concerns:

### 1. Entity Model Core

```cpp
namespace IEEE::_1722_1::_2021::aem {

class EntityModel {
public:
    // IEEE 1722.1-2021 mandatory descriptor access
    const EntityDescriptor& getEntityDescriptor() const;
    const ConfigurationDescriptor& getConfiguration(uint16_t config_index) const;
    const StreamDescriptor& getStreamDescriptor(uint16_t config_index, 
                                                uint16_t stream_index) const;
    
    // Dynamic configuration management
    Result setConfiguration(uint16_t config_index);
    Result validateConfiguration(uint16_t config_index) const;
    
    // Runtime state management
    void updateAvailableIndex();
    EntityState getCurrentState() const;
    
private:
    EntityDescriptor entity_descriptor_;
    std::vector<ConfigurationDescriptor> configurations_;
    uint16_t current_configuration_;
    std::atomic<uint32_t> available_index_;
    
    // Descriptor validation and consistency checking
    bool validateDescriptorHierarchy() const;
    bool validateDescriptorCounts() const;
};

} // namespace IEEE::_1722_1::_2021::aem
```

### 2. Descriptor Type System

```cpp
// Base descriptor interface following IEEE 1722.1-2021
class DescriptorBase {
public:
    virtual ~DescriptorBase() = default;
    virtual uint16_t getDescriptorType() const = 0;
    virtual uint16_t getDescriptorIndex() const = 0;
    virtual size_t serialize(uint8_t* buffer, size_t max_length) const = 0;
    virtual bool deserialize(const uint8_t* buffer, size_t length) = 0;
    virtual bool validate() const = 0;
};

// Concrete descriptor implementations
class EntityDescriptor : public DescriptorBase {
public:
    uint64_t entity_id;
    uint64_t entity_model_id;
    uint32_t entity_capabilities;
    uint16_t talker_stream_sources;
    uint16_t talker_capabilities;
    uint16_t listener_stream_sinks;
    uint16_t listener_capabilities;
    uint32_t controller_capabilities;
    uint32_t available_index;
    uint64_t association_id;
    std::string entity_name;
    uint16_t vendor_name_string;
    uint16_t model_name_string;
    std::string firmware_version;
    std::string group_name;
    std::string serial_number;
    uint16_t configurations_count;
    uint16_t current_configuration;
    
    // IEEE 1722.1-2021 compliance validation
    bool validate() const override;
};
```

### 3. Configuration Management Layer

```cpp
class ConfigurationManager {
public:
    // Configuration lifecycle management
    Result loadConfiguration(uint16_t config_index);
    Result validateConfiguration(uint16_t config_index) const;
    Result switchConfiguration(uint16_t new_config_index);
    
    // Dynamic configuration capabilities
    bool supportsConfigurationSwitch() const;
    std::vector<uint16_t> getAvailableConfigurations() const;
    
    // Stream and control management per configuration
    const std::vector<StreamDescriptor>& getInputStreams(uint16_t config_index) const;
    const std::vector<StreamDescriptor>& getOutputStreams(uint16_t config_index) const;
    const std::vector<ControlDescriptor>& getControls(uint16_t config_index) const;
    
private:
    EntityModel* entity_model_;
    std::unordered_map<uint16_t, ConfigurationState> configuration_states_;
    
    // Configuration transition validation
    bool validateConfigurationTransition(uint16_t from_config, uint16_t to_config) const;
    void notifyConfigurationChange(uint16_t old_config, uint16_t new_config);
};
```

### 4. Milan Extensions Integration

```cpp
namespace AVnu::Milan::v1_2 {

class MilanEntityModelExtensions {
public:
    // Milan-specific entity information
    struct MilanInfo {
        uint32_t milan_version;
        uint32_t certification_level;
        std::vector<uint32_t> supported_formats;
        bool redundancy_support;
        uint16_t max_transit_time_ms;
    };
    
    // Milan MVU command support
    MilanInfo getMilanInfo() const;
    uint64_t getSystemUniqueId() const;
    bool isMilanCompliantDevice() const;
    
    // Milan-specific descriptor extensions
    void addMilanCapabilities(EntityDescriptor& entity_desc) const;
    void addRedundancyInformation(StreamDescriptor& stream_desc) const;
    
private:
    MilanInfo milan_info_;
    uint64_t system_unique_id_;
};

} // namespace AVnu::Milan::v1_2
```

## Rationale

### **Separation of Concerns**

- **Entity Model Core**: Focuses on IEEE 1722.1-2021 compliance and basic functionality
- **Configuration Manager**: Handles complex configuration switching and validation
- **Milan Extensions**: Isolated Milan-specific functionality without polluting core model
- **Descriptor System**: Type-safe descriptor handling with validation

### **Performance Benefits**

- **Efficient Memory Layout**: Descriptors stored contiguously for cache efficiency
- **Lazy Loading**: Large descriptor sets loaded only when accessed
- **Reference Semantics**: Descriptors returned by const reference to avoid copying
- **Atomic Operations**: Available index updates use atomic operations for thread safety

### **Extensibility Design**

- **Plugin Architecture**: Milan and vendor extensions can be added without core changes
- **Validation Framework**: Pluggable validation for different compliance requirements
- **Descriptor Factory**: New descriptor types can be added through factory pattern
- **Event System**: Configuration changes trigger events for dependent systems

## Consequences

### **Positive Impacts**

- **IEEE 1722.1 Compliance**: Architecture directly supports all IEEE specification requirements
- **Milan Compatibility**: Clean integration with Milan professional audio profiles  
- **Memory Efficiency**: Optimized memory layout for large professional audio devices
- **Type Safety**: Strong typing prevents descriptor misuse and improves reliability
- **Testability**: Isolated components enable comprehensive unit testing

### **Negative Impacts**

- **Implementation Complexity**: Layered architecture requires more initial development effort
- **Memory Overhead**: Object-oriented design has higher memory overhead than C structs
- **Configuration Latency**: Complex validation may increase configuration switch time
- **Learning Curve**: Developers need to understand multiple architectural layers

### **Risk Mitigation**

- **Standards Compliance**: Comprehensive IEEE 1722.1-2021 compliance testing validates architecture
- **Performance Testing**: Benchmarking ensures configuration operations meet real-time requirements
- **Documentation**: Detailed API documentation and examples reduce learning curve
- **Migration Path**: Gradual migration from existing implementations through adapter patterns

## Implementation Requirements

- Entity model implementation following IEEE 1722.1-2021 descriptor specifications
- Configuration management with atomic switching and rollback capabilities  
- Milan extension framework with certification level validation
- Comprehensive descriptor validation with IEEE compliance checking
- Memory-efficient descriptor storage with optimized access patterns
- Thread-safe state management for concurrent access scenarios

## Verification Criteria

- All IEEE 1722.1-2021 descriptor types implemented and validated
- Configuration switching completes within 100ms for typical professional audio devices
- Milan extension integration passes AVnu Alliance certification tests  
- Memory usage scales linearly with descriptor count (O(n) complexity)
- Concurrent access from multiple threads maintains data consistency
- Descriptor validation catches all IEEE specification violations

**References**: IEEE 1722.1-2021, AVnu Milan Specification v1.2, ADR-001 (Hardware Abstraction), ADR-002 (Standards Layering)