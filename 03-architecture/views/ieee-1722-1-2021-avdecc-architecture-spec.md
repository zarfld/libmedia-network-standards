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
    - "REQ-F-002"
    - "REQ-F-003"
    - "REQ-NF-001"
    - "REQ-NF-002"
    - "REQ-NF-003"
---

# IEEE 1722.1-2021 AVDECC Architecture Specification

**Standards Compliance**: ISO/IEC/IEEE 42010:2011 Architecture Description  
**Standard Reference**: IEEE Std 1722.1-2021 "IEEE Standard for Device Discovery, Connection Management, and Control Protocol for IEEE 802 Based Networks"  
**Document Version**: 1.0  
**Date**: October 12, 2025  
**Prepared by**: Standards-Compliant Architecture Team

## Architectural Decisions

This architecture implements:
- **ADR-001**: Hardware Abstraction Interfaces - Generic AVDECC hardware APIs
- **ADR-002**: IEEE Standards Layering - AVDECC as device control layer above AVTP
- **ADR-007**: IEEE 1722.1 AVDECC Entity Model Architecture - Comprehensive entity representation
- **ADR-008**: IEEE 1722.1 Protocol State Machine Architecture - ADP/AECP/ACMP implementation
- **ADR-009**: Milan Professional Audio Integration - Milan extension support pattern

## Stakeholder Concerns

- **Audio Engineers**: Require zero-configuration device discovery and reliable control for professional audio systems
- **Equipment Manufacturers**: Need full IEEE 1722.1-2021 compliance with Milan compatibility for market acceptance
- **Network Administrators**: Require secure, manageable, and monitorable device control infrastructure  
- **Software Developers**: Need complete, well-documented API for building professional audio control applications
- **Compliance Bodies**: Require verifiable standards compliance and certification capability

## Architectural Viewpoints

- **Device Control Viewpoint**: AVDECC entity model and control protocol behavior
- **Network Integration Viewpoint**: Integration with AVTP transport and gPTP timing layers
- **State Management Viewpoint**: Protocol state machines and entity lifecycle management
- **Interoperability Viewpoint**: IEEE 1722.1-2021 and Milan professional audio compatibility
- **Security Viewpoint**: Entity authentication, authorization, and secure control mechanisms

## Architecture Quality Scenarios

### QS-AVDECC-001: Device Discovery Performance
- **Source**: Audio engineer connecting new device to network
- **Stimulus**: New AVDECC entity joins network
- **Environment**: Professional audio network with 50 existing devices
- **Response**: Device discovered and available in control software
- **Measure**: Discovery completed within 2 seconds of network connection

### QS-AVDECC-002: Command Response Reliability  
- **Source**: Control application sending AEM command to entity
- **Stimulus**: READ_DESCRIPTOR command sent to remote entity
- **Environment**: Network with 10ms latency and 0.1% packet loss
- **Response**: Command processed and response delivered
- **Measure**: 99.9% command success rate, <100ms response time

### QS-AVDECC-003: Stream Connection Establishment
- **Source**: User initiating audio stream connection
- **Stimulus**: CONNECT_TX_COMMAND for professional audio stream
- **Environment**: Milan-compliant network with QoS enabled
- **Response**: Stream connection established with proper resource allocation
- **Measure**: Connection established within 500ms with guaranteed bandwidth

## C4 Architecture Model

### Level 1: System Context Diagram

The AVDECC system enables device discovery, control, and connection management in professional audio networks:

```mermaid
graph TB
    AudioEng[Audio Engineer<br/>Configures and monitors<br/>professional audio devices]
    NetAdmin[Network Administrator<br/>Manages network infrastructure<br/>and device security]
    
    AVDECC[IEEE 1722.1 AVDECC<br/>Device Discovery, Control<br/>and Connection Management]
    
    AudioDevice[Professional Audio Device<br/>Milan-compliant streaming<br/>audio equipment]
    NetworkInfra[TSN Network Infrastructure<br/>IEEE 802.1Q bridges with<br/>QoS and timing sync]
    gPTP[IEEE 802.1AS gPTP<br/>Precision timing<br/>synchronization service]
    AVTP[IEEE 1722 AVTP<br/>Audio/video stream<br/>transport protocol]
    
    AudioEng --> AVDECC
    NetAdmin --> AVDECC
    AVDECC --> AudioDevice
    AVDECC --> NetworkInfra
    AVDECC --> gPTP
    AVDECC --> AVTP
    
    classDef person fill:#08427b,stroke:#052e56,stroke-width:2px,color:#fff
    classDef system fill:#1168bd,stroke:#0b4884,stroke-width:2px,color:#fff
    classDef external fill:#999999,stroke:#6b6b6b,stroke-width:2px,color:#fff
    
    class AudioEng,NetAdmin person
    class AVDECC system
    class AudioDevice,NetworkInfra,gPTP,AVTP external
```

### Level 2: Container Diagram

The AVDECC system consists of three main protocol containers:

```mermaid
graph TB
    subgraph "IEEE 1722.1 AVDECC System"
        ADP[ADP Container<br/>AVDECC Discovery Protocol<br/>Entity advertisement and discovery]
        AECP[AECP Container<br/>Enumeration and Control Protocol<br/>Device control and configuration]
        ACMP[ACMP Container<br/>Connection Management Protocol<br/>Stream connection establishment]
        
        EntityDB[(Entity Database<br/>PostgreSQL/SQLite<br/>Device and capability storage)]
        
        MilanExt[Milan Extensions<br/>C++ Library<br/>Professional audio profiles]
        NetInterface[Network Interface Abstraction<br/>C++ Hardware Abstraction<br/>Multi-platform networking]
    end
    
    AudioApp[Audio Control Application] --> AECP
    AudioApp --> ACMP
    
    ADP --> EntityDB
    AECP --> EntityDB
    ACMP --> EntityDB
    
    AECP --> MilanExt
    
    ADP --> NetInterface
    AECP --> NetInterface
    ACMP --> NetInterface
    
    NetInterface --> AVTPTransport[IEEE 1722 AVTP Transport]
    NetInterface --> gPTPService[IEEE 802.1AS gPTP Service]
    
    classDef container fill:#438dd5,stroke:#2e6da4,stroke-width:2px,color:#fff
    classDef database fill:#8fbc8b,stroke:#5d8b5d,stroke-width:2px,color:#fff
    classDef external fill:#999999,stroke:#6b6b6b,stroke-width:2px,color:#fff
    
    class ADP,AECP,ACMP,MilanExt,NetInterface container
    class EntityDB database
    class AudioApp,AVTPTransport,gPTPService external
```

### Level 3: Component Diagram - AECP Container

The AECP container handles entity enumeration and control:

```mermaid
graph TB
    subgraph "AECP Container"
        AEMProcessor[AEM Command Processor<br/>Processes AEM commands<br/>and generates responses]
        
        EntityModel[Entity Model Manager<br/>Manages entity descriptors<br/>and configurations]
        
        CommandDispatcher[Command Dispatcher<br/>Routes commands to<br/>appropriate handlers]
        
        ResponseManager[Response Manager<br/>Manages command timeouts<br/>and retransmissions]
        
        MilanHandler[Milan Command Handler<br/>Processes Milan-specific<br/>MVU commands]
    end
    
    subgraph "External Dependencies"
        EntityDB[(Entity Database)]
        MilanLib[Milan Extensions Library]
        NetworkLayer[Network Interface Abstraction]
    end
    
    AudioApp[Audio Control Application] --> CommandDispatcher
    
    CommandDispatcher --> AEMProcessor
    CommandDispatcher --> MilanHandler
    
    AEMProcessor --> EntityModel
    AEMProcessor --> ResponseManager
    
    EntityModel --> EntityDB
    MilanHandler --> MilanLib
    
    ResponseManager --> NetworkLayer
    
    classDef component fill:#85bbf0,stroke:#5d9cdb,stroke-width:2px,color:#fff
    classDef external fill:#999999,stroke:#6b6b6b,stroke-width:2px,color:#fff
    
    class AEMProcessor,EntityModel,CommandDispatcher,ResponseManager,MilanHandler component
    class EntityDB,MilanLib,NetworkLayer,AudioApp external
```

### Level 4: Code Diagram - Entity Model Manager

The Entity Model Manager implements IEEE 1722.1-2021 entity representation:

```mermaid
classDiagram
    class EntityModelManager {
        +getEntityDescriptor() EntityDescriptor
        +getConfigurationDescriptor(uint16) ConfigurationDescriptor  
        +getStreamDescriptor(uint16, uint16) StreamDescriptor
        +validateEntityModel() bool
        +updateConfiguration(uint16) Result
        -entityDescriptor: EntityDescriptor
        -configurations: vector~ConfigurationDescriptor~
        -validateDescriptorReferences() bool
    }
    
    class EntityDescriptor {
        +entity_id: uint64
        +entity_model_id: uint64
        +entity_capabilities: uint32
        +talker_stream_sources: uint16
        +talker_capabilities: uint16
        +listener_stream_sinks: uint16
        +listener_capabilities: uint16
        +controller_capabilities: uint32
        +available_index: uint32
        +association_id: uint64
        +entity_name: string
    }
    
    class ConfigurationDescriptor {
        +descriptor_type: uint16
        +descriptor_index: uint16
        +object_name: string
        +localized_description: uint16
        +descriptor_counts_count: uint16
        +descriptor_counts: vector~DescriptorCount~
        +validateConfiguration() bool
    }
    
    class StreamDescriptor {
        +descriptor_type: uint16
        +descriptor_index: uint16
        +object_name: string
        +clock_domain_index: uint16
        +stream_flags: uint16
        +current_format: StreamFormat
        +formats_count: uint16
        +formats: vector~StreamFormat~
    }
    
    class MilanExtensions {
        +getMilanInfo() MilanInfo
        +getSystemUniqueId() uint64
        +isMilanDevice() bool
        +getCertificationLevel() MilanCertLevel
    }
    
    EntityModelManager --> EntityDescriptor
    EntityModelManager --> ConfigurationDescriptor
    EntityModelManager --> StreamDescriptor
    EntityModelManager --> MilanExtensions
    
    ConfigurationDescriptor --> StreamDescriptor : contains
```

## Architecture Views (4+1 Model)

### Logical View: AVDECC Protocol Layers

```mermaid
graph TB
    subgraph "Application Layer"
        AudioApp[Audio Control Applications]
        NetworkMgmt[Network Management Systems]
    end
    
    subgraph "AVDECC Protocol Layer"
        ADP[ADP - Discovery Protocol]
        AECP[AECP - Enumeration & Control]
        ACMP[ACMP - Connection Management]
    end
    
    subgraph "Entity Model Layer"
        EntityModel[AEM - Entity Model]
        Descriptors[Descriptor Management]
        StateMgmt[State Management]
    end
    
    subgraph "Transport Layer"
        AVTP[IEEE 1722 AVTP Transport]
        Ethernet[Raw Ethernet Frames]
    end
    
    subgraph "Timing Layer"
        gPTP[IEEE 802.1AS gPTP]
        Clock[Hardware Timestamping]
    end
    
    AudioApp --> AECP
    AudioApp --> ACMP
    NetworkMgmt --> ADP
    
    ADP --> EntityModel
    AECP --> EntityModel
    ACMP --> EntityModel
    
    EntityModel --> Descriptors
    EntityModel --> StateMgmt
    
    AECP --> AVTP
    ACMP --> AVTP
    ADP --> Ethernet
    
    AVTP --> gPTP
    Ethernet --> Clock
```

### Process View: AVDECC Entity Discovery Sequence

```mermaid
sequenceDiagram
    participant Controller as AVDECC Controller
    participant Network as Network
    participant Entity as AVDECC Entity
    participant DB as Entity Database
    
    Note over Controller,Entity: Entity Discovery Process
    
    Controller->>Network: ENTITY_DISCOVER (Multicast)
    Network->>Entity: Forward ENTITY_DISCOVER
    
    Entity->>Entity: Process ADP Discovery Request
    Entity->>Network: ENTITY_AVAILABLE Response
    Network->>Controller: Forward ENTITY_AVAILABLE
    
    Controller->>DB: Store Entity Information
    DB-->>Controller: Entity Stored
    
    Note over Controller,Entity: Entity Enumeration Process
    
    Controller->>Entity: AECP READ_DESCRIPTOR (Entity)
    Entity->>Entity: Get Entity Descriptor
    Entity-->>Controller: AECP Response (Entity Descriptor)
    
    Controller->>Entity: AECP READ_DESCRIPTOR (Configuration)
    Entity->>Entity: Get Configuration Descriptor
    Entity-->>Controller: AECP Response (Configuration)
    
    Controller->>DB: Update Entity Model
    DB-->>Controller: Model Updated
    
    Note over Controller,Entity: Periodic Advertisement
    
    loop Every 2-62 seconds
        Entity->>Network: ENTITY_AVAILABLE (Advertisement)
        Network->>Controller: Forward Advertisement
        Controller->>DB: Update Entity Timestamp
    end
```

### Development View: AVDECC Module Organization

```
IEEE/1722.1/2021/
├── adp/                          # AVDECC Discovery Protocol
│   ├── include/
│   │   ├── adp_state_machine.hpp
│   │   ├── entity_advertisement.hpp
│   │   └── discovery_database.hpp
│   └── src/
│       ├── adp_state_machine.cpp
│       ├── entity_advertisement.cpp
│       └── discovery_database.cpp
├── aecp/                         # Enumeration and Control Protocol
│   ├── include/
│   │   ├── aem_command_processor.hpp
│   │   ├── command_dispatcher.hpp
│   │   └── response_manager.hpp
│   └── src/
│       ├── aem_command_processor.cpp
│       ├── command_dispatcher.cpp
│       └── response_manager.cpp
├── acmp/                         # Connection Management Protocol
│   ├── include/
│   │   ├── acmp_state_machine.hpp
│   │   ├── stream_connection.hpp
│   │   └── connection_database.hpp
│   └── src/
│       ├── acmp_state_machine.cpp
│       ├── stream_connection.cpp
│       └── connection_database.cpp
├── aem/                          # AVDECC Entity Model
│   ├── include/
│   │   ├── entity_model.hpp
│   │   ├── descriptors.hpp
│   │   └── entity_state.hpp
│   └── src/
│       ├── entity_model.cpp
│       ├── descriptors.cpp
│       └── entity_state.cpp
├── milan/                        # Milan Professional Audio Extensions
│   ├── include/
│   │   ├── milan_discovery.hpp
│   │   ├── milan_commands.hpp
│   │   └── milan_certification.hpp
│   └── src/
│       ├── milan_discovery.cpp
│       ├── milan_commands.cpp
│       └── milan_certification.cpp
└── conformity/                   # IEEE 1722.1 Conformance Testing
    ├── test_adp_compliance.cpp
    ├── test_aecp_compliance.cpp
    ├── test_acmp_compliance.cpp
    └── test_milan_interop.cpp
```

### Physical View: Professional Audio Network Deployment

```mermaid
graph TB
    subgraph "Audio Production Network"
        subgraph "Control Room"
            Controller[Audio Control Station<br/>AVDECC Controller<br/>Dell Precision 5570]
        end
        
        subgraph "Core Network"
            Switch1[Core Switch<br/>AVnu-certified TSN Switch<br/>Marvell 88E6393X]
            Switch2[Distribution Switch<br/>Milan-certified Switch<br/>Broadcom BCM56870]
        end
        
        subgraph "Device Rack 1"
            AudioIF1[Audio Interface<br/>Milan Entity/Talker<br/>Intel I210 Ethernet]
            AudioIF2[Audio Interface<br/>Milan Entity/Listener<br/>Broadcom BCM5720]
        end
        
        subgraph "Device Rack 2"
            Mixer[Digital Mixer<br/>Milan Controller/Entity<br/>Marvell 88Q5072]
            Amplifier[Power Amplifier<br/>Milan Listener<br/>Intel I225-V]
        end
    end
    
    Controller --> Switch1
    Switch1 --> Switch2
    Switch1 --> AudioIF1
    Switch2 --> AudioIF2
    Switch2 --> Mixer
    Switch2 --> Amplifier
    
    AudioIF1 -.-> AudioIF2 : AVTP Audio Stream
    Mixer -.-> Amplifier : AVTP Audio Stream
    Controller -.-> AudioIF1 : AVDECC Control
    Controller -.-> Mixer : AVDECC Control
```

## Architecture Constraints

### Technical Constraints

- **IEEE 1722.1-2021 Compliance**: Full specification compliance required for certification
- **Milan Compatibility**: Support for Milan v1.2 and v2.0a professional audio profiles
- **Real-time Performance**: Sub-100ms command response, sub-500ms connection establishment
- **Hardware Abstraction**: Support for Intel, Broadcom, Marvell network controllers
- **Multi-platform Support**: Windows and Linux operating systems with consistent behavior

### Business Constraints

- **Certification Requirements**: AVnu Alliance Milan certification for professional audio markets
- **Development Timeline**: Implementation must align with product release schedules
- **Resource Constraints**: Development team expertise in IEEE standards and real-time networking
- **Maintenance Overhead**: Long-term support for multiple IEEE 1722.1 versions and Milan profiles

### Regulatory Constraints

- **Standards Compliance**: Mandatory IEEE 1722.1-2021 compliance for professional audio equipment
- **Interoperability Requirements**: Multi-vendor network compatibility in professional environments
- **Security Standards**: Network security compliance for broadcast and enterprise installations
- **Export Control**: Compliance with international regulations for professional audio equipment

## Implementation Approach

### Phase 1: Core Protocol Implementation (Months 1-3)
- ADP entity discovery and advertisement state machines
- Basic AECP command processing (READ_DESCRIPTOR, ACQUIRE_ENTITY)
- Entity model framework with mandatory descriptors
- Hardware abstraction interface definition

### Phase 2: Advanced Control Features (Months 4-6)
- Complete AEM command set implementation
- ACMP stream connection management
- Milan MVU command support
- Configuration and state persistence

### Phase 3: Professional Features (Months 7-9)
- Milan professional audio profile compliance
- Advanced entity model validation
- Security and authentication mechanisms
- Performance optimization and testing

### Phase 4: Certification and Validation (Months 10-12)
- IEEE 1722.1-2021 compliance testing
- Milan certification test suite
- Multi-vendor interoperability validation
- Performance benchmarking and optimization

## Success Criteria

### Functional Success Criteria
- **Complete IEEE 1722.1-2021 Implementation**: All mandatory protocol features implemented and tested
- **Milan Compliance**: Full Milan v1.2/v2.0a compatibility with certification readiness
- **Multi-vendor Interoperability**: Validated operation with Intel, Broadcom, Marvell hardware
- **Professional Audio Integration**: Seamless integration with existing professional audio workflows

### Performance Success Criteria  
- **Discovery Performance**: Device discovery within 2 seconds of network connection
- **Command Response**: 99.9% command success rate with <100ms average response time
- **Connection Establishment**: Stream connections established within 500ms
- **Network Scalability**: Support for 200+ entities in network discovery database

### Quality Success Criteria
- **Standards Compliance**: 100% pass rate on IEEE 1722.1-2021 compliance tests
- **Code Quality**: 90%+ test coverage, zero critical security vulnerabilities
- **Documentation**: Complete API documentation with usage examples
- **Certification**: AVnu Alliance Milan certification achieved

---

**Document History**
- v1.0.0 - Initial architecture specification (October 12, 2025)

**References**
- IEEE Std 1722.1-2021 - IEEE Standard for Device Discovery, Connection Management, and Control Protocol
- Milan Specification v1.2/v2.0a - AVnu Alliance Professional Audio Profile
- ISO/IEC/IEEE 42010:2011 - Systems and software engineering — Architecture description