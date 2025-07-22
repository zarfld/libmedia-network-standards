/**
 * @file ieee_1722_1_2021_library.cpp
 * @brief IEEE 1722.1-2021 Protocol State Machines Library Implementation
 * @details Implementation of the library design for IEEE 1722.1-2021 AVDECC state machines
 * 
 * @author OpenAvnu AVDECC Implementation
 * @date 2025
 * @version 2.0.0
 * 
 * @copyright
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ieee_1722_1_2021_library.h"
#include <stdexcept>
#include <cstring>

namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace StateMachines {

// ============================================================================
// PROTOCOL MESSAGE SERIALIZATION IMPLEMENTATION
// ============================================================================

size_t ProtocolMessageSerializer::serializeADPMessage(const ADPMessage& message, uint8_t* buffer, size_t bufferSize) {
    // Implementation note: This would serialize the ADP message according to IEEE 1722.1-2021 format
    // For now, provide a stub implementation
    if (!buffer || bufferSize < 64) {
        return 0; // Need at least 64 bytes for ADP message
    }
    
    // IEEE 1722.1-2021 ADP PDU format implementation would go here
    // - AVTPDU Common Header (12 bytes)
    // - ADP Specific Fields (variable based on message type)
    
    // Stub: Just copy the message type and basic data
    memset(buffer, 0, bufferSize);
    buffer[0] = static_cast<uint8_t>(message.type);
    
    // Return actual serialized size (stub returns minimum ADP size)
    return 64;
}

bool ProtocolMessageSerializer::deserializeADPMessage(const uint8_t* data, size_t length, ADPMessage& message) {
    // Implementation note: This would deserialize according to IEEE 1722.1-2021 format
    if (!data || length < 64) {
        return false;
    }
    
    // Stub implementation
    message.type = static_cast<ADPMessageType>(data[0]);
    memset(&message.data, 0, sizeof(message.data));
    
    return true;
}

size_t ProtocolMessageSerializer::serializeACMPMessage(const ACMPMessage& message, uint8_t* buffer, size_t bufferSize) {
    // Implementation note: IEEE 1722.1-2021 ACMP PDU serialization
    if (!buffer || bufferSize < 64) {
        return 0;
    }
    
    // Stub implementation
    memset(buffer, 0, bufferSize);
    buffer[0] = static_cast<uint8_t>(message.type);
    size_t copySize = (sizeof(message.pdu_data) < 63) ? sizeof(message.pdu_data) : 63;
    memcpy(buffer + 1, message.pdu_data, copySize);
    
    return 64;
}

bool ProtocolMessageSerializer::deserializeACMPMessage(const uint8_t* data, size_t length, ACMPMessage& message) {
    if (!data || length < 64) {
        return false;
    }
    
    message.type = static_cast<ACMPMessageType_enum>(data[0]);
    size_t copySize = (sizeof(message.pdu_data) < 63) ? sizeof(message.pdu_data) : 63;
    memcpy(message.pdu_data, data + 1, copySize);
    
    return true;
}

size_t ProtocolMessageSerializer::serializeAECPMessage(const AECPMessage& message, uint8_t* buffer, size_t bufferSize) {
    // Implementation note: IEEE 1722.1-2021 AECP PDU serialization
    if (!buffer || bufferSize < 64) {
        return 0;
    }
    
    // Stub implementation
    memset(buffer, 0, bufferSize);
    buffer[0] = static_cast<uint8_t>(message.type);
    size_t maxCopy = (bufferSize > 1) ? bufferSize - 1 : 0;
    size_t copySize = (sizeof(message.pdu_data) < maxCopy) ? sizeof(message.pdu_data) : maxCopy;
    memcpy(buffer + 1, message.pdu_data, copySize);
    
    return copySize + 1;
}

bool ProtocolMessageSerializer::deserializeAECPMessage(const uint8_t* data, size_t length, AECPMessage& message) {
    if (!data || length < 64) {
        return false;
    }
    
    message.type = static_cast<AECPMessageType_enum>(data[0]);
    size_t maxCopy = (length > 1) ? length - 1 : 0;
    size_t copySize = (sizeof(message.pdu_data) < maxCopy) ? sizeof(message.pdu_data) : maxCopy;
    memcpy(message.pdu_data, data + 1, copySize);
    
    return true;
}

// ============================================================================
// AVDECC LIBRARY IMPLEMENTATION (PIMPL)
// ============================================================================

class AVDECCLibrary::Implementation {
public:
    Implementation(std::unique_ptr<INetworkInterface> networkInterface,
                   std::unique_ptr<IEntityModelDelegate> entityDelegate,
                   std::unique_ptr<AVDECCLibrary::Delegate> libraryDelegate)
        : _networkInterface(std::move(networkInterface))
        , _entityDelegate(std::move(entityDelegate))
        , _libraryDelegate(std::move(libraryDelegate))
        , _running(false)
        , _discoveryInterval(2000)  // 2 seconds default
        , _entityTimeout(10)        // 10 seconds default
        , _commandTimeout(1000)     // 1 second default
    {
        if (!_networkInterface) {
            throw std::invalid_argument("Network interface cannot be null");
        }
        if (!_entityDelegate) {
            throw std::invalid_argument("Entity delegate cannot be null");
        }
    }
    
    ~Implementation() {
        stop();
    }
    
    bool start() {
        if (_running) {
            return false; // Already running
        }
        
        if (!_networkInterface->isReady()) {
            return false; // Network not ready
        }
        
        _running = true;
        _workerThread = std::thread(&Implementation::workerThreadLoop, this);
        
        return true;
    }
    
    void stop() {
        if (!_running) {
            return;
        }
        
        _running = false;
        
        if (_workerThread.joinable()) {
            _workerThread.join();
        }
        
        // Clear discovered entities
        std::lock_guard<std::mutex> lock(_entitiesMutex);
        _discoveredEntities.clear();
    }
    
    bool isRunning() const {
        return _running;
    }
    
    void discoverAllEntities() {
        if (!_running) {
            return;
        }
        
        // Send global discovery message (EntityID = 0)
        sendDiscoveryMessage(EntityID{0});
    }
    
    void discoverEntity(EntityID entityID) {
        if (!_running) {
            return;
        }
        
        sendDiscoveryMessage(entityID);
    }
    
    std::vector<DiscoveredEntity> getDiscoveredEntities() const {
        std::lock_guard<std::mutex> lock(_entitiesMutex);
        std::vector<DiscoveredEntity> entities;
        entities.reserve(_discoveredEntities.size());
        
        for (const auto& pair : _discoveredEntities) {
            entities.push_back(pair.second);
        }
        
        return entities;
    }
    
    bool connectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                      EntityID listenerEntityID, uint16_t listenerUniqueID) {
        if (!_running) {
            return false;
        }
        
        // Create ACMP CONNECT_TX_COMMAND
        ACMPConnectTxCommand command;
        command.sequenceID = getNextSequenceID();
        command.talkerEntityID = talkerEntityID;
        command.listenerEntityID = listenerEntityID;
        command.talkerUniqueID = talkerUniqueID;
        command.listenerUniqueID = listenerUniqueID;
        // Initialize other fields as needed
        
        return sendACMPCommand(command);
    }
    
    bool disconnectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                         EntityID listenerEntityID, uint16_t listenerUniqueID) {
        if (!_running) {
            return false;
        }
        
        // Create ACMP DISCONNECT_TX_COMMAND
        ACMPDisconnectTxCommand command;
        command.sequenceID = getNextSequenceID();
        command.talkerEntityID = talkerEntityID;
        command.listenerEntityID = listenerEntityID;
        command.talkerUniqueID = talkerUniqueID;
        command.listenerUniqueID = listenerUniqueID;
        
        return sendACMPDisconnectCommand(command);
    }
    
    bool readDescriptor(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex,
                       void* descriptorData, size_t& descriptorSize) {
        if (!_running || !descriptorData) {
            return false;
        }
        
        // Create AECP READ_DESCRIPTOR command
        AECPAemCommandMessage command;
        command.sequenceID = getNextSequenceID();
        command.targetEntityID = entityID;
        command.controllerEntityID = getLocalEntityID();
        // command.commandType = AEM_COMMAND_READ_DESCRIPTOR; // Would use proper constant
        // Set command data for descriptor type and index
        
        return sendAECPCommand(command);
    }
    
    void setDiscoveryInterval(std::chrono::milliseconds interval) {
        _discoveryInterval = interval;
    }
    
    void setEntityTimeout(std::chrono::seconds timeout) {
        _entityTimeout = timeout;
    }
    
    void setCommandTimeout(std::chrono::milliseconds timeout) {
        _commandTimeout = timeout;
    }

private:
    // Core components
    std::unique_ptr<INetworkInterface> _networkInterface;
    std::unique_ptr<IEntityModelDelegate> _entityDelegate;
    std::unique_ptr<AVDECCLibrary::Delegate> _libraryDelegate;
    
    // Threading and lifecycle
    std::atomic<bool> _running;
    std::thread _workerThread;
    
    // Configuration
    std::chrono::milliseconds _discoveryInterval;
    std::chrono::seconds _entityTimeout;
    std::chrono::milliseconds _commandTimeout;
    
    // State management
    std::unordered_map<EntityID, DiscoveredEntity> _discoveredEntities;
    mutable std::mutex _entitiesMutex;
    
    // Sequence ID management
    std::atomic<uint16_t> _sequenceID{1};
    
    // Internal methods
    void workerThreadLoop() {
        auto lastDiscovery = std::chrono::steady_clock::now();
        
        while (_running) {
            auto now = std::chrono::steady_clock::now();
            
            // Periodic discovery
            if (now - lastDiscovery >= _discoveryInterval) {
                discoverAllEntities();
                lastDiscovery = now;
            }
            
            // Remove timed-out entities
            removeTimedOutEntities(now);
            
            // Process incoming messages (would be implemented with actual network polling)
            processIncomingMessages();
            
            // Sleep for a short time to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void sendDiscoveryMessage(EntityID targetEntityID) {
        ADPMessage message;
        message.type = ADPMessageType::ENTITY_DISCOVER;
        
        auto* discover = message.asDiscover();
        discover->entityID = targetEntityID;
        discover->sourceMac = _networkInterface->getLocalMacAddress();
        discover->timestamp = std::chrono::steady_clock::now();
        
        // Serialize and send
        uint8_t buffer[1500];
        size_t size = ProtocolMessageSerializer::serializeADPMessage(message, buffer, sizeof(buffer));
        if (size > 0) {
            _networkInterface->sendRawMessage(buffer, size, MacAddress::getMulticast());
        }
    }
    
    bool sendACMPCommand(const ACMPConnectTxCommand& command) {
        // Implementation would create proper ACMP message and send it
        // For now, just return true as stub
        return true;
    }
    
    bool sendACMPDisconnectCommand(const ACMPDisconnectTxCommand& command) {
        // Implementation would create proper ACMP message and send it
        return true;
    }
    
    bool sendAECPCommand(const AECPAemCommandMessage& command) {
        // Implementation would create proper AECP message and send it
        return true;
    }
    
    void processIncomingMessages() {
        // Implementation would poll network interface for incoming messages
        // and process them according to IEEE 1722.1-2021 specification
        
        // Stub: In real implementation, this would:
        // 1. Poll network interface for incoming Ethernet frames
        // 2. Parse AVTPDU headers to determine message type (ADP/ACMP/AECP)
        // 3. Deserialize messages using ProtocolMessageSerializer
        // 4. Process messages according to state machine logic
        // 5. Generate appropriate responses
        // 6. Call delegate methods for application callbacks
    }
    
    void removeTimedOutEntities(std::chrono::steady_clock::time_point now) {
        std::lock_guard<std::mutex> lock(_entitiesMutex);
        
        auto it = _discoveredEntities.begin();
        while (it != _discoveredEntities.end()) {
            if (now >= it->second.timeout) {
                EntityID entityID = it->first;
                it = _discoveredEntities.erase(it);
                
                // Notify application of entity departure
                if (_libraryDelegate) {
                    _libraryDelegate->onEntityDeparted(entityID);
                }
            } else {
                ++it;
            }
        }
    }
    
    uint16_t getNextSequenceID() {
        return _sequenceID.fetch_add(1);
    }
    
    EntityID getLocalEntityID() {
        // Implementation would return the local entity ID
        // For now, return a placeholder
        return 0x001122334455667ULL;
    }
};

// ============================================================================
// AVDECC LIBRARY PUBLIC API IMPLEMENTATION
// ============================================================================

AVDECCLibrary::AVDECCLibrary(std::unique_ptr<INetworkInterface> networkInterface,
                             std::unique_ptr<IEntityModelDelegate> entityDelegate,
                             std::unique_ptr<Delegate> libraryDelegate)
    : _impl(std::make_unique<Implementation>(std::move(networkInterface),
                                           std::move(entityDelegate),
                                           std::move(libraryDelegate)))
{
}

AVDECCLibrary::~AVDECCLibrary() = default;

bool AVDECCLibrary::start() {
    return _impl->start();
}

void AVDECCLibrary::stop() {
    _impl->stop();
}

bool AVDECCLibrary::isRunning() const {
    return _impl->isRunning();
}

void AVDECCLibrary::discoverAllEntities() {
    _impl->discoverAllEntities();
}

void AVDECCLibrary::discoverEntity(EntityID entityID) {
    _impl->discoverEntity(entityID);
}

std::vector<DiscoveredEntity> AVDECCLibrary::getDiscoveredEntities() const {
    return _impl->getDiscoveredEntities();
}

bool AVDECCLibrary::connectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                                 EntityID listenerEntityID, uint16_t listenerUniqueID) {
    return _impl->connectStream(talkerEntityID, talkerUniqueID, listenerEntityID, listenerUniqueID);
}

bool AVDECCLibrary::disconnectStream(EntityID talkerEntityID, uint16_t talkerUniqueID,
                                    EntityID listenerEntityID, uint16_t listenerUniqueID) {
    return _impl->disconnectStream(talkerEntityID, talkerUniqueID, listenerEntityID, listenerUniqueID);
}

bool AVDECCLibrary::readDescriptor(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex,
                                  void* descriptorData, size_t& descriptorSize) {
    return _impl->readDescriptor(entityID, descriptorType, descriptorIndex, descriptorData, descriptorSize);
}

bool AVDECCLibrary::setConfiguration(EntityID entityID, uint16_t configurationIndex) {
    // Implementation would send SET_CONFIGURATION command
    return false; // Stub
}

bool AVDECCLibrary::setStreamFormat(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex, uint64_t streamFormat) {
    // Implementation would send SET_STREAM_FORMAT command
    return false; // Stub
}

void AVDECCLibrary::setDiscoveryInterval(std::chrono::milliseconds interval) {
    _impl->setDiscoveryInterval(interval);
}

void AVDECCLibrary::setEntityTimeout(std::chrono::seconds timeout) {
    _impl->setEntityTimeout(timeout);
}

void AVDECCLibrary::setCommandTimeout(std::chrono::milliseconds timeout) {
    _impl->setCommandTimeout(timeout);
}

// ============================================================================
// LIBRARY FACTORY IMPLEMENTATION
// ============================================================================

namespace LibraryFactory {

std::unique_ptr<AVDECCLibrary> createController(
    std::unique_ptr<INetworkInterface> networkInterface,
    std::unique_ptr<AVDECCLibrary::Delegate> delegate) {
    
    // Create a minimal entity delegate for controller-only mode
    class ControllerOnlyEntityDelegate : public IEntityModelDelegate {
    public:
        void onEntityDiscovered(const DiscoveredEntity& entity) override {
            // Forward to library delegate
            if (_libraryDelegate) {
                _libraryDelegate->onEntityDiscovered(entity);
            }
        }
        
        void onEntityDeparted(EntityID entityID) override {
            if (_libraryDelegate) {
                _libraryDelegate->onEntityDeparted(entityID);
            }
        }
        
        void onEntityUpdated(const DiscoveredEntity& entity) override {
            // Controller typically doesn't need entity updates
        }
        
        void onStreamConnectionRequest(const ACMPConnectTxCommand& request, ACMPConnectTxResponse& response) override {
            // Controllers don't handle connection requests
            response.status = ACMP_Status::NOT_SUPPORTED;
        }
        
        void onStreamDisconnectionRequest(const ACMPDisconnectTxCommand& request, ACMPDisconnectTxResponse& response) override {
            response.status = ACMP_Status::NOT_SUPPORTED;
        }
        
        void onDescriptorReadRequest(DescriptorType descriptorType, uint16_t descriptorIndex, 
                                    AemCommandStatus& status, void* descriptorData, size_t& descriptorSize) override {
            status = AemCommandStatus::NOT_SUPPORTED;
        }
        
        void onConfigurationChangeRequest(uint16_t configurationIndex, AemCommandStatus& status) override {
            status = AemCommandStatus::NOT_SUPPORTED;
        }
        
        void onStreamFormatChangeRequest(DescriptorType descriptorType, uint16_t descriptorIndex, 
                                        uint64_t streamFormat, AemCommandStatus& status) override {
            status = AemCommandStatus::NOT_SUPPORTED;
        }
        
        void setLibraryDelegate(AVDECCLibrary::Delegate* delegate) {
            _libraryDelegate = delegate;
        }
        
    private:
        AVDECCLibrary::Delegate* _libraryDelegate = nullptr;
    };
    
    auto entityDelegate = std::make_unique<ControllerOnlyEntityDelegate>();
    entityDelegate->setLibraryDelegate(delegate.get());
    
    return std::make_unique<AVDECCLibrary>(std::move(networkInterface),
                                          std::move(entityDelegate),
                                          std::move(delegate));
}

std::unique_ptr<AVDECCLibrary> createEntity(
    std::unique_ptr<INetworkInterface> networkInterface,
    std::unique_ptr<IEntityModelDelegate> entityDelegate,
    const LocalEntity& localEntity) {
    
    return std::make_unique<AVDECCLibrary>(std::move(networkInterface),
                                          std::move(entityDelegate),
                                          nullptr); // No library delegate for entity-only mode
}

std::unique_ptr<AVDECCLibrary> createMixed(
    std::unique_ptr<INetworkInterface> networkInterface,
    std::unique_ptr<IEntityModelDelegate> entityDelegate,
    std::unique_ptr<AVDECCLibrary::Delegate> delegate,
    const LocalEntity& localEntity) {
    
    return std::make_unique<AVDECCLibrary>(std::move(networkInterface),
                                          std::move(entityDelegate),
                                          std::move(delegate));
}

} // namespace LibraryFactory

} // namespace StateMachines
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
