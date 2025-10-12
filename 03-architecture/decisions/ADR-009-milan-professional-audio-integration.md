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
    - "REQ-F-002"
    - "REQ-F-003"
    - "REQ-NF-002"
---

# ADR-009: Milan Professional Audio Integration Pattern

## Status

Accepted

## Context

Milan is a professional audio profile built on IEEE 1722.1 AVDECC that adds specific requirements for professional audio applications. Milan compatibility is essential for professional audio equipment market acceptance while maintaining full IEEE 1722.1-2021 compliance.

### Milan Integration Requirements

- **Profile Compliance**: Full compatibility with Milan v1.2 and v2.0a specifications
- **Certification Support**: Architecture must enable AVnu Alliance Milan certification
- **Enhanced Discovery**: Milan-specific device identification and capability reporting
- **Stream Management**: Professional audio stream formats and redundancy patterns
- **Extension Framework**: Clean integration without compromising IEEE 1722.1 compliance

### Technical Challenges

- **Standards Hierarchy**: Milan extends IEEE 1722.1 but must not break base standard compliance
- **Backward Compatibility**: Milan devices must interoperate with standard AVDECC devices
- **Certification Requirements**: Specific test patterns and behaviors required for Milan certification
- **Professional Features**: Enhanced timing, redundancy, and quality requirements
- **Version Management**: Support for multiple Milan versions simultaneously

## Stakeholder Concerns

- **Audio Professionals**: Require guaranteed Milan compatibility for professional audio workflows
- **Equipment Manufacturers**: Need Milan certification capability for professional audio markets  
- **Network Integrators**: Require reliable mixed Milan/standard AVDECC network operation
- **Certification Bodies**: Need verifiable Milan compliance and test capability

## Architectural Viewpoints

- **Compliance Viewpoint**: Maintaining IEEE 1722.1-2021 compliance while adding Milan features
- **Extensibility Viewpoint**: Clean extension pattern supporting future Milan versions
- **Interoperability Viewpoint**: Milan/standard AVDECC device coexistence and interaction
- **Certification Viewpoint**: Architecture enabling automated Milan certification testing

## Decision

We will implement a **Layered Extension Pattern** with Milan as an optional professional audio layer:

### 1. Milan Extension Framework

```cpp
namespace AVnu::Milan {

// Milan version abstraction
enum class MilanVersion {
    VERSION_1_2,
    VERSION_2_0a,
    VERSION_UNKNOWN
};

// Milan capability detection and reporting
class MilanCapabilities {
public:
    struct MilanInfo {
        MilanVersion version;
        uint32_t certification_level;
        bool redundancy_support;
        uint16_t max_transit_time_ms;
        std::vector<uint32_t> supported_formats;
        bool srp_support;
        bool maap_support;
    };
    
    static bool isMilanDevice(const IEEE::_1722_1::_2021::aem::EntityDescriptor& entity);
    static MilanInfo extractMilanInfo(const IEEE::_1722_1::_2021::aem::EntityModel& model);
    static void addMilanCapabilities(IEEE::_1722_1::_2021::aem::EntityDescriptor& entity);
    
private:
    static constexpr uint64_t MILAN_VENDOR_ID = 0x001B92;  // AVnu Alliance OUI
    static constexpr uint32_t MILAN_MODEL_PREFIX = 0x80;   // Milan model ID prefix
};

} // namespace AVnu::Milan
```

### 2. Milan Command Extensions

```cpp
namespace AVnu::Milan::v1_2 {

// Milan Manufacturer Vendor Unique (MVU) commands
class MilanCommandProcessor {
public:
    MilanCommandProcessor(IEEE::_1722_1::_2021::aem::EntityModel& entity_model);
    
    // Milan MVU command handlers
    void processGetMilanInfo(const IEEE::_1722_1::_2021::aecp::AEMCommand& command,
                           IEEE::_1722_1::_2021::aecp::AEMResponse& response);
    
    void processGetSystemUniqueId(const IEEE::_1722_1::_2021::aecp::AEMCommand& command,
                                IEEE::_1722_1::_2021::aecp::AEMResponse& response);
    
    void processSetSystemUniqueId(const IEEE::_1722_1::_2021::aecp::AEMCommand& command,
                                IEEE::_1722_1::_2021::aecp::AEMResponse& response);
    
    // Milan discovery extensions  
    void enhanceEntityAdvertisement(IEEE::_1722_1::_2021::adp::EntityAvailableMessage& msg);
    bool validateMilanEntity(const IEEE::_1722_1::_2021::aem::EntityModel& model);
    
private:
    IEEE::_1722_1::_2021::aem::EntityModel& entity_model_;
    uint64_t system_unique_id_;
    MilanCapabilities::MilanInfo milan_info_;
    
    // Milan-specific validation
    bool validateStreamFormats(const std::vector<uint32_t>& formats) const;
    bool validateRedundancyConfiguration(const IEEE::_1722_1::_2021::aem::StreamDescriptor& stream) const;
    bool validateTimingRequirements() const;
};

} // namespace AVnu::Milan::v1_2
```

### 3. Professional Audio Stream Extensions

```cpp
namespace AVnu::Milan::streams {

// Milan professional audio formats
class MilanStreamFormats {
public:
    // Standard Milan audio formats (IEC 61883-6 AM824)
    static constexpr uint64_t MILAN_FORMAT_SAF_48K_16CH_24BIT = 0x00A0020840001000ULL;
    static constexpr uint64_t MILAN_FORMAT_SAF_96K_8CH_24BIT  = 0x00A0020840002000ULL;
    
    // Milan format validation
    static bool isMilanSupportedFormat(uint64_t stream_format);
    static std::vector<uint64_t> getMilanMandatoryFormats();
    static bool validateFormatParameters(uint64_t stream_format, uint16_t channels, uint32_t sample_rate);
    
    // Professional audio specific parameters
    static uint16_t getMaxChannelsForFormat(uint64_t stream_format);
    static uint32_t getMaxSampleRate(uint64_t stream_format);
    static bool requiresRedundancy(uint64_t stream_format);
};

// Milan redundancy management
class MilanRedundancyManager {
public:
    struct RedundantStreamPair {
        IEEE::_1722_1::_2021::StreamID primary_stream;
        IEEE::_1722_1::_2021::StreamID secondary_stream;
        bool seamless_switching_support;
    };
    
    void registerRedundantPair(const RedundantStreamPair& pair);
    void switchToPrimary(const IEEE::_1722_1::_2021::StreamID& stream_id);
    void switchToSecondary(const IEEE::_1722_1::_2021::StreamID& stream_id);
    
    bool isPrimaryActive(const IEEE::_1722_1::_2021::StreamID& stream_id) const;
    std::optional<RedundantStreamPair> getRedundantPair(const IEEE::_1722_1::_2021::StreamID& stream_id) const;
    
private:
    std::unordered_map<IEEE::_1722_1::_2021::StreamID, RedundantStreamPair> redundant_pairs_;
    std::unordered_map<IEEE::_1722_1::_2021::StreamID, bool> primary_active_state_;
    
    void notifyRedundancySwitch(const IEEE::_1722_1::_2021::StreamID& stream_id, bool to_primary);
};

} // namespace AVnu::Milan::streams
```

### 4. Milan Integration Manager

```cpp
class MilanIntegrationManager {
public:
    MilanIntegrationManager(IEEE::_1722_1::_2021::protocols::AVDECCProtocolCoordinator& avdecc);
    
    void initialize(AVnu::Milan::MilanVersion target_version);
    void enableMilanExtensions(bool enable);
    
    // Milan certification support
    bool runCertificationTests();
    AVnu::Milan::MilanCapabilities::MilanInfo getCertificationInfo() const;
    
    // Integration with AVDECC protocols
    void registerWithAECP(IEEE::_1722_1::_2021::aecp::AECPStateMachine& aecp);
    void registerWithADP(IEEE::_1722_1::_2021::adp::ADPStateMachine& adp);
    void registerWithACMP(IEEE::_1722_1::_2021::acmp::ACMPStateMachine& acmp);
    
private:
    IEEE::_1722_1::_2021::protocols::AVDECCProtocolCoordinator& avdecc_coordinator_;
    AVnu::Milan::MilanVersion milan_version_;
    bool milan_enabled_ = false;
    
    // Milan processors per version
    std::unique_ptr<AVnu::Milan::v1_2::MilanCommandProcessor> milan_v12_processor_;
    std::unique_ptr<AVnu::Milan::streams::MilanRedundancyManager> redundancy_manager_;
    std::unique_ptr<AVnu::Milan::streams::MilanStreamFormats> format_manager_;
    
    // Milan certification testing
    bool testDiscoveryExtensions();
    bool testStreamFormatCompliance();
    bool testRedundancyBehavior();
    bool testTimingRequirements();
};
```

### 5. Backward Compatibility Layer

```cpp
namespace AVnu::Milan::compatibility {

// Ensures Milan devices work with standard AVDECC controllers
class BackwardCompatibilityManager {
public:
    // Standard AVDECC behavior when Milan features not supported
    void handleNonMilanController(const IEEE::_1722_1::_2021::MacAddress& controller_mac);
    void degradeToStandardAVDECC(const IEEE::_1722_1::_2021::StreamID& stream_id);
    
    // Milan feature negotiation
    bool negotiateMilanFeatures(const IEEE::_1722_1::_2021::aem::EntityDescriptor& remote_entity);
    std::set<AVnu::Milan::MilanCapabilities::MilanInfo> getSupportedFeatures() const;
    
private:
    std::set<IEEE::_1722_1::_2021::MacAddress> non_milan_controllers_;
    std::unordered_map<IEEE::_1722_1::_2021::StreamID, bool> milan_degraded_streams_;
    
    bool isStandardAVDECCController(const IEEE::_1722_1::_2021::MacAddress& controller) const;
    void disableMilanFeatures(const IEEE::_1722_1::_2021::StreamID& stream_id);
};

} // namespace AVnu::Milan::compatibility
```

## Rationale

### **Layered Extension Architecture**

- **Standards Compliance**: Milan layer built on top of IEEE 1722.1-2021 without modification
- **Optional Integration**: Milan features can be disabled for standard AVDECC operation
- **Version Support**: Architecture supports multiple Milan versions simultaneously
- **Clean Separation**: Milan code isolated from core IEEE implementation

### **Professional Audio Focus**

- **Format Validation**: Milan audio formats validated against professional requirements
- **Redundancy Support**: Built-in redundancy management for professional audio reliability
- **Timing Precision**: Enhanced timing requirements for professional audio applications
- **Certification Ready**: Architecture directly supports AVnu Alliance certification testing

### **Interoperability Design**

- **Backward Compatibility**: Milan devices work with standard AVDECC controllers
- **Feature Negotiation**: Automatic detection and negotiation of Milan capabilities
- **Graceful Degradation**: Milan features disabled when not supported by remote devices
- **Standards Hierarchy**: Clear hierarchy from IEEE 1722.1 → Milan → vendor extensions

## Consequences

### **Positive Impacts**

- **Market Access**: Milan certification enables professional audio market participation
- **Professional Features**: Enhanced capabilities for professional audio applications
- **Standards Compliance**: Maintains full IEEE 1722.1-2021 compliance and interoperability
- **Future-Proof**: Architecture supports future Milan versions and enhancements
- **Certification Support**: Built-in testing framework for Milan certification

### **Negative Impacts**

- **Implementation Complexity**: Additional layer increases development and testing effort
- **Memory Overhead**: Milan extensions require additional memory for enhanced features
- **Certification Cost**: Milan certification process adds development timeline and cost
- **Version Management**: Supporting multiple Milan versions increases maintenance complexity

### **Risk Mitigation**

- **Modular Design**: Milan extensions can be disabled if certification not required
- **Standards Testing**: Comprehensive IEEE 1722.1 testing ensures base compliance maintained
- **Certification Framework**: Built-in test framework reduces certification preparation time
- **Professional Validation**: Validation with professional audio equipment manufacturers

## Implementation Requirements

- Milan extension framework supporting v1.2 and v2.0a specifications
- Professional audio stream format validation and management
- Redundancy management with seamless switching capabilities
- Backward compatibility with standard AVDECC devices and controllers
- Certification test framework for automated Milan compliance validation
- Integration with existing IEEE 1722.1 protocol state machines

## Verification Criteria

- Full Milan v1.2 and v2.0a specification compliance validation
- AVnu Alliance certification test suite passes with 100% success rate
- Backward compatibility validated with standard AVDECC controllers
- Professional audio stream formats supported with proper validation
- Redundancy switching completes within Milan timing requirements (<5ms)
- Milan features gracefully disable when interoperating with non-Milan devices

**References**: AVnu Milan Specification v1.2/v2.0a, IEEE 1722.1-2021, ADR-007 (Entity Model), ADR-008 (Protocol State Machines)