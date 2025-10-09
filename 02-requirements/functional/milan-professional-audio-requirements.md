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
    - "REQ-STK-MILAN-001"
    - "REQ-STK-MILAN-002"
    - "REQ-STK-MILAN-003"
    - "REQ-STK-MILAN-004"
    - "REQ-STK-MILAN-005"
---
# AVnu Milan Professional Audio Requirements Specification

**Standards Compliance**: ISO/IEC/IEEE 29148:2018  
**Standard Reference**: AVnu Alliance Milan Specification v1.2-2023 "Professional Audio AVB Device Application Profile"  
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

This document specifies requirements for AVnu Alliance Milan professional audio extensions within the professional media networking ecosystem. Milan provides application-level profiles and enhancements that enable seamless interoperability for professional audio devices based on IEEE AVB/TSN standards.

### Scope

The requirements cover the complete Milan specification including:

- Professional Audio AVB Device (PAAD) profiles and capabilities
- Milan baseline interoperability requirements
- Enhanced AVDECC commands and entity model extensions
- Professional audio device certification requirements
- Interoperability testing and validation procedures

### Context

Milan Professional Audio Extensions serve as the application layer in the professional media networking protocol stack:

```text
┌─────────────────┐
│ Milan Extensions │ ← Professional Audio Application Profiles (THIS SPECIFICATION)
├─────────────────┤
│ IEEE 1722.1     │ ← AVDECC Device Control
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

### SR-01: Professional Audio System Integrators

**Need**: Seamless interoperability between professional audio equipment from different manufacturers

**Rationale**: Professional audio installations require guaranteed interoperability to reduce integration risk and ensure reliable system operation

**Success Criteria**: Plug-and-play interoperability, automatic device discovery and configuration, standardized control interfaces

### SR-02: Professional Audio Equipment Manufacturers

**Need**: Clear specifications and certification process for Milan-compliant professional audio devices

**Rationale**: Equipment manufacturers require detailed specifications to ensure their products achieve Milan certification and market acceptance

**Success Criteria**: Milan certification achieved, interoperability demonstrated, competitive advantage through standards compliance

### SR-03: Live Sound Production Companies

**Need**: Reliable, professional-grade audio networking for live performance environments

**Rationale**: Live production environments require zero-failure networking with predictable behavior and professional audio quality

**Success Criteria**: Professional audio quality maintained, fault tolerance demonstrated, real-time performance verified

### SR-04: Recording and Post-Production Facilities

**Need**: High-quality, low-latency audio networking for professional content creation

**Rationale**: Professional recording requires pristine audio quality with minimal latency for artist monitoring and production workflows

**Success Criteria**: Uncompressed audio quality, low-latency monitoring, multi-room distribution capabilities

### SR-05: Broadcast and Media Organizations

**Need**: Standards-based professional audio infrastructure for broadcast production and distribution

**Rationale**: Broadcast facilities require standardized, interoperable audio infrastructure that integrates with existing broadcast workflows

**Success Criteria**: Broadcast integration achieved, reliability demonstrated, regulatory compliance maintained

## System Requirements

### SYS-01: Milan Baseline Interoperability

**Requirement**: The implementation SHALL achieve Milan baseline interoperability for seamless multi-vendor operation

**Derived From**: SR-01, SR-02

**Details**: Complete Milan v1.2 baseline requirements, certification compliance, interoperability validation

### SYS-02: Professional Audio Quality

**Requirement**: The implementation SHALL maintain professional audio quality standards throughout the signal chain

**Derived From**: SR-03, SR-04, SR-05

**Criteria**: Uncompressed audio transport, professional sample rates and bit depths, minimal latency and jitter

### SYS-03: Enhanced Device Control

**Requirement**: The implementation SHALL provide enhanced device control capabilities beyond basic IEEE 1722.1

**Derived From**: SR-01, SR-02

**Features**: Milan-specific AVDECC commands, enhanced entity model, professional audio device management

### SYS-04: Certification and Compliance

**Requirement**: The implementation SHALL meet all Milan certification requirements and testing procedures

**Derived From**: SR-02, SR-05

**Verification**: Milan certification testing, interoperability validation, compliance demonstration

### SYS-05: Real-Time Performance

**Requirement**: The implementation SHALL deliver real-time performance suitable for professional audio applications

**Derived From**: SR-03, SR-04

**Criteria**: Deterministic latency, real-time control response, professional monitoring capabilities

## Functional Requirements

### Milan PAAD (Professional Audio AVB Device) Requirements

#### FR-PAAD-01: Device Identification and Capabilities

**Requirement**: The implementation SHALL support Milan PAAD device identification and capability advertisement

**Details**:

- Milan device model identification with proper vendor and model codes
- Professional audio capability advertisement (channels, sample rates, formats)
- Milan version and certification level reporting
- System unique identifier management for device tracking

**Acceptance Criteria**: Milan devices are properly identified, capabilities are accurately reported, certification status is verifiable

#### FR-PAAD-02: Professional Audio Streaming

**Requirement**: The implementation SHALL support professional audio streaming with Milan requirements

**Details**:

- AAF audio format with professional specifications (48kHz/24-bit minimum)
- Multi-channel audio streaming (minimum 8 channels per stream)
- Professional sample rate support (44.1, 48, 88.2, 96, 176.4, 192 kHz)
- AES67 audio format compatibility for broadcast integration

**Acceptance Criteria**: Professional audio formats are supported correctly, audio quality meets professional standards, AES67 compatibility is maintained

### Milan Enhanced AVDECC Requirements

#### FR-AVDECC-01: Milan MVU Commands

**Requirement**: The implementation SHALL support Milan Manufacturer Vendor Unique (MVU) commands for enhanced functionality

**Details**:

- GET_MILAN_INFO command for Milan capability discovery
- SET/GET_SYSTEM_UNIQUE_ID commands for device identification
- SET/GET_MEDIA_CLOCK_REFERENCE_INFO for clock management
- Milan-specific status and diagnostic commands

**Acceptance Criteria**: All Milan MVU commands are implemented correctly, responses provide accurate information, error handling is robust

#### FR-AVDECC-02: Enhanced Entity Model

**Requirement**: The implementation SHALL provide enhanced entity model support for professional audio devices

**Details**:

- Milan-specific descriptor extensions and enhancements
- Professional audio control descriptors (gain, mute, phantom power, etc.)
- Enhanced stream format descriptors for professional audio
- Milan certification and capability descriptors

**Acceptance Criteria**: Enhanced entity model provides complete device representation, professional audio controls are accessible, Milan extensions are functional

### Interoperability and Baseline Requirements

#### FR-INTEROP-01: Milan Baseline Compliance

**Requirement**: The implementation SHALL comply with Milan baseline interoperability requirements

**Details**:

- Milan baseline device behavior requirements
- Mandatory AVDECC command support for Milan devices
- Professional audio streaming baseline requirements
- Fault tolerance and error handling requirements

**Acceptance Criteria**: Milan baseline requirements are fully met, interoperability with other Milan devices is demonstrated, certification testing is passed

#### FR-INTEROP-02: Multi-Vendor Interoperability

**Requirement**: The implementation SHALL demonstrate interoperability with Milan devices from multiple vendors

**Details**:

- Cross-vendor device discovery and control
- Stream establishment between different vendor devices
- Consistent behavior across vendor implementations
- Interoperability testing validation procedures

**Acceptance Criteria**: Interoperability is demonstrated with major professional audio vendors, cross-vendor stream connections function correctly, behavior is consistent

### Professional Audio Control Requirements

#### FR-CONTROL-01: Professional Audio Parameters

**Requirement**: The implementation SHALL provide control access to professional audio parameters

**Details**:

- Gain control with professional audio range and precision
- Mute and solo control for professional mixing applications
- Phantom power control for microphone inputs
- Input/output routing and matrix control capabilities

**Acceptance Criteria**: All professional audio parameters are controllable via AVDECC, control precision meets professional requirements, parameter changes are immediate

#### FR-CONTROL-02: Real-Time Monitoring

**Requirement**: The implementation SHALL provide real-time monitoring capabilities for professional audio systems

**Details**:

- Audio level monitoring with professional metering accuracy
- Stream status and quality monitoring
- Device health and diagnostic information
- Network performance and timing quality monitoring

**Acceptance Criteria**: Monitoring information is accurate and timely, professional metering standards are met, diagnostic information aids troubleshooting

### Milan Certification Requirements

#### FR-CERT-01: Certification Testing Support

**Requirement**: The implementation SHALL support Milan certification testing procedures and requirements

**Details**:

- Automated testing interface for certification procedures
- Compliance reporting and validation capabilities
- Test mode support for certification testing equipment
- Certification documentation and traceability support

**Acceptance Criteria**: Certification testing procedures are supported, compliance can be automatically verified, certification documentation is generated

#### FR-CERT-02: Quality Assurance and Validation

**Requirement**: The implementation SHALL provide comprehensive quality assurance and validation capabilities

**Details**:

- Built-in self-test (BIST) capabilities for device validation
- Performance benchmarking and compliance verification
- Interoperability test suite execution
- Quality metrics collection and reporting

**Acceptance Criteria**: Self-test capabilities validate device operation, performance meets specified benchmarks, interoperability is automatically verified

## Non-Functional Requirements

### NFR-01: Professional Audio Performance

**Audio Quality**: Professional audio quality SHALL be maintained throughout the signal chain with THD+N < 0.001%

**Latency**: End-to-end audio latency SHALL not exceed 3ms for professional monitoring applications

**Jitter**: Audio timing jitter SHALL be maintained below 1 microsecond for sample-accurate applications

**Dynamic Range**: Audio dynamic range SHALL exceed 120dB for professional recording applications

### NFR-02: Real-Time Responsiveness

**Control Response**: Control parameter changes SHALL be applied within 10ms of command receipt

**Monitoring Updates**: Monitoring information SHALL be updated at minimum 10Hz refresh rate

**Stream Establishment**: Audio stream connections SHALL be established within 2 seconds of request

**Fault Detection**: Network and device faults SHALL be detected and reported within 1 second

### NFR-03: Interoperability Requirements

**Multi-Vendor Support**: The implementation SHALL demonstrate interoperability with minimum 5 different Milan device manufacturers

**Certification Success**: The implementation SHALL achieve 100% success rate in Milan certification testing

**Baseline Compliance**: All Milan baseline requirements SHALL be met with 100% compliance

**Regression Testing**: Interoperability SHALL be maintained across software updates and revisions

### NFR-04: Reliability and Availability

**System Availability**: Milan devices SHALL maintain 99.99% availability during continuous professional operation

**Fault Recovery**: The implementation SHALL automatically recover from transient network faults within 5 seconds

**Error Rate**: Command processing error rate SHALL not exceed 0.01% under normal operating conditions

**Long-Term Stability**: The implementation SHALL operate continuously for minimum 30 days without restart or intervention

## Use Cases

### UC-01: Professional Studio Installation

**Actors**: Audio Engineer, Professional Audio Equipment, Milan Infrastructure

**Description**: Professional recording studio setup with Milan-compliant equipment from multiple vendors

**Preconditions**: Milan-certified professional audio equipment and network infrastructure

**Flow**:

1. Audio engineer connects Milan devices to network infrastructure
2. Devices automatically discover each other through Milan baseline procedures
3. Studio routing and monitoring are configured using Milan control capabilities
4. Recording session begins with professional audio quality and low latency
5. Multi-room monitoring and talkback are established using Milan streams

**Postconditions**: Professional recording studio is operational with multi-vendor interoperability

### UC-02: Live Concert Production

**Actors**: Live Sound Engineer, Professional Audio Systems, Production Network

**Description**: Large-scale live concert with distributed audio systems and real-time control

**Preconditions**: Milan-compliant live sound equipment and production network

**Flow**:

1. Live sound equipment is networked using Milan infrastructure
2. Front-of-house and monitor systems are connected via audio streams
3. Real-time mixing and control are performed using Milan control capabilities
4. Redundant systems provide fault tolerance during live performance
5. Post-show analysis uses Milan monitoring data for system optimization

**Postconditions**: Live concert is successfully produced with professional audio quality and reliability

### UC-03: Broadcast Facility Integration

**Actors**: Broadcast Engineer, Broadcast Equipment, Facility Infrastructure

**Description**: Broadcast facility upgrade with Milan-enabled professional audio infrastructure

**Preconditions**: Existing broadcast infrastructure with Milan integration capability

**Flow**:

1. Milan devices are integrated with existing broadcast infrastructure
2. AES67 compatibility provides bridge between Milan and broadcast systems
3. Professional audio routing and control are established using Milan capabilities
4. Live broadcast production utilizes Milan audio streams and control
5. System monitoring and management integrate with facility operations

**Postconditions**: Broadcast facility operates with enhanced professional audio capabilities and maintained compatibility

## User Stories

### US-01: Plug-and-Play Professional Audio

**As a** professional audio engineer  
**I want** plug-and-play interoperability between professional audio equipment  
**So that** I can focus on creative work instead of technical integration  

**Acceptance Criteria**:

- Equipment from different manufacturers automatically interoperates
- Audio streams are established without manual network configuration
- Professional audio quality is maintained across all equipment

### US-02: Real-Time Professional Control

**As a** live sound engineer  
**I want** real-time control of all audio parameters across the network  
**So that** I can make immediate adjustments during live performance  

**Acceptance Criteria**:

- Control changes are applied immediately without perceptible delay
- All professional audio parameters are accessible and controllable
- Control interface responds consistently across different equipment

### US-03: Professional Audio Monitoring

**As a** recording engineer  
**I want** comprehensive monitoring of audio quality and system status  
**So that** I can ensure optimal recording conditions and quickly identify issues  

**Acceptance Criteria**:

- Audio level monitoring meets professional metering standards
- System status information is comprehensive and accurate
- Issue identification and troubleshooting are supported by monitoring data

## Acceptance Criteria

### AC-01: Milan Specification Compliance

- [ ] Milan v1.2 baseline requirements fully implemented and validated
- [ ] All Milan MVU commands implemented with correct behavior
- [ ] Enhanced entity model supports complete professional audio device representation
- [ ] Milan certification testing passed with 100% success rate
- [ ] Interoperability demonstrated with multiple Milan device vendors

### AC-02: Professional Audio Quality

- [ ] Professional audio formats supported (44.1-192kHz, up to 32-bit)
- [ ] Audio quality metrics meet professional standards (THD+N < 0.001%)
- [ ] Latency requirements met for professional monitoring applications
- [ ] AES67 compatibility maintained for broadcast integration
- [ ] Multi-channel audio streaming validated up to professional requirements

### AC-03: Real-Time Performance

- [ ] Control response times meet professional requirements (<10ms)
- [ ] Audio streaming latency under 3ms achieved and validated
- [ ] Real-time monitoring provides accurate, timely information
- [ ] Fault detection and recovery meet professional availability requirements
- [ ] System stability validated through extended testing

### AC-04: Integration and Certification

- [ ] Milan certification achieved through official testing procedures
- [ ] Multi-vendor interoperability demonstrated and validated
- [ ] Professional audio equipment integration successful
- [ ] Broadcast facility integration validated with AES67 compatibility
- [ ] Quality assurance procedures validate consistent operation

## Requirements Traceability

### Stakeholder to System Requirements Mapping

| Stakeholder Requirement | System Requirements | Implementation Focus |
|------------------------|-------------------|---------------------|
| SR-01 Professional Audio System Integrators | SYS-01, SYS-03 | Interoperability, enhanced control |
| SR-02 Professional Audio Equipment Manufacturers | SYS-01, SYS-04 | Certification, compliance |
| SR-03 Live Sound Production Companies | SYS-02, SYS-05 | Audio quality, real-time performance |
| SR-04 Recording and Post-Production Facilities | SYS-02, SYS-05 | Audio quality, low latency |
| SR-05 Broadcast and Media Organizations | SYS-02, SYS-04 | Professional standards, compliance |

### Functional Requirements Implementation

| Functional Requirement | Implementation Components | Validation Method |
|----------------------|-------------------------|------------------|
| FR-PAAD-01 Device Identification | MilanPAADEntity class, device capability reporting | Milan device discovery testing |
| FR-AVDECC-01 MVU Commands | Milan MVU command handlers, response processing | MVU command testing suite |
| FR-INTEROP-01 Baseline Compliance | Milan baseline behavior implementation | Milan certification testing |
| FR-CONTROL-01 Audio Parameters | Professional audio control implementation | Audio parameter control testing |
| FR-CERT-01 Certification Support | Automated testing interface, compliance reporting | Milan certification procedures |

### Quality Requirements Validation

| Quality Requirement | Measurement Method | Validation Criteria |
|--------------------|-------------------|-------------------|
| Professional Audio Quality | Audio analysis testing, THD+N measurement | <0.001% THD+N |
| Real-Time Performance | Latency measurement, response time testing | <3ms audio latency |
| Interoperability | Multi-vendor testing, cross-device validation | 100% interoperability success |
| Certification Compliance | Milan certification testing procedures | 100% certification test success |
| System Reliability | Extended operation testing, fault injection | 99.99% availability |

---

## Document History

- Version 1.0 (October 9, 2025): Initial requirements specification based on Milan v1.2 implementation analysis

## References

- AVnu Alliance Milan Specification v1.2-2023: Professional Audio AVB Device Application Profile
- IEEE Std 1722.1-2021: IEEE Standard for Device Discovery, Connection Management, and Control Protocol for Time-Sensitive Networking Systems
- AES67-2018: AES standard for audio applications of networks - High-performance streaming audio-over-IP interoperability
- ISO/IEC/IEEE 29148:2018: Systems and software engineering - Life cycle processes - Requirements engineering
