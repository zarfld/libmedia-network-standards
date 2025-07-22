#pragma once

/**
 * IEEE 1722.1-2021 Core Definitions - Phase 2C Library Activation
 * 
 * Purpose: Complete IEEE 1722.1-2021 AVDECC types compatible with real implementation requirements
 * Phase: 2C - Library Activation with Real Implementation Compatibility
 * 
 * This header provides the core IEEE 1722.1-2021 types that are compatible with
 * the real protocol implementation signatures and method requirements.
 * 
 * Key Features:
 * - Complete namespace IEEE::_1722_1::_2021::AECP hierarchy
 * - Message types compatible with implementation requirements  
 * - Real command/response structures with proper member access
 * - Windows MSVC compatibility
 * - Proper method signatures matching implementation expectations
 */

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <queue>

// Windows compatibility
#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

namespace IEEE {
namespace _1722_1 {
namespace _2021 {

// Basic IEEE 1722.1 types
using EntityID = uint64_t;

namespace AECP {

// AECP Message Types - IEEE 1722.1-2021 Standard
enum class AECPMessageType : uint8_t {
    AEM_COMMAND = 0,
    AEM_RESPONSE = 1,
    ADDRESS_ACCESS_COMMAND = 2,
    ADDRESS_ACCESS_RESPONSE = 3,
    AVC_COMMAND = 4,
    AVC_RESPONSE = 5,
    VENDOR_UNIQUE_COMMAND = 6,
    VENDOR_UNIQUE_RESPONSE = 7,
    HDCP_APM_COMMAND = 8,
    HDCP_APM_RESPONSE = 9,
    EXTENDED_COMMAND = 14,
    EXTENDED_RESPONSE = 15
};

// AEM Command Types - IEEE 1722.1-2021 Standard
enum class AEMCommandType : uint16_t {
    ACQUIRE_ENTITY = 0x0000,
    LOCK_ENTITY = 0x0001,
    ENTITY_AVAILABLE = 0x0002,
    CONTROLLER_AVAILABLE = 0x0003,
    READ_DESCRIPTOR = 0x0004,
    WRITE_DESCRIPTOR = 0x0005,
    SET_CONFIGURATION = 0x0006,
    GET_CONFIGURATION = 0x0007,
    SET_STREAM_FORMAT = 0x0008,
    GET_STREAM_FORMAT = 0x0009,
    SET_VIDEO_FORMAT = 0x000A,
    GET_VIDEO_FORMAT = 0x000B,
    SET_SENSOR_FORMAT = 0x000C,
    GET_SENSOR_FORMAT = 0x000D,
    SET_STREAM_INFO = 0x000E,
    GET_STREAM_INFO = 0x000F,
    SET_NAME = 0x0010,
    GET_NAME = 0x0011,
    SET_ASSOCIATION_ID = 0x0012,
    GET_ASSOCIATION_ID = 0x0013,
    SET_SAMPLING_RATE = 0x0014,
    GET_SAMPLING_RATE = 0x0015,
    SET_CLOCK_SOURCE = 0x0016,
    GET_CLOCK_SOURCE = 0x0017,
    SET_CONTROL = 0x0018,
    GET_CONTROL = 0x0019,
    INCREMENT_CONTROL = 0x001A,
    DECREMENT_CONTROL = 0x001B,
    SET_SIGNAL_SELECTOR = 0x001C,
    GET_SIGNAL_SELECTOR = 0x001D,
    SET_MIXER = 0x001E,
    GET_MIXER = 0x001F,
    SET_MATRIX = 0x0020,
    GET_MATRIX = 0x0021,
    START_STREAMING = 0x0022,
    STOP_STREAMING = 0x0023,
    REGISTER_UNSOLICITED_NOTIFICATION = 0x0024,
    DEREGISTER_UNSOLICITED_NOTIFICATION = 0x0025,
    IDENTIFY_NOTIFICATION = 0x0026,
    GET_AVB_INFO = 0x0027,
    GET_AS_PATH = 0x0028,
    GET_COUNTERS = 0x0029,
    REBOOT = 0x002A,
    GET_AUDIO_MAP = 0x002B,
    ADD_AUDIO_MAPPINGS = 0x002C,
    REMOVE_AUDIO_MAPPINGS = 0x002D,
    GET_VIDEO_MAP = 0x002E,
    ADD_VIDEO_MAPPINGS = 0x002F,
    REMOVE_VIDEO_MAPPINGS = 0x0030,
    GET_SENSOR_MAP = 0x0031,
    ADD_SENSOR_MAPPINGS = 0x0032,
    REMOVE_SENSOR_MAPPINGS = 0x0033,
    START_OPERATION = 0x0034,
    ABORT_OPERATION = 0x0035,
    OPERATION_STATUS = 0x0036,
    AUTH_ADD_KEY = 0x0037,
    AUTH_DELETE_KEY = 0x0038,
    AUTH_GET_KEY_LIST = 0x0039,
    AUTH_GET_KEY = 0x003A,
    AUTH_ADD_KEY_TO_CHAIN = 0x003B,
    AUTH_DELETE_KEY_FROM_CHAIN = 0x003C,
    AUTH_GET_KEYCHAIN_LIST = 0x003D,
    AUTH_GET_IDENTITY = 0x003E,
    AUTH_ADD_TOKEN = 0x003F,
    AUTH_DELETE_TOKEN = 0x0040,
    AUTHENTICATE = 0x0041,
    DEAUTHENTICATE = 0x0042,
    ENABLE_TRANSPORT_SECURITY = 0x0043,
    DISABLE_TRANSPORT_SECURITY = 0x0044,
    ENABLE_STREAM_ENCRYPTION = 0x0045,
    DISABLE_STREAM_ENCRYPTION = 0x0046,
    SET_MEMORY_OBJECT_LENGTH = 0x0047,
    GET_MEMORY_OBJECT_LENGTH = 0x0048,
    SET_STREAM_BACKUP = 0x0049,
    GET_STREAM_BACKUP = 0x004A,
    EXPANSION = 0x7FFF
};

// AEM Status Codes - IEEE 1722.1-2021 Standard  
enum class AEMStatusCode : uint8_t {
    SUCCESS = 0,
    NOT_IMPLEMENTED = 1,
    NO_SUCH_DESCRIPTOR = 2,
    ENTITY_LOCKED = 3,
    ENTITY_ACQUIRED = 4,
    NOT_AUTHENTICATED = 5,
    AUTHENTICATION_DISABLED = 6,
    BAD_ARGUMENTS = 7,
    NO_RESOURCES = 8,
    IN_PROGRESS = 9,
    ENTITY_MISBEHAVING = 10,
    NOT_SUPPORTED = 11,
    STREAM_IS_RUNNING = 12
};

// Forward declarations for message types  
struct AEMCommandMessage;
struct AEMResponseMessage;

// Command and Response Messages with implementation-compatible interface
struct AEMCommandMessage {
    AECPMessageType message_type;
    AEMCommandType command_type;
    uint16_t sequence_id;
    EntityID target_entity_id;
    EntityID controller_entity_id;
    std::vector<uint8_t> payload;
    
    // Implementation-compatible methods
    AEMCommandType getAEMCommandType() const { return command_type; }
    uint16_t getSequenceId() const { return sequence_id; }
    EntityID getTargetEntityId() const { return target_entity_id; }
    EntityID getControllerEntityId() const { return controller_entity_id; }
    AECPMessageType getMessageType() const { return message_type; }
    
    // Setters for implementation
    void setAEMCommandType(AEMCommandType type) { command_type = type; }
    void setSequenceId(uint16_t id) { sequence_id = id; }
    void setTargetEntityId(EntityID id) { target_entity_id = id; }
    void setControllerEntityId(EntityID id) { controller_entity_id = id; }
    void setMessageType(AECPMessageType type) { message_type = type; }
};

struct AEMResponseMessage {
    AECPMessageType message_type;
    AEMCommandType command_type;
    AEMStatusCode status;
    uint16_t sequence_id;
    EntityID target_entity_id;
    EntityID controller_entity_id;
    std::vector<uint8_t> payload;
    
    // Implementation-compatible methods
    AEMCommandType getAEMCommandType() const { return command_type; }
    AEMStatusCode getStatus() const { return status; }
    uint16_t getSequenceId() const { return sequence_id; }
    EntityID getTargetEntityId() const { return target_entity_id; }
    EntityID getControllerEntityId() const { return controller_entity_id; }
    AECPMessageType getMessageType() const { return message_type; }
    
    // Setters for implementation
    void setAEMCommandType(AEMCommandType type) { command_type = type; }
    void setStatus(AEMStatusCode s) { status = s; }
    void setSequenceId(uint16_t id) { sequence_id = id; }
    void setTargetEntityId(EntityID id) { target_entity_id = id; }
    void setControllerEntityId(EntityID id) { controller_entity_id = id; }
    void setMessageType(AECPMessageType type) { message_type = type; }
};

// Specific Command/Response types with proper member access
struct ReadDescriptorCommand {
    uint16_t descriptor_type;
    uint16_t descriptor_index;
    uint16_t configuration_index;
    uint16_t reserved;
    
    // Getters
    uint16_t getDescriptorType() const { return descriptor_type; }
    uint16_t getDescriptorIndex() const { return descriptor_index; }
    uint16_t getConfigurationIndex() const { return configuration_index; }
};

struct ReadDescriptorResponse {
    uint16_t descriptor_type;
    uint16_t descriptor_index;
    uint16_t configuration_index;
    uint16_t reserved;
    std::vector<uint8_t> descriptor_data;
    
    // Getters and setters  
    uint16_t getDescriptorType() const { return descriptor_type; }
    uint16_t getDescriptorIndex() const { return descriptor_index; }
    uint16_t getConfigurationIndex() const { return configuration_index; }
    
    void setDescriptorType(uint16_t type) { descriptor_type = type; }
    void setDescriptorIndex(uint16_t index) { descriptor_index = index; }
    void setConfigurationIndex(uint16_t index) { configuration_index = index; }
};

struct AcquireEntityCommand {
    uint32_t acquire_flags;
    EntityID owner_entity_id;
    uint16_t descriptor_type;
    uint16_t descriptor_index;
};

struct AcquireEntityResponse {
    uint32_t acquire_flags;
    EntityID owner_entity_id;
    uint16_t descriptor_type;
    uint16_t descriptor_index;
};

struct LockEntityCommand {
    uint32_t lock_flags;
    EntityID locked_entity_id;
    uint16_t descriptor_type;
    uint16_t descriptor_index;
};

struct LockEntityResponse {
    uint32_t lock_flags;
    EntityID locked_entity_id;
    uint16_t descriptor_type;
    uint16_t descriptor_index;
};

struct GetConfigurationCommand {
    uint16_t reserved;
};

struct GetConfigurationResponse {
    uint16_t reserved;
    uint16_t current_configuration;
};

struct SetConfigurationCommand {
    uint16_t reserved;
    uint16_t requested_configuration;
};

struct SetConfigurationResponse {
    uint16_t reserved;
    uint16_t requested_configuration;
};

// Protocol Handler Interface - Compatible with real implementation
class ProtocolHandler {
public:
    virtual ~ProtocolHandler() = default;
    
    // Implementation-compatible interface matching real method signatures
    virtual bool readDescriptor(uint16_t descriptorType, uint16_t descriptorIndex, void* descriptorData, size_t& descriptorSize) = 0;
    virtual bool acquireEntity(EntityID entityId, uint32_t flags, EntityID* ownerEntityId) = 0;
    virtual bool lockEntity(EntityID entityId, uint32_t flags, EntityID* lockedEntityId) = 0;
    virtual bool getCurrentConfiguration(uint16_t* currentConfiguration) = 0;
    virtual bool setConfiguration(uint16_t requestedConfiguration) = 0;
    
    // Command processing interface
    virtual bool processCommand(const AEMCommandMessage& command, AEMResponseMessage& response) = 0;
    virtual void handleTimeout(uint16_t sequenceId) = 0;
    
    // Entity management
    virtual bool isEntityAcquired(EntityID entityId) const = 0;
    virtual bool isEntityLocked(EntityID entityId) const = 0;
    virtual EntityID getEntityOwner(EntityID entityId) const = 0;
};

// Factory helper class 
class ResponseFactory {
public:
    static AEMResponseMessage createSuccessResponse(const AEMCommandMessage& command) {
        AEMResponseMessage response;
        response.setMessageType(AECPMessageType::AEM_RESPONSE);
        response.setAEMCommandType(command.getAEMCommandType());
        response.setStatus(AEMStatusCode::SUCCESS);
        response.setSequenceId(command.getSequenceId());
        response.setTargetEntityId(command.getTargetEntityId());
        response.setControllerEntityId(command.getControllerEntityId());
        return response;
    }
    
    static AEMResponseMessage createErrorResponse(const AEMCommandMessage& command, AEMStatusCode status) {
        AEMResponseMessage response;
        response.setMessageType(AECPMessageType::AEM_RESPONSE);
        response.setAEMCommandType(command.getAEMCommandType());
        response.setStatus(status);
        response.setSequenceId(command.getSequenceId());
        response.setTargetEntityId(command.getTargetEntityId());
        response.setControllerEntityId(command.getControllerEntityId());
        return response;
    }
};

} // namespace AECP
} // namespace _2021  
} // namespace _1722_1
} // namespace IEEE
