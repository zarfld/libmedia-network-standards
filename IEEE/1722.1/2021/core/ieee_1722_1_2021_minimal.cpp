#include "ieee_1722_1_2021_minimal.h"

namespace IEEE {
namespace _1722_1 {
namespace _2021 {

// AVDECCEntity Implementation
AVDECCEntity::AVDECCEntity(EntityID id, EntityID model_id) 
    : entity_id(id)
    , entity_model_id(model_id)
    , entity_capabilities(0)
    , talker_stream_sources(0)
    , talker_capabilities(0)
    , listener_stream_sinks(0)
    , listener_capabilities(0)
    , controller_capabilities(0)
    , available_index(0)
    , association_id(0)
{
    // Initialize with valid default values
    if (entity_id == 0) {
        // Generate a basic entity ID (in production, this should be MAC-based)
        entity_id = 0x001122FFFE334455ULL;
    }
    
    if (entity_model_id == 0) {
        // Default entity model ID
        entity_model_id = 0x001122FFFE334456ULL;
    }
    
    // Set basic capabilities
    entity_capabilities = 0x00000001; // Basic AEM support
    available_index = 1;
}

bool AVDECCEntity::supports_aem() const {
    return (entity_capabilities & 0x00000001) != 0;
}

bool AVDECCEntity::has_talker_capabilities() const {
    return (talker_stream_sources > 0) && (talker_capabilities > 0);
}

bool AVDECCEntity::has_listener_capabilities() const {
    return (listener_stream_sinks > 0) && (listener_capabilities > 0);
}

void AVDECCEntity::set_entity_name(const char* name) {
    // In a full implementation, this would store the name
    // For now, just validate it's not null
    if (name != nullptr) {
        // Name accepted
    }
}

void AVDECCEntity::set_firmware_version(const char* version) {
    // In a full implementation, this would store the version
    if (version != nullptr) {
        // Version accepted
    }
}

void AVDECCEntity::set_group_name(const char* name) {
    // In a full implementation, this would store the group name
    if (name != nullptr) {
        // Group name accepted
    }
}

void AVDECCEntity::set_serial_number(const char* serial) {
    // In a full implementation, this would store the serial number
    if (serial != nullptr) {
        // Serial number accepted
    }
}

// AEMCommand Implementation
AEMCommand::AEMCommand(CommandType cmd_type) 
    : command_type(cmd_type)
    , target_entity_id(0)
    , sequence_id(0)
    , unsolicited(false)
    , controller_request(true)
{
}

AEMCommand::AEMCommand(CommandType cmd_type, EntityID target_id, uint16_t seq_id)
    : command_type(cmd_type)
    , target_entity_id(target_id)
    , sequence_id(seq_id)
    , unsolicited(false)
    , controller_request(true)
{
}

size_t AEMCommand::serialize(uint8_t* buffer, size_t buffer_size) const {
    if (buffer_size < 12) { // Minimum AEM command size
        return 0;
    }
    
    uint8_t* ptr = buffer;
    
    // Command Type (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(static_cast<uint16_t>(command_type));
    ptr += 2;
    
    // Target Entity ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(target_entity_id);
    ptr += 8;
    
    // Sequence ID (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(sequence_id);
    ptr += 2;
    
    return ptr - buffer;
}

bool AEMCommand::deserialize(const uint8_t* buffer, size_t buffer_size) {
    if (buffer_size < 12) {
        return false;
    }
    
    const uint8_t* ptr = buffer;
    
    // Command Type (2 bytes)
    command_type = static_cast<CommandType>(ntohs(*reinterpret_cast<const uint16_t*>(ptr)));
    ptr += 2;
    
    // Target Entity ID (8 bytes)
    target_entity_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    // Sequence ID (2 bytes)
    sequence_id = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    return true;
}

// EntityDescriptor Implementation
EntityDescriptor::EntityDescriptor()
    : descriptor_type(ENTITY)
    , descriptor_index(0)
    , entity_id(0)
    , entity_model_id(0)
{
}

size_t EntityDescriptor::serialize(uint8_t* buffer, size_t buffer_size) const {
    if (buffer_size < 20) { // Minimum entity descriptor size
        return 0;
    }
    
    uint8_t* ptr = buffer;
    
    // Descriptor Type (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(static_cast<uint16_t>(descriptor_type));
    ptr += 2;
    
    // Descriptor Index (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(descriptor_index);
    ptr += 2;
    
    // Entity ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(entity_id);
    ptr += 8;
    
    // Entity Model ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(entity_model_id);
    ptr += 8;
    
    return ptr - buffer;
}

bool EntityDescriptor::deserialize(const uint8_t* buffer, size_t buffer_size) {
    if (buffer_size < 20) {
        return false;
    }
    
    const uint8_t* ptr = buffer;
    
    // Descriptor Type (2 bytes)
    descriptor_type = static_cast<DescriptorType>(ntohs(*reinterpret_cast<const uint16_t*>(ptr)));
    ptr += 2;
    
    // Descriptor Index (2 bytes)
    descriptor_index = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Entity ID (8 bytes)
    entity_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    // Entity Model ID (8 bytes)
    entity_model_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    return true;
}

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
