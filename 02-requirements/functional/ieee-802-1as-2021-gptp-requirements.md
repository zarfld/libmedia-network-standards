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
    - REQ-STK-IEEE8021AS-001
    - REQ-STK-IEEE8021AS-002
    - REQ-STK-IEEE8021AS-003
    - REQ-STK-IEEE8021AS-004
    - REQ-STK-IEEE8021AS-005
---
# IEEE 802.1AS-2021 gPTP Requirements Specification

**Standards Compliance**: ISO/IEC/IEEE 29148:2018  
**Standard Reference**: IEEE Std 802.1AS-2021 "IEEE Standard for Local and metropolitan area networks - Timing and Synchronization for Time-Sensitive Applications in Bridged Local Area Networks"  
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

This document specifies requirements for IEEE 802.1AS-2021 generalized Precision Time Protocol (gPTP) implementation within the professional media networking ecosystem. gPTP provides the critical timing synchronization layer that enables deterministic, time-sensitive networking for professional audio and video applications.

### Scope

The requirements cover the complete IEEE 802.1AS-2021 specification including:

- Enhanced multi-domain support for complex network topologies
- Improved path delay mechanisms (peer-to-peer and end-to-end)
- Extended TLV support for enhanced capabilities
- Better security framework for secure time distribution
- YANG model support for network management integration
- Backward compatibility with 802.1AS-2020/2011

### External Authority Documents

The following authoritative standards documents SHALL be used as additional reference for architecture and implementation. Only the current standard version and its direct foundational dependencies are included:

#### Primary Standard (THIS SPECIFICATION)

- **ISO-IEC-IEEE 8802-1AS-2021-en.pdf** - IEEE Standard for Local and Metropolitan Area Networks - Timing and Synchronization for Time-Sensitive Applications in Bridged Local Area Networks
  - **MCP Path**: `file://D:/SyncDrive/SynologyDrive/MCP/Standards/ISO-IEC-IEEE_8802-1AS-2021-en.pdf`
  - **Usage**: PRIMARY and ONLY specification for IEEE 802.1AS-2021 gPTP implementation
  - **Key Sections**: All protocol state machines, multi-domain support, path delay mechanisms

#### Direct Foundation Dependencies (REQUIRED)

- **IEEE 1588-2019-en.pdf** - IEEE Standard for Precision Time Protocol (PTPv2)
  - **MCP Path**: `file://D:/SyncDrive/SynologyDrive/MCP/Standards/IEEE_1588-2019-en.pdf`
  - **Usage**: Foundation PTP concepts that gPTP extends and profiles
  - **Key Sections**: Basic PTP concepts, clock types, message formats

#### MCP Usage Instructions

To access the primary standard during development:

```bash
# Access IEEE 802.1AS-2021 specification (PRIMARY AUTHORITY)
mcp_markitdown_convert_to_markdown "file://D:/SyncDrive/SynologyDrive/MCP/Standards/ISO-IEC-IEEE_8802-1AS-2021-en.pdf"
```

**CRITICAL**: Only use IEEE 802.1AS-2021 for gPTP implementation. Do NOT mix with other gPTP versions (2020, 2011) to prevent standard conflicts.

**COPYRIGHT NOTICE**: Reference by section/clause number only. No content reproduction permitted.

### Context

IEEE 802.1AS-2021 gPTP serves as the timing synchronization foundation in the professional media networking protocol stack:

```text
┌─────────────────┐
│ Milan Extensions │ ← Professional Audio Application Profiles
├─────────────────┤
│ IEEE 1722.1     │ ← AVDECC Device Control
│ AVDECC          │
├─────────────────┤
│ IEEE 1722 AVTP  │ ← Transport Protocol
├─────────────────┤
│ IEEE 802.1AS    │ ← Timing Synchronization (THIS SPECIFICATION)
│ gPTP            │
├─────────────────┤
│ IEEE 802.1Q     │ ← Bridging & QoS
│ Bridging/TSN    │
└─────────────────┘
```

## Stakeholder Requirements

### REQ-STK-IEEE8021AS-001: Professional Audio Engineers

**Need**: Precise timing synchronization for professional audio production systems

**Rationale**: Professional audio applications require sub-microsecond timing accuracy for multi-channel recording, live mixing, and broadcast production

**Success Criteria**: Timing accuracy better than 1 microsecond across all network nodes, synchronized sample clock distribution

### REQ-STK-IEEE8021AS-002: Video Production Teams

**Need**: Frame-accurate timing synchronization for professional video production workflows

**Rationale**: Video production requires precise frame timing for multi-camera synchronization, live switching, and post-production workflows

**Success Criteria**: Frame-locked timing distribution, synchronization accuracy supporting 4K/8K video production

### REQ-STK-IEEE8021AS-003: Broadcast Infrastructure Operators

**Need**: Reliable, scalable timing distribution for broadcast facility infrastructure

**Rationale**: Broadcast facilities require fault-tolerant timing distribution across large-scale networks with multiple redundant paths

**Success Criteria**: Multi-domain support, redundant grandmaster capability, seamless failover mechanisms

### REQ-STK-IEEE8021AS-004: TSN Network Equipment Manufacturers

**Need**: IEEE 802.1AS-2021 compliant timing distribution for TSN infrastructure products

**Rationale**: TSN switches and endpoints must provide standards-compliant gPTP implementation for certification and interoperability

**Success Criteria**: Full IEEE 802.1AS-2021 compliance, interoperability with major TSN vendors, certification readiness

### REQ-STK-IEEE8021AS-005: System Integration Companies

**Need**: Manageable and monitorable timing distribution for complex installation projects

**Rationale**: Large-scale professional AV installations require timing distribution that can be monitored, managed, and troubleshooted

**Success Criteria**: Network management integration, timing quality monitoring, diagnostic capabilities

## System Requirements

### REQ-SYS-IEEE8021AS-001: Enhanced Multi-Domain Support

**Requirement**: The IEEE 802.1AS-2021 implementation SHALL support multiple timing domains for complex network topologies

**Derived From**: REQ-STK-IEEE8021AS-003, REQ-STK-IEEE8021AS-005

**Dependencies**: Domain-aware network infrastructure, multi-domain capable bridges

### REQ-SYS-IEEE8021AS-002: Precision Timing Accuracy

**Requirement**: The implementation SHALL achieve sub-microsecond timing synchronization accuracy

**Derived From**: REQ-STK-IEEE8021AS-001, REQ-STK-IEEE8021AS-002

**Criteria**: Timing accuracy better than 500 nanoseconds under normal network conditions

### REQ-SYS-IEEE8021AS-003: Scalable Network Architecture

**Requirement**: The implementation SHALL support scalable timing distribution across large network topologies

**Derived From**: REQ-STK-IEEE8021AS-003, REQ-STK-IEEE8021AS-005

**Constraints**: Support for minimum 1000 nodes, maximum 20 hop paths, multiple redundant timing sources

### REQ-SYS-IEEE8021AS-004: Backward Compatibility

**Requirement**: The implementation SHALL maintain backward compatibility with existing 802.1AS deployments

**Derived From**: REQ-STK-IEEE8021AS-004, REQ-STK-IEEE8021AS-005

**Dependencies**: IEEE 802.1AS-2020/2011 interoperability, graceful feature negotiation

### REQ-SYS-IEEE8021AS-005: Security and Management

**Requirement**: The implementation SHALL provide enhanced security and management capabilities

**Derived From**: REQ-STK-IEEE8021AS-003, REQ-STK-IEEE8021AS-005

**Features**: Secure time distribution, YANG model support, network management integration

## Functional Requirements

### Multi-Domain Support Requirements

#### REQ-FUN-IEEE8021AS-001: Domain Configuration

**Requirement**: The implementation SHALL support configuration and management of multiple timing domains

**Details**:

- Domain number assignment and validation (0-127)
- Per-domain timing parameters configuration
- Domain isolation and cross-domain communication
- Domain priority and selection algorithms

**Acceptance Criteria**: Multiple timing domains operate independently, domain conflicts are resolved according to specification

#### REQ-FUN-IEEE8021AS-002: Cross-Domain Synchronization

**Requirement**: The implementation SHALL support synchronization relationships between timing domains

**Details**:

- Inter-domain timing relationships
- Domain boundary clock functionality
- Timing domain translation capabilities
- Domain synchronization monitoring

**Acceptance Criteria**: Cross-domain timing relationships maintain specified accuracy, domain boundaries function correctly

### Path Delay Mechanism Requirements

#### REQ-FUN-IEEE8021AS-003: Peer-to-Peer Path Delay

**Requirement**: The implementation SHALL support peer-to-peer path delay measurement mechanism

**Details**:

- Pdelay_Req message transmission and processing
- Pdelay_Resp message handling with timestamps
- Path delay calculation and filtering
- Asymmetry correction capabilities

**Acceptance Criteria**: Path delay measurements are accurate within specification limits, delay calculations update smoothly

#### REQ-FUN-IEEE8021AS-004: End-to-End Path Delay

**Requirement**: The implementation SHALL support end-to-end path delay measurement mechanism (new in 2021)

**Details**:

- Delay_Req message transmission from slave clocks
- Delay_Resp message processing from master clocks
- End-to-end delay calculation and correction
- Hybrid delay mechanism support

**Acceptance Criteria**: End-to-end delay measurements provide accurate timing correction, mechanism selection operates correctly

### Best Master Clock Algorithm (BMCA) Requirements

#### REQ-FUN-IEEE8021AS-005: Enhanced Clock Selection

**Requirement**: The implementation SHALL support enhanced BMCA for optimal grandmaster selection

**Details**:

- Priority1/Priority2 field processing
- Clock quality evaluation with enhanced metrics
- Clock class and accuracy comparisons
- Variance and stability assessments

**Acceptance Criteria**: Best master clock selection produces optimal timing source, clock changes are handled gracefully

#### REQ-FUN-IEEE8021AS-006: Multi-Domain BMCA

**Requirement**: The implementation SHALL support BMCA operation across multiple domains

**Details**:

- Per-domain BMCA execution
- Domain-specific clock priorities
- Inter-domain timing quality comparison
- Domain boundary clock BMCA participation

**Acceptance Criteria**: BMCA operates correctly within each domain, domain interactions maintain timing integrity

### Message Processing Requirements

#### REQ-FUN-IEEE8021AS-007: Core gPTP Messages

**Requirement**: The implementation SHALL support all core gPTP message types with enhanced capabilities

**Details**:

- Sync message transmission and reception with precise timestamps
- Follow_Up message processing with correction field handling
- Announce message processing with enhanced TLV support
- Signaling message support for capability negotiation

**Acceptance Criteria**: All message types are processed correctly, timestamps are accurate, TLVs are handled properly

#### REQ-FUN-IEEE8021AS-008: Extended TLV Support

**Requirement**: The implementation SHALL support extended TLV capabilities for enhanced functionality

**Details**:

- Standard TLV types processing (PATH_TRACE, ALTERNATE_TIME_OFFSET, etc.)
- Custom TLV support for vendor extensions
- TLV validation and error handling
- Dynamic TLV configuration capabilities

**Acceptance Criteria**: All standard TLVs are supported, custom TLVs can be processed, invalid TLVs are handled gracefully

### State Machine Requirements

#### REQ-FUN-IEEE8021AS-009: Port State Machines

**Requirement**: The implementation SHALL implement enhanced port state machines for improved reliability

**Details**:

- Master/Slave/Passive/Disabled state management
- State transition timing and conditions
- Fault detection and recovery mechanisms
- Port role announcement and negotiation

**Acceptance Criteria**: State transitions occur correctly, fault recovery is automatic, port roles are properly negotiated

#### REQ-FUN-IEEE8021AS-010: Site Synchronization

**Requirement**: The implementation SHALL support site synchronization state machines for multi-port devices

**Details**:

- Local clock synchronization across multiple ports
- Clock servo algorithms for frequency and phase correction
- Synchronization quality monitoring and reporting
- Clock drift compensation and stability

**Acceptance Criteria**: Multi-port synchronization maintains accuracy, clock servo performs effectively, synchronization quality is monitored

## Non-Functional Requirements

### REQ-NFR-IEEE8021AS-001: Timing Performance Requirements

**Accuracy**: Timing synchronization SHALL achieve better than 500 nanoseconds accuracy under normal conditions

**Stability**: Clock frequency stability SHALL be maintained within ±100 parts per billion

**Convergence**: Initial synchronization SHALL be achieved within 10 seconds of network connection

**Recovery**: Timing recovery after fault SHALL complete within 2 seconds

### REQ-NFR-IEEE8021AS-002: Scalability Requirements

**Network Size**: The implementation SHALL support networks with minimum 1000 synchronized nodes

**Hop Count**: Timing distribution SHALL maintain accuracy across minimum 20 network hops

**Domain Count**: The implementation SHALL support minimum 16 simultaneous timing domains

**Message Rate**: The implementation SHALL handle minimum 1000 gPTP messages per second per port

### REQ-NFR-IEEE8021AS-003: Reliability Requirements

**Availability**: The timing synchronization SHALL maintain 99.99% availability during continuous operation

**Fault Tolerance**: The implementation SHALL automatically recover from single-point failures

**Redundancy**: Multiple grandmaster support SHALL provide seamless failover capability

**Error Rate**: Message processing error rate SHALL not exceed 0.01% under normal conditions

### REQ-NFR-IEEE8021AS-004: Security Requirements

**Authentication**: The implementation SHALL support authentication mechanisms for secure time distribution

**Integrity**: Timing messages SHALL include integrity verification mechanisms

**Authorization**: Access to timing configuration SHALL be controlled through secure mechanisms

**Audit**: Security events SHALL be logged for audit and compliance purposes

## Use Cases

### UC-IEEE8021AS-001: Professional Audio Studio Setup

**Actors**: Audio Engineer, Audio Equipment, gPTP Infrastructure

**Description**: Audio engineer sets up synchronized recording studio with multiple audio devices

**Preconditions**: TSN network infrastructure with gPTP support is available

**Flow**:

1. Audio devices connect to TSN network and begin gPTP synchronization
2. Grandmaster clock is selected through BMCA process
3. All devices achieve timing synchronization within convergence time
4. Audio sampling clocks are locked to network timing reference
5. Recording session begins with all devices synchronized

**Postconditions**: All audio devices are precisely synchronized, enabling multi-channel recording

### UC-IEEE8021AS-002: Live Broadcast Production

**Actors**: Broadcast Engineer, Video/Audio Equipment, Timing Infrastructure

**Description**: Live broadcast production requires frame-accurate synchronization across production chain

**Preconditions**: Broadcast facility network with redundant timing infrastructure

**Flow**:

1. Production equipment connects to primary timing domain
2. Backup timing domain operates in parallel for redundancy
3. Frame timing is distributed to all cameras and production equipment
4. Live production begins with synchronized video/audio capture
5. Automatic failover occurs if primary timing source fails

**Postconditions**: Live production continues with maintained synchronization despite timing failures

### UC-IEEE8021AS-003: Large-Scale Installation

**Actors**: System Integrator, Professional AV Equipment, Network Management

**Description**: Large-scale professional AV installation with complex timing requirements

**Preconditions**: Multi-domain network design with timing quality monitoring

**Flow**:

1. System integrator configures multiple timing domains for different AV zones
2. Equipment in each zone synchronizes to appropriate domain
3. Cross-domain synchronization is established where required
4. Network management system monitors timing quality across installation
5. Timing performance is validated and optimized

**Postconditions**: Complex AV installation operates with maintained timing across all zones

## User Stories

### US-IEEE8021AS-001: Automatic Timing Synchronization

**As a** professional audio engineer  
**I want** automatic timing synchronization across all my audio equipment  
**So that** I can record and mix audio without worrying about timing issues  

**Acceptance Criteria**:

- All audio devices automatically synchronize when connected to network
- Synchronization accuracy meets professional audio requirements
- Timing status is clearly visible in equipment interfaces

### US-IEEE8021AS-002: Redundant Timing Infrastructure

**As a** broadcast facility operator  
**I want** redundant timing distribution with automatic failover  
**So that** live broadcasts continue uninterrupted during timing failures  

**Acceptance Criteria**:

- Multiple grandmaster clocks provide redundancy
- Failover occurs automatically without service interruption
- Timing quality is monitored and alerted

### US-IEEE8021AS-003: Multi-Zone Timing Management

**As a** system integrator  
**I want** to manage timing across multiple zones in large installations  
**So that** I can optimize timing performance for different application requirements  

**Acceptance Criteria**:

- Multiple timing domains can be configured and managed
- Timing performance is monitored across all zones
- Configuration changes can be made without service interruption

## Acceptance Criteria

### AC-IEEE8021AS-001: IEEE 802.1AS-2021 Compliance

- [ ] All mandatory gPTP message types implemented correctly
- [ ] Multi-domain support fully functional and tested
- [ ] Enhanced BMCA algorithm implemented and validated
- [ ] Extended TLV support complete and interoperable
- [ ] Security framework implemented according to specification
- [ ] YANG model support for network management integration

### AC-IEEE8021AS-002: Timing Performance Validation

- [ ] Timing accuracy better than 500 nanoseconds achieved in testing
- [ ] Convergence time meets specification requirements
- [ ] Clock stability maintained under various load conditions
- [ ] Scalability validated with large network topologies
- [ ] Fault recovery tested and meets timing requirements

### AC-IEEE8021AS-003: Interoperability and Compatibility

- [ ] Interoperability demonstrated with major TSN equipment vendors
- [ ] Backward compatibility with 802.1AS-2020/2011 verified
- [ ] Professional audio equipment synchronization validated
- [ ] Milan compatibility demonstrated and tested
- [ ] Network management integration functional

### AC-IEEE8021AS-004: Reliability and Security

- [ ] Redundant timing operation tested and validated
- [ ] Security mechanisms implemented and tested
- [ ] Fault injection testing passed
- [ ] Long-term stability testing completed
- [ ] Performance under stress conditions validated

## Requirements Traceability

### Stakeholder to System Requirements Mapping

| Stakeholder Requirement | System Requirements | Implementation Focus |
|------------------------|-------------------|---------------------|
| REQ-STK-IEEE8021AS-001 Professional Audio Engineers | REQ-SYS-IEEE8021AS-002, REQ-SYS-IEEE8021AS-003 | Precision timing, scalability |
| REQ-STK-IEEE8021AS-002 Video Production Teams | REQ-SYS-IEEE8021AS-002, REQ-SYS-IEEE8021AS-004 | Timing accuracy, compatibility |
| REQ-STK-IEEE8021AS-003 Broadcast Infrastructure | REQ-SYS-IEEE8021AS-001, REQ-SYS-IEEE8021AS-003, REQ-SYS-IEEE8021AS-005 | Multi-domain, scalability, security |
| REQ-STK-IEEE8021AS-004 TSN Equipment Manufacturers | REQ-SYS-IEEE8021AS-004, REQ-SYS-IEEE8021AS-005 | Standards compliance, management |
| REQ-STK-IEEE8021AS-005 System Integration Companies | REQ-SYS-IEEE8021AS-001, REQ-SYS-IEEE8021AS-005 | Multi-domain, management capabilities |

### Functional Requirements Implementation

| Functional Requirement | Implementation Components | Test Coverage |
|----------------------|-------------------------|--------------|
| REQ-FUN-IEEE8021AS-001 Domain Configuration | Multi-domain state machines, configuration management | Domain isolation tests |
| REQ-FUN-IEEE8021AS-003 Peer-to-Peer Delay | Path delay measurement, timestamp processing | Delay accuracy tests |
| REQ-FUN-IEEE8021AS-005 Clock Selection | Enhanced BMCA algorithm, clock quality evaluation | Master selection tests |
| REQ-FUN-IEEE8021AS-007 Message Processing | Message handlers, timestamp generation | Protocol compliance tests |
| REQ-FUN-IEEE8021AS-009 State Machines | Port state management, transition logic | State machine validation |

---

**Document History**
- Version 1.0 (October 9, 2025): Initial requirements specification based on IEEE 802.1AS-2021 implementation analysis

**References**
- IEEE Std 802.1AS-2021: IEEE Standard for Local and metropolitan area networks - Timing and Synchronization for Time-Sensitive Applications in Bridged Local Area Networks
- ISO/IEC/IEEE 29148:2018: Systems and software engineering - Life cycle processes - Requirements engineering
- IEEE Std 1588-2019: IEEE Standard for a Precision Clock Synchronization Protocol for Networked Measurement and Control Systems
