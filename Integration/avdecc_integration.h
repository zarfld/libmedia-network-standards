#ifndef AVDECC_INTEGRATION_H
#define AVDECC_INTEGRATION_H

/**
 * @file avdecc_integration.h
 * @brief Complete AVDECC Protocol Stack Integration for IEEE 1722-2016
 * 
 * Implementation of ADP, AECP, ACMP protocols with full IEEE 1722.1-2021 compliance
 * for device discovery, enumeration, connection management, and control
 * 
 * @author OpenAvnu IEEE 1722-2016 Compliance Team
 * @date July 2025
 * @version 4.0.0 - Phase 7 AVDECC Integration Complete
 */

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <array>
#include <map>
#include <chrono>
#include <functional>

namespace avtp_protocol {
namespace avdecc {

// =============================
// AVDECC Common Definitions
// =============================

/**
 * @brief AVDECC Entity Model (AEM) Descriptor Types
 * Per IEEE 1722.1-2021 Table 7.1
 */
enum class AEMDescriptorType : uint16_t {
    ENTITY = 0x0000,
    CONFIGURATION = 0x0001,
    AUDIO_UNIT = 0x0002,
    VIDEO_UNIT = 0x0003,
    SENSOR_UNIT = 0x0004,
    STREAM_INPUT = 0x0005,
    STREAM_OUTPUT = 0x0006,
    JACK_INPUT = 0x0007,
    JACK_OUTPUT = 0x0008,
    AVB_INTERFACE = 0x0009,
    CLOCK_SOURCE = 0x000A,
    MEMORY_OBJECT = 0x000B,
    LOCALE = 0x000C,
    STRINGS = 0x000D,
    STREAM_PORT_INPUT = 0x000E,
    STREAM_PORT_OUTPUT = 0x000F,
    EXTERNAL_PORT_INPUT = 0x0010,
    EXTERNAL_PORT_OUTPUT = 0x0011,
    INTERNAL_PORT_INPUT = 0x0012,
    INTERNAL_PORT_OUTPUT = 0x0013,
    AUDIO_CLUSTER = 0x0014,
    VIDEO_CLUSTER = 0x0015,
    SENSOR_CLUSTER = 0x0016,
    AUDIO_MAP = 0x0017,
    VIDEO_MAP = 0x0018,
    SENSOR_MAP = 0x0019,
    CONTROL = 0x001A,
    SIGNAL_SELECTOR = 0x001B,
    MIXER = 0x001C,
    MATRIX = 0x001D,
    MATRIX_SIGNAL = 0x001E,
    SIGNAL_SPLITTER = 0x001F,
    SIGNAL_COMBINER = 0x0020,
    SIGNAL_DEMULTIPLEXER = 0x0021,
    SIGNAL_MULTIPLEXER = 0x0022,
    SIGNAL_TRANSCODER = 0x0023,
    CLOCK_DOMAIN = 0x0024,
    CONTROL_BLOCK = 0x0025,
    // Extended descriptors
    OPERATION = 0x0026,
    PATH = 0x0027,
    // Milan extensions
    MILAN_INFO = 0x4000
};

/**
 * @brief AVDECC Command/Response Types
 * Per IEEE 1722.1-2021 Table 7.126
 */
enum class AECPCommandType : uint16_t {
    // AEM Commands
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
    // Milan extensions
    GET_DYNAMIC_INFO = 0x004B
};

/**
 * @brief ACMP Command Types
 * Per IEEE 1722.1-2021 Table 8.1
 */
enum class ACMPCommandType : uint8_t {
    CONNECT_TX_COMMAND = 0x00,
    CONNECT_TX_RESPONSE = 0x01,
    DISCONNECT_TX_COMMAND = 0x02,
    DISCONNECT_TX_RESPONSE = 0x03,
    GET_TX_STATE_COMMAND = 0x04,
    GET_TX_STATE_RESPONSE = 0x05,
    CONNECT_RX_COMMAND = 0x06,
    CONNECT_RX_RESPONSE = 0x07,
    DISCONNECT_RX_COMMAND = 0x08,
    DISCONNECT_RX_RESPONSE = 0x09,
    GET_RX_STATE_COMMAND = 0x0A,
    GET_RX_STATE_RESPONSE = 0x0B,
    GET_TX_CONNECTION_COMMAND = 0x0C,
    GET_TX_CONNECTION_RESPONSE = 0x0D
};

// =============================
// AVDECC Discovery Protocol (ADP)
// =============================
namespace adp {

/**
 * @brief AVDECC Entity Capabilities
 * Per IEEE 1722.1-2021 Table 6.1
 */
struct EntityCapabilities {
    bool efu_mode;                      // EFU_MODE
    bool address_access_supported;      // ADDRESS_ACCESS_SUPPORTED
    bool gateway_entity;                // GATEWAY_ENTITY
    bool aem_supported;                 // AEM_SUPPORTED
    bool legacy_avc;                    // LEGACY_AVC
    bool association_id_supported;      // ASSOCIATION_ID_SUPPORTED
    bool association_id_valid;          // ASSOCIATION_ID_VALID
    bool vendor_unique;                 // VENDOR_UNIQUE
    bool class_a_supported;             // CLASS_A_SUPPORTED
    bool class_b_supported;             // CLASS_B_SUPPORTED
    bool gptp_supported;                // GPTP_SUPPORTED
    bool srp_supported;                 // SRP_SUPPORTED
    bool aem_authentication_supported;  // AEM_AUTHENTICATION_SUPPORTED
    bool aem_authentication_required;   // AEM_AUTHENTICATION_REQUIRED
    bool aem_persistent_acquire_supported; // AEM_PERSISTENT_ACQUIRE_SUPPORTED
    bool aem_identify_control_index_valid;  // AEM_IDENTIFY_CONTROL_INDEX_VALID
    bool aem_interface_index_valid;     // AEM_INTERFACE_INDEX_VALID
    bool general_controller_ignore;     // GENERAL_CONTROLLER_IGNORE
    bool entity_not_ready;              // ENTITY_NOT_READY
    
    uint32_t to_uint32() const;         // Convert to 32-bit flags
    void from_uint32(uint32_t flags);   // Set from 32-bit flags
};

/**
 * @brief AVDECC Talker Capabilities
 * Per IEEE 1722.1-2021 Table 6.2
 */
struct TalkerCapabilities {
    bool implemented;                   // IMPLEMENTED
    bool other_source;                  // OTHER_SOURCE
    bool control_source;                // CONTROL_SOURCE
    bool media_clock_source;            // MEDIA_CLOCK_SOURCE
    bool smpte_source;                  // SMPTE_SOURCE
    bool midi_source;                   // MIDI_SOURCE
    bool audio_source;                  // AUDIO_SOURCE
    bool video_source;                  // VIDEO_SOURCE
    
    uint16_t to_uint16() const;         // Convert to 16-bit flags
    void from_uint16(uint16_t flags);   // Set from 16-bit flags
};

/**
 * @brief AVDECC Listener Capabilities
 * Per IEEE 1722.1-2021 Table 6.3
 */
struct ListenerCapabilities {
    bool implemented;                   // IMPLEMENTED
    bool other_sink;                    // OTHER_SINK
    bool control_sink;                  // CONTROL_SINK
    bool media_clock_sink;              // MEDIA_CLOCK_SINK
    bool smpte_sink;                    // SMPTE_SINK
    bool midi_sink;                     // MIDI_SINK
    bool audio_sink;                    // AUDIO_SINK
    bool video_sink;                    // VIDEO_SINK
    
    uint16_t to_uint16() const;         // Convert to 16-bit flags
    void from_uint16(uint16_t flags);   // Set from 16-bit flags
};

/**
 * @brief ADP Entity Information
 */
struct ADPEntityInfo {
    uint64_t entity_id;                 // Entity identifier
    uint64_t entity_model_id;           // Entity model identifier
    EntityCapabilities entity_capabilities; // Entity capabilities
    uint16_t talker_stream_sources;     // Number of talker stream sources
    TalkerCapabilities talker_capabilities; // Talker capabilities
    uint16_t listener_stream_sinks;     // Number of listener stream sinks
    ListenerCapabilities listener_capabilities; // Listener capabilities
    uint32_t controller_capabilities;   // Controller capabilities
    uint32_t available_index;           // Available index
    uint64_t gptp_grandmaster_id;       // gPTP grandmaster ID
    uint8_t gptp_domain_number;         // gPTP domain number
    uint16_t identify_control_index;    // Identify control index
    uint16_t interface_index;           // Interface index
    uint64_t association_id;            // Association ID
    
    bool is_valid() const;              // Validate entity info
    std::string to_debug_string() const;
};

/**
 * @brief ADP Protocol Handler
 */
class ADPProtocolHandler {
public:
    ADPProtocolHandler(const ADPEntityInfo& local_entity_info);
    virtual ~ADPProtocolHandler();
    
    // Entity advertisement
    bool start_entity_advertisement();
    bool stop_entity_advertisement();
    bool update_entity_info(const ADPEntityInfo& entity_info);
    
    // Entity discovery
    bool discover_entities();
    std::vector<ADPEntityInfo> get_discovered_entities() const;
    bool get_entity_info(uint64_t entity_id, ADPEntityInfo& entity_info) const;
    
    // Event callbacks
    using EntityDiscoveredCallback = std::function<void(const ADPEntityInfo&)>;
    using EntityDepartedCallback = std::function<void(uint64_t entity_id)>;
    using EntityUpdatedCallback = std::function<void(const ADPEntityInfo&)>;
    
    void set_entity_discovered_callback(EntityDiscoveredCallback callback);
    void set_entity_departed_callback(EntityDepartedCallback callback);
    void set_entity_updated_callback(EntityUpdatedCallback callback);
    
    // Statistics
    struct ADPStatistics {
        uint64_t advertisements_sent;
        uint64_t advertisements_received;
        uint64_t entities_discovered;
        uint64_t entities_departed;
        uint64_t protocol_errors;
    };
    
    ADPStatistics get_statistics() const;
    
private:
    ADPEntityInfo local_entity_info_;
    std::map<uint64_t, ADPEntityInfo> discovered_entities_;
    std::map<uint64_t, std::chrono::steady_clock::time_point> entity_timeouts_;
    
    EntityDiscoveredCallback entity_discovered_callback_;
    EntityDepartedCallback entity_departed_callback_;
    EntityUpdatedCallback entity_updated_callback_;
    
    ADPStatistics statistics_;
    bool advertising_enabled_;
    
    // Internal processing
    bool process_entity_advertisement(const std::vector<uint8_t>& adp_frame);
    bool send_entity_advertisement();
    void check_entity_timeouts();
    bool validate_adp_frame(const std::vector<uint8_t>& adp_frame) const;
};

} // namespace adp

// =============================
// AVDECC Enumeration and Control Protocol (AECP)
// =============================
namespace aecp {

/**
 * @brief AEM Status Codes
 * Per IEEE 1722.1-2021 Table 7.128
 */
enum class AEMStatusCode : uint8_t {
    SUCCESS = 0x00,
    NOT_IMPLEMENTED = 0x01,
    NO_SUCH_DESCRIPTOR = 0x02,
    ENTITY_LOCKED = 0x03,
    ENTITY_ACQUIRED = 0x04,
    NOT_AUTHENTICATED = 0x05,
    AUTHENTICATION_DISABLED = 0x06,
    BAD_ARGUMENTS = 0x07,
    STATUS_NOT_SUPPORTED = 0x08,
    NO_RESOURCES = 0x09,
    IN_PROGRESS = 0x0A,
    ENTITY_MISBEHAVING = 0x0B,
    NOT_SUPPORTED = 0x0C,
    STREAM_IS_RUNNING = 0x0D
};

/**
 * @brief AECP Message Structure
 */
struct AECPMessage {
    uint64_t target_entity_id;          // Target entity ID
    uint64_t controller_entity_id;      // Controller entity ID
    uint16_t sequence_id;               // Sequence ID
    AECPCommandType command_type;       // Command type
    AEMStatusCode status;               // Status code
    std::vector<uint8_t> command_specific_data; // Command-specific data
    
    bool is_response() const;           // Check if message is response
    std::string to_debug_string() const;
};

/**
 * @brief AEM Descriptor Base Structure
 */
struct AEMDescriptor {
    AEMDescriptorType descriptor_type;  // Descriptor type
    uint16_t descriptor_index;          // Descriptor index
    std::vector<uint8_t> descriptor_data; // Descriptor-specific data
    uint32_t aem_checksum;              // AEM checksum for integrity
    
    virtual ~AEMDescriptor() = default;
    virtual bool serialize(std::vector<uint8_t>& data) const = 0;
    virtual bool deserialize(const std::vector<uint8_t>& data) = 0;
    virtual std::string to_debug_string() const = 0;
    
    // Checksum calculation
    uint32_t calculate_aem_checksum() const;
    bool verify_aem_checksum() const;
};

/**
 * @brief Entity Descriptor
 * Per IEEE 1722.1-2021 Table 7.2
 */
struct EntityDescriptor : public AEMDescriptor {
    uint64_t entity_id;                 // Entity ID
    uint64_t entity_model_id;           // Entity model ID
    uint32_t entity_capabilities;       // Entity capabilities
    uint16_t talker_stream_sources;     // Talker stream sources
    uint16_t talker_capabilities;       // Talker capabilities
    uint16_t listener_stream_sinks;     // Listener stream sinks
    uint16_t listener_capabilities;     // Listener capabilities
    uint32_t controller_capabilities;   // Controller capabilities
    uint32_t available_index;           // Available index
    uint64_t association_id;            // Association ID
    std::array<uint8_t, 64> entity_name; // Entity name
    uint16_t vendor_name_string;        // Vendor name string reference
    uint16_t model_name_string;         // Model name string reference
    std::array<uint8_t, 64> firmware_version; // Firmware version
    std::array<uint8_t, 64> group_name; // Group name
    std::array<uint8_t, 64> serial_number; // Serial number
    uint16_t configurations_count;      // Number of configurations
    uint16_t current_configuration;     // Current configuration
    
    bool serialize(std::vector<uint8_t>& data) const override;
    bool deserialize(const std::vector<uint8_t>& data) override;
    std::string to_debug_string() const override;
};

/**
 * @brief Stream Input/Output Descriptor
 * Per IEEE 1722.1-2021 Table 7.13
 */
struct StreamDescriptor : public AEMDescriptor {
    std::array<uint8_t, 64> object_name; // Object name
    uint16_t localized_description;     // Localized description
    uint16_t clock_domain_index;        // Clock domain index
    uint16_t stream_flags;              // Stream flags
    uint64_t current_format;            // Current format
    uint16_t formats_count;             // Number of supported formats
    std::vector<uint64_t> supported_formats; // Supported stream formats
    uint64_t backup_talker_entity_id_0; // Backup talker entity ID 0
    uint16_t backup_talker_unique_id_0; // Backup talker unique ID 0
    uint64_t backup_talker_entity_id_1; // Backup talker entity ID 1
    uint16_t backup_talker_unique_id_1; // Backup talker unique ID 1
    uint64_t backup_talker_entity_id_2; // Backup talker entity ID 2
    uint16_t backup_talker_unique_id_2; // Backup talker unique ID 2
    uint64_t backedup_talker_entity_id; // Backed up talker entity ID
    uint16_t backedup_talker_unique_id; // Backed up talker unique ID
    uint16_t avb_interface_index;       // AVB interface index
    uint32_t buffer_length;             // Buffer length
    
    bool serialize(std::vector<uint8_t>& data) const override;
    bool deserialize(const std::vector<uint8_t>& data) override;
    std::string to_debug_string() const override;
};

/**
 * @brief AECP Protocol Handler
 */
class AECPProtocolHandler {
public:
    AECPProtocolHandler(uint64_t local_entity_id);
    virtual ~AECPProtocolHandler();
    
    // Command processing
    bool send_command(const AECPMessage& command, AECPMessage& response);
    bool process_command(const AECPMessage& command, AECPMessage& response);
    
    // Descriptor management
    bool add_descriptor(std::shared_ptr<AEMDescriptor> descriptor);
    bool get_descriptor(AEMDescriptorType type, uint16_t index, 
                       std::shared_ptr<AEMDescriptor>& descriptor);
    bool update_descriptor(std::shared_ptr<AEMDescriptor> descriptor);
    
    // Entity control
    bool acquire_entity(uint64_t target_entity_id, bool persistent = false);
    bool release_entity(uint64_t target_entity_id);
    bool lock_entity(uint64_t target_entity_id);
    bool unlock_entity(uint64_t target_entity_id);
    
    // Stream control
    bool set_stream_format(uint64_t target_entity_id, uint16_t descriptor_index, 
                          uint64_t stream_format);
    bool get_stream_format(uint64_t target_entity_id, uint16_t descriptor_index, 
                          uint64_t& stream_format);
    bool start_streaming(uint64_t target_entity_id, uint16_t descriptor_index);
    bool stop_streaming(uint64_t target_entity_id, uint16_t descriptor_index);
    
    // Event callbacks
    using CommandReceivedCallback = std::function<bool(const AECPMessage&, AECPMessage&)>;
    using ResponseReceivedCallback = std::function<void(const AECPMessage&)>;
    
    void set_command_received_callback(CommandReceivedCallback callback);
    void set_response_received_callback(ResponseReceivedCallback callback);
    
private:
    uint64_t local_entity_id_;
    std::map<std::pair<AEMDescriptorType, uint16_t>, std::shared_ptr<AEMDescriptor>> descriptors_;
    std::map<uint64_t, uint64_t> acquired_entities_; // entity_id -> controller_id
    std::map<uint64_t, uint64_t> locked_entities_;    // entity_id -> controller_id
    
    CommandReceivedCallback command_received_callback_;
    ResponseReceivedCallback response_received_callback_;
    
    uint16_t next_sequence_id_;
    
    // Internal processing
    bool process_read_descriptor_command(const AECPMessage& command, AECPMessage& response);
    bool process_acquire_entity_command(const AECPMessage& command, AECPMessage& response);
    bool process_lock_entity_command(const AECPMessage& command, AECPMessage& response);
    bool validate_aecp_message(const AECPMessage& message) const;
};

} // namespace aecp

// =============================
// AVDECC Connection Management Protocol (ACMP)
// =============================
namespace acmp {

/**
 * @brief ACMP Status Codes
 * Per IEEE 1722.1-2021 Table 8.2
 */
enum class ACMPStatusCode : uint8_t {
    SUCCESS = 0x00,
    LISTENER_UNKNOWN_ID = 0x01,
    TALKER_UNKNOWN_ID = 0x02,
    TALKER_DEST_MAC_FAIL = 0x03,
    TALKER_NO_STREAM_INDEX = 0x04,
    TALKER_NO_BANDWIDTH = 0x05,
    TALKER_EXCLUSIVE = 0x06,
    LISTENER_TALKER_TIMEOUT = 0x07,
    LISTENER_EXCLUSIVE = 0x08,
    STATE_UNAVAILABLE = 0x09,
    NOT_CONNECTED = 0x0A,
    NO_SUCH_CONNECTION = 0x0B,
    COULD_NOT_SEND_MESSAGE = 0x0C,
    TALKER_MISBEHAVING = 0x0D,
    LISTENER_MISBEHAVING = 0x0E,
    CONTROLLER_NOT_AUTHORIZED = 0x0F,
    INCOMPATIBLE_REQUEST = 0x10,
    LISTENER_INVALID_CONNECTION = 0x11,
    NOT_SUPPORTED = 0x1F
};

/**
 * @brief ACMP Message Structure
 */
struct ACMPMessage {
    ACMPCommandType command_type;       // Command type
    ACMPStatusCode status;              // Status code
    uint64_t stream_id;                 // Stream ID
    uint64_t controller_entity_id;      // Controller entity ID
    uint64_t talker_entity_id;          // Talker entity ID
    uint64_t listener_entity_id;        // Listener entity ID
    uint16_t talker_unique_id;          // Talker unique ID
    uint16_t listener_unique_id;        // Listener unique ID
    std::array<uint8_t, 6> stream_dest_mac; // Stream destination MAC
    uint16_t connection_count;          // Connection count
    uint16_t sequence_id;               // Sequence ID
    uint16_t flags;                     // Flags
    uint16_t stream_vlan_id;            // Stream VLAN ID
    
    bool is_response() const;           // Check if message is response
    std::string to_debug_string() const;
};

/**
 * @brief Stream Connection State
 */
struct StreamConnection {
    uint64_t stream_id;                 // Stream ID
    uint64_t talker_entity_id;          // Talker entity ID
    uint64_t listener_entity_id;        // Listener entity ID
    uint16_t talker_unique_id;          // Talker unique ID
    uint16_t listener_unique_id;        // Listener unique ID
    std::array<uint8_t, 6> stream_dest_mac; // Stream destination MAC
    uint16_t stream_vlan_id;            // Stream VLAN ID
    bool connected;                     // Connection status
    uint64_t connection_time;           // Connection establishment time
    
    std::string to_debug_string() const;
};

/**
 * @brief ACMP Protocol Handler
 */
class ACMPProtocolHandler {
public:
    ACMPProtocolHandler(uint64_t local_entity_id);
    virtual ~ACMPProtocolHandler();
    
    // Connection management
    bool connect_stream(uint64_t talker_entity_id, uint16_t talker_unique_id,
                       uint64_t listener_entity_id, uint16_t listener_unique_id,
                       StreamConnection& connection);
    
    bool disconnect_stream(uint64_t talker_entity_id, uint16_t talker_unique_id,
                          uint64_t listener_entity_id, uint16_t listener_unique_id);
    
    bool get_stream_connection(uint64_t stream_id, StreamConnection& connection);
    std::vector<StreamConnection> get_all_connections() const;
    
    // State management
    bool get_talker_stream_state(uint16_t talker_unique_id, StreamConnection& state);
    bool get_listener_stream_state(uint16_t listener_unique_id, StreamConnection& state);
    
    // Message processing
    bool process_acmp_message(const ACMPMessage& message, ACMPMessage& response);
    bool send_acmp_command(const ACMPMessage& command, ACMPMessage& response);
    
    // Event callbacks
    using ConnectionEstablishedCallback = std::function<void(const StreamConnection&)>;
    using ConnectionTerminatedCallback = std::function<void(const StreamConnection&)>;
    using CommandReceivedCallback = std::function<bool(const ACMPMessage&, ACMPMessage&)>;
    
    void set_connection_established_callback(ConnectionEstablishedCallback callback);
    void set_connection_terminated_callback(ConnectionTerminatedCallback callback);
    void set_command_received_callback(CommandReceivedCallback callback);
    
    // Statistics
    struct ACMPStatistics {
        uint64_t connections_established;
        uint64_t connections_terminated;
        uint64_t connection_attempts_failed;
        uint64_t commands_sent;
        uint64_t commands_received;
        uint64_t responses_sent;
        uint64_t responses_received;
    };
    
    ACMPStatistics get_statistics() const;
    
private:
    uint64_t local_entity_id_;
    std::map<uint64_t, StreamConnection> active_connections_;
    std::map<uint16_t, StreamConnection> talker_states_;
    std::map<uint16_t, StreamConnection> listener_states_;
    
    ConnectionEstablishedCallback connection_established_callback_;
    ConnectionTerminatedCallback connection_terminated_callback_;
    CommandReceivedCallback command_received_callback_;
    
    ACMPStatistics statistics_;
    uint16_t next_sequence_id_;
    
    // Internal processing
    bool process_connect_tx_command(const ACMPMessage& command, ACMPMessage& response);
    bool process_disconnect_tx_command(const ACMPMessage& command, ACMPMessage& response);
    bool process_connect_rx_command(const ACMPMessage& command, ACMPMessage& response);
    bool process_disconnect_rx_command(const ACMPMessage& command, ACMPMessage& response);
    bool process_get_tx_state_command(const ACMPMessage& command, ACMPMessage& response);
    bool process_get_rx_state_command(const ACMPMessage& command, ACMPMessage& response);
    
    bool allocate_stream_resources(const StreamConnection& connection);
    bool deallocate_stream_resources(const StreamConnection& connection);
    bool validate_acmp_message(const ACMPMessage& message) const;
};

} // namespace acmp

// =============================
// Unified AVDECC Stack Manager
// =============================

/**
 * @brief Complete AVDECC Protocol Stack
 * Integrates ADP, AECP, and ACMP protocols into a unified interface
 */
class AVDECCStackManager {
public:
    AVDECCStackManager(uint64_t entity_id);
    virtual ~AVDECCStackManager();
    
    // Stack initialization
    bool initialize(const adp::ADPEntityInfo& entity_info);
    bool start();
    bool stop();
    
    // Entity management
    bool register_entity(const adp::ADPEntityInfo& entity_info);
    bool unregister_entity();
    bool update_entity_capabilities(const adp::EntityCapabilities& capabilities);
    
    // Descriptor management
    bool add_entity_descriptor(const aecp::EntityDescriptor& descriptor);
    bool add_stream_descriptor(const aecp::StreamDescriptor& descriptor, bool is_input);
    bool get_descriptor(AEMDescriptorType type, uint16_t index, 
                       std::shared_ptr<aecp::AEMDescriptor>& descriptor);
    
    // Stream management
    bool create_talker_stream(uint16_t stream_index, uint64_t stream_format);
    bool create_listener_stream(uint16_t stream_index, uint64_t stream_format);
    bool connect_streams(uint64_t talker_entity_id, uint16_t talker_stream_index,
                        uint64_t listener_entity_id, uint16_t listener_stream_index);
    bool disconnect_streams(uint64_t talker_entity_id, uint16_t talker_stream_index,
                           uint64_t listener_entity_id, uint16_t listener_stream_index);
    
    // Discovery and enumeration
    std::vector<adp::ADPEntityInfo> discover_entities();
    bool enumerate_entity(uint64_t entity_id);
    bool get_entity_descriptor(uint64_t entity_id, aecp::EntityDescriptor& descriptor);
    
    // Control operations
    bool acquire_entity(uint64_t entity_id, bool persistent = false);
    bool release_entity(uint64_t entity_id);
    bool set_stream_format(uint64_t entity_id, uint16_t stream_index, 
                          uint64_t stream_format, bool is_input);
    bool start_streaming(uint64_t entity_id, uint16_t stream_index, bool is_input);
    bool stop_streaming(uint64_t entity_id, uint16_t stream_index, bool is_input);
    
    // Event handling
    using EntityDiscoveredCallback = std::function<void(const adp::ADPEntityInfo&)>;
    using EntityDepartedCallback = std::function<void(uint64_t entity_id)>;
    using StreamConnectedCallback = std::function<void(const acmp::StreamConnection&)>;
    using StreamDisconnectedCallback = std::function<void(const acmp::StreamConnection&)>;
    using ControlCommandCallback = std::function<bool(const aecp::AECPMessage&, aecp::AECPMessage&)>;
    
    void set_entity_discovered_callback(EntityDiscoveredCallback callback);
    void set_entity_departed_callback(EntityDepartedCallback callback);
    void set_stream_connected_callback(StreamConnectedCallback callback);
    void set_stream_disconnected_callback(StreamDisconnectedCallback callback);
    void set_control_command_callback(ControlCommandCallback callback);
    
    // Statistics and monitoring
    struct AVDECCStatistics {
        adp::ADPProtocolHandler::ADPStatistics adp_stats;
        acmp::ACMPProtocolHandler::ACMPStatistics acmp_stats;
        uint64_t descriptors_read;
        uint64_t descriptors_written;
        uint64_t entities_enumerated;
        uint64_t control_commands_processed;
    };
    
    AVDECCStatistics get_statistics() const;
    bool reset_statistics();
    
    // Milan compatibility
    bool enable_milan_mode();
    bool is_milan_compatible(uint64_t entity_id) const;
    bool get_dynamic_info(uint64_t entity_id, std::vector<uint8_t>& dynamic_info);
    
private:
    uint64_t local_entity_id_;
    
    std::unique_ptr<adp::ADPProtocolHandler> adp_handler_;
    std::unique_ptr<aecp::AECPProtocolHandler> aecp_handler_;
    std::unique_ptr<acmp::ACMPProtocolHandler> acmp_handler_;
    
    // Callbacks
    EntityDiscoveredCallback entity_discovered_callback_;
    EntityDepartedCallback entity_departed_callback_;
    StreamConnectedCallback stream_connected_callback_;
    StreamDisconnectedCallback stream_disconnected_callback_;
    ControlCommandCallback control_command_callback_;
    
    bool stack_running_;
    bool milan_mode_enabled_;
    
    // Internal event routing
    void handle_entity_discovered(const adp::ADPEntityInfo& entity_info);
    void handle_entity_departed(uint64_t entity_id);
    void handle_stream_connected(const acmp::StreamConnection& connection);
    void handle_stream_disconnected(const acmp::StreamConnection& connection);
    bool handle_control_command(const aecp::AECPMessage& command, aecp::AECPMessage& response);
    
    // Internal validation
    bool validate_stream_format(uint64_t stream_format) const;
    bool validate_entity_id(uint64_t entity_id) const;
};

} // namespace avdecc
} // namespace avtp_protocol

#endif // AVDECC_INTEGRATION_H
