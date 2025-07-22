#pragma once

#include <cstdint>
#include <iostream>
#include <cstring>
#include <vector>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    // Windows byte order functions
    #define htobe64(x) _byteswap_uint64(x)
    #define be64toh(x) _byteswap_uint64(x)
    #define htonll(x) _byteswap_uint64(x)
    #define ntohll(x) _byteswap_uint64(x)
#else
    #include <arpa/inet.h>
    #include <endian.h>
#endif

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

    // Bitwise operators for EntityCapabilities
    inline EntityCapabilities operator|(EntityCapabilities a, EntityCapabilities b) {
        return static_cast<EntityCapabilities>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline EntityCapabilities operator&(EntityCapabilities a, EntityCapabilities b) {
        return static_cast<EntityCapabilities>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
    inline EntityCapabilities operator^(EntityCapabilities a, EntityCapabilities b) {
        return static_cast<EntityCapabilities>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
    }
    inline EntityCapabilities operator~(EntityCapabilities a) {
        return static_cast<EntityCapabilities>(~static_cast<uint32_t>(a));
    }
    inline EntityCapabilities& operator|=(EntityCapabilities& a, EntityCapabilities b) {
        return a = a | b;
    }
    inline EntityCapabilities& operator&=(EntityCapabilities& a, EntityCapabilities b) {
        return a = a & b;
    }
    inline EntityCapabilities& operator^=(EntityCapabilities& a, EntityCapabilities b) {
        return a = a ^ b;
    }

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
            uint16_t talker_stream_sources;         ///< Maximum number of Talker streams
            TalkerCapabilities talker_capabilities; ///< Talker capabilities
            uint16_t listener_stream_sinks;         ///< Maximum number of Listener streams  
            ListenerCapabilities listener_capabilities; ///< Listener capabilities
            ControllerCapabilities controller_capabilities; ///< Controller capabilities
            uint32_t available_index;               ///< Current value of the available_index field
            uint64_t association_id;                ///< Association ID for grouping related entities (EUI64)
            char entity_name[64];                   ///< Human-readable name for the entity
            char vendor_name_string[64];            ///< Vendor name string
            char model_name_string[64];             ///< Model name string
            char firmware_version[64];              ///< Firmware version string
            char group_name[64];                    ///< Group name string
            char serial_number[64];                 ///< Serial number string
            uint16_t configurations_count;          ///< Number of configurations available
            uint16_t current_configuration;         ///< Current configuration index for the entity
        };

        /**
         * @brief CONFIGURATION Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a complete configuration of the ATDECC Entity.
         */
        struct ConfigurationDescriptor {
            char object_name[64];                   ///< Localized name for this configuration
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t descriptor_counts[64];         ///< Array of descriptor counts indexed by descriptor type
        };

        /**
         * @brief AUDIO_UNIT Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes an Audio Unit within the ATDECC Entity.
         */
        struct AudioUnitDescriptor {
            char object_name[64];                   ///< Localized name for this Audio Unit
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t clock_domain_index;            ///< Index of the Clock Domain this Audio Unit uses
            uint16_t number_of_stream_input_ports;  ///< Number of Stream Input Ports
            uint16_t base_stream_input_port;        ///< Index of the first Stream Input Port
            uint16_t number_of_stream_output_ports; ///< Number of Stream Output Ports
            uint16_t base_stream_output_port;       ///< Index of the first Stream Output Port
            uint16_t number_of_external_input_ports; ///< Number of External Input Ports
            uint16_t base_external_input_port;      ///< Index of the first External Input Port
            uint16_t number_of_external_output_ports; ///< Number of External Output Ports
            uint16_t base_external_output_port;     ///< Index of the first External Output Port
            uint16_t number_of_internal_input_ports; ///< Number of Internal Input Ports
            uint16_t base_internal_input_port;      ///< Index of the first Internal Input Port
            uint16_t number_of_internal_output_ports; ///< Number of Internal Output Ports
            uint16_t base_internal_output_port;     ///< Index of the first Internal Output Port
            uint16_t number_of_controls;            ///< Number of Controls
            uint16_t base_control;                  ///< Index of the first Control
            uint16_t number_of_signal_selectors;    ///< Number of Signal Selectors
            uint16_t base_signal_selector;          ///< Index of the first Signal Selector
            uint16_t number_of_mixers;              ///< Number of Mixers
            uint16_t base_mixer;                    ///< Index of the first Mixer
            uint16_t number_of_matrices;            ///< Number of Matrices
            uint16_t base_matrix;                   ///< Index of the first Matrix
            uint16_t number_of_splitters;           ///< Number of Signal Splitters
            uint16_t base_splitter;                 ///< Index of the first Signal Splitter
            uint16_t number_of_combiners;           ///< Number of Signal Combiners
            uint16_t base_combiner;                 ///< Index of the first Signal Combiner
            uint16_t number_of_demultiplexers;      ///< Number of Signal Demultiplexers
            uint16_t base_demultiplexer;            ///< Index of the first Signal Demultiplexer
            uint16_t number_of_multiplexers;        ///< Number of Signal Multiplexers
            uint16_t base_multiplexer;              ///< Index of the first Signal Multiplexer
            uint16_t number_of_transcoders;         ///< Number of Signal Transcoders
            uint16_t base_transcoder;               ///< Index of the first Signal Transcoder
            uint16_t number_of_control_blocks;      ///< Number of Control Blocks
            uint16_t base_control_block;            ///< Index of the first Control Block
            uint32_t current_sampling_rate;         ///< Current sampling rate
            uint16_t sampling_rates_offset;         ///< Offset to sampling rates array
            uint16_t sampling_rates_count;          ///< Number of supported sampling rates
            // Note: Variable-length sampling_rates array follows
        };

        /**
         * @brief STREAM_INPUT Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Stream Input of the ATDECC Entity.
         */
        struct StreamInputDescriptor {
            char object_name[64];                   ///< Localized name for this Stream Input
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t clock_domain_index;            ///< Index of the Clock Domain this stream uses
            uint16_t stream_flags;                  ///< Flags for this stream
            uint64_t current_format;                ///< Current stream format
            uint16_t formats_offset;                ///< Offset to formats array
            uint16_t number_of_formats;             ///< Number of supported stream formats
            uint64_t backup_talker_entity_id_0;     ///< Backup talker Entity ID 0
            uint16_t backup_talker_unique_id_0;     ///< Backup talker unique ID 0
            uint64_t backup_talker_entity_id_1;     ///< Backup talker Entity ID 1
            uint16_t backup_talker_unique_id_1;     ///< Backup talker unique ID 1
            uint64_t backup_talker_entity_id_2;     ///< Backup talker Entity ID 2
            uint16_t backup_talker_unique_id_2;     ///< Backup talker unique ID 2
            uint64_t backed_up_talker_entity_id;    ///< Backed up talker Entity ID
            uint16_t backed_up_talker_unique_id;    ///< Backed up talker unique ID
            uint16_t avb_interface_index;           ///< AVB interface index
            uint32_t buffer_length;                 ///< Buffer length in nanoseconds
            // Note: Variable-length formats array follows
        };

        /**
         * @brief STREAM_OUTPUT Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Stream Output of the ATDECC Entity.
         */
        struct StreamOutputDescriptor {
            char object_name[64];                   ///< Localized name for this Stream Output
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t clock_domain_index;            ///< Index of the Clock Domain this stream uses
            uint16_t stream_flags;                  ///< Flags for this stream
            uint64_t current_format;                ///< Current stream format
            uint16_t formats_offset;                ///< Offset to formats array
            uint16_t number_of_formats;             ///< Number of supported stream formats
            uint64_t backup_talker_entity_id_0;     ///< Backup talker Entity ID 0
            uint16_t backup_talker_unique_id_0;     ///< Backup talker unique ID 0
            uint64_t backup_talker_entity_id_1;     ///< Backup talker Entity ID 1
            uint16_t backup_talker_unique_id_1;     ///< Backup talker unique ID 1
            uint64_t backup_talker_entity_id_2;     ///< Backup talker Entity ID 2
            uint16_t backup_talker_unique_id_2;     ///< Backup talker unique ID 2
            uint64_t backed_up_talker_entity_id;    ///< Backed up talker Entity ID
            uint16_t backed_up_talker_unique_id;    ///< Backed up talker unique ID
            uint16_t avb_interface_index;           ///< AVB interface index
            uint32_t buffer_length;                 ///< Buffer length in nanoseconds
            // Note: Variable-length formats array follows
        };

        /**
         * @brief AVB_INTERFACE Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes an AVB Interface of the ATDECC Entity.
         */
        struct AvbInterfaceDescriptor {
            char object_name[64];                   ///< Localized name for this AVB Interface
            uint16_t localized_description;         ///< String reference for localized description
            uint8_t mac_address[6];                 ///< MAC address of the AVB interface
            uint16_t interface_flags;               ///< Interface flags
            uint64_t clock_identity;                ///< Clock identity for this interface
            uint8_t priority1;                      ///< Priority1 value for gPTP
            uint8_t clock_class;                    ///< Clock class for gPTP
            uint16_t offset_scaled_log_variance;    ///< Offset scaled log variance for gPTP
            uint8_t clock_accuracy;                 ///< Clock accuracy for gPTP
            uint8_t priority2;                      ///< Priority2 value for gPTP
            uint8_t domain_number;                  ///< gPTP domain number
            int8_t log_sync_interval;               ///< Log sync interval for gPTP
            int8_t log_announce_interval;           ///< Log announce interval for gPTP
            int8_t log_pdelay_interval;             ///< Log Pdelay interval for gPTP
            uint16_t port_number;                   ///< Port number for gPTP
        };

        /**
         * @brief CLOCK_SOURCE Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Clock Source of the ATDECC Entity.
         */
        struct ClockSourceDescriptor {
            char object_name[64];                   ///< Localized name for this Clock Source
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t clock_source_flags;            ///< Clock source flags
            uint16_t clock_source_type;             ///< Type of clock source
            uint64_t clock_source_identifier;       ///< Clock source identifier
            uint16_t clock_source_location_type;    ///< Location type of clock source
            uint16_t clock_source_location_index;   ///< Location index of clock source
        };

        /**
         * @brief CLOCK_DOMAIN Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Clock Domain of the ATDECC Entity.
         */
        struct ClockDomainDescriptor {
            char object_name[64];                   ///< Localized name for this Clock Domain
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t clock_source_index;            ///< Current clock source index
            uint16_t clock_sources_offset;          ///< Offset to clock sources array
            uint16_t clock_sources_count;           ///< Number of clock sources
            // Note: Variable-length clock_sources array follows
        };

        /**
         * @brief STREAM_PORT_INPUT Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Stream Input Port of the ATDECC Entity.
         */
        struct StreamPortInputDescriptor {
            char object_name[64];                   ///< Localized name for this Stream Port
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t clock_domain_index;            ///< Index of the Clock Domain this port uses
            uint16_t port_flags;                    ///< Port flags
            uint16_t number_of_controls;            ///< Number of Controls
            uint16_t base_control;                  ///< Index of the first Control
            uint16_t number_of_clusters;            ///< Number of Audio Clusters
            uint16_t base_cluster;                  ///< Index of the first Audio Cluster
            uint16_t number_of_maps;                ///< Number of Audio Maps
            uint16_t base_map;                      ///< Index of the first Audio Map
        };

        /**
         * @brief STREAM_PORT_OUTPUT Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Stream Output Port of the ATDECC Entity.
         */
        struct StreamPortOutputDescriptor {
            char object_name[64];                   ///< Localized name for this Stream Port
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t clock_domain_index;            ///< Index of the Clock Domain this port uses
            uint16_t port_flags;                    ///< Port flags
            uint16_t number_of_controls;            ///< Number of Controls
            uint16_t base_control;                  ///< Index of the first Control
            uint16_t number_of_clusters;            ///< Number of Audio Clusters
            uint16_t base_cluster;                  ///< Index of the first Audio Cluster
            uint16_t number_of_maps;                ///< Number of Audio Maps
            uint16_t base_map;                      ///< Index of the first Audio Map
        };

        /**
         * @brief AUDIO_CLUSTER Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes an Audio Cluster within the ATDECC Entity.
         */
        struct AudioClusterDescriptor {
            char object_name[64];                   ///< Localized name for this Audio Cluster
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t signal_type;                   ///< Type of audio signal
            uint16_t signal_index;                  ///< Index of the signal
            uint16_t signal_output;                 ///< Output index of the signal
            uint32_t path_latency;                  ///< Signal path latency in nanoseconds
            uint32_t block_latency;                 ///< Block processing latency in nanoseconds
            uint16_t channel_count;                 ///< Number of channels in this cluster
            uint8_t format;                         ///< Audio format (see Table 7.14)
        };

        /**
         * @brief AUDIO_MAP Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes the mapping between Audio Clusters and Stream channels.
         */
        struct AudioMapDescriptor {
            char object_name[64];                   ///< Localized name for this Audio Map
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t mappings_offset;               ///< Offset to mappings array
            uint16_t number_of_mappings;            ///< Number of mappings
            // Note: Variable-length mappings array follows (AudioMapping structures)
        };

        /**
         * @brief CONTROL Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Control of the ATDECC Entity.
         */
        struct ControlDescriptor {
            char object_name[64];                   ///< Localized name for this Control
            uint16_t localized_description;         ///< String reference for localized description
            uint32_t block_latency;                 ///< Block processing latency in nanoseconds
            uint32_t control_latency;               ///< Control latency in nanoseconds
            uint16_t control_domain;                ///< Control domain
            uint16_t control_value_type;            ///< Type of control value
            uint64_t control_type;                  ///< Control type identifier
            uint32_t reset_time;                    ///< Reset time in milliseconds
            uint16_t values_offset;                 ///< Offset to values
            uint16_t number_of_values;              ///< Number of values
            uint16_t signal_type;                   ///< Signal type
            uint16_t signal_index;                  ///< Signal index
            uint16_t signal_output;                 ///< Signal output
        };

        /**
         * @brief SIGNAL_SELECTOR Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Signal Selector of the ATDECC Entity.
         */
        struct SignalSelectorDescriptor {
            char object_name[64];                   ///< Localized name for this Signal Selector
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t selector_flags;                ///< Signal selector flags
            uint16_t current_signal_type;           ///< Current signal type
            uint16_t current_signal_index;          ///< Current signal index
            uint16_t current_signal_output;         ///< Current signal output
            uint16_t default_signal_type;           ///< Default signal type
            uint16_t default_signal_index;          ///< Default signal index
            uint16_t default_signal_output;         ///< Default signal output
            uint16_t control_index;                 ///< Control index
            uint16_t block_latency;                 ///< Block processing latency
            uint16_t signal_latency;                ///< Signal latency
            uint16_t sources_offset;                ///< Offset to sources array
            uint16_t number_of_sources;             ///< Number of signal sources
            // Note: Variable-length sources array follows
        };

        /**
         * @brief MIXER Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Mixer of the ATDECC Entity.
         */
        struct MixerDescriptor {
            char object_name[64];                   ///< Localized name for this Mixer
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t mixer_flags;                   ///< Mixer flags
            uint16_t control_index;                 ///< Control index
            uint32_t block_latency;                 ///< Block processing latency in nanoseconds
            uint16_t number_of_sources;             ///< Number of mixer sources
            uint16_t base_source;                   ///< Index of the first mixer source
            uint16_t number_of_destinations;        ///< Number of mixer destinations
            uint16_t base_destination;              ///< Index of the first mixer destination
            uint16_t number_of_matrices;            ///< Number of matrices
            uint16_t base_matrix;                   ///< Index of the first matrix
        };

        /**
         * @brief MATRIX Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Matrix of the ATDECC Entity.
         */
        struct MatrixDescriptor {
            char object_name[64];                   ///< Localized name for this Matrix
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t matrix_flags;                  ///< Matrix flags
            uint16_t control_index;                 ///< Control index
            uint32_t block_latency;                 ///< Block processing latency in nanoseconds
            uint16_t width;                         ///< Matrix width (number of sources)
            uint16_t height;                        ///< Matrix height (number of destinations)
            uint16_t number_of_sources;             ///< Number of matrix sources
            uint16_t base_source;                   ///< Index of the first matrix source
            uint16_t number_of_destinations;        ///< Number of matrix destinations
            uint16_t base_destination;              ///< Index of the first matrix destination
        };

        /**
         * @brief LOCALE Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Locale supported by the ATDECC Entity.
         */
        struct LocaleDescriptor {
            char locale_identifier[64];             ///< Locale identifier (e.g., "en-US")
            uint16_t number_of_strings;             ///< Number of strings descriptors for this locale
            uint16_t base_strings;                  ///< Index of the first strings descriptor
        };

        /**
         * @brief STRINGS Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor provides localized strings for the ATDECC Entity.
         */
        struct StringsDescriptor {
            uint16_t string0[64];                   ///< String 0 (UTF-16)
            uint16_t string1[64];                   ///< String 1 (UTF-16)
            uint16_t string2[64];                   ///< String 2 (UTF-16)
            uint16_t string3[64];                   ///< String 3 (UTF-16)
            uint16_t string4[64];                   ///< String 4 (UTF-16)
            uint16_t string5[64];                   ///< String 5 (UTF-16)
            uint16_t string6[64];                   ///< String 6 (UTF-16)
            // Note: Up to 7 strings per descriptor (indices 0-6)
        };

        /**
         * @brief JACK_INPUT Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a physical Jack Input of the ATDECC Entity.
         */
        struct JackInputDescriptor {
            char object_name[64];                   ///< Localized name for this Jack Input
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t jack_flags;                    ///< Jack flags
            uint16_t jack_type;                     ///< Type of jack (see Table 7.16)
            uint16_t number_of_controls;            ///< Number of Controls
            uint16_t base_control;                  ///< Index of the first Control
        };

        /**
         * @brief JACK_OUTPUT Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a physical Jack Output of the ATDECC Entity.
         */
        struct JackOutputDescriptor {
            char object_name[64];                   ///< Localized name for this Jack Output
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t jack_flags;                    ///< Jack flags
            uint16_t jack_type;                     ///< Type of jack (see Table 7.16)
            uint16_t number_of_controls;            ///< Number of Controls
            uint16_t base_control;                  ///< Index of the first Control
        };

        /**
         * @brief MEMORY_OBJECT Descriptor for IEEE 1722.1-2021.
         * 
         * This descriptor describes a Memory Object of the ATDECC Entity.
         */
        struct MemoryObjectDescriptor {
            char object_name[64];                   ///< Localized name for this Memory Object
            uint16_t localized_description;         ///< String reference for localized description
            uint16_t memory_object_type;            ///< Type of memory object
            uint16_t target_descriptor_type;        ///< Target descriptor type
            uint16_t target_descriptor_index;       ///< Target descriptor index
            uint64_t start_address;                 ///< Start address of memory object
            uint64_t maximum_length;                ///< Maximum length of memory object
            uint64_t length;                        ///< Current length of memory object
        };

        // Stream format definitions
        using StreamFormat = uint64_t;

        // Stream identification
        struct StreamIdentification {
            uint64_t entity_id;                     ///< Entity ID of the stream
            uint16_t stream_index;                  ///< Stream index within the entity
        };

        // Port flags for stream ports
        enum class StreamPortFlags : uint16_t {
            None                    = 0x0000,
            ClockSyncSource         = 0x0001,       ///< Port can be a clock sync source
            AsyncSampleRateConv     = 0x0002,       ///< Port supports async sample rate conversion
            SyncSampleRateConv      = 0x0004        ///< Port supports sync sample rate conversion
        };

        // Bitwise operators for StreamPortFlags
        inline StreamPortFlags operator|(StreamPortFlags a, StreamPortFlags b) {
            return static_cast<StreamPortFlags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
        }
        inline StreamPortFlags operator&(StreamPortFlags a, StreamPortFlags b) {
            return static_cast<StreamPortFlags>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
        }
        inline StreamPortFlags operator^(StreamPortFlags a, StreamPortFlags b) {
            return static_cast<StreamPortFlags>(static_cast<uint16_t>(a) ^ static_cast<uint16_t>(b));
        }
        inline StreamPortFlags operator~(StreamPortFlags a) {
            return static_cast<StreamPortFlags>(~static_cast<uint16_t>(a));
        }
        inline StreamPortFlags& operator|=(StreamPortFlags& a, StreamPortFlags b) {
            return a = a | b;
        }
        inline StreamPortFlags& operator&=(StreamPortFlags& a, StreamPortFlags b) {
            return a = a & b;
        }
        inline StreamPortFlags& operator^=(StreamPortFlags& a, StreamPortFlags b) {
            return a = a ^ b;
        }

        // Audio format constants (from IEEE 1722.1-2021 Table 7.14)
        const uint8_t AUDIO_FORMAT_IEC_61883_6     = 0x00;
        const uint8_t AUDIO_FORMAT_AAF             = 0x01;
        const uint8_t AUDIO_FORMAT_EXPERIMENTAL    = 0xFF;

        // Control value types (from IEEE 1722.1-2021 Table 7.17)
        enum class ControlValueType : uint16_t {
            LinearInt8      = 0x0000,
            LinearUInt8     = 0x0001,
            LinearInt16     = 0x0002,
            LinearUInt16    = 0x0003,
            LinearInt32     = 0x0004,
            LinearUInt32    = 0x0005,
            LinearInt64     = 0x0006,
            LinearUInt64    = 0x0007,
            LinearFloat     = 0x0008,
            LinearDouble    = 0x0009,
            SelectorInt8    = 0x000A,
            SelectorUInt8   = 0x000B,
            SelectorInt16   = 0x000C,
            SelectorUInt16  = 0x000D,
            SelectorInt32   = 0x000E,
            SelectorUInt32  = 0x000F,
            SelectorInt64   = 0x0010,
            SelectorUInt64  = 0x0011,
            SelectorFloat   = 0x0012,
            SelectorDouble  = 0x0013,
            ArrayInt8       = 0x0014,
            ArrayUInt8      = 0x0015,
            ArrayInt16      = 0x0016,
            ArrayUInt16     = 0x0017,
            ArrayInt32      = 0x0018,
            ArrayUInt32     = 0x0019,
            ArrayInt64      = 0x001A,
            ArrayUInt64     = 0x001B,
            ArrayFloat      = 0x001C,
            ArrayDouble     = 0x001D,
            UTF8            = 0x001E,
            BodePlot        = 0x001F,
            SmpteTime       = 0x0020,
            SampleRate      = 0x0021,
            GptpTime        = 0x0022,
            VendorDefined   = 0xFFFE,
            Expansion       = 0xFFFF
        };

        // Control type constants (from IEEE 1722.1-2021 Table 7.18)
        const uint64_t CONTROL_TYPE_ENABLE                    = 0x0000000000000001ULL;
        const uint64_t CONTROL_TYPE_IDENTIFY                  = 0x0000000000000002ULL;
        const uint64_t CONTROL_TYPE_MUTE                      = 0x0000000000000003ULL;
        const uint64_t CONTROL_TYPE_INVERT                    = 0x0000000000000004ULL;
        const uint64_t CONTROL_TYPE_GAIN                      = 0x0000000000000005ULL;
        const uint64_t CONTROL_TYPE_ATTENUATE                 = 0x0000000000000006ULL;
        const uint64_t CONTROL_TYPE_DELAY                     = 0x0000000000000007ULL;
        const uint64_t CONTROL_TYPE_SRC_MODE                  = 0x0000000000000008ULL;
        const uint64_t CONTROL_TYPE_SNAPSHOT                  = 0x0000000000000009ULL;
        const uint64_t CONTROL_TYPE_POW_LINE_FREQ             = 0x000000000000000AULL;
        const uint64_t CONTROL_TYPE_POWER_STATUS              = 0x000000000000000BULL;
        const uint64_t CONTROL_TYPE_FAN_STATUS                = 0x000000000000000CULL;
        const uint64_t CONTROL_TYPE_TEMPERATURE               = 0x000000000000000DULL;
        const uint64_t CONTROL_TYPE_ALTITUDE                  = 0x000000000000000EULL;
        const uint64_t CONTROL_TYPE_ABSOLUTE_HUMIDITY         = 0x000000000000000FULL;
        const uint64_t CONTROL_TYPE_RELATIVE_HUMIDITY         = 0x0000000000000010ULL;
        const uint64_t CONTROL_TYPE_ORIENTATION               = 0x0000000000000011ULL;
        const uint64_t CONTROL_TYPE_VELOCITY                  = 0x0000000000000012ULL;
        const uint64_t CONTROL_TYPE_ACCELERATION              = 0x0000000000000013ULL;
        const uint64_t CONTROL_TYPE_FILTER_RESPONSE           = 0x0000000000000014ULL;
        const uint64_t CONTROL_TYPE_PANPOT_LR                 = 0x0000000000000015ULL;
        const uint64_t CONTROL_TYPE_PANPOT_FB                 = 0x0000000000000016ULL;
        const uint64_t CONTROL_TYPE_PANPOT_UD                 = 0x0000000000000017ULL;
        const uint64_t CONTROL_TYPE_PHANTOM_POWER             = 0x0000000000000018ULL;
        const uint64_t CONTROL_TYPE_AUDIO_SCALE               = 0x0000000000000019ULL;
        const uint64_t CONTROL_TYPE_AUDIO_METERS              = 0x000000000000001AULL;
        const uint64_t CONTROL_TYPE_AUDIO_SPECTRUM            = 0x000000000000001BULL;

        // Jack type constants (from IEEE 1722.1-2021 Table 7.16)
        const uint16_t JACK_TYPE_SPEAKER                      = 0x0000;
        const uint16_t JACK_TYPE_HEADPHONE                    = 0x0001;
        const uint16_t JACK_TYPE_ANALOG_MICROPHONE            = 0x0002;
        const uint16_t JACK_TYPE_SPDIF                        = 0x0003;
        const uint16_t JACK_TYPE_ADAT                         = 0x0004;
        const uint16_t JACK_TYPE_TDIF                         = 0x0005;
        const uint16_t JACK_TYPE_MADI                         = 0x0006;
        const uint16_t JACK_TYPE_UNBALANCED_ANALOG            = 0x0007;
        const uint16_t JACK_TYPE_BALANCED_ANALOG              = 0x0008;
        const uint16_t JACK_TYPE_DIGITAL                      = 0x0009;
        const uint16_t JACK_TYPE_MIDI                         = 0x000A;
        const uint16_t JACK_TYPE_AES_EBU                      = 0x000B;
        const uint16_t JACK_TYPE_COMPOSITE_VIDEO              = 0x000C;
        const uint16_t JACK_TYPE_SVIDEO                       = 0x000D;
        const uint16_t JACK_TYPE_COMPONENT_VIDEO              = 0x000E;
        const uint16_t JACK_TYPE_DVI                          = 0x000F;
        const uint16_t JACK_TYPE_HDMI                         = 0x0010;
        const uint16_t JACK_TYPE_UDI                          = 0x0011;
        const uint16_t JACK_TYPE_DISPLAYPORT                  = 0x0012;
        const uint16_t JACK_TYPE_ANTENNA                      = 0x0013;
        const uint16_t JACK_TYPE_ANALOG_TUNER                 = 0x0014;
        const uint16_t JACK_TYPE_ETHERNET                     = 0x0015;
        const uint16_t JACK_TYPE_WIFI                         = 0x0016;
        const uint16_t JACK_TYPE_USB                          = 0x0017;
        const uint16_t JACK_TYPE_PCI                          = 0x0018;
        const uint16_t JACK_TYPE_PCI_E                        = 0x0019;
        const uint16_t JACK_TYPE_SCSI                         = 0x001A;
        const uint16_t JACK_TYPE_ATA                          = 0x001B;
        const uint16_t JACK_TYPE_IMAGER                       = 0x001C;
        const uint16_t JACK_TYPE_IR                           = 0x001D;
        const uint16_t JACK_TYPE_THUNDERBOLT                  = 0x001E;
        const uint16_t JACK_TYPE_SATA                         = 0x001F;
        const uint16_t JACK_TYPE_SMARTMEDIA                   = 0x0020;
        const uint16_t JACK_TYPE_COMPACTFLASH                 = 0x0021;
        const uint16_t JACK_TYPE_SDCARD                       = 0x0022;
        const uint16_t JACK_TYPE_USB_C                        = 0x0023;

        // Memory object type constants
        const uint16_t MEMORY_OBJECT_TYPE_FIRMWARE_IMAGE      = 0x0000;
        const uint16_t MEMORY_OBJECT_TYPE_VENDOR_SPECIFIC     = 0x0001;
        const uint16_t MEMORY_OBJECT_TYPE_CRASH_DUMP          = 0x0002;
        const uint16_t MEMORY_OBJECT_TYPE_LOG_OBJECT          = 0x0003;
        const uint16_t MEMORY_OBJECT_TYPE_AUTOSTART_SETTINGS  = 0x0004;
        const uint16_t MEMORY_OBJECT_TYPE_SNAPSHOT_SETTINGS   = 0x0005;
        const uint16_t MEMORY_OBJECT_TYPE_SVG_MANUFACT        = 0x0006;
        const uint16_t MEMORY_OBJECT_TYPE_SVG_ENTITY          = 0x0007;
        const uint16_t MEMORY_OBJECT_TYPE_SVG_GENERIC         = 0x0008;
        const uint16_t MEMORY_OBJECT_TYPE_PNG_MANUFACT        = 0x0009;
        const uint16_t MEMORY_OBJECT_TYPE_PNG_ENTITY          = 0x000A;
        const uint16_t MEMORY_OBJECT_TYPE_PNG_GENERIC         = 0x000B;
        const uint16_t MEMORY_OBJECT_TYPE_DEC_MANUFACT        = 0x000C;
        const uint16_t MEMORY_OBJECT_TYPE_DEC_ENTITY          = 0x000D;
        const uint16_t MEMORY_OBJECT_TYPE_DEC_GENERIC         = 0x000E;

        // Jack flags
        enum class JackFlags : uint16_t {
            None                    = 0x0000,
            ClockSyncSource         = 0x0001,       ///< Jack can be a clock sync source
            CaptiveJack             = 0x0002        ///< Jack is captive (cannot be disconnected)
        };

        // Bitwise operators for JackFlags
        inline JackFlags operator|(JackFlags a, JackFlags b) {
            return static_cast<JackFlags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
        }
        inline JackFlags operator&(JackFlags a, JackFlags b) {
            return static_cast<JackFlags>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
        }
        inline JackFlags operator^(JackFlags a, JackFlags b) {
            return static_cast<JackFlags>(static_cast<uint16_t>(a) ^ static_cast<uint16_t>(b));
        }
        inline JackFlags operator~(JackFlags a) {
            return static_cast<JackFlags>(~static_cast<uint16_t>(a));
        }
        inline JackFlags& operator|=(JackFlags& a, JackFlags b) {
            return a = a | b;
        }
        inline JackFlags& operator&=(JackFlags& a, JackFlags b) {
            return a = a & b;
        }
        inline JackFlags& operator^=(JackFlags& a, JackFlags b) {
            return a = a ^ b;
        }

        // Signal type constants (from IEEE 1722.1-2021 Table 7.15)
        const uint16_t SIGNAL_TYPE_AUDIO_CLUSTER              = 0x0000;
        const uint16_t SIGNAL_TYPE_VIDEO_CLUSTER              = 0x0001;
        const uint16_t SIGNAL_TYPE_SENSOR_CLUSTER             = 0x0002;
        const uint16_t SIGNAL_TYPE_CLOCK_REFERENCE            = 0x0003;

        // Clock source type constants (from IEEE 1722.1-2021 Table 7.13)
        const uint16_t CLOCK_SOURCE_TYPE_INTERNAL             = 0x0000;
        const uint16_t CLOCK_SOURCE_TYPE_EXTERNAL             = 0x0001;
        const uint16_t CLOCK_SOURCE_TYPE_INPUT_STREAM         = 0x0002;
        const uint16_t CLOCK_SOURCE_TYPE_MEDIA_CLOCK_STREAM   = 0x0003;

        // Clock source location type constants
        const uint16_t CLOCK_SOURCE_LOCATION_TYPE_LOCAL       = 0x0000;
        const uint16_t CLOCK_SOURCE_LOCATION_TYPE_STREAM_INPUT = 0x0001;
        const uint16_t CLOCK_SOURCE_LOCATION_TYPE_STREAM_OUTPUT = 0x0002;
        const uint16_t CLOCK_SOURCE_LOCATION_TYPE_AVB_INTERFACE = 0x0003;

        // Mixer and matrix flags
        enum class MixerFlags : uint16_t {
            None            = 0x0000,
            LinearGain      = 0x0001,                   ///< Supports linear gain control
            LogarithmicGain = 0x0002,                   ///< Supports logarithmic gain control
            MatrixControl   = 0x0004                    ///< Has matrix control capability
        };

        // Bitwise operators for MixerFlags
        inline MixerFlags operator|(MixerFlags a, MixerFlags b) {
            return static_cast<MixerFlags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
        }
        inline MixerFlags operator&(MixerFlags a, MixerFlags b) {
            return static_cast<MixerFlags>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
        }
        inline MixerFlags operator^(MixerFlags a, MixerFlags b) {
            return static_cast<MixerFlags>(static_cast<uint16_t>(a) ^ static_cast<uint16_t>(b));
        }
        inline MixerFlags operator~(MixerFlags a) {
            return static_cast<MixerFlags>(~static_cast<uint16_t>(a));
        }
        inline MixerFlags& operator|=(MixerFlags& a, MixerFlags b) {
            return a = a | b;
        }
        inline MixerFlags& operator&=(MixerFlags& a, MixerFlags b) {
            return a = a & b;
        }
        inline MixerFlags& operator^=(MixerFlags& a, MixerFlags b) {
            return a = a ^ b;
        }

        enum class MatrixFlags : uint16_t {
            None            = 0x0000,
            LinearGain      = 0x0001,                   ///< Supports linear gain control
            LogarithmicGain = 0x0002                    ///< Supports logarithmic gain control
        };

        // Bitwise operators for MatrixFlags
        inline MatrixFlags operator|(MatrixFlags a, MatrixFlags b) {
            return static_cast<MatrixFlags>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
        }
        inline MatrixFlags operator&(MatrixFlags a, MatrixFlags b) {
            return static_cast<MatrixFlags>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
        }
        inline MatrixFlags operator^(MatrixFlags a, MatrixFlags b) {
            return static_cast<MatrixFlags>(static_cast<uint16_t>(a) ^ static_cast<uint16_t>(b));
        }
        inline MatrixFlags operator~(MatrixFlags a) {
            return static_cast<MatrixFlags>(~static_cast<uint16_t>(a));
        }
        inline MatrixFlags& operator|=(MatrixFlags& a, MatrixFlags b) {
            return a = a | b;
        }
        inline MatrixFlags& operator&=(MatrixFlags& a, MatrixFlags b) {
            return a = a & b;
        }
        inline MatrixFlags& operator^=(MatrixFlags& a, MatrixFlags b) {
            return a = a ^ b;
        }

        // Stream connection flags
        enum class ConnectionFlags : uint16_t {
            None            = 0x0000,
            ClassB          = 0x0001,               ///< Stream is Class B (default is Class A)
            FastConnect     = 0x0002,               ///< Fast connect mode
            SavedState      = 0x0004,               ///< Connection has saved state
            StreamingWait   = 0x0008,               ///< Waiting for streaming to start
            SupportsEncrypted = 0x0010,             ///< Supports encrypted streaming
            EncryptedPDU    = 0x0020,               ///< PDU is encrypted
            TalkerFailed    = 0x0040                ///< Talker has failed
        };

        // Stream info flags
        enum class StreamInfoFlags : uint32_t {
            None                    = 0x00000000,
            ClassB                  = 0x00000001,   ///< Stream is Class B
            FastConnect             = 0x00000002,   ///< Fast connect capable
            SavedState              = 0x00000004,   ///< Connection has saved state
            StreamingWait           = 0x00000008,   ///< Waiting for streaming
            SupportsEncrypted       = 0x00000010,   ///< Supports encrypted streaming
            EncryptedPDU           = 0x00000020,   ///< PDU is encrypted
            TalkerFailed           = 0x00000040,   ///< Talker has failed
            StreamVlanIDValid      = 0x00000080,   ///< Stream VLAN ID is valid
            Connected              = 0x00000100,   ///< Stream is connected
            MSRPFailureValid       = 0x00000200,   ///< MSRP failure info is valid
            DestMACValid           = 0x00000400,   ///< Destination MAC is valid
            MSRPAccLatValid        = 0x00000800,   ///< MSRP accumulated latency is valid
            StreamIDValid          = 0x00000800,   ///< Stream ID is valid
            StreamFormatValid      = 0x00001000    ///< Stream format is valid
        };

        // Bitwise operators for StreamInfoFlags
        inline StreamInfoFlags operator|(StreamInfoFlags a, StreamInfoFlags b) {
            return static_cast<StreamInfoFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }
        inline StreamInfoFlags operator&(StreamInfoFlags a, StreamInfoFlags b) {
            return static_cast<StreamInfoFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
        }
        inline StreamInfoFlags operator^(StreamInfoFlags a, StreamInfoFlags b) {
            return static_cast<StreamInfoFlags>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
        }
        inline StreamInfoFlags operator~(StreamInfoFlags a) {
            return static_cast<StreamInfoFlags>(~static_cast<uint32_t>(a));
        }
        inline StreamInfoFlags& operator|=(StreamInfoFlags& a, StreamInfoFlags b) {
            return a = a | b;
        }
        inline StreamInfoFlags& operator&=(StreamInfoFlags& a, StreamInfoFlags b) {
            return a = a & b;
        }
        inline StreamInfoFlags& operator^=(StreamInfoFlags& a, StreamInfoFlags b) {
            return a = a ^ b;
        }

        // Stream dynamic information structure
        struct StreamInfo {
            StreamInfoFlags stream_info_flags;      ///< Stream information flags
            StreamFormat stream_format;             ///< Current stream format
            uint64_t stream_id;                     ///< Stream ID (MAC address + unique ID)
            uint32_t msrp_accumulated_latency;      ///< MSRP accumulated latency
            uint8_t stream_dest_mac[6];             ///< Stream destination MAC address
            uint8_t msrp_failure_code;              ///< MSRP failure code
            uint8_t reserved;                       ///< Reserved byte
            uint64_t msrp_failure_bridge_id;        ///< MSRP failure bridge ID
            uint16_t stream_vlan_id;                ///< Stream VLAN ID
            uint16_t reserved2;                     ///< Reserved field
        };

        // AVB interface information flags
        enum class AvbInfoFlags : uint32_t {
            None                    = 0x00000000,
            ASCapable               = 0x00000001,   ///< AS capable
            GPTPEnabled             = 0x00000002,   ///< gPTP enabled
            SRPEnabled              = 0x00000004,   ///< SRP enabled
            GPTPGrandmasterSupported = 0x00000008   ///< gPTP grandmaster supported
        };

        // Bitwise operators for AvbInfoFlags
        inline AvbInfoFlags operator|(AvbInfoFlags a, AvbInfoFlags b) {
            return static_cast<AvbInfoFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
        }
        inline AvbInfoFlags operator&(AvbInfoFlags a, AvbInfoFlags b) {
            return static_cast<AvbInfoFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
        }
        inline AvbInfoFlags operator^(AvbInfoFlags a, AvbInfoFlags b) {
            return static_cast<AvbInfoFlags>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b));
        }
        inline AvbInfoFlags operator~(AvbInfoFlags a) {
            return static_cast<AvbInfoFlags>(~static_cast<uint32_t>(a));
        }
        inline AvbInfoFlags& operator|=(AvbInfoFlags& a, AvbInfoFlags b) {
            return a = a | b;
        }
        inline AvbInfoFlags& operator&=(AvbInfoFlags& a, AvbInfoFlags b) {
            return a = a & b;
        }
        inline AvbInfoFlags& operator^=(AvbInfoFlags& a, AvbInfoFlags b) {
            return a = a ^ b;
        }

        // AVB interface dynamic information
        struct AvbInfo {
            uint64_t gptp_grandmaster_id;           ///< gPTP grandmaster ID
            uint32_t propagation_delay;             ///< Propagation delay
            uint8_t gptp_domain_number;             ///< gPTP domain number
            AvbInfoFlags flags;                     ///< AVB info flags
        };

        // Audio mapping structure
        struct AudioMapping {
            uint16_t stream_index;                  ///< Stream index
            uint16_t stream_channel;                ///< Stream channel
            uint16_t cluster_offset;                ///< Cluster offset
            uint16_t cluster_channel;               ///< Cluster channel
        };

        using AudioMappings = std::vector<AudioMapping>;

        // Acquire/Lock state enumeration
        enum class AcquireState : uint8_t {
            NotSupported    = 0,                    ///< Acquire not supported
            NotAcquired     = 1,                    ///< Entity is not acquired
            TryAcquire      = 2,                    ///< Trying to acquire entity
            Acquired        = 3                     ///< Entity is acquired
        };

        enum class LockState : uint8_t {
            NotSupported    = 0,                    ///< Lock not supported
            NotLocked       = 1,                    ///< Entity is not locked
            TryLock         = 2,                    ///< Trying to lock entity
            Locked          = 3                     ///< Entity is locked
        };

        // Enumeration step flags for state machine control
        enum class EnumerationStep : uint16_t {
            None                    = 0x0000,
            GetMilanInfo           = 0x0001,        ///< Get Milan information
            CheckPackedDynamicInfo = 0x0002,        ///< Check packed dynamic info support
            RegisterUnsol          = 0x0004,        ///< Register for unsolicited notifications
            GetStaticModel         = 0x0008,        ///< Get static model descriptors
            GetDescriptorDynamicInfo = 0x0010,      ///< Get descriptor dynamic information
            GetDynamicInfo         = 0x0020         ///< Get other dynamic information
        };

        // Entity discovery state
        struct DiscoveryState {
            bool is_online;                         ///< Entity is online
            uint32_t available_index;               ///< Last seen available index
            uint64_t last_seen_time;                ///< Last seen timestamp (milliseconds since epoch)
            EnumerationStep enumeration_steps;      ///< Completed enumeration steps
        };

        // Command status tracking for AECP state machines
        enum class AemCommandStatus : uint8_t {
            Success                 = 0x00,         ///< Command succeeded
            NotImplemented          = 0x01,         ///< Command not implemented
            NoSuchDescriptor        = 0x02,         ///< No such descriptor
            EntityLocked            = 0x03,         ///< Entity is locked
            EntityAcquired          = 0x04,         ///< Entity is acquired
            NotAuthenticated        = 0x05,         ///< Not authenticated
            AuthenticationDisabled  = 0x06,         ///< Authentication disabled
            BadArguments            = 0x07,         ///< Bad arguments
            NoResources             = 0x08,         ///< No resources
            InProgress              = 0x09,         ///< Command in progress
            EntityMisbehaving       = 0x0A,         ///< Entity misbehaving
            NotSupported            = 0x0B,         ///< Command not supported
            StreamIsRunning         = 0x0C          ///< Stream is running
        };

        // Inflight command tracking for state machine retry logic
        struct InflightCommand {
            uint16_t sequence_id;                   ///< Command sequence ID
            uint64_t send_time_ms;                  ///< Send time in milliseconds since epoch
            uint8_t retry_count;                    ///< Current retry count
            uint8_t max_retries;                    ///< Maximum retries allowed
            uint32_t timeout_ms;                    ///< Timeout in milliseconds
            uint16_t command_type;                  ///< Command type (use AECP::AEM_Command_type values)
            uint64_t target_entity_id;              ///< Target entity ID
        };

        // Utility functions for descriptor validation
        bool is_valid_descriptor_type(uint16_t type);
        bool validate_entity_descriptor(const EntityDescriptor& descriptor);

        // Helper functions for string operations
        void safe_string_copy(char* dest, const char* src, size_t dest_size);
        void clear_string_field(char* str, size_t size);

        // Command payload structures for AECP AEM commands
        
        /**
         * @brief READ_DESCRIPTOR command/response payload
         */
        struct ReadDescriptorCommand {
            uint16_t configuration_index;          ///< Configuration index
            uint16_t descriptor_type;              ///< Descriptor type
            uint16_t descriptor_index;             ///< Descriptor index
        };

        struct ReadDescriptorResponse : ReadDescriptorCommand {
            // Variable-length descriptor data follows
            uint8_t descriptor_data[512];          ///< Descriptor data (variable length)
        };

        /**
         * @brief ACQUIRE_ENTITY command/response payload
         */
        struct AcquireEntityCommand {
            uint32_t flags;                        ///< Acquire flags
            uint64_t owner_id;                     ///< Owner entity ID
            uint16_t descriptor_type;              ///< Descriptor type to acquire
            uint16_t descriptor_index;             ///< Descriptor index to acquire
        };

        struct AcquireEntityResponse : AcquireEntityCommand {
            // No additional fields
        };

        /**
         * @brief LOCK_ENTITY command/response payload
         */
        struct LockEntityCommand {
            uint32_t flags;                        ///< Lock flags
            uint64_t locked_id;                    ///< Locked entity ID
            uint16_t descriptor_type;              ///< Descriptor type to lock
            uint16_t descriptor_index;             ///< Descriptor index to lock
        };

        struct LockEntityResponse : LockEntityCommand {
            // No additional fields
        };

        /**
         * @brief SET_CONFIGURATION command/response payload
         */
        struct SetConfigurationCommand {
            uint16_t configuration_index;          ///< Configuration index to set
        };

        struct SetConfigurationResponse : SetConfigurationCommand {
            // No additional fields
        };

        /**
         * @brief GET_CONFIGURATION command/response payload
         */
        struct GetConfigurationCommand {
            // No fields
        };

        struct GetConfigurationResponse {
            uint16_t configuration_index;          ///< Current configuration index
        };

        /**
         * @brief SET_STREAM_FORMAT command/response payload
         */
        struct SetStreamFormatCommand {
            uint16_t descriptor_type;              ///< Stream descriptor type
            uint16_t descriptor_index;             ///< Stream descriptor index
            uint64_t stream_format;                ///< Stream format to set
        };

        struct SetStreamFormatResponse : SetStreamFormatCommand {
            // No additional fields
        };

        /**
         * @brief GET_STREAM_FORMAT command/response payload
         */
        struct GetStreamFormatCommand {
            uint16_t descriptor_type;              ///< Stream descriptor type
            uint16_t descriptor_index;             ///< Stream descriptor index
        };

        struct GetStreamFormatResponse : GetStreamFormatCommand {
            uint64_t stream_format;                ///< Current stream format
        };

        /**
         * @brief SET_STREAM_INFO command/response payload
         */
        struct SetStreamInfoCommand {
            uint16_t descriptor_type;              ///< Stream descriptor type
            uint16_t descriptor_index;             ///< Stream descriptor index
            StreamInfo stream_info;                ///< Stream info to set
        };

        struct SetStreamInfoResponse : SetStreamInfoCommand {
            // No additional fields
        };

        /**
         * @brief GET_STREAM_INFO command/response payload
         */
        struct GetStreamInfoCommand {
            uint16_t descriptor_type;              ///< Stream descriptor type
            uint16_t descriptor_index;             ///< Stream descriptor index
        };

        struct GetStreamInfoResponse : GetStreamInfoCommand {
            StreamInfo stream_info;                ///< Current stream info
        };

        /**
         * @brief GET_AVB_INFO command/response payload
         */
        struct GetAvbInfoCommand {
            uint16_t descriptor_type;              ///< AVB interface descriptor type
            uint16_t descriptor_index;             ///< AVB interface descriptor index
        };

        struct GetAvbInfoResponse : GetAvbInfoCommand {
            AvbInfo avb_info;                      ///< Current AVB interface info
        };

        /**
         * @brief START_STREAMING command/response payload
         */
        struct StartStreamingCommand {
            uint16_t descriptor_type;              ///< Stream descriptor type
            uint16_t descriptor_index;             ///< Stream descriptor index
        };

        struct StartStreamingResponse : StartStreamingCommand {
            // No additional fields
        };

        /**
         * @brief STOP_STREAMING command/response payload
         */
        struct StopStreamingCommand {
            uint16_t descriptor_type;              ///< Stream descriptor type
            uint16_t descriptor_index;             ///< Stream descriptor index
        };

        struct StopStreamingResponse : StopStreamingCommand {
            // No additional fields
        };

        /**
         * @brief REGISTER_UNSOLICITED_NOTIFICATION command/response payload
         */
        struct RegisterUnsolicitedNotificationCommand {
            // No fields - registers for all notifications
        };

        struct RegisterUnsolicitedNotificationResponse {
            // No fields
        };

        /**
         * @brief GET_AUDIO_MAP command/response payload
         */
        struct GetAudioMapCommand {
            uint16_t descriptor_type;              ///< Audio map descriptor type
            uint16_t descriptor_index;             ///< Audio map descriptor index
            uint16_t map_index;                    ///< Map index within descriptor
        };

        struct GetAudioMapResponse : GetAudioMapCommand {
            uint16_t number_of_mappings;           ///< Number of mappings
            uint16_t mappings_offset;              ///< Offset to mappings
            // Variable-length mappings array follows (AudioMapping structures)
            AudioMapping mappings[64];             ///< Audio mappings (variable length)
        };

        // State machine support structures
        
        /**
         * @brief Entity model storage for discovered entities
         */
        struct EntityModel {
            EntityDescriptor entity_descriptor;
            std::vector<ConfigurationDescriptor> configurations;
            std::vector<AudioUnitDescriptor> audio_units;
            std::vector<StreamInputDescriptor> stream_inputs;
            std::vector<StreamOutputDescriptor> stream_outputs;
            std::vector<StreamPortInputDescriptor> stream_port_inputs;
            std::vector<StreamPortOutputDescriptor> stream_port_outputs;
            std::vector<AudioClusterDescriptor> audio_clusters;
            std::vector<AudioMapDescriptor> audio_maps;
            std::vector<AvbInterfaceDescriptor> avb_interfaces;
            std::vector<ClockSourceDescriptor> clock_sources;
            std::vector<ClockDomainDescriptor> clock_domains;
            std::vector<ControlDescriptor> controls;
            std::vector<SignalSelectorDescriptor> signal_selectors;
            std::vector<MixerDescriptor> mixers;
            std::vector<MatrixDescriptor> matrices;
            std::vector<LocaleDescriptor> locales;
            std::vector<StringsDescriptor> strings;
            std::vector<JackInputDescriptor> jack_inputs;
            std::vector<JackOutputDescriptor> jack_outputs;
            std::vector<MemoryObjectDescriptor> memory_objects;
            
            bool is_milan_device;                   ///< Indicates Milan compatibility
            bool supports_dynamic_info;            ///< Supports GET_DYNAMIC_INFO command
        };

        /**
         * @brief Dynamic entity state tracking
         */
        struct EntityDynamicState {
            DiscoveryState discovery;
            AcquireState acquire_state;
            LockState lock_state;
            uint64_t controlling_entity_id;        ///< Entity ID of controller (if acquired)
            uint64_t locked_by_entity_id;          ///< Entity ID of locker (if locked)
            
            // Dynamic info for each stream
            std::vector<StreamInfo> stream_input_info;
            std::vector<StreamInfo> stream_output_info;
            
            // Dynamic info for AVB interfaces
            std::vector<AvbInfo> avb_interface_info;
            
            // Current audio mappings
            std::vector<AudioMappings> audio_map_info;
            
            // Inflight command tracking
            std::vector<InflightCommand> inflight_commands;
            
            uint64_t last_update_time;              ///< Last update timestamp
        };

        /**
         * @brief Complete entity information combining static and dynamic data
         */
        struct EntityInfo {
            uint64_t entity_id;                     ///< Entity ID
            EntityModel static_model;               ///< Static entity model
            EntityDynamicState dynamic_state;       ///< Dynamic entity state
            
            bool is_online() const { return dynamic_state.discovery.is_online; }
            bool is_acquired() const { return dynamic_state.acquire_state == AcquireState::Acquired; }
            bool is_locked() const { return dynamic_state.lock_state == LockState::Locked; }
            
            // Convenience accessors
            const EntityDescriptor& get_entity_descriptor() const { return static_model.entity_descriptor; }
            uint16_t get_current_configuration() const { return static_model.entity_descriptor.current_configuration; }
            uint32_t get_available_index() const { return dynamic_state.discovery.available_index; }
        };

        /**
         * @brief Enumeration step tracking for entity discovery
         */
        struct EnumerationProgress {
            uint64_t entity_id;
            EnumerationStep completed_steps;
            EnumerationStep current_step;
            bool enumeration_complete;
            uint64_t enumeration_start_time;
            std::vector<uint16_t> remaining_descriptors; ///< Descriptor types still to read
        };

        // Command timeout and retry constants
        const uint32_t DEFAULT_COMMAND_TIMEOUT_MS = 5000;
        const uint8_t DEFAULT_MAX_RETRIES = 3;
        const uint32_t DISCOVERY_RETRY_DELAY_MS = 1000;
        const uint32_t ENUMERATION_TIMEOUT_MS = 30000;
        
        // Utility functions for command management
        bool is_command_inflight(const std::vector<InflightCommand>& inflight_commands, uint16_t sequence_id);
        void remove_inflight_command(std::vector<InflightCommand>& inflight_commands, uint16_t sequence_id);
        bool is_command_timeout(const InflightCommand& command, uint64_t current_time_ms);
        bool should_retry_command(const InflightCommand& command);

        // Enumeration helper functions
        EnumerationStep get_next_enumeration_step(EnumerationStep current_steps);
        bool is_enumeration_complete(const EntityModel& model, EnumerationStep completed_steps);
        std::vector<uint16_t> get_required_descriptor_types(const EntityModel& model);

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

        /**
         * @class ATDECCConnectionManagementProtocolPDU
         * @brief IEEE 1722.1-2021 ACMP Protocol Data Unit implementation
         * 
         * Implements the complete ACMP protocol for stream connection management
         * as specified in IEEE 1722.1-2021 Clause 8.2.
         */
        class ATDECCConnectionManagementProtocolPDU {
        private:
            uint8_t raw_octets[56]; // 44 octets after common header (12+44)

        public:
            // IEEE 1722-2016 Common Header Fields
            uint8_t subtype = 0x7B;        // ACMP subtype
            bool h = false;                // Header specific bit (always 0)
            uint8_t version = 0;           // Version (always 0)
            ACMP::ACMP_Message_type message_type;
            ACMP::ACMP_Status status;
            uint16_t control_data_length = 44;

            // ACMP Specific Fields (IEEE 1722.1-2021 Table 8.1)
            uint64_t stream_id;
            uint64_t controller_entity_id;
            uint64_t talker_entity_id;
            uint64_t listener_entity_id;
            uint16_t talker_unique_id;
            uint16_t listener_unique_id;
            uint8_t stream_dest_mac[6];
            uint16_t connection_count;
            uint16_t sequence_id;
            uint16_t flags;
            uint16_t stream_vlan_id;

            // Constructors
            ATDECCConnectionManagementProtocolPDU();
            ATDECCConnectionManagementProtocolPDU(const uint8_t* data, size_t length);

            // Serialization methods
            void serialize();
            void deserialize();
            uint8_t* get_raw_octets() { return raw_octets; }
            size_t get_size() const { return sizeof(raw_octets); }

            // Protocol helper methods
            bool is_command() const;
            bool is_response() const;
            void create_response(ACMP_Status response_status);
        };

    } // namespace ACMP

    /**
     * @namespace AECP
     * @brief ATDECC Enumeration and Control Protocol implementation
     * 
     * The AECP provides enumeration of and control of ATDECC Entities.
     */
    namespace AECP {

        // AECP Message Types (from IEEE 1722.1-2021 Table 9.1)
        enum class AECP_Message_type : uint8_t {
            AEM_COMMAND                     = 0x00,
            AEM_RESPONSE                    = 0x01,
            ADDRESS_ACCESS_COMMAND          = 0x02,
            ADDRESS_ACCESS_RESPONSE         = 0x03,
            AVC_COMMAND                     = 0x04,
            AVC_RESPONSE                    = 0x05,
            VENDOR_UNIQUE_COMMAND           = 0x06,
            VENDOR_UNIQUE_RESPONSE          = 0x07,
            HDCP_APM_COMMAND               = 0x08,
            HDCP_APM_RESPONSE              = 0x09,
            EXTENDED_COMMAND               = 0x0E,
            EXTENDED_RESPONSE              = 0x0F
        };

        // AECP Status Codes (from IEEE 1722.1-2021 Table 7.5)
        enum class AECP_Status : uint8_t {
            SUCCESS                        = 0x00,
            NOT_IMPLEMENTED                = 0x01,
            NO_SUCH_DESCRIPTOR             = 0x02,
            ENTITY_LOCKED                  = 0x03,
            ENTITY_ACQUIRED                = 0x04,
            NOT_AUTHENTICATED              = 0x05,
            AUTHENTICATION_DISABLED        = 0x06,
            BAD_ARGUMENTS                  = 0x07,
            NO_RESOURCES                   = 0x08,
            IN_PROGRESS                    = 0x09,
            ENTITY_MISBEHAVING             = 0x0A,
            NOT_SUPPORTED                  = 0x0B,
            STREAM_IS_RUNNING              = 0x0C
        };

        // AEM Command Types (from IEEE 1722.1-2021 Table 7.4)
        enum class AEM_Command_type : uint16_t {
            ACQUIRE_ENTITY                 = 0x0000,
            LOCK_ENTITY                    = 0x0001,
            ENTITY_AVAILABLE               = 0x0002,
            CONTROLLER_AVAILABLE           = 0x0003,
            READ_DESCRIPTOR                = 0x0004,
            WRITE_DESCRIPTOR               = 0x0005,
            SET_CONFIGURATION              = 0x0006,
            GET_CONFIGURATION              = 0x0007,
            SET_STREAM_FORMAT              = 0x0008,
            GET_STREAM_FORMAT              = 0x0009,
            SET_VIDEO_FORMAT               = 0x000A,
            GET_VIDEO_FORMAT               = 0x000B,
            SET_SENSOR_FORMAT              = 0x000C,
            GET_SENSOR_FORMAT              = 0x000D,
            SET_STREAM_INFO                = 0x000E,
            GET_STREAM_INFO                = 0x000F,
            SET_NAME                       = 0x0010,
            GET_NAME                       = 0x0011,
            SET_ASSOCIATION_ID             = 0x0012,
            GET_ASSOCIATION_ID             = 0x0013,
            SET_SAMPLING_RATE              = 0x0014,
            GET_SAMPLING_RATE              = 0x0015,
            SET_CLOCK_SOURCE               = 0x0016,
            GET_CLOCK_SOURCE               = 0x0017,
            SET_CONTROL                    = 0x0018,
            GET_CONTROL                    = 0x0019,
            INCREMENT_CONTROL              = 0x001A,
            DECREMENT_CONTROL              = 0x001B,
            SET_SIGNAL_SELECTOR            = 0x001C,
            GET_SIGNAL_SELECTOR            = 0x001D,
            SET_MIXER                      = 0x001E,
            GET_MIXER                      = 0x001F,
            SET_MATRIX                     = 0x0020,
            GET_MATRIX                     = 0x0021,
            START_STREAMING                = 0x0022,
            STOP_STREAMING                 = 0x0023,
            REGISTER_UNSOLICITED_NOTIFICATION = 0x0024,
            DEREGISTER_UNSOLICITED_NOTIFICATION = 0x0025,
            IDENTIFY_NOTIFICATION          = 0x0026,
            GET_AVB_INFO                   = 0x0027,
            GET_AS_PATH                    = 0x0028,
            GET_COUNTERS                   = 0x0029,
            REBOOT                         = 0x002A,
            GET_AUDIO_MAP                  = 0x002B,
            ADD_AUDIO_MAPPINGS             = 0x002C,
            REMOVE_AUDIO_MAPPINGS          = 0x002D,
            GET_VIDEO_MAP                  = 0x002E,
            ADD_VIDEO_MAPPINGS             = 0x002F,
            REMOVE_VIDEO_MAPPINGS          = 0x0030,
            GET_SENSOR_MAP                 = 0x0031,
            ADD_SENSOR_MAPPINGS            = 0x0032,
            REMOVE_SENSOR_MAPPINGS         = 0x0033,
            START_OPERATION                = 0x0034,
            ABORT_OPERATION                = 0x0035,
            OPERATION_STATUS               = 0x0036,
            AUTH_ADD_KEY                   = 0x0037,
            AUTH_DELETE_KEY                = 0x0038,
            AUTH_GET_KEY_LIST              = 0x0039,
            AUTH_GET_KEY                   = 0x003A,
            AUTH_ADD_KEY_TO_CHAIN          = 0x003B,
            AUTH_DELETE_KEY_FROM_CHAIN     = 0x003C,
            AUTH_GET_KEYCHAIN_LIST         = 0x003D,
            AUTH_GET_IDENTITY              = 0x003E,
            AUTH_ADD_CERTIFICATE_TO_CHAIN  = 0x003F,
            AUTH_DELETE_CERTIFICATE_FROM_CHAIN = 0x0040,
            AUTH_GET_CERTIFICATE_CHAIN     = 0x0041,
            AUTH_GET_CERTIFICATE           = 0x0042,
            AUTH_ADD_CRL                   = 0x0043,
            AUTH_DELETE_CRL                = 0x0044,
            AUTH_GET_CRL_LIST              = 0x0045,
            AUTH_GET_CRL                   = 0x0046,
            AUTHENTICATE                   = 0x0047,
            DEAUTHENTICATE                 = 0x0048,
            ENABLE_TRANSPORT_SECURITY      = 0x0049,
            DISABLE_TRANSPORT_SECURITY     = 0x004A,
            GET_DYNAMIC_INFO               = 0x004B  // Milan extension
        };

        /**
         * @class ATDECCEnumerationControlProtocolPDU
         * @brief IEEE 1722.1-2021 AECP Protocol Data Unit implementation
         * 
         * Implements the complete AECP protocol for entity enumeration and control
         * as specified in IEEE 1722.1-2021 Clause 9.
         */
        class ATDECCEnumerationControlProtocolPDU {
        private:
            uint8_t raw_octets[524]; // Maximum AECP payload size per IEEE 1722.1

        public:
            // IEEE 1722-2016 Common Header Fields
            uint8_t subtype = 0x7B;        // AECP subtype (same as ACMP)
            bool h = false;                // Header specific bit (always 0)
            uint8_t version = 0;           // Version (always 0)
            AECP::AECP_Message_type message_type;
            AECP::AECP_Status status;
            uint16_t control_data_length;

            // AECP Common Fields (IEEE 1722.1-2021 Clause 9.2.1.1)
            uint64_t target_entity_id;
            uint64_t controller_entity_id;
            uint16_t sequence_id;

            // AEM Specific Fields (for AEM_COMMAND/AEM_RESPONSE)
            bool unsolicited = false;      // Unsolicited notification flag
            AEM_Command_type command_type;
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
            void create_response(AECP_Status response_status);

            // AEM-specific helper methods
            void set_read_descriptor_command(uint16_t config_index, uint16_t desc_type, uint16_t desc_index);
            void set_read_descriptor_response(uint16_t config_index, uint16_t desc_type, uint16_t desc_index, 
                                            const uint8_t* descriptor_data, size_t descriptor_length);
            void get_read_descriptor_command(uint16_t& config_index, uint16_t& desc_type, uint16_t& desc_index) const;
            void get_read_descriptor_response(uint16_t& config_index, uint16_t& desc_type, uint16_t& desc_index,
                                            const uint8_t*& descriptor_data, size_t& descriptor_length) const;
        };

    } // namespace AECP

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
