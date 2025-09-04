#pragma once

/**
 * IEEE 1722.1-2021 Core Definitions - Phase 2C Library Activation
 * 
 * Purpose: Complete IEEE 1722.1-2021 AVDECC types compatible with real implementation requirements
 * Phase: 2C - Library Activation with Real Implementation Compatibility
 * 
 * };

// Base Command/Response Message structureses the core IEEE 1722.1-2021 types that are compatible with
 * the real protocol implementation signatures and method requirements.
 * 
 * Key Features:
 * - Complete namespace IEEE::_1722_1::_2021::AECP hierarchy
 * - Message types compatible with implementation requirements  
 * - Real command/response structures with proper member access
 * - Windows MSVC compatibility
 * - Proper method signatures matching implementation expectations
 */

// REVIEW [Project convention]: Prefer authoritative jdksavdecc constants over hardcoded values.
// Include (when available): "../avdecc-lib/jdksavdecc-c/include/jdksavdecc_aem_command.h"
// Use JDKSAVDECC_AEM_COMMAND_READ_DESCRIPTOR etc. to avoid mismatch with spec updates.

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <queue>
#include <array>

// Forward declarations to avoid Windows header conflicts
#ifdef _WIN32
// Define necessary types without including full Windows headers
using socket_t = uintptr_t;
// MSVC byte swap macros - only define if winsock2.h is not already included
#if !defined(_WINSOCK2API_) && !defined(_WINSOCKAPI_)
    #define htons(x) ((uint16_t)((((uint16_t)(x) & 0x00ff) << 8) | (((uint16_t)(x) & 0xff00) >> 8)))
    #define ntohs(x) htons(x)
    #define htonl(x) ((uint32_t)((((uint32_t)(x) & 0x000000ff) << 24) | (((uint32_t)(x) & 0x0000ff00) << 8) | (((uint32_t)(x) & 0x00ff0000) >> 8) | (((uint32_t)(x) & 0xff000000) >> 24)))
    #define ntohl(x) htonl(x)
#endif
#else
#include <arpa/inet.h>
using socket_t = int;
#endif

// REVIEW [IEEE 1722.1-2021, 5.x General]: All multi-byte fields on the wire are big-endian.
// Ensure serialization/deserialization paths apply htons/htonl/ntohs/ntohl where appropriate.

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
    // REVIEW [Milan extension]: Support GET_DYNAMIC_INFO 0x004B per Milan (AVnu) extensions when Milan profile is enabled.
    // Using jdksavdecc constants (e.g., JDKSAVDECC_AEM_COMMAND_GET_DYNAMIC_INFO) is preferred to avoid divergence.
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

// Legacy type aliases for compatibility with existing test code
using AECP_Message_type = AECPMessageType;
using AEM_Command_type = AEMCommandType;
using AECP_Status = AEMStatusCode;

// Legacy PDU type for compatibility with test code
struct ATDECCEnumerationControlProtocolPDU {
    EntityID target_entity_id = 0;
    EntityID controller_entity_id = 0;
    uint16_t sequence_id = 0;
    AECPMessageType message_type = AECPMessageType::AEM_COMMAND;
    AEMCommandType command_type = AEMCommandType::READ_DESCRIPTOR;
    AEMStatusCode status = AEMStatusCode::SUCCESS;
    std::vector<uint8_t> command_specific_data;
    
    // Compatibility methods for test code
    bool is_command() const { 
        return message_type == AECPMessageType::AEM_COMMAND; 
    }
    
    bool is_response() const { 
        return message_type == AECPMessageType::AEM_RESPONSE; 
    }
    
    bool is_aem_message() const {
        return message_type == AECPMessageType::AEM_COMMAND || 
               message_type == AECPMessageType::AEM_RESPONSE;
    }
    
    void set_read_descriptor_command(uint16_t config_index, uint16_t desc_type, uint16_t desc_index) {
        message_type = AECPMessageType::AEM_COMMAND;
        command_type = AEMCommandType::READ_DESCRIPTOR;
        command_specific_data.resize(6);
        // Pack the parameters (in network byte order for real implementation)
        command_specific_data[0] = (config_index >> 8) & 0xFF;
        command_specific_data[1] = config_index & 0xFF;
        command_specific_data[2] = (desc_type >> 8) & 0xFF;
        command_specific_data[3] = desc_type & 0xFF;
        command_specific_data[4] = (desc_index >> 8) & 0xFF;
        command_specific_data[5] = desc_index & 0xFF;
    }
    
    void set_read_descriptor_response(uint16_t config_index, uint16_t desc_type, uint16_t desc_index, 
                                    const uint8_t* descriptor_data, size_t descriptor_length) {
        message_type = AECPMessageType::AEM_RESPONSE;
        command_type = AEMCommandType::READ_DESCRIPTOR;
        status = AEMStatusCode::SUCCESS;
        
        command_specific_data.resize(6 + descriptor_length);
        // Pack the header parameters
        command_specific_data[0] = (config_index >> 8) & 0xFF;
        command_specific_data[1] = config_index & 0xFF;
        command_specific_data[2] = (desc_type >> 8) & 0xFF;
        command_specific_data[3] = desc_type & 0xFF;
        command_specific_data[4] = (desc_index >> 8) & 0xFF;
        command_specific_data[5] = desc_index & 0xFF;
        
        // Copy descriptor data
        if (descriptor_data && descriptor_length > 0) {
            std::memcpy(&command_specific_data[6], descriptor_data, descriptor_length);
        }
    }
    
    void get_read_descriptor_command(uint16_t& config_index, uint16_t& desc_type, uint16_t& desc_index) const {
        if (command_specific_data.size() >= 6) {
            config_index = (command_specific_data[0] << 8) | command_specific_data[1];
            desc_type = (command_specific_data[2] << 8) | command_specific_data[3];
            desc_index = (command_specific_data[4] << 8) | command_specific_data[5];
        }
    }
    
    void get_read_descriptor_response(uint16_t& config_index, uint16_t& desc_type, uint16_t& desc_index,
                                    const uint8_t*& descriptor_data, size_t& descriptor_length) const {
        if (command_specific_data.size() >= 6) {
            config_index = (command_specific_data[0] << 8) | command_specific_data[1];
            desc_type = (command_specific_data[2] << 8) | command_specific_data[3];
            desc_index = (command_specific_data[4] << 8) | command_specific_data[5];
            
            if (command_specific_data.size() > 6) {
                descriptor_data = &command_specific_data[6];
                descriptor_length = command_specific_data.size() - 6;
            } else {
                descriptor_data = nullptr;
                descriptor_length = 0;
            }
        }
    }
    
    // Serialization compatibility methods (stub implementations for testing)
    void serialize() {
        // In a real implementation, this would serialize to network byte order
        // For testing, this is a no-op
    }
    
    void create_response(AEMStatusCode response_status) {
        message_type = AECPMessageType::AEM_RESPONSE;
        status = response_status;
        // Swap entity IDs for response
        EntityID temp = target_entity_id;
        target_entity_id = controller_entity_id;
        controller_entity_id = temp;
    }
    
    const uint8_t* get_raw_octets() const {
        return command_specific_data.data();
    }
    
    size_t get_size() const {
        return command_specific_data.size();
    }
    
    // Constructor from raw data (for deserialization testing)
    ATDECCEnumerationControlProtocolPDU() = default;
    
    ATDECCEnumerationControlProtocolPDU(const uint8_t* data, size_t length) {
        if (data && length > 0) {
            command_specific_data.assign(data, data + length);
        }
    }
};

// AEM Status Codes - IEEE 1722.1-2021 Standard  
enum class AEMStatusCode_Backup : uint8_t {
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

    // REVIEW [IEEE 1722.1-2021, AEM descriptor integrity]: When returning descriptors, compute and insert AEM checksum (CRC32)
    // over the descriptor excluding the checksum field; store checksum in network byte order.
    
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
    // REVIEW [AEM available_index handling]: For READ_DESCRIPTOR of descriptors supporting available_index,
    // increment available_index only on state change as per spec; do not bump on every read.
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

    // REVIEW [Compliance hooks]: Implementations should validate descriptor CRC32, enforce Milan constraints
    // (e.g., for Audio/Stream formats), and support optional Milan GET_DYNAMIC_INFO command.
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

// AEM (AVDECC Entity Model) - IEEE 1722.1-2021 
namespace AEM {

// Descriptor Types - IEEE 1722.1-2021 Standard
enum DescriptorType : uint16_t {
    DESCRIPTOR_ENTITY = 0x0000,
    DESCRIPTOR_CONFIGURATION = 0x0001,
    DESCRIPTOR_AUDIO_UNIT = 0x0002,
    DESCRIPTOR_VIDEO_UNIT = 0x0003,
    DESCRIPTOR_SENSOR_UNIT = 0x0004,
    DESCRIPTOR_STREAM_INPUT = 0x0005,
    DESCRIPTOR_STREAM_OUTPUT = 0x0006,
    DESCRIPTOR_JACK_INPUT = 0x0007,
    DESCRIPTOR_JACK_OUTPUT = 0x0008,
    DESCRIPTOR_AVB_INTERFACE = 0x0009,
    DESCRIPTOR_CLOCK_SOURCE = 0x000A,
    DESCRIPTOR_MEMORY_OBJECT = 0x000B,
    DESCRIPTOR_LOCALE = 0x000C,
    DESCRIPTOR_STRINGS = 0x000D,
    DESCRIPTOR_STREAM_PORT_INPUT = 0x000E,
    DESCRIPTOR_STREAM_PORT_OUTPUT = 0x000F,
    DESCRIPTOR_EXTERNAL_PORT_INPUT = 0x0010,
    DESCRIPTOR_EXTERNAL_PORT_OUTPUT = 0x0011,
    DESCRIPTOR_INTERNAL_PORT_INPUT = 0x0012,
    DESCRIPTOR_INTERNAL_PORT_OUTPUT = 0x0013,
    DESCRIPTOR_AUDIO_CLUSTER = 0x0014,
    DESCRIPTOR_VIDEO_CLUSTER = 0x0015,
    DESCRIPTOR_SENSOR_CLUSTER = 0x0016,
    DESCRIPTOR_AUDIO_MAP = 0x0017,
    DESCRIPTOR_VIDEO_MAP = 0x0018,
    DESCRIPTOR_SENSOR_MAP = 0x0019,
    DESCRIPTOR_CONTROL = 0x001A,
    DESCRIPTOR_SIGNAL_SELECTOR = 0x001B,
    DESCRIPTOR_MIXER = 0x001C,
    DESCRIPTOR_MATRIX = 0x001D,
    DESCRIPTOR_MATRIX_SIGNAL = 0x001E,
    DESCRIPTOR_SIGNAL_SPLITTER = 0x001F,
    DESCRIPTOR_SIGNAL_COMBINER = 0x0020,
    DESCRIPTOR_SIGNAL_DEMULTIPLEXER = 0x0021,
    DESCRIPTOR_SIGNAL_MULTIPLEXER = 0x0022,
    DESCRIPTOR_SIGNAL_TRANSCODER = 0x0023,
    DESCRIPTOR_CLOCK_DOMAIN = 0x0024,
    DESCRIPTOR_CONTROL_BLOCK = 0x0025
};

// Entity Capabilities
enum EntityCapabilities : uint32_t {
    EFU_MODE = 0x00000001,
    ADDRESS_ACCESS_SUPPORTED = 0x00000002,
    GATEWAY_ENTITY = 0x00000004,
    AEM_SUPPORTED = 0x00000008,
    LEGACY_AVC = 0x00000010,
    ASSOCIATION_ID_SUPPORTED = 0x00000020,
    ASSOCIATION_ID_VALID = 0x00000040,
    VENDOR_UNIQUE = 0x00000080,
    CLASS_A_SUPPORTED = 0x00000100,
    CLASS_B_SUPPORTED = 0x00000200,
    GPTP_SUPPORTED = 0x00000400,
    AEM_AUTHENTICATION_SUPPORTED = 0x00000800,
    AEM_AUTHENTICATION_REQUIRED = 0x00001000,
    AEM_PERSISTENT_ACQUIRE_SUPPORTED = 0x00002000,
    AEM_IDENTIFY_CONTROL_INDEX_VALID = 0x00004000,
    AEM_INTERFACE_INDEX_VALID = 0x00008000,
    GENERAL_CONTROLLER_IGNORE = 0x00010000,
    ENTITY_NOT_READY = 0x00020000
};

// Talker Capabilities
enum class TalkerCapabilities : uint16_t {
    IMPLEMENTED = 0x0001,
    AUDIO_SOURCE = 0x0002,
    VIDEO_SOURCE = 0x0004,
    CONTROL_SOURCE = 0x0008,
    OTHER_SOURCE = 0x0010
};

// Listener Capabilities  
enum class ListenerCapabilities : uint16_t {
    IMPLEMENTED = 0x0001,
    AUDIO_SINK = 0x0002,
    VIDEO_SINK = 0x0004,
    CONTROL_SINK = 0x0008,
    OTHER_SINK = 0x0010
};

// Controller Capabilities
enum class ControllerCapabilities : uint32_t {
    IMPLEMENTED = 0x00000001,
    LAYER3_PROXY = 0x00000002
};

// Descriptor structures
struct EntityDescriptor {
    uint64_t entity_id = 0;
    uint64_t entity_model_id = 0;
    uint32_t entity_capabilities = 0;
    uint16_t talker_stream_sources = 0;
    uint16_t talker_capabilities = 0;
    uint16_t listener_stream_sinks = 0;
    uint16_t listener_capabilities = 0;
    uint32_t controller_capabilities = 0;
    uint32_t available_index = 0;
    uint64_t association_id = 0;
    std::string entity_name;
    uint16_t vendor_name_string = 0;
    uint16_t model_name_string = 0;
    std::string firmware_version;
    std::string group_name;
    std::string serial_number;
    uint16_t configurations_count = 0;
    uint16_t current_configuration = 0;
};

struct ConfigurationDescriptor {
    std::string object_name;
    uint16_t localized_description = 0;
    uint16_t descriptor_counts_count = 0;
    uint16_t descriptor_counts_offset = 0;
    std::vector<std::pair<uint16_t, uint16_t>> descriptor_counts; // type, count pairs
};

struct AudioUnitDescriptor {
    std::string object_name;
    uint16_t localized_description = 0;
    uint16_t clock_domain_index = 0;
    uint16_t number_of_stream_input_ports = 0;
    uint16_t base_stream_input_port = 0;
    uint16_t number_of_stream_output_ports = 0;
    uint16_t base_stream_output_port = 0;
    uint16_t number_of_external_input_ports = 0;
    uint16_t base_external_input_port = 0;
    uint16_t number_of_external_output_ports = 0;
    uint16_t base_external_output_port = 0;
    uint16_t number_of_internal_input_ports = 0;
    uint16_t base_internal_input_port = 0;
    uint16_t number_of_internal_output_ports = 0;
    uint16_t base_internal_output_port = 0;
    uint16_t number_of_controls = 0;
    uint16_t base_control = 0;
    uint16_t number_of_signal_selectors = 0;
    uint16_t base_signal_selector = 0;
    uint16_t number_of_mixers = 0;
    uint16_t base_mixer = 0;
    uint16_t number_of_matrices = 0;
    uint16_t base_matrix = 0;
    uint16_t number_of_splitters = 0;
    uint16_t base_splitter = 0;
    uint16_t number_of_combiners = 0;
    uint16_t base_combiner = 0;
    uint16_t number_of_demultiplexers = 0;
    uint16_t base_demultiplexer = 0;
    uint16_t number_of_multiplexers = 0;
    uint16_t base_multiplexer = 0;
    uint16_t number_of_transcoders = 0;
    uint16_t base_transcoder = 0;
    uint16_t number_of_control_blocks = 0;
    uint16_t base_control_block = 0;
    uint32_t current_sampling_rate = 0;
    uint16_t sampling_rates_offset = 0;
    uint16_t sampling_rates_count = 0;
    std::vector<uint32_t> sampling_rates;
};

struct StreamInputDescriptor {
    std::string object_name;
    uint16_t localized_description = 0;
    uint16_t clock_domain_index = 0;
    uint16_t stream_flags = 0;
    uint64_t current_format = 0;
    uint16_t formats_offset = 0;
    uint16_t number_of_formats = 0;
    std::vector<uint64_t> stream_formats;
    uint64_t backup_talker_entity_id_0 = 0;
    uint16_t backup_talker_unique_id_0 = 0;
    uint64_t backup_talker_entity_id_1 = 0;
    uint16_t backup_talker_unique_id_1 = 0;
    uint64_t backup_talker_entity_id_2 = 0;
    uint16_t backup_talker_unique_id_2 = 0;
    uint64_t backed_up_talker_entity_id = 0;
    uint16_t backed_up_talker_unique = 0;
    uint16_t avb_interface_index = 0;
    uint32_t buffer_length = 0;
};

struct StreamOutputDescriptor {
    std::string object_name;
    uint16_t localized_description = 0;
    uint16_t clock_domain_index = 0;
    uint16_t stream_flags = 0;
    uint64_t current_format = 0;
    uint16_t formats_offset = 0;
    uint16_t number_of_formats = 0;
    std::vector<uint64_t> stream_formats;
    uint64_t backup_talker_entity_id_0 = 0;
    uint16_t backup_talker_unique_id_0 = 0;
    uint64_t backup_talker_entity_id_1 = 0;
    uint16_t backup_talker_unique_id_1 = 0;
    uint64_t backup_talker_entity_id_2 = 0;
    uint16_t backup_talker_unique_id_2 = 0;
    uint64_t backed_up_talker_entity_id = 0;
    uint16_t backed_up_talker_unique = 0;
    uint16_t avb_interface_index = 0;
    uint32_t buffer_length = 0;
};

struct AvbInterfaceDescriptor {
    std::string object_name;
    uint16_t localized_description = 0;
    std::array<uint8_t, 6> mac_address = {0};
    uint16_t interface_flags = 0;
    uint64_t clock_identity = 0;
    uint8_t priority1 = 0;
    uint8_t clock_class = 0;
    uint16_t offset_scaled_log_variance = 0;
    uint8_t clock_accuracy = 0;
    uint8_t priority2 = 0;
    uint8_t domain_number = 0;
    int8_t log_sync_interval = 0;
    int8_t log_announce_interval = 0;
    int8_t log_pdelay_interval = 0;
    uint16_t port_number = 0;
};

struct ClockSourceDescriptor {
    std::string object_name;
    uint16_t localized_description = 0;
    uint16_t clock_source_flags = 0;
    uint16_t clock_source_type = 0;
    uint64_t clock_source_identifier = 0;
    uint16_t clock_source_location_type = 0;
    uint16_t clock_source_location_index = 0;
};

struct ClockDomainDescriptor {
    std::string object_name;
    uint16_t localized_description = 0;
    uint16_t current_clock_source = 0;
    uint16_t clock_sources_offset = 0;
    uint16_t clock_sources_count = 0;
    std::vector<uint16_t> clock_sources;
};

struct StreamPortInputDescriptor {
    uint16_t clock_domain_index = 0;
    uint16_t port_flags = 0;
    uint16_t number_of_controls = 0;
    uint16_t base_control = 0;
    uint16_t number_of_clusters = 0;
    uint16_t base_cluster = 0;
    uint16_t number_of_maps = 0;
    uint16_t base_map = 0;
};

struct StreamPortOutputDescriptor {
    uint16_t clock_domain_index = 0;
    uint16_t port_flags = 0;
    uint16_t number_of_controls = 0;
    uint16_t base_control = 0;
    uint16_t number_of_clusters = 0;
    uint16_t base_cluster = 0;
    uint16_t number_of_maps = 0;
    uint16_t base_map = 0;
};

// Utility functions for AEM operations
inline void safe_string_copy(char* dest, const char* src, size_t dest_size) {
    if (dest && src && dest_size > 0) {
        size_t src_len = strlen(src);
        size_t copy_len = (src_len < dest_size - 1) ? src_len : dest_size - 1;
        memcpy(dest, src, copy_len);
        dest[copy_len] = '\0';
    }
}

inline bool validate_entity_descriptor(const EntityDescriptor& desc) {
    // Basic validation - check required fields
    return desc.entity_id != 0 && 
           desc.entity_model_id != 0 &&
           !desc.entity_name.empty();
}

} // namespace AEM

// ADPDU (AVDECC Discovery Protocol Data Unit) namespace
namespace ADPDU {

// ADP Message Types
enum class ADPMessageType : uint8_t {
    ENTITY_AVAILABLE = 0,
    ENTITY_DEPARTING = 1,
    ENTITY_DISCOVER = 2
};

// ADP Constants
constexpr uint8_t ADP_ENTITY_AVAILABLE = 0;
constexpr uint8_t ADP_ENTITY_DEPARTING = 1;
constexpr uint8_t ADP_ENTITY_DISCOVERY_REQUEST = 2;

class ATDECCDiscoveryProtocolPDU {
public:
    uint8_t message_type = 0;
    uint64_t entity_id = 0;
    uint64_t entity_model_id = 0;
    uint32_t entity_capabilities = 0;
    uint16_t talker_stream_sources = 0;
    uint16_t talker_capabilities = 0;
    uint16_t listener_stream_sinks = 0;
    uint16_t listener_capabilities = 0;
    uint32_t controller_capabilities = 0;
    uint32_t available_index = 0;
    uint64_t gptp_grandmaster_id = 0;
    uint8_t gptp_domain_number = 0;
    uint16_t identify_control_index = 0;
    uint16_t interface_index = 0;
    uint64_t association_id = 0;
    
    // Methods for test compatibility
    void set_entity_id(uint64_t id) { entity_id = id; }
    void set_entity_model_id(uint64_t model_id) { entity_model_id = model_id; }
    void set_entity_capabilities(AEM::EntityCapabilities caps) { 
        entity_capabilities = static_cast<uint32_t>(caps); 
    }
    void set_talker_info(uint16_t sources, AEM::TalkerCapabilities caps) {
        talker_stream_sources = sources;
        talker_capabilities = static_cast<uint16_t>(caps);
    }
    void set_listener_info(uint16_t sinks, AEM::ListenerCapabilities caps) {
        listener_stream_sinks = sinks;
        listener_capabilities = static_cast<uint16_t>(caps);
    }
    void set_controller_capabilities(AEM::ControllerCapabilities caps) {
        controller_capabilities = static_cast<uint32_t>(caps);
    }
    
    // Serialization methods for test compatibility
    void serialize() {
        // Convert to network byte order for real implementation
        // For testing, this is a no-op
    }
    
    const uint8_t* get_raw_octets() const {
        return reinterpret_cast<const uint8_t*>(this);
    }
    
    size_t get_size() const {
        return sizeof(*this);
    }
    
    bool is_valid() const {
        return entity_id != 0 && entity_model_id != 0;
    }
    
    // Constructor for deserialization testing
    ATDECCDiscoveryProtocolPDU() = default;
    ATDECCDiscoveryProtocolPDU(const uint8_t* data, size_t length) {
        if (data && length >= sizeof(*this)) {
            memcpy(this, data, sizeof(*this));
        }
    }
};

} // namespace ADPDU

// ACMP (AVDECC Connection Management Protocol) namespace  
namespace ACMP {

// ACMP Message Types
enum class ACMP_Message_type : uint8_t {
    CONNECT_TX_COMMAND = 0,
    CONNECT_TX_RESPONSE = 1,
    DISCONNECT_TX_COMMAND = 2,
    DISCONNECT_TX_RESPONSE = 3,
    GET_TX_STATE_COMMAND = 4,
    GET_TX_STATE_RESPONSE = 5,
    CONNECT_RX_COMMAND = 6,
    CONNECT_RX_RESPONSE = 7,
    DISCONNECT_RX_COMMAND = 8,
    DISCONNECT_RX_RESPONSE = 9,
    GET_RX_STATE_COMMAND = 10,
    GET_RX_STATE_RESPONSE = 11,
    GET_TX_CONNECTION_COMMAND = 12,
    GET_TX_CONNECTION_RESPONSE = 13
};
enum class ACMPMessageType : uint8_t {
    CONNECT_TX_COMMAND = 0,
    CONNECT_TX_RESPONSE = 1,
    DISCONNECT_TX_COMMAND = 2,
    DISCONNECT_TX_RESPONSE = 3,
    GET_TX_STATE_COMMAND = 4,
    GET_TX_STATE_RESPONSE = 5,
    CONNECT_RX_COMMAND = 6,
    CONNECT_RX_RESPONSE = 7,
    DISCONNECT_RX_COMMAND = 8,
    DISCONNECT_RX_RESPONSE = 9,
    GET_RX_STATE_COMMAND = 10,
    GET_RX_STATE_RESPONSE = 11,
    GET_TX_CONNECTION_COMMAND = 12,
    GET_TX_CONNECTION_RESPONSE = 13
};

// ACMP Status Codes
enum class ACMPStatusCode : uint8_t {
    SUCCESS = 0,
    LISTENER_UNKNOWN_ID = 1,
    TALKER_UNKNOWN_ID = 2,
    TALKER_DEST_MAC_FAIL = 3,
    TALKER_NO_STREAM_INDEX = 4,
    TALKER_NO_BANDWIDTH = 5,
    TALKER_EXCLUSIVE = 6,
    LISTENER_TALKER_TIMEOUT = 7,
    LISTENER_EXCLUSIVE = 8,
    STATE_UNAVAILABLE = 9,
    NOT_CONNECTED = 10,
    NO_SUCH_CONNECTION = 11,
    COULD_NOT_SEND_MESSAGE = 12,
    TALKER_MISBEHAVING = 13,
    LISTENER_MISBEHAVING = 14,
    RESERVED = 15,
    CONTROLLER_NOT_AUTHORIZED = 16,
    INCOMPATIBLE_REQUEST = 17,
    LISTENER_INVALID_CONNECTION = 18,
    NOT_SUPPORTED = 31
};

// Constants for test compatibility
constexpr uint8_t CONNECT_TX_COMMAND = 0;
constexpr uint8_t GET_TX_STATE_RESPONSE = 5;
constexpr uint8_t SUCCESS = 0;

class ATDECCConnectionManagementProtocolPDU {
public:
    uint8_t message_type = 0;
    uint64_t talker_entity_id = 0;
    uint64_t listener_entity_id = 0;
    uint16_t talker_unique_id = 0;
    uint16_t listener_unique_id = 0;
    std::array<uint8_t, 6> stream_dest_mac = {0};
    uint16_t connection_count = 0;
    uint16_t sequence_id = 0;
    uint16_t flags = 0;
    uint16_t stream_vlan_id = 0;
    uint16_t reserved = 0;
    
    // Methods for test compatibility
    void serialize() {
        // Convert to network byte order for real implementation
        // For testing, this is a no-op
    }
    
    void set_message_type(ACMP_Message_type type) {
        message_type = static_cast<uint8_t>(type);
    }
    
    const uint8_t* get_raw_octets() const {
        return reinterpret_cast<const uint8_t*>(this);
    }
    
    size_t get_size() const {
        return sizeof(*this);
    }
    
    bool is_valid() const {
        return talker_entity_id != 0 || listener_entity_id != 0;
    }
    
    // Constructor
    ATDECCConnectionManagementProtocolPDU() = default;
    ATDECCConnectionManagementProtocolPDU(const uint8_t* data, size_t length) {
        if (data && length >= sizeof(*this)) {
            memcpy(this, data, sizeof(*this));
        }
    }
};

} // namespace ACMP

// Protocol Constants
constexpr std::array<uint8_t, 6> AVDECC_MULTICAST_MAC = {0x91, 0xE0, 0xF0, 0x01, 0x00, 0x00};
constexpr uint16_t AVDECC_ETHERTYPE = 0x22F0;

// ACMP Message Type Constants for production code compatibility
constexpr uint8_t CONNECT_TX_COMMAND = 0;
constexpr uint8_t CONNECT_TX_RESPONSE = 1;
constexpr uint8_t DISCONNECT_TX_COMMAND = 2;
constexpr uint8_t DISCONNECT_TX_RESPONSE = 3;
constexpr uint8_t GET_TX_STATE_COMMAND = 4;
constexpr uint8_t GET_TX_STATE_RESPONSE = 5;
constexpr uint8_t CONNECT_RX_COMMAND = 6;
constexpr uint8_t CONNECT_RX_RESPONSE = 7;
constexpr uint8_t DISCONNECT_RX_COMMAND = 8;
constexpr uint8_t DISCONNECT_RX_RESPONSE = 9;
constexpr uint8_t GET_RX_STATE_COMMAND = 10;
constexpr uint8_t GET_RX_STATE_RESPONSE = 11;
constexpr uint8_t GET_TX_CONNECTION_COMMAND = 12;
constexpr uint8_t GET_TX_CONNECTION_RESPONSE = 13;

// ACMP Status Code Constants for production code compatibility
constexpr uint8_t SUCCESS = 0;
constexpr uint8_t LISTENER_UNKNOWN_ID = 1;
constexpr uint8_t TALKER_UNKNOWN_ID = 2;
constexpr uint8_t TALKER_DEST_MAC_FAIL = 3;
constexpr uint8_t TALKER_NO_STREAM_INDEX = 4;
constexpr uint8_t TALKER_NO_BANDWIDTH = 5;
constexpr uint8_t TALKER_EXCLUSIVE = 6;
constexpr uint8_t LISTENER_TALKER_TIMEOUT = 7;
constexpr uint8_t LISTENER_EXCLUSIVE = 8;
constexpr uint8_t STATE_UNAVAILABLE = 9;
constexpr uint8_t NOT_CONNECTED = 10;
constexpr uint8_t NO_SUCH_CONNECTION = 11;
constexpr uint8_t COULD_NOT_SEND_MESSAGE = 12;
constexpr uint8_t TALKER_MISBEHAVING = 13;
constexpr uint8_t LISTENER_MISBEHAVING = 14;
constexpr uint8_t RESERVED = 15;
constexpr uint8_t CONTROLLER_NOT_AUTHORIZED = 16;
constexpr uint8_t INCOMPATIBLE_REQUEST = 17;
constexpr uint8_t LISTENER_INVALID_CONNECTION = 18;
constexpr uint8_t NOT_SUPPORTED = 31;

} // namespace _2021  
} // namespace _1722_1
} // namespace IEEE
