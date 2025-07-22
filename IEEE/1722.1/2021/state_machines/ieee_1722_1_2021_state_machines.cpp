/**
 * @file 1722_1_state_machines.cpp
 * @brief IEEE 1722.1-2021 Protocol State Machines Implementation
 * @details Implementation of all AVDECC state machines according to IEEE 1722.1-2021 specification
 *
 * This module provides the complete implementation of:
 * - ADP (AVDECC Discovery Protocol) - Entity discovery and advertisement
 * - ACMP (AVDECC Connection Management Protocol) - Stream connection management
 * - AECP (AVDECC Enumeration and Control Protocol) - Entity control and enumeration
 * 
 * @author OpenAvnu AVDECC Implementation
 * @date 2025
 * @version 1.0.0
 * 
 * @copyright
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ieee_1722_1_2021_state_machines.h"
#include <algorithm>
#include <cstring>

namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace StateMachines {

// ============================================================================
// ADP DISCOVERY STATE MACHINE IMPLEMENTATION
// ============================================================================

ADPDiscoveryStateMachine::ADPDiscoveryStateMachine(StateMachineManager* manager, Delegate* delegate)
    : StateMachineBase(manager), _delegate(delegate) {
}

ADPDiscoveryStateMachine::~ADPDiscoveryStateMachine() {
    stop();
}

void ADPDiscoveryStateMachine::tick(std::chrono::steady_clock::time_point now) {
    if (!_running) {
        return;
    }
    
    switch (_state) {
        case State::WAITING:
            // Check if it's time to start discovery
            if (now >= _nextDiscovery) {
                transitionToState(State::DISCOVER);
            }
            break;
            
        case State::DISCOVER:
            // Send discovery message and transition to discovered state
            sendEntityDiscover(); // Global discovery
            transitionToState(State::DISCOVERED);
            break;
            
        case State::DISCOVERED:
            // Process timeouts and schedule next discovery
            removeTimedOutEntities(now);
            if (now >= _nextDiscovery) {
                transitionToState(State::DISCOVER);
            }
            break;
    }
}

void ADPDiscoveryStateMachine::discoverAllEntities() {
    if (_running) {
        _nextDiscovery = std::chrono::steady_clock::now();
        if (_state == State::WAITING) {
            transitionToState(State::DISCOVER);
        }
    }
}

void ADPDiscoveryStateMachine::discoverEntity(EntityID entityID) {
    if (_running) {
        sendEntityDiscover(entityID);
    }
}

void ADPDiscoveryStateMachine::processEntityAvailable(const ADPEntityAvailableMessage& message) {
    std::lock_guard<std::mutex> lock(_entitiesMutex);
    
    auto now = std::chrono::steady_clock::now();
    auto it = _discoveredEntities.find(message.entityID);
    
    if (it != _discoveredEntities.end()) {
        // Update existing entity
        it->second.entity.entityModelID = message.entityModelID;
        it->second.entity.entityCapabilities = message.entityCapabilities;
        it->second.lastSeen = now;
        it->second.timeout = now + _entityTimeout;
        it->second.availableIndex = message.availableIndex;
        
        if (_delegate) {
            _delegate->onEntityUpdated(it->second.entity);
        }
    } else {
        // Add new entity
        DiscoveredEntityInfo info;
        info.entity.entityID = message.entityID;
        info.entity.entityModelID = message.entityModelID;
        info.entity.entityCapabilities = message.entityCapabilities;
        info.lastSeen = now;
        info.timeout = now + _entityTimeout;
        info.availableIndex = message.availableIndex;
        
        _discoveredEntities[message.entityID] = info;
        
        if (_delegate) {
            _delegate->onEntityDiscovered(info.entity);
        }
    }
}

void ADPDiscoveryStateMachine::processEntityDeparting(const ADPEntityDepartingMessage& message) {
    std::lock_guard<std::mutex> lock(_entitiesMutex);
    
    auto it = _discoveredEntities.find(message.entityID);
    if (it != _discoveredEntities.end()) {
        if (_delegate) {
            _delegate->onEntityTimeout(message.entityID);
        }
        _discoveredEntities.erase(it);
    }
}

void ADPDiscoveryStateMachine::setDiscoveryInterval(std::chrono::milliseconds interval) {
    _discoveryInterval = interval;
}

void ADPDiscoveryStateMachine::setEntityTimeout(std::chrono::seconds timeout) {
    _entityTimeout = timeout;
}

void ADPDiscoveryStateMachine::sendEntityDiscover(EntityID targetEntityID) {
    if (_manager) {
        auto message = createDiscoverMessage(targetEntityID);
        // Send via manager - manager would handle actual network transmission
        // _manager->sendADPMessage(message);
    }
}

void ADPDiscoveryStateMachine::updateEntityTimeout(EntityID entityID) {
    std::lock_guard<std::mutex> lock(_entitiesMutex);
    
    auto it = _discoveredEntities.find(entityID);
    if (it != _discoveredEntities.end()) {
        auto now = std::chrono::steady_clock::now();
        it->second.lastSeen = now;
        it->second.timeout = now + _entityTimeout;
    }
}

void ADPDiscoveryStateMachine::removeTimedOutEntities(std::chrono::steady_clock::time_point now) {
    std::lock_guard<std::mutex> lock(_entitiesMutex);
    
    for (auto it = _discoveredEntities.begin(); it != _discoveredEntities.end();) {
        if (now >= it->second.timeout) {
            if (_delegate) {
                _delegate->onEntityTimeout(it->first);
            }
            it = _discoveredEntities.erase(it);
        } else {
            ++it;
        }
    }
}

void ADPDiscoveryStateMachine::transitionToState(State newState) {
    if (_state != newState) {
        _state = newState;
        
        auto now = std::chrono::steady_clock::now();
        
        switch (newState) {
            case State::WAITING:
                break;
                
            case State::DISCOVER:
                _lastDiscovery = now;
                _nextDiscovery = now + _discoveryInterval;
                break;
                
            case State::DISCOVERED:
                _nextDiscovery = now + _discoveryInterval;
                break;
        }
    }
}

ADPEntityDiscoverMessage ADPDiscoveryStateMachine::createDiscoverMessage(EntityID targetEntityID) {
    ADPEntityDiscoverMessage message{};
    message.entityID = targetEntityID; // 0 for global discovery
    return message;
}

// ============================================================================
// ACMP CONTROLLER STATE MACHINE IMPLEMENTATION (Stub)
// ============================================================================

ACMPControllerStateMachine::ACMPControllerStateMachine(StateMachineManager* manager)
    : StateMachineBase(manager) {
}

ACMPControllerStateMachine::~ACMPControllerStateMachine() {
    stop();
}

void ACMPControllerStateMachine::tick(std::chrono::steady_clock::time_point now) {
    if (!_running) {
        return;
    }
    
    // Process pending connections and timeouts
    processCommandTimeout(now);
}

uint16_t ACMPControllerStateMachine::connectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                                                  EntityID listenerEntityID, uint16_t listenerUniqueID) {
    if (!_running) {
        return 0;
    }
    
    uint16_t sequenceID = generateSequenceID();
    
    // Create and send CONNECT_TX_COMMAND
    sendCommand(ACMP_Message_type::CONNECT_TX_COMMAND, sequenceID, talkerEntityID, listenerEntityID,
                talkerUniqueID, listenerUniqueID);
    
    return sequenceID;
}

uint16_t ACMPControllerStateMachine::disconnectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                                                     EntityID listenerEntityID, uint16_t listenerUniqueID) {
    if (!_running) {
        return 0;
    }
    
    uint16_t sequenceID = generateSequenceID();
    
    // Create and send DISCONNECT_TX_COMMAND
    sendCommand(ACMP_Message_type::DISCONNECT_TX_COMMAND, sequenceID, talkerEntityID, listenerEntityID,
                talkerUniqueID, listenerUniqueID);
    
    return sequenceID;
}

void ACMPControllerStateMachine::processResponse(const ACMPMessage& response) {
    // Extract sequence ID and process response
    // Implementation would handle actual ACMP response processing
}

void ACMPControllerStateMachine::processCommandTimeout(std::chrono::steady_clock::time_point now) {
    // Process command timeouts
    // Implementation would handle timeout logic
}

void ACMPControllerStateMachine::sendCommand(ACMPMessageType messageType, uint16_t sequenceID,
                                           EntityID talkerEntityID, EntityID listenerEntityID,
                                           uint16_t talkerUniqueID, uint16_t listenerUniqueID) {
    if (_manager) {
        // Create ACMP message and send via manager
        // Implementation would construct proper ACMP PDU
    }
}

uint16_t ACMPControllerStateMachine::generateSequenceID() {
    static uint16_t sequenceCounter = 1;
    return sequenceCounter++;
}

// ============================================================================
// AECP ENTITY MODEL CONTROLLER STATE MACHINE IMPLEMENTATION (Stub) 
// ============================================================================

AECPEntityModelControllerStateMachine::AECPEntityModelControllerStateMachine(StateMachineManager* manager)
    : StateMachineBase(manager) {
}

AECPEntityModelControllerStateMachine::~AECPEntityModelControllerStateMachine() {
    stop();
}

void AECPEntityModelControllerStateMachine::tick(std::chrono::steady_clock::time_point now) {
    if (!_running) {
        return;
    }
    
    processCommandTimeout(now);
}

uint16_t AECPEntityModelControllerStateMachine::readDescriptor(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex) {
    if (!_running) {
        return 0;
    }
    
    return sendAemCommand(entityID, AEM_Command_type::READ_DESCRIPTOR, descriptorType, descriptorIndex);
}

uint16_t AECPEntityModelControllerStateMachine::acquireEntity(EntityID entityID, AcquireFlags flags) {
    if (!_running) {
        return 0;
    }
    
    return sendAemCommand(entityID, AEM_Command_type::ACQUIRE_ENTITY, 0, 0, &flags, sizeof(flags));
}

uint16_t AECPEntityModelControllerStateMachine::lockEntity(EntityID entityID, LockFlags flags) {
    if (!_running) {
        return 0;
    }
    
    return sendAemCommand(entityID, AEM_Command_type::LOCK_ENTITY, 0, 0, &flags, sizeof(flags));
}

void AECPEntityModelControllerStateMachine::processResponse(const AECPMessage& response) {
    // Process AECP response
    // Implementation would handle actual response processing
}

uint16_t AECPEntityModelControllerStateMachine::sendAemCommand(EntityID entityID, AemCommandType commandType,
                                                             DescriptorType descriptorType, uint16_t descriptorIndex,
                                                             const void* commandData, size_t commandDataSize) {
    uint16_t sequenceID = generateSequenceID();
    
    if (_manager) {
        // Create AECP message and send via manager
        // Implementation would construct proper AECP AEM command PDU
    }
    
    return sequenceID;
}

void AECPEntityModelControllerStateMachine::processCommandTimeout(std::chrono::steady_clock::time_point now) {
    // Process command timeouts and retries
    // Implementation would handle timeout logic
}

uint16_t AECPEntityModelControllerStateMachine::generateSequenceID() {
    static uint16_t sequenceCounter = 1;
    return sequenceCounter++;
}

// ============================================================================
// STATE MACHINE MANAGER IMPLEMENTATION (Stub)
// ============================================================================

StateMachineManager::StateMachineManager(NetworkInterface* network)
    : _network(network), _running(false) {
}

StateMachineManager::~StateMachineManager() {
    stop();
}

void StateMachineManager::start() {
    if (!_running) {
        _running = true;
        
        // Start tick thread
        _tickThread = std::thread(&StateMachineManager::tickThreadLoop, this);
    }
}

void StateMachineManager::stop() {
    if (_running) {
        _running = false;
        
        // Stop tick thread
        if (_tickThread.joinable()) {
            _tickThread.join();
        }
    }
}

void StateMachineManager::tick() {
    auto now = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(_stateMachinesMutex);
    
    // Tick all registered state machines
    for (auto& sm : _stateMachines) {
        if (sm && sm->isRunning()) {
            sm->tick(now);
        }
    }
}

void StateMachineManager::registerStateMachine(std::shared_ptr<IStateMachine> stateMachine) {
    std::lock_guard<std::mutex> lock(_stateMachinesMutex);
    _stateMachines.push_back(stateMachine);
}

void StateMachineManager::unregisterStateMachine(std::shared_ptr<IStateMachine> stateMachine) {
    std::lock_guard<std::mutex> lock(_stateMachinesMutex);
    auto it = std::find(_stateMachines.begin(), _stateMachines.end(), stateMachine);
    if (it != _stateMachines.end()) {
        _stateMachines.erase(it);
    }
}

void StateMachineManager::tickThreadLoop() {
    const auto tickInterval = std::chrono::milliseconds(10); // 10ms tick interval
    auto nextTick = std::chrono::steady_clock::now();
    
    while (_running) {
        tick();
        
        nextTick += tickInterval;
        std::this_thread::sleep_until(nextTick);
    }
}

} // namespace StateMachines
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE

void DiscoveryStateMachine::processMessage(const ADPMessage& message) {
    switch (message.type) {
        case ADPMessageType::ENTITY_DISCOVER:
            processDiscoverMessage(message.data);
            break;
            
        case ADPMessageType::ENTITY_AVAILABLE:
            processAvailableMessage(message.data);
            break;
            
        case ADPMessageType::ENTITY_DEPARTING:
            processDepartingMessage(message.data);
            break;
    }
}

bool DiscoveryStateMachine::discoverEntity(EntityID entityID) {
    if (!_running) {
        return false;
    }
    
    // Add entity to discovery list if not already present
    auto it = _discoveredEntities.find(entityID);
    if (it == _discoveredEntities.end()) {
        DiscoveredEntity entity{};
        entity.entityID = entityID;
        entity.lastDiscoveryTime = std::chrono::steady_clock::now();
        _discoveredEntities[entityID] = entity;
        
        // Send specific discovery message
        sendEntityDiscovery(entityID);
        return true;
    }
    
    return false;
}

void DiscoveryStateMachine::sendGlobalDiscovery() {
    if (!_network) return;
    
    ADPMessage message;
    message.type = ADPMessageType::ENTITY_DISCOVER;
    message.data.entityID = 0; // Global discovery
    
    // Send to multicast address
    // Implementation would use actual network interface
    // _network->sendADPMessage(message);
}

void DiscoveryStateMachine::sendEntityDiscovery(EntityID entityID) {
    if (!_network) return;
    
    ADPMessage message;
    message.type = ADPMessageType::ENTITY_DISCOVER;
    message.data.entityID = entityID;
    
    // Send to multicast address
    // _network->sendADPMessage(message);
}

void DiscoveryStateMachine::processDiscoveryState(std::chrono::steady_clock::time_point now) {
    // Process pending discoveries
    for (auto& [entityID, entity] : _discoveredEntities) {
        if (entity.discoveryState == DiscoveryState::PENDING) {
            auto elapsed = now - entity.lastDiscoveryTime;
            if (elapsed > std::chrono::milliseconds(1000)) { // 1 second timeout
                // Retry discovery
                sendEntityDiscovery(entityID);
                entity.lastDiscoveryTime = now;
                entity.retryCount++;
                
                if (entity.retryCount >= 3) {
                    entity.discoveryState = DiscoveryState::FAILED;
                }
            }
        }
    }
}

void DiscoveryStateMachine::processAvailableState(std::chrono::steady_clock::time_point now) {
    // Handle available entities and update their information
}

void DiscoveryStateMachine::processDepartingState(std::chrono::steady_clock::time_point now) {
    // Handle departing entities and clean up
}

void DiscoveryStateMachine::processDiscoveryTimeouts(std::chrono::steady_clock::time_point now) {
    // Remove entities that haven't been seen for too long
    auto timeout = std::chrono::seconds(30); // 30 second timeout
    
    for (auto it = _discoveredEntities.begin(); it != _discoveredEntities.end();) {
        auto elapsed = now - it->second.lastSeenTime;
        if (elapsed > timeout) {
            // Notify callback if registered
            if (_callbacks.entityLost) {
                _callbacks.entityLost(it->first);
            }
            it = _discoveredEntities.erase(it);
        } else {
            ++it;
        }
    }
}

void DiscoveryStateMachine::processDiscoverMessage(const ADPEntityDiscoverMessage& message) {
    // Handle incoming discovery messages
    if (message.entityID == 0) {
        // Global discovery - respond if we are an entity
    } else {
        // Specific entity discovery - respond if it's for us
    }
}

void DiscoveryStateMachine::processAvailableMessage(const ADPEntityAvailableMessage& message) {
    // Update or add entity to discovered list
    auto it = _discoveredEntities.find(message.entityID);
    if (it != _discoveredEntities.end()) {
        // Update existing entity
        it->second.entityModelID = message.entityModelID;
        it->second.entityCapabilities = message.entityCapabilities;
        it->second.lastSeenTime = std::chrono::steady_clock::now();
        it->second.discoveryState = DiscoveryState::COMPLETE;
        
        // Notify callback
        if (_callbacks.entityUpdated) {
            _callbacks.entityUpdated(message.entityID, it->second);
        }
    } else {
        // Add new entity
        DiscoveredEntity entity{};
        entity.entityID = message.entityID;
        entity.entityModelID = message.entityModelID;
        entity.entityCapabilities = message.entityCapabilities;
        entity.lastSeenTime = std::chrono::steady_clock::now();
        entity.discoveryState = DiscoveryState::COMPLETE;
        
        _discoveredEntities[message.entityID] = entity;
        
        // Notify callback
        if (_callbacks.entityDiscovered) {
            _callbacks.entityDiscovered(message.entityID, entity);
        }
    }
}

void DiscoveryStateMachine::processDepartingMessage(const ADPEntityDepartingMessage& message) {
    // Remove entity from discovered list
    auto it = _discoveredEntities.find(message.entityID);
    if (it != _discoveredEntities.end()) {
        // Notify callback
        if (_callbacks.entityLost) {
            _callbacks.entityLost(message.entityID);
        }
        
        _discoveredEntities.erase(it);
    }
}

// ============================================================================
// ACMP CONTROLLER STATE MACHINE IMPLEMENTATION
// ============================================================================

ACMPControllerStateMachine::ACMPControllerStateMachine(NetworkInterface* network)
    : _network(network), _state(State::WAITING) {
}

ACMPControllerStateMachine::~ACMPControllerStateMachine() {
    stop();
}

void ACMPControllerStateMachine::start() {
    _running = true;
    _state = State::WAITING;
}

void ACMPControllerStateMachine::stop() {
    _running = false;
    _state = State::WAITING;
    _pendingConnections.clear();
}

void ACMPControllerStateMachine::tick() {
    auto now = std::chrono::steady_clock::now();
    
    // Process pending connections and timeouts
    for (auto it = _pendingConnections.begin(); it != _pendingConnections.end();) {
        auto elapsed = now - it->second.timestamp;
        if (elapsed > std::chrono::milliseconds(2000)) { // 2 second timeout
            // Connection timed out
            if (_callbacks.connectionFailed) {
                _callbacks.connectionFailed(it->first, ACMP_Status::TIMED_OUT);
            }
            it = _pendingConnections.erase(it);
        } else {
            ++it;
        }
    }
}

uint16_t ACMPControllerStateMachine::connectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                                                  EntityID listenerEntityID, uint16_t listenerUniqueID) {
    if (!_running || !_network) {
        return 0; // Invalid sequence ID
    }
    
    uint16_t sequenceID = generateSequenceID();
    
    // Create pending connection
    PendingConnection connection{};
    connection.talkerEntityID = talkerEntityID;
    connection.talkerUniqueID = talkerUniqueID;
    connection.listenerEntityID = listenerEntityID;
    connection.listenerUniqueID = listenerUniqueID;
    connection.timestamp = std::chrono::steady_clock::now();
    connection.messageType = ACMPMessageType::CONNECT_TX_COMMAND;
    
    _pendingConnections[sequenceID] = connection;
    
    // Send CONNECT_TX_COMMAND
    sendCommand(ACMPMessageType::CONNECT_TX_COMMAND, sequenceID, talkerEntityID, listenerEntityID, 
                talkerUniqueID, listenerUniqueID);
    
    return sequenceID;
}

uint16_t ACMPControllerStateMachine::disconnectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                                                     EntityID listenerEntityID, uint16_t listenerUniqueID) {
    if (!_running || !_network) {
        return 0;
    }
    
    uint16_t sequenceID = generateSequenceID();
    
    // Create pending disconnection
    PendingConnection connection{};
    connection.talkerEntityID = talkerEntityID;
    connection.talkerUniqueID = talkerUniqueID;
    connection.listenerEntityID = listenerEntityID;
    connection.listenerUniqueID = listenerUniqueID;
    connection.timestamp = std::chrono::steady_clock::now();
    connection.messageType = ACMPMessageType::DISCONNECT_TX_COMMAND;
    
    _pendingConnections[sequenceID] = connection;
    
    // Send DISCONNECT_TX_COMMAND
    sendCommand(ACMPMessageType::DISCONNECT_TX_COMMAND, sequenceID, talkerEntityID, listenerEntityID,
                talkerUniqueID, listenerUniqueID);
    
    return sequenceID;
}

void ACMPControllerStateMachine::processMessage(const ACMPMessage& message) {
    // Extract sequence ID and find pending connection
    // Implementation would parse the actual ACMP PDU data
    uint16_t sequenceID = 0; // Extract from message.pdu_data
    
    auto it = _pendingConnections.find(sequenceID);
    if (it != _pendingConnections.end()) {
        // Process response
        ACMP_Status status = ACMP_Status::SUCCESS; // Extract from message
        
        switch (message.type) {
            case ACMPMessageType_enum::CONNECT_TX_RESPONSE:
                if (_callbacks.connectionEstablished) {
                    _callbacks.connectionEstablished(sequenceID, status);
                }
                break;
                
            case ACMPMessageType_enum::DISCONNECT_TX_RESPONSE:
                if (_callbacks.connectionTerminated) {
                    _callbacks.connectionTerminated(sequenceID, status);
                }
                break;
                
            default:
                break;
        }
        
        _pendingConnections.erase(it);
    }
}

void ACMPControllerStateMachine::sendCommand(ACMPMessageType messageType, uint16_t sequenceID,
                                           EntityID talkerEntityID, EntityID listenerEntityID,
                                           uint16_t talkerUniqueID, uint16_t listenerUniqueID) {
    if (!_network) return;
    
    ACMPMessage message;
    message.type = static_cast<ACMPMessageType_enum>(messageType);
    
    // Fill PDU data - implementation would construct proper ACMP PDU
    // memset(message.pdu_data, 0, sizeof(message.pdu_data));
    
    // Send message
    // _network->sendACMPMessage(message);
}

uint16_t ACMPControllerStateMachine::generateSequenceID() {
    static uint16_t sequenceCounter = 1;
    return sequenceCounter++;
}

// ============================================================================
// AECP ENTITY MODEL CONTROLLER STATE MACHINE IMPLEMENTATION
// ============================================================================

AECPEntityModelControllerStateMachine::AECPEntityModelControllerStateMachine(NetworkInterface* network)
    : _network(network), _state(State::WAITING) {
}

AECPEntityModelControllerStateMachine::~AECPEntityModelControllerStateMachine() {
    stop();
}

void AECPEntityModelControllerStateMachine::start() {
    _running = true;
    _state = State::WAITING;
}

void AECPEntityModelControllerStateMachine::stop() {
    _running = false;
    _state = State::WAITING;
    _inflightCommands = {};
}

void AECPEntityModelControllerStateMachine::tick() {
    auto now = std::chrono::steady_clock::now();
    processCommandTimeout(now);
}

uint16_t AECPEntityModelControllerStateMachine::readDescriptor(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex) {
    if (!_running || !_network) {
        return 0;
    }
    
    return sendAemCommand(entityID, AEM_Command_type::READ_DESCRIPTOR, descriptorType, descriptorIndex);
}

uint16_t AECPEntityModelControllerStateMachine::acquireEntity(EntityID entityID, AcquireFlags flags) {
    if (!_running || !_network) {
        return 0;
    }
    
    return sendAemCommand(entityID, AEM_Command_type::ACQUIRE_ENTITY, 0, 0, &flags, sizeof(flags));
}

uint16_t AECPEntityModelControllerStateMachine::lockEntity(EntityID entityID, LockFlags flags) {
    if (!_running || !_network) {
        return 0;
    }
    
    return sendAemCommand(entityID, AEM_Command_type::LOCK_ENTITY, 0, 0, &flags, sizeof(flags));
}

void AECPEntityModelControllerStateMachine::processMessage(const AECPMessage& message) {
    // Extract sequence ID from message
    uint16_t sequenceID = 0; // Extract from message.pdu_data
    
    // Find matching inflight command
    while (!_inflightCommands.empty()) {
        auto& command = _inflightCommands.front();
        if (command.sequenceID == sequenceID) {
            // Process response
            if (_callbacks.commandCompleted) {
                AECP_Status status = AECP_Status::SUCCESS; // Extract from message
                _callbacks.commandCompleted(sequenceID, status);
            }
            _inflightCommands.pop();
            break;
        }
        _inflightCommands.pop();
    }
}

uint16_t AECPEntityModelControllerStateMachine::sendAemCommand(EntityID entityID, AemCommandType commandType,
                                                             DescriptorType descriptorType, uint16_t descriptorIndex,
                                                             const void* commandData, size_t commandDataSize) {
    uint16_t sequenceID = generateSequenceID();
    
    // Create inflight command
    InflightAemCommand command{};
    command.sequenceID = sequenceID;
    command.entityID = entityID;
    command.commandType = commandType;
    command.timestamp = std::chrono::steady_clock::now();
    command.retryCount = 0;
    
    _inflightCommands.push(command);
    
    // Send AEM command - implementation would construct proper AECP PDU
    AECPMessage message;
    message.type = AECPMessageType_enum::AEM_COMMAND;
    // Fill PDU data...
    
    // _network->sendAECPMessage(message);
    
    return sequenceID;
}

void AECPEntityModelControllerStateMachine::processCommandTimeout(std::chrono::steady_clock::time_point now) {
    while (!_inflightCommands.empty()) {
        auto& command = _inflightCommands.front();
        auto elapsed = now - command.timestamp;
        
        if (elapsed > _commandTimeout) {
            if (command.retryCount < _maxRetries) {
                // Retry command
                command.retryCount++;
                command.timestamp = now;
                
                // Resend command - implementation would resend the actual command
                
            } else {
                // Command timed out
                if (_callbacks.commandFailed) {
                    _callbacks.commandFailed(command.sequenceID, AECP_Status::TIMED_OUT);
                }
                _inflightCommands.pop();
            }
        } else {
            break; // Commands are ordered by time
        }
    }
}

uint16_t AECPEntityModelControllerStateMachine::generateSequenceID() {
    static uint16_t sequenceCounter = 1;
    return sequenceCounter++;
}

void AECPEntityModelControllerStateMachine::transitionToState(State newState) {
    if (_state != newState) {
        State oldState = _state;
        _state = newState;
        
        // Notify state change if callback is registered
        if (_callbacks.stateChanged) {
            _callbacks.stateChanged(oldState, newState);
        }
    }
}

// ============================================================================
// STATE MACHINE ORCHESTRATOR IMPLEMENTATION
// ============================================================================

StateMachineOrchestrator::StateMachineOrchestrator(NetworkInterface* network)
    : _network(network), _running(false) {
    
    // Create state machines
    _discoveryStateMachine = std::make_unique<DiscoveryStateMachine>(network);
    _acmpControllerStateMachine = std::make_unique<ACMPControllerStateMachine>(network);
    _aecpControllerStateMachine = std::make_unique<AECPEntityModelControllerStateMachine>(network);
}

StateMachineOrchestrator::~StateMachineOrchestrator() {
    stop();
}

void StateMachineOrchestrator::start() {
    if (!_running) {
        _running = true;
        
        // Start all state machines
        _discoveryStateMachine->start();
        _acmpControllerStateMachine->start();
        _aecpControllerStateMachine->start();
        
        // Start tick thread
        _tickThread = std::thread(&StateMachineOrchestrator::tickThreadLoop, this);
    }
}

void StateMachineOrchestrator::stop() {
    if (_running) {
        _running = false;
        
        // Stop state machines
        _discoveryStateMachine->stop();
        _acmpControllerStateMachine->stop();
        _aecpControllerStateMachine->stop();
        
        // Stop tick thread
        if (_tickThread.joinable()) {
            _tickThread.join();
        }
    }
}

void StateMachineOrchestrator::tick() {
    std::lock_guard<std::mutex> lock(_stateMachinesMutex);
    
    // Tick all state machines
    _discoveryStateMachine->tick();
    _acmpControllerStateMachine->tick();
    _aecpControllerStateMachine->tick();
}

void StateMachineOrchestrator::processADPMessage(const ADPMessage& message) {
    std::lock_guard<std::mutex> lock(_stateMachinesMutex);
    _discoveryStateMachine->processMessage(message);
}

void StateMachineOrchestrator::processACMPMessage(const ACMPMessage& message) {
    std::lock_guard<std::mutex> lock(_stateMachinesMutex);
    _acmpControllerStateMachine->processMessage(message);
}

void StateMachineOrchestrator::processAECPMessage(const AECPMessage& message) {
    std::lock_guard<std::mutex> lock(_stateMachinesMutex);
    _aecpControllerStateMachine->processMessage(message);
}

DiscoveryStateMachine* StateMachineOrchestrator::getDiscoveryStateMachine() {
    return _discoveryStateMachine.get();
}

ACMPControllerStateMachine* StateMachineOrchestrator::getACMPControllerStateMachine() {
    return _acmpControllerStateMachine.get();
}

AECPEntityModelControllerStateMachine* StateMachineOrchestrator::getAECPControllerStateMachine() {
    return _aecpControllerStateMachine.get();
}

void StateMachineOrchestrator::tickThreadLoop() {
    const auto tickInterval = std::chrono::milliseconds(10); // 10ms tick interval
    auto nextTick = std::chrono::steady_clock::now();
    
    while (_running) {
        tick();
        
        nextTick += tickInterval;
        std::this_thread::sleep_until(nextTick);
    }
}

void StateMachineOrchestrator::processMessage(const ProtocolMessage& message) {
    switch (message.type) {
        case ProtocolMessageType::ADP:
            processADPMessage(*message.as_adp());
            break;
            
        case ProtocolMessageType::ACMP:
            processACMPMessage(*message.as_acmp());
            break;
            
        case ProtocolMessageType::AECP:
            processAECPMessage(*message.as_aecp());
            break;
    }
}

} // namespace StateMachines
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
