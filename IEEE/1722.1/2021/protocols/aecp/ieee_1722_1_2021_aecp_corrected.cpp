/**
 * @file ieee_1722_1_2021_aecp_corrected.cpp
 * @brief IEEE 1722.1-2021 AECP Implementation - Korrekte Namespace-Referenzen
 * 
 * PRIORITÄT: IEEE 1722.1-2021 erst danach bei 1722-2016!
 * Verwendet korrekte AECP-Namespace-Referenzen aus 1722_1-2021.h
 */

#include "ieee_1722_1_2021_base.h"
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>

// Windows networking compatibility
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #ifndef htobe64
        #define htobe64(x) _byteswap_uint64(x)
        #define be64toh(x) _byteswap_uint64(x)
    #endif
    #ifndef htons
        #define htons(x) _byteswap_ushort(x)
        #define ntohs(x) _byteswap_ushort(x)
    #endif
    #ifndef GetTickCount64
        // Fallback for older Windows versions
        #define GetTickCount64() GetTickCount()
    #endif
#else
    #include <arpa/inet.h>
    #include <endian.h>
    #include <sys/time.h>
    
    // Compatibility function for non-Windows
    static uint64_t GetTickCount64() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }
#endif

// ============================================================================
// IEEE 1722.1-2021 AECP Production Implementation
// ============================================================================

/**
 * @brief Produktive AECP Protocol Handler - Windows + Linux kompatibel
 * Verwendet korrekte Namespace-Referenzen aus 1722_1-2021.h
 */
namespace IEEE1722_1_2021_AECP {

// Einfache Entity State Struktur
struct EntityState {
    bool acquired;
    bool locked;
    uint64_t ownerId;
    uint64_t acquireTime;  // milliseconds since epoch
    uint64_t lockTime;     // milliseconds since epoch
    uint16_t currentConfiguration;
    
    EntityState() : acquired(false), locked(false), ownerId(0), 
                   acquireTime(0), lockTime(0), currentConfiguration(0) {}
};

// Einfache Descriptor Storage
struct DescriptorStorage {
    struct DescriptorEntry {
        uint16_t type;
        uint16_t index;
        std::vector<uint8_t> data;
        bool valid;
        
        DescriptorEntry() : type(0), index(0), valid(false) {}
    };
    
    static const size_t MAX_DESCRIPTORS = 256;
    DescriptorEntry descriptors[MAX_DESCRIPTORS];
    size_t descriptorCount;
    std::mutex mutex;
    
    DescriptorStorage() : descriptorCount(0) {}
    
    bool addDescriptor(uint16_t type, uint16_t index, const std::vector<uint8_t>& data) {
        std::lock_guard<std::mutex> lock(mutex);
        if (descriptorCount >= MAX_DESCRIPTORS) return false;
        
        auto& desc = descriptors[descriptorCount++];
        desc.type = type;
        desc.index = index;
        desc.data = data;
        desc.valid = true;
        return true;
    }
    
    bool findDescriptor(uint16_t type, uint16_t index, std::vector<uint8_t>& data) {
        std::lock_guard<std::mutex> lock(mutex);
        for (size_t i = 0; i < descriptorCount; ++i) {
            if (descriptors[i].valid && descriptors[i].type == type && descriptors[i].index == index) {
                data = descriptors[i].data;
                return true;
            }
        }
        return false;
    }
};

/**
 * @brief Produktive AECP Protocol Handler Implementation
 */
class AECPProtocolHandler {
private:
    static const size_t MAX_ENTITIES = 64;
    EntityState entityStates[MAX_ENTITIES];
    uint64_t entityIds[MAX_ENTITIES];
    size_t entityCount;
    
    DescriptorStorage descriptorStorage;
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
        entityStates[entityCount] = EntityState();
        entityCount++;
        return true;
    }
    
    // Get current time in milliseconds
    uint64_t getCurrentTimeMs() {
        return GetTickCount64();
    }

public:
    AECPProtocolHandler() : entityCount(0), nextSequenceId(1) {}
    
    // ========================================================================
    // IEEE 1722.1-2021 AECP Core Implementation
    // ========================================================================
    
    /**
     * @brief Process incoming AECP command - KERN-FUNKTIONALITÄT
     * Verwendet korrekte AECP-Typen aus dem globalen AECP-Namespace
     */
    bool processCommand(const AECP::AEMCommandMessage& command, AECP::AEMResponseMessage& response) {
        std::lock_guard<std::mutex> lock(stateMutex);
        
        // Initialize response with proper zero-initialization
        memset(&response, 0, sizeof(response));
        response.subtype = 0xFB;  // AECP subtype
        response.sv_version_msg_type = static_cast<uint8_t>(AECP::AECPMessageType::AEM_RESPONSE);
        response.setTargetEntityId(command.getControllerEntityId());
        response.setControllerEntityId(command.getTargetEntityId());
        response.setSequenceId(command.getSequenceId());
        response.setCommandType(command.getCommandType());
        
        // Process based on command type
        AECP::AEMCommandType cmdType = command.getAEMCommandType();
        
        switch (cmdType) {
            case AECP::AEMCommandType::READ_DESCRIPTOR:
                return processReadDescriptor(command, response);
                
            case AECP::AEMCommandType::ACQUIRE_ENTITY:
                return processAcquireEntity(command, response);
                
            case AECP::AEMCommandType::LOCK_ENTITY:
                return processLockEntity(command, response);
                
            case AECP::AEMCommandType::GET_CONFIGURATION:
                return processGetConfiguration(command, response);
                
            case AECP::AEMCommandType::SET_CONFIGURATION:
                return processSetConfiguration(command, response);
                
            default:
                response.setStatus(AECP::AEMStatusCode::NOT_IMPLEMENTED);
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
        std::vector<uint8_t> data;
        if (!descriptorStorage.findDescriptor(descriptorType, descriptorIndex, data)) {
            return false;
        }
        
        if (descriptorSize < data.size()) {
            descriptorSize = data.size();
            return false;
        }
        
        memcpy(descriptorData, data.data(), data.size());
        descriptorSize = data.size();
        return true;
    }
    
    bool writeDescriptor(uint16_t descriptorType, uint16_t descriptorIndex,
                        const void* descriptorData, size_t descriptorSize) {
        std::vector<uint8_t> data(static_cast<const uint8_t*>(descriptorData),
                                  static_cast<const uint8_t*>(descriptorData) + descriptorSize);
        return descriptorStorage.addDescriptor(descriptorType, descriptorIndex, data);
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
    bool processReadDescriptor(const AECP::AEMCommandMessage& command, AECP::AEMResponseMessage& response) {
        const auto* cmdData = reinterpret_cast<const AECP::ReadDescriptorCommand*>(command.command_specific_data);
        auto* respData = reinterpret_cast<AECP::ReadDescriptorResponse*>(response.response_specific_data);
        
        uint16_t descriptorType = ntohs(cmdData->descriptor_type);
        uint16_t descriptorIndex = ntohs(cmdData->descriptor_index);
        
        std::vector<uint8_t> descriptorData;
        if (descriptorStorage.findDescriptor(descriptorType, descriptorIndex, descriptorData)) {
            respData->configuration_index = cmdData->configuration_index;
            respData->reserved = 0;
            respData->descriptor_type = cmdData->descriptor_type;
            respData->descriptor_index = cmdData->descriptor_index;
            
            size_t copySize = std::min(descriptorData.size(), sizeof(respData->descriptor_data));
            memcpy(respData->descriptor_data, descriptorData.data(), copySize);
            
            response.setStatus(AECP::AEMStatusCode::SUCCESS);
        } else {
            response.setStatus(AECP::AEMStatusCode::NO_SUCH_DESCRIPTOR);
        }
        
        return true;
    }
    
    bool processAcquireEntity(const AECP::AEMCommandMessage& command, AECP::AEMResponseMessage& response) {
        uint64_t targetEntityId = command.getTargetEntityId();
        uint64_t controllerEntityId = command.getControllerEntityId();
        
        if (acquireEntity(targetEntityId, controllerEntityId)) {
            response.setStatus(AECP::AEMStatusCode::SUCCESS);
        } else {
            response.setStatus(AECP::AEMStatusCode::ENTITY_ACQUIRED);
        }
        
        return true;
    }
    
    bool processLockEntity(const AECP::AEMCommandMessage& command, AECP::AEMResponseMessage& response) {
        uint64_t targetEntityId = command.getTargetEntityId();
        uint64_t controllerEntityId = command.getControllerEntityId();
        
        std::lock_guard<std::mutex> lock(stateMutex);
        int idx = findEntityIndex(targetEntityId);
        
        if (idx < 0) {
            response.setStatus(AECP::AEMStatusCode::NO_SUCH_DESCRIPTOR);
            return true;
        }
        
        auto& state = entityStates[idx];
        if (state.acquired && state.ownerId == controllerEntityId) {
            state.locked = true;
            state.lockTime = getCurrentTimeMs();
            response.setStatus(AECP::AEMStatusCode::SUCCESS);
        } else {
            response.setStatus(AECP::AEMStatusCode::ENTITY_LOCKED);
        }
        
        return true;
    }
    
    bool processGetConfiguration(const AECP::AEMCommandMessage& command, AECP::AEMResponseMessage& response) {
        auto* respData = reinterpret_cast<AECP::GetConfigurationResponse*>(response.response_specific_data);
        
        respData->reserved = 0;
        respData->configuration_index = htons(getCurrentConfiguration());
        response.setStatus(AECP::AEMStatusCode::SUCCESS);
        
        return true;
    }
    
    bool processSetConfiguration(const AECP::AEMCommandMessage& command, AECP::AEMResponseMessage& response) {
        const auto* cmdData = reinterpret_cast<const AECP::SetConfigurationCommand*>(command.command_specific_data);
        auto* respData = reinterpret_cast<AECP::SetConfigurationResponse*>(response.response_specific_data);
        
        uint16_t configIndex = ntohs(cmdData->configuration_index);
        
        if (setConfiguration(configIndex)) {
            respData->reserved = 0;
            respData->configuration_index = htons(configIndex);
            response.setStatus(AECP::AEMStatusCode::SUCCESS);
        } else {
            response.setStatus(AECP::AEMStatusCode::BAD_ARGUMENTS);
        }
        
        return true;
    }
};

// ============================================================================
// Global Instance Management
// ============================================================================

static std::unique_ptr<AECPProtocolHandler> globalHandler;
static std::mutex globalMutex;

/**
 * @brief Get global AECP protocol handler instance
 */
AECPProtocolHandler* getAECPHandler() {
    std::lock_guard<std::mutex> lock(globalMutex);
    if (!globalHandler) {
        globalHandler = std::make_unique<AECPProtocolHandler>();
    }
    return globalHandler.get();
}

/**
 * @brief Initialize AECP protocol handler
 */
bool initializeAECPProtocol() {
    std::lock_guard<std::mutex> lock(globalMutex);
    if (!globalHandler) {
        globalHandler = std::make_unique<AECPProtocolHandler>();
        return true;
    }
    return false;  // Already initialized
}

/**
 * @brief Cleanup AECP protocol handler
 */
void cleanupAECPProtocol() {
    std::lock_guard<std::mutex> lock(globalMutex);
    globalHandler.reset();
}

} // namespace IEEE1722_1_2021_AECP

// ============================================================================
// C Interface for Integration
// ============================================================================

extern "C" {
    
/**
 * @brief C interface for AECP command processing
 */
int aecp_process_command(const void* command_data, size_t command_size,
                        void* response_data, size_t* response_size) {
    if (!command_data || !response_data || !response_size) {
        return -1;
    }
    
    if (command_size < sizeof(AECP::AEMCommandMessage)) {
        return -2;
    }
    
    auto* handler = IEEE1722_1_2021_AECP::getAECPHandler();
    if (!handler) {
        return -3;
    }
    
    const auto* command = static_cast<const AECP::AEMCommandMessage*>(command_data);
    AECP::AEMResponseMessage response;
    
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
int aecp_initialize() {
    return IEEE1722_1_2021_AECP::initializeAECPProtocol() ? 0 : -1;
}

/**
 * @brief Cleanup AECP from C code
 */
void aecp_cleanup() {
    IEEE1722_1_2021_AECP::cleanupAECPProtocol();
}

} // extern "C"
