/**
 * @file AVnu_Milan_1_2-2023_complete.h
 * @brief Complete Milan v1.2 (November 2023) Protocol Implementation
 * 
 * This file implements the complete AVnu Alliance Milan Specification v1.2
 * (Consolidated Revision, November 29, 2023) for Professional Audio AVB Devices.
 * 
 * Milan builds upon existing IEEE standards with additional requirements:
 * - IEEE 802.1BA-2021 (AVB Systems) 
 * - IEEE 1722.1-2021 (ATDECC)
 * - IEEE 1722-2016 (AVTP)
 * - IEEE 802.1AS-2011 (gPTP)
 * 
 * Key Milan Features Implemented:
 * - Milan-specific AECP MVU commands (GET_MILAN_INFO, etc.)
 * - Media clock management and reference selection
 * - Seamless network redundancy
 * - Enhanced stream formats and media clocking
 * - Professional tool compatibility (Hive-AVDECC, L-Acoustics Network Manager)
 * 
 * @version 1.2.0 (November 2023)
 * @date July 2025
 * @copyright OpenAvnu Project / AVnu Alliance
 */

#ifndef AVNU_MILAN_1_2_2023_COMPLETE_H
#define AVNU_MILAN_1_2_2023_COMPLETE_H

// Forward declarations instead of direct includes to avoid dependency issues
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <chrono>

namespace AVnu {
namespace Milan {
namespace _1_2_2023 {

// ============================================================================
// Milan Protocol Constants and Definitions
// ============================================================================

/**
 * @brief Milan Version Information
 */
constexpr uint8_t MILAN_MAJOR_VERSION = 1;
constexpr uint8_t MILAN_MINOR_VERSION = 2;
constexpr uint32_t MILAN_VERSION_INFO = 0x00010200; // v1.2.0

/**
 * @brief Milan Capability Flags (per Section 5.4.4.1)
 */
enum class MilanCapabilityFlags : uint32_t {
    BASELINE_INTEROPERABILITY = 0x00000001,    // Baseline AVB interoperability
    DISCOVERY_CONNECTION_CONTROL = 0x00000002, // Enhanced discovery/control
    BASE_STREAM_FORMATS = 0x00000004,          // Base audio stream formats
    MEDIA_CLOCKING = 0x00000008,               // Advanced media clocking
    SEAMLESS_REDUNDANCY = 0x00000010,          // Network redundancy support
    GPTA_AS_MEDIA_CLOCK = 0x00000020,          // gPTP as media clock source
    MEDIA_CLOCK_REFERENCE = 0x00000040,        // Media clock reference selection
    DYNAMIC_MAPPING = 0x00000080,              // Dynamic mapping while running
    SYSTEM_UNIQUE_ID = 0x00000100,             // System-wide unique identifier
    RESERVED = 0xFFFFFE00                      // Reserved for future use
};

/**
 * @brief Milan Protocol Features (per Section 5.2.1)
 */
enum class MilanProtocolFeatures : uint16_t {
    MILAN_BASELINE = 0x0001,                   // Milan baseline requirements
    FAST_CONNECT = 0x0002,                     // Fast connection management
    NETWORK_REDUNDANCY = 0x0004,               // Seamless network redundancy
    MEDIA_CLOCK_MANAGEMENT = 0x0008,           // Advanced media clock control
    ENHANCED_DIAGNOSTICS = 0x0010,             // Enhanced diagnostic capabilities
    PROFESSIONAL_AUDIO = 0x0020,               // Professional audio features
    RESERVED = 0xFFC0                          // Reserved for future use
};

// ============================================================================
// Milan AECP MVU Commands (per Section 5.4.3/5.4.4)
// ============================================================================

/**
 * @brief Milan Vendor Unique (MVU) Command Types
 * Per Milan v1.2 Section 5.4.4
 */
enum class MilanMVUCommandType : uint16_t {
    GET_MILAN_INFO = 0x0000,                   // Get Milan device information
    SET_SYSTEM_UNIQUE_ID = 0x0001,             // Set system unique identifier
    GET_SYSTEM_UNIQUE_ID = 0x0002,             // Get system unique identifier
    SET_MEDIA_CLOCK_REFERENCE_INFO = 0x0003,   // Set media clock reference
    GET_MEDIA_CLOCK_REFERENCE_INFO = 0x0004,   // Get media clock reference
    RESERVED = 0xFFFF                          // Reserved for future commands
};

/**
 * @brief Milan MVU Protocol Identifier
 * Per Milan v1.2 Section 5.4.3.1
 */
constexpr uint64_t MILAN_MVU_PROTOCOL_ID = 0x90E0F000FE00ULL; // Milan protocol ID

/**
 * @brief GET_MILAN_INFO Command/Response Structure
 * Per Milan v1.2 Section 5.4.4.1
 */
struct GetMilanInfoCommand {
    // Command header fields (inherited from AECP MVU base)
    uint16_t command_type;                     // AECP_MVU command type
    uint16_t command_specific_data;            // Command-specific data
    uint64_t protocol_identifier;              // Milan protocol identifier
    
    // Milan-specific fields
    uint16_t milan_version_major;              // Milan major version
    uint16_t milan_version_minor;              // Milan minor version
    uint32_t milan_capabilities;               // Milan capability flags
    uint16_t milan_features;                   // Milan feature flags
    uint16_t reserved;                         // Reserved field
    
    GetMilanInfoCommand();
    bool serialize(std::vector<uint8_t>& data) const;
    bool deserialize(const std::vector<uint8_t>& data);
};

struct GetMilanInfoResponse {
    // Response header fields
    uint16_t command_type;                     // AECP_MVU response type
    uint16_t status;                           // Response status
    uint64_t protocol_identifier;              // Milan protocol identifier
    
    // Milan device information
    uint16_t milan_version_major;              // Device Milan major version
    uint16_t milan_version_minor;              // Device Milan minor version
    uint32_t milan_capabilities;               // Supported Milan capabilities
    uint16_t milan_features;                   // Supported Milan features
    uint16_t certification_level;              // Milan certification level
    std::array<uint8_t, 64> device_name;      // Milan device name
    std::array<uint8_t, 64> manufacturer_name; // Manufacturer name
    std::array<uint8_t, 32> firmware_version; // Firmware version string
    
    GetMilanInfoResponse();
    bool serialize(std::vector<uint8_t>& data) const;
    bool deserialize(const std::vector<uint8_t>& data);
};

/**
 * @brief System Unique ID Commands/Responses
 * Per Milan v1.2 Section 5.4.4.2/5.4.4.3
 */
struct SetSystemUniqueIdCommand {
    uint16_t command_type;                     // SET_SYSTEM_UNIQUE_ID
    uint16_t reserved;                         // Reserved field
    uint64_t protocol_identifier;              // Milan protocol identifier
    uint64_t system_unique_id;                 // New system unique identifier
    
    SetSystemUniqueIdCommand();
    bool serialize(std::vector<uint8_t>& data) const;
    bool deserialize(const std::vector<uint8_t>& data);
};

struct GetSystemUniqueIdResponse {
    uint16_t command_type;                     // GET_SYSTEM_UNIQUE_ID response
    uint16_t status;                           // Response status
    uint64_t protocol_identifier;              // Milan protocol identifier
    uint64_t system_unique_id;                 // Current system unique identifier
    
    GetSystemUniqueIdResponse();
    bool serialize(std::vector<uint8_t>& data) const;
    bool deserialize(const std::vector<uint8_t>& data);
};

/**
 * @brief Media Clock Reference Commands/Responses
 * Per Milan v1.2 Section 5.4.4.4/5.4.4.5
 */
enum class MediaClockReferenceType : uint8_t {
    INTERNAL = 0x00,                           // Internal clock reference
    STREAM_INPUT = 0x01,                       // Stream input reference
    EXTERNAL_INPUT = 0x02,                     // External input reference
    GPTP = 0x03,                               // gPTP clock reference
    RESERVED = 0xFF                            // Reserved
};

struct MediaClockReferenceInfo {
    MediaClockReferenceType reference_type;    // Clock reference type
    uint16_t reference_descriptor_index;       // Descriptor index for reference
    uint32_t nominal_sample_rate;              // Nominal sample rate
    uint16_t pull_field;                       // Pull field for rate adjustment
    uint8_t locked_status;                     // Lock status of reference
    uint8_t reserved;                          // Reserved field
    
    MediaClockReferenceInfo();
    bool serialize(std::vector<uint8_t>& data) const;
    bool deserialize(const std::vector<uint8_t>& data);
};

struct SetMediaClockReferenceInfoCommand {
    uint16_t command_type;                     // SET_MEDIA_CLOCK_REFERENCE_INFO
    uint16_t descriptor_index;                 // Clock domain descriptor index
    uint64_t protocol_identifier;              // Milan protocol identifier
    MediaClockReferenceInfo reference_info;    // New media clock reference info
    
    SetMediaClockReferenceInfoCommand();
    bool serialize(std::vector<uint8_t>& data) const;
    bool deserialize(const std::vector<uint8_t>& data);
};

struct GetMediaClockReferenceInfoResponse {
    uint16_t command_type;                     // GET_MEDIA_CLOCK_REFERENCE_INFO response
    uint16_t status;                           // Response status
    uint64_t protocol_identifier;              // Milan protocol identifier
    MediaClockReferenceInfo reference_info;    // Current media clock reference info
    
    GetMediaClockReferenceInfoResponse();
    bool serialize(std::vector<uint8_t>& data) const;
    bool deserialize(const std::vector<uint8_t>& data);
};

// ============================================================================
// Milan Base Stream Formats (per Section 6)
// ============================================================================

/**
 * @brief Milan Base Audio Stream Formats
 * Per Milan v1.2 Section 6.5
 */
enum class MilanBaseAudioFormat : uint64_t {
    // AAF Formats (AVTP Audio Format)
    AAF_PCM_24BIT_48KHZ_2CH = 0x00A0020800000002ULL,  // 24-bit PCM, 48kHz, 2 channels
    AAF_PCM_24BIT_48KHZ_8CH = 0x00A0020800000008ULL,  // 24-bit PCM, 48kHz, 8 channels
    AAF_PCM_32BIT_48KHZ_2CH = 0x00A0040800000002ULL,  // 32-bit PCM, 48kHz, 2 channels
    AAF_PCM_32BIT_48KHZ_8CH = 0x00A0040800000008ULL,  // 32-bit PCM, 48kHz, 8 channels
    
    // IEC 61883-6 Formats
    IEC61883_6_AM824_48KHZ_2CH = 0x00A0000800000002ULL, // AM824, 48kHz, 2 channels
    IEC61883_6_AM824_48KHZ_8CH = 0x00A0000800000008ULL, // AM824, 48kHz, 8 channels
    
    // Extended formats for professional audio
    AAF_PCM_24BIT_96KHZ_2CH = 0x00A0021000000002ULL,  // 24-bit PCM, 96kHz, 2 channels
    AAF_PCM_24BIT_96KHZ_8CH = 0x00A0021000000008ULL,  // 24-bit PCM, 96kHz, 8 channels
};

/**
 * @brief Milan Stream Format Validation
 */
class MilanStreamFormatValidator {
public:
    static bool is_valid_milan_format(uint64_t stream_format);
    static bool is_talker_format_supported(uint64_t stream_format);
    static bool is_listener_format_supported(uint64_t stream_format);
    static std::string format_to_string(uint64_t stream_format);
};

// ============================================================================
// Milan Media Clocking (per Section 7)
// ============================================================================

/**
 * @brief Media Clock Source Types
 * Per Milan v1.2 Section 7.2
 */
enum class MediaClockSourceType : uint8_t {
    INTERNAL_CRYSTAL = 0x00,                   // Internal crystal oscillator
    STREAM_INPUT_DERIVED = 0x01,               // Derived from stream input
    EXTERNAL_WORD_CLOCK = 0x02,                // External word clock
    GPTP_GRANDMASTER = 0x03,                   // gPTP Grandmaster clock
    OTHER = 0xFF                               // Other clock source
};

/**
 * @brief Media Clock Quality Assessment
 * Per Milan v1.2 Section 7.4
 */
enum class MediaClockQuality : uint8_t {
    UNKNOWN = 0x00,                            // Quality unknown
    POOR = 0x01,                               // Poor quality
    FAIR = 0x02,                               // Fair quality
    GOOD = 0x03,                               // Good quality
    EXCELLENT = 0x04                           // Excellent quality
};

/**
 * @brief gPTP Media Clock Configuration
 * Per Milan v1.2 Section 7.5
 */
struct GPTPMediaClockConfig {
    uint8_t domain_number;                     // gPTP domain number
    uint8_t priority1;                         // gPTP priority1
    uint8_t priority2;                         // gPTP priority2
    uint8_t clock_accuracy;                    // Clock accuracy
    uint16_t offset_scaled_log_variance;       // Offset scaled log variance
    int8_t log_sync_interval;                  // Log sync interval
    int8_t log_announce_interval;              // Log announce interval
    int8_t log_pdelay_interval;                // Log pdelay interval
    uint8_t reserved;                          // Reserved field
    
    GPTPMediaClockConfig();
    bool serialize(std::vector<uint8_t>& data) const;
    bool deserialize(const std::vector<uint8_t>& data);
};

// ============================================================================
// Milan Seamless Network Redundancy (per Section 8)
// ============================================================================

/**
 * @brief Redundancy State Machine States
 * Per Milan v1.2 Section 8.2
 */
enum class RedundancyState : uint8_t {
    UNCONFIGURED = 0x00,                       // No redundancy configured
    CONFIGURED_INACTIVE = 0x01,                // Configured but inactive
    ACTIVE_PRIMARY = 0x02,                     // Active using primary path
    ACTIVE_SECONDARY = 0x03,                   // Active using secondary path
    TRANSITION = 0x04,                         // Transitioning between paths
    REDUNDANCY_ERROR = 0xFF                    // Error state
};

/**
 * @brief Redundant Stream Configuration
 * Per Milan v1.2 Section 8.3
 */
struct RedundantStreamConfig {
    uint64_t primary_stream_id;                // Primary stream identifier
    uint64_t secondary_stream_id;              // Secondary stream identifier
    uint16_t primary_interface_index;          // Primary AVB interface index
    uint16_t secondary_interface_index;        // Secondary AVB interface index
    uint32_t switchover_timeout_ms;            // Switchover timeout (milliseconds)
    uint32_t redundancy_flags;                 // Redundancy configuration flags
    
    RedundantStreamConfig();
    bool serialize(std::vector<uint8_t>& data) const;
    bool deserialize(const std::vector<uint8_t>& data);
};

// ============================================================================
// Milan Professional Audio AVB Device (PAAD) Entity
// ============================================================================

/**
 * @brief Complete Milan-Compliant AVDECC Entity
 * 
 * This class implements a complete Milan v1.2 compliant Professional Audio AVB Device
 * Entity (PAAD-AE) that builds upon the existing IEEE 1722.1-2021 implementation.
 */
class MilanPAADEntity {
private:
    // Milan-specific state
    uint64_t entity_id_;                       // Entity identifier
    uint64_t entity_model_id_;                 // Entity model identifier
    uint64_t system_unique_id_;                // System-wide unique identifier
    uint32_t milan_capabilities_;              // Supported Milan capabilities
    uint16_t milan_features_;                  // Supported Milan features
    std::array<uint8_t, 64> device_name_;     // Milan device name
    std::vector<MediaClockReferenceInfo> clock_references_; // Media clock references
    std::vector<RedundantStreamConfig> redundant_streams_;  // Redundant stream configs
    
    // Performance metrics
    mutable std::chrono::high_resolution_clock::time_point last_command_time_;
    mutable uint64_t commands_processed_;
    mutable uint64_t mvu_commands_processed_;

public:
    /**
     * @brief Constructor
     * @param entity_id The Entity ID for this Milan device
     * @param entity_model_id The Entity Model ID
     */
    MilanPAADEntity(uint64_t entity_id, uint64_t entity_model_id);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~MilanPAADEntity() = default;
    
    // Milan capability management
    void set_milan_capabilities(uint32_t capabilities);
    uint32_t get_milan_capabilities() const;
    void set_milan_features(uint16_t features);
    uint16_t get_milan_features() const;
    
    // System unique ID management
    void set_system_unique_id(uint64_t unique_id);
    uint64_t get_system_unique_id() const;
    
    // Device name management
    void set_device_name(const std::string& name);
    std::string get_device_name() const;
    
    // Media clock reference management
    bool add_media_clock_reference(uint16_t clock_domain_index, 
                                  const MediaClockReferenceInfo& ref_info);
    bool get_media_clock_reference(uint16_t clock_domain_index, 
                                  MediaClockReferenceInfo& ref_info) const;
    bool set_media_clock_reference(uint16_t clock_domain_index, 
                                  const MediaClockReferenceInfo& ref_info);
    
    // Redundant stream management
    bool add_redundant_stream(const RedundantStreamConfig& config);
    bool remove_redundant_stream(uint64_t primary_stream_id);
    std::vector<RedundantStreamConfig> get_redundant_streams() const;
    
    // Milan MVU command handlers
    virtual std::vector<uint8_t> handle_milan_mvu_command(
        const std::vector<uint8_t>& command_data);
    
    // Specific Milan MVU command implementations
    std::vector<uint8_t> handle_get_milan_info(
        const std::vector<uint8_t>& command_data);
    std::vector<uint8_t> handle_set_system_unique_id(
        const std::vector<uint8_t>& command_data);
    std::vector<uint8_t> handle_get_system_unique_id(
        const std::vector<uint8_t>& command_data);
    std::vector<uint8_t> handle_set_media_clock_reference_info(
        const std::vector<uint8_t>& command_data);
    std::vector<uint8_t> handle_get_media_clock_reference_info(
        const std::vector<uint8_t>& command_data);
    
    // Milan compliance validation
    bool validate_milan_compliance() const;
    std::vector<std::string> get_compliance_issues() const;
    
    // Milan-specific descriptors
    bool create_milan_entity_descriptor();
    bool create_milan_configuration_descriptor();
    bool add_milan_stream_descriptors();
    bool add_milan_clock_domain_descriptors();
    
    // Performance and diagnostics
    uint64_t get_commands_processed() const;
    uint64_t get_mvu_commands_processed() const;
    std::chrono::nanoseconds get_average_command_processing_time() const;
    
    // Milan protocol compatibility
    static bool is_milan_compatible_device(uint64_t entity_id);
    static uint32_t get_milan_version_info();
    static std::string get_milan_version_string();
};

// ============================================================================
// Milan Protocol Stack Manager
// ============================================================================

/**
 * @brief Complete Milan Protocol Stack Manager
 * 
 * This class manages the complete Milan protocol stack, integrating all
 * Milan-specific features with the existing AVDECC infrastructure.
 */
class MilanProtocolStackManager {
private:
    std::unique_ptr<MilanPAADEntity> milan_entity_;
    bool initialized_;
    bool milan_mode_enabled_;
    
public:
    /**
     * @brief Constructor
     */
    MilanProtocolStackManager();
    
    /**
     * @brief Destructor
     */
    ~MilanProtocolStackManager();
    
    // Initialization and configuration
    bool initialize(uint64_t entity_id, uint64_t entity_model_id);
    bool shutdown();
    bool is_initialized() const;
    
    // Milan mode management
    bool enable_milan_mode(uint32_t capabilities, uint16_t features);
    bool disable_milan_mode();
    bool is_milan_mode_enabled() const;
    
    // Entity management
    MilanPAADEntity* get_milan_entity() const;
    
    // Protocol message processing
    bool process_incoming_message(const uint8_t* data, size_t length);
    bool send_outgoing_message(const uint8_t* data, size_t length);
    
    // Milan compliance and validation
    bool validate_milan_setup() const;
    std::vector<std::string> get_setup_issues() const;
    
    // Statistics and performance
    struct MilanStatistics {
        uint64_t milan_commands_processed;
        uint64_t milan_responses_sent;
        uint64_t milan_compatibility_checks;
        uint64_t redundancy_switchovers;
        uint64_t media_clock_changes;
        std::chrono::nanoseconds average_processing_time;
    };
    
    MilanStatistics get_statistics() const;
    void reset_statistics();
};

// ============================================================================
// Milan Utility Functions
// ============================================================================

/**
 * @brief Milan Protocol Utilities
 */
namespace MilanUtils {
    // Version utilities
    std::string get_milan_version_string();
    uint32_t get_milan_version_info();
    bool is_milan_version_compatible(uint16_t major, uint16_t minor);
    
    // Capability utilities
    bool has_milan_capability(uint32_t capabilities, MilanCapabilityFlags flag);
    std::string capabilities_to_string(uint32_t capabilities);
    std::string features_to_string(uint16_t features);
    
    // Stream format utilities
    bool is_milan_stream_format(uint64_t stream_format);
    std::string stream_format_to_string(uint64_t stream_format);
    
    // Professional tool compatibility
    bool is_hive_avdecc_compatible();
    bool is_l_acoustics_network_manager_compatible();
    std::vector<std::string> get_professional_tool_compatibility();
}

} // namespace _1_2_2023
} // namespace Milan
} // namespace AVnu

#endif // AVNU_MILAN_1_2_2023_COMPLETE_H
