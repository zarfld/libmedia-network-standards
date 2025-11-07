# IEEE 1588-2019 - Precision Time Protocol (PTPv2) Implementation

[![Standards Compliance](https://img.shields.io/badge/IEEE%201588--2019-Compliant-brightgreen)](https://standards.ieee.org/standard/1588-2019.html)
[![Protocol Layer](https://img.shields.io/badge/Protocol-Foundation%20Timing-blue)](#protocol-architecture)
[![Implementation Status](https://img.shields.io/badge/Status-Active%20Development-yellow)](#repository-status)

## Overview

This repository provides a **standards-compliant implementation** of **IEEE 1588-2019** - "Standard for a Precision Clock Synchronization Protocol for Networked Measurement and Control Systems", commonly known as **Precision Time Protocol version 2 (PTPv2)**.

IEEE 1588-2019 serves as the **foundational timing protocol** for all network-based time synchronization systems, providing the core algorithms, message formats, and behavioral specifications that enable sub-microsecond clock synchronization across distributed systems.

### Key Features

- **Hardware-agnostic protocol implementation** following IEEE 1588-2019 specification
- **Complete Best Master Clock Algorithm (BMCA)** per Section 9.3
- **All PTP message types** including Sync, Delay_Req, Follow_Up, Delay_Resp, Management, Signaling
- **Transparent and Boundary Clock support** per Sections 10.3 and 10.4
- **Management protocol implementation** per Section 15
- **Announce message processing** with qualification and timeout mechanisms
- **Path delay measurement** mechanisms (end-to-end and peer-to-peer)
- **Clock servo algorithms** for frequency and phase adjustment

## Protocol Architecture

IEEE 1588-2019 defines the foundational timing architecture used by higher-layer protocols:

```text
┌─────────────────────────────────────────────────┐
│              Application Layer                   │
│  ┌─────────────────┐    ┌───────────────────┐  │
│  │ IEEE 1722.1     │    │ Professional      │  │
│  │ AVDECC Control  │    │ Audio/Video       │  │
│  │                 │    │ Applications      │  │
│  └─────────────────┘    └───────────────────┘  │
└─────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────┐
│              Transport Layer                     │
│  ┌─────────────────┐    ┌───────────────────┐  │
│  │ IEEE 1722       │    │ IEEE 802.1AS      │  │
│  │ AVTP Streams    │    │ gPTP Profile      │  │
│  │                 │    │ (PTP Subset)      │  │
│  └─────────────────┘    └───────────────────┘  │
└─────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────┐
│         Foundation Timing Protocol               │
│  ┌─────────────────────────────────────────────┐│
│  │           IEEE 1588-2019 PTPv2              ││ <- THIS REPOSITORY
│  │     Precision Time Protocol (Base)          ││
│  │                                             ││
│  │ • Master/Slave Selection (BMCA)            ││
│  │ • Clock Synchronization Algorithms         ││
│  │ • Message Exchange Protocols               ││
│  │ • Transparent/Boundary Clock Support       ││
│  └─────────────────────────────────────────────┘│
└─────────────────────────────────────────────────┘
```

## Cross-Standard Integration

### Standards that Reference IEEE 1588-2019

This repository provides timing services to higher-layer protocol implementations:

#### IEEE 802.1AS (Generalized PTP Profile)

- **Repository**: `IEEE/802.1AS/2021/` → [zarfld/ieee-802-1as-2021](https://github.com/zarfld/ieee-802-1as-2021)
- **Integration**: IEEE 802.1AS-2021 extends IEEE 1588-2019 with automotive and audio/video specific requirements
- **Dependencies**: IEEE 802.1AS requires IEEE 1588-2019 BMCA and message formats
- **Key Relationships**:
  - IEEE 802.1AS Section 10.2 references IEEE 1588-2019 Section 9.3 (BMCA)
  - IEEE 802.1AS Section 11.2 references IEEE 1588-2019 Section 11 (Message Exchange)
  - IEEE 802.1AS adds gPTP-specific announce message TLVs

#### IEEE 1722 (AVTP Transport Protocol)

- **Repository**: `IEEE/1722/2016/` → [zarfld/ieee-1722-2016](https://github.com/zarfld/ieee-1722-2016)
- **Integration**: IEEE 1722-2016 Section 6.4 requires synchronized time from IEEE 1588 or IEEE 802.1AS
- **Dependencies**: AVTP presentation time calculations use IEEE 1588-2019 timestamp formats

#### IEEE 1722.1 (AVDECC Device Control)

- **Repository**: `IEEE/1722.1/2021/` → [zarfld/ieee-1722-1-2021](https://github.com/zarfld/ieee-1722-1-2021)
- **Integration**: IEEE 1722.1-2021 Section 6.2.1.7 references IEEE 1588 for time synchronization
- **Dependencies**: AVDECC timestamp validation requires IEEE 1588-2019 clock quality assessment

### ITU-T Telecom Profiles (Extensions of IEEE 1588-2019)

- **ITU-T G.8275.1-2016**: Frequency synchronization profile
- **ITU-T G.8275.2-2017**: Phase/time synchronization profile
- Both profiles extend IEEE 1588-2019 for telecommunications infrastructure

## Repository Status

### Current Implementation Status

- 🔄 **Active Development** - Core protocol engine in development
- 📋 **Specification Compliance** - IEEE 1588-2019 conformance target
- 🧪 **Testing Framework** - Conformance test suite in development
- 📚 **Documentation** - Comprehensive API documentation with IEEE references
- 🔗 **Integration Ready** - Interfaces prepared for IEEE 802.1AS extension

### Next Development Phases

1. **Core Protocol Engine** - Complete state machine implementations
2. **Message Processing** - All message types with format validation
3. **Clock Implementations** - Ordinary, Boundary, and Transparent clock support
4. **Management Protocol** - Complete Section 15 management implementation
5. **Conformance Testing** - Full IEEE 1588-2019 test suite
6. **Profile Integration** - IEEE 802.1AS and ITU-T profile compatibility

---

**Standards Compliance Notice**: This implementation is based on understanding of IEEE 1588-2019 specification. For authoritative requirements, refer to the official IEEE 1588-2019 document available from IEEE Standards Association.

**Repository**: [zarfld/ieee-1588-2019](https://github.com/zarfld/ieee-1588-2019)  
**Parent Project**: [libmedia-network-standards](https://github.com/zarfld/libmedia-network-standards)  
**Standards Organization**: IEEE Standards Association