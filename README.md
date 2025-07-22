# IEEE 1722.1-2021 Standard Implementation

This directory contains a comprehensive C++ implementation of the IEEE 1722.1-2021 standard for Device Discovery, Connection Management, and Control Protocol for Time-Sensitive Networking Systems.

## Overview

The IEEE 1722.1-2021 standard, also known as ATDECC (AVB Transport Protocol Device Discovery, Connection Management and Control), defines protocols for:

- **Device Discovery** - Finding ATDECC entities on the network
- **Connection Management** - Establishing and managing streaming connections
- **Device Control** - Controlling and configuring ATDECC entities

## Features Implemented

### ✅ ADPDU (ATDECC Discovery Protocol Data Unit)
- Complete PDU structure with all fields per IEEE 1722.1-2021
- Serialization and deserialization methods
- Network byte order handling
- Cross-platform compatibility (Windows/Linux)
- Entity capabilities, talker/listener capabilities
- Message types: ENTITY_AVAILABLE, ENTITY_DEPARTING, DISCOVERY_REQUEST

### ✅ AEM (ATDECC Entity Model)
- Entity Descriptor structure
- Descriptor type constants for all standard descriptors
- String manipulation utilities
- Validation functions
- Support for future descriptor extensions

### ✅ ACMP (ATDECC Connection Management Protocol)
- Message type enumerations
- Status code definitions
- Foundation for stream connection management

### ✅ AECP (ATDECC Enumeration and Control Protocol)  
- Message type enumerations
- Status code definitions
- Foundation for entity control operations

## Files

- **`1722_1-2021.h`** - Main header file with all declarations
- **`1722_1-2021.cpp`** - Implementation of core functionality
- **`test_1722_1_2021.cpp`** - Comprehensive test program
- **`CMakeLists.txt`** - CMake build configuration
- **`README.md`** - This documentation file

## Building

### Using CMake

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build . --config Release

# Run tests
cmake --build . --target run_ieee_tests
```

### Manual Compilation (Visual Studio)

```batch
cl /EHsc /std:c++17 test_1722_1_2021.cpp 1722_1-2021.cpp /Fe:test_ieee.exe ws2_32.lib
```

### Manual Compilation (GCC/Clang)

```bash
g++ -std=c++17 -Wall -Wextra test_1722_1_2021.cpp 1722_1-2021.cpp -o test_ieee
```

## Usage Example

```cpp
#include "1722_1-2021.h"
using namespace IEEE::_1722_1::_2021;

// Create an ATDECC Discovery PDU
ADPDU::ATDECCDiscoveryProtocolPDU adpdu;

// Set entity information
adpdu.set_entity_id(0x0123456789ABCDEFULL);
adpdu.set_entity_model_id(0xFEDCBA9876543210ULL);

// Configure capabilities
EntityCapabilities caps = static_cast<EntityCapabilities>(
    static_cast<uint32_t>(EntityCapabilities::AEM_SUPPORTED) |
    static_cast<uint32_t>(EntityCapabilities::CLASS_A_SUPPORTED)
);
adpdu.set_entity_capabilities(caps);

// Set talker/listener information
adpdu.set_talker_info(4, TalkerCapabilities::AUDIO_SOURCE);
adpdu.set_listener_info(2, ListenerCapabilities::AUDIO_SINK);

// Serialize for network transmission
adpdu.serialize();
const uint8_t* raw_data = adpdu.get_raw_octets();
size_t data_size = adpdu.get_size();

// Send raw_data over network...
```

## Standard Compliance

This implementation follows IEEE 1722.1-2021 specifications:

- **Section 6**: ATDECC Discovery Protocol (ADP) ✅
- **Section 7**: ATDECC Entity Model (AEM) ✅ (Core descriptors)
- **Section 8**: ATDECC Connection Management Protocol (ACMP) ⚠️ (Enums only)
- **Section 9**: ATDECC Enumeration and Control Protocol (AECP) ⚠️ (Enums only)

## Platform Support

- **Windows** - Full support with Winsock2
- **Linux** - Full support with standard networking headers
- **macOS** - Should work (not explicitly tested)
- **Embedded** - Should work with appropriate networking stack

## Key Design Decisions

1. **Namespace Organization**: Clean separation into `ADPDU`, `AEM`, `ACMP`, `AECP` namespaces
2. **Type Safety**: Strong typing with `enum class` where appropriate
3. **Cross-Platform**: Conditional compilation for platform-specific headers
4. **Memory Management**: Stack-based structures, no dynamic allocation
5. **Standard Compliance**: Direct mapping to IEEE standard field names and sizes

## Testing

The test program (`test_1722_1_2021.cpp`) demonstrates:

- ADPDU creation and configuration
- Serialization/deserialization round-trip
- Entity descriptor validation
- Protocol constants verification
- Cross-platform byte order handling

Run the test with:
```bash
./test_ieee_1722_1_2021
```

## Future Enhancements

Areas for potential expansion:

1. **Complete ACMP Implementation** - Full PDU structures and state machines
2. **Complete AECP Implementation** - AEM commands and responses
3. **Additional AEM Descriptors** - STREAM_INPUT/OUTPUT, JACK_INPUT/OUTPUT, etc.
4. **State Machines** - Full protocol state machine implementations
5. **Network Interface** - Socket-based networking layer
6. **Validation** - More comprehensive PDU validation
7. **Utilities** - Higher-level API for common operations

## Contributing

When extending this implementation:

1. Follow the existing namespace structure
2. Maintain IEEE standard field names and sizes
3. Add comprehensive tests for new functionality
4. Update this README with new features
5. Ensure cross-platform compatibility

## License

This implementation is part of the OpenAvnu project. Please refer to the main project license.

## References

- IEEE Std 1722.1-2021: Standard for Device Discovery, Connection Management, and Control Protocol for Time-Sensitive Networking Systems
- IEEE Std 1722-2016: Standard for a Transport Protocol for Time-Sensitive Applications in Bridged Local Area Networks
- AVnu Alliance specifications and documentation
