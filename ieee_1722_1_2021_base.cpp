#include "ieee_1722_1_2021_clean.h"
#include <cstring>

namespace IEEE {
namespace _1722_1 {
namespace _2021 {

namespace ADPDU {

// Static constants for AVDECC protocol
const uint8_t ADP_SUBTYPE = 0x7A;
const uint8_t ADP_VERSION = 0x00;

ATDECCDiscoveryProtocolPDU::ATDECCDiscoveryProtocolPDU() {
    memset(raw_octets, 0, sizeof(raw_octets));
    
    // Initialize common header fields
    subtype = ADP_SUBTYPE;
    h = false;
    version = ADP_VERSION;
    message_type = ADP_ENTITY_AVAILABLE;
    valid_time = 0;
    control_data_length = 56; // As per standard
    
    // Initialize entity fields to default values
    entity_id = 0;
    entity_model_id = 0;
    entity_capabilities = EntityCapabilities::None;
    talker_stream_sources = 0;
    talker_capabilities = TalkerCapabilities::None;
    listener_stream_sinks = 0;
    listener_capabilities = ListenerCapabilities::None;
    controller_capabilities = ControllerCapabilities::None;
    available_index = 0;
    gptp_grandmaster_id = 0;
    gptp_domain_number = 0;
    reserved0 = 0;
    current_configuration_index = 0;
    identify_control_index = 0;
    interface_index = 0;
    association_id = 0;
    reserved1 = 0;
}

ATDECCDiscoveryProtocolPDU::ATDECCDiscoveryProtocolPDU(const uint8_t* data, size_t length) {
    if (length >= sizeof(raw_octets)) {
        memcpy(raw_octets, data, sizeof(raw_octets));
        deserialize();
    } else {
        memset(raw_octets, 0, sizeof(raw_octets));
        // Initialize with default values if data is insufficient
        *this = ATDECCDiscoveryProtocolPDU();
    }
}

void ATDECCDiscoveryProtocolPDU::serialize() {
    uint8_t* ptr = raw_octets;
    
    // IEEE 1722-2016 Common Header
    *ptr++ = subtype;
    
    // Pack h(1), version(3), message_type(4) bits into one byte
    uint8_t header_byte = (h ? 0x80 : 0x00) | 
                         ((version & 0x07) << 4) | 
                         (static_cast<uint8_t>(message_type) & 0x0F);
    *ptr++ = header_byte;
    
    // Pack valid_time(5), control_data_length(11) into 2 bytes
    uint16_t length_field = ((valid_time & 0x1F) << 11) | (control_data_length & 0x07FF);
    *reinterpret_cast<uint16_t*>(ptr) = htons(length_field);
    ptr += 2;
    
    // Entity ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(entity_id);
    ptr += 8;
    
    // Entity Model ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(entity_model_id);
    ptr += 8;
    
    // Entity Capabilities (4 bytes)
    *reinterpret_cast<uint32_t*>(ptr) = htonl(static_cast<uint32_t>(entity_capabilities));
    ptr += 4;
    
    // Talker Stream Sources (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(talker_stream_sources);
    ptr += 2;
    
    // Talker Capabilities (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(static_cast<uint16_t>(talker_capabilities));
    ptr += 2;
    
    // Listener Stream Sinks (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(listener_stream_sinks);
    ptr += 2;
    
    // Listener Capabilities (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(static_cast<uint16_t>(listener_capabilities));
    ptr += 2;
    
    // Controller Capabilities (4 bytes)
    *reinterpret_cast<uint32_t*>(ptr) = htonl(static_cast<uint32_t>(controller_capabilities));
    ptr += 4;
    
    // Available Index (4 bytes)
    *reinterpret_cast<uint32_t*>(ptr) = htonl(available_index);
    ptr += 4;
    
    // gPTP Grandmaster ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(gptp_grandmaster_id);
    ptr += 8;
    
    // gPTP Domain Number (1 byte)
    *ptr++ = gptp_domain_number;
    
    // Reserved0 (1 byte)
    *ptr++ = reserved0;
    
    // Current Configuration Index (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(current_configuration_index);
    ptr += 2;
    
    // Identify Control Index (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(identify_control_index);
    ptr += 2;
    
    // Interface Index (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(interface_index);
    ptr += 2;
    
    // Association ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(association_id);
    ptr += 8;
    
    // Reserved1 (4 bytes)
    *reinterpret_cast<uint32_t*>(ptr) = htonl(reserved1);
}

void ATDECCDiscoveryProtocolPDU::deserialize() {
    const uint8_t* ptr = raw_octets;
    
    // IEEE 1722-2016 Common Header
    subtype = *ptr++;
    
    // Unpack h(1), version(3), message_type(4) bits
    uint8_t header_byte = *ptr++;
    h = (header_byte & 0x80) != 0;
    version = (header_byte >> 4) & 0x07;
    message_type = static_cast<ADP_Message_type>(header_byte & 0x0F);
    
    // Unpack valid_time(5), control_data_length(11)
    uint16_t length_field = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    valid_time = (length_field >> 11) & 0x1F;
    control_data_length = length_field & 0x07FF;
    
    // Entity ID (8 bytes)
    entity_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    // Entity Model ID (8 bytes)
    entity_model_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    // Entity Capabilities (4 bytes)
    entity_capabilities = static_cast<EntityCapabilities>(ntohl(*reinterpret_cast<const uint32_t*>(ptr)));
    ptr += 4;
    
    // Talker Stream Sources (2 bytes)
    talker_stream_sources = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Talker Capabilities (2 bytes)
    talker_capabilities = static_cast<TalkerCapabilities>(ntohs(*reinterpret_cast<const uint16_t*>(ptr)));
    ptr += 2;
    
    // Listener Stream Sinks (2 bytes)
    listener_stream_sinks = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Listener Capabilities (2 bytes)
    listener_capabilities = static_cast<ListenerCapabilities>(ntohs(*reinterpret_cast<const uint16_t*>(ptr)));
    ptr += 2;
    
    // Controller Capabilities (4 bytes)
    controller_capabilities = static_cast<ControllerCapabilities>(ntohl(*reinterpret_cast<const uint32_t*>(ptr)));
    ptr += 4;
    
    // Available Index (4 bytes)
    available_index = ntohl(*reinterpret_cast<const uint32_t*>(ptr));
    ptr += 4;
    
    // gPTP Grandmaster ID (8 bytes)
    gptp_grandmaster_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    // gPTP Domain Number (1 byte)
    gptp_domain_number = *ptr++;
    
    // Reserved0 (1 byte)
    reserved0 = *ptr++;
    
    // Current Configuration Index (2 bytes)
    current_configuration_index = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Identify Control Index (2 bytes)
    identify_control_index = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Interface Index (2 bytes)
    interface_index = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Association ID (8 bytes)
    association_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    // Reserved1 (4 bytes)
    reserved1 = ntohl(*reinterpret_cast<const uint32_t*>(ptr));
}

bool ATDECCDiscoveryProtocolPDU::is_valid() const {
    return (subtype == ADP_SUBTYPE) && 
           (version == ADP_VERSION) && 
           (control_data_length == 56);
}

size_t ATDECCDiscoveryProtocolPDU::get_size() const {
    return sizeof(raw_octets);
}

const uint8_t* ATDECCDiscoveryProtocolPDU::get_raw_octets() const {
    return raw_octets;
}

uint8_t* ATDECCDiscoveryProtocolPDU::get_raw_octets() {
    return raw_octets;
}

void ATDECCDiscoveryProtocolPDU::set_entity_id(uint64_t id) {
    entity_id = id;
}

void ATDECCDiscoveryProtocolPDU::set_entity_model_id(uint64_t model_id) {
    entity_model_id = model_id;
}

void ATDECCDiscoveryProtocolPDU::set_entity_capabilities(EntityCapabilities capabilities) {
    entity_capabilities = capabilities;
}

void ATDECCDiscoveryProtocolPDU::set_talker_info(uint16_t stream_sources, TalkerCapabilities capabilities) {
    talker_stream_sources = stream_sources;
    talker_capabilities = capabilities;
}

void ATDECCDiscoveryProtocolPDU::set_listener_info(uint16_t stream_sinks, ListenerCapabilities capabilities) {
    listener_stream_sinks = stream_sinks;
    listener_capabilities = capabilities;
}

void ATDECCDiscoveryProtocolPDU::set_controller_capabilities(ControllerCapabilities capabilities) {
    controller_capabilities = capabilities;
}

void ATDECCDiscoveryProtocolPDU::set_message_type(ADP_Message_type type) {
    message_type = type;
}

void ATDECCDiscoveryProtocolPDU::set_valid_time(uint16_t time) {
    valid_time = time & 0x1F; // Ensure only 5 bits are used
}

} // namespace ADPDU

namespace AEM {

// Utility functions for descriptor validation
bool is_valid_descriptor_type(uint16_t type) {
    return (type >= DESCRIPTOR_ENTITY && type <= DESCRIPTOR_CONTROL_BLOCK) ||
           (type >= 0x8000); // Vendor-specific descriptors
}

bool validate_entity_descriptor(const EntityDescriptor& descriptor) {
    // Basic validation - entity_id should not be zero
    return descriptor.entity_id != 0;
}

// Helper functions for string operations
void safe_string_copy(char* dest, const char* src, size_t dest_size) {
    if (dest && src && dest_size > 0) {
        strncpy(dest, src, dest_size - 1);
        dest[dest_size - 1] = '\0'; // Ensure null termination
    }
}

void clear_string_field(char* str, size_t size) {
    if (str && size > 0) {
        memset(str, 0, size);
    }
}

} // namespace AEM

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
