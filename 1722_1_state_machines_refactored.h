/**
 * @file 1722_1_state_machines_refactored.h
 * @brief IEEE 1722.1-2021 Protocol State Machines Library Implementation
 * @details Complete library design for IEEE 1722.1-2021 AVDECC state machines
 *
 * This library provides:
 * - Complete IEEE 1722.1-2021 state machine implementation (internal)
 * - Clean interface abstractions for application integration
 * - Protocol message serialization/deserialization (internal)
 * - Entity model delegation for application-specific behavior
 * 
 * @author OpenAvnu AVDECC Implementation
 * @date 2025
 * @version 2.0.0
 * 
 * @copyright
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "1722_1-2021_clean.h"
#include <chrono>
#include <memory>
#include <functional>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <atomic>
#include <vector>
#include <cstring>
#include <cstdio>

namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace StateMachines {

// Import types from parent namespaces
using namespace IEEE::_1722_1::_2021::AEM;
using namespace IEEE::_1722_1::_2021::ADPDU;
using namespace IEEE::_1722_1::_2021::ACMP;
using namespace IEEE::_1722_1::_2021::AECP;

// Basic type aliases for cleaner code
using EntityID = uint64_t;
using EntityModelID = uint64_t;
using AemCommandType = AEM_Command_type;
using AemCommandStatus = AECP_Status;
using ACMPMessageType = ACMP_Message_type;
using DescriptorType = uint16_t;
using AcquireFlags = uint32_t;
using LockFlags = uint32_t;

// ============================================================================
// MAC ADDRESS TYPE  
// ============================================================================

/** MAC Address representation */
struct MacAddress {
    uint8_t value[6];
    
    MacAddress() {
        std::memset(value, 0, 6);
    }
    
    MacAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5) {
        value[0] = b0; value[1] = b1; value[2] = b2;
        value[3] = b3; value[4] = b4; value[5] = b5;
    }
    
    bool operator==(const MacAddress& other) const {
        return std::memcmp(value, other.value, 6) == 0;
    }
    
    bool operator!=(const MacAddress& other) const {
        return !(*this == other);
    }
    
    std::string toString() const {
        char buffer[18];
        std::snprintf(buffer, sizeof(buffer), "%02X:%02X:%02X:%02X:%02X:%02X",
                     value[0], value[1], value[2], value[3], value[4], value[5]);
        return std::string(buffer);
    }
    
    static const MacAddress& getMulticast() {
        static MacAddress multicast(0x91, 0xE0, 0xF0, 0x01, 0x00, 0x00);
        return multicast;
    }
};

// ============================================================================
// CONNECTION INFO STRUCTURE
// ============================================================================

struct ConnectionInfo {
    EntityID talkerEntityID;
    EntityID listenerEntityID;
    uint16_t talkerUniqueID;
    uint16_t listenerUniqueID;
    uint64_t streamID;
    uint8_t destinationMAC[6];
    ConnectionFlags connectionCount;
    StreamInfoFlags flags;
    uint16_t streamVlanID;
    
    ConnectionInfo() : talkerEntityID(0), listenerEntityID(0), talkerUniqueID(0), 
                      listenerUniqueID(0), streamID(0), connectionCount{}, flags{}, 
                      streamVlanID(0) {
        memset(destinationMAC, 0, 6);
    }
};

// ============================================================================
// PROTOCOL MESSAGE TYPES
// ============================================================================

/** ADP (Discovery Protocol) Messages */
struct ADPEntityDiscoverMessage {
    EntityID entityID;  // 0 for global discovery
    MacAddress sourceMac;
    std::chrono::steady_clock::time_point timestamp;
};

struct ADPEntityAvailableMessage {
    EntityID entityID;
    EntityModelID entityModelID;
    EntityCapabilities entityCapabilities;
    uint16_t talkerStreamSources;
    TalkerCapabilities talkerCapabilities;
    uint16_t listenerStreamSinks;
    ListenerCapabilities listenerCapabilities;
    ControllerCapabilities controllerCapabilities;
    uint32_t availableIndex;
    EntityID gptpGrandmasterID;
    uint8_t gptpDomainNumber;
    uint16_t identifyControlIndex;
    uint16_t interfaceIndex;
    EntityID associationID;
    uint8_t validTime;
    MacAddress sourceMac;
    std::chrono::steady_clock::time_point timestamp;
};

struct ADPEntityDepartingMessage {
    EntityID entityID;
    EntityModelID entityModelID;
    uint32_t availableIndex;
    MacAddress sourceMac;
    std::chrono::steady_clock::time_point timestamp;
};

// ADP Message container
enum class ADPMessageType {
    ENTITY_DISCOVER,
    ENTITY_AVAILABLE, 
    ENTITY_DEPARTING
};

struct ADPMessage {
    ADPMessageType type;
    // Use largest message type to hold all data
    ADPEntityAvailableMessage data;
    
    ADPMessage() : type(ADPMessageType::ENTITY_DISCOVER) {
        memset(&data, 0, sizeof(data));
    }
    
    explicit ADPMessage(ADPMessageType t) : type(t) {
        memset(&data, 0, sizeof(data));
    }
    
    // Helper methods for typed access
    ADPEntityDiscoverMessage* asDiscover() { return reinterpret_cast<ADPEntityDiscoverMessage*>(&data); }
    const ADPEntityDiscoverMessage* asDiscover() const { return reinterpret_cast<const ADPEntityDiscoverMessage*>(&data); }
    
    ADPEntityAvailableMessage* asAvailable() { return &data; }
    const ADPEntityAvailableMessage* asAvailable() const { return &data; }
    
    ADPEntityDepartingMessage* asDeparting() { return reinterpret_cast<ADPEntityDepartingMessage*>(&data); }
    const ADPEntityDepartingMessage* asDeparting() const { return reinterpret_cast<const ADPEntityDepartingMessage*>(&data); }
};

/** ACMP (Connection Management Protocol) Messages */
struct ACMPConnectTxCommand {
    uint16_t sequenceID;
    EntityID talkerEntityID;
    EntityID listenerEntityID;
    uint16_t talkerUniqueID;
    uint16_t listenerUniqueID;
    MacAddress streamDestAddress;
    ConnectionFlags connectionCount;
    StreamInfoFlags flags;
};

struct ACMPConnectTxResponse {
    uint16_t sequenceID;
    ACMP_Status status;
    EntityID talkerEntityID;
    EntityID listenerEntityID;
    uint16_t talkerUniqueID;
    uint16_t listenerUniqueID;
    MacAddress streamDestAddress;
    ConnectionFlags connectionCount;
    StreamInfoFlags flags;
    uint16_t streamVlanID;
};

struct ACMPDisconnectTxCommand {
    uint16_t sequenceID;
    EntityID talkerEntityID;
    EntityID listenerEntityID;
    uint16_t talkerUniqueID;
    uint16_t listenerUniqueID;
};

struct ACMPDisconnectTxResponse {
    uint16_t sequenceID;
    ACMP_Status status;
    EntityID talkerEntityID;
    EntityID listenerEntityID;
    uint16_t talkerUniqueID;
    uint16_t listenerUniqueID;
};

// ACMP Message container
enum class ACMPMessageType_enum {
    CONNECT_TX_COMMAND,
    CONNECT_TX_RESPONSE,
    DISCONNECT_TX_COMMAND,
    DISCONNECT_TX_RESPONSE,
    GET_TX_STATE_COMMAND,
    GET_TX_STATE_RESPONSE,
    CONNECT_RX_COMMAND,
    CONNECT_RX_RESPONSE,
    DISCONNECT_RX_COMMAND,
    DISCONNECT_RX_RESPONSE,
    GET_RX_STATE_COMMAND,
    GET_RX_STATE_RESPONSE,
    GET_TX_CONNECTION_COMMAND,
    GET_TX_CONNECTION_RESPONSE
};

struct ACMPMessage {
    ACMPMessageType_enum type;
    uint8_t pdu_data[64]; // Raw PDU data
    
    ACMPMessage() : type(ACMPMessageType_enum::CONNECT_TX_COMMAND) {
        memset(pdu_data, 0, sizeof(pdu_data));
    }
};

/** AECP (Entity Control Protocol) Messages */
struct AECPAemCommandMessage {
    uint16_t sequenceID;
    EntityID targetEntityID;
    EntityID controllerEntityID;
    AemCommandType commandType;
    uint8_t commandData[256];
    MacAddress sourceMac;
    std::chrono::steady_clock::time_point timestamp;
};

struct AECPAemResponseMessage {
    uint16_t sequenceID;
    EntityID targetEntityID;
    EntityID controllerEntityID;
    AemCommandType commandType;
    AemCommandStatus status;
    uint8_t responseData[256];
    MacAddress sourceMac;
    std::chrono::steady_clock::time_point timestamp;
};

// AECP Message container
enum class AECPMessageType_enum {
    AEM_COMMAND,
    AEM_RESPONSE
};

struct AECPMessage {
    AECPMessageType_enum type;
    uint8_t pdu_data[512]; // Raw PDU data
    
    AECPMessage() : type(AECPMessageType_enum::AEM_COMMAND) {
        memset(pdu_data, 0, sizeof(pdu_data));
    }
};

// ============================================================================
// ENTITY MODEL TYPES
// ============================================================================

/** Discovered Entity Information */
struct DiscoveredEntity {
    EntityID entityID;
    EntityModelID entityModelID;
    EntityCapabilities entityCapabilities;
    
    // Stream capabilities
    uint16_t talkerStreamSources;
    TalkerCapabilities talkerCapabilities;
    uint16_t listenerStreamSinks;
    ListenerCapabilities listenerCapabilities;
    ControllerCapabilities controllerCapabilities;
    
    // Network information
    MacAddress macAddress;
    uint32_t availableIndex;
    
    // gPTP information
    EntityID gptpGrandmasterID;
    uint8_t gptpDomainNumber;
    
    // Additional information
    uint16_t identifyControlIndex;
    uint16_t interfaceIndex;
    EntityID associationID;
    
    // Discovery metadata
    std::chrono::steady_clock::time_point lastSeen;
    std::chrono::steady_clock::time_point timeout;
};

/** Local Entity Information */
struct LocalEntity {
    EntityID entityID;
    EntityModelID entityModelID;
    EntityCapabilities entityCapabilities;
    
    // Stream capabilities
    uint16_t talkerStreamSources;
    TalkerCapabilities talkerCapabilities;
    uint16_t listenerStreamSinks;
    ListenerCapabilities listenerCapabilities;
    ControllerCapabilities controllerCapabilities;
    
    // Network information
    MacAddress macAddress;
    uint8_t validTime;
    
    // gPTP information
    EntityID gptpGrandmasterID;
    uint8_t gptpDomainNumber;
    
    // Additional information
    uint16_t identifyControlIndex;
    uint16_t interfaceIndex;
    EntityID associationID;
    
    // Entity model
    EntityModel entityModel;
    EntityDynamicState dynamicState;
};

// ============================================================================
// LIBRARY INTERFACE ABSTRACTIONS
// ============================================================================

/** 
 * @brief Network interface abstraction for application-provided networking
 * @details Applications must implement this interface to provide platform-specific networking
 */
class INetworkInterface {
public:
    virtual ~INetworkInterface() = default;
    
    /** Send raw Ethernet frame to specified MAC address */
    virtual bool sendRawMessage(const uint8_t* data, size_t length, const MacAddress& destination) = 0;
    
    /** Get local network interface MAC address */
    virtual MacAddress getLocalMacAddress() const = 0;
    
    /** Check if network interface is ready for communication */
    virtual bool isReady() const = 0;
    
    /** Get maximum transmission unit size */
    virtual size_t getMTU() const { return 1500; }
};

/**
 * @brief Entity model delegate for application-specific entity behavior
 * @details Applications implement this interface to handle entity-specific decisions
 */
class IEntityModelDelegate {
public:
    virtual ~IEntityModelDelegate() = default;
    
    // Entity discovery callbacks
    virtual void onEntityDiscovered(const DiscoveredEntity& entity) = 0;
    virtual void onEntityDeparted(EntityID entityID) = 0;
    virtual void onEntityUpdated(const DiscoveredEntity& entity) = 0;
    
    // Stream connection callbacks (for listeners/talkers)
    virtual void onStreamConnectionRequest(const ACMPConnectTxCommand& request, ACMPConnectTxResponse& response) = 0;
    virtual void onStreamDisconnectionRequest(const ACMPDisconnectTxCommand& request, ACMPDisconnectTxResponse& response) = 0;
    
    // Entity model queries (for entities)
    virtual void onDescriptorReadRequest(DescriptorType descriptorType, uint16_t descriptorIndex, 
                                        AemCommandStatus& status, void* descriptorData, size_t& descriptorSize) = 0;
    virtual void onConfigurationChangeRequest(uint16_t configurationIndex, AemCommandStatus& status) = 0;
    virtual void onStreamFormatChangeRequest(DescriptorType descriptorType, uint16_t descriptorIndex, 
                                            uint64_t streamFormat, AemCommandStatus& status) = 0;
};

/**
 * @brief Protocol message serialization interface (internal library functionality)
 * @details The library implements all IEEE 1722.1-2021 PDU serialization/deserialization
 */
class ProtocolMessageSerializer {
public:
    // ADP Message serialization
    static size_t serializeADPMessage(const ADPMessage& message, uint8_t* buffer, size_t bufferSize);
    static bool deserializeADPMessage(const uint8_t* data, size_t length, ADPMessage& message);
    
    // ACMP Message serialization  
    static size_t serializeACMPMessage(const ACMPMessage& message, uint8_t* buffer, size_t bufferSize);
    static bool deserializeACMPMessage(const uint8_t* data, size_t length, ACMPMessage& message);
    
    // AECP Message serialization
    static size_t serializeAECPMessage(const AECPMessage& message, uint8_t* buffer, size_t bufferSize);
    static bool deserializeAECPMessage(const uint8_t* data, size_t length, AECPMessage& message);
};

// ============================================================================
// AVDECC LIBRARY PUBLIC API
// ============================================================================

/**
 * @brief Main IEEE 1722.1-2021 Library Interface
 * @details High-level API for AVDECC applications
 */
class AVDECCLibrary {
public:
    /** Library event callbacks */
    class Delegate {
    public:
        virtual ~Delegate() = default;
        
        // High-level discovery events
        virtual void onEntityDiscovered(const DiscoveredEntity& entity) = 0;
        virtual void onEntityDeparted(EntityID entityID) = 0;
        
        // High-level connection events
        virtual void onStreamConnected(EntityID talkerEntityID, uint16_t talkerUniqueID,
                                      EntityID listenerEntityID, uint16_t listenerUniqueID) = 0;
        virtual void onStreamDisconnected(EntityID talkerEntityID, uint16_t talkerUniqueID,
                                         EntityID listenerEntityID, uint16_t listenerUniqueID) = 0;
        
        // High-level control events
        virtual void onDescriptorChanged(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex) = 0;
        virtual void onConfigurationChanged(EntityID entityID, uint16_t configurationIndex) = 0;
    };

    /** Constructor requires application-provided interfaces */
    AVDECCLibrary(std::unique_ptr<INetworkInterface> networkInterface,
                  std::unique_ptr<IEntityModelDelegate> entityDelegate,
                  std::unique_ptr<Delegate> libraryDelegate = nullptr);
    
    /** Destructor automatically stops all operations */
    ~AVDECCLibrary();
    
    // Library lifecycle
    bool start();
    void stop();
    bool isRunning() const;
    
    // High-level discovery operations
    void discoverAllEntities();
    void discoverEntity(EntityID entityID);
    std::vector<DiscoveredEntity> getDiscoveredEntities() const;
    
    // High-level connection operations
    bool connectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                      EntityID listenerEntityID, uint16_t listenerUniqueID);
    bool disconnectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                         EntityID listenerEntityID, uint16_t listenerUniqueID);
    
    // High-level control operations
    bool readDescriptor(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex,
                       void* descriptorData, size_t& descriptorSize);
    bool setConfiguration(EntityID entityID, uint16_t configurationIndex);
    bool setStreamFormat(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex, uint64_t streamFormat);
    
    // Configuration
    void setDiscoveryInterval(std::chrono::milliseconds interval);
    void setEntityTimeout(std::chrono::seconds timeout);
    void setCommandTimeout(std::chrono::milliseconds timeout);

private:
    // Implementation details hidden from applications
    class Implementation;
    std::unique_ptr<Implementation> _impl;
};

// ============================================================================
// CONVENIENCE FACTORY FUNCTIONS
// ============================================================================

/**
 * @brief Create AVDECC library with common configurations
 */
namespace LibraryFactory {
    
    /** Create library for controller applications */
    std::unique_ptr<AVDECCLibrary> createController(
        std::unique_ptr<INetworkInterface> networkInterface,
        std::unique_ptr<AVDECCLibrary::Delegate> delegate);
    
    /** Create library for entity/device applications */
    std::unique_ptr<AVDECCLibrary> createEntity(
        std::unique_ptr<INetworkInterface> networkInterface,
        std::unique_ptr<IEntityModelDelegate> entityDelegate,
        const LocalEntity& localEntity);
    
    /** Create library for mixed controller/entity applications */
    std::unique_ptr<AVDECCLibrary> createMixed(
        std::unique_ptr<INetworkInterface> networkInterface,
        std::unique_ptr<IEntityModelDelegate> entityDelegate,
        std::unique_ptr<AVDECCLibrary::Delegate> delegate,
        const LocalEntity& localEntity);
}

} // namespace StateMachines
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
