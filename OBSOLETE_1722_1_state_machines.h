/**
 * @file 1722_1_state_machines.h
 * @brief IEEE 1722.1-2021 Protocol State Machines Implementation
 * @details Complete implementation of all AVDECC state machines according to IEEE 1722.1-2021 specification
 *
 * This module provides comprehensive state machine implementations for:
 * - ADP (AVDECC Discovery Protocol) - Entity discovery and advertisement
 * - ACMP (AVDECC Connection Management Protocol) - Stream connection management
 * - AECP (AVDECC Enumeration and Control Protocol) - Entity control and enumeration
 *
 * All state machines follow the precise specifications defined in IEEE 1722.1-2021 clauses 6, 8, and 9.
 * 
 * @author OpenAvnu AVDECC Implementation
 * @date 2025
 * @version 1.0.0
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

// Use legacy unions instead of std::variant for broader compatibility
#define VARIANT_UNION

namespace IEEE {
namespace _1722_1 {
namespace _2021 {

// Forward declarations for state machines
namespace StateMachines {

// Import types from parent namespaces
using namespace IEEE::_1722_1::_2021::AEM;
using namespace IEEE::_1722_1::_2021::ADPDU;
using namespace IEEE::_1722_1::_2021::ACMP;
using namespace IEEE::_1722_1::_2021::AECP;

// Basic type aliases for cleaner code
using EntityID = uint64_t;
using EntityModelID = uint64_t;

// Type aliases for missing types
using AemCommandType = AEM_Command_type;
using AemCommandStatus = AECP_Status;
using ACMPMessageType = ACMP_Message_type;
using DescriptorType = uint16_t; // Descriptor type constants are uint16_t
using AcquireFlags = uint32_t;   // Simple flags for now
using LockFlags = uint32_t;      // Simple flags for now

// Forward declarations
struct ConnectionInfo;
struct MacAddress;
struct DiscoveredEntity;
struct LocalEntity;
struct ADPMessage;
struct ACMPMessage;
struct AECPMessage;
struct ACMPConnectTxCommand;
struct ACMPConnectTxResponse;
struct ACMPDisconnectTxCommand;
struct ACMPDisconnectTxResponse;

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

// Forward declarations
class StateMachineManager;
class NetworkInterface;

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
// MESSAGE TYPE UNIONS (Legacy C++ compatible)
// ============================================================================

// ADP Message Types
enum class ADPMessageType {
    ENTITY_DISCOVER,
    ENTITY_AVAILABLE, 
    ENTITY_DEPARTING
};

// ADP Messages
struct ADPMessage {
    ADPMessageType type;
    // Common data - use the largest message struct
    ADPEntityAvailableMessage data; // Largest of the three message types
    
    ADPMessage() : type(ADPMessageType::ENTITY_DISCOVER) {
        memset(&data, 0, sizeof(data));
    }
    
    explicit ADPMessage(ADPMessageType t) : type(t) {
        memset(&data, 0, sizeof(data));
    }
};

// ACMP Message Types
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
    // Use base ACMP PDU for all message data
    uint8_t pdu_data[64]; // Enough space for ACMP PDU
    
    ACMPMessage() : type(ACMPMessageType_enum::CONNECT_TX_COMMAND) {
        memset(pdu_data, 0, sizeof(pdu_data));
    }
};

// AECP Message Types  
enum class AECPMessageType_enum {
    AEM_COMMAND,
    AEM_RESPONSE
};

struct AECPMessage {
    AECPMessageType_enum type;
    // Use base AECP PDU for all message data
    uint8_t pdu_data[512]; // Enough space for AECP PDU with command data
    
    AECPMessage() : type(AECPMessageType_enum::AEM_COMMAND) {
        memset(pdu_data, 0, sizeof(pdu_data));
    }
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
// PROTOCOL MESSAGE TYPES (Internal Library Structures)
// ============================================================================

/** ADP (Discovery Protocol) Messages - Internal library representation */
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

// Forward declarations
struct ConnectionInfo;

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
// MESSAGE TYPE UNIONS (Legacy C++ compatible)
// ============================================================================

// ============================================================================
// MESSAGE STRUCTURES (Simplified approach without unions/variants)
// ============================================================================

// ADP Message Types
enum class ADPMessageType {
    ENTITY_DISCOVER,
    ENTITY_AVAILABLE, 
    ENTITY_DEPARTING
};

// ADP Messages
struct ADPMessage {
    ADPMessageType type;
    // Common data - use the largest message struct
    ADPEntityAvailableMessage data; // Largest of the three message types
    
    ADPMessage() : type(ADPMessageType::ENTITY_DISCOVER) {
        memset(&data, 0, sizeof(data));
    }
    
    explicit ADPMessage(ADPMessageType t) : type(t) {
        memset(&data, 0, sizeof(data));
    }
};

// ACMP Message Types
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
    // Use base ACMP PDU for all message data
    uint8_t pdu_data[64]; // Enough space for ACMP PDU
    
    ACMPMessage() : type(ACMPMessageType_enum::CONNECT_TX_COMMAND) {
        memset(pdu_data, 0, sizeof(pdu_data));
    }
};

// AECP Message Types  
enum class AECPMessageType_enum {
    AEM_COMMAND,
    AEM_RESPONSE
};

struct AECPMessage {
    AECPMessageType_enum type;
    // Use base AECP PDU for all message data
    uint8_t pdu_data[512]; // Enough space for AECP PDU with command data
    
    AECPMessage() : type(AECPMessageType_enum::AEM_COMMAND) {
        memset(pdu_data, 0, sizeof(pdu_data));
    }
};

// Protocol Message (simple struct instead of variant)
enum class ProtocolMessageType {
    ADP,
    ACMP,
    AECP
};

struct ProtocolMessage {
    ProtocolMessageType type;
    // Use raw bytes to avoid union constructor issues
    uint8_t message_data[512]; // Large enough for any message type
    
    ProtocolMessage() : type(ProtocolMessageType::ADP) {
        memset(message_data, 0, sizeof(message_data));
    }
    
    // Helper methods to get typed access
    ADPMessage* as_adp() { return reinterpret_cast<ADPMessage*>(message_data); }
    const ADPMessage* as_adp() const { return reinterpret_cast<const ADPMessage*>(message_data); }
    
    ACMPMessage* as_acmp() { return reinterpret_cast<ACMPMessage*>(message_data); }
    const ACMPMessage* as_acmp() const { return reinterpret_cast<const ACMPMessage*>(message_data); }
    
    AECPMessage* as_aecp() { return reinterpret_cast<AECPMessage*>(message_data); }
    const AECPMessage* as_aecp() const { return reinterpret_cast<const AECPMessage*>(message_data); }
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

struct ACMPGetTxStateCommand {
    uint16_t sequenceID;
    EntityID talkerEntityID;
    uint16_t talkerUniqueID;
};

struct ACMPGetTxStateResponse {
    uint16_t sequenceID;
    ACMP_Status status;
    EntityID talkerEntityID;
    uint16_t talkerUniqueID;
    StreamInfo streamInfo;
};

struct ACMPGetTxConnectionCommand {
    uint16_t sequenceID;
    EntityID talkerEntityID;
    uint16_t talkerUniqueID;
    uint16_t connectionIndex;
};

struct ACMPGetTxConnectionResponse {
    uint16_t sequenceID;
    ACMP_Status status;
    EntityID talkerEntityID;
    uint16_t talkerUniqueID;
    uint16_t connectionIndex;
    ConnectionInfo connectionInfo;
};

// Similar structures for RX commands...
using ACMPConnectRxCommand = ACMPConnectTxCommand;
using ACMPConnectRxResponse = ACMPConnectTxResponse;
using ACMPDisconnectRxCommand = ACMPDisconnectTxCommand;
using ACMPDisconnectRxResponse = ACMPDisconnectTxResponse;
using ACMPGetRxStateCommand = ACMPGetTxStateCommand;
using ACMPGetRxStateResponse = ACMPGetTxStateResponse;

// ============================================================================
// LEGACY MESSAGE STRUCTURES (simplified - remove if not needed)
// ============================================================================

/** AECP (Entity Control Protocol) Messages - Legacy structures */
struct AECPAemCommandMessage {
    uint16_t sequenceID;
    EntityID targetEntityID;
    EntityID controllerEntityID;
    AemCommandType commandType;
    uint8_t commandData[256]; // Fixed size instead of vector
    uint8_t sourceMac[6];     // Fixed size instead of MacAddress
    std::chrono::steady_clock::time_point timestamp;
};

struct AECPAemResponseMessage {
    uint16_t sequenceID;
    EntityID targetEntityID;
    EntityID controllerEntityID;
    AemCommandType commandType;
    AemCommandStatus status;
    uint8_t responseData[256]; // Fixed size instead of vector
    uint8_t sourceMac[6];      // Fixed size instead of MacAddress
    std::chrono::steady_clock::time_point timestamp;
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

/** Interface Information */
struct InterfaceInformation {
    MacAddress macAddress;
    uint8_t validTime;
    uint32_t availableIndex;
    EntityID gptpGrandmasterID;
    uint8_t gptpDomainNumber;
};

/** Stream Connection Information */
struct StreamConnectionInfo {
    EntityID talkerEntityID;
    EntityID listenerEntityID;
    uint16_t talkerUniqueID;
    uint16_t listenerUniqueID;
    MacAddress streamDestAddress;
    ConnectionFlags flags;
    uint16_t streamVlanID;
    uint64_t streamFormat;
};

// ============================================================================
// MAC ADDRESS TYPE (if not already defined elsewhere)
// ============================================================================

// MacAddress is already defined above

// ============================================================================
// STATE MACHINE BASE CLASSES
// ============================================================================

/**
 * @brief Base interface for all AVDECC state machines
 * @details Provides common state machine functionality and lifecycle management
 */
class IStateMachine {
public:
    virtual ~IStateMachine() = default;
    
    /** Start the state machine */
    virtual void start() = 0;
    
    /** Stop the state machine */
    virtual void stop() = 0;
    
    /** Process periodic tasks (called by manager) */
    virtual void tick(std::chrono::steady_clock::time_point now) = 0;
    
    /** Check if state machine is running */
    virtual bool isRunning() const = 0;
    
    /** Get state machine name for debugging */
    virtual const char* getName() const = 0;
};

/**
 * @brief Base state machine implementation with common functionality
 */
class StateMachineBase : public IStateMachine {
protected:
    std::atomic<bool> _running{false};
    StateMachineManager* _manager;
    
public:
    explicit StateMachineBase(StateMachineManager* manager) : _manager(manager) {}
    
    void start() override {
        _running = true;
        onStart();
    }
    
    void stop() override {
        _running = false;
        onStop();
    }
    
    bool isRunning() const override {
        return _running;
    }

protected:
    virtual void onStart() {}
    virtual void onStop() {}
};

// ============================================================================
// ADP (AVDECC DISCOVERY PROTOCOL) STATE MACHINES
// ============================================================================

/**
 * @brief ADP Discovery State Machine (IEEE 1722.1-2021 clause 6.2.6)
 * @details Handles discovery of remote AVDECC entities through ADP messages
 * 
 * States:
 * - WAITING: Initial state, waiting for discovery triggers
 * - DISCOVER: Sending ENTITY_DISCOVER messages  
 * - DISCOVERED: Processing ENTITY_AVAILABLE messages
 */
class ADPDiscoveryStateMachine : public StateMachineBase {
public:
    enum class State {
        WAITING,
        DISCOVER,
        DISCOVERED
    };
    
    /** Discovery event delegate */
    class Delegate {
    public:
        virtual ~Delegate() = default;
        virtual void onEntityDiscovered(const DiscoveredEntity& entity) = 0;
        virtual void onEntityTimeout(EntityID entityID) = 0;
        virtual void onEntityUpdated(const DiscoveredEntity& entity) = 0;
    };

    explicit ADPDiscoveryStateMachine(StateMachineManager* manager, Delegate* delegate);
    ~ADPDiscoveryStateMachine();
    
    // IStateMachine interface
    void tick(std::chrono::steady_clock::time_point now) override;
    const char* getName() const override { return "ADP Discovery"; }
    
    // Discovery operations
    void discoverAllEntities();
    void discoverEntity(EntityID entityID);
    void processEntityAvailable(const ADPEntityAvailableMessage& message);
    void processEntityDeparting(const ADPEntityDepartingMessage& message);
    
    // Configuration
    void setDiscoveryInterval(std::chrono::milliseconds interval);
    void setEntityTimeout(std::chrono::seconds timeout);

private:
    struct DiscoveredEntityInfo {
        DiscoveredEntity entity;
        std::chrono::steady_clock::time_point lastSeen;
        std::chrono::steady_clock::time_point timeout;
        uint32_t availableIndex;
    };
    
    State _state{State::WAITING};
    Delegate* _delegate;
    
    // Discovery configuration
    std::chrono::milliseconds _discoveryInterval{2000};  // 2 seconds default
    std::chrono::seconds _entityTimeout{10};             // 10 seconds default
    
    // Discovery timing
    std::chrono::steady_clock::time_point _lastDiscovery;
    std::chrono::steady_clock::time_point _nextDiscovery;
    
    // Discovered entities tracking
    std::unordered_map<EntityID, DiscoveredEntityInfo> _discoveredEntities;
    mutable std::mutex _entitiesMutex;
    
    // Internal methods
    void sendEntityDiscover(EntityID targetEntityID = EntityID{0});
    void updateEntityTimeout(EntityID entityID);
    void removeTimedOutEntities(std::chrono::steady_clock::time_point now);
    void transitionToState(State newState);
    
    // ADP message creation
    ADPEntityDiscoverMessage createDiscoverMessage(EntityID targetEntityID);
};

/**
 * @brief ADP Advertising Entity State Machine (IEEE 1722.1-2021 clause 6.2.4)
 * @details Handles advertisement of local AVDECC entity presence
 * 
 * States:
 * - INITIALIZE: Setting up entity advertisement
 * - ADVERTISE: Periodically sending ENTITY_AVAILABLE messages
 * - DEPARTING: Sending ENTITY_DEPARTING message before shutdown
 */
class ADPAdvertisingEntityStateMachine : public StateMachineBase {
public:
    enum class State {
        INITIALIZE,
        ADVERTISE,
        DEPARTING
    };
    
    explicit ADPAdvertisingEntityStateMachine(StateMachineManager* manager, 
                                              const LocalEntity& entity);
    ~ADPAdvertisingEntityStateMachine();
    
    // IStateMachine interface
    void tick(std::chrono::steady_clock::time_point now) override;
    const char* getName() const override { return "ADP Advertising Entity"; }
    
    // Entity management
    void updateEntityInformation(const LocalEntity& entity);
    void setValidTime(uint8_t validTime);
    void forceAdvertisement();

protected:
    void onStop() override;

private:
    State _state{State::INITIALIZE};
    LocalEntity _entity;
    
    // Advertisement timing
    uint8_t _validTime{2};  // 2 seconds default
    std::chrono::milliseconds _advertiseInterval;
    std::chrono::steady_clock::time_point _nextAdvertisement;
    uint32_t _availableIndex{0};
    
    // Internal methods
    void sendEntityAvailable();
    void sendEntityDeparting();
    void transitionToState(State newState);
    void calculateAdvertiseInterval();
    
    // ADP message creation
    ADPEntityAvailableMessage createAvailableMessage();
    ADPEntityDepartingMessage createDepartingMessage();
};

/**
 * @brief ADP Advertising Interface State Machine (IEEE 1722.1-2021 clause 6.2.5)
 * @details Handles per-interface advertisement for multi-interface entities
 */
class ADPAdvertisingInterfaceStateMachine : public StateMachineBase {
public:
    explicit ADPAdvertisingInterfaceStateMachine(StateMachineManager* manager,
                                                 const LocalEntity& entity,
                                                 uint16_t interfaceIndex);
    
    // IStateMachine interface
    void tick(std::chrono::steady_clock::time_point now) override;
    const char* getName() const override { return "ADP Advertising Interface"; }
    
    // Interface management
    void updateInterfaceInformation(const InterfaceInformation& interfaceInfo);

private:
    LocalEntity _entity;
    uint16_t _interfaceIndex;
    InterfaceInformation _interfaceInfo;
    std::chrono::steady_clock::time_point _nextAdvertisement;
    uint32_t _availableIndex{0};
    
    void sendInterfaceAdvertisement();
};

// ============================================================================
// ACMP (AVDECC CONNECTION MANAGEMENT PROTOCOL) STATE MACHINES
// ============================================================================

/**
 * @brief ACMP Controller State Machine (IEEE 1722.1-2021 clause 8.2.3)
 * @details Handles stream connection management from controller perspective
 * 
 * States:
 * - WAITING: Ready to process connection commands
 * - CONNECT: Processing CONNECT_TX_COMMAND
 * - DISCONNECT: Processing DISCONNECT_TX_COMMAND  
 * - GET_TX_STATE: Processing GET_TX_STATE_COMMAND
 * - GET_TX_CONNECTION: Processing GET_TX_CONNECTION_COMMAND
 */
class ACMPControllerStateMachine : public StateMachineBase {
public:
    enum class State {
        WAITING,
        CONNECT,
        DISCONNECT,
        GET_TX_STATE,
        GET_TX_CONNECTION
    };
    
    /** ACMP Controller event delegate */
    class Delegate {
    public:
        virtual ~Delegate() = default;
        virtual void onConnectResponse(uint16_t sequenceID, ACMP_Status status, const StreamConnectionInfo& info) = 0;
        virtual void onDisconnectResponse(uint16_t sequenceID, ACMP_Status status) = 0;
        virtual void onGetTxStateResponse(uint16_t sequenceID, ACMP_Status status, const StreamInfo& info) = 0;
        virtual void onGetTxConnectionResponse(uint16_t sequenceID, ACMP_Status status, const ConnectionInfo& info) = 0;
    };

    explicit ACMPControllerStateMachine(StateMachineManager* manager, Delegate* delegate);
    ~ACMPControllerStateMachine();
    
    // IStateMachine interface
    void tick(std::chrono::steady_clock::time_point now) override;
    const char* getName() const override { return "ACMP Controller"; }
    
    // Connection management
    uint16_t connectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                          EntityID listenerEntityID, uint16_t listenerUniqueID,
                          const ConnectionFlags& flags = ConnectionFlags{});
    uint16_t disconnectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                             EntityID listenerEntityID, uint16_t listenerUniqueID);
    uint16_t getTxState(EntityID talkerEntityID, uint16_t talkerUniqueID);
    uint16_t getTxConnection(EntityID talkerEntityID, uint16_t talkerUniqueID, uint16_t connectionIndex);
    
    // Response processing
    void processConnectTxResponse(const ACMPConnectTxResponse& response);
    void processDisconnectTxResponse(const ACMPDisconnectTxResponse& response);
    void processGetTxStateResponse(const ACMPGetTxStateResponse& response);
    void processGetTxConnectionResponse(const ACMPGetTxConnectionResponse& response);

private:
    struct InflightCommand {
        uint16_t sequenceID;
        ACMPMessageType messageType;
        std::chrono::steady_clock::time_point timeout;
        uint8_t retryCount;
        
        // Command-specific data
        EntityID talkerEntityID;
        uint16_t talkerUniqueID;
        EntityID listenerEntityID;
        uint16_t listenerUniqueID;
        ConnectionFlags flags;
        uint16_t connectionIndex;
    };
    
    State _state{State::WAITING};
    Delegate* _delegate;
    
    // Command tracking
    uint16_t _sequenceID{1};
    std::queue<InflightCommand> _inflightCommands;
    std::chrono::milliseconds _commandTimeout{1000};  // 1 second
    uint8_t _maxRetries{3};
    
    // Internal methods
    uint16_t sendCommand(ACMPMessageType messageType, 
                        EntityID talkerEntityID, uint16_t talkerUniqueID,
                        EntityID listenerEntityID, uint16_t listenerUniqueID,
                        const ConnectionFlags& flags = ConnectionFlags{},
                        uint16_t connectionIndex = 0);
    void processCommandTimeout(std::chrono::steady_clock::time_point now);
    void transitionToState(State newState);
    uint16_t getNextSequenceID();
    
    // ACMP message creation
    ACMPConnectTxCommand createConnectCommand(const InflightCommand& cmd);
    ACMPDisconnectTxCommand createDisconnectCommand(const InflightCommand& cmd);
    ACMPGetTxStateCommand createGetTxStateCommand(const InflightCommand& cmd);
    ACMPGetTxConnectionCommand createGetTxConnectionCommand(const InflightCommand& cmd);
};

/**
 * @brief ACMP Listener State Machine (IEEE 1722.1-2021 clause 8.2.4)
 * @details Handles stream connection requests from listener perspective
 * 
 * States:
 * - WAITING: Ready to receive connection commands
 * - CONNECT_RX_COMMAND: Processing incoming connect request
 * - DISCONNECT_RX_COMMAND: Processing incoming disconnect request
 * - GET_RX_STATE_COMMAND: Processing state query request
 */
class ACMPListenerStateMachine : public StateMachineBase {
public:
    enum class State {
        WAITING,
        CONNECT_RX_COMMAND,
        DISCONNECT_RX_COMMAND,
        GET_RX_STATE_COMMAND
    };
    
    /** ACMP Listener event delegate */
    class Delegate {
    public:
        virtual ~Delegate() = default;
        virtual void onConnectRequest(const ACMPConnectRxCommand& command, ACMP_Status& status, StreamConnectionInfo& info) = 0;
        virtual void onDisconnectRequest(const ACMPDisconnectRxCommand& command, ACMP_Status& status) = 0;
        virtual void onGetRxStateRequest(const ACMPGetRxStateCommand& command, ACMP_Status& status, StreamInfo& info) = 0;
    };

    explicit ACMPListenerStateMachine(StateMachineManager* manager, 
                                      const LocalEntity& entity, 
                                      Delegate* delegate);
    ~ACMPListenerStateMachine();
    
    // IStateMachine interface
    void tick(std::chrono::steady_clock::time_point now) override;
    const char* getName() const override { return "ACMP Listener"; }
    
    // Command processing
    void processConnectRxCommand(const ACMPConnectRxCommand& command);
    void processDisconnectRxCommand(const ACMPDisconnectRxCommand& command);
    void processGetRxStateCommand(const ACMPGetRxStateCommand& command);

private:
    State _state{State::WAITING};
    LocalEntity _entity;
    Delegate* _delegate;
    
    // Stream state tracking
    std::unordered_map<uint16_t, StreamConnectionInfo> _activeConnections;  // listener_unique_id -> connection
    mutable std::mutex _connectionsMutex;
    
    // Internal methods
    void transitionToState(State newState);
    void sendConnectRxResponse(const ACMPConnectRxCommand& command, ACMP_Status status, 
                              const StreamConnectionInfo& info);
    void sendDisconnectRxResponse(const ACMPDisconnectRxCommand& command, ACMP_Status status);
    void sendGetRxStateResponse(const ACMPGetRxStateCommand& command, ACMP_Status status,
                               const StreamInfo& info);
    
    // Response creation
    ACMPConnectRxResponse createConnectResponse(const ACMPConnectRxCommand& command, 
                                               ACMP_Status status, const StreamConnectionInfo& info);
    ACMPDisconnectRxResponse createDisconnectResponse(const ACMPDisconnectRxCommand& command, 
                                                     ACMP_Status status);
    ACMPGetRxStateResponse createGetRxStateResponse(const ACMPGetRxStateCommand& command,
                                                   ACMP_Status status, const StreamInfo& info);
};

/**
 * @brief ACMP Talker State Machine (IEEE 1722.1-2021 clause 8.2.5)
 * @details Handles stream connection requests from talker perspective
 * 
 * States:
 * - WAITING: Ready to receive connection commands
 * - CONNECT_TX_COMMAND: Processing incoming connect request
 * - DISCONNECT_TX_COMMAND: Processing incoming disconnect request
 * - GET_TX_STATE_COMMAND: Processing state query request
 * - GET_TX_CONNECTION_COMMAND: Processing connection query request
 */
class ACMPTalkerStateMachine : public StateMachineBase {
public:
    enum class State {
        WAITING,
        CONNECT_TX_COMMAND,
        DISCONNECT_TX_COMMAND,
        GET_TX_STATE_COMMAND,
        GET_TX_CONNECTION_COMMAND
    };
    
    /** ACMP Talker event delegate */
    class Delegate {
    public:
        virtual ~Delegate() = default;
        virtual void onConnectRequest(const ACMPConnectTxCommand& command, ACMP_Status& status, StreamConnectionInfo& info) = 0;
        virtual void onDisconnectRequest(const ACMPDisconnectTxCommand& command, ACMP_Status& status) = 0;
        virtual void onGetTxStateRequest(const ACMPGetTxStateCommand& command, ACMP_Status& status, StreamInfo& info) = 0;
        virtual void onGetTxConnectionRequest(const ACMPGetTxConnectionCommand& command, ACMP_Status& status, ConnectionInfo& info) = 0;
    };

    explicit ACMPTalkerStateMachine(StateMachineManager* manager, 
                                    const LocalEntity& entity, 
                                    Delegate* delegate);
    ~ACMPTalkerStateMachine();
    
    // IStateMachine interface
    void tick(std::chrono::steady_clock::time_point now) override;
    const char* getName() const override { return "ACMP Talker"; }
    
    // Command processing
    void processConnectTxCommand(const ACMPConnectTxCommand& command);
    void processDisconnectTxCommand(const ACMPDisconnectTxCommand& command);
    void processGetTxStateCommand(const ACMPGetTxStateCommand& command);
    void processGetTxConnectionCommand(const ACMPGetTxConnectionCommand& command);

private:
    State _state{State::WAITING};
    LocalEntity _entity;
    Delegate* _delegate;
    
    // Stream state tracking
    std::unordered_map<uint16_t, std::vector<StreamConnectionInfo>> _talkerConnections;  // talker_unique_id -> connections
    mutable std::mutex _connectionsMutex;
    
    // Internal methods
    void transitionToState(State newState);
    void sendConnectTxResponse(const ACMPConnectTxCommand& command, ACMP_Status status,
                              const StreamConnectionInfo& info);
    void sendDisconnectTxResponse(const ACMPDisconnectTxCommand& command, ACMP_Status status);
    void sendGetTxStateResponse(const ACMPGetTxStateCommand& command, ACMP_Status status,
                               const StreamInfo& info);
    void sendGetTxConnectionResponse(const ACMPGetTxConnectionCommand& command, ACMP_Status status,
                                    const ConnectionInfo& info);
    
    // Response creation methods...
};

// ============================================================================
// AECP (AVDECC ENUMERATION AND CONTROL PROTOCOL) STATE MACHINES
// ============================================================================

/**
 * @brief AECP Entity Model Entity State Machine (IEEE 1722.1-2021 clause 9.3.5)
 * @details Handles AEM command processing for entity operations
 * 
 * States:
 * - WAITING: Ready to receive AEM commands
 * - RECEIVED_COMMAND: Processing incoming AEM command
 * - RECEIVED_UNSOLICITED_RESPONSE: Processing unsolicited response
 */
class AECPEntityModelEntityStateMachine : public StateMachineBase {
public:
    enum class State {
        WAITING,
        RECEIVED_COMMAND,
        RECEIVED_UNSOLICITED_RESPONSE
    };
    
    /** AECP Entity event delegate */
    class Delegate {
    public:
        virtual ~Delegate() = default;
        
        // AEM Command handlers
        virtual void onAcquireEntityCommand(const AcquireEntityCommand& command, AcquireEntityResponse& response) = 0;
        virtual void onLockEntityCommand(const LockEntityCommand& command, LockEntityResponse& response) = 0;
        virtual void onReadDescriptorCommand(const ReadDescriptorCommand& command, ReadDescriptorResponse& response) = 0;
        virtual void onSetConfigurationCommand(const SetConfigurationCommand& command, SetConfigurationResponse& response) = 0;
        virtual void onGetConfigurationCommand(const GetConfigurationCommand& command, GetConfigurationResponse& response) = 0;
        virtual void onSetStreamFormatCommand(const SetStreamFormatCommand& command, SetStreamFormatResponse& response) = 0;
        virtual void onGetStreamFormatCommand(const GetStreamFormatCommand& command, GetStreamFormatResponse& response) = 0;
        virtual void onSetStreamInfoCommand(const SetStreamInfoCommand& command, SetStreamInfoResponse& response) = 0;
        virtual void onGetStreamInfoCommand(const GetStreamInfoCommand& command, GetStreamInfoResponse& response) = 0;
        virtual void onStartStreamingCommand(const StartStreamingCommand& command, StartStreamingResponse& response) = 0;
        virtual void onStopStreamingCommand(const StopStreamingCommand& command, StopStreamingResponse& response) = 0;
        virtual void onGetAvbInfoCommand(const GetAvbInfoCommand& command, GetAvbInfoResponse& response) = 0;
        virtual void onGetAudioMapCommand(const GetAudioMapCommand& command, GetAudioMapResponse& response) = 0;
        virtual void onRegisterUnsolicitedNotificationCommand(const RegisterUnsolicitedNotificationCommand& command, RegisterUnsolicitedNotificationResponse& response) = 0;
    };

    explicit AECPEntityModelEntityStateMachine(StateMachineManager* manager,
                                               const LocalEntity& entity,
                                               Delegate* delegate);
    ~AECPEntityModelEntityStateMachine();
    
    // IStateMachine interface
    void tick(std::chrono::steady_clock::time_point now) override;
    const char* getName() const override { return "AECP Entity Model Entity"; }
    
    // Command processing
    void processAemCommand(const AECPAemCommandMessage& command);
    void sendUnsolicitedResponse(const AECPAemResponseMessage& response);

private:
    State _state{State::WAITING};
    LocalEntity _entity;
    Delegate* _delegate;
    
    // Entity state
    EntityDynamicState _dynamicState;
    mutable std::mutex _stateMutex;
    
    // Unsolicited notification tracking
    std::vector<EntityID> _unsolicitedSubscribers;
    mutable std::mutex _subscribersMutex;
    
    // Internal methods
    void transitionToState(State newState);
    void sendAemResponse(const AECPAemCommandMessage& command, AemCommandStatus status,
                        const void* responseData = nullptr, size_t responseDataSize = 0);
    void processCommandByType(const AECPAemCommandMessage& command);
    
    // Command dispatch
    void handleAcquireEntity(const AECPAemCommandMessage& command);
    void handleLockEntity(const AECPAemCommandMessage& command);
    void handleReadDescriptor(const AECPAemCommandMessage& command);
    void handleSetConfiguration(const AECPAemCommandMessage& command);
    void handleGetConfiguration(const AECPAemCommandMessage& command);
    void handleSetStreamFormat(const AECPAemCommandMessage& command);
    void handleGetStreamFormat(const AECPAemCommandMessage& command);
    void handleSetStreamInfo(const AECPAemCommandMessage& command);
    void handleGetStreamInfo(const AECPAemCommandMessage& command);
    void handleStartStreaming(const AECPAemCommandMessage& command);
    void handleStopStreaming(const AECPAemCommandMessage& command);
    void handleGetAvbInfo(const AECPAemCommandMessage& command);
    void handleGetAudioMap(const AECPAemCommandMessage& command);
    void handleRegisterUnsolicitedNotification(const AECPAemCommandMessage& command);
};

/**
 * @brief AECP Entity Model Controller State Machine (IEEE 1722.1-2021 clause 9.3.6)
 * @details Handles AEM command sending and response processing for controller operations
 * 
 * States:
 * - WAITING: Ready to send commands
 * - SEND_COMMAND: Sending AEM command and waiting for response
 * - RECEIVED_RESPONSE: Processing AEM response
 * - RECEIVED_UNSOLICITED: Processing unsolicited notification
 */
class AECPEntityModelControllerStateMachine : public StateMachineBase {
public:
    enum class State {
        WAITING,
        SEND_COMMAND,
        RECEIVED_RESPONSE,
        RECEIVED_UNSOLICITED
    };
    
    /** AECP Controller event delegate */
    class Delegate {
    public:
        virtual ~Delegate() = default;
        
        // AEM Response handlers
        virtual void onAcquireEntityResponse(uint16_t sequenceID, AemCommandStatus status, const AcquireEntityResponse& response) = 0;
        virtual void onLockEntityResponse(uint16_t sequenceID, AemCommandStatus status, const LockEntityResponse& response) = 0;
        virtual void onReadDescriptorResponse(uint16_t sequenceID, AemCommandStatus status, const ReadDescriptorResponse& response) = 0;
        virtual void onSetConfigurationResponse(uint16_t sequenceID, AemCommandStatus status, const SetConfigurationResponse& response) = 0;
        virtual void onGetConfigurationResponse(uint16_t sequenceID, AemCommandStatus status, const GetConfigurationResponse& response) = 0;
        virtual void onSetStreamFormatResponse(uint16_t sequenceID, AemCommandStatus status, const SetStreamFormatResponse& response) = 0;
        virtual void onGetStreamFormatResponse(uint16_t sequenceID, AemCommandStatus status, const GetStreamFormatResponse& response) = 0;
        virtual void onSetStreamInfoResponse(uint16_t sequenceID, AemCommandStatus status, const SetStreamInfoResponse& response) = 0;
        virtual void onGetStreamInfoResponse(uint16_t sequenceID, AemCommandStatus status, const GetStreamInfoResponse& response) = 0;
        virtual void onStartStreamingResponse(uint16_t sequenceID, AemCommandStatus status, const StartStreamingResponse& response) = 0;
        virtual void onStopStreamingResponse(uint16_t sequenceID, AemCommandStatus status, const StopStreamingResponse& response) = 0;
        virtual void onGetAvbInfoResponse(uint16_t sequenceID, AemCommandStatus status, const GetAvbInfoResponse& response) = 0;
        virtual void onGetAudioMapResponse(uint16_t sequenceID, AemCommandStatus status, const GetAudioMapResponse& response) = 0;
        virtual void onRegisterUnsolicitedNotificationResponse(uint16_t sequenceID, AemCommandStatus status, const RegisterUnsolicitedNotificationResponse& response) = 0;
        
        // Unsolicited notifications
        virtual void onUnsolicitedResponse(const AECPAemResponseMessage& response) = 0;
    };

    explicit AECPEntityModelControllerStateMachine(StateMachineManager* manager, Delegate* delegate);
    ~AECPEntityModelControllerStateMachine();
    
    // IStateMachine interface
    void tick(std::chrono::steady_clock::time_point now) override;
    const char* getName() const override { return "AECP Entity Model Controller"; }
    
    // Command operations
    uint16_t acquireEntity(EntityID entityID, AcquireFlags flags = AcquireFlags{});
    uint16_t lockEntity(EntityID entityID, LockFlags flags = LockFlags{});
    uint16_t readDescriptor(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex);
    uint16_t setConfiguration(EntityID entityID, uint16_t configurationIndex);
    uint16_t getConfiguration(EntityID entityID);
    uint16_t setStreamFormat(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex, uint64_t streamFormat);
    uint16_t getStreamFormat(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex);
    uint16_t setStreamInfo(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex, const StreamInfo& streamInfo);
    uint16_t getStreamInfo(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex);
    uint16_t startStreaming(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex);
    uint16_t stopStreaming(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex);
    uint16_t getAvbInfo(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex);
    uint16_t getAudioMap(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex);
    uint16_t registerUnsolicitedNotification(EntityID entityID, bool enable = true);
    
    // Response processing
    void processAemResponse(const AECPAemResponseMessage& response);
    void processUnsolicitedResponse(const AECPAemResponseMessage& response);

private:
    struct InflightAemCommand {
        uint16_t sequenceID;
        AemCommandType commandType;
        EntityID targetEntityID;
        std::chrono::steady_clock::time_point timeout;
        uint8_t retryCount;
        
        // Command-specific data
        DescriptorType descriptorType;
        uint16_t descriptorIndex;
        std::vector<uint8_t> commandData;
    };
    
    State _state{State::WAITING};
    Delegate* _delegate;
    
    // Command tracking
    uint16_t _sequenceID{1};
    std::queue<InflightAemCommand> _inflightCommands;
    std::chrono::milliseconds _commandTimeout{1000};  // 1 second
    uint8_t _maxRetries{3};
    
    // Internal methods
    uint16_t sendAemCommand(EntityID entityID, AemCommandType commandType,
                           DescriptorType descriptorType = 0,
                           uint16_t descriptorIndex = 0,
                           const void* commandData = nullptr, size_t commandDataSize = 0);
    void processCommandTimeout(std::chrono::steady_clock::time_point now);
    void transitionToState(State newState);
    uint16_t getNextSequenceID();
    
    // Response dispatch
    void handleResponseByType(const AECPAemResponseMessage& response, const InflightAemCommand& command);
};

// ============================================================================
// STATE MACHINE MANAGER
// ============================================================================

/**
 * @brief Centralized manager for all AVDECC state machines
 * @details Coordinates state machine lifecycle, timing, and message routing
 */
class StateMachineManager {
public:
    /** State machine manager delegate */
    class Delegate {
    public:
        virtual ~Delegate() = default;
        
        // Network operations
        virtual void sendADPMessage(const ADPMessage& message) = 0;
        virtual void sendACMPMessage(const ACMPMessage& message) = 0;
        virtual void sendAECPMessage(const AECPMessage& message) = 0;
        
        // Entity management
        virtual LocalEntity getLocalEntity() const = 0;
        virtual void updateLocalEntity(const LocalEntity& entity) = 0;
    };

    explicit StateMachineManager(Delegate* delegate);
    ~StateMachineManager();
    
    // Lifecycle management
    void start();
    void stop();
    void tick();
    
    // State machine registration
    void registerDiscoveryStateMachine(std::unique_ptr<ADPDiscoveryStateMachine> stateMachine);
    void registerAdvertisingEntityStateMachine(std::unique_ptr<ADPAdvertisingEntityStateMachine> stateMachine);
    void registerACMPControllerStateMachine(std::unique_ptr<ACMPControllerStateMachine> stateMachine);
    void registerACMPListenerStateMachine(std::unique_ptr<ACMPListenerStateMachine> stateMachine);
    void registerACMPTalkerStateMachine(std::unique_ptr<ACMPTalkerStateMachine> stateMachine);
    void registerAECPEntityStateMachine(std::unique_ptr<AECPEntityModelEntityStateMachine> stateMachine);
    void registerAECPControllerStateMachine(std::unique_ptr<AECPEntityModelControllerStateMachine> stateMachine);
    
    // Message routing
    void processADPMessage(const ADPMessage& message);
    void processACMPMessage(const ACMPMessage& message);
    void processAECPMessage(const AECPMessage& message);
    
    // Configuration
    void setTickInterval(std::chrono::milliseconds interval);
    
    // Network interface
    void setNetworkInterface(std::shared_ptr<NetworkInterface> networkInterface);

private:
    Delegate* _delegate;
    std::shared_ptr<NetworkInterface> _networkInterface;
    
    // State machines
    std::vector<std::unique_ptr<IStateMachine>> _stateMachines;
    
    // Timing
    std::chrono::milliseconds _tickInterval{10};  // 10ms default
    std::chrono::steady_clock::time_point _lastTick;
    
    // Threading
    std::atomic<bool> _running{false};
    std::thread _tickThread;
    mutable std::mutex _stateMachinesMutex;
    
    // Internal methods
    void tickThreadLoop();
    void processMessage(const ProtocolMessage& message);
};

// ============================================================================
// NETWORK INTERFACE ABSTRACTION
// ============================================================================

/**
 * @brief Abstract network interface for AVDECC protocol messaging
 */
class NetworkInterface {
public:
    virtual ~NetworkInterface() = default;
    
    /** Send raw AVDECC message */
    virtual bool sendMessage(const uint8_t* data, size_t size, const MacAddress& destination) = 0;
    
    /** Get local MAC address */
    virtual MacAddress getLocalMacAddress() const = 0;
    
    /** Check if interface is ready */
    virtual bool isReady() const = 0;
};

} // namespace StateMachines
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
