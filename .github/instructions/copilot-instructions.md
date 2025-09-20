---
applyTo: '**'
---

# Standards Layer AI Coding Instructions

## CRITICAL: Hardware-Agnostic Protocol Implementation

The Standards layer (`lib/Standards/`) is the **pure protocol implementation layer** that MUST remain completely hardware and vendor agnostic.

## Working Principles for Standards Layer

- **Understand protocols before implementing** - study IEEE specifications and state machines thoroughly
- **No Fake, No Stubs, no Simulations** - implement complete protocol logic, not simplified placeholders
- **No implementation-based assumptions** - use IEEE specification or analysis results only (ask if required)
- **No false advertising** - prove and ensure protocol compliance and correctness
- **Never break protocol APIs** in ways that violate IEEE standards - prefer fixing implementation instead
- **Prevent dead code or orphan files** - fix code rather than creating new versions, ensure ALL code compiles
- **Always reference ### Documentation Requirements with Copyright Compliance

### Function Documentation with IEEE Context
```cpp
/**
 * @brief Parse IEEE 1722.1 AVDECC Entity descriptor from network packet
 * 
 * Validates and extracts entity descriptor according to IEEE 1722.1-2021
 * specification section 7.2.1. Performs integrity checks including AEM
 * checksum validation per specification requirements.
 *
 * @param packet_data Raw ethernet packet data
 * @param packet_length Length of packet data in bytes
 * @param entity_desc Output buffer for parsed descriptor
 * @return 0 on success, negative error code on failure
 * 
 * @note Implements IEEE 1722.1-2021 Table 7.1 entity descriptor format
 * @see IEEE 1722.1-2021, Section 7.2.1 "ENTITY descriptor"
 * @see IEEE 1722.1-2021, Section 7.3.5 "AEM checksum calculation"
 * 
 * IMPORTANT: This implementation is based on understanding of IEEE 1722.1-2021
 * specification. No copyrighted content from IEEE documents is reproduced.
 * Refer to original IEEE specification for authoritative requirements.
 */
int avdecc_parse_entity_descriptor(const uint8_t* packet_data, 
                                   size_t packet_length,
                                   avdecc_entity_descriptor_t* entity_desc);
```

### Copyright-Compliant Implementation Comments
```cpp
// ✅ CORRECT - Reference without reproduction
// Implements entity descriptor parsing per IEEE 1722.1-2021 Section 7.2.1
#define ENTITY_DESCRIPTOR_TYPE 0x0000  // As specified in IEEE 1722.1-2021

// ❌ WRONG - Reproducing copyrighted content
/*
// DON'T DO THIS - This reproduces copyrighted IEEE content:
// "The entity_descriptor field shall contain the following sub-fields:
//  descriptor_type (16 bits): shall be set to ENTITY (0x0000)
//  descriptor_index (16 bits): shall be set to 0x0000..."
// This is direct reproduction of IEEE copyrighted text!
*/

// ✅ CORRECT - Original implementation based on specification understanding
struct entity_descriptor {
    uint16_t descriptor_type;    // IEEE 1722.1-2021 Table 7.1 
    uint16_t descriptor_index;   // IEEE 1722.1-2021 Table 7.1
    uint64_t entity_id;         // IEEE 1722.1-2021 Table 7.1
    uint64_t entity_model_id;   // IEEE 1722.1-2021 Table 7.1
    // ... implementation based on specification understanding
};
```

### Standards Compliance Notes with Copyright Respect
- **Always reference specific IEEE section and table numbers** when implementing protocol features
- **NEVER reproduce copyrighted text** from specifications in comments or documentation
- **Document Milan extensions separately** with AVnu specification references (respecting AVnu copyright)
- **Include timing requirements** with original implementation based on specification understanding
- **Specify endianness handling** based on IEEE network byte order requirements
- **Use only factual technical information** that is not subject to copyright (constants, field sizes, etc.)ification sections** when implementing protocol features
- **Validate all protocol fields** with range checks or masks from the IEEE specification
- **Every function must have documentation** explaining purpose, parameters, return values, and IEEE context
- **No duplicate or redundant implementations** to avoid inconsistencies and confusion
- **Prevent downgrades** - fix rather than delete existing protocol functionality
- **No ad-hoc file copies** (e.g., *_fixed, *_new, *_correct) - refactor in place step-by-step

## Core Principle: Standards-Only Implementation

- **ONLY IEEE/AVnu/AES protocol logic** - no hardware-specific code
- **Hardware agnostic** - compilable without any vendor drivers or hardware headers
- **Mockable/testable** - runnable without physical hardware present
- **Dependency injection pattern** - receives hardware abstraction via interfaces/function pointers

### FORBIDDEN in Standards Layer
```cpp
// ❌ NEVER include vendor-specific headers
#include "intel_ethernet_hal.h"           // NO - Intel specific
#include "../../intel_avb/include/*.h"    // NO - Intel specific  
#include "../../common/hal/network_hal.h" // NO - HAL abstraction
#include <linux/if_packet.h>              // NO - OS specific
#include <winsock2.h>                     // NO - OS specific
```

### REQUIRED in Standards Layer
```cpp
// ✅ Only standards and protocol headers
#include "jdksavdecc.h"                   // IEEE 1722.1 protocol
#include "ieee1722_avtp.h"                // IEEE 1722 AVTP
#include "ieee8021as_gptp.h"              // IEEE 802.1AS gPTP
#include "avnu_milan.h"                   // AVnu Milan extensions
```

## Architecture Compliance

### Standards Layer Responsibility
- **Protocol state machines** (IEEE 1722.1 AVDECC, IEEE 1722 AVTP, IEEE 802.1AS gPTP)
- **Packet format handling** (parsing, validation, construction)
- **Protocol calculations** (timing, addressing, checksums)
- **Standard-defined structures** and constants
- **Protocol compliance validation**

### Interface Pattern for Hardware Access
```cpp
// ✅ CORRECT: Dependency injection interface
typedef struct {
    int (*send_packet)(const void* packet, size_t length);
    int (*receive_packet)(void* buffer, size_t* length);
    uint64_t (*get_time_ns)(void);
    int (*set_timer)(uint32_t interval_us, timer_callback_t callback);
} network_interface_t;

// Protocol implementation receives interface
int avdecc_entity_init(const network_interface_t* net_interface);
```

### Hardware Bridge (Service Layer Responsibility)
```cpp
// This code belongs in SERVICE layer (e.g., gPTP daemon), NOT Standards
static int intel_send_packet(const void* packet, size_t length) {
    return intel_hal_send_packet(packet, length);  // Intel-specific call
}

static network_interface_t intel_interface = {
    .send_packet = intel_send_packet,
    .receive_packet = intel_receive_packet,
    // ... other Intel-specific implementations
};

// Service layer bridges Standards to Intel Hardware
avdecc_entity_init(&intel_interface);
```

## IEEE Standards Reference

### CRITICAL: Use Authoritative Standards Documents
When implementing protocols, reference these authoritative documents via MCP-Server "markitdown_standards":

#### IEEE Protocol Standards:
- `IEEE 1588-2019-en.pdf` - Precision Time Protocol (PTPv2) - Foundation timing protocol
- `ISO-IEC-IEEE 8802-1AS-2021-en.pdf` - Generalized Precision Time Protocol (gPTP) - CURRENT
- `ISO-IEC-IEEE 8802-1Q-2020-en.pdf` - VLAN and QoS Standards - Network layer foundation
- `ISO-IEC-IEEE 8802-1BA-2016-en.pdf` - Audio Video Bridging (AVB) Profiles
- `IEEE 1722-2016-en.pdf` - Audio Video Transport Protocol (AVTP) - CURRENT
- `IEEE 1722.1-2021-en.pdf` - AVDECC Device Control (Latest) - CURRENT
- `IEEE 1722.1-2013-en.pdf` - AVDECC Device Control (Legacy)
- `IEEE-802.17-2011-en.pdf` - Resilient Packet Ring (RPR)

#### AES Audio Engineering Standards:
- `AES 67-2018-en.pdf` - AES67-2018 Audio-over-IP interoperability - CURRENT
- `AES-70-1-2018-en.pdf` - AES70-1-2018 OCA Framework - CURRENT
- `AES 70-2-2018-en.pdf` - AES70-2-2018 OCA Class Structure - CURRENT  
- `AES 70-3-2018-en.pdf` - AES70-3-2018 OCA Protocol and Data Types - CURRENT
- `AES 70-3-2015-en.pdf` - AES70-3-2015 OCA Protocol (Legacy)
- `AES 3-1-2009 (R2014)-en.pdf` - AES3-1-2009 Digital Audio Interface Part 1
- `AES 3-2-2009 (R2014)-en.pdf` - AES3-2-2009 Digital Audio Interface Part 2
- `AES 3-3-2009 (R2014)-en.pdf` - AES3-3-2009 Digital Audio Interface Part 3
- `AES 3-4-2009 (R2014)-en.pdf` - AES3-4-2009 Digital Audio Interface Part 4
- `AES 5-2018-en.pdf` - AES5-2018 Preferred Sampling Frequencies
- `AES 11-2009 (R2014)-en.pdf` - AES11-2009 Digital Audio Reference Signals
- `aes60id-2020-i.pdf` - AES60id-2020 Object-based Audio Metadata

#### AVnu Alliance Milan Standards:
- `Milan_Specification_Consolidated_v1.2_Final_Approved-20231130.pdf` - Milan v1.2 (CURRENT)
- `Milan-Baseline-Interoperability-Specification-2.0a.pdf` - Milan Baseline Interop 2.0a

#### AVnu Alliance Interoperability Specifications:
- `Avnu_Base-and-ProAV_Functional_Interop_Specification_1.1.pdf` - Base and ProAV Functional Interop 1.1
- `Automotive-Ethernet-AVB-Func-Interop-Spec-v1.5-Public.pdf` - Automotive Ethernet AVB v1.5
- `Auto-Ethernet-AVB-Func-Interop-Spec_v1.6.pdf` - Automotive Ethernet AVB v1.6

**CRITICAL COPYRIGHT AND LICENSING RESTRICTIONS**:

⚠️ **ABSOLUTELY FORBIDDEN**:
- **DO NOT copy any text, tables, figures, or content** directly from IEEE, AES, or AVnu documents
- **DO NOT reproduce specification text** verbatim in source code comments
- **DO NOT include copyrighted tables or figures** in repository documentation
- **DO NOT paste specification sections** into code or documentation files
- **DO NOT create derivative works** that reproduce substantial portions of standards

✅ **PERMITTED USAGE**:
- **Reference document sections** by number only (e.g., "See IEEE 1722.1-2021, Section 7.2.1")
- **Implement protocol logic** based on understanding of specifications
- **Create original code** that achieves compliance with standard requirements
- **Use specification constants and values** in implementation (packet types, field sizes, etc.)
- **Reference via MCP-Server** for compliance verification during development only

**LICENSE NOTICE**: These documents are referenced for compliance verification and implementation guidance only. All content remains under copyright of respective standards organizations (IEEE, AES, AVnu Alliance). Any reproduction, distribution, or derivative works require proper licensing from copyright holders.

**Usage Pattern**: When implementing standards-related code, reference these documents via MCP-Server to ensure:
- Protocol message format compliance
- State machine behavior correctness  
- Timing requirement adherence
- Milan professional interoperability
- IEEE specification conformance
- AES audio format compliance
- AVnu interoperability requirements

### Protocol Compliance Requirements
```cpp
// ✅ Use authoritative constants from jdksavdecc
#include "jdksavdecc_aem_command.h"
uint16_t command_type = JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR;

// ❌ NEVER use hardcoded values
uint16_t command_type = 0x0004;  // NO - not standards compliant
```

## Testing Approach

### Unit Testing (Hardware-Independent)
```cpp
// ✅ Mock network interface for testing
static int mock_send_packet(const void* packet, size_t length) {
    // Record packet for verification
    return 0;
}

static network_interface_t mock_interface = {
    .send_packet = mock_send_packet,
    // ... other mock implementations
};

// Test protocol logic without hardware
void test_avdecc_entity_descriptor_response(void) {
    avdecc_entity_init(&mock_interface);
    // ... test protocol behavior
}
```

### Integration Testing (Service Layer Responsibility)
Integration with real hardware happens in the Service Layer, NOT in Standards.

## Common Violations to Avoid

### ❌ Direct Hardware Calls
```cpp
// WRONG - Standards calling Intel HAL directly
intel_hal_result_t result = intel_hal_send_packet(packet, length);
```

### ❌ OS-Specific Code
```cpp
// WRONG - OS-specific networking in Standards
#ifdef _WIN32
    SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
#else
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
#endif
```

### ❌ Hardware Assumptions
```cpp
// WRONG - Assuming Intel hardware capabilities
if (intel_hal_supports_ptp()) {  // Hardware-specific assumption
    enable_hardware_timestamping();
}
```

### ✅ Correct Abstraction
```cpp
// CORRECT - Protocol logic with abstracted capabilities
if (net_interface->capabilities & NETWORK_CAP_HARDWARE_TIMESTAMP) {
    enable_precision_timing();  // Protocol behavior, not hardware call
}
```

## Protocol Implementation Guidelines

### IEEE 1722.1 AVDECC
- Use jdksavdecc-c library for all packet structures
- Implement AEM command/response state machines according to IEEE 1722.1-2021
- Handle Milan extensions (GET_DYNAMIC_INFO command 0x004b, etc.)
- Calculate proper AEM checksums using CRC32 over descriptor excluding checksum field
- Support available_index increment correctly (only on actual state change)
- Include all mandatory descriptors (ENTITY, CONFIGURATION, STREAM_INPUT/OUTPUT, AVB_INTERFACE)

### IEEE 1722 AVTP
- Support all defined stream formats (AAF, CRF, TSCF) per IEEE 1722-2016
- Implement proper timestamp handling with presentation time validation
- Handle stream reservation coordination with accurate timing requirements
- Support Milan redundancy requirements (primary/secondary stream pairs)
- Validate packet headers against IEEE specification table formats

### IEEE 802.1AS gPTP
- Implement PTP state machines (Master/Slave selection) per IEEE 802.1AS
- Handle sync/follow_up/pdelay message processing with nanosecond precision
- Calculate path delay and offset corrections according to specification algorithms
- Support grandmaster clock selection algorithm with proper priority comparisons
- Maintain ±80ns synchronization accuracy for Milan compliance

## Clean Submit Rules for Standards

- **Each commit compiles and passes IEEE compliance checks**
- **Small, single-purpose, reviewable diffs** (no WIP noise)
- **No dead or commented-out code** - remove unused protocol implementations
- **Run formatter and static analysis** before commit
- **Update documentation and reference IEEE spec sections** in commit messages
- **Use feature flags for incremental protocol changes** when they risk breaking IEEE compliance
- **Reference exact IEEE section numbers** (e.g., "Implements IEEE 1722.1-2021 Section 7.2.1")

## Documentation Requirements

### Function Documentation with IEEE Context
```cpp
/**
 * @brief Parse IEEE 1722.1 AVDECC Entity descriptor from network packet
 * 
 * Validates and extracts entity descriptor according to IEEE 1722.1-2021
 * specification section 7.2.1. Performs integrity checks including AEM
 * checksum validation per specification requirements.
 *
 * @param packet_data Raw ethernet packet data
 * @param packet_length Length of packet data in bytes
 * @param entity_desc Output buffer for parsed descriptor
 * @return 0 on success, negative error code on failure
 * 
 * @note Implements IEEE 1722.1-2021 Table 7.1 entity descriptor format
 * @see IEEE 1722.1-2021, Section 7.2.1 "ENTITY descriptor"
 * @see IEEE 1722.1-2021, Section 7.3.5 "AEM checksum calculation"
 */
int avdecc_parse_entity_descriptor(const uint8_t* packet_data, 
                                   size_t packet_length,
                                   avdecc_entity_descriptor_t* entity_desc);
```

### Standards Compliance Notes
- **Always reference specific IEEE section and table numbers** when implementing protocol features
- **Document Milan extensions separately** with AVnu specification references
- **Include timing requirements** with nanosecond precision for real-time protocols
- **Specify endianness handling** for network byte order conversions

## Build System Integration

### CMake Configuration for Standards
The Standards layer should compile independently:
```cmake
# Standards layer has NO hardware dependencies
add_library(openavnu_standards STATIC
    src/ieee1722_avtp.c
    src/ieee1722_1_avdecc.c  
    src/ieee8021as_gptp.c
    src/avnu_milan.c
)

# Only protocol headers, no hardware/HAL
target_include_directories(openavnu_standards PUBLIC
    include/
    ../thirdparty/jdksavdecc-c/include/
)

# NO hardware libraries linked to Standards
# target_link_libraries(openavnu_standards intel_ethernet_hal)  # ❌ WRONG
```

### Testing Framework Integration
Use unified testing framework for protocol validation:
```cmake
# Protocol compliance tests (hardware-independent)
add_executable(standards_protocol_tests
    tests/test_ieee1722_compliance.c
    tests/test_avdecc_state_machines.c
    tests/test_milan_extensions.c
)

target_link_libraries(standards_protocol_tests 
    openavnu_standards 
    CppUTest
)
```

## Performance and Correctness

### Memory Management
- **Use static allocation** where possible for real-time protocol processing
- **Validate buffer bounds** against IEEE maximum packet sizes
- **Initialize all protocol structures** to prevent undefined behavior

### Timing Considerations
- **Nanosecond precision** for gPTP timing calculations
- **Microsecond accuracy** for AVTP presentation time
- **Avoid blocking operations** in protocol state machines

### Error Handling
- **Return IEEE-compliant error codes** where specifications define them
- **Log protocol violations** with specific IEEE section references
- **Graceful degradation** for optional protocol features

## Mandatory Namespace and Folder Structure

### Generic Structure Definition

**FUNDAMENTAL PATTERN**: All Standards implementations must follow this generic hierarchy:

```
<Organization>/<Standard>/<Subpart>/<Version>/
```

**Components Definition:**
- **Organization**: Standards body (IEEE, AVnu, AES, ITU, IETF, etc.)
- **Standard**: Standard family or main specification number  
- **Subpart**: Sub-specifications, working groups, or functional areas
- **Version**: Year of publication or version number

**Translation Rules:**
- **Folder Structure**: Preserve dots and organization naming (`IEEE/802.1/AS/2021/`)
- **Namespace Structure**: Convert dots to underscores, preserve organization (`IEEE::_802_1::AS::_2021`)
- **File Structure**: Follow folder pattern with appropriate extensions

### Concrete Examples by Organization

#### IEEE Standards Structure
```
IEEE/<Standard>/<Subpart>/<Year>/
├── IEEE/802.1/AS/2021/          # IEEE 802.1AS-2021 (gPTP)
├── IEEE/802.1/AS/2020/          # IEEE 802.1AS-2020 (previous version)
├── IEEE/802.1/Q/2018/           # IEEE 802.1Q-2018 (VLAN/QoS)
├── IEEE/1722/2016/              # IEEE 1722-2016 (AVTP)
├── IEEE/1722/2011/              # IEEE 1722-2011 (legacy AVTP)
├── IEEE/1722.1/2021/            # IEEE 1722.1-2021 (AVDECC)
├── IEEE/1722.1/2013/            # IEEE 1722.1-2013 (legacy AVDECC)
└── IEEE/1588/2019/              # IEEE 1588-2019 (PTPv2)
```

#### AVnu Alliance Structure  
```
AVnu/<Standard>/<Subpart>/<Version>/
├── AVnu/Milan/v1.2/             # Milan v1.2 professional audio
├── AVnu/Milan/v1.1/             # Milan v1.1 (legacy)
├── AVnu/Profiles/AVB/2.0/       # AVB interoperability profiles
└── AVnu/CertificationSuite/3.0/ # Certification test suites
```

#### Audio Engineering Society Structure
```
AES/<Standard>/<Subpart>/<Version>/
├── AES/AES67/2018/              # AES67-2018 audio-over-IP
├── AES/AES70/2021/              # AES70-2021 device control (OCA)
└── AES/AES3/2009/               # AES3-2009 digital audio interface
```

#### ITU-T Structure (if needed)
```
ITU/<Series>/<Standard>/<Version>/
├── ITU/G/G.8275.1/2016/         # ITU-T G.8275.1-2016 telecom PTP
└── ITU/G/G.8275.2/2017/         # ITU-T G.8275.2-2017 telecom PTP
```

### Required Directory Structure
```
lib/Standards/
├── IEEE/
│   ├── 802.1/                   # IEEE 802.1 family
│   │   └── AS/                  # 802.1AS - Generalized Precision Time Protocol
│   │       ├── 2021/           # IEEE 802.1AS-2021 specification (CURRENT)
│   │       │   ├── core/       # Core gPTP state machines
│   │       │   ├── messages/   # PTP message formats
│   │       │   ├── clock/      # Clock selection algorithms
│   │       │   └── conformity/ # Conformance testing
│   │       ├── 2020/           # IEEE 802.1AS-2020 
│   │       └── 2011/           # Legacy IEEE 802.1AS-2011
│   ├── 1722/                   # IEEE 1722 - Audio Video Transport Protocol
│   │   ├── 2016/               # IEEE 1722-2016 specification (CURRENT)
│   │   │   ├── avtp/           # Core AVTP protocol
│   │   │   ├── formats/        # Stream data formats (AAF, CRF, etc.)
│   │   │   ├── crf/            # Clock Reference Format
│   │   │   └── conformity/     # AVTP conformance testing
│   │   └── 2011/               # Legacy IEEE 1722-2011
│   └── 1722.1/                 # IEEE 1722.1 - Audio Video Device Control
│       ├── 2021/               # IEEE 1722.1-2021 specification (CURRENT)
│       │   ├── aem/            # AVDECC Entity Model
│       │   ├── aecp/           # AVDECC Entity Control Protocol
│       │   ├── acmp/           # AVDECC Connection Management Protocol
│       │   ├── adp/            # AVDECC Discovery Protocol
│       │   ├── descriptors/    # All descriptor types
│       │   └── conformity/     # AVDECC conformance testing
│       ├── 2013/               # IEEE 1722.1-2013 (legacy support)
│       └── 2011/               # IEEE 1722.1-2011 (legacy support)
├── AVnu/                       # AVnu Alliance specifications
│   └── Milan/                  # Milan professional audio
│       ├── v1.2/              # Milan v1.2 (CURRENT)
│       │   ├── discovery/      # Milan discovery extensions
│       │   ├── connection/     # Milan connection management
│       │   ├── control/        # Milan control extensions
│       │   └── conformity/     # Milan conformance testing
│       └── v1.1/              # Milan v1.1 (legacy)
├── AES/                       # Audio Engineering Society
│   ├── AES67/                 # AES67 audio-over-IP
│   └── AES70/                 # AES70 device control (OCA)
├── Common/                    # Cross-standard utilities
│   ├── utils/                 # Shared protocol utilities
│   ├── testing/               # Common testing frameworks
│   └── interfaces/            # Hardware abstraction interfaces
└── Documentation/             # Standards documentation
    ├── conformance/           # Conformance test specifications
    ├── interop/              # Interoperability guidelines
    └── examples/             # Protocol usage examples
```

### Required C++ Namespace Structure Following Generic Pattern

**NAMESPACE TRANSLATION RULE**: `<Organization>::<Standard_with_underscores>::<Subpart>::<Version_with_underscores>`

**Examples of Translation:**
- Folder: `IEEE/802.1/AS/2021/` → Namespace: `IEEE::_802_1::AS::_2021`
- Folder: `IEEE/1722.1/2021/` → Namespace: `IEEE::_1722_1::_2021`  
- Folder: `AVnu/Milan/v1.2/` → Namespace: `AVnu::Milan::v1_2`
- Folder: `AES/AES67/2018/` → Namespace: `AES::AES67::_2018`

```cpp
// ✅ CORRECT namespace hierarchy following generic pattern
namespace IEEE {
    namespace _802_1 {           // Standard: 802.1 (dots→underscores)
        namespace AS {           // Subpart: AS (Audio/Video bridging timing)
            namespace _2021 {    // Version: 2021 (year)
                // IEEE 802.1AS-2021 gPTP implementation
                namespace core {
                    class GPTPStateMachine;
                    class PortSyncSyncSend;
                }
                namespace messages {
                    struct SyncMessage;
                    struct FollowUpMessage;
                }
                namespace clock {
                    class BestMasterClockAlgorithm;
                }
                namespace Testing {
                    class ConformityTestFramework;
                }
            }
            namespace _2020 {    // Version: 2020 (previous year)
                // IEEE 802.1AS-2020 implementation
            }
            namespace _2011 {    // Version: 2011 (legacy year)
                // IEEE 802.1AS-2011 legacy implementation
            }
        }
        namespace Q {            // Subpart: Q (VLAN and QoS)
            namespace _2018 {    // Version: 2018
                // IEEE 802.1Q-2018 implementation
            }
        }
    }
    
    namespace _1722 {            // Standard: 1722 (AVTP)
        namespace _2016 {        // Version: 2016 (year)
            // IEEE 1722-2016 AVTP implementation
            namespace avtp {
                class AVTPPacket;
                class StreamDataHeader;
            }
            namespace formats {
                namespace aaf {
                    class AudioFormat;
                }
                namespace crf {
                    class ClockReferenceFormat;
                }
            }
        }
        namespace _2011 {        // Version: 2011 (legacy year)
            // IEEE 1722-2011 legacy implementation
        }
    }
    
    namespace _1722_1 {          // Standard: 1722.1 (dots→underscores)
        namespace _2021 {        // Version: 2021 (year)
            // IEEE 1722.1-2021 AVDECC implementation
            namespace aem {
                class EntityModel;
                class ConfigurationDescriptor;
            }
            namespace aecp {
                class EntityControlProtocol;
                class AEMCommand;
            }
            namespace acmp {
                class ConnectionManagementProtocol;
                class ConnectTXCommand;
            }
            namespace adp {
                class DiscoveryProtocol;
                class EntityAvailable;
            }
            namespace descriptors {
                class EntityDescriptor;
                class StreamInputDescriptor;
                class StreamOutputDescriptor;
            }
        }
        namespace _2013 {        // Version: 2013 (legacy year)
            // IEEE 1722.1-2013 legacy implementation
        }
        namespace _2011 {        // Version: 2011 (legacy year)
            // IEEE 1722.1-2011 legacy implementation
        }
    }
    
    namespace _1588 {            // Standard: 1588 (PTPv2)
        namespace _2019 {        // Version: 2019 (year)
            // IEEE 1588-2019 PTPv2 implementation
        }
    }
} // namespace IEEE

namespace AVnu {                 // Organization: AVnu Alliance
    namespace Milan {            // Standard: Milan professional audio
        namespace v1_2 {         // Version: v1.2 (version number)
            // Milan v1.2 extensions
            namespace discovery {
                class MilanDiscoveryExtensions;
            }
            namespace connection {
                class RedundantStreamPairs;
            }
        }
        namespace v1_1 {         // Version: v1.1 (legacy version)
            // Milan v1.1 legacy implementation
        }
    }
    namespace Profiles {         // Standard: Profiles
        namespace AVB {          // Subpart: AVB
            namespace _2_0 {     // Version: 2.0 (dots→underscores)
                // AVB Profile 2.0 implementation
            }
        }
    }
} // namespace AVnu

namespace AES {                  // Organization: Audio Engineering Society
    namespace AES67 {            // Standard: AES67 (audio-over-IP)
        namespace _2018 {        // Version: 2018 (year)
            class AudioOverIP;
        }
    }
    namespace AES70 {            // Standard: AES70 (device control)
        namespace _2021 {        // Version: 2021 (year)
            class DeviceControl;
        }
    }
} // namespace AES

namespace ITU {                  // Organization: ITU-T (if needed)
    namespace G {                // Standard: G-series recommendations
        namespace G8275_1 {      // Subpart: G.8275.1 (dots→underscores)
            namespace _2016 {    // Version: 2016 (year)
                // ITU-T G.8275.1-2016 telecom PTP
            }
        }
    }
} // namespace ITU

namespace Common {               // Cross-organization utilities
    namespace interfaces {
        // Hardware abstraction interfaces - ONLY in Common namespace
        class NetworkInterface;
        class TimerInterface;
        class ClockInterface;
    }
    namespace utils {
        // Cross-standard utilities
        class PacketParser;
        class CRCCalculator;
    }
    namespace testing {
        // Common testing frameworks
        class TestFrameworkBase;
    }
} // namespace Common
```

### File Naming Conventions Following Generic Pattern

**PATTERN**: `<Organization>/<Standard>/<Subpart>/<Version>/<functional_area>/<file_name>.<ext>`

```cpp
// ✅ CORRECT file naming following generic pattern

// IEEE Examples:
lib/Standards/IEEE/1722.1/2021/aem/entity_model.hpp                    // IEEE::_1722_1::_2021::aem
lib/Standards/IEEE/1722.1/2021/aem/entity_model.cpp
lib/Standards/IEEE/1722.1/2021/aecp/aem_command.hpp                    // IEEE::_1722_1::_2021::aecp
lib/Standards/IEEE/1722.1/2021/aecp/aem_command.cpp
lib/Standards/IEEE/1722.1/2021/descriptors/entity_descriptor.hpp       // IEEE::_1722_1::_2021::descriptors

lib/Standards/IEEE/802.1/AS/2021/core/conformity_test_framework.h       // IEEE::_802_1::AS::_2021::core
lib/Standards/IEEE/802.1/AS/2021/core/gptp_state_machine.hpp           // IEEE::_802_1::AS::_2021::core
lib/Standards/IEEE/802.1/AS/2021/messages/sync_message.hpp             // IEEE::_802_1::AS::_2021::messages
lib/Standards/IEEE/802.1/AS/2021/messages/sync_message.cpp
lib/Standards/IEEE/802.1/AS/2021/clock/best_master_clock.hpp           // IEEE::_802_1::AS::_2021::clock

lib/Standards/IEEE/1722/2016/avtp/avtp_packet.hpp                      // IEEE::_1722::_2016::avtp
lib/Standards/IEEE/1722/2016/avtp/avtp_packet.cpp
lib/Standards/IEEE/1722/2016/formats/aaf/audio_format.hpp              // IEEE::_1722::_2016::formats::aaf
lib/Standards/IEEE/1722/2016/formats/crf/clock_reference.hpp           // IEEE::_1722::_2016::formats::crf

lib/Standards/IEEE/1588/2019/core/ptp_state_machine.hpp                // IEEE::_1588::_2019::core
lib/Standards/IEEE/802.1/Q/2018/vlan/vlan_management.hpp               // IEEE::_802_1::Q::_2018::vlan

// AVnu Examples:
lib/Standards/AVnu/Milan/v1.2/discovery/milan_discovery.hpp            // AVnu::Milan::v1_2::discovery
lib/Standards/AVnu/Milan/v1.2/discovery/milan_discovery.cpp
lib/Standards/AVnu/Milan/v1.2/connection/redundant_streams.hpp         // AVnu::Milan::v1_2::connection
lib/Standards/AVnu/Profiles/AVB/2.0/interop/profile_validation.hpp     // AVnu::Profiles::AVB::_2_0::interop

// AES Examples:
lib/Standards/AES/AES67/2018/audio/audio_over_ip.hpp                   // AES::AES67::_2018::audio
lib/Standards/AES/AES67/2018/audio/audio_over_ip.cpp
lib/Standards/AES/AES70/2021/control/device_control.hpp                // AES::AES70::_2021::control

// ITU Examples (if needed):
lib/Standards/ITU/G/G.8275.1/2016/telecom/telecom_ptp.hpp              // ITU::G::G8275_1::_2016::telecom

// Common utilities (organization-agnostic):
lib/Standards/Common/interfaces/network_interface.hpp                   // Common::interfaces
lib/Standards/Common/utils/packet_parser.hpp                           // Common::utils
lib/Standards/Common/testing/test_framework_base.hpp                    // Common::testing
```

### Header Guard and Include Conventions Following Actual Pattern
```cpp
// ✅ CORRECT header guards following IEEE namespace structure
#ifndef IEEE_1722_1_2021_AEM_ENTITY_MODEL_H
#define IEEE_1722_1_2021_AEM_ENTITY_MODEL_H

// ✅ CORRECT include structure - relative paths from IEEE namespace
#include "../descriptors/entity_descriptor.h"
#include "../../../../Common/interfaces/network_interface.h"

namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace aem {

class EntityModel {
    // IEEE 1722.1-2021 compliant implementation
};

} // namespace aem
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE

#endif // IEEE_1722_1_2021_AEM_ENTITY_MODEL_H
```

### Correct Include Patterns for Cross-Standard Dependencies
```cpp
// ✅ CORRECT - IEEE standards can reference each other
#include "../../1722/2016/avtp/avtp_packet.h"        // AVDECC using AVTP
#include "../../802.1/AS/2021/core/time_sync.h"      // AVDECC using gPTP

// ✅ CORRECT - Common utilities accessible to all standards
#include "../../../Common/interfaces/network_interface.h"
#include "../../../Common/utils/packet_parser.h"

// ❌ WRONG - No hardware-specific includes in IEEE namespace
// #include "../../../../../intel_avb/include/intel_hal.h"  // NO!
// #include "../../../../../common/hal/network_hal.h"       // NO!
```

### Cross-Standard Reuse and Dependencies

**MANDATORY RULE**: When an IEEE standard references or builds upon another IEEE standard, **ALWAYS reuse the existing implementation** rather than creating redundant code.

#### Examples of Required Cross-Standard Reuse:

**IEEE 1722.1 (AVDECC) Dependencies:**
```cpp
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace aecp {

// ✅ CORRECT - Reuse IEEE 1722 AVTP implementation
#include "../../../1722/2016/avtp/avtp_packet.h"
using IEEE::_1722::_2016::avtp::AVTPPacket;

// ✅ CORRECT - Reuse IEEE 802.1AS time synchronization
#include "../../../802.1/AS/2021/core/time_sync.h"
using IEEE::_802_1::AS::_2021::core::TimeSynchronization;

class AEMCommand {
    // AVDECC commands are transported over AVTP
    IEEE::_1722::_2016::avtp::AVTPPacket create_avtp_packet() {
        // Reuse AVTP implementation, don't reimplement
        return IEEE::_1722::_2016::avtp::AVTPPacket::create_aecp_packet();
    }
    
    // AVDECC requires synchronized time from gPTP
    uint64_t get_synchronized_time() {
        // Reuse gPTP time, don't reimplement time sync
        return IEEE::_802_1::AS::_2021::core::TimeSynchronization::get_current_time();
    }
};

} // namespace aecp
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
```

**IEEE 1722 (AVTP) Dependencies:**
```cpp
namespace IEEE {
namespace _1722 {
namespace _2016 {
namespace avtp {

// ✅ CORRECT - Reuse IEEE 802.1AS timing for presentation time
#include "../../../802.1/AS/2021/core/time_sync.h"

class StreamDataHeader {
    uint64_t calculate_presentation_time(uint32_t delay_ns) {
        // Reuse gPTP synchronized time, don't reimplement
        auto current_time = IEEE::_802_1::AS::_2021::core::TimeSynchronization::get_current_time();
        return current_time + delay_ns;
    }
};

} // namespace avtp
} // namespace _2016
} // namespace _1722
} // namespace IEEE
```

**Milan Extensions Dependencies:**
```cpp
namespace AVnu {
namespace Milan {
namespace v1_2 {
namespace discovery {

// ✅ CORRECT - Milan builds on IEEE 1722.1, reuse implementation
#include "../../../../IEEE/1722.1/2021/adp/discovery_protocol.h"
#include "../../../../IEEE/1722.1/2021/aem/entity_model.h"

class MilanDiscoveryExtensions : public IEEE::_1722_1::_2021::adp::DiscoveryProtocol {
    // Milan extends IEEE 1722.1 AVDECC, inherit don't reimplement
public:
    // Milan-specific discovery features
    void discover_milan_devices() {
        // Use base IEEE 1722.1 discovery, add Milan extensions
        DiscoveryProtocol::discover_devices();
        apply_milan_filtering();
    }
    
private:
    void apply_milan_filtering() {
        // Milan-specific logic only
    }
};

} // namespace discovery
} // namespace v1_2
} // namespace Milan
} // namespace AVnu
```

#### Forbidden Redundant Implementations:
```cpp
// ❌ WRONG - Reimplementing existing IEEE standards
namespace IEEE {
namespace _1722_1 {
namespace _2021 {

// DON'T DO THIS - AVTP already exists in IEEE::_1722
class AVDECCTransportPacket {  // NO - use IEEE::_1722::_2016::avtp::AVTPPacket
    // ... redundant AVTP implementation
};

// DON'T DO THIS - gPTP already exists in IEEE::_802_1::AS
class AVDECCTimeSync {  // NO - use IEEE::_802_1::AS::_2021::core::TimeSynchronization
    // ... redundant time sync implementation
};

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
```

#### Cross-Standard Dependency Rules:
1. **IEEE Layering Hierarchy** (higher layers depend on lower layers):
   - **Application Layer**: IEEE 1722.1 (AVDECC)
   - **Transport Layer**: IEEE 1722 (AVTP) 
   - **Timing Layer**: IEEE 802.1AS (gPTP)
   - **Network Layer**: IEEE 802.1Q (VLAN/QoS)

2. **Dependency Direction**: 
   - ✅ **IEEE 1722.1 CAN depend on IEEE 1722 and IEEE 802.1AS**
   - ✅ **IEEE 1722 CAN depend on IEEE 802.1AS**
   - ❌ **IEEE 802.1AS CANNOT depend on IEEE 1722 or IEEE 1722.1**

3. **Extension Standards**:
   - ✅ **Milan CAN depend on any IEEE standard it extends**
   - ✅ **AES67 CAN depend on IEEE standards it references**
   - ✅ **Multiple standard versions CAN coexist** (2021, 2016, 2013)

#### CMake Dependencies for Cross-Standard Reuse:
```cmake
# ✅ CORRECT - Respect IEEE layering in CMake dependencies
target_link_libraries(ieee_1722_1_2021
    ieee_1722_2016               # AVDECC depends on AVTP
    ieee_802_1_as_2021          # AVDECC depends on gPTP
    standards_common            # All can use Common utilities
)

target_link_libraries(ieee_1722_2016
    ieee_802_1_as_2021          # AVTP depends on gPTP for timing
    standards_common
)

target_link_libraries(avnu_milan_v12
    ieee_1722_1_2021            # Milan extends IEEE 1722.1
    ieee_1722_2016              # Milan may use AVTP directly
    ieee_802_1_as_2021          # Milan requires precise timing
    standards_common
)

# ❌ WRONG - Violates layering hierarchy
# target_link_libraries(ieee_802_1_as_2021
#     ieee_1722_2016            # NO - gPTP cannot depend on AVTP
# )
```

This approach ensures:
- **No code duplication** across IEEE standards
- **Proper architectural layering** following IEEE specifications  
- **Consistent behavior** when standards reference each other
- **Maintainable codebase** with single source of truth for each protocol feature

### Forbidden Namespace Violations - Corrected
```cpp
// ❌ WRONG - mixing standards with hardware (corrected understanding)
namespace IEEE {
namespace intel {              // NO - hardware vendor in IEEE namespace
    class IntelAVBInterface;
}
}

// ❌ WRONG - OS-specific namespaces in IEEE standards
namespace IEEE {
namespace windows {            // NO - OS specific in IEEE namespace
    class WinSockInterface;
}
}

// ❌ WRONG - implementation details in IEEE namespace  
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
    class WindowsSocketImpl;  // NO - implementation detail, not protocol
}
}
}

// ✅ CORRECT - IEEE standards are pure protocol implementations
namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace aem {
    class EntityModel;        // YES - pure IEEE 1722.1-2021 protocol
}
}
}
}
```

### CMake Integration with Correct Structure
```cmake
# ✅ CORRECT CMake structure following actual IEEE hierarchy
add_library(ieee_802_1_as_2021 STATIC
    IEEE/802.1/AS/2021/core/gptp_state_machine.cpp
    IEEE/802.1/AS/2021/messages/sync_message.cpp
    IEEE/802.1/AS/2021/clock/best_master_clock.cpp
    IEEE/802.1/AS/2021/core/conformity_test_framework.cpp
)

add_library(ieee_1722_1_2021 STATIC
    IEEE/1722.1/2021/aem/entity_model.cpp
    IEEE/1722.1/2021/aecp/aem_command.cpp
    IEEE/1722.1/2021/descriptors/entity_descriptor.cpp
)

add_library(ieee_1722_2016 STATIC
    IEEE/1722/2016/avtp/avtp_packet.cpp
    IEEE/1722/2016/formats/aaf/audio_format.cpp
)

add_library(avnu_milan_v12 STATIC
    AVnu/Milan/v1.2/discovery/milan_discovery.cpp
    AVnu/Milan/v1.2/connection/redundant_streams.cpp
)

add_library(standards_common STATIC
    Common/interfaces/network_interface.cpp
    Common/utils/packet_parser.cpp
    Common/testing/test_framework_base.cpp
)

# IEEE standards libraries can depend on each other following IEEE layering
target_link_libraries(ieee_1722_1_2021
    ieee_1722_2016               # AVDECC depends on AVTP
    ieee_802_1_as_2021          # AVDECC depends on gPTP
    standards_common            # All can use Common utilities
)
```

### Documentation Structure Requirements - Corrected
```cpp
/**
 * @file conformity_test_framework.h
 * @brief IEEE 802.1AS-2021 Conformity Testing Framework
 * @namespace IEEE::_802_1::AS::_2021::Testing
 * 
 * Implements conformity testing according to IEEE 802.1AS-2021 specification.
 * This namespace contains all testing functionality for validating IEEE 802.1AS-2021
 * compliance including state machine behavior, timing requirements, and interoperability.
 * 
 * @see IEEE 802.1AS-2021, Clauses 11.2-11.5 "Conformance requirements"
 * @see IEEE 802.1AS-2021, Annex A "Implementation conformance statement (ICS)"
 */
```

### Enforcement Rules - Corrected with Copyright Compliance
1. **IEEE namespaces are top-level** - not wrapped in `openavnu::standards`
2. **Namespace must match folder structure** exactly (`IEEE::_802_1::AS::_2021` = `IEEE/802.1/AS/2021/`)
3. **Version numbers use underscores** in namespaces (`_2021`, `_2016`) to avoid conflicts
4. **Dots become underscores** in namespaces (`802.1` becomes `_802_1`, `1722.1` becomes `_1722_1`)
5. **No hardware vendors** in IEEE namespace hierarchy
6. **No OS-specific namespaces** in IEEE standards
7. **IEEE layering respected** - higher layer standards can depend on lower layers
8. **Cross-standard utilities** only in Common namespace
9. **Conformance testing** isolated in Testing sub-namespaces
10. **COPYRIGHT COMPLIANCE MANDATORY**:
    - **NEVER reproduce copyrighted specification content** in source code or documentation
    - **Reference specifications by section number only** (e.g., "IEEE 1722.1-2021, Section 7.2.1")
    - **Implement based on understanding**, not by copying specification text
    - **Use MCP-Server for compliance verification only**, not content reproduction
    - **Respect all copyright holders**: IEEE, AES, AVnu Alliance, ITU, etc.
    - **Include copyright disclaimer** in implementations referencing multiple standards
    - **Document original implementation** that achieves compliance through understanding

### Repository Copyright Policy
```cpp
// ✅ REQUIRED copyright notice for standards-based implementations
/*
 * This file implements protocol functionality based on understanding of:
 * - IEEE 1722.1-2021 (AVDECC) - Copyright IEEE
 * - IEEE 1722-2016 (AVTP) - Copyright IEEE  
 * - IEEE 802.1AS-2021 (gPTP) - Copyright IEEE
 * - Milan v1.2 - Copyright AVnu Alliance
 * 
 * No copyrighted content from these specifications is reproduced.
 * Implementation is original work achieving compliance through
 * understanding of specification requirements.
 * 
 * For authoritative requirements, refer to original specifications
 * available from respective standards organizations.
 */
```

This structure ensures clear separation of IEEE standards versions, prevents architectural violations, and maintains the hardware-agnostic principle while following the actual implementation pattern used in the codebase.

This architecture ensures the Standards layer remains pure, testable, reusable across different hardware platforms, and maintains strict IEEE compliance while following OpenAvnu's core development principles.