/**
 * @file ieee_1722_1_2021_state_machines_internal.h
 * @brief IEEE 1722.1-2021 Internal State Machine Classes (Layer 2)
 * @details Detailed state machine implementations with timers, retries, and state transitions
 * 
 * This module implements the internal state machines that power the high-level library API.
 * Applications should not use these classes directly - use AVDECCLibrary instead.
 * 
 * @author OpenAvnu AVDECC Implementation
 * @date 2025
 * @version 2.0.0
 * 
 * @copyright
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include "ieee_1722_1_2021_library.h"
#include <chrono>
#include <queue>
#include <unordered_map>
#include <atomic>
#include <mutex>

namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace StateMachines {
namespace Internal {

// ============================================================================
// TIMER MANAGEMENT SYSTEM
// ============================================================================

/**
 * @brief Timer callback interface for state machine events
 */
class TimerCallback {
public:
    virtual ~TimerCallback() = default;
    virtual void onTimeout() = 0;
};

/**
 * @brief High-precision timer for state machine timeouts
 * Integrates with OpenAvnu's timing infrastructure
 */
class Timer {
public:
    Timer(TimerCallback* callback, std::chrono::milliseconds interval, bool repeating = false);
    ~Timer();
    
    void start();
    void stop();
    void restart();
    bool isRunning() const { return _running; }
    
    // OpenAvnu integration point for hardware timestamps
    void setGptpTimeSource(std::function<uint64_t()> gptpTimeGetter);
    
    // Allow TimerManager to access private members
    friend class TimerManager;
    
private:
    TimerCallback* _callback;
    std::chrono::milliseconds _interval;
    bool _repeating;
    std::atomic<bool> _running{false};
    std::chrono::steady_clock::time_point _startTime;
    std::function<uint64_t()> _gptpTimeGetter;
};

/**
 * @brief Timer manager for coordinating multiple state machine timers
 */
class TimerManager {
public:
    TimerManager();
    ~TimerManager();
    
    void tick();  // Called by library main loop
    std::shared_ptr<Timer> createTimer(TimerCallback* callback, std::chrono::milliseconds interval, bool repeating = false);
    
private:
    std::vector<std::weak_ptr<Timer>> _timers;
    std::mutex _timersMutex;
};

// ============================================================================
// ADP (DISCOVERY PROTOCOL) STATE MACHINES
// ============================================================================

/**
 * @brief ADP Discovery State Machine (IEEE 1722.1-2021 Clause 6.2.5)
 * Implements entity discovery with proper timeout and retry logic
 */
class ADPDiscoveryStateMachine : public TimerCallback {
public:
    enum class State {
        IDLE,               // No discovery in progress
        DISCOVER,           // Discovery message sent, waiting for responses
        WAITING             // Waiting before next discovery cycle
    };
    
    ADPDiscoveryStateMachine(INetworkInterface* network, TimerManager* timerManager);
    ~ADPDiscoveryStateMachine();
    
    // Public API
    void startGlobalDiscovery();
    void discoverSpecificEntity(EntityID entityID);
    void stop();
    
    // Message processing
    void processAvailableMessage(const ADPEntityAvailableMessage& message);
    void processDepartingMessage(const ADPEntityDepartingMessage& message);
    
    // Event callbacks
    std::function<void(const DiscoveredEntity&)> onEntityDiscovered;
    std::function<void(EntityID)> onEntityDeparted;
    
    // Configuration
    void setDiscoveryInterval(std::chrono::milliseconds interval);
    void setEntityTimeout(std::chrono::seconds timeout);
    
private:
    // TimerCallback implementation
    void onTimeout() override;
    
    // Internal state management
    void setState(State newState);
    void sendDiscoveryMessage(EntityID entityID = 0);  // 0 = global discovery
    void processEntityTimeout();
    void cleanupExpiredEntities();
    
    // State machine data
    INetworkInterface* _network;
    TimerManager* _timerManager;
    State _currentState{State::IDLE};
    std::shared_ptr<Timer> _discoveryTimer;
    std::shared_ptr<Timer> _entityTimeoutTimer;
    
    // Configuration
    std::chrono::milliseconds _discoveryInterval{2000};  // 2 seconds default
    std::chrono::seconds _entityTimeout{62};              // 62 seconds per IEEE standard
    
    // Discovered entities
    std::unordered_map<EntityID, DiscoveredEntity> _discoveredEntities;
    std::mutex _entitiesMutex;
    
    // Statistics
    std::atomic<uint32_t> _discoveryMessagesSent{0};
    std::atomic<uint32_t> _entitiesDiscovered{0};
};

/**
 * @brief ADP Advertising State Machine (IEEE 1722.1-2021 Clause 6.2.4)
 * Implements entity advertising with proper timing and available_index management
 */
class ADPAdvertisingStateMachine : public TimerCallback {
public:
    enum class State {
        IDLE,               // Not advertising
        ADVERTISE,          // Sending periodic advertisements
        DEPARTING           // Sending departure messages
    };
    
    ADPAdvertisingStateMachine(INetworkInterface* network, TimerManager* timerManager);
    ~ADPAdvertisingStateMachine();
    
    // Public API
    void startAdvertising(const LocalEntity& entity);
    void updateEntity(const LocalEntity& entity);
    void stop();
    
    // Configuration
    void setAdvertiseInterval(std::chrono::milliseconds interval);
    
private:
    // TimerCallback implementation
    void onTimeout() override;
    
    // Internal state management
    void setState(State newState);
    void sendAdvertisement();
    void sendDeparture();
    void incrementAvailableIndex();
    
    // State machine data
    INetworkInterface* _network;
    TimerManager* _timerManager;
    State _currentState{State::IDLE};
    std::shared_ptr<Timer> _advertisingTimer;
    
    // Configuration
    std::chrono::milliseconds _advertiseInterval{2000};  // 2 seconds default
    
    // Local entity data
    LocalEntity _localEntity;
    std::atomic<uint32_t> _availableIndex{0};
    std::mutex _entityMutex;
    
    // Statistics
    std::atomic<uint32_t> _advertisementsSent{0};
};

// ============================================================================
// ACMP (CONNECTION MANAGEMENT) STATE MACHINES
// ============================================================================

/**
 * @brief ACMP Controller State Machine (IEEE 1722.1-2021 Clause 8.2.2.1)
 * Implements stream connection management with retry logic and timeout handling
 */
class ACMPControllerStateMachine : public TimerCallback {
public:
    enum class State {
        IDLE,               // No operation in progress
        CONNECT_TX_COMMAND, // Connect command sent, waiting for response
        DISCONNECT_TX_COMMAND, // Disconnect command sent, waiting for response
        GET_TX_STATE_COMMAND,  // Get state command sent, waiting for response
        GET_RX_STATE_COMMAND   // Get RX state command sent, waiting for response
    };
    
    struct PendingCommand {
        uint16_t sequenceID;
        ACMPMessageType commandType;
        EntityID talkerEntityID;
        EntityID listenerEntityID;
        uint16_t talkerUniqueID;
        uint16_t listenerUniqueID;
        std::chrono::steady_clock::time_point sentTime;
        uint8_t retryCount{0};
        std::function<void(bool success, ACMP_Status status)> callback;
    };
    
    ACMPControllerStateMachine(INetworkInterface* network, TimerManager* timerManager);
    ~ACMPControllerStateMachine();
    
    // Public API
    uint16_t connectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                          EntityID listenerEntityID, uint16_t listenerUniqueID,
                          std::function<void(bool, ACMP_Status)> callback);
    
    uint16_t disconnectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                             EntityID listenerEntityID, uint16_t listenerUniqueID,
                             std::function<void(bool, ACMP_Status)> callback);
    
    uint16_t getTalkerState(EntityID talkerEntityID, uint16_t talkerUniqueID,
                           std::function<void(bool, const ATDECCConnectionManagementProtocolPDU&)> callback);
    
    uint16_t getListenerState(EntityID listenerEntityID, uint16_t listenerUniqueID,
                             std::function<void(bool, const ATDECCConnectionManagementProtocolPDU&)> callback);
    
    // Message processing
    void processConnectTxResponse(const ATDECCConnectionManagementProtocolPDU& response);
    void processDisconnectTxResponse(const ATDECCConnectionManagementProtocolPDU& response);
    void processGetTxStateResponse(const ATDECCConnectionManagementProtocolPDU& response);
    void processGetRxStateResponse(const ATDECCConnectionManagementProtocolPDU& response);
    
    // Configuration
    void setCommandTimeout(std::chrono::milliseconds timeout);
    void setMaxRetries(uint8_t maxRetries);
    
private:
    // TimerCallback implementation
    void onTimeout() override;
    
    // Internal command management
    uint16_t sendCommand(ACMPMessageType commandType, EntityID talkerEntityID, EntityID listenerEntityID,
                        uint16_t talkerUniqueID, uint16_t listenerUniqueID,
                        std::function<void(bool, ACMP_Status)> callback);
    
    void processResponse(uint16_t sequenceID, bool success, ACMP_Status status);
    void retryCommand(uint16_t sequenceID);
    void timeoutCommand(uint16_t sequenceID);
    uint16_t generateSequenceID();
    
    // State machine data
    INetworkInterface* _network;
    TimerManager* _timerManager;
    State _currentState{State::IDLE};
    std::shared_ptr<Timer> _commandTimer;
    
    // Configuration
    std::chrono::milliseconds _commandTimeout{500};  // 500ms default
    uint8_t _maxRetries{3};                          // 3 retries default
    
    // Pending commands
    std::unordered_map<uint16_t, PendingCommand> _pendingCommands;
    std::mutex _commandsMutex;
    std::atomic<uint16_t> _nextSequenceID{1};
    
    // Statistics
    std::atomic<uint32_t> _commandsSent{0};
    std::atomic<uint32_t> _responsesReceived{0};
    std::atomic<uint32_t> _timeouts{0};
};

/**
 * @brief ACMP Talker State Machine (IEEE 1722.1-2021 Clause 8.2.2.2)
 * Implements talker stream management with proper state tracking
 */
class ACMPTalkerStateMachine : public TimerCallback {
public:
    enum class State {
        IDLE,               // No active streams
        STREAMING,          // Has active listener connections
        FAST_CONNECT        // Processing fast connect sequence
    };
    
    struct TalkerStreamState {
        uint16_t talkerUniqueID;
        EntityID streamID;
        MacAddress streamDestAddress;
        std::vector<EntityID> connectedListeners;
        uint16_t connectionCount{0};
        StreamInfoFlags streamInfoFlags{StreamInfoFlags::None};
        uint16_t streamVlanID{0};
        bool streamingActive{false};
    };
    
    ACMPTalkerStateMachine(INetworkInterface* network, TimerManager* timerManager);
    ~ACMPTalkerStateMachine();
    
    // Message processing
    void processConnectTxCommand(const ATDECCConnectionManagementProtocolPDU& command);
    void processDisconnectTxCommand(const ATDECCConnectionManagementProtocolPDU& command);
    void processGetTxStateCommand(const ATDECCConnectionManagementProtocolPDU& command);
    
    // Stream management
    void addTalkerStream(uint16_t talkerUniqueID, const TalkerStreamState& streamState);
    void removeTalkerStream(uint16_t talkerUniqueID);
    std::vector<TalkerStreamState> getTalkerStreams() const;
    
    // Event callbacks
    std::function<void(uint16_t talkerUniqueID, EntityID listenerEntityID)> onListenerConnected;
    std::function<void(uint16_t talkerUniqueID, EntityID listenerEntityID)> onListenerDisconnected;
    
private:
    // TimerCallback implementation
    void onTimeout() override;
    
    // Internal stream management
    void sendConnectTxResponse(const ATDECCConnectionManagementProtocolPDU& command, ACMP_Status status);
    void sendDisconnectTxResponse(const ATDECCConnectionManagementProtocolPDU& command, ACMP_Status status);
    void sendGetTxStateResponse(const ATDECCConnectionManagementProtocolPDU& command);
    
    bool validateConnectRequest(const ATDECCConnectionManagementProtocolPDU& command);
    void updateStreamDestination(uint16_t talkerUniqueID);
    
    // State machine data
    INetworkInterface* _network;
    TimerManager* _timerManager;
    State _currentState{State::IDLE};
    
    // Stream states
    std::unordered_map<uint16_t, TalkerStreamState> _talkerStreams;
    std::mutex _streamsMutex;
    
    // Statistics
    std::atomic<uint32_t> _connectionsAccepted{0};
    std::atomic<uint32_t> _connectionsRejected{0};
};

/**
 * @brief ACMP Listener State Machine (IEEE 1722.1-2021 Clause 8.2.2.3)
 * Implements listener stream management with proper connection tracking
 */
class ACMPListenerStateMachine : public TimerCallback {
public:
    enum class State {
        IDLE,               // No active connections
        CONNECTED,          // Connected to talker stream
        FAST_CONNECT        // Processing fast connect sequence
    };
    
    struct ListenerStreamState {
        uint16_t listenerUniqueID;
        EntityID talkerEntityID{0};
        uint16_t talkerUniqueID{0};
        EntityID streamID{0};
        MacAddress streamDestAddress;
        uint16_t connectionCount{0};
        StreamInfoFlags streamInfoFlags{StreamInfoFlags::None};
        uint16_t streamVlanID{0};
        bool streamingActive{false};
    };
    
    ACMPListenerStateMachine(INetworkInterface* network, TimerManager* timerManager);
    ~ACMPListenerStateMachine();
    
    // Message processing
    void processConnectRxCommand(const ATDECCConnectionManagementProtocolPDU& command);
    void processDisconnectRxCommand(const ATDECCConnectionManagementProtocolPDU& command);
    void processGetRxStateCommand(const ATDECCConnectionManagementProtocolPDU& command);
    
    // Stream management
    void addListenerStream(uint16_t listenerUniqueID, const ListenerStreamState& streamState);
    void removeListenerStream(uint16_t listenerUniqueID);
    std::vector<ListenerStreamState> getListenerStreams() const;
    
    // Event callbacks
    std::function<void(uint16_t listenerUniqueID, EntityID talkerEntityID)> onTalkerConnected;
    std::function<void(uint16_t listenerUniqueID, EntityID talkerEntityID)> onTalkerDisconnected;
    
private:
    // TimerCallback implementation
    void onTimeout() override;
    
    // Internal stream management
    void sendConnectRxResponse(const ATDECCConnectionManagementProtocolPDU& command, ACMP_Status status);
    void sendDisconnectRxResponse(const ATDECCConnectionManagementProtocolPDU& command, ACMP_Status status);
    void sendGetRxStateResponse(const ATDECCConnectionManagementProtocolPDU& command);
    
    bool validateConnectRequest(const ATDECCConnectionManagementProtocolPDU& command);
    
    // State machine data
    INetworkInterface* _network;
    TimerManager* _timerManager;
    State _currentState{State::IDLE};
    
    // Stream states
    std::unordered_map<uint16_t, ListenerStreamState> _listenerStreams;
    std::mutex _streamsMutex;
    
    // Statistics
    std::atomic<uint32_t> _connectionsAccepted{0};
    std::atomic<uint32_t> _connectionsRejected{0};
};

// ============================================================================
// AECP (ENTITY CONTROL) STATE MACHINES
// ============================================================================

/**
 * @brief AECP Controller State Machine (IEEE 1722.1-2021 Clause 9.2.2.1)
 * Implements entity control with command queuing and retry logic
 */
class AECPControllerStateMachine : public TimerCallback {
public:
    enum class State {
        IDLE,               // No commands in progress
        COMMAND_SENT,       // Command sent, waiting for response
        UNSOLICITED_RESPONSE // Processing unsolicited response
    };
    
    struct PendingCommand {
        uint16_t sequenceID;
        AemCommandType commandType;
        EntityID targetEntityID;
        std::vector<uint8_t> commandData;
        std::chrono::steady_clock::time_point sentTime;
        uint8_t retryCount{0};
        std::function<void(bool success, AemCommandStatus status, const std::vector<uint8_t>&)> callback;
    };
    
    AECPControllerStateMachine(INetworkInterface* network, TimerManager* timerManager);
    ~AECPControllerStateMachine();
    
    // Public API
    uint16_t readDescriptor(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex,
                           std::function<void(bool, AemCommandStatus, const std::vector<uint8_t>&)> callback);
    
    uint16_t acquireEntity(EntityID entityID, AcquireFlags flags,
                          std::function<void(bool, AemCommandStatus)> callback);
    
    uint16_t lockEntity(EntityID entityID, LockFlags flags,
                       std::function<void(bool, AemCommandStatus)> callback);
    
    uint16_t setConfiguration(EntityID entityID, uint16_t configurationIndex,
                             std::function<void(bool, AemCommandStatus)> callback);
    
    uint16_t setStreamFormat(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex,
                            uint64_t streamFormat, std::function<void(bool, AemCommandStatus)> callback);
    
    // Message processing
    void processAemResponse(const AECPMessage& response);
    void processUnsolicitedResponse(const AECPMessage& response);
    
    // Configuration
    void setCommandTimeout(std::chrono::milliseconds timeout);
    void setMaxRetries(uint8_t maxRetries);
    
private:
    // TimerCallback implementation
    void onTimeout() override;
    
    // Internal command management
    uint16_t sendCommand(AemCommandType commandType, EntityID targetEntityID,
                        const std::vector<uint8_t>& commandData,
                        std::function<void(bool, AemCommandStatus, const std::vector<uint8_t>&)> callback);
    
    void processResponse(uint16_t sequenceID, bool success, AemCommandStatus status, const std::vector<uint8_t>& responseData);
    void retryCommand(uint16_t sequenceID);
    void timeoutCommand(uint16_t sequenceID);
    uint16_t generateSequenceID();
    
    // State machine data
    INetworkInterface* _network;
    TimerManager* _timerManager;
    State _currentState{State::IDLE};
    std::shared_ptr<Timer> _commandTimer;
    
    // Configuration
    std::chrono::milliseconds _commandTimeout{1000};  // 1 second default
    uint8_t _maxRetries{3};                           // 3 retries default
    
    // Pending commands
    std::unordered_map<uint16_t, PendingCommand> _pendingCommands;
    std::mutex _commandsMutex;
    std::atomic<uint16_t> _nextSequenceID{1};
    
    // Statistics
    std::atomic<uint32_t> _commandsSent{0};
    std::atomic<uint32_t> _responsesReceived{0};
    std::atomic<uint32_t> _timeouts{0};
};

/**
 * @brief AECP Entity State Machine (IEEE 1722.1-2021 Clause 9.2.2.2)
 * Implements entity command processing with proper response generation
 */
class AECPEntityStateMachine : public TimerCallback {
public:
    enum class State {
        IDLE,               // Ready to process commands
        PROCESSING_COMMAND, // Processing received command
        SENDING_RESPONSE    // Sending command response
    };
    
    AECPEntityStateMachine(INetworkInterface* network, TimerManager* timerManager, IEntityModelDelegate* entityDelegate);
    ~AECPEntityStateMachine();
    
    // Message processing
    void processAemCommand(const AECPMessage& command);
    
    // Entity model updates (for unsolicited notifications)
    void notifyDescriptorChanged(DescriptorType descriptorType, uint16_t descriptorIndex);
    void notifyConfigurationChanged(uint16_t configurationIndex);
    
private:
    // TimerCallback implementation
    void onTimeout() override;
    
    // Internal command processing
    void sendAemResponse(const AECPMessage& originalCommand, AemCommandStatus status, const std::vector<uint8_t>& responseData);
    void sendUnsolicitedNotification(AemCommandType notificationType, const std::vector<uint8_t>& notificationData);
    
    // Command handlers
    void handleReadDescriptor(const AECPMessage& command);
    void handleAcquireEntity(const AECPMessage& command);
    void handleLockEntity(const AECPMessage& command);
    void handleSetConfiguration(const AECPMessage& command);
    void handleSetStreamFormat(const AECPMessage& command);
    
    // State machine data
    INetworkInterface* _network;
    TimerManager* _timerManager;
    IEntityModelDelegate* _entityDelegate;
    State _currentState{State::IDLE};
    
    // Statistics
    std::atomic<uint32_t> _commandsProcessed{0};
    std::atomic<uint32_t> _responsesSent{0};
    std::atomic<uint32_t> _notificationsSent{0};
};

} // namespace Internal
} // namespace StateMachines
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
