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

namespace ACMP {

// Static constants for ACMP protocol
const uint8_t ACMP_SUBTYPE = 0x7B;

ATDECCConnectionManagementProtocolPDU::ATDECCConnectionManagementProtocolPDU() {
    memset(raw_octets, 0, sizeof(raw_octets));
    
    // Initialize common header fields
    subtype = ACMP_SUBTYPE;
    h = false;
    version = 0;
    message_type = ACMP_Message_type::CONNECT_TX_COMMAND;
    status = ACMP_Status::SUCCESS;
    control_data_length = 44; // As per IEEE 1722.1-2021
    
    // Initialize ACMP fields to default values
    stream_id = 0;
    controller_entity_id = 0;
    talker_entity_id = 0;
    listener_entity_id = 0;
    talker_unique_id = 0;
    listener_unique_id = 0;
    memset(stream_dest_mac, 0, 6);
    connection_count = 0;
    sequence_id = 0;
    flags = 0;
    stream_vlan_id = 0;
}

ATDECCConnectionManagementProtocolPDU::ATDECCConnectionManagementProtocolPDU(const uint8_t* data, size_t length) {
    if (length >= sizeof(raw_octets)) {
        memcpy(raw_octets, data, sizeof(raw_octets));
        deserialize();
    } else {
        memset(raw_octets, 0, sizeof(raw_octets));
        // Initialize with default values if data is insufficient
        *this = ATDECCConnectionManagementProtocolPDU();
    }
}

void ATDECCConnectionManagementProtocolPDU::serialize() {
    uint8_t* ptr = raw_octets;
    
    // IEEE 1722-2016 Common Header
    *ptr++ = subtype;
    
    // Pack h(1), version(3), message_type(4) bits into one byte
    uint8_t header_byte = (h ? 0x80 : 0x00) | 
                         ((version & 0x07) << 4) | 
                         (static_cast<uint8_t>(message_type) & 0x0F);
    *ptr++ = header_byte;
    
    // Status and Control Data Length
    *ptr++ = static_cast<uint8_t>(status);
    *ptr++ = (control_data_length >> 8) & 0xFF;
    *ptr++ = control_data_length & 0xFF;
    
    // Stream ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(stream_id);
    ptr += 8;
    
    // Controller Entity ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(controller_entity_id);
    ptr += 8;
    
    // Talker Entity ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(talker_entity_id);
    ptr += 8;
    
    // Listener Entity ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(listener_entity_id);
    ptr += 8;
    
    // Talker Unique ID (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(talker_unique_id);
    ptr += 2;
    
    // Listener Unique ID (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(listener_unique_id);
    ptr += 2;
    
    // Stream Destination MAC (6 bytes)
    memcpy(ptr, stream_dest_mac, 6);
    ptr += 6;
    
    // Connection Count (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(connection_count);
    ptr += 2;
    
    // Sequence ID (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(sequence_id);
    ptr += 2;
    
    // Flags (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(flags);
    ptr += 2;
    
    // Stream VLAN ID (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(stream_vlan_id);
    ptr += 2;
}

void ATDECCConnectionManagementProtocolPDU::deserialize() {
    const uint8_t* ptr = raw_octets;
    
    // IEEE 1722-2016 Common Header
    subtype = *ptr++;
    
    // Unpack h(1), version(3), message_type(4) bits
    uint8_t header_byte = *ptr++;
    h = (header_byte & 0x80) != 0;
    version = (header_byte >> 4) & 0x07;
    message_type = static_cast<ACMP_Message_type>(header_byte & 0x0F);
    
    // Status and Control Data Length
    status = static_cast<ACMP_Status>(*ptr++);
    uint16_t length_high = *ptr++;
    uint16_t length_low = *ptr++;
    control_data_length = (length_high << 8) | length_low;
    
    // Stream ID (8 bytes)
    stream_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    // Controller Entity ID (8 bytes)
    controller_entity_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    // Talker Entity ID (8 bytes)
    talker_entity_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    // Listener Entity ID (8 bytes)
    listener_entity_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    // Talker Unique ID (2 bytes)
    talker_unique_id = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Listener Unique ID (2 bytes)
    listener_unique_id = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Stream Destination MAC (6 bytes)
    memcpy(stream_dest_mac, ptr, 6);
    ptr += 6;
    
    // Connection Count (2 bytes)
    connection_count = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Sequence ID (2 bytes)
    sequence_id = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Flags (2 bytes)
    flags = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Stream VLAN ID (2 bytes)
    stream_vlan_id = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
}

bool ATDECCConnectionManagementProtocolPDU::is_command() const {
    uint8_t msg_type = static_cast<uint8_t>(message_type);
    return (msg_type % 2) == 0; // Even values are commands
}

bool ATDECCConnectionManagementProtocolPDU::is_response() const {
    uint8_t msg_type = static_cast<uint8_t>(message_type);
    return (msg_type % 2) == 1; // Odd values are responses
}

void ATDECCConnectionManagementProtocolPDU::create_response(ACMP_Status response_status) {
    // Convert command to response by adding 1 to message type
    if (is_command()) {
        uint8_t cmd_val = static_cast<uint8_t>(message_type);
        message_type = static_cast<ACMP_Message_type>(cmd_val + 1);
    }
    status = response_status;
    serialize();
}

} // namespace ACMP

namespace AECP {

// Static constants for AECP protocol
const uint8_t AECP_SUBTYPE = 0x7B;
const uint8_t AECP_VERSION = 0x00;

ATDECCEnumerationControlProtocolPDU::ATDECCEnumerationControlProtocolPDU() {
    memset(raw_octets, 0, sizeof(raw_octets));
    
    // Initialize common header fields
    subtype = AECP_SUBTYPE;
    h = false;
    version = AECP_VERSION;
    message_type = AECP_Message_type::AEM_COMMAND;
    status = AECP_Status::SUCCESS;
    control_data_length = 12; // Default: sequence_id + command_type
    
    // Initialize AECP fields
    target_entity_id = 0;
    controller_entity_id = 0;
    sequence_id = 0;
    
    // Initialize AEM fields
    unsolicited = false;
    command_type = AEM_Command_type::READ_DESCRIPTOR;
    command_specific_length = 0;
    memset(command_specific_data, 0, sizeof(command_specific_data));
}

ATDECCEnumerationControlProtocolPDU::ATDECCEnumerationControlProtocolPDU(const uint8_t* data, size_t length) {
    if (length >= 24 && length <= sizeof(raw_octets)) { // Minimum AECP header is 24 bytes
        memcpy(raw_octets, data, length);
        deserialize();
    } else {
        memset(raw_octets, 0, sizeof(raw_octets));
        // Initialize with default values if data is insufficient
        *this = ATDECCEnumerationControlProtocolPDU();
    }
}

void ATDECCEnumerationControlProtocolPDU::serialize() {
    uint8_t* ptr = raw_octets;
    
    // IEEE 1722-2016 Common Header (12 bytes)
    *ptr++ = subtype;
    
    // Pack h(1), version(3), message_type(4) bits into one byte
    uint8_t header_byte = (h ? 0x80 : 0x00) | 
                         ((version & 0x07) << 4) | 
                         (static_cast<uint8_t>(message_type) & 0x0F);
    *ptr++ = header_byte;
    
    // Status (1 byte)
    *ptr++ = static_cast<uint8_t>(status);
    
    // Control Data Length (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(control_data_length);
    ptr += 2;
    
    // Skip reserved byte
    *ptr++ = 0;
    
    // Target Entity ID (8 bytes) - stored in stream_id field
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(target_entity_id);
    ptr += 8;
    
    // AECP Common Data (10 bytes)
    // Controller Entity ID (8 bytes)
    *reinterpret_cast<uint64_t*>(ptr) = htobe64(controller_entity_id);
    ptr += 8;
    
    // Sequence ID (2 bytes)
    *reinterpret_cast<uint16_t*>(ptr) = htons(sequence_id);
    ptr += 2;
    
    // AEM Specific Data (for AEM messages)
    if (is_aem_message()) {
        // Command Type with unsolicited flag (2 bytes)
        uint16_t cmd_type_field = static_cast<uint16_t>(command_type);
        if (unsolicited) {
            cmd_type_field |= 0x8000; // Set unsolicited bit
        }
        *reinterpret_cast<uint16_t*>(ptr) = htons(cmd_type_field);
        ptr += 2;
        
        // Command-specific data
        if (command_specific_length > 0 && command_specific_length <= sizeof(command_specific_data)) {
            memcpy(ptr, command_specific_data, command_specific_length);
        }
    }
}

void ATDECCEnumerationControlProtocolPDU::deserialize() {
    const uint8_t* ptr = raw_octets;
    
    // IEEE 1722-2016 Common Header
    subtype = *ptr++;
    
    uint8_t header_byte = *ptr++;
    h = (header_byte & 0x80) != 0;
    version = (header_byte >> 4) & 0x07;
    message_type = static_cast<AECP_Message_type>(header_byte & 0x0F);
    
    status = static_cast<AECP_Status>(*ptr++);
    
    control_data_length = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // Skip reserved byte
    ptr++;
    
    // Target Entity ID (8 bytes)
    target_entity_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    // AECP Common Data
    controller_entity_id = be64toh(*reinterpret_cast<const uint64_t*>(ptr));
    ptr += 8;
    
    sequence_id = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
    ptr += 2;
    
    // AEM Specific Data (for AEM messages)
    if (is_aem_message() && control_data_length >= 12) {
        uint16_t cmd_type_field = ntohs(*reinterpret_cast<const uint16_t*>(ptr));
        ptr += 2;
        
        unsolicited = (cmd_type_field & 0x8000) != 0;
        command_type = static_cast<AEM_Command_type>(cmd_type_field & 0x7FFF);
        
        // Calculate command-specific data length
        command_specific_length = control_data_length - 12; // Subtract AECP header + command_type
        if (command_specific_length > 0 && command_specific_length <= sizeof(command_specific_data)) {
            memcpy(command_specific_data, ptr, command_specific_length);
        }
    }
}

size_t ATDECCEnumerationControlProtocolPDU::get_size() const {
    return 22 + control_data_length; // Common header (12) + AECP header (10) + payload
}

bool ATDECCEnumerationControlProtocolPDU::is_command() const {
    uint8_t msg_type = static_cast<uint8_t>(message_type);
    return (msg_type % 2) == 0; // Even values are commands
}

bool ATDECCEnumerationControlProtocolPDU::is_response() const {
    uint8_t msg_type = static_cast<uint8_t>(message_type);
    return (msg_type % 2) == 1; // Odd values are responses
}

bool ATDECCEnumerationControlProtocolPDU::is_aem_message() const {
    return message_type == AECP_Message_type::AEM_COMMAND || 
           message_type == AECP_Message_type::AEM_RESPONSE;
}

void ATDECCEnumerationControlProtocolPDU::create_response(AECP_Status response_status) {
    // Convert command to response by adding 1 to message type
    if (is_command()) {
        uint8_t cmd_val = static_cast<uint8_t>(message_type);
        message_type = static_cast<AECP_Message_type>(cmd_val + 1);
    }
    status = response_status;
    serialize();
}

// AEM-specific helper methods for READ_DESCRIPTOR command/response
void ATDECCEnumerationControlProtocolPDU::set_read_descriptor_command(uint16_t config_index, uint16_t desc_type, uint16_t desc_index) {
    message_type = AECP_Message_type::AEM_COMMAND;
    command_type = AEM_Command_type::READ_DESCRIPTOR;
    command_specific_length = 8; // 2 + 2 + 2 + 2 (config_index + reserved + desc_type + desc_index)
    
    uint8_t* cmd_data = command_specific_data;
    *reinterpret_cast<uint16_t*>(cmd_data) = htons(config_index);
    cmd_data += 2;
    *reinterpret_cast<uint16_t*>(cmd_data) = 0; // Reserved
    cmd_data += 2;
    *reinterpret_cast<uint16_t*>(cmd_data) = htons(desc_type);
    cmd_data += 2;
    *reinterpret_cast<uint16_t*>(cmd_data) = htons(desc_index);
    cmd_data += 2;
    
    control_data_length = 12 + command_specific_length; // AECP header + AEM header + command data
    serialize();
}

void ATDECCEnumerationControlProtocolPDU::set_read_descriptor_response(uint16_t config_index, uint16_t desc_type, uint16_t desc_index,
                                                                       const uint8_t* descriptor_data, size_t descriptor_length) {
    message_type = AECP_Message_type::AEM_RESPONSE;
    command_type = AEM_Command_type::READ_DESCRIPTOR;
    
    // Response includes config_index + reserved + descriptor_data
    command_specific_length = 4 + static_cast<uint16_t>(descriptor_length); // 2 + 2 + descriptor_length
    
    if (command_specific_length <= sizeof(command_specific_data)) {
        uint8_t* cmd_data = command_specific_data;
        *reinterpret_cast<uint16_t*>(cmd_data) = htons(config_index);
        cmd_data += 2;
        *reinterpret_cast<uint16_t*>(cmd_data) = 0; // Reserved
        cmd_data += 2;
        
        if (descriptor_data && descriptor_length > 0) {
            memcpy(cmd_data, descriptor_data, descriptor_length);
        }
        
        control_data_length = 12 + command_specific_length;
    }
    serialize();
}

void ATDECCEnumerationControlProtocolPDU::get_read_descriptor_command(uint16_t& config_index, uint16_t& desc_type, uint16_t& desc_index) const {
    if (command_type == AEM_Command_type::READ_DESCRIPTOR && command_specific_length >= 8) {
        const uint8_t* cmd_data = command_specific_data;
        config_index = ntohs(*reinterpret_cast<const uint16_t*>(cmd_data));
        cmd_data += 2;
        // Skip reserved (2 bytes)
        cmd_data += 2;
        desc_type = ntohs(*reinterpret_cast<const uint16_t*>(cmd_data));
        cmd_data += 2;
        desc_index = ntohs(*reinterpret_cast<const uint16_t*>(cmd_data));
    }
}

void ATDECCEnumerationControlProtocolPDU::get_read_descriptor_response(uint16_t& config_index, uint16_t& desc_type, uint16_t& desc_index,
                                                                       const uint8_t*& descriptor_data, size_t& descriptor_length) const {
    if (command_type == AEM_Command_type::READ_DESCRIPTOR && command_specific_length >= 4) {
        const uint8_t* cmd_data = command_specific_data;
        config_index = ntohs(*reinterpret_cast<const uint16_t*>(cmd_data));
        cmd_data += 2;
        // Skip reserved (2 bytes)
        cmd_data += 2;
        
        descriptor_data = cmd_data;
        descriptor_length = command_specific_length - 4; // Subtract config_index + reserved
        
        // Extract descriptor type and index from descriptor data (first 4 bytes)
        if (descriptor_length >= 4) {
            desc_type = ntohs(*reinterpret_cast<const uint16_t*>(descriptor_data));
            desc_index = ntohs(*reinterpret_cast<const uint16_t*>(descriptor_data + 2));
        }
    }
}

} // namespace AECP

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
