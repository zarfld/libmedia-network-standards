// IEEE 1722.1-2021 AVDECC Implementation
// Implementation of AVDECC protocol classes defined in 1722_1-2021.h

#include "ieee_1722_1_2021_base.h"
#include <cstring>
#include <random>

namespace IEEE {
namespace _1722_1 {
namespace _2021 {

// AVDECC Entity Implementation
AVDECCEntity::AVDECCEntity() {
    memset(&entity_id, 0, sizeof(entity_id));
    memset(&entity_model_id, 0, sizeof(entity_model_id));
    
    // Generate random entity ID (in production, use proper unique ID)
    std::random_device rd;
    std::mt19937_64 gen(rd());
    uint64_t id = gen();
    memcpy(entity_id, &id, sizeof(entity_id));
    
    entity_capabilities = 0x0001;  // AEM supported
    talker_stream_sources = 1;
    talker_capabilities = 0x4001;  // Implemented, Audio supported
    listener_stream_sinks = 1;
    listener_capabilities = 0x4001;  // Implemented, Audio supported
    controller_capabilities = 0x0001;  // Implemented
    available_index = 0;
    association_id = 0;
}

void AVDECCEntity::set_entity_name(const std::string& name) {
    entity_name = name;
}

void AVDECCEntity::set_firmware_version(const std::string& version) {
    firmware_version = version;
}

void AVDECCEntity::set_group_name(const std::string& name) {
    group_name = name;
}

void AVDECCEntity::set_serial_number(const std::string& serial) {
    serial_number = serial;
}

uint32_t AVDECCEntity::get_available_index() const {
    return available_index;
}

void AVDECCEntity::increment_available_index() {
    available_index++;
}

bool AVDECCEntity::supports_aem() const {
    return (entity_capabilities & 0x0001) != 0;
}

bool AVDECCEntity::has_talker_capabilities() const {
    return talker_stream_sources > 0 && (talker_capabilities & 0x0001) != 0;
}

bool AVDECCEntity::has_listener_capabilities() const {
    return listener_stream_sinks > 0 && (listener_capabilities & 0x0001) != 0;
}

// AEM Command Implementation
AEMCommand::AEMCommand() {
    command_type = CommandType::READ_DESCRIPTOR;
    memset(&target_entity_id, 0, sizeof(target_entity_id));
    sequence_id = 0;
    unsolicited = false;
    controller_request = true;
}

AEMCommand::AEMCommand(CommandType cmd_type) : AEMCommand() {
    command_type = cmd_type;
}

bool AEMCommand::serialize(uint8_t* buffer, size_t& length) const {
    if (!buffer || length < get_min_size()) return false;
    
    // Serialize basic AEM command structure
    buffer[0] = static_cast<uint8_t>(command_type >> 8);
    buffer[1] = static_cast<uint8_t>(command_type & 0xFF);
    buffer[2] = (unsolicited ? 0x80 : 0) | (controller_request ? 0x40 : 0);
    buffer[3] = 0; // Reserved
    
    // Target entity ID
    memcpy(buffer + 4, target_entity_id, 8);
    
    // Sequence ID
    buffer[12] = (sequence_id >> 8) & 0xFF;
    buffer[13] = sequence_id & 0xFF;
    
    length = get_min_size();
    return true;
}

bool AEMCommand::deserialize(const uint8_t* data, size_t length) {
    if (!data || length < get_min_size()) return false;
    
    command_type = static_cast<CommandType>((data[0] << 8) | data[1]);
    unsolicited = (data[2] & 0x80) != 0;
    controller_request = (data[2] & 0x40) != 0;
    
    memcpy(target_entity_id, data + 4, 8);
    sequence_id = (data[12] << 8) | data[13];
    
    return true;
}

// EntityDescriptor Implementation
EntityDescriptor::EntityDescriptor() {
    descriptor_type = DescriptorType::ENTITY;
    descriptor_index = 0;
    memset(&entity_id, 0, sizeof(entity_id));
    memset(&entity_model_id, 0, sizeof(entity_model_id));
    entity_capabilities = 0x0001;  // AEM supported
    talker_stream_sources = 1;
    talker_capabilities = 0x4001;
    listener_stream_sinks = 1;
    listener_capabilities = 0x4001;
    controller_capabilities = 0x0001;
    available_index = 0;
    association_id = 0;
    
    // Default strings
    entity_name = "OpenAvnu Entity";
    firmware_version = "1.0.0";
    group_name = "OpenAvnu";
    serial_number = "000001";
    
    // Calculate AEM checksum (placeholder - real implementation needs CRC32)
    aem_checksum = calculate_aem_checksum();
}

uint32_t EntityDescriptor::calculate_aem_checksum() const {
    // Simplified checksum for now - in production use proper CRC32
    return 0x12345678;
}

bool EntityDescriptor::serialize(uint8_t* buffer, size_t& length) const {
    if (!buffer || length < get_min_size()) return false;
    
    size_t offset = 0;
    
    // Descriptor type and index
    buffer[offset++] = static_cast<uint8_t>(descriptor_type >> 8);
    buffer[offset++] = static_cast<uint8_t>(descriptor_type & 0xFF);
    buffer[offset++] = (descriptor_index >> 8) & 0xFF;
    buffer[offset++] = descriptor_index & 0xFF;
    
    // Entity ID
    memcpy(buffer + offset, entity_id, 8);
    offset += 8;
    
    // Entity Model ID  
    memcpy(buffer + offset, entity_model_id, 8);
    offset += 8;
    
    // Capabilities and counts
    buffer[offset++] = (entity_capabilities >> 24) & 0xFF;
    buffer[offset++] = (entity_capabilities >> 16) & 0xFF;
    buffer[offset++] = (entity_capabilities >> 8) & 0xFF;
    buffer[offset++] = entity_capabilities & 0xFF;
    
    buffer[offset++] = (talker_stream_sources >> 8) & 0xFF;
    buffer[offset++] = talker_stream_sources & 0xFF;
    
    buffer[offset++] = (talker_capabilities >> 8) & 0xFF;
    buffer[offset++] = talker_capabilities & 0xFF;
    
    buffer[offset++] = (listener_stream_sinks >> 8) & 0xFF;
    buffer[offset++] = listener_stream_sinks & 0xFF;
    
    buffer[offset++] = (listener_capabilities >> 8) & 0xFF;
    buffer[offset++] = listener_capabilities & 0xFF;
    
    buffer[offset++] = (controller_capabilities >> 8) & 0xFF;
    buffer[offset++] = controller_capabilities & 0xFF;
    
    // Available index
    buffer[offset++] = (available_index >> 24) & 0xFF;
    buffer[offset++] = (available_index >> 16) & 0xFF;
    buffer[offset++] = (available_index >> 8) & 0xFF;
    buffer[offset++] = available_index & 0xFF;
    
    // Association ID
    memcpy(buffer + offset, &association_id, 8);
    offset += 8;
    
    // Entity name (64 bytes max)
    memset(buffer + offset, 0, 64);
    strncpy(reinterpret_cast<char*>(buffer + offset), entity_name.c_str(), 63);
    offset += 64;
    
    // Firmware version (64 bytes max)
    memset(buffer + offset, 0, 64);
    strncpy(reinterpret_cast<char*>(buffer + offset), firmware_version.c_str(), 63);
    offset += 64;
    
    // Group name (64 bytes max)
    memset(buffer + offset, 0, 64);
    strncpy(reinterpret_cast<char*>(buffer + offset), group_name.c_str(), 63);
    offset += 64;
    
    // Serial number (64 bytes max)
    memset(buffer + offset, 0, 64);
    strncpy(reinterpret_cast<char*>(buffer + offset), serial_number.c_str(), 63);
    offset += 64;
    
    // AEM checksum
    buffer[offset++] = (aem_checksum >> 24) & 0xFF;
    buffer[offset++] = (aem_checksum >> 16) & 0xFF;
    buffer[offset++] = (aem_checksum >> 8) & 0xFF;
    buffer[offset++] = aem_checksum & 0xFF;
    
    length = offset;
    return true;
}

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
