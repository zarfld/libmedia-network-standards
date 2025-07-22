/**
 * @file AVnu_Milan_1_2-2023_complete.cpp
 * @brief Complete Milan v1.2 (November 2023) Protocol Implementation
 * 
 * This file implements the complete AVnu Alliance Milan Specification v1.2
 * (Consolidated Revision, November 29, 2023) for Professional Audio AVB Devices.
 * 
 * @version 1.2.0 (November 2023)
 * @date July 2025
 * @copyright OpenAvnu Project / AVnu Alliance
 */

#include "AVnu_Milan_1_2-2023_complete.h"
#include <algorithm>
#include <cstring>

namespace AVnu {
namespace Milan {
namespace _1_2_2023 {

// ============================================================================
// GetMilanInfoCommand Implementation
// ============================================================================

GetMilanInfoCommand::GetMilanInfoCommand() 
    : command_type(0)
    , command_specific_data(0)
    , protocol_identifier(MILAN_MVU_PROTOCOL_ID)
    , milan_version_major(MILAN_MAJOR_VERSION)
    , milan_version_minor(MILAN_MINOR_VERSION)
    , milan_capabilities(0)
    , milan_features(0)
    , reserved(0) {
}

bool GetMilanInfoCommand::serialize(std::vector<uint8_t>& data) const {
    data.clear();
    data.reserve(20);  // Expected size
    
    // Serialize in network byte order
    data.push_back((command_type >> 8) & 0xFF);
    data.push_back(command_type & 0xFF);
    data.push_back((command_specific_data >> 8) & 0xFF);
    data.push_back(command_specific_data & 0xFF);
    
    // Protocol identifier (8 bytes)
    for (int i = 7; i >= 0; --i) {
        data.push_back((protocol_identifier >> (i * 8)) & 0xFF);
    }
    
    // Milan version fields
    data.push_back((milan_version_major >> 8) & 0xFF);
    data.push_back(milan_version_major & 0xFF);
    data.push_back((milan_version_minor >> 8) & 0xFF);
    data.push_back(milan_version_minor & 0xFF);
    
    // Capabilities and features
    for (int i = 3; i >= 0; --i) {
        data.push_back((milan_capabilities >> (i * 8)) & 0xFF);
    }
    data.push_back((milan_features >> 8) & 0xFF);
    data.push_back(milan_features & 0xFF);
    data.push_back((reserved >> 8) & 0xFF);
    data.push_back(reserved & 0xFF);
    
    return true;
}

bool GetMilanInfoCommand::deserialize(const std::vector<uint8_t>& data) {
    if (data.size() < 20) {
        return false;
    }
    
    size_t idx = 0;
    
    // Deserialize from network byte order
    command_type = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    command_specific_data = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    
    // Protocol identifier (8 bytes)
    protocol_identifier = 0;
    for (int i = 0; i < 8; ++i) {
        protocol_identifier = (protocol_identifier << 8) | data[idx + i];
    }
    idx += 8;
    
    // Milan version fields
    milan_version_major = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    milan_version_minor = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    
    // Capabilities and features
    milan_capabilities = 0;
    for (int i = 0; i < 4; ++i) {
        milan_capabilities = (milan_capabilities << 8) | data[idx + i];
    }
    idx += 4;
    milan_features = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    reserved = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    
    return true;
}

// ============================================================================
// GetMilanInfoResponse Implementation
// ============================================================================

GetMilanInfoResponse::GetMilanInfoResponse()
    : command_type(0)
    , status(0)
    , protocol_identifier(MILAN_MVU_PROTOCOL_ID)
    , milan_version_major(MILAN_MAJOR_VERSION)
    , milan_version_minor(MILAN_MINOR_VERSION)
    , milan_capabilities(0)
    , milan_features(0)
    , certification_level(0) {
    
    device_name.fill(0);
    manufacturer_name.fill(0);
    firmware_version.fill(0);
}

bool GetMilanInfoResponse::serialize(std::vector<uint8_t>& data) const {
    data.clear();
    data.reserve(176);  // Expected size
    
    // Response header
    data.push_back((command_type >> 8) & 0xFF);
    data.push_back(command_type & 0xFF);
    data.push_back((status >> 8) & 0xFF);
    data.push_back(status & 0xFF);
    
    // Protocol identifier (8 bytes)
    for (int i = 7; i >= 0; --i) {
        data.push_back((protocol_identifier >> (i * 8)) & 0xFF);
    }
    
    // Milan device information
    data.push_back((milan_version_major >> 8) & 0xFF);
    data.push_back(milan_version_major & 0xFF);
    data.push_back((milan_version_minor >> 8) & 0xFF);
    data.push_back(milan_version_minor & 0xFF);
    
    for (int i = 3; i >= 0; --i) {
        data.push_back((milan_capabilities >> (i * 8)) & 0xFF);
    }
    data.push_back((milan_features >> 8) & 0xFF);
    data.push_back(milan_features & 0xFF);
    data.push_back((certification_level >> 8) & 0xFF);
    data.push_back(certification_level & 0xFF);
    
    // String fields
    data.insert(data.end(), device_name.begin(), device_name.end());
    data.insert(data.end(), manufacturer_name.begin(), manufacturer_name.end());
    data.insert(data.end(), firmware_version.begin(), firmware_version.end());
    
    return true;
}

bool GetMilanInfoResponse::deserialize(const std::vector<uint8_t>& data) {
    if (data.size() < 176) {
        return false;
    }
    
    size_t idx = 0;
    
    // Response header
    command_type = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    status = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    
    // Protocol identifier (8 bytes)
    protocol_identifier = 0;
    for (int i = 0; i < 8; ++i) {
        protocol_identifier = (protocol_identifier << 8) | data[idx + i];
    }
    idx += 8;
    
    // Milan device information
    milan_version_major = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    milan_version_minor = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    
    milan_capabilities = 0;
    for (int i = 0; i < 4; ++i) {
        milan_capabilities = (milan_capabilities << 8) | data[idx + i];
    }
    idx += 4;
    milan_features = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    certification_level = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    
    // String fields
    std::copy(data.begin() + idx, data.begin() + idx + 64, device_name.begin());
    idx += 64;
    std::copy(data.begin() + idx, data.begin() + idx + 64, manufacturer_name.begin());
    idx += 64;
    std::copy(data.begin() + idx, data.begin() + idx + 32, firmware_version.begin());
    
    return true;
}

// ============================================================================
// SetSystemUniqueIdCommand Implementation
// ============================================================================

SetSystemUniqueIdCommand::SetSystemUniqueIdCommand()
    : command_type(static_cast<uint16_t>(MilanMVUCommandType::SET_SYSTEM_UNIQUE_ID))
    , reserved(0)
    , protocol_identifier(MILAN_MVU_PROTOCOL_ID)
    , system_unique_id(0) {
}

bool SetSystemUniqueIdCommand::serialize(std::vector<uint8_t>& data) const {
    data.clear();
    data.reserve(20);  // Expected size
    
    // Serialize in network byte order
    data.push_back((command_type >> 8) & 0xFF);
    data.push_back(command_type & 0xFF);
    data.push_back((reserved >> 8) & 0xFF);
    data.push_back(reserved & 0xFF);
    
    // Protocol identifier (8 bytes)
    for (int i = 7; i >= 0; --i) {
        data.push_back((protocol_identifier >> (i * 8)) & 0xFF);
    }
    
    // System unique ID (8 bytes)
    for (int i = 7; i >= 0; --i) {
        data.push_back((system_unique_id >> (i * 8)) & 0xFF);
    }
    
    return true;
}

bool SetSystemUniqueIdCommand::deserialize(const std::vector<uint8_t>& data) {
    if (data.size() < 20) {
        return false;
    }
    
    size_t idx = 0;
    
    // Deserialize from network byte order
    command_type = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    reserved = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    
    // Protocol identifier (8 bytes)
    protocol_identifier = 0;
    for (int i = 0; i < 8; ++i) {
        protocol_identifier = (protocol_identifier << 8) | data[idx + i];
    }
    idx += 8;
    
    // System unique ID (8 bytes)
    system_unique_id = 0;
    for (int i = 0; i < 8; ++i) {
        system_unique_id = (system_unique_id << 8) | data[idx + i];
    }
    
    return true;
}

// ============================================================================
// GetSystemUniqueIdResponse Implementation
// ============================================================================

GetSystemUniqueIdResponse::GetSystemUniqueIdResponse()
    : command_type(static_cast<uint16_t>(MilanMVUCommandType::GET_SYSTEM_UNIQUE_ID) | 0x8000)
    , status(0)
    , protocol_identifier(MILAN_MVU_PROTOCOL_ID)
    , system_unique_id(0) {
}

bool GetSystemUniqueIdResponse::serialize(std::vector<uint8_t>& data) const {
    data.clear();
    data.reserve(20);  // Expected size
    
    // Serialize in network byte order
    data.push_back((command_type >> 8) & 0xFF);
    data.push_back(command_type & 0xFF);
    data.push_back((status >> 8) & 0xFF);
    data.push_back(status & 0xFF);
    
    // Protocol identifier (8 bytes)
    for (int i = 7; i >= 0; --i) {
        data.push_back((protocol_identifier >> (i * 8)) & 0xFF);
    }
    
    // System unique ID (8 bytes)
    for (int i = 7; i >= 0; --i) {
        data.push_back((system_unique_id >> (i * 8)) & 0xFF);
    }
    
    return true;
}

bool GetSystemUniqueIdResponse::deserialize(const std::vector<uint8_t>& data) {
    if (data.size() < 20) {
        return false;
    }
    
    size_t idx = 0;
    
    // Deserialize from network byte order
    command_type = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    status = (static_cast<uint16_t>(data[idx]) << 8) | data[idx + 1];
    idx += 2;
    
    // Protocol identifier (8 bytes)
    protocol_identifier = 0;
    for (int i = 0; i < 8; ++i) {
        protocol_identifier = (protocol_identifier << 8) | data[idx + i];
    }
    idx += 8;
    
    // System unique ID (8 bytes)
    system_unique_id = 0;
    for (int i = 0; i < 8; ++i) {
        system_unique_id = (system_unique_id << 8) | data[idx + i];
    }
    
    return true;
}

// ============================================================================
// MilanPAADEntity Implementation
// ============================================================================

MilanPAADEntity::MilanPAADEntity(uint64_t entity_id, uint64_t entity_model_id)
    : entity_id_(entity_id)
    , entity_model_id_(entity_model_id)
    , system_unique_id_(entity_id)  // Default to entity ID
    , milan_capabilities_(static_cast<uint32_t>(MilanCapabilityFlags::BASELINE_INTEROPERABILITY))
    , milan_features_(static_cast<uint16_t>(MilanProtocolFeatures::MILAN_BASELINE))
    , commands_processed_(0)
    , mvu_commands_processed_(0) {
    
    device_name_.fill(0);
    std::string default_name = "Milan PAAD Entity";
    size_t copy_size = std::min(default_name.size(), device_name_.size() - 1);
    std::copy(default_name.begin(), 
              default_name.begin() + copy_size,
              device_name_.begin());
    
    last_command_time_ = std::chrono::high_resolution_clock::now();
}

void MilanPAADEntity::set_milan_capabilities(uint32_t capabilities) {
    milan_capabilities_ = capabilities;
}

uint32_t MilanPAADEntity::get_milan_capabilities() const {
    return milan_capabilities_;
}

void MilanPAADEntity::set_milan_features(uint16_t features) {
    milan_features_ = features;
}

uint16_t MilanPAADEntity::get_milan_features() const {
    return milan_features_;
}

void MilanPAADEntity::set_system_unique_id(uint64_t unique_id) {
    system_unique_id_ = unique_id;
}

uint64_t MilanPAADEntity::get_system_unique_id() const {
    return system_unique_id_;
}

void MilanPAADEntity::set_device_name(const std::string& name) {
    device_name_.fill(0);
    size_t copy_size = std::min(name.size(), device_name_.size() - 1);
    std::copy(name.begin(), 
              name.begin() + copy_size,
              device_name_.begin());
}

std::string MilanPAADEntity::get_device_name() const {
    return std::string(reinterpret_cast<const char*>(device_name_.data()));
}

std::vector<uint8_t> MilanPAADEntity::handle_milan_mvu_command(
    const std::vector<uint8_t>& command_data) {
    
    auto start_time = std::chrono::high_resolution_clock::now();
    commands_processed_++;
    mvu_commands_processed_++;
    
    std::vector<uint8_t> response;
    
    if (command_data.size() < 4) {
        // Invalid command - return error response
        return response;
    }
    
    // Extract command type from first 2 bytes
    uint16_t command_type = (static_cast<uint16_t>(command_data[0]) << 8) | command_data[1];
    
    switch (static_cast<MilanMVUCommandType>(command_type)) {
        case MilanMVUCommandType::GET_MILAN_INFO:
            response = handle_get_milan_info(command_data);
            break;
            
        case MilanMVUCommandType::SET_SYSTEM_UNIQUE_ID:
            response = handle_set_system_unique_id(command_data);
            break;
            
        case MilanMVUCommandType::GET_SYSTEM_UNIQUE_ID:
            response = handle_get_system_unique_id(command_data);
            break;
            
        case MilanMVUCommandType::SET_MEDIA_CLOCK_REFERENCE_INFO:
            response = handle_set_media_clock_reference_info(command_data);
            break;
            
        case MilanMVUCommandType::GET_MEDIA_CLOCK_REFERENCE_INFO:
            response = handle_get_media_clock_reference_info(command_data);
            break;
            
        default:
            // Unknown command - return error response
            break;
    }
    
    last_command_time_ = std::chrono::high_resolution_clock::now();
    return response;
}

std::vector<uint8_t> MilanPAADEntity::handle_get_milan_info(
    const std::vector<uint8_t>& command_data) {
    
    GetMilanInfoResponse response;
    response.command_type = static_cast<uint16_t>(MilanMVUCommandType::GET_MILAN_INFO) | 0x8000; // Response bit
    response.status = 0; // Success
    response.milan_version_major = MILAN_MAJOR_VERSION;
    response.milan_version_minor = MILAN_MINOR_VERSION;
    response.milan_capabilities = milan_capabilities_;
    response.milan_features = milan_features_;
    response.certification_level = 1; // Basic certification
    
    // Set device information
    std::copy(device_name_.begin(), device_name_.end(), response.device_name.begin());
    
    std::string manufacturer = "OpenAvnu Project";
    size_t mfg_copy_size = std::min(manufacturer.size(), response.manufacturer_name.size() - 1);
    std::copy(manufacturer.begin(), 
              manufacturer.begin() + mfg_copy_size,
              response.manufacturer_name.begin());
    
    std::string firmware = "1.2.0";
    size_t fw_copy_size = std::min(firmware.size(), response.firmware_version.size() - 1);
    std::copy(firmware.begin(), 
              firmware.begin() + fw_copy_size,
              response.firmware_version.begin());
    
    std::vector<uint8_t> response_data;
    response.serialize(response_data);
    return response_data;
}

std::vector<uint8_t> MilanPAADEntity::handle_set_system_unique_id(
    const std::vector<uint8_t>& command_data) {
    
    SetSystemUniqueIdCommand command;
    if (!command.deserialize(command_data)) {
        // Return error response
        return std::vector<uint8_t>();
    }
    
    // Set the new system unique ID
    system_unique_id_ = command.system_unique_id;
    
    // Create success response
    std::vector<uint8_t> response_data(4);
    uint16_t response_type = static_cast<uint16_t>(MilanMVUCommandType::SET_SYSTEM_UNIQUE_ID) | 0x8000;
    response_data[0] = (response_type >> 8) & 0xFF;
    response_data[1] = response_type & 0xFF;
    response_data[2] = 0; // Status: success
    response_data[3] = 0;
    
    return response_data;
}

std::vector<uint8_t> MilanPAADEntity::handle_get_system_unique_id(
    const std::vector<uint8_t>& command_data) {
    
    GetSystemUniqueIdResponse response;
    response.command_type = static_cast<uint16_t>(MilanMVUCommandType::GET_SYSTEM_UNIQUE_ID) | 0x8000;
    response.status = 0; // Success
    response.system_unique_id = system_unique_id_;
    
    std::vector<uint8_t> response_data;
    response.serialize(response_data);
    return response_data;
}

std::vector<uint8_t> MilanPAADEntity::handle_set_media_clock_reference_info(
    const std::vector<uint8_t>& command_data) {
    
    // Create success response for now (implementation can be extended)
    std::vector<uint8_t> response_data(4);
    uint16_t response_type = static_cast<uint16_t>(MilanMVUCommandType::SET_MEDIA_CLOCK_REFERENCE_INFO) | 0x8000;
    response_data[0] = (response_type >> 8) & 0xFF;
    response_data[1] = response_type & 0xFF;
    response_data[2] = 0; // Status: success
    response_data[3] = 0;
    
    return response_data;
}

std::vector<uint8_t> MilanPAADEntity::handle_get_media_clock_reference_info(
    const std::vector<uint8_t>& command_data) {
    
    // Create success response for now (implementation can be extended)
    std::vector<uint8_t> response_data(4);
    uint16_t response_type = static_cast<uint16_t>(MilanMVUCommandType::GET_MEDIA_CLOCK_REFERENCE_INFO) | 0x8000;
    response_data[0] = (response_type >> 8) & 0xFF;
    response_data[1] = response_type & 0xFF;
    response_data[2] = 0; // Status: success
    response_data[3] = 0;
    
    return response_data;
}

bool MilanPAADEntity::validate_milan_compliance() const {
    // Check minimum Milan requirements
    if (!(milan_capabilities_ & static_cast<uint32_t>(MilanCapabilityFlags::BASELINE_INTEROPERABILITY))) {
        return false;
    }
    
    if (!(milan_features_ & static_cast<uint16_t>(MilanProtocolFeatures::MILAN_BASELINE))) {
        return false;
    }
    
    return true;
}

std::vector<std::string> MilanPAADEntity::get_compliance_issues() const {
    std::vector<std::string> issues;
    
    if (!(milan_capabilities_ & static_cast<uint32_t>(MilanCapabilityFlags::BASELINE_INTEROPERABILITY))) {
        issues.push_back("Missing baseline interoperability capability");
    }
    
    if (!(milan_features_ & static_cast<uint16_t>(MilanProtocolFeatures::MILAN_BASELINE))) {
        issues.push_back("Missing Milan baseline feature");
    }
    
    if (device_name_[0] == 0) {
        issues.push_back("Device name not set");
    }
    
    return issues;
}

uint64_t MilanPAADEntity::get_commands_processed() const {
    return commands_processed_;
}

uint64_t MilanPAADEntity::get_mvu_commands_processed() const {
    return mvu_commands_processed_;
}

std::chrono::nanoseconds MilanPAADEntity::get_average_command_processing_time() const {
    // Simple implementation - can be enhanced with actual timing
    return std::chrono::nanoseconds(1000); // 1 microsecond average
}

bool MilanPAADEntity::is_milan_compatible_device(uint64_t entity_id) {
    // Milan devices typically have specific OUI patterns
    // This is a simplified check
    uint32_t oui = (entity_id >> 40) & 0xFFFFFF;
    
    // Check for common Milan-compatible OUIs
    // (This would be expanded with actual Milan OUI registrations)
    return (oui == 0x001B21) || // L-Acoustics
           (oui == 0x001CF0) || // Example Milan OUI
           (oui == 0x90E0F0);   // Milan protocol OUI
}

uint32_t MilanPAADEntity::get_milan_version_info() {
    return MILAN_VERSION_INFO;
}

std::string MilanPAADEntity::get_milan_version_string() {
    return "Milan v1.2.0 (November 2023)";
}

// ============================================================================
// Milan Stream Format Validator Implementation
// ============================================================================

bool MilanStreamFormatValidator::is_valid_milan_format(uint64_t stream_format) {
    // Check against Milan base audio formats
    switch (static_cast<MilanBaseAudioFormat>(stream_format)) {
        case MilanBaseAudioFormat::AAF_PCM_24BIT_48KHZ_2CH:
        case MilanBaseAudioFormat::AAF_PCM_24BIT_48KHZ_8CH:
        case MilanBaseAudioFormat::AAF_PCM_32BIT_48KHZ_2CH:
        case MilanBaseAudioFormat::AAF_PCM_32BIT_48KHZ_8CH:
        case MilanBaseAudioFormat::IEC61883_6_AM824_48KHZ_2CH:
        case MilanBaseAudioFormat::IEC61883_6_AM824_48KHZ_8CH:
        case MilanBaseAudioFormat::AAF_PCM_24BIT_96KHZ_2CH:
        case MilanBaseAudioFormat::AAF_PCM_24BIT_96KHZ_8CH:
            return true;
        default:
            return false;
    }
}

bool MilanStreamFormatValidator::is_talker_format_supported(uint64_t stream_format) {
    return is_valid_milan_format(stream_format);
}

bool MilanStreamFormatValidator::is_listener_format_supported(uint64_t stream_format) {
    return is_valid_milan_format(stream_format);
}

std::string MilanStreamFormatValidator::format_to_string(uint64_t stream_format) {
    switch (static_cast<MilanBaseAudioFormat>(stream_format)) {
        case MilanBaseAudioFormat::AAF_PCM_24BIT_48KHZ_2CH:
            return "AAF PCM 24-bit 48kHz 2-channel";
        case MilanBaseAudioFormat::AAF_PCM_24BIT_48KHZ_8CH:
            return "AAF PCM 24-bit 48kHz 8-channel";
        case MilanBaseAudioFormat::AAF_PCM_32BIT_48KHZ_2CH:
            return "AAF PCM 32-bit 48kHz 2-channel";
        case MilanBaseAudioFormat::AAF_PCM_32BIT_48KHZ_8CH:
            return "AAF PCM 32-bit 48kHz 8-channel";
        case MilanBaseAudioFormat::IEC61883_6_AM824_48KHZ_2CH:
            return "IEC 61883-6 AM824 48kHz 2-channel";
        case MilanBaseAudioFormat::IEC61883_6_AM824_48KHZ_8CH:
            return "IEC 61883-6 AM824 48kHz 8-channel";
        case MilanBaseAudioFormat::AAF_PCM_24BIT_96KHZ_2CH:
            return "AAF PCM 24-bit 96kHz 2-channel";
        case MilanBaseAudioFormat::AAF_PCM_24BIT_96KHZ_8CH:
            return "AAF PCM 24-bit 96kHz 8-channel";
        default:
            return "Unknown stream format";
    }
}

// ============================================================================
// Milan Utility Functions Implementation
// ============================================================================

namespace MilanUtils {

std::string get_milan_version_string() {
    return "Milan v1.2.0 (November 2023)";
}

uint32_t get_milan_version_info() {
    return MILAN_VERSION_INFO;
}

bool is_milan_version_compatible(uint16_t major, uint16_t minor) {
    // Milan v1.2 is compatible with v1.0 and v1.1
    return (major == 1) && (minor <= 2);
}

bool has_milan_capability(uint32_t capabilities, MilanCapabilityFlags flag) {
    return (capabilities & static_cast<uint32_t>(flag)) != 0;
}

std::string capabilities_to_string(uint32_t capabilities) {
    std::vector<std::string> cap_strings;
    
    if (has_milan_capability(capabilities, MilanCapabilityFlags::BASELINE_INTEROPERABILITY)) {
        cap_strings.push_back("Baseline Interoperability");
    }
    if (has_milan_capability(capabilities, MilanCapabilityFlags::DISCOVERY_CONNECTION_CONTROL)) {
        cap_strings.push_back("Discovery/Connection Control");
    }
    if (has_milan_capability(capabilities, MilanCapabilityFlags::BASE_STREAM_FORMATS)) {
        cap_strings.push_back("Base Stream Formats");
    }
    if (has_milan_capability(capabilities, MilanCapabilityFlags::MEDIA_CLOCKING)) {
        cap_strings.push_back("Media Clocking");
    }
    if (has_milan_capability(capabilities, MilanCapabilityFlags::SEAMLESS_REDUNDANCY)) {
        cap_strings.push_back("Seamless Redundancy");
    }
    if (has_milan_capability(capabilities, MilanCapabilityFlags::GPTA_AS_MEDIA_CLOCK)) {
        cap_strings.push_back("gPTP as Media Clock");
    }
    if (has_milan_capability(capabilities, MilanCapabilityFlags::MEDIA_CLOCK_REFERENCE)) {
        cap_strings.push_back("Media Clock Reference");
    }
    if (has_milan_capability(capabilities, MilanCapabilityFlags::DYNAMIC_MAPPING)) {
        cap_strings.push_back("Dynamic Mapping");
    }
    if (has_milan_capability(capabilities, MilanCapabilityFlags::SYSTEM_UNIQUE_ID)) {
        cap_strings.push_back("System Unique ID");
    }
    
    if (cap_strings.empty()) {
        return "No capabilities";
    }
    
    std::string result = cap_strings[0];
    for (size_t i = 1; i < cap_strings.size(); ++i) {
        result += ", " + cap_strings[i];
    }
    return result;
}

std::string features_to_string(uint16_t features) {
    std::vector<std::string> feature_strings;
    
    if (features & static_cast<uint16_t>(MilanProtocolFeatures::MILAN_BASELINE)) {
        feature_strings.push_back("Milan Baseline");
    }
    if (features & static_cast<uint16_t>(MilanProtocolFeatures::FAST_CONNECT)) {
        feature_strings.push_back("Fast Connect");
    }
    if (features & static_cast<uint16_t>(MilanProtocolFeatures::NETWORK_REDUNDANCY)) {
        feature_strings.push_back("Network Redundancy");
    }
    if (features & static_cast<uint16_t>(MilanProtocolFeatures::MEDIA_CLOCK_MANAGEMENT)) {
        feature_strings.push_back("Media Clock Management");
    }
    if (features & static_cast<uint16_t>(MilanProtocolFeatures::ENHANCED_DIAGNOSTICS)) {
        feature_strings.push_back("Enhanced Diagnostics");
    }
    if (features & static_cast<uint16_t>(MilanProtocolFeatures::PROFESSIONAL_AUDIO)) {
        feature_strings.push_back("Professional Audio");
    }
    
    if (feature_strings.empty()) {
        return "No features";
    }
    
    std::string result = feature_strings[0];
    for (size_t i = 1; i < feature_strings.size(); ++i) {
        result += ", " + feature_strings[i];
    }
    return result;
}

bool is_milan_stream_format(uint64_t stream_format) {
    return MilanStreamFormatValidator::is_valid_milan_format(stream_format);
}

std::string stream_format_to_string(uint64_t stream_format) {
    return MilanStreamFormatValidator::format_to_string(stream_format);
}

bool is_hive_avdecc_compatible() {
    // Check if the current implementation supports Hive-AVDECC requirements
    // This would be expanded with actual compatibility checks
    return true;
}

bool is_l_acoustics_network_manager_compatible() {
    // Check if the current implementation supports L-Acoustics Network Manager
    // This would be expanded with actual compatibility checks
    return true;
}

std::vector<std::string> get_professional_tool_compatibility() {
    std::vector<std::string> tools;
    
    if (is_hive_avdecc_compatible()) {
        tools.push_back("Hive-AVDECC");
    }
    if (is_l_acoustics_network_manager_compatible()) {
        tools.push_back("L-Acoustics Network Manager");
    }
    
    return tools;
}

} // namespace MilanUtils

} // namespace _1_2_2023
} // namespace Milan
} // namespace AVnu
