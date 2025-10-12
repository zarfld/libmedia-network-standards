---
title: "IEEE 1588-2019 Transport Layer - Detailed Design"
specType: design
version: 0.1.0
status: draft
author: AI Standards Implementation Agent
date: 2024-12-10
relatedArchitecture:
  - ARC-C-004-Transport
  - ARC-C-005-HardwareAbstraction
relatedRequirements:
  - REQ-F-1588-004-TransportLayerSupport
  - REQ-NFR-1588-003-HardwareAbstraction
traceability:
  - from: ARC-C-004-Transport
  - to: DES-C-006-TransportManager
---

# IEEE 1588-2019 Transport Layer - Detailed Design

> **Purpose**: Detailed component design for IEEE 1588-2019 multi-transport abstraction
> **Standard Compliance**: IEEE 1016-2009 Software Design Description format  
> **Protocol Reference**: IEEE 1588-2019 Annex D "Transport of PTP messages"

## DES-C-006: Transport Manager Component

### **Component Overview**

The Transport Manager provides hardware-agnostic multi-transport support for IEEE 1588-2019 per Annex D, coordinating Ethernet and UDP transport mechanisms.

### **Design Constraints**

- **Hardware Abstraction**: No direct network hardware dependencies
- **Multi-Transport**: Simultaneous Ethernet L2 and UDP/IPv4/IPv6 support
- **Performance**: <50μs packet send/receive latency overhead
- **Thread Safety**: Concurrent access from multiple PTP ports

### **Transport Interface Abstraction (DES-I-005)**

```cpp
namespace IEEE::_1588::PTP::_2019::Transport {

/**
 * @brief Generic transport interface for PTP message delivery
 * @design DES-I-005-TransportInterface
 * @traces ARC-C-004-Transport
 */
class ITransport {
public:
    virtual ~ITransport() = default;
    
    /**
     * @brief Initialize transport with configuration
     * @param config Transport-specific configuration
     * @return Success/failure result
     * @design DES-I-005a-Initialize
     */
    virtual PTPResult<void> initialize(const TransportConfig& config) = 0;
    
    /**
     * @brief Send PTP message via transport
     * @param message PTP message to send
     * @param destination Target address (transport-specific)
     * @param timestamp_request Request hardware timestamp on send
     * @return Send result with optional timestamp
     * @design DES-I-005b-SendMessage
     * @timing Must complete within 50μs for real-time requirements
     */
    virtual PTPResult<SendResult> sendMessage(
        const PTPMessage& message,
        const TransportAddress& destination,
        bool timestamp_request = false) = 0;
    
    /**
     * @brief Receive PTP messages from transport
     * @param buffer Buffer to receive message data
     * @param buffer_size Maximum buffer size
     * @param timeout_ms Receive timeout in milliseconds
     * @return Received message with metadata
     * @design DES-I-005c-ReceiveMessage
     */
    virtual PTPResult<ReceiveResult> receiveMessage(
        void* buffer,
        size_t buffer_size,
        uint32_t timeout_ms = 0) = 0;
    
    /**
     * @brief Get transport capabilities
     * @return Supported features and limitations
     * @design DES-I-005d-GetCapabilities
     */
    virtual TransportCapabilities getCapabilities() const noexcept = 0;
    
    /**
     * @brief Enable/disable hardware timestamping
     * @param enable Enable hardware timestamping if supported
     * @return Success if supported, failure if not available
     * @design DES-I-005e-ConfigureTimestamping
     */
    virtual PTPResult<void> configureHardwareTimestamping(bool enable) = 0;
};

/**
 * @brief Transport-agnostic address abstraction
 * @design DES-D-009-TransportAddress
 */
struct TransportAddress {
    enum class Type : uint8_t {
        ETHERNET_MULTICAST,    ///< Layer 2 Ethernet multicast
        UDP_IPV4_MULTICAST,   ///< UDP/IPv4 multicast
        UDP_IPV6_MULTICAST,   ///< UDP/IPv6 multicast
        UDP_IPV4_UNICAST,     ///< UDP/IPv4 unicast
        UDP_IPV6_UNICAST      ///< UDP/IPv6 unicast
    };
    
    Type type;
    union {
        struct {
            uint8_t mac[6];        ///< Ethernet MAC address
        } ethernet;
        struct {
            uint32_t addr;         ///< IPv4 address (network order)
            uint16_t port;         ///< UDP port number
        } ipv4;
        struct {
            uint8_t addr[16];      ///< IPv6 address
            uint16_t port;         ///< UDP port number
        } ipv6;
    } address;
    
    /**
     * @brief Validate address for transport type
     * @design DES-D-009a-ValidateAddress
     */
    bool isValid() const noexcept;
};

} // namespace IEEE::_1588::PTP::_2019::Transport
```

### **Ethernet Transport Implementation (DES-C-007)**

```cpp
/**
 * @brief IEEE 802.3 Ethernet transport per IEEE 1588-2019 Annex D.2
 * @design DES-C-007-EthernetTransport
 * @traces REQ-F-1588-004-TransportLayerSupport
 */
class EthernetTransport : public ITransport {
private:
    std::unique_ptr<INetworkInterface> network_interface_;  ///< Hardware abstraction
    EthernetConfig config_;
    std::atomic<bool> hardware_timestamping_enabled_{false};
    
    // PTP Ethernet constants per IEEE 1588-2019 Annex D.2.2
    static constexpr uint8_t PTP_MULTICAST_MAC[6] = {0x01, 0x1B, 0x19, 0x00, 0x00, 0x00};
    static constexpr uint8_t PDELAY_MULTICAST_MAC[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E};
    static constexpr uint16_t PTP_ETHERTYPE = 0x88F7;  ///< IEEE 1588 EtherType
    
public:
    /**
     * @brief Construct Ethernet transport with hardware interface
     * @param network_interface Hardware abstraction interface
     * @design DES-C-007a-Constructor
     */
    explicit EthernetTransport(std::unique_ptr<INetworkInterface> network_interface);
    
    /**
     * @brief Initialize Ethernet transport per IEEE 1588-2019 Annex D.2
     * @design DES-C-007b-Initialize
     */
    PTPResult<void> initialize(const TransportConfig& config) override;
    
    /**
     * @brief Send PTP message over Ethernet
     * @design DES-C-007c-SendMessage
     * @algorithm Per IEEE 1588-2019 Annex D.2.3 message format
     */
    PTPResult<SendResult> sendMessage(
        const PTPMessage& message,
        const TransportAddress& destination,
        bool timestamp_request = false) override;
    
    /**
     * @brief Receive PTP messages from Ethernet
     * @design DES-C-007d-ReceiveMessage  
     * @filter Only accepts IEEE 1588 EtherType (0x88F7) frames
     */
    PTPResult<ReceiveResult> receiveMessage(
        void* buffer,
        size_t buffer_size,
        uint32_t timeout_ms = 0) override;
    
    /**
     * @brief Get Ethernet transport capabilities
     * @design DES-C-007e-GetCapabilities
     */
    TransportCapabilities getCapabilities() const noexcept override;
    
    /**
     * @brief Configure hardware timestamping on Ethernet interface
     * @design DES-C-007f-ConfigureTimestamping
     */
    PTPResult<void> configureHardwareTimestamping(bool enable) override;

private:
    /**
     * @brief Build Ethernet frame header per IEEE 1588-2019
     * @design DES-C-007g-BuildEthernetHeader
     */
    size_t buildEthernetHeader(uint8_t* frame_buffer,
                              const TransportAddress& dest,
                              size_t payload_size);
    
    /**
     * @brief Parse received Ethernet frame
     * @design DES-C-007h-ParseEthernetFrame
     */
    PTPResult<EthernetFrameInfo> parseEthernetFrame(const uint8_t* frame_data,
                                                   size_t frame_size);
};
```

### **UDP Transport Implementation (DES-C-008)**

```cpp
/**
 * @brief UDP/IP transport per IEEE 1588-2019 Annex D.3
 * @design DES-C-008-UDPTransport
 * @traces REQ-F-1588-004-TransportLayerSupport
 */
class UDPTransport : public ITransport {
private:
    std::unique_ptr<IUDPInterface> udp_interface_;  ///< Hardware abstraction
    UDPConfig config_;
    
    // UDP port assignments per IEEE 1588-2019 Annex D.3.2
    static constexpr uint16_t PTP_EVENT_PORT = 319;     ///< Event messages
    static constexpr uint16_t PTP_GENERAL_PORT = 320;   ///< General messages
    
    // Multicast addresses per IEEE 1588-2019 Annex D.3.3
    static constexpr uint32_t IPV4_MULTICAST_ADDR = 0xE0000181;  // 224.0.1.129
    static constexpr uint8_t IPV6_MULTICAST_ADDR[16] = {
        0xFF, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x00, 0x00
    };
    
public:
    /**
     * @brief Construct UDP transport with hardware interface
     * @design DES-C-008a-Constructor
     */
    explicit UDPTransport(std::unique_ptr<IUDPInterface> udp_interface);
    
    /**
     * @brief Initialize UDP transport per IEEE 1588-2019 Annex D.3
     * @design DES-C-008b-Initialize
     */
    PTPResult<void> initialize(const TransportConfig& config) override;
    
    /**
     * @brief Send PTP message over UDP
     * @design DES-C-008c-SendMessage
     * @algorithm Per IEEE 1588-2019 Annex D.3.4 message encapsulation
     */
    PTPResult<SendResult> sendMessage(
        const PTPMessage& message,
        const TransportAddress& destination,
        bool timestamp_request = false) override;
    
    /**
     * @brief Receive PTP messages from UDP sockets
     * @design DES-C-008d-ReceiveMessage
     */
    PTPResult<ReceiveResult> receiveMessage(
        void* buffer,
        size_t buffer_size,
        uint32_t timeout_ms = 0) override;
    
    /**
     * @brief Get UDP transport capabilities  
     * @design DES-C-008e-GetCapabilities
     */
    TransportCapabilities getCapabilities() const noexcept override;
    
    /**
     * @brief Configure UDP socket timestamping
     * @design DES-C-008f-ConfigureTimestamping
     */
    PTPResult<void> configureHardwareTimestamping(bool enable) override;

private:
    /**
     * @brief Determine UDP port based on message type
     * @design DES-C-008g-SelectPort
     */
    uint16_t selectUDPPort(const PTPMessage& message) const noexcept;
    
    /**
     * @brief Build UDP packet header
     * @design DES-C-008h-BuildUDPHeader
     */
    size_t buildUDPHeader(uint8_t* packet_buffer,
                         const TransportAddress& dest,
                         size_t payload_size);
};
```

### **Transport Manager Coordination (DES-C-009)**

```cpp
/**
 * @brief Coordinates multiple transport mechanisms
 * @design DES-C-009-TransportManager
 * @traces ARC-C-004-Transport
 */
class TransportManager {
private:
    std::vector<std::unique_ptr<ITransport>> transports_;
    TransportManagerConfig config_;
    std::shared_mutex transport_mutex_;  ///< Protects transport collection
    
public:
    /**
     * @brief Construct transport manager
     * @design DES-C-009a-Constructor
     */
    explicit TransportManager(const TransportManagerConfig& config);
    
    /**
     * @brief Register transport implementation
     * @param transport Transport implementation to register
     * @return Registration result
     * @design DES-C-009b-RegisterTransport
     */
    PTPResult<void> registerTransport(std::unique_ptr<ITransport> transport);
    
    /**
     * @brief Send message via appropriate transport
     * @param message PTP message to send
     * @param destination Target address
     * @param preferred_transport Preferred transport type
     * @return Send result from selected transport
     * @design DES-C-009c-SendMessage
     */
    PTPResult<SendResult> sendMessage(
        const PTPMessage& message,
        const TransportAddress& destination,
        TransportType preferred_transport = TransportType::AUTO_SELECT);
    
    /**
     * @brief Receive messages from all active transports
     * @param timeout_ms Receive timeout
     * @return Message from first responding transport
     * @design DES-C-009d-ReceiveMessage
     */
    PTPResult<ReceiveResult> receiveMessage(uint32_t timeout_ms = 0);
    
    /**
     * @brief Get combined capabilities from all transports
     * @design DES-C-009e-GetCapabilities
     */
    CombinedTransportCapabilities getCapabilities() const;

private:
    /**
     * @brief Select best transport for destination
     * @design DES-C-009f-SelectTransport
     */
    ITransport* selectTransportForDestination(const TransportAddress& dest);
    
    /**
     * @brief Poll all transports for incoming messages
     * @design DES-C-009g-PollTransports
     */
    PTPResult<ReceiveResult> pollAllTransports(uint32_t timeout_ms);
};
```

## **Hardware Abstraction Interfaces (DES-I-006)**

```cpp
/**
 * @brief Network interface hardware abstraction
 * @design DES-I-006-NetworkInterface
 * @traces ARC-C-005-HardwareAbstraction
 */
class INetworkInterface {
public:
    virtual ~INetworkInterface() = default;
    
    /**
     * @brief Send raw Ethernet frame
     * @param frame_data Complete Ethernet frame
     * @param frame_size Frame size in bytes
     * @param timestamp_request Request hardware timestamp
     * @return Send result with optional timestamp
     * @design DES-I-006a-SendFrame
     */
    virtual PTPResult<NetworkSendResult> sendFrame(
        const void* frame_data,
        size_t frame_size,
        bool timestamp_request = false) = 0;
    
    /**
     * @brief Receive raw Ethernet frame
     * @param buffer Buffer for received frame
     * @param buffer_size Maximum buffer size
     * @param timeout_ms Receive timeout
     * @return Received frame with metadata
     * @design DES-I-006b-ReceiveFrame
     */
    virtual PTPResult<NetworkReceiveResult> receiveFrame(
        void* buffer,
        size_t buffer_size,
        uint32_t timeout_ms = 0) = 0;
    
    /**
     * @brief Get interface MAC address
     * @param mac_addr Output buffer for MAC address (6 bytes)
     * @design DES-I-006c-GetMACAddress
     */
    virtual PTPResult<void> getMACAddress(uint8_t mac_addr[6]) const = 0;
    
    /**
     * @brief Check hardware timestamping support
     * @design DES-I-006d-SupportsTimestamping
     */
    virtual bool supportsHardwareTimestamping() const noexcept = 0;
};
```

## **Performance Requirements & Test Design (DES-T-003)**

### **Performance Test Requirements**

- **Send Latency**: <50μs from API call to wire transmission
- **Receive Latency**: <50μs from wire reception to API return
- **Concurrent Transport Capacity**: Support 4 simultaneous transports
- **Message Throughput**: 1000 PTP messages/second per transport
- **Memory Usage**: <16KB per transport instance

### **Unit Test Interface Design**

```cpp
/**
 * @brief Test fixture for transport layer validation
 * @design DES-T-003-TransportTestFixture
 * @traces DES-C-009-TransportManager
 */
class TransportLayerTest {
private:
    std::unique_ptr<TransportManager> transport_manager_;
    MockNetworkInterface mock_ethernet_;
    MockUDPInterface mock_udp_;
    
public:
    /**
     * Test scenarios per IEEE 1588-2019 specification:
     * - test_EthernetMessageTransmission()
     * - test_UDPMessageTransmission()
     * - test_MultiTransportCoordination()
     * - test_HardwareTimestamping()
     * - test_AddressResolution()
     * - test_TransportFailover()
     */
};
```

## **Traceability Matrix**

| Design Element | Traces To Architecture | Traces To Requirements | Implementation Target |
|----------------|------------------------|------------------------|----------------------|
| DES-C-006 | ARC-C-004-Transport | REQ-F-1588-004 | `src/transport_manager.cpp` |
| DES-I-005 | ARC-C-004-Transport | REQ-F-1588-004 | `include/transport.hpp` |
| DES-C-007 | ARC-C-004-Transport | REQ-F-1588-004 | `src/ethernet_transport.cpp` |
| DES-C-008 | ARC-C-004-Transport | REQ-F-1588-004 | `src/udp_transport.cpp` |
| DES-I-006 | ARC-C-005-HardwareAbstraction | REQ-NFR-1588-003 | `Common/interfaces/` |

## **Implementation Readiness**

### **Phase 04 Exit Criteria Status**

- ✅ **Multi-Transport Design**: Ethernet and UDP transports specified per IEEE 1588-2019
- ✅ **Hardware Abstraction**: Complete interface abstraction for network hardware
- ✅ **Performance Requirements**: <50μs latency specifications defined
- ✅ **Thread Safety Design**: Concurrent transport access patterns specified
- ✅ **Test Design Prepared**: Transport validation scenarios defined

### **Ready for Phase 05 Implementation**

This transport design enables TDD implementation with:

- Complete IEEE 1588-2019 Annex D compliance specification
- Hardware abstraction for cross-platform deployment
- Performance requirements for real-time media networking
- Multi-transport coordination and failover capabilities
- Comprehensive test coverage for transport reliability