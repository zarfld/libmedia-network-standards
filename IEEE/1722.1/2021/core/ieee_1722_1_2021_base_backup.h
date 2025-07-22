#pragma once

// Windows compatibility - must be included BEFORE any other headers
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
#else
    #include <arpa/inet.h>
    #include <endian.h>
#endif

#include <cstdint>
#include <iostream>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    // Windows byte order functions
    #define htobe64(x) _byteswap_uint64(x)
    #define be64toh(x) _byteswap_uint64(x)
    #define htonll(x) _byteswap_uint64(x)
    #define ntohll(x) _byteswap_uint64(x)
#endif

    // ============================================================================
    // IEEE 1722.1-2021 AECP (AVDECC Enumeration and Control Protocol)
    // COMPLETE IMPLEMENTATION - HÖCHSTE PRIORITÄT
    // ============================================================================
    namespace AECP {
        
        // AECP Message Types (IEEE 1722.1-2021 Table 9.1)
        enum class AECPMessageType : uint8_t {
            AEM_COMMAND             = 0x00,
            AEM_RESPONSE            = 0x01,
            ADDRESS_ACCESS_COMMAND  = 0x02,
            ADDRESS_ACCESS_RESPONSE = 0x03,
            AVC_COMMAND             = 0x04,
            AVC_RESPONSE            = 0x05,
            VENDOR_UNIQUE_COMMAND   = 0x06,
            VENDOR_UNIQUE_RESPONSE  = 0x07,
            HDCP_APM_COMMAND        = 0x08,
            HDCP_APM_RESPONSE       = 0x09,
            EXTENDED_COMMAND        = 0x0E,
            EXTENDED_RESPONSE       = 0x0F
        };

        // AEM Command Codes (IEEE 1722.1-2021 Table 7.126)
        enum class AEMCommandType : uint16_t {
            ACQUIRE_ENTITY          = 0x0000,
            LOCK_ENTITY             = 0x0001,
            ENTITY_AVAILABLE        = 0x0002,
            CONTROLLER_AVAILABLE    = 0x0003,
            READ_DESCRIPTOR         = 0x0004,
            WRITE_DESCRIPTOR        = 0x0005,
            SET_CONFIGURATION       = 0x0006,
            GET_CONFIGURATION       = 0x0007,
            SET_STREAM_FORMAT       = 0x0008,
            GET_STREAM_FORMAT       = 0x0009,
            SET_VIDEO_FORMAT        = 0x000A,
            GET_VIDEO_FORMAT        = 0x000B,
            SET_SENSOR_FORMAT       = 0x000C,
            GET_SENSOR_FORMAT       = 0x000D,
            SET_STREAM_INFO         = 0x000E,
            GET_STREAM_INFO         = 0x000F,
            SET_NAME                = 0x0010,
            GET_NAME                = 0x0011,
            SET_ASSOCIATION_ID      = 0x0012,
            GET_ASSOCIATION_ID      = 0x0013,
            SET_SAMPLING_RATE       = 0x0014,
            GET_SAMPLING_RATE       = 0x0015,
            SET_CLOCK_SOURCE        = 0x0016,
            GET_CLOCK_SOURCE        = 0x0017,
            SET_CONTROL             = 0x0018,
            GET_CONTROL             = 0x0019,
            INCREMENT_CONTROL       = 0x001A,
            DECREMENT_CONTROL       = 0x001B,
            SET_SIGNAL_SELECTOR     = 0x001C,
            GET_SIGNAL_SELECTOR     = 0x001D,
            SET_MIXER               = 0x001E,
            GET_MIXER               = 0x001F,
            SET_MATRIX              = 0x0020,
            GET_MATRIX              = 0x0021,
            START_STREAMING         = 0x0022,
            STOP_STREAMING          = 0x0023,
            REGISTER_UNSOLICITED_NOTIFICATION = 0x0024,
            DEREGISTER_UNSOLICITED_NOTIFICATION = 0x0025,
            IDENTIFY_NOTIFICATION   = 0x0026,
            GET_AVB_INFO            = 0x0027,
            GET_AS_PATH             = 0x0028,
            GET_COUNTERS            = 0x0029,
            REBOOT                  = 0x002A,
            GET_AUDIO_MAP           = 0x002B,
            ADD_AUDIO_MAPPINGS      = 0x002C,
            REMOVE_AUDIO_MAPPINGS   = 0x002D,
            GET_VIDEO_MAP           = 0x002E,
            ADD_VIDEO_MAPPINGS      = 0x002F,
            REMOVE_VIDEO_MAPPINGS   = 0x0030,
            GET_SENSOR_MAP          = 0x0031,
            ADD_SENSOR_MAPPINGS     = 0x0032,
            REMOVE_SENSOR_MAPPINGS  = 0x0033,
            START_OPERATION         = 0x0034,
            ABORT_OPERATION         = 0x0035,
            OPERATION_STATUS        = 0x0036,
            AUTH_ADD_KEY            = 0x0037,
            AUTH_DELETE_KEY         = 0x0038,
            AUTH_GET_KEY_LIST       = 0x0039,
            AUTH_GET_KEY            = 0x003A,
            AUTH_ADD_KEY_TO_CHAIN   = 0x003B,
            AUTH_DELETE_KEY_FROM_CHAIN = 0x003C,
            AUTH_GET_KEYCHAIN_LIST  = 0x003D,
            AUTH_GET_IDENTITY       = 0x003E,
            AUTH_ADD_TOKEN          = 0x003F,
            AUTH_DELETE_TOKEN       = 0x0040,
            AUTH_GET_TOKEN          = 0x0041,
            AUTHENTICATE            = 0x0042,
            DEAUTHENTICATE          = 0x0043,
            ENABLE_TRANSPORT_SECURITY = 0x0044,
            DISABLE_TRANSPORT_SECURITY = 0x0045,
            GET_DYNAMIC_INFO        = 0x004B,  // Milan extension
            EXPANSION               = 0x7FFF
        };

        // AEM Status Codes (IEEE 1722.1-2021 Table 7.127) - VOLLSTÄNDIG
        enum class AEMStatusCode : uint8_t {
            SUCCESS                 = 0x00,
            NOT_IMPLEMENTED         = 0x01,
            NO_SUCH_DESCRIPTOR      = 0x02,
            ENTITY_LOCKED           = 0x03,
            ENTITY_ACQUIRED         = 0x04,
            NOT_AUTHORIZED          = 0x05,
            INSUFFICIENT_PRIVILEGES = 0x06,
            BAD_ARGUMENTS           = 0x07,
            NO_RESOURCES            = 0x08,
            IN_PROGRESS             = 0x09,
            ENTITY_MISBEHAVING      = 0x0A,
            NOT_SUPPORTED           = 0x0B,
            STREAM_IS_RUNNING       = 0x0C
        };

        // Backwards compatibility aliases for alternative naming conventions
        using AECP_Message_type = AECPMessageType;          // Alternative name support
        using AECP_Status = AEMStatusCode;                  // Alternative name support  
        using AEM_Command_type = AEMCommandType;            // Alternative name support

                };

        // Backwards compatibility aliases for code that uses alternative naming
        using AECP_Message_type = AECPMessageType;
        using AECP_Status = AEMStatusCode;
        using AEM_Command_type = AEMCommandType;

        // Complete AECP PDU class with full IEEE 1722.1-2021 implementation
        class ATDECCEnumerationControlProtocolPDU {
        private:
            uint8_t raw_octets[524]; // Maximum AECP payload size per IEEE 1722.1

        public:
            // IEEE 1722-2016 Common Header Fields
            uint8_t subtype = 0x7B;        // AECP subtype (same as ACMP)
            bool h = false;                // Header specific bit (always 0)
            uint8_t version = 0;           // Version (always 0)
            AECPMessageType message_type;
            AEMStatusCode status;
            uint16_t control_data_length;

            // AECP Common Fields (IEEE 1722.1-2021 Clause 9.2.1.1)
            uint64_t target_entity_id;
            uint64_t controller_entity_id;
            uint16_t sequence_id;

            // AEM Specific Fields (for AEM_COMMAND/AEM_RESPONSE)
            bool unsolicited = false;      // Unsolicited notification flag
            AEMCommandType command_type;
            uint16_t command_specific_length = 0;
            uint8_t command_specific_data[512]; // Command-specific payload

            // Constructors
            ATDECCEnumerationControlProtocolPDU();
            ATDECCEnumerationControlProtocolPDU(const uint8_t* data, size_t length);

            // Serialization methods
            void serialize();
            void deserialize();
            uint8_t* get_raw_octets() { return raw_octets; }
            size_t get_size() const;

            // Protocol helper methods
            bool is_command() const;
            bool is_response() const;
            bool is_aem_message() const;
            void create_response(AEMStatusCode response_status);

            // AEM-specific helper methods
            void set_read_descriptor_command(uint16_t config_index, uint16_t desc_type, uint16_t desc_index);
            void set_read_descriptor_response(uint16_t config_index, uint16_t desc_type, uint16_t desc_index, 
                                            const uint8_t* descriptor_data, size_t descriptor_length);
            void get_read_descriptor_command(uint16_t& config_index, uint16_t& desc_type, uint16_t& desc_index) const;
            void get_read_descriptor_response(uint16_t& config_index, uint16_t& desc_type, uint16_t& desc_index,
                                            const uint8_t*& descriptor_data, size_t& descriptor_length) const;
        };

        // AECP Common PDU Format (IEEE 1722.1-2021 Figure 9.1)
        struct AECPCommonFormat {
            // AVTPDU Common header (12 bytes)
            uint8_t subtype;                    // 0xFB for AECP
            uint8_t sv_version_msg_type;        // Stream valid, version, message type
            uint8_t status_control_data_length_h; // Status, upper 3 bits of control_data_length
            uint8_t control_data_length_l;      // Lower 8 bits of control_data_length
            uint64_t target_entity_id;          // Target entity ID (big endian)
            
            // AECP header
            uint64_t controller_entity_id;      // Controller entity ID (big endian)
            uint16_t sequence_id;               // Sequence ID (big endian)
            uint16_t command_type;              // Command type (big endian)
            
            // Helper functions for byte order conversion
            uint64_t getTargetEntityId() const { return be64toh(target_entity_id); }
            uint64_t getControllerEntityId() const { return be64toh(controller_entity_id); }
            uint16_t getSequenceId() const { return ntohs(sequence_id); }
            uint16_t getCommandType() const { return ntohs(command_type); }
            
            void setTargetEntityId(uint64_t id) { target_entity_id = htobe64(id); }
            void setControllerEntityId(uint64_t id) { controller_entity_id = htobe64(id); }
            void setSequenceId(uint16_t id) { sequence_id = htons(id); }
            void setCommandType(uint16_t type) { command_type = htons(type); }
        } __attribute__((packed));

        // AEM Command Message Format
        struct AEMCommandMessage : public AECPCommonFormat {
            // AEM Command specific fields (variable based on command type)
            uint8_t command_specific_data[506]; // Maximum payload for AEM commands
            
            // Get command type as enum
            AEMCommandType getAEMCommandType() const {
                return static_cast<AEMCommandType>(getCommandType());
            }
            
            // Set command type from enum
            void setAEMCommandType(AEMCommandType type) {
                setCommandType(static_cast<uint16_t>(type));
            }
        } __attribute__((packed));

        // AEM Response Message Format  
        struct AEMResponseMessage : public AECPCommonFormat {
            // AEM Response specific fields (variable based on command type)
            uint8_t response_specific_data[506]; // Maximum payload for AEM responses
            
            // Get status from the message
            AEMStatusCode getStatus() const {
                return static_cast<AEMStatusCode>((status_control_data_length_h >> 3) & 0x1F);
            }
            
            // Set status in the message
            void setStatus(AEMStatusCode status) {
                uint8_t status_bits = static_cast<uint8_t>(status) & 0x1F;
                status_control_data_length_h = (status_control_data_length_h & 0x07) | (status_bits << 3);
            }
        } __attribute__((packed));

        // READ_DESCRIPTOR Command/Response (IEEE 1722.1-2021 Section 7.4.5)
        struct ReadDescriptorCommand {
            uint16_t configuration_index;      // Configuration index (0 for current)
            uint16_t reserved;                  // Reserved, set to 0
            uint16_t descriptor_type;           // Descriptor type
            uint16_t descriptor_index;          // Descriptor index
        } __attribute__((packed));

        struct ReadDescriptorResponse {
            uint16_t configuration_index;      // Configuration index
            uint16_t reserved;                  // Reserved
            uint16_t descriptor_type;           // Descriptor type  
            uint16_t descriptor_index;          // Descriptor index
            uint8_t descriptor_data[504];       // Actual descriptor data
        } __attribute__((packed));

        // ACQUIRE_ENTITY Command/Response (IEEE 1722.1-2021 Section 7.4.1)
        struct AcquireEntityCommand {
            uint32_t flags;                     // Flags
            uint64_t owner_id;                  // Owner ID
            uint16_t descriptor_type;           // Descriptor type
            uint16_t descriptor_index;          // Descriptor index
        } __attribute__((packed));

        struct AcquireEntityResponse {
            uint32_t flags;                     // Flags
            uint64_t owner_id;                  // Owner ID
            uint16_t descriptor_type;           // Descriptor type
            uint16_t descriptor_index;          // Descriptor index
        } __attribute__((packed));

        // GET_CONFIGURATION Command/Response
        struct GetConfigurationCommand {
            uint16_t reserved;                  // Reserved, set to 0
        } __attribute__((packed));

        struct GetConfigurationResponse {
            uint16_t reserved;                  // Reserved
            uint16_t configuration_index;       // Current configuration index
        } __attribute__((packed));

        // SET_CONFIGURATION Command/Response
        struct SetConfigurationCommand {
            uint16_t reserved;                  // Reserved, set to 0
            uint16_t configuration_index;       // Configuration index to set
        } __attribute__((packed));

        struct SetConfigurationResponse {
            uint16_t reserved;                  // Reserved
            uint16_t configuration_index;       // Actual configuration index set
        } __attribute__((packed));

        // Protocol Handler Interface - FUNKTIONAL IMPLEMENTIERT
        class ProtocolHandler {
        public:
            virtual ~ProtocolHandler() = default;
            
            // Core command processing - MUSS IMPLEMENTIERT WERDEN
            virtual bool processCommand(const AEMCommandMessage& command, AEMResponseMessage& response) = 0;
            virtual bool processResponse(const AEMResponseMessage& response) = 0;
            
            // Entity state management - KRITISCH FÜR 1722.1-2021
            virtual bool isEntityAcquired(uint64_t entityId) const = 0;
            virtual bool isEntityLocked(uint64_t entityId) const = 0;
            virtual bool acquireEntity(uint64_t entityId, uint64_t controllerId) = 0;
            virtual bool releaseEntity(uint64_t entityId, uint64_t controllerId) = 0;
            
            // Descriptor operations - KERN-FUNKTIONALITÄT
            virtual bool readDescriptor(uint16_t descriptorType, uint16_t descriptorIndex, 
                                      void* descriptorData, size_t& descriptorSize) = 0;
            virtual bool writeDescriptor(uint16_t descriptorType, uint16_t descriptorIndex,
                                       const void* descriptorData, size_t descriptorSize) = 0;
            
            // Configuration management
            virtual uint16_t getCurrentConfiguration() const = 0;
            virtual bool setConfiguration(uint16_t configIndex) = 0;
            
            // Stream management  
            virtual bool startStreaming(uint16_t descriptorType, uint16_t descriptorIndex) = 0;
            virtual bool stopStreaming(uint16_t descriptorType, uint16_t descriptorIndex) = 0;
            
            // Control operations
            virtual bool setControl(uint16_t descriptorIndex, const void* controlData, size_t dataSize) = 0;
            virtual bool getControl(uint16_t descriptorIndex, void* controlData, size_t& dataSize) = 0;
        };

        // Command Factory - Vereinfacht Command-Erstellung
        class CommandFactory {
        public:
            static AEMCommandMessage createReadDescriptorCommand(uint64_t targetEntityId, 
                                                               uint64_t controllerEntityId,
                                                               uint16_t sequenceId,
                                                               uint16_t descriptorType,
                                                               uint16_t descriptorIndex) {
                AEMCommandMessage cmd = {};
                cmd.subtype = 0xFB;  // AECP subtype
                cmd.sv_version_msg_type = static_cast<uint8_t>(AECPMessageType::AEM_COMMAND);
                cmd.setTargetEntityId(targetEntityId);
                cmd.setControllerEntityId(controllerEntityId);
                cmd.setSequenceId(sequenceId);
                cmd.setAEMCommandType(AEMCommandType::READ_DESCRIPTOR);
                
                // Set command data
                ReadDescriptorCommand* cmdData = reinterpret_cast<ReadDescriptorCommand*>(cmd.command_specific_data);
                cmdData->configuration_index = 0;  // Current configuration
                cmdData->reserved = 0;
                cmdData->descriptor_type = htons(descriptorType);
                cmdData->descriptor_index = htons(descriptorIndex);
                
                return cmd;
            }
            
            static AEMCommandMessage createAcquireEntityCommand(uint64_t targetEntityId,
                                                              uint64_t controllerEntityId,
                                                              uint16_t sequenceId,
                                                              uint32_t flags,
                                                              uint64_t ownerId) {
                AEMCommandMessage cmd = {};
                cmd.subtype = 0xFB;
                cmd.sv_version_msg_type = static_cast<uint8_t>(AECPMessageType::AEM_COMMAND);
                cmd.setTargetEntityId(targetEntityId);
                cmd.setControllerEntityId(controllerEntityId);
                cmd.setSequenceId(sequenceId);
                cmd.setAEMCommandType(AEMCommandType::ACQUIRE_ENTITY);
                
                AcquireEntityCommand* cmdData = reinterpret_cast<AcquireEntityCommand*>(cmd.command_specific_data);
                cmdData->flags = htonl(flags);
                cmdData->owner_id = htobe64(ownerId);
                cmdData->descriptor_type = 0;
                cmdData->descriptor_index = 0;
                
                return cmd;
            }
        };

        // Response Factory - Vereinfacht Response-Erstellung
        class ResponseFactory {
        public:
            static AEMResponseMessage createSuccessResponse(const AEMCommandMessage& command) {
                AEMResponseMessage response = {};
                response.subtype = 0xFB;
                response.sv_version_msg_type = static_cast<uint8_t>(AECPMessageType::AEM_RESPONSE);
                response.setTargetEntityId(command.getControllerEntityId());
                response.setControllerEntityId(command.getTargetEntityId());
                response.setSequenceId(command.getSequenceId());
                response.setCommandType(command.getCommandType());
                response.setStatus(AEMStatusCode::SUCCESS);
                
                return response;
            }
            
            static AEMResponseMessage createErrorResponse(const AEMCommandMessage& command, 
                                                        AEMStatusCode status) {
                AEMResponseMessage response = createSuccessResponse(command);
                response.setStatus(status);
                return response;
            }
        };

    } // namespace AECP
#endif // 1722_1_2021_H

namespace IEEE {
namespace _1722_1 {
namespace _2021 {

    // Common constants
    const uint8_t AVDECC_MULTICAST_MAC[6] = {0x91, 0xE0, 0xF0, 0x01, 0x00, 0x00};
    const uint16_t AVDECC_ETHERTYPE = 0x22F0;

    // Common enums used across multiple protocols
    enum ADP_Message_type : uint8_t {
        ADP_ENTITY_AVAILABLE = 0x00,
        ADP_ENTITY_DEPARTING = 0x01,
        ADP_ENTITY_DISCOVERY_REQUEST = 0x02
    };

    enum class EntityCapabilities : uint32_t {
        None                                 = 0x00000000,
        EFU_MODE                             = 0x80000000, // Bit 31
        ADDRESS_ACCESS_SUPPORTED             = 0x40000000, // Bit 30
        GATEWAY_ENTITY                       = 0x20000000, // Bit 29
        AEM_SUPPORTED                        = 0x10000000, // Bit 28
        LEGACY_AVC                           = 0x08000000, // Bit 27
        ASSOCIATION_ID_SUPPORTED            = 0x04000000, // Bit 26
        ASSOCIATION_ID_VALID                = 0x02000000, // Bit 25
        VENDOR_UNIQUE_SUPPORTED             = 0x01000000, // Bit 24
        CLASS_A_SUPPORTED                   = 0x00800000, // Bit 23
        CLASS_B_SUPPORTED                   = 0x00400000, // Bit 22
        GPTP_SUPPORTED                      = 0x00200000, // Bit 21
        AEM_AUTHENTICATION_SUPPORTED        = 0x00100000, // Bit 20
        AEM_AUTHENTICATION_REQUIRED         = 0x00080000, // Bit 19
        AEM_PERSISTENT_ACQUIRE_SUPPORTED    = 0x00040000, // Bit 18
        AEM_IDENTIFY_CONTROL_INDEX_VALID    = 0x00020000, // Bit 17
        AEM_INTERFACE_INDEX_VALID           = 0x00010000, // Bit 16
        GENERAL_CONTROLLER_IGNORE           = 0x00008000, // Bit 15
        ENTITY_NOT_READY                    = 0x00004000, // Bit 14
        ACMP_ACQUIRE_WITH_AEM               = 0x00002000, // Bit 13
        ACMP_AUTHENTICATE_WITH_AEM          = 0x00001000, // Bit 12
        SUPPORTS_UDPv4_ATDECC               = 0x00000800, // Bit 11
        SUPPORTS_UDPv4_STREAMING            = 0x00000400, // Bit 10
        SUPPORTS_UDPv6_ATDECC               = 0x00000200, // Bit 9
        SUPPORTS_UDPv6_STREAMING            = 0x00000100, // Bit 8
        MULTIPLE_PTP_INSTANCES              = 0x00000080, // Bit 7
        AEM_CONFIGURATION_INDEX_VALID       = 0x00000040, // Bit 6
        // Bits 0–5: Reserved
    };

    enum class TalkerCapabilities : uint16_t {
        None               = 0x0000,
        VIDEO_SOURCE       = 0x0001, // Bit 0
        AUDIO_SOURCE       = 0x0002, // Bit 1
        MIDI_SOURCE        = 0x0004, // Bit 2
        SMPTE_SOURCE       = 0x0008, // Bit 3
        MEDIA_CLOCK_SOURCE = 0x0010, // Bit 4
        CONTROL_SOURCE     = 0x0020, // Bit 5
        OTHER_SOURCE       = 0x0040, // Bit 6
        IMPLEMENTED        = 0x8000  // Bit 15
        // Bits 7–14 are reserved
    };

    enum class ListenerCapabilities : uint16_t {
        None               = 0x0000,
        VIDEO_SINK         = 0x0001, // Bit 0
        AUDIO_SINK         = 0x0002, // Bit 1
        MIDI_SINK          = 0x0004, // Bit 2
        SMPTE_SINK         = 0x0008, // Bit 3
        MEDIA_CLOCK_SINK   = 0x0010, // Bit 4
        CONTROL_SINK       = 0x0020, // Bit 5
        OTHER_SINK         = 0x0040, // Bit 6
        IMPLEMENTED        = 0x8000  // Bit 15
        // Bits 7–14 are reserved
    };

    enum class ControllerCapabilities : uint32_t {
        None         = 0x00000000,
        IMPLEMENTED  = 0x00000001  // Bit 0
        // Bits 1–31 are reserved for future use
    };

    /**
     * @namespace ADPDU
     * @brief ATDECC Discovery Protocol Data Unit implementation
     * 
     * ATDECC Entity discovery is the process in which ATDECC Controllers identify all of the ATDECC Entities
     * currently available on a network and identify them as they are added to or removed from the network.
     * The ATDECC Discovery Protocol (ADP) is used for this purpose.
     */
    namespace ADPDU {

        /**
         * @brief ATDECC Discovery Protocol Data Unit (ADPDU) for IEEE 1722.1-2021
         * 
         * The ADPDU follows the IEEE Std 1722-2016 alternative AVTPDU header.
         * Used to communicate between ATDECC Entities and ATDECC Controllers.
         */
        class ATDECCDiscoveryProtocolPDU {
        private:
            uint8_t raw_octets[68]; // Total PDU size: 12 bytes common header + 56 bytes payload

        public:
            // IEEE 1722-2016 Common Header Fields
            uint8_t subtype;                        // 1 octet - ADP subtype
            bool h;                                 // 1 bit - header specific flag
            uint8_t version;                        // 3 bits - version field
            ADP_Message_type message_type;          // 4 bits - message type
            uint16_t valid_time;                    // 5 bits - validity period
            uint16_t control_data_length;           // 11 bits - payload length

            // ADPDU Payload Fields
            uint64_t entity_id;                     // 64 bits - unique entity identifier
            uint64_t entity_model_id;               // 64 bits - entity model identifier
            EntityCapabilities entity_capabilities; // 32 bits - entity capabilities
            uint16_t talker_stream_sources;         // 16 bits - max talker streams
            TalkerCapabilities talker_capabilities; // 16 bits - talker capabilities
            uint16_t listener_stream_sinks;         // 16 bits - max listener streams
            ListenerCapabilities listener_capabilities; // 16 bits - listener capabilities
            ControllerCapabilities controller_capabilities; // 32 bits - controller capabilities
            uint32_t available_index;               // 32 bits - availability tracking
            uint64_t gptp_grandmaster_id;           // 64 bits - gPTP grandmaster ID
            uint8_t gptp_domain_number;             // 8 bits - gPTP domain number
            uint8_t reserved0;                      // 8 bits - reserved
            uint16_t current_configuration_index;   // 16 bits - current configuration
            uint16_t identify_control_index;        // 16 bits - identify control index
            uint16_t interface_index;               // 16 bits - interface index
            uint64_t association_id;                // 64 bits - association identifier
            uint32_t reserved1;                     // 32 bits - reserved

            // Constructors
            ATDECCDiscoveryProtocolPDU();
            ATDECCDiscoveryProtocolPDU(const uint8_t* data, size_t length);

            // Serialization methods
            void serialize();
            void deserialize();
            
            // Validation
            bool is_valid() const;
            
            // Data access
            size_t get_size() const;
            const uint8_t* get_raw_octets() const;
            uint8_t* get_raw_octets();
            
            // Setter methods
            void set_entity_id(uint64_t id);
            void set_entity_model_id(uint64_t model_id);
            void set_entity_capabilities(EntityCapabilities capabilities);
            void set_talker_info(uint16_t stream_sources, TalkerCapabilities capabilities);
            void set_listener_info(uint16_t stream_sinks, ListenerCapabilities capabilities);
            void set_controller_capabilities(ControllerCapabilities capabilities);
            void set_message_type(ADP_Message_type type);
            void set_valid_time(uint16_t time);
        };

    } // namespace ADPDU

    /**
     * @namespace AEM
     * @brief ATDECC Entity Model implementation
     * 
     * The ATDECC Entity Model (AEM) defines the structure and content of an ATDECC Entity.
     */
    namespace AEM {

        // Descriptor Type Constants (from IEEE 1722.1-2021 Table 7.1)
        const uint16_t DESCRIPTOR_ENTITY                 = 0x0000;
        const uint16_t DESCRIPTOR_CONFIGURATION          = 0x0001;
        const uint16_t DESCRIPTOR_AUDIO_UNIT             = 0x0002;
        const uint16_t DESCRIPTOR_VIDEO_UNIT             = 0x0003;
        const uint16_t DESCRIPTOR_SENSOR_UNIT            = 0x0004;
        const uint16_t DESCRIPTOR_STREAM_INPUT           = 0x0005;
        const uint16_t DESCRIPTOR_STREAM_OUTPUT          = 0x0006;
        const uint16_t DESCRIPTOR_JACK_INPUT             = 0x0007;
        const uint16_t DESCRIPTOR_JACK_OUTPUT            = 0x0008;
        const uint16_t DESCRIPTOR_AVB_INTERFACE          = 0x0009;
        const uint16_t DESCRIPTOR_CLOCK_SOURCE           = 0x000A;
        const uint16_t DESCRIPTOR_MEMORY_OBJECT          = 0x000B;
        const uint16_t DESCRIPTOR_LOCALE                 = 0x000C;
        const uint16_t DESCRIPTOR_STRINGS                = 0x000D;
        const uint16_t DESCRIPTOR_STREAM_PORT_INPUT      = 0x000E;
        const uint16_t DESCRIPTOR_STREAM_PORT_OUTPUT     = 0x000F;
        const uint16_t DESCRIPTOR_EXTERNAL_PORT_INPUT    = 0x0010;
        const uint16_t DESCRIPTOR_EXTERNAL_PORT_OUTPUT   = 0x0011;
        const uint16_t DESCRIPTOR_INTERNAL_PORT_INPUT    = 0x0012;
        const uint16_t DESCRIPTOR_INTERNAL_PORT_OUTPUT   = 0x0013;
        const uint16_t DESCRIPTOR_AUDIO_CLUSTER          = 0x0014;
        const uint16_t DESCRIPTOR_VIDEO_CLUSTER          = 0x0015;
        const uint16_t DESCRIPTOR_SENSOR_CLUSTER         = 0x0016;
        const uint16_t DESCRIPTOR_AUDIO_MAP              = 0x0017;
        const uint16_t DESCRIPTOR_VIDEO_MAP              = 0x0018;
        const uint16_t DESCRIPTOR_SENSOR_MAP             = 0x0019;
        const uint16_t DESCRIPTOR_CONTROL                = 0x001A;
        const uint16_t DESCRIPTOR_SIGNAL_SELECTOR        = 0x001B;
        const uint16_t DESCRIPTOR_MIXER                  = 0x001C;
        const uint16_t DESCRIPTOR_MATRIX                 = 0x001D;
        const uint16_t DESCRIPTOR_MATRIX_SIGNAL          = 0x001E;
        const uint16_t DESCRIPTOR_SIGNAL_SPLITTER        = 0x001F;
        const uint16_t DESCRIPTOR_SIGNAL_COMBINER        = 0x0020;
        const uint16_t DESCRIPTOR_SIGNAL_DEMULTIPLEXER   = 0x0021;
        const uint16_t DESCRIPTOR_SIGNAL_MULTIPLEXER     = 0x0022;
        const uint16_t DESCRIPTOR_SIGNAL_TRANSCODER      = 0x0023;
        const uint16_t DESCRIPTOR_CLOCK_DOMAIN           = 0x0024;
        const uint16_t DESCRIPTOR_CONTROL_BLOCK          = 0x0025;

        /**
         * @brief ENTITY Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor provides information about the ATDECC Entity, including its identity, capabilities, and configuration.
         * Doxygen comments are taken directly from the standard for clarity and compliance.
         */
        struct EntityDescriptor {
            uint64_t entity_id;                     ///< Unique identifier for the ATDECC Entity (EUI64)
            uint64_t entity_model_id;               ///< Model identifier for the ATDECC Entity (EUI64)
            EntityCapabilities entity_capabilities; ///< Bitfield indicating the capabilities of the ATDECC Entity
            uint32_t available_index;               ///< Current value of the available_index field
            uint64_t association_id;                ///< Association ID for grouping related entities (EUI64)
            uint16_t current_configuration;         ///< Current configuration index for the entity
            char entity_name[64];                   ///< Human-readable name for the entity
            char firmware_version[32];              ///< Firmware version string
            char serial_number[32];                 ///< Serial number string
            char group_name[32];                    ///< Group name string
            uint32_t current_sampling_rate;         ///< Current sampling rate (if applicable)
            uint32_t current_format;                ///< Current format (if applicable)
            uint32_t current_aspect_ratio;          ///< Current aspect ratio (for video clusters, if applicable)
            uint32_t current_size;                  ///< Current size (for video clusters, if applicable)
            uint32_t current_color_space;           ///< Current color space (for video clusters, if applicable)
            uint32_t memory_object_length;          ///< Length of memory object (if applicable)
            uint8_t mac_address[6];                 ///< MAC address for AVB interface (if applicable)
            uint64_t clock_identity;                ///< Clock identity for AVB interface (if applicable)
            uint8_t priority1;                      ///< Priority1 for AVB interface (if applicable)
            uint8_t clock_class;                    ///< Clock class for AVB interface (if applicable)
            uint16_t offset_scaled_log_variance;    ///< Offset scaled log variance for AVB interface (if applicable)
            uint8_t clock_accuracy;                 ///< Clock accuracy for AVB interface (if applicable)
            uint8_t priority2;                      ///< Priority2 for AVB interface (if applicable)
            uint8_t domain_number;                  ///< Domain number for AVB interface (if applicable)
            int8_t log_sync_interval;               ///< Log sync interval for AVB interface (if applicable)
            int8_t log_announce_interval;           ///< Log announce interval for AVB interface (if applicable)
            int8_t log_pdelay_interval;             ///< Log pdelay interval for AVB interface (if applicable)
            uint8_t reserved[32];                   ///< Reserved for future use
        };

        // Utility functions for descriptor validation
        bool is_valid_descriptor_type(uint16_t type);
        bool validate_entity_descriptor(const EntityDescriptor& descriptor);

        // Helper functions for string operations
        void safe_string_copy(char* dest, const char* src, size_t dest_size);
        void clear_string_field(char* str, size_t size);

    } // namespace AEM

    /**
     * @namespace ACMP
     * @brief ATDECC Connection Management Protocol implementation
     * 
     * The ACMP is used to establish, modify, and remove streaming connections between ATDECC Entities.
     */
    namespace ACMP {

        // ACMP Message Types (from IEEE 1722.1-2021 Table 8.1)
        enum class ACMP_Message_type : uint8_t {
            CONNECT_TX_COMMAND         = 0x00,
            CONNECT_TX_RESPONSE        = 0x01,
            DISCONNECT_TX_COMMAND      = 0x02,
            DISCONNECT_TX_RESPONSE     = 0x03,
            GET_TX_STATE_COMMAND       = 0x04,
            GET_TX_STATE_RESPONSE      = 0x05,
            CONNECT_RX_COMMAND         = 0x06,
            CONNECT_RX_RESPONSE        = 0x07,
            DISCONNECT_RX_COMMAND      = 0x08,
            DISCONNECT_RX_RESPONSE     = 0x09,
            GET_RX_STATE_COMMAND       = 0x0A,
            GET_RX_STATE_RESPONSE      = 0x0B,
            GET_TX_CONNECTION_COMMAND  = 0x0C,
            GET_TX_CONNECTION_RESPONSE = 0x0D
        };

        // ACMP Status Codes (from IEEE 1722.1-2021 Table 8.2)
        enum class ACMP_Status : uint8_t {
            SUCCESS                    = 0x00,
            LISTENER_UNKNOWN_ID        = 0x01,
            TALKER_UNKNOWN_ID          = 0x02,
            TALKER_DEST_MAC_FAIL       = 0x03,
            TALKER_NO_STREAM_INDEX     = 0x04,
            TALKER_NO_BANDWIDTH        = 0x05,
            TALKER_EXCLUSIVE           = 0x06,
            LISTENER_TALKER_TIMEOUT    = 0x07,
            LISTENER_EXCLUSIVE         = 0x08,
            STATE_UNAVAILABLE          = 0x09,
            NOT_CONNECTED              = 0x0A,
            NO_SUCH_CONNECTION         = 0x0B,
            COULD_NOT_SEND_MESSAGE     = 0x0C,
            TALKER_MISBEHAVING         = 0x0D,
            LISTENER_MISBEHAVING       = 0x0E,
            CONTROLLER_NOT_AUTHORIZED  = 0x0F,
            INCOMPATIBLE_REQUEST       = 0x10,
            NOT_SUPPORTED              = 0x1F
        };

    } // namespace ACMP

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
