/**
 * @file 1722_1-2013.h
 * @brief IEEE 1722.1-2013 Standard Implementation
 * 
 * Implementation of IEEE Standard for Device Discovery, Connection Management, 
 * and Control Protocol for IEEE 1722™ Based Devices (2013 version).
 * 
 * This header provides structures and definitions for:
 * - AVDECC Discovery Protocol (ADP)
 * - AVDECC Entity Model (AEM) basic structures
 * - AVDECC Connection Management Protocol (ACMP) enums
 * - AVDECC Enumeration and Control Protocol (AECP) enums
 * 
 * @version 1.0.0
 * @date 2025
 * @copyright OpenAvnu Project
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

// Cross-platform networking headers
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
#endif

namespace IEEE {
namespace _1722_1 {
namespace _2013 {

    // Forward declarations
    class AVDECCDiscoveryProtocolPDU;

    // ==============================
    // ADP (AVDECC Discovery Protocol)
    // ==============================

    namespace ADP {
        
        /**
         * @brief ADP Message Types (Table 6.1)
         */
        enum ADPMessageType : uint8_t {
            ENTITY_AVAILABLE = 0,   ///< The AVDECC Entity is available
            ENTITY_DEPARTING = 1,   ///< The AVDECC Entity is going away
            ENTITY_DISCOVER = 2     ///< Request for AVDECC Entities to send ENTITY_AVAILABLE
        };

        /**
         * @brief Entity Capabilities (Table 6.2)
         */
        enum class EntityCapabilities : uint32_t {
            EFU_MODE                          = 0x00000001, ///< Entity Firmware Upgrade mode
            ADDRESS_ACCESS_SUPPORTED          = 0x00000002, ///< Supports ADDRESS_ACCESS commands
            GATEWAY_ENTITY                    = 0x00000004, ///< Entity serves as a gateway
            AEM_SUPPORTED                     = 0x00000008, ///< Supports AEM AECP commands
            LEGACY_AVC                        = 0x00000010, ///< Supports IEEE 1394 AV/C protocol
            ASSOCIATION_ID_SUPPORTED          = 0x00000020, ///< Supports association_id field
            ASSOCIATION_ID_VALID              = 0x00000040, ///< association_id field contains valid value
            VENDOR_UNIQUE_SUPPORTED           = 0x00000080, ///< Supports VENDOR_UNIQUE commands
            CLASS_A_SUPPORTED                 = 0x00000100, ///< Supports Class A Streams
            CLASS_B_SUPPORTED                 = 0x00000200, ///< Supports Class B Streams
            GPTP_SUPPORTED                    = 0x00000400, ///< Implements IEEE 802.1AS-2011
            AEM_AUTHENTICATION_SUPPORTED      = 0x00000800, ///< Supports AEM Authentication
            AEM_AUTHENTICATION_REQUIRED       = 0x00001000, ///< Requires AEM Authentication
            AEM_PERSISTENT_ACQUIRE_SUPPORTED  = 0x00002000, ///< Supports PERSISTENT flag in ACQUIRE
            AEM_IDENTIFY_CONTROL_INDEX_VALID  = 0x00004000, ///< identify_control_index field is valid
            AEM_INTERFACE_INDEX_VALID         = 0x00008000, ///< interface_index field is valid
            GENERAL_CONTROLLER_IGNORE         = 0x00010000, ///< General Controllers ignore this entity
            ENTITY_NOT_READY                  = 0x00020000  ///< Entity not ready for enumeration
        };

        /**
         * @brief Talker Capabilities (Table 6.3)
         */
        enum class TalkerCapabilities : uint16_t {
            IMPLEMENTED    = 0x0001, ///< Implements an AVDECC Talker
            OTHER_SOURCE   = 0x0020, ///< Has other Stream sources
            CONTROL_SOURCE = 0x0040, ///< Has Control Stream sources
            MEDIA_CLOCK_SOURCE = 0x0080, ///< Has Media Clock Stream sources
            SMPTE_SOURCE   = 0x0100, ///< Has SMPTE time code Stream sources
            MIDI_SOURCE    = 0x0200, ///< Has MIDI Stream sources
            AUDIO_SOURCE   = 0x0400, ///< Has Audio Stream sources
            VIDEO_SOURCE   = 0x0800  ///< Has Video Stream sources
        };

        /**
         * @brief Listener Capabilities (Table 6.4)
         */
        enum class ListenerCapabilities : uint16_t {
            IMPLEMENTED  = 0x0001, ///< Implements an AVDECC Listener
            OTHER_SINK   = 0x0020, ///< Has other Stream sinks
            CONTROL_SINK = 0x0040, ///< Has Control Stream sinks
            MEDIA_CLOCK_SINK = 0x0080, ///< Has Media Clock Stream sinks
            SMPTE_SINK   = 0x0100, ///< Has SMPTE time code Stream sinks
            MIDI_SINK    = 0x0200, ///< Has MIDI Stream sinks
            AUDIO_SINK   = 0x0400, ///< Has Audio Stream sinks
            VIDEO_SINK   = 0x0800  ///< Has Video Stream sinks
        };

        /**
         * @brief Controller Capabilities (Table 6.5)
         */
        enum class ControllerCapabilities : uint32_t {
            IMPLEMENTED = 0x00000001 ///< Implements an AVDECC Controller
        };

        /**
         * @brief AVDECC Discovery Protocol Data Unit (ADPDU)
         * 
         * Based on Figure 6.1 and section 6.2.1 of IEEE 1722.1-2013.
         * This structure represents the payload following the IEEE 1722 control AVTPDU header.
         */
        class AVDECCDiscoveryProtocolPDU {
        public:
            // IEEE 1722 Control AVTPDU header redefined fields
            ADPMessageType message_type;        ///< Redefined from control_data
            uint8_t valid_time;                 ///< Redefined from status
            uint64_t entity_id;                 ///< Redefined from stream_id

            // ADPDU specific fields (56 bytes following header)
            uint64_t entity_model_id;           ///< Entity Model ID (EUI-64)
            EntityCapabilities entity_capabilities; ///< Entity capabilities bitfield
            uint16_t talker_stream_sources;     ///< Number of Talker Stream sources
            TalkerCapabilities talker_capabilities; ///< Talker capabilities bitfield
            uint16_t listener_stream_sinks;     ///< Number of Listener Stream sinks
            ListenerCapabilities listener_capabilities; ///< Listener capabilities bitfield
            ControllerCapabilities controller_capabilities; ///< Controller capabilities bitfield
            uint32_t available_index;           ///< Availability cycle index
            uint64_t gptp_grandmaster_id;       ///< gPTP grandmaster clock identity
            uint8_t gptp_domain_number;         ///< gPTP domain number
            uint8_t reserved0[3];               ///< Reserved (24 bits)
            uint16_t identify_control_index;    ///< AEM CONTROL descriptor index for IDENTIFY
            uint16_t interface_index;           ///< AEM AVB_INTERFACE descriptor index
            uint64_t association_id;            ///< Association ID for entity grouping
            uint32_t reserved1;                 ///< Reserved (32 bits)

        private:
            std::vector<uint8_t> raw_octets_;
            bool is_valid_;
            
        public:
            AVDECCDiscoveryProtocolPDU();
            AVDECCDiscoveryProtocolPDU(const uint8_t* data, size_t length);
            
            // Serialization methods
            void serialize();
            bool deserialize(const uint8_t* data, size_t length);
            
            // Data access
            const uint8_t* get_raw_octets() const { return raw_octets_.data(); }
            size_t get_size() const { return raw_octets_.size(); }
            bool is_valid() const { return is_valid_; }
            
            // Setters
            void set_message_type(ADPMessageType type) { message_type = type; }
            void set_valid_time(uint8_t time) { valid_time = time; }
            void set_entity_id(uint64_t id) { entity_id = id; }
            void set_entity_model_id(uint64_t id) { entity_model_id = id; }
            void set_entity_capabilities(EntityCapabilities caps) { entity_capabilities = caps; }
            void set_talker_info(uint16_t sources, TalkerCapabilities caps) {
                talker_stream_sources = sources;
                talker_capabilities = caps;
            }
            void set_listener_info(uint16_t sinks, ListenerCapabilities caps) {
                listener_stream_sinks = sinks;
                listener_capabilities = caps;
            }
            void set_controller_capabilities(ControllerCapabilities caps) { controller_capabilities = caps; }
            void set_available_index(uint32_t index) { available_index = index; }
            void set_gptp_info(uint64_t grandmaster_id, uint8_t domain_num) {
                gptp_grandmaster_id = grandmaster_id;
                gptp_domain_number = domain_num;
            }
            void set_identify_control_index(uint16_t index) { identify_control_index = index; }
            void set_interface_index(uint16_t index) { interface_index = index; }
            void set_association_id(uint64_t id) { association_id = id; }
        };

    } // namespace ADP

    // ==============================
    // AEM (AVDECC Entity Model)
    // ==============================

    namespace AEM {

        /**
         * @brief Descriptor Types
         */
        enum DescriptorType : uint16_t {
            ENTITY                  = 0x0000,
            CONFIGURATION          = 0x0001,
            AUDIO_UNIT             = 0x0002,
            VIDEO_UNIT             = 0x0003,
            SENSOR_UNIT            = 0x0004,
            STREAM_INPUT           = 0x0005,
            STREAM_OUTPUT          = 0x0006,
            JACK_INPUT             = 0x0007,
            JACK_OUTPUT            = 0x0008,
            AVB_INTERFACE          = 0x0009,
            CLOCK_SOURCE           = 0x000A,
            MEMORY_OBJECT          = 0x000B,
            LOCALE                 = 0x000C,
            STRINGS                = 0x000D,
            STREAM_PORT_INPUT      = 0x000E,
            STREAM_PORT_OUTPUT     = 0x000F,
            EXTERNAL_PORT_INPUT    = 0x0010,
            EXTERNAL_PORT_OUTPUT   = 0x0011,
            INTERNAL_PORT_INPUT    = 0x0012,
            INTERNAL_PORT_OUTPUT   = 0x0013,
            AUDIO_CLUSTER          = 0x0014,
            VIDEO_CLUSTER          = 0x0015,
            SENSOR_CLUSTER         = 0x0016,
            AUDIO_MAP              = 0x0017,
            VIDEO_MAP              = 0x0018,
            SENSOR_MAP             = 0x0019,
            CONTROL                = 0x001A,
            SIGNAL_SELECTOR        = 0x001B,
            MIXER                  = 0x001C,
            MATRIX                 = 0x001D,
            LOCALE_SPECIFIC        = 0x001E,
            CLOCK_DOMAIN           = 0x001F,
            SIGNAL_SPLITTER        = 0x0020,
            SIGNAL_COMBINER        = 0x0021,
            SIGNAL_DEMULTIPLEXER   = 0x0022,
            SIGNAL_MULTIPLEXER     = 0x0023,
            SIGNAL_TRANSCODER      = 0x0024,
            CONTROL_BLOCK          = 0x0025,
            INVALID                = 0xFFFF
        };

        /**
         * @brief ENTITY Descriptor (basic structure)
         */
        struct EntityDescriptor {
            DescriptorType descriptor_type;    ///< Always ENTITY
            uint16_t descriptor_index;         ///< Always 0 for ENTITY
            uint64_t entity_id;               ///< Entity ID (EUI-64)
            uint64_t entity_model_id;         ///< Entity Model ID (EUI-64)
            ADP::EntityCapabilities entity_capabilities; ///< Entity capabilities
            uint16_t talker_stream_sources;   ///< Number of Talker Stream sources
            ADP::TalkerCapabilities talker_capabilities; ///< Talker capabilities
            uint16_t listener_stream_sinks;   ///< Number of Listener Stream sinks
            ADP::ListenerCapabilities listener_capabilities; ///< Listener capabilities
            ADP::ControllerCapabilities controller_capabilities; ///< Controller capabilities
            uint32_t available_index;         ///< Current available index
            uint64_t association_id;          ///< Association ID
            char entity_name[64];            ///< UTF-8 entity name
            uint16_t vendor_name_string;      ///< Localized vendor name reference
            uint16_t model_name_string;       ///< Localized model name reference
            char firmware_version[64];        ///< UTF-8 firmware version
            char group_name[64];             ///< UTF-8 group name
            char serial_number[64];          ///< UTF-8 serial number
            uint16_t configurations_count;    ///< Number of CONFIGURATION descriptors
            uint16_t current_configuration;   ///< Current configuration index
        };

        /**
         * @brief Jack Types (Table 7.12)
         */
        enum JackType : uint16_t {
            SPEAKER                = 0x0000,
            HEADPHONE             = 0x0001,
            ANALOG_MICROPHONE     = 0x0002,
            SPDIF                 = 0x0003,
            ADAT                  = 0x0004,
            TDIF                  = 0x0005,
            MADI                  = 0x0006,
            UNBALANCED_ANALOG     = 0x0007,
            BALANCED_ANALOG       = 0x0008,
            DIGITAL               = 0x0009,
            MIDI                  = 0x000A,
            AES_EBU               = 0x000B,
            COMPOSITE_VIDEO       = 0x000C,
            S_VHS_VIDEO           = 0x000D,
            COMPONENT_VIDEO       = 0x000E,
            DVI                   = 0x000F,
            HDMI                  = 0x0010,
            UDI                   = 0x0011,
            DISPLAYPORT           = 0x0012,
            ANTENNA               = 0x0013,
            ANALOG_TUNER          = 0x0014,
            ETHERNET              = 0x0015,
            WIFI                  = 0x0016,
            USB                   = 0x0017,
            PCI                   = 0x0018,
            PCI_E                 = 0x0019,
            SCSI                  = 0x001A,
            ATA                   = 0x001B,
            IMAGER                = 0x001C,
            IR                    = 0x001D,
            THUNDERBOLT           = 0x001E,
            SATA                  = 0x001F,
            SMPTE_LTC             = 0x0020,
            DIGITAL_MICROPHONE    = 0x0021,
            AUDIO_MEDIA_CLOCK     = 0x0022,
            VIDEO_MEDIA_CLOCK     = 0x0023,
            GNSS_CLOCK            = 0x0024,
            PPS                   = 0x0025,
            EXPANSION             = 0xFFFF
        };

        /**
         * @brief Utility functions
         */
        inline void safe_string_copy(char* dest, const char* src, size_t dest_size) {
            if (dest && src && dest_size > 0) {
                strncpy(dest, src, dest_size - 1);
                dest[dest_size - 1] = '\0';
            }
        }

    } // namespace AEM

    // ==============================
    // ACMP (Connection Management Protocol)
    // ==============================

    namespace ACMP {

        /**
         * @brief ACMP Message Types
         */
        enum ACMPMessageType : uint8_t {
            CONNECT_TX_COMMAND      = 0,
            CONNECT_TX_RESPONSE     = 1,
            DISCONNECT_TX_COMMAND   = 2,
            DISCONNECT_TX_RESPONSE  = 3,
            GET_TX_STATE_COMMAND    = 4,
            GET_TX_STATE_RESPONSE   = 5,
            CONNECT_RX_COMMAND      = 6,
            CONNECT_RX_RESPONSE     = 7,
            DISCONNECT_RX_COMMAND   = 8,
            DISCONNECT_RX_RESPONSE  = 9,
            GET_RX_STATE_COMMAND    = 10,
            GET_RX_STATE_RESPONSE   = 11,
            GET_TX_CONNECTION_COMMAND = 12,
            GET_TX_CONNECTION_RESPONSE = 13
        };

        /**
         * @brief ACMP Status Codes
         */
        enum ACMPStatus : uint8_t {
            SUCCESS                     = 0,
            LISTENER_UNKNOWN_ID         = 1,
            TALKER_UNKNOWN_ID          = 2,
            TALKER_DEST_MAC_FAIL       = 3,
            TALKER_NO_STREAM_INDEX     = 4,
            TALKER_NO_BANDWIDTH        = 5,
            TALKER_EXCLUSIVE           = 6,
            LISTENER_TALKER_TIMEOUT    = 7,
            LISTENER_EXCLUSIVE         = 8,
            STATE_UNAVAILABLE          = 9,
            NOT_CONNECTED              = 10,
            NO_SUCH_CONNECTION         = 11,
            COULD_NOT_SEND_MESSAGE     = 12,
            TALKER_MISBEHAVING         = 13,
            LISTENER_MISBEHAVING       = 14,
            CONTROLLER_NOT_AUTHORIZED  = 16,
            INCOMPATIBLE_REQUEST       = 17,
            NOT_SUPPORTED              = 31
        };

    } // namespace ACMP

    // ==============================
    // AECP (Enumeration and Control Protocol)
    // ==============================

    namespace AECP {

        /**
         * @brief AECP Message Types
         */
        enum AECPMessageType : uint8_t {
            AEM_COMMAND             = 0,
            AEM_RESPONSE            = 1,
            ADDRESS_ACCESS_COMMAND  = 2,
            ADDRESS_ACCESS_RESPONSE = 3,
            AVC_COMMAND             = 4,
            AVC_RESPONSE            = 5,
            VENDOR_UNIQUE_COMMAND   = 6,
            VENDOR_UNIQUE_RESPONSE  = 7,
            HDCP_APM_COMMAND        = 8,
            HDCP_APM_RESPONSE       = 9,
            EXTENDED_COMMAND        = 14,
            EXTENDED_RESPONSE       = 15
        };

        /**
         * @brief AEM Command Types
         */
        enum AEMCommandType : uint16_t {
            ACQUIRE_ENTITY                      = 0x0000,
            LOCK_ENTITY                         = 0x0001,
            ENTITY_AVAILABLE                    = 0x0002,
            CONTROLLER_AVAILABLE                = 0x0003,
            READ_DESCRIPTOR                     = 0x0004,
            WRITE_DESCRIPTOR                    = 0x0005,
            SET_CONFIGURATION                   = 0x0006,
            GET_CONFIGURATION                   = 0x0007,
            SET_STREAM_FORMAT                   = 0x0008,
            GET_STREAM_FORMAT                   = 0x0009,
            SET_VIDEO_FORMAT                    = 0x000A,
            GET_VIDEO_FORMAT                    = 0x000B,
            SET_SENSOR_FORMAT                   = 0x000C,
            GET_SENSOR_FORMAT                   = 0x000D,
            SET_STREAM_INFO                     = 0x000E,
            GET_STREAM_INFO                     = 0x000F,
            SET_NAME                            = 0x0010,
            GET_NAME                            = 0x0011,
            SET_ASSOCIATION_ID                  = 0x0012,
            GET_ASSOCIATION_ID                  = 0x0013,
            SET_SAMPLING_RATE                   = 0x0014,
            GET_SAMPLING_RATE                   = 0x0015,
            SET_CLOCK_SOURCE                    = 0x0016,
            GET_CLOCK_SOURCE                    = 0x0017,
            SET_CONTROL                         = 0x0018,
            GET_CONTROL                         = 0x0019,
            INCREMENT_CONTROL                   = 0x001A,
            DECREMENT_CONTROL                   = 0x001B,
            SET_SIGNAL_SELECTOR                 = 0x001C,
            GET_SIGNAL_SELECTOR                 = 0x001D,
            SET_MIXER                           = 0x001E,
            GET_MIXER                           = 0x001F,
            SET_MATRIX                          = 0x0020,
            GET_MATRIX                          = 0x0021,
            START_STREAMING                     = 0x0022,
            STOP_STREAMING                      = 0x0023,
            REGISTER_UNSOLICITED_NOTIFICATION   = 0x0024,
            DEREGISTER_UNSOLICITED_NOTIFICATION = 0x0025,
            IDENTIFY_NOTIFICATION               = 0x0026,
            GET_AVB_INFO                        = 0x0027,
            GET_AS_PATH                         = 0x0028,
            GET_COUNTERS                        = 0x0029,
            REBOOT                              = 0x002A,
            GET_AUDIO_MAP                       = 0x002B,
            ADD_AUDIO_MAPPINGS                  = 0x002C,
            REMOVE_AUDIO_MAPPINGS               = 0x002D,
            GET_VIDEO_MAP                       = 0x002E,
            ADD_VIDEO_MAPPINGS                  = 0x002F,
            REMOVE_VIDEO_MAPPINGS               = 0x0030,
            GET_SENSOR_MAP                      = 0x0031,
            ADD_SENSOR_MAPPINGS                 = 0x0032,
            REMOVE_SENSOR_MAPPINGS              = 0x0033,
            START_OPERATION                     = 0x0034,
            ABORT_OPERATION                     = 0x0035,
            OPERATION_STATUS                    = 0x0036,
            AUTH_ADD_KEY                        = 0x0037,
            AUTH_DELETE_KEY                     = 0x0038,
            AUTH_GET_KEY_LIST                   = 0x0039,
            AUTH_GET_KEY                        = 0x003A,
            AUTH_ADD_KEY_TO_CHAIN               = 0x003B,
            AUTH_DELETE_KEY_FROM_CHAIN          = 0x003C,
            AUTH_GET_KEYCHAIN_LIST              = 0x003D,
            AUTH_GET_IDENTITY                   = 0x003E,
            AUTH_ADD_TOKEN                      = 0x003F,
            AUTH_DELETE_TOKEN                   = 0x0040,
            AUTHENTICATE                        = 0x0041,
            DEAUTHENTICATE                      = 0x0042,
            ENABLE_TRANSPORT_SECURITY           = 0x0043,
            DISABLE_TRANSPORT_SECURITY          = 0x0044,
            ENABLE_STREAM_ENCRYPTION            = 0x0045,
            DISABLE_STREAM_ENCRYPTION           = 0x0046,
            SET_MEMORY_OBJECT_LENGTH            = 0x0047,
            GET_MEMORY_OBJECT_LENGTH            = 0x0048,
            SET_STREAM_BACKUP                   = 0x0049,
            GET_STREAM_BACKUP                   = 0x004A,
            EXPANSION                           = 0x7FFF
        };

        /**
         * @brief AEM Status Codes
         */
        enum AEMStatus : uint8_t {
            SUCCESS                    = 0,
            NOT_IMPLEMENTED            = 1,
            NO_SUCH_DESCRIPTOR         = 2,
            ENTITY_LOCKED              = 3,
            ENTITY_ACQUIRED            = 4,
            NOT_AUTHENTICATED          = 5,
            AUTHENTICATION_DISABLED    = 6,
            BAD_ARGUMENTS              = 7,
            NO_RESOURCES               = 8,
            IN_PROGRESS                = 9,
            ENTITY_MISBEHAVING         = 10,
            NOT_SUPPORTED              = 11,
            STREAM_IS_RUNNING          = 12
        };

    } // namespace AECP

} // namespace _2013
} // namespace _1722_1
} // namespace IEEE