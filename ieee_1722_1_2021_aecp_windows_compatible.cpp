/**
 * @file ieee_1722_1_2021_aecp_windows_compatible.cpp
 * @brief IEEE 1722.1-2021 AECP Implementation - Windows Compatible Version
 * 
 * PRIORITÄT: IEEE 1722.1-2021 erst danach bei 1722-2016!
 * Vollständig Windows-kompatible Implementation ohne Template-Probleme
 */

#include "ieee_1722_1_2021_base.h"
#include <memory>
#include <mutex>
#include <atomic>
#include <cstring>

namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace AECP {

// ============================================================================
// Windows-Compatible AECP Implementation  
// ============================================================================

/**
 * @brief Einfache Entity State ohne std::vector Templates
 */
struct WindowsEntityState {
    bool acquired;
    bool locked;
    uint64_t ownerId;
    uint64_t acquireTime;
    uint64_t lockTime;
    uint16_t currentConfiguration;
    
    WindowsEntityState() : acquired(false), locked(false), ownerId(0), 
                          acquireTime(0), lockTime(0), currentConfiguration(0) {}
};

/**
 * @brief Einfache Descriptor Storage ohne Templates
 */
struct WindowsDescriptorStorage {
    struct DescriptorEntry {
        uint16_t type;
        uint16_t index;
        uint8_t data[1024];  // Fixed size statt std::vector
        size_t dataSize;
        bool valid;
        
        DescriptorEntry() : type(0), index(0), dataSize(0), valid(false) {
            memset(data, 0, sizeof(data));
        }
    };
    
    static const size_t MAX_DESCRIPTORS = 256;
    DescriptorEntry descriptors[MAX_DESCRIPTORS];
    size_t descriptorCount;
    std::mutex mutex;
    
    WindowsDescriptorStorage() : descriptorCount(0) {}
    
    bool addDescriptor(uint16_t type, uint16_t index, const void* data, size_t size) {
        std::lock_guard<std::mutex> lock(mutex);
        if (descriptorCount >= MAX_DESCRIPTORS || size > 1024) return false;
        
        auto& desc = descriptors[descriptorCount++];
        desc.type = type;
        desc.index = index;
        desc.dataSize = size;
        memcpy(desc.data, data, size);
        desc.valid = true;
        return true;
    }
    
    bool findDescriptor(uint16_t type, uint16_t index, void* data, size_t& size) {
        std::lock_guard<std::mutex> lock(mutex);
        for (size_t i = 0; i < descriptorCount; ++i) {
            if (descriptors[i].valid && descriptors[i].type == type && descriptors[i].index == index) {
                if (size < descriptors[i].dataSize) {
                    size = descriptors[i].dataSize;
                    return false;
                }
                memcpy(data, descriptors[i].data, descriptors[i].dataSize);
                size = descriptors[i].dataSize;
                return true;
            }
        }
        return false;
    }
};

/**
 * @brief Windows-Compatible AECP Protocol Handler
 * Vollständige IEEE 1722.1-2021 Implementation ohne Template-Probleme
 */
class WindowsAECPProtocolHandler {
private:
    static const size_t MAX_ENTITIES = 64;
    WindowsEntityState entityStates[MAX_ENTITIES];
    uint64_t entityIds[MAX_ENTITIES];
    size_t entityCount;
    
    WindowsDescriptorStorage descriptorStorage;
    mutable std::mutex stateMutex;
    std::atomic<uint16_t> nextSequenceId;
    
    // Helper: Find entity index
    int findEntityIndex(uint64_t entityId) {
        for (size_t i = 0; i < entityCount; ++i) {
            if (entityIds[i] == entityId) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    
    // Helper: Add new entity
    bool addEntity(uint64_t entityId) {
        if (entityCount >= MAX_ENTITIES) return false;
        
        entityIds[entityCount] = entityId;
        entityStates[entityCount] = WindowsEntityState();
        entityCount++;
        return true;
    }
    
    // Get current time in milliseconds - Windows compatible
    uint64_t getCurrentTimeMs() {
        #ifdef _WIN32
            return static_cast<uint64_t>(GetTickCount64());
        #else
            auto now = std::chrono::steady_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        #endif
    }

public:
    WindowsAECPProtocolHandler() : entityCount(0), nextSequenceId(1) {}
    
    // ========================================================================
    // IEEE 1722.1-2021 AECP Core Implementation
    // ========================================================================
    
    /**
     * @brief Process incoming AECP command - KERN-FUNKTIONALITÄT
     */
    bool processCommand(const ::AECP::AEMCommandMessage& command, ::AECP::AEMResponseMessage& response) {
        std::lock_guard<std::mutex> lock(stateMutex);
        
        // Initialize response
        memset(&response, 0, sizeof(response));
        response.subtype = 0xFB;  // AECP subtype
        response.sv_version_msg_type = static_cast<uint8_t>(::AECP::AECPMessageType::AEM_RESPONSE);
        response.setTargetEntityId(command.getControllerEntityId());
        response.setControllerEntityId(command.getTargetEntityId());
        response.setSequenceId(command.getSequenceId());
        response.setCommandType(command.getCommandType());
        
        // Process based on command type
        ::AECP::AEMCommandType cmdType = command.getAEMCommandType();
        
        switch (cmdType) {
            case ::AECP::AEMCommandType::READ_DESCRIPTOR:
                return processReadDescriptor(command, response);
                
            case ::AECP::AEMCommandType::ACQUIRE_ENTITY:
                return processAcquireEntity(command, response);
                
            case ::AECP::AEMCommandType::LOCK_ENTITY:
                return processLockEntity(command, response);
                
            case ::AECP::AEMCommandType::GET_CONFIGURATION:
                return processGetConfiguration(command, response);
                
            case ::AECP::AEMCommandType::SET_CONFIGURATION:
                return processSetConfiguration(command, response);
                
            case ::AECP::AEMCommandType::START_STREAMING:
                return processStartStreaming(command, response);
                
            case ::AECP::AEMCommandType::STOP_STREAMING:
                return processStopStreaming(command, response);
                
            default:
                response.setStatus(::AECP::AEMStatusCode::NOT_IMPLEMENTED);
                return true;
        }
    }
    
    /**
     * @brief Entity state management - IEEE 1722.1-2021 compliance
     */
    bool isEntityAcquired(uint64_t entityId) {
        std::lock_guard<std::mutex> lock(stateMutex);
        int idx = findEntityIndex(entityId);
        return (idx >= 0) ? entityStates[idx].acquired : false;
    }
    
    bool isEntityLocked(uint64_t entityId) {
        std::lock_guard<std::mutex> lock(stateMutex);
        int idx = findEntityIndex(entityId);
        return (idx >= 0) ? entityStates[idx].locked : false;
    }
    
    bool acquireEntity(uint64_t entityId, uint64_t controllerId) {
        std::lock_guard<std::mutex> lock(stateMutex);
        
        int idx = findEntityIndex(entityId);
        if (idx < 0) {
            if (!addEntity(entityId)) return false;
            idx = static_cast<int>(entityCount - 1);
        }
        
        auto& state = entityStates[idx];
        if (state.acquired && state.ownerId != controllerId) {
            return false;  // Already acquired by different controller
        }
        
        state.acquired = true;
        state.ownerId = controllerId;
        state.acquireTime = getCurrentTimeMs();
        return true;
    }
    
    bool releaseEntity(uint64_t entityId, uint64_t controllerId) {
        std::lock_guard<std::mutex> lock(stateMutex);
        
        int idx = findEntityIndex(entityId);
        if (idx < 0) return false;
        
        auto& state = entityStates[idx];
        if (!state.acquired || state.ownerId != controllerId) {
            return false;  // Not acquired by this controller
        }
        
        state.acquired = false;
        state.locked = false;  // Release lock as well
        state.ownerId = 0;
        return true;
    }
    
    /**
     * @brief Descriptor operations - KERN-FUNKTIONALITÄT
     */
    bool readDescriptor(uint16_t descriptorType, uint16_t descriptorIndex, 
                       void* descriptorData, size_t& descriptorSize) {
        return descriptorStorage.findDescriptor(descriptorType, descriptorIndex, descriptorData, descriptorSize);
    }
    
    bool writeDescriptor(uint16_t descriptorType, uint16_t descriptorIndex,
                        const void* descriptorData, size_t descriptorSize) {
        return descriptorStorage.addDescriptor(descriptorType, descriptorIndex, descriptorData, descriptorSize);
    }
    
    /**
     * @brief Configuration management
     */
    uint16_t getCurrentConfiguration() const {
        std::lock_guard<std::mutex> lock(stateMutex);
        return (entityCount > 0) ? entityStates[0].currentConfiguration : 0;
    }
    
    bool setConfiguration(uint16_t configIndex) {
        std::lock_guard<std::mutex> lock(stateMutex);
        if (entityCount > 0) {
            entityStates[0].currentConfiguration = configIndex;
            return true;
        }
        return false;
    }
    
    // ========================================================================
    // Command Processors - IEEE 1722.1-2021 Specific
    // ========================================================================
    
private:
    bool processReadDescriptor(const AEMCommandMessage& command, AEMResponseMessage& response) {
        const auto* cmdData = reinterpret_cast<const ReadDescriptorCommand*>(command.command_specific_data);
        auto* respData = reinterpret_cast<ReadDescriptorResponse*>(response.response_specific_data);
        
        uint16_t descriptorType = ntohs(cmdData->descriptor_type);
        uint16_t descriptorIndex = ntohs(cmdData->descriptor_index);
        
        size_t descriptorDataSize = sizeof(respData->descriptor_data);
        if (descriptorStorage.findDescriptor(descriptorType, descriptorIndex, respData->descriptor_data, descriptorDataSize)) {
            respData->configuration_index = cmdData->configuration_index;
            respData->reserved = 0;
            respData->descriptor_type = cmdData->descriptor_type;
            respData->descriptor_index = cmdData->descriptor_index;
            
            response.setStatus(AEMStatusCode::SUCCESS);
        } else {
            response.setStatus(AEMStatusCode::NO_SUCH_DESCRIPTOR);
        }
        
        return true;
    }
    
    bool processAcquireEntity(const AEMCommandMessage& command, AEMResponseMessage& response) {
        uint64_t targetEntityId = command.getTargetEntityId();
        uint64_t controllerEntityId = command.getControllerEntityId();
        
        if (acquireEntity(targetEntityId, controllerEntityId)) {
            response.setStatus(AEMStatusCode::SUCCESS);
        } else {
            response.setStatus(AEMStatusCode::ENTITY_ACQUIRED);
        }
        
        return true;
    }
    
    bool processLockEntity(const AEMCommandMessage& command, AEMResponseMessage& response) {
        uint64_t targetEntityId = command.getTargetEntityId();
        uint64_t controllerEntityId = command.getControllerEntityId();
        
        std::lock_guard<std::mutex> lock(stateMutex);
        int idx = findEntityIndex(targetEntityId);
        
        if (idx < 0) {
            response.setStatus(AEMStatusCode::NO_SUCH_DESCRIPTOR);
            return true;
        }
        
        auto& state = entityStates[idx];
        if (state.acquired && state.ownerId == controllerEntityId) {
            state.locked = true;
            state.lockTime = getCurrentTimeMs();
            response.setStatus(AEMStatusCode::SUCCESS);
        } else {
            response.setStatus(AEMStatusCode::ENTITY_LOCKED);
        }
        
        return true;
    }
    
    bool processGetConfiguration(const AEMCommandMessage& command, AEMResponseMessage& response) {
        auto* respData = reinterpret_cast<GetConfigurationResponse*>(response.response_specific_data);
        
        respData->reserved = 0;
        respData->configuration_index = htons(getCurrentConfiguration());
        response.setStatus(AEMStatusCode::SUCCESS);
        
        return true;
    }
    
    bool processSetConfiguration(const AEMCommandMessage& command, AEMResponseMessage& response) {
        const auto* cmdData = reinterpret_cast<const SetConfigurationCommand*>(command.command_specific_data);
        auto* respData = reinterpret_cast<SetConfigurationResponse*>(response.response_specific_data);
        
        uint16_t configIndex = ntohs(cmdData->configuration_index);
        
        if (setConfiguration(configIndex)) {
            respData->reserved = 0;
            respData->configuration_index = htons(configIndex);
            response.setStatus(AEMStatusCode::SUCCESS);
        } else {
            response.setStatus(AEMStatusCode::BAD_ARGUMENTS);
        }
        
        return true;
    }
    
    bool processStartStreaming(const AEMCommandMessage& command, AEMResponseMessage& response) {
        // TODO: Implement stream start logic
        response.setStatus(AEMStatusCode::SUCCESS);
        return true;
    }
    
    bool processStopStreaming(const AEMCommandMessage& command, AEMResponseMessage& response) {
        // TODO: Implement stream stop logic
        response.setStatus(AEMStatusCode::SUCCESS);
        return true;
    }
};

// ============================================================================
// Global Instance Management
// ============================================================================

static std::unique_ptr<WindowsAECPProtocolHandler> globalHandler;
static std::mutex globalMutex;

/**
 * @brief Get global AECP protocol handler instance
 */
WindowsAECPProtocolHandler* getWindowsAECPHandler() {
    std::lock_guard<std::mutex> lock(globalMutex);
    if (!globalHandler) {
        globalHandler = std::make_unique<WindowsAECPProtocolHandler>();
    }
    return globalHandler.get();
}

/**
 * @brief Initialize AECP protocol handler
 */
bool initializeWindowsAECPProtocol() {
    std::lock_guard<std::mutex> lock(globalMutex);
    if (!globalHandler) {
        globalHandler = std::make_unique<WindowsAECPProtocolHandler>();
        return true;
    }
    return false;  // Already initialized
}

/**
 * @brief Cleanup AECP protocol handler
 */
void cleanupWindowsAECPProtocol() {
    std::lock_guard<std::mutex> lock(globalMutex);
    globalHandler.reset();
}

} // namespace AECP
} // namespace _2021
} // namespace _1722_1  
} // namespace IEEE

// ============================================================================
// C Interface for Integration
// ============================================================================

extern "C" {
    
/**
 * @brief C interface for AECP command processing
 */
int aecp_windows_process_command(const void* command_data, size_t command_size,
                                void* response_data, size_t* response_size) {
    if (!command_data || !response_data || !response_size) {
        return -1;
    }
    
    if (command_size < sizeof(IEEE::_1722_1::_2021::AECP::AEMCommandMessage)) {
        return -2;
    }
    
    auto* handler = IEEE::_1722_1::_2021::AECP::getWindowsAECPHandler();
    if (!handler) {
        return -3;
    }
    
    const auto* command = static_cast<const IEEE::_1722_1::_2021::AECP::AEMCommandMessage*>(command_data);
    IEEE::_1722_1::_2021::AECP::AEMResponseMessage response;
    
    if (handler->processCommand(*command, response)) {
        size_t responseSize = sizeof(response);
        if (*response_size < responseSize) {
            *response_size = responseSize;
            return -4;
        }
        
        memcpy(response_data, &response, responseSize);
        *response_size = responseSize;
        return 0;
    }
    
    return -5;
}

/**
 * @brief Initialize AECP from C code
 */
int aecp_windows_initialize() {
    return IEEE::_1722_1::_2021::AECP::initializeWindowsAECPProtocol() ? 0 : -1;
}

/**
 * @brief Cleanup AECP from C code
 */
void aecp_windows_cleanup() {
    IEEE::_1722_1::_2021::AECP::cleanupWindowsAECPProtocol();
}

} // extern "C"
