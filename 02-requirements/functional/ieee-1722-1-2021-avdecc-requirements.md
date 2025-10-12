---
specType: requirements
standard: "29148"
phase: "02-requirements"
version: "1.0.0"
author: "Requirements Engineering Team"
date: "2025-10-10"
status: "approved"
traceability:
  stakeholderRequirements:
    - "REQ-STK-IEEE17221-001"
    - "REQ-STK-IEEE17221-002"
    - "REQ-STK-IEEE17221-003"
    - "REQ-STK-IEEE17221-004"
    - "REQ-STK-IEEE17221-005"
---

# IEEE 1722.1-2021 AVDECC Requirements Specification

**Standards Compliance**: ISO/IEC/IEEE 29148:2018  
**Standard Reference**: IEEE Std 1722.1-2021 "IEEE Standard for Device Discovery, Connection Management, and Control Protocol for Time-Sensitive Networking System"  
**Document Version**: 1.0  
**Date**: October 9, 2025  
**Prepared by**: Standards-Compliant Software Development Team

## Table of Contents

1. [Introduction](#introduction)
2. [Stakeholder Requirements](#stakeholder-requirements)
3. [System Requirements](#system-requirements)
4. [Functional Requirements](#functional-requirements)
5. [Non-Functional Requirements](#non-functional-requirements)
6. [Use Cases](#use-cases)
7. [User Stories](#user-stories)
8. [Acceptance Criteria](#acceptance-criteria)
9. [Requirements Traceability](#requirements-traceability)

## Introduction

### Purpose

This document specifies requirements for IEEE 1722.1-2021 AVDECC (Audio Video Discovery, Enumeration, Connection management, and Control) protocol implementation within the professional media networking ecosystem. AVDECC provides the foundational device control layer for Time-Sensitive Networking (TSN) systems.

### Scope

The requirements cover the complete IEEE 1722.1-2021 specification including:

- ADP (AVDECC Discovery Protocol) for entity advertisement and discovery
- AECP (AVDECC Enumeration and Control Protocol) for device enumeration and control
- ACMP (AVDECC Connection Management Protocol) for stream connection management
- AEM (AVDECC Entity Model) for device representation and descriptor management

### External Authority Documents

The following authoritative standards documents SHALL be used as additional reference for architecture and implementation. Only the current standard version and its direct foundational dependencies are included to prevent standard version conflicts:

#### Primary Standard (THIS SPECIFICATION)

- **IEEE 1722.1-2021-en.pdf** - IEEE Standard for Device Discovery, Connection Management, and Control Protocol for IEEE 802 Based Networks
  - **MCP Path**: `file://D:/SyncDrive/SynologyDrive/MCP/Standards/IEEE_1722_1-2021-en.pdf`
  - **Usage**: PRIMARY and ONLY specification for IEEE 1722.1-2021 AVDECC protocol implementation
  - **Key Sections**: All protocol state machines, message formats, entity model definitions

#### Direct Foundation Dependencies (REQUIRED)

- **IEEE 1722-2016-en.pdf** - IEEE Standard for Audio Video Transport Protocol (AVTP)
  - **MCP Path**: `file://D:/SyncDrive/SynologyDrive/MCP/Standards/IEEE_1722-2016-en.pdf`
  - **Usage**: Required for ACMP stream connection management (AVDECC depends on AVTP)
  - **Key Sections**: Stream format specifications, stream ID definitions

- **ISO-IEC-IEEE 8802-1AS-2021-en.pdf** - Generalized Precision Time Protocol (gPTP) 
  - **MCP Path**: `file://D:/SyncDrive/SynologyDrive/MCP/Standards/ISO-IEC-IEEE_8802-1AS-2021-en.pdf`
  - **Usage**: Required for timing synchronization mechanisms in AVDECC entities
  - **Key Sections**: gPTP domain support, grandmaster clock identification

#### MCP Usage Instructions

To access the primary standard during development:

```bash
# Access IEEE 1722.1-2021 specification (PRIMARY AUTHORITY)
mcp_markitdown_convert_to_markdown "file://D:/SyncDrive/SynologyDrive/MCP/Standards/IEEE_1722_1-2021-en.pdf"
```

**CRITICAL**: Only use IEEE 1722.1-2021 for AVDECC implementation. Do NOT mix with other AVDECC versions (2013, 2011) to prevent standard conflicts.

**COPYRIGHT NOTICE**: Reference by section/clause number only. No content reproduction permitted.

### Context

IEEE 1722.1-2021 AVDECC serves as the core device control protocol in the professional media networking protocol stack:

```text
┌─────────────────┐
│ Milan Extensions │ ← Professional Audio Application Profiles
├─────────────────┤
│ IEEE 1722.1     │ ← AVDECC Device Control (THIS SPECIFICATION)
│ AVDECC          │
├─────────────────┤
│ IEEE 1722 AVTP  │ ← Transport Protocol
├─────────────────┤
│ IEEE 802.1AS    │ ← Timing Synchronization
│ gPTP            │
├─────────────────┤
│ IEEE 802.1Q     │ ← Bridging & QoS
│ Bridging/TSN    │
└─────────────────┘
```

## Stakeholder Requirements

### REQ-STK-IEEE17221-001: Professional Audio System Integrators

**Need**: Deterministic device discovery and connection management for professional audio networking systems

**Rationale**: Professional audio environments require reliable, standards-based device control for live production, broadcast, and installed sound systems

**Success Criteria**: Zero-configuration device discovery, automated connection establishment, real-time control capabilities

### REQ-STK-IEEE17221-002: Audio Equipment Manufacturers

**Need**: Standards-compliant device control protocol implementation for professional audio equipment

**Rationale**: Interoperability across multi-vendor professional audio networks requires IEEE 1722.1 compliance

**Success Criteria**: Full IEEE 1722.1-2021 compliance, Milan compatibility, certification readiness

### REQ-STK-IEEE17221-003: Network Administrators

**Need**: Manageable, secure, and monitorable professional audio network infrastructure

**Rationale**: Enterprise and broadcast facilities require network visibility, control, and security for mission-critical audio systems

**Success Criteria**: Network management integration, security compliance, operational monitoring

### REQ-STK-IEEE17221-004: Software Developers

**Need**: Complete, well-documented, and maintainable IEEE 1722.1 implementation library

**Rationale**: Application developers require reliable foundation for building professional audio control applications

**Success Criteria**: Complete API coverage, comprehensive documentation, example implementations

### REQ-STK-IEEE17221-005: Regulatory and Compliance Bodies

**Need**: Verifiable IEEE 1722.1-2021 standards compliance and certification capability

**Rationale**: Professional audio equipment must demonstrate standards compliance for market acceptance

**Success Criteria**: Full standards compliance, comprehensive test coverage, certification documentation

## System Requirements

### REQ-SYS-IEEE17221-001: Protocol Stack Integration

**Requirement**: The IEEE 1722.1 AVDECC implementation SHALL integrate seamlessly with the professional media networking protocol stack

**Derived From**: REQ-STK-IEEE17221-001, REQ-STK-IEEE17221-002

**Dependencies**: IEEE 1722 AVTP transport, IEEE 802.1AS timing synchronization, IEEE 802.1Q bridging

### REQ-SYS-IEEE17221-002: Standards Compliance

**Requirement**: The implementation SHALL fully comply with IEEE Std 1722.1-2021 specification

**Derived From**: REQ-STK-IEEE17221-002, REQ-STK-IEEE17221-005

**Verification**: Standards compliance testing, certification validation

### REQ-SYS-IEEE17221-003: Milan Compatibility

**Requirement**: The implementation SHALL support Milan professional audio extensions and interoperability requirements

**Derived From**: REQ-STK-IEEE17221-001, REQ-STK-IEEE17221-002

**Dependencies**: AVnu Milan v1.2/v2.0a specification compliance

### REQ-SYS-IEEE17221-004: Cross-Platform Support

**Requirement**: The implementation SHALL support Windows and Linux operating systems with consistent behavior

**Derived From**: REQ-STK-IEEE17221-002, REQ-STK-IEEE17221-004

**Constraints**: C++17 standard, CMake build system, platform abstraction layer

### REQ-SYS-IEEE17221-005: Real-Time Performance

**Requirement**: The implementation SHALL meet real-time performance requirements for professional audio applications

**Derived From**: REQ-STK-IEEE17221-001, REQ-STK-IEEE17221-003

**Criteria**: Sub-millisecond command response times, deterministic behavior, priority-based processing

## Functional Requirements

### ADP (AVDECC Discovery Protocol) Requirements

#### REQ-FUN-IEEE17221-001: Entity Advertisement

**Requirement**: The implementation SHALL support ADP entity advertisement for device discovery

**Details**:
- ENTITY_AVAILABLE message transmission with configurable intervals
- Entity capabilities advertisement (Talker, Listener, Controller)
- gPTP synchronization status reporting
- Milan professional audio device identification

**Acceptance Criteria**: Entity advertisements are transmitted according to IEEE 1722.1-2021 timing requirements, received devices are discovered within 2 seconds

#### REQ-FUN-IEEE17221-002: Entity Discovery

**Requirement**: The implementation SHALL support ADP entity discovery for network device enumeration

**Details**:
- ENTITY_DISCOVER message processing
- Entity database maintenance with timeout handling
- Network topology tracking
- Multi-interface entity support

**Acceptance Criteria**: All available entities are discovered and maintained in entity database, departing entities are detected within valid_time interval

### AECP (AVDECC Enumeration and Control Protocol) Requirements

#### REQ-FUN-IEEE17221-003: AEM Command Processing

**Requirement**: The implementation SHALL support complete AEM command set for device control

**Details**:
- READ_DESCRIPTOR command for entity model enumeration
- GET/SET_CONFIGURATION commands for device configuration
- ACQUIRE_ENTITY/RELEASE_ENTITY commands for exclusive control
- GET/SET_STREAM_FORMAT commands for stream configuration

**Acceptance Criteria**: All mandatory AEM commands are processed correctly with appropriate response codes and data

#### REQ-FUN-IEEE17221-004: Command Response Handling

**Requirement**: The implementation SHALL provide reliable command/response transaction handling

**Details**:
- Sequence number management for command correlation
- Timeout and retransmission mechanisms
- Status code interpretation and error handling
- Concurrent command processing support

**Acceptance Criteria**: Commands are reliably delivered with proper timeout handling, duplicate detection, and error reporting

#### REQ-FUN-IEEE17221-005: Milan MVU Command Support

**Requirement**: The implementation SHALL support Milan Manufacturer Specific commands for professional audio features

**Details**:
- GET_MILAN_INFO command for Milan capability discovery
- GET/SET_SYSTEM_UNIQUE_ID commands for device identification
- MILAN_INFO response with professional audio capabilities
- Milan certification level reporting

**Acceptance Criteria**: Milan MVU commands are processed correctly, Milan capabilities are properly reported

### ACMP (AVDECC Connection Management Protocol) Requirements

#### REQ-FUN-IEEE17221-006: Stream Connection Management

**Requirement**: The implementation SHALL support ACMP stream connection establishment and management

**Details**:
- CONNECT_TX_COMMAND/RESPONSE for Talker stream connections
- CONNECT_RX_COMMAND/RESPONSE for Listener stream connections
- DISCONNECT_TX/RX commands for connection teardown
- GET_TX/RX_STATE commands for connection status

**Acceptance Criteria**: Stream connections are established reliably, connection state is maintained accurately, disconnections are handled gracefully

#### REQ-FUN-IEEE17221-007: Connection State Management

**Requirement**: The implementation SHALL maintain accurate stream connection state information

**Details**:
- Connection database with stream ID mapping
- Connection count tracking per stream
- Connection flags and VLAN ID management
- Stream format validation before connection

**Acceptance Criteria**: Connection state database remains consistent, connection conflicts are detected and prevented

### AEM (AVDECC Entity Model) Requirements

#### REQ-FUN-IEEE17221-008: Descriptor Management

**Requirement**: The implementation SHALL provide complete AEM descriptor support for device representation

**Details**:
- Entity Descriptor with device identification and capabilities
- Configuration Descriptors for multiple device configurations
- Stream Input/Output Descriptors for stream endpoints
- Control Descriptors for device control points

**Acceptance Criteria**: All descriptor types are implemented correctly, descriptor data is accessible through READ_DESCRIPTOR commands

#### REQ-FUN-IEEE17221-009: Entity Model Validation

**Requirement**: The implementation SHALL validate entity model consistency and compliance

**Details**:
- Descriptor cross-reference validation
- Configuration completeness checking
- Stream format compatibility verification
- Control value range validation

**Acceptance Criteria**: Entity model is validated for consistency, invalid configurations are rejected with appropriate error codes

## Non-Functional Requirements

### REQ-NFR-IEEE17221-001: Performance Requirements

**Response Time**: AEM command processing SHALL complete within 100ms for simple operations, 500ms for complex operations

**Throughput**: The implementation SHALL handle minimum 1000 AEM commands per second per entity

**Scalability**: The implementation SHALL support minimum 200 simultaneous entities in network discovery database

### REQ-NFR-IEEE17221-002: Reliability Requirements

**Availability**: The AVDECC protocol implementation SHALL maintain 99.9% uptime during continuous operation

**Error Rate**: Command processing error rate SHALL not exceed 0.1% under normal network conditions

**Recovery**: The implementation SHALL automatically recover from network failures within 5 seconds

### REQ-NFR-IEEE17221-003: Security Requirements

**Authentication**: The implementation SHALL support secure authentication mechanisms where specified by IEEE 1722.1-2021

**Authorization**: Entity control access SHALL be managed through acquisition/release mechanisms

**Data Integrity**: All protocol messages SHALL include integrity verification mechanisms

### REQ-NFR-IEEE17221-004: Compliance Requirements

**Standards Compliance**: The implementation SHALL pass 100% of IEEE 1722.1-2021 conformance tests

**Milan Compliance**: The implementation SHALL achieve Milan certification for professional audio applications

**Interoperability**: The implementation SHALL demonstrate interoperability with major professional audio equipment vendors

## Use Cases

### UC-IEEE17221-001: Professional Audio System Setup

**Actors**: Audio Engineer, Professional Audio Devices

**Description**: An audio engineer configures a professional audio system for live performance

**Preconditions**: Professional audio devices are connected to TSN network

**Flow**:
1. Audio devices advertise themselves via ADP ENTITY_AVAILABLE messages
2. Control application discovers devices and reads entity models via AECP
3. Engineer selects stream connections between devices
4. Control application establishes connections via ACMP
5. Audio streams are established and monitored

**Postconditions**: Professional audio system is configured and operational with active stream connections

### UC-IEEE17221-002: Device Control and Monitoring

**Actors**: System Operator, Milan-Compatible Devices  

**Description**: A system operator monitors and controls professional audio devices during operation

**Preconditions**: Devices are discovered and connections established

**Flow**:
1. Operator requests device status via AECP commands
2. Devices respond with current configuration and operational status
3. Operator adjusts device parameters via SET commands
4. Devices confirm parameter changes via response messages
5. System continues monitoring with periodic status requests

**Postconditions**: Device parameters are adjusted, operational status is continuously monitored

### UC-IEEE17221-003: Network Fault Handling

**Actors**: Network Infrastructure, Professional Audio Devices

**Description**: System handles network connectivity failures gracefully

**Preconditions**: Audio system is operational with active connections

**Flow**:
1. Network connectivity failure occurs
2. Devices detect missing ENTITY_AVAILABLE messages
3. Connection timeouts trigger reconnection attempts
4. Network connectivity is restored
5. Devices re-establish connections automatically

**Postconditions**: Audio system resumes normal operation after network recovery

## User Stories

### US-IEEE17221-001: Device Discovery
**As a** professional audio system integrator  
**I want** automatic device discovery capabilities  
**So that** I can quickly identify and configure all audio devices on the network  

**Acceptance Criteria**:
- All connected devices appear in discovery list within 2 seconds
- Device capabilities and model information are displayed
- Offline devices are automatically removed from list

### US-IEEE17221-002: Stream Connection Management  
**As an** audio engineer  
**I want** reliable stream connection management  
**So that** I can establish audio paths between devices without manual network configuration  

**Acceptance Criteria**:
- Stream connections are established with single click/command
- Connection status is clearly indicated
- Failed connections provide clear error messages

### US-IEEE17221-003: Real-Time Control
**As a** live sound operator  
**I want** responsive device control capabilities  
**So that** I can make real-time adjustments during performance  

**Acceptance Criteria**:
- Control commands respond within 100ms
- Parameter changes are immediately reflected in device behavior
- Control interface remains responsive during high network activity

## Acceptance Criteria

### AC-IEEE17221-001: IEEE 1722.1-2021 Compliance
- [ ] All mandatory ADP messages implemented and compliant
- [ ] Complete AEM command set implemented with correct behavior  
- [ ] ACMP connection management fully functional
- [ ] Entity model descriptors complete and validated
- [ ] Protocol message formats comply with IEEE specification
- [ ] Timing requirements met for all protocol operations

### AC-IEEE17221-002: Milan Professional Audio Support
- [ ] Milan MVU commands implemented and tested
- [ ] Professional audio capabilities properly advertised
- [ ] Milan interoperability baseline requirements met
- [ ] Milan certification testing passed
- [ ] Milan v1.2 and v2.0a features supported

### AC-IEEE17221-003: Integration and Performance  
- [ ] Seamless integration with IEEE 1722 AVTP transport
- [ ] Proper gPTP synchronization integration
- [ ] Cross-platform compatibility verified (Windows/Linux)
- [ ] Performance requirements met under load testing
- [ ] Memory usage within acceptable limits
- [ ] No memory leaks during extended operation

### AC-IEEE17221-004: Quality and Maintainability
- [ ] Code coverage > 90% for all protocol components
- [ ] API documentation complete and accurate  
- [ ] Example applications demonstrate key use cases
- [ ] Error handling comprehensive and tested
- [ ] Logging and debugging capabilities implemented

## Requirements Traceability

### Stakeholder to System Requirements Mapping

| Stakeholder Requirement | System Requirements | Rationale |
|------------------------|-------------------|-----------|
| REQ-STK-IEEE17221-001 Professional Audio System Integrators | REQ-SYS-IEEE17221-001, REQ-SYS-IEEE17221-005 | Protocol integration and performance |
| REQ-STK-IEEE17221-002 Audio Equipment Manufacturers | REQ-SYS-IEEE17221-002, REQ-SYS-IEEE17221-003, REQ-SYS-IEEE17221-004 | Standards compliance and compatibility |  
| REQ-STK-IEEE17221-003 Network Administrators | REQ-SYS-IEEE17221-001, REQ-SYS-IEEE17221-005 | Infrastructure integration and performance |
| REQ-STK-IEEE17221-004 Software Developers | REQ-SYS-IEEE17221-004 | Development environment support |
| REQ-STK-IEEE17221-005 Regulatory Bodies | REQ-SYS-IEEE17221-002, REQ-SYS-IEEE17221-003 | Compliance and certification |

### System to Functional Requirements Mapping

| System Requirement | Functional Requirements | Implementation |
|-------------------|----------------------|---------------|
| REQ-SYS-IEEE17221-001 Protocol Stack Integration | REQ-FUN-IEEE17221-001, REQ-FUN-IEEE17221-002, REQ-FUN-IEEE17221-006 | Protocol layer implementation |
| REQ-SYS-IEEE17221-002 Standards Compliance | All REQ-FUN-IEEE17221 requirements | IEEE 1722.1-2021 full implementation |
| REQ-SYS-IEEE17221-003 Milan Compatibility | REQ-FUN-IEEE17221-005, REQ-FUN-IEEE17221-009 | Milan MVU and entity model extensions |
| REQ-SYS-IEEE17221-004 Cross-Platform Support | All REQ-FUN-IEEE17221 requirements | Platform abstraction implementation |
| REQ-SYS-IEEE17221-005 Real-Time Performance | REQ-FUN-IEEE17221-004, REQ-NFR-IEEE17221-001 | Optimized command processing |

### Implementation Traceability

| Functional Requirement | Implementation Components | Test Cases |
|----------------------|-------------------------|-----------|
| REQ-FUN-IEEE17221-001 Entity Advertisement | `ATDECCDiscoveryProtocolPDU`, `ADP_Message_type` | ADP protocol tests |
| REQ-FUN-IEEE17221-003 AEM Commands | `AEMCommandMessage`, `AEMResponseMessage` | AECP command tests |  
| REQ-FUN-IEEE17221-005 Milan MVU | `MilanPAADEntity`, `GetMilanInfoCommand` | Milan extension tests |
| REQ-FUN-IEEE17221-006 Connection Management | `ACMPMessage`, Connection state machines | ACMP protocol tests |
| REQ-FUN-IEEE17221-008 Descriptors | `EntityDescriptor`, Descriptor management | Entity model tests |

---

**Document History**
- Version 1.0 (October 9, 2025): Initial requirements specification based on comprehensive IEEE 1722.1-2021 implementation analysis

**References**
- IEEE Std 1722.1-2021: IEEE Standard for Device Discovery, Connection Management, and Control Protocol for Time-Sensitive Networking Systems
- ISO/IEC/IEEE 29148:2018: Systems and software engineering - Life cycle processes - Requirements engineering
- AVnu Alliance Milan Specification v1.2-2023: Professional Audio AVB Devices