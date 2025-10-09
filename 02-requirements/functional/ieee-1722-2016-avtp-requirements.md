# IEEE 1722-2016 AVTP Requirements Specification

**Standards Compliance**: ISO/IEC/IEEE 29148:2018  
**Standard Reference**: IEEE Std 1722-2016 "IEEE Standard for a Transport Protocol for Time-Sensitive Applications in Bridged Local Area Networks"  
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

This document specifies requirements for IEEE 1722-2016 Audio Video Transport Protocol (AVTP) implementation within the professional media networking ecosystem. AVTP provides the standardized transport layer for time-sensitive audio and video streaming over Ethernet networks.

### Scope

The requirements cover the complete IEEE 1722-2016 specification including:

- AVTP common transport framework and packet structure
- Audio transport formats (AAF - AVTP Audio Format)
- Video transport formats (CVF - Compressed Video Format, RVF - Raw Video Format)
- Control and data synchronization mechanisms
- Stream reservation and bandwidth management
- Integration with IEEE 802.1AS timing synchronization

### Context

IEEE 1722-2016 AVTP serves as the media transport layer in the professional media networking protocol stack:

```text
┌─────────────────┐
│ Milan Extensions │ ← Professional Audio Application Profiles
├─────────────────┤
│ IEEE 1722.1     │ ← AVDECC Device Control
│ AVDECC          │
├─────────────────┤
│ IEEE 1722 AVTP  │ ← Transport Protocol (THIS SPECIFICATION)
├─────────────────┤
│ IEEE 802.1AS    │ ← Timing Synchronization
│ gPTP            │
├─────────────────┤
│ IEEE 802.1Q     │ ← Bridging & QoS
│ Bridging/TSN    │
└─────────────────┘
```

## Stakeholder Requirements

### REQ-STK-IEEE1722-01: Professional Audio Content Creators

**Need**: High-quality, low-latency audio streaming for professional content creation

**Rationale**: Professional audio production requires uncompressed, multi-channel audio streaming with deterministic latency for real-time monitoring and recording

**Success Criteria**: Uncompressed audio streaming, latency under 5ms, support for industry-standard sample rates and bit depths

### REQ-STK-IEEE1722-02: Video Production Professionals

**Need**: Professional-grade video streaming for live production and post-production workflows

**Rationale**: Professional video production requires high-bandwidth, low-latency video streaming for live switching, multi-camera production, and real-time collaboration

**Success Criteria**: Support for professional video formats, frame-accurate delivery, minimal compression artifacts

### REQ-STK-IEEE1722-03: Live Event Production Companies

**Need**: Reliable, scalable media streaming for large-scale live events

**Rationale**: Live events require robust media transport that can handle multiple concurrent streams with guaranteed delivery and fault tolerance

**Success Criteria**: Multi-stream support, automatic failover capabilities, real-time monitoring and diagnostics

### REQ-STK-IEEE1722-04: Broadcast Infrastructure Providers

**Need**: Standards-compliant media transport for broadcast facility infrastructure

**Rationale**: Broadcast facilities require standardized media transport that integrates with existing infrastructure and supports industry workflows

**Success Criteria**: Standards compliance, legacy system integration, scalable infrastructure support

### REQ-STK-IEEE1722-05: Audio/Video Equipment Manufacturers

**Need**: Standardized media transport implementation for professional equipment

**Rationale**: Equipment manufacturers need consistent, interoperable transport protocols to ensure multi-vendor system compatibility

**Success Criteria**: Full IEEE 1722-2016 compliance, interoperability testing, certification capability

## System Requirements

### REQ-SYS-IEEE1722-01: Media Format Support

**Requirement**: The IEEE 1722-2016 implementation SHALL support comprehensive professional audio and video formats

**Derived From**: REQ-STK-IEEE1722-01, REQ-STK-IEEE1722-02

**Details**: AAF for professional audio, CVF for compressed video, RVF for raw video formats

### REQ-SYS-IEEE1722-02: Timing Synchronization Integration

**Requirement**: The implementation SHALL integrate with IEEE 802.1AS gPTP for media clock synchronization

**Derived From**: REQ-STK-IEEE1722-01, REQ-STK-IEEE1722-02, REQ-STK-IEEE1722-03

**Dependencies**: IEEE 802.1AS gPTP implementation, media clock domain support

### REQ-SYS-IEEE1722-03: Stream Quality of Service

**Requirement**: The implementation SHALL provide guaranteed Quality of Service for media streams

**Derived From**: REQ-STK-IEEE1722-03, REQ-STK-IEEE1722-04

**Features**: Stream reservation, traffic shaping, bandwidth management, priority queuing

### REQ-SYS-IEEE1722-04: Scalability and Performance

**Requirement**: The implementation SHALL support large-scale professional media installations

**Derived From**: REQ-STK-IEEE1722-03, REQ-STK-IEEE1722-04

**Criteria**: Multiple concurrent streams, high aggregate bandwidth, low-latency delivery

### REQ-SYS-IEEE1722-05: Standards Compliance and Interoperability

**Requirement**: The implementation SHALL achieve full IEEE 1722-2016 compliance and multi-vendor interoperability

**Derived From**: REQ-STK-IEEE1722-04, REQ-STK-IEEE1722-05

**Verification**: Compliance testing, interoperability validation, certification processes

## Functional Requirements

### AVTP Common Framework Requirements

#### FR-AVTP-01: AVTPDU Structure

**Requirement**: The implementation SHALL support the complete AVTPDU (AVTP Data Unit) structure and common header format

**Details**:

- 20-byte AVTP common header with all required fields
- Subtype identification for different media formats
- Stream ID management and validation
- Sequence numbering for stream continuity
- Timestamp fields for media synchronization

**Acceptance Criteria**: AVTPDU structure complies with specification, all header fields are processed correctly

#### FR-AVTP-02: Stream Management

**Requirement**: The implementation SHALL provide comprehensive stream management capabilities

**Details**:

- Stream ID generation and assignment
- Stream state management (active, inactive, suspended)
- Stream registration with network infrastructure
- Multi-stream coordination and scheduling
- Stream priority and QoS management

**Acceptance Criteria**: Streams are managed correctly, state transitions are handled properly, QoS is maintained

#### FR-AVTP-03: Timing and Synchronization

**Requirement**: The implementation SHALL support media clock synchronization through IEEE 802.1AS integration

**Details**:

- AVTP timestamp generation based on gPTP time
- Media clock recovery from AVTP streams
- Presentation time calculation and scheduling
- Clock domain identification and management
- Synchronization quality monitoring

**Acceptance Criteria**: Media streams maintain timing accuracy, presentation timing is precise, synchronization quality meets requirements

### Audio Transport Format (AAF) Requirements

#### FR-AAREQ-FUN-IEEE1722-01: Professional Audio Support

**Requirement**: The implementation SHALL support AAF for professional audio streaming

**Details**:

- Uncompressed PCM audio streaming
- Support for professional sample rates (44.1, 48, 88.2, 96, 176.4, 192 kHz)
- Multiple bit depths (16, 24, 32-bit)
- Multi-channel audio (up to 32 channels per stream)
- AES67 audio format compatibility

**Acceptance Criteria**: All professional audio formats are supported, audio quality is maintained, AES67 compatibility is verified

#### FR-AAREQ-FUN-IEEE1722-02: Audio Stream Formatting

**Requirement**: The implementation SHALL provide correct AAF packet formatting and processing

**Details**:

- AAF header structure implementation
- Sample packing and unpacking algorithms
- Channel mapping and routing capabilities
- Sample rate adaptation and conversion
- Audio format negotiation mechanisms

**Acceptance Criteria**: AAF packets are formatted correctly, audio samples are preserved accurately, format negotiation works properly

### Video Transport Format Requirements

#### FR-CVREQ-FUN-IEEE1722-01: Compressed Video Format

**Requirement**: The implementation SHALL support CVF for compressed video streaming

**Details**:

- H.264/AVC video format support
- H.265/HEVC video format support
- JPEG-XS low-latency compression support
- Video format parameter negotiation
- Frame boundary detection and handling

**Acceptance Criteria**: Compressed video formats are supported, video quality is maintained, frame timing is accurate

#### FR-CVREQ-FUN-IEEE1722-02: Video Stream Management

**Requirement**: The implementation SHALL provide video stream management capabilities

**Details**:

- Video frame packetization and reassembly
- Frame timing and presentation scheduling
- Video resolution and format adaptation
- Frame rate conversion and synchronization
- Video quality monitoring and control

**Acceptance Criteria**: Video frames are handled correctly, frame timing is maintained, quality monitoring provides accurate feedback

#### FR-RVREQ-FUN-IEEE1722-01: Raw Video Format

**Requirement**: The implementation SHALL support RVF for uncompressed video streaming

**Details**:

- Raw video pixel format support (RGB, YUV)
- High-resolution format support (4K, 8K)
- Progressive and interlaced video handling
- Color space conversion capabilities
- Raw video bandwidth optimization

**Acceptance Criteria**: Raw video formats are supported, high resolutions are handled efficiently, color accuracy is maintained

### Control and Signaling Requirements

#### FR-CTRL-01: Stream Control

**Requirement**: The implementation SHALL provide stream control mechanisms for media management

**Details**:

- Stream start, stop, pause, resume commands
- Stream parameter modification during operation
- Stream health monitoring and reporting
- Error detection and recovery mechanisms
- Stream statistics collection and reporting

**Acceptance Criteria**: Stream control operations work reliably, monitoring provides accurate information, recovery mechanisms function correctly

#### FR-CTRL-02: Network Integration

**Requirement**: The implementation SHALL integrate with network infrastructure for optimized delivery

**Details**:

- Stream Reservation Protocol (SRP) integration
- Traffic shaping and bandwidth management
- Priority queue utilization for media streams
- Network fault detection and adaptation
- Quality of Service monitoring and adjustment

**Acceptance Criteria**: Network integration provides guaranteed delivery, QoS is maintained, fault recovery is automatic

## Non-Functional Requirements

### REQ-NFR-IEEE1722-01: Latency Requirements

**Audio Latency**: End-to-end audio latency SHALL not exceed 5ms under normal network conditions

**Video Latency**: End-to-end video latency SHALL not exceed 16ms for professional production applications

**Jitter**: Media delivery jitter SHALL be minimized through buffering and timing control mechanisms

**Synchronization**: Audio-video synchronization SHALL be maintained within 40ms accuracy

### REQ-NFR-IEEE1722-02: Throughput and Scalability

**Audio Throughput**: The implementation SHALL support minimum 512 channels of professional audio at 48kHz/24-bit

**Video Throughput**: The implementation SHALL support minimum 10 concurrent 4K video streams

**Stream Count**: The implementation SHALL support minimum 100 concurrent media streams per device

**Aggregate Bandwidth**: The implementation SHALL efficiently utilize available network bandwidth up to 10Gbps

### REQ-NFR-IEEE1722-03: Reliability and Quality

**Packet Loss**: Media delivery SHALL maintain quality with packet loss rates up to 0.1%

**Error Recovery**: The implementation SHALL provide error concealment and recovery mechanisms

**Stream Continuity**: Media streams SHALL maintain continuity during network topology changes

**Quality Monitoring**: Real-time quality metrics SHALL be available for all active streams

### REQ-NFR-IEEE1722-04: Interoperability Requirements

**Standards Compliance**: The implementation SHALL pass 100% of IEEE 1722-2016 conformance tests

**Multi-vendor Support**: The implementation SHALL demonstrate interoperability with major professional equipment manufacturers

**Legacy Integration**: The implementation SHALL support integration with existing professional audio/video infrastructure

**Protocol Compatibility**: The implementation SHALL maintain compatibility with related IEEE 802.1 protocols

## Use Cases

### UC-IEEE1722-01: Professional Recording Studio

**Actors**: Recording Engineer, Audio Equipment, AVTP Infrastructure

**Description**: Multi-channel audio recording session with synchronized playback and monitoring

**Preconditions**: Professional audio equipment connected via AVTP-enabled network

**Flow**:

1. Recording engineer configures multi-channel audio streams via AVDECC
2. AVTP streams are established between audio interfaces and recording systems
3. Real-time audio monitoring streams are distributed to multiple locations
4. Recording session proceeds with synchronized capture and playback
5. Stream quality is monitored throughout the session

**Postconditions**: High-quality multi-channel recording is completed with maintained synchronization

### UC-IEEE1722-02: Live Broadcast Production

**Actors**: Broadcast Engineer, Video/Audio Equipment, Production Infrastructure

**Description**: Live broadcast with multiple camera feeds and audio sources

**Preconditions**: Broadcast facility with AVTP-enabled production infrastructure

**Flow**:

1. Multiple camera feeds are configured as AVTP video streams
2. Audio feeds from various sources are established as AVTP audio streams
3. Live production begins with real-time switching between sources
4. Stream priorities are managed to ensure critical feeds are maintained
5. Backup streams are automatically activated if primary streams fail

**Postconditions**: Live broadcast is delivered successfully with maintained quality and synchronization

### UC-IEEE1722-03: Large Conference Facility

**Actors**: AV Technician, Conference Equipment, Facility Infrastructure

**Description**: Large conference with distributed audio/video throughout facility

**Preconditions**: Conference facility with comprehensive AV infrastructure

**Flow**:

1. Conference audio is distributed via AVTP to multiple zones
2. Presentation video is streamed to displays throughout facility
3. Recording and streaming systems capture conference content
4. Interactive features like audience response systems are synchronized
5. Content is delivered to remote participants via streaming systems

**Postconditions**: Conference content is successfully distributed with consistent quality across all endpoints

## User Stories

### IEEE1722-US-01: High-Quality Audio Streaming

**As a** professional audio engineer  
**I want** uncompressed, multi-channel audio streaming over the network  
**So that** I can maintain professional audio quality in distributed production environments  

**Acceptance Criteria**:

- Audio streams maintain full resolution and dynamic range
- Multi-channel configurations are supported (stereo, 5.1, 7.1, etc.)
- Latency is imperceptible for real-time monitoring applications

### IEEE1722-US-02: Reliable Video Production

**As a** video production professional  
**I want** reliable, low-latency video streaming for live production  
**So that** I can deliver high-quality content with precise timing control  

**Acceptance Criteria**:

- Video streams maintain frame accuracy and color fidelity
- Multiple video formats are supported for different production needs
- Stream switching occurs without visible artifacts or interruptions

### IEEE1722-US-03: Scalable Media Distribution

**As a** system integrator  
**I want** scalable media distribution for large installations  
**So that** I can efficiently deliver content to multiple endpoints  

**Acceptance Criteria**:

- Multiple concurrent streams are supported without quality degradation
- Bandwidth is efficiently managed across available network capacity
- System scales to support additional endpoints without reconfiguration

## Acceptance Criteria

### AC-01: IEEE 1722-2016 Compliance

- [ ] Complete AVTPDU structure implementation and validation
- [ ] AAF audio format support for all professional requirements
- [ ] CVF compressed video format implementation
- [ ] RVF raw video format support for professional applications
- [ ] All timing and synchronization mechanisms functional
- [ ] Protocol compliance testing passed with 100% success rate

### AC-02: Media Quality and Performance

- [ ] Audio latency under 5ms achieved in testing
- [ ] Video latency under 16ms achieved for professional formats
- [ ] Multi-channel audio streaming validated up to 32 channels
- [ ] 4K video streaming demonstrated with maintained quality
- [ ] Stream synchronization accuracy meets professional requirements

### AC-03: Integration and Interoperability

- [ ] IEEE 802.1AS gPTP integration functional and tested
- [ ] Stream Reservation Protocol (SRP) integration working
- [ ] Multi-vendor interoperability demonstrated
- [ ] AES67 audio compatibility verified
- [ ] Professional equipment integration validated

### AC-04: Scalability and Reliability

- [ ] Concurrent stream support validated at specification limits
- [ ] Network bandwidth utilization optimized and tested
- [ ] Error recovery mechanisms functional under fault conditions
- [ ] Long-term reliability testing completed successfully
- [ ] Quality monitoring provides accurate real-time metrics

## Requirements Traceability

### Stakeholder to System Requirements Mapping

| Stakeholder Requirement | System Requirements | Implementation Priority |
|------------------------|-------------------|------------------------|
| REQ-STK-IEEE1722-01 Professional Audio Creators | REQ-SYS-IEEE1722-01, REQ-SYS-IEEE1722-02 | High - Core audio functionality |
| REQ-STK-IEEE1722-02 Video Production Professionals | REQ-SYS-IEEE1722-01, REQ-SYS-IEEE1722-04 | High - Core video functionality |
| REQ-STK-IEEE1722-03 Live Event Production | REQ-SYS-IEEE1722-03, REQ-SYS-IEEE1722-04 | Medium - Scalability features |
| REQ-STK-IEEE1722-04 Broadcast Infrastructure | REQ-SYS-IEEE1722-03, REQ-SYS-IEEE1722-05 | Medium - Standards compliance |
| REQ-STK-IEEE1722-05 Equipment Manufacturers | REQ-SYS-IEEE1722-05 | High - Interoperability |

### Functional Requirements Implementation

| Functional Requirement | Implementation Components | Validation Method |
|----------------------|-------------------------|------------------|
| FR-AVTP-01 AVTPDU Structure | Common header processing, packet handling | Protocol conformance testing |
| FR-AAREQ-FUN-IEEE1722-01 Professional Audio | AAF format implementation, sample processing | Audio quality testing |
| FR-CVREQ-FUN-IEEE1722-01 Compressed Video | CVF format support, video processing | Video quality validation |
| FR-CTRL-01 Stream Control | Control message handling, state management | Functional testing |
| FR-CTRL-02 Network Integration | SRP integration, QoS management | Network integration testing |

### Performance Requirements Validation

| Performance Requirement | Measurement Method | Acceptance Threshold |
|-----------------------|-------------------|---------------------|
| Audio Latency | End-to-end timing measurement | <5ms |
| Video Latency | Frame timing analysis | <16ms |
| Stream Count | Concurrent stream loading | >100 streams |
| Throughput | Bandwidth utilization testing | >512 audio channels |
| Reliability | Extended operation testing | >99.9% uptime |

---

## Document History

- Version 1.0 (October 9, 2025): Initial requirements specification based on IEEE 1722-2016 implementation analysis

## References

- IEEE Std 1722-2016: IEEE Standard for a Transport Protocol for Time-Sensitive Applications in Bridged Local Area Networks
- IEEE Std 1722.1-2021: IEEE Standard for Device Discovery, Connection Management, and Control Protocol for Time-Sensitive Networking Systems
- ISO/IEC/IEEE 29148:2018: Systems and software engineering - Life cycle processes - Requirements engineering
- AES67-2018: AES standard for audio applications of networks - High-performance streaming audio-over-IP interoperability



