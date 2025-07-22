/**
 * @file ieee_1722_1_2013_complete.cpp
 * @brief IEEE 1722.1-2013 Standard - VOLLSTÄNDIGE EIGENSTÄNDIGE IMPLEMENTATION
 * 
 * Implementation of IEEE Standard for Device Discovery, Connection Management, 
 * and Control Protocol for IEEE 1722™ Based Devices (2013 version).
 * 
 * Diese Implementation ist vollständig eigenständig und funktioniert ohne IEEE 1722.1-2021 Dateien.
 * 
 * @version 1.0.0
 * @date Juli 2025
 * @copyright OpenAvnu Project
 */

#include "ieee_1722_1_2013_complete.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <chrono>

using namespace IEEE::_1722_1::_2013;

// ==============================
// ADP Implementation
// ==============================

namespace IEEE::_1722_1::_2013::ADP {

    AVDECCDiscoveryProtocolPDU::AVDECCDiscoveryProtocolPDU() 
        : message_type(ENTITY_AVAILABLE), valid_time(62), entity_id(0), entity_model_id(0),
          entity_capabilities(EntityCapabilities::AEM_SUPPORTED), 
          talker_stream_sources(0), talker_capabilities(TalkerCapabilities::IMPLEMENTED),
          listener_stream_sinks(0), listener_capabilities(ListenerCapabilities::IMPLEMENTED),
          controller_capabilities(ControllerCapabilities::IMPLEMENTED),
          available_index(0), gptp_grandmaster_id(0), gptp_domain_number(0),
          identify_control_index(0), interface_index(0), association_id(0),
          reserved1(0), is_valid_(false) {
        
        // Clear reserved fields
        memset(reserved0, 0, sizeof(reserved0));
        
        // Allocate space for serialized data (56 bytes for ADPDU)
        raw_octets_.resize(56);
    }

    AVDECCDiscoveryProtocolPDU::AVDECCDiscoveryProtocolPDU(const uint8_t* data, size_t length)
        : AVDECCDiscoveryProtocolPDU() {
        deserialize(data, length);
    }

    void AVDECCDiscoveryProtocolPDU::serialize() {
        if (raw_octets_.size() < 56) {
            raw_octets_.resize(56);
        }

        uint8_t* ptr = raw_octets_.data();
        size_t offset = 0;

        // Serialize fields according to IEEE 1722.1-2013 Table 6.6
        ptr[offset++] = static_cast<uint8_t>(message_type);
        ptr[offset++] = valid_time;
        
        // Reserved (2 bytes)
        ptr[offset++] = 0;
        ptr[offset++] = 0;
        
        // entity_id (8 bytes, network byte order)
        uint64_t net_entity_id = htonll(entity_id);
        memcpy(ptr + offset, &net_entity_id, 8);
        offset += 8;
        
        // entity_model_id (8 bytes, network byte order)
        uint64_t net_entity_model_id = htonll(entity_model_id);
        memcpy(ptr + offset, &net_entity_model_id, 8);
        offset += 8;
        
        // entity_capabilities (4 bytes, network byte order)
        uint32_t net_capabilities = htonl(static_cast<uint32_t>(entity_capabilities));
        memcpy(ptr + offset, &net_capabilities, 4);
        offset += 4;
        
        // talker_stream_sources (2 bytes, network byte order)
        uint16_t net_talker_sources = htons(talker_stream_sources);
        memcpy(ptr + offset, &net_talker_sources, 2);
        offset += 2;
        
        // talker_capabilities (2 bytes, network byte order)
        uint16_t net_talker_caps = htons(static_cast<uint16_t>(talker_capabilities));
        memcpy(ptr + offset, &net_talker_caps, 2);
        offset += 2;
        
        // listener_stream_sinks (2 bytes, network byte order)
        uint16_t net_listener_sinks = htons(listener_stream_sinks);
        memcpy(ptr + offset, &net_listener_sinks, 2);
        offset += 2;
        
        // listener_capabilities (2 bytes, network byte order)
        uint16_t net_listener_caps = htons(static_cast<uint16_t>(listener_capabilities));
        memcpy(ptr + offset, &net_listener_caps, 2);
        offset += 2;
        
        // controller_capabilities (4 bytes, network byte order)
        uint32_t net_controller_caps = htonl(static_cast<uint32_t>(controller_capabilities));
        memcpy(ptr + offset, &net_controller_caps, 4);
        offset += 4;
        
        // available_index (4 bytes, network byte order)
        uint32_t net_available_index = htonl(available_index);
        memcpy(ptr + offset, &net_available_index, 4);
        offset += 4;
        
        // gptp_grandmaster_id (8 bytes, network byte order)
        uint64_t net_gptp_gm = htonll(gptp_grandmaster_id);
        memcpy(ptr + offset, &net_gptp_gm, 8);
        offset += 8;
        
        // gptp_domain_number (1 byte)
        ptr[offset++] = gptp_domain_number;
        
        // reserved0 (3 bytes)
        memcpy(ptr + offset, reserved0, 3);
        offset += 3;
        
        // identify_control_index (2 bytes, network byte order)
        uint16_t net_identify_index = htons(identify_control_index);
        memcpy(ptr + offset, &net_identify_index, 2);
        offset += 2;
        
        // interface_index (2 bytes, network byte order)
        uint16_t net_interface_index = htons(interface_index);
        memcpy(ptr + offset, &net_interface_index, 2);
        offset += 2;
        
        // association_id (8 bytes, network byte order)
        uint64_t net_association_id = htonll(association_id);
        memcpy(ptr + offset, &net_association_id, 8);
        offset += 8;
        
        // reserved1 (4 bytes)
        uint32_t net_reserved1 = htonl(reserved1);
        memcpy(ptr + offset, &net_reserved1, 4);
        offset += 4;

        is_valid_ = true;
    }

    bool AVDECCDiscoveryProtocolPDU::deserialize(const uint8_t* data, size_t length) {
        if (!data || length < 56) {
            is_valid_ = false;
            return false;
        }

        const uint8_t* ptr = data;
        size_t offset = 0;

        // Deserialize fields according to IEEE 1722.1-2013 Table 6.6
        message_type = static_cast<ADPMessageType>(ptr[offset++]);
        valid_time = ptr[offset++];
        
        // Skip reserved (2 bytes)
        offset += 2;
        
        // entity_id (8 bytes, network byte order)
        uint64_t net_entity_id;
        memcpy(&net_entity_id, ptr + offset, 8);
        entity_id = ntohll(net_entity_id);
        offset += 8;
        
        // entity_model_id (8 bytes, network byte order)
        uint64_t net_entity_model_id;
        memcpy(&net_entity_model_id, ptr + offset, 8);
        entity_model_id = ntohll(net_entity_model_id);
        offset += 8;
        
        // entity_capabilities (4 bytes, network byte order)
        uint32_t net_capabilities;
        memcpy(&net_capabilities, ptr + offset, 4);
        entity_capabilities = static_cast<EntityCapabilities>(ntohl(net_capabilities));
        offset += 4;
        
        // talker_stream_sources (2 bytes, network byte order)
        uint16_t net_talker_sources;
        memcpy(&net_talker_sources, ptr + offset, 2);
        talker_stream_sources = ntohs(net_talker_sources);
        offset += 2;
        
        // talker_capabilities (2 bytes, network byte order)
        uint16_t net_talker_caps;
        memcpy(&net_talker_caps, ptr + offset, 2);
        talker_capabilities = static_cast<TalkerCapabilities>(ntohs(net_talker_caps));
        offset += 2;
        
        // listener_stream_sinks (2 bytes, network byte order)
        uint16_t net_listener_sinks;
        memcpy(&net_listener_sinks, ptr + offset, 2);
        listener_stream_sinks = ntohs(net_listener_sinks);
        offset += 2;
        
        // listener_capabilities (2 bytes, network byte order)
        uint16_t net_listener_caps;
        memcpy(&net_listener_caps, ptr + offset, 2);
        listener_capabilities = static_cast<ListenerCapabilities>(ntohs(net_listener_caps));
        offset += 2;
        
        // controller_capabilities (4 bytes, network byte order)
        uint32_t net_controller_caps;
        memcpy(&net_controller_caps, ptr + offset, 4);
        controller_capabilities = static_cast<ControllerCapabilities>(ntohl(net_controller_caps));
        offset += 4;
        
        // available_index (4 bytes, network byte order)
        uint32_t net_available_index;
        memcpy(&net_available_index, ptr + offset, 4);
        available_index = ntohl(net_available_index);
        offset += 4;
        
        // gptp_grandmaster_id (8 bytes, network byte order)
        uint64_t net_gptp_gm;
        memcpy(&net_gptp_gm, ptr + offset, 8);
        gptp_grandmaster_id = ntohll(net_gptp_gm);
        offset += 8;
        
        // gptp_domain_number (1 byte)
        gptp_domain_number = ptr[offset++];
        
        // reserved0 (3 bytes)
        memcpy(reserved0, ptr + offset, 3);
        offset += 3;
        
        // identify_control_index (2 bytes, network byte order)
        uint16_t net_identify_index;
        memcpy(&net_identify_index, ptr + offset, 2);
        identify_control_index = ntohs(net_identify_index);
        offset += 2;
        
        // interface_index (2 bytes, network byte order)
        uint16_t net_interface_index;
        memcpy(&net_interface_index, ptr + offset, 2);
        interface_index = ntohs(net_interface_index);
        offset += 2;
        
        // association_id (8 bytes, network byte order)
        uint64_t net_association_id;
        memcpy(&net_association_id, ptr + offset, 8);
        association_id = ntohll(net_association_id);
        offset += 8;
        
        // reserved1 (4 bytes)
        uint32_t net_reserved1;
        memcpy(&net_reserved1, ptr + offset, 4);
        reserved1 = ntohl(net_reserved1);
        offset += 4;

        // Store raw data
        raw_octets_.assign(data, data + length);
        is_valid_ = true;
        return true;
    }

} // namespace ADP

// ==============================
// ACMP Implementation
// ==============================

namespace IEEE::_1722_1::_2013::ACMP {

    ACMPMessage::ACMPMessage() 
        : message_type(CONNECT_TX_COMMAND), status(SUCCESS), control_data_length(44),
          stream_id(0), controller_entity_id(0), talker_entity_id(0), listener_entity_id(0),
          talker_unique_id(0), listener_unique_id(0), connection_count(0), sequence_id(0),
          flags(static_cast<ACMPFlags>(0)), stream_vlan_id(0), reserved(0) {
        
        stream_dest_mac.fill(0);
    }

    void ACMPMessage::serialize(std::vector<uint8_t>& buffer) const {
        buffer.resize(44); // ACMP message size
        uint8_t* ptr = buffer.data();
        size_t offset = 0;

        // Serialize according to IEEE 1722.1-2013 Table 8.3
        ptr[offset++] = static_cast<uint8_t>(message_type);
        ptr[offset++] = static_cast<uint8_t>(status);
        
        // control_data_length (2 bytes, network byte order)
        uint16_t net_control_length = htons(control_data_length);
        memcpy(ptr + offset, &net_control_length, 2);
        offset += 2;
        
        // stream_id (8 bytes, network byte order)
        uint64_t net_stream_id = htonll(stream_id);
        memcpy(ptr + offset, &net_stream_id, 8);
        offset += 8;
        
        // controller_entity_id (8 bytes, network byte order)
        uint64_t net_controller_id = htonll(controller_entity_id);
        memcpy(ptr + offset, &net_controller_id, 8);
        offset += 8;
        
        // talker_entity_id (8 bytes, network byte order)
        uint64_t net_talker_id = htonll(talker_entity_id);
        memcpy(ptr + offset, &net_talker_id, 8);
        offset += 8;
        
        // listener_entity_id (8 bytes, network byte order)
        uint64_t net_listener_id = htonll(listener_entity_id);
        memcpy(ptr + offset, &net_listener_id, 8);
        offset += 8;
        
        // talker_unique_id (2 bytes, network byte order)
        uint16_t net_talker_unique = htons(talker_unique_id);
        memcpy(ptr + offset, &net_talker_unique, 2);
        offset += 2;
        
        // listener_unique_id (2 bytes, network byte order)
        uint16_t net_listener_unique = htons(listener_unique_id);
        memcpy(ptr + offset, &net_listener_unique, 2);
        offset += 2;
        
        // stream_dest_mac (6 bytes)
        memcpy(ptr + offset, stream_dest_mac.data(), 6);
        offset += 6;
        
        // connection_count (2 bytes, network byte order)
        uint16_t net_connection_count = htons(connection_count);
        memcpy(ptr + offset, &net_connection_count, 2);
        offset += 2;
        
        // sequence_id (2 bytes, network byte order)
        uint16_t net_sequence_id = htons(sequence_id);
        memcpy(ptr + offset, &net_sequence_id, 2);
        offset += 2;
        
        // flags (2 bytes, network byte order)
        uint16_t net_flags = htons(static_cast<uint16_t>(flags));
        memcpy(ptr + offset, &net_flags, 2);
        offset += 2;
        
        // stream_vlan_id (2 bytes, network byte order)
        uint16_t net_vlan_id = htons(stream_vlan_id);
        memcpy(ptr + offset, &net_vlan_id, 2);
        offset += 2;
        
        // reserved (2 bytes, network byte order)
        uint16_t net_reserved = htons(reserved);
        memcpy(ptr + offset, &net_reserved, 2);
        offset += 2;
    }

    bool ACMPMessage::deserialize(const uint8_t* data, size_t length) {
        if (!data || length < 44) {
            return false;
        }

        const uint8_t* ptr = data;
        size_t offset = 0;

        // Deserialize according to IEEE 1722.1-2013 Table 8.3
        message_type = static_cast<ACMPMessageType>(ptr[offset++]);
        status = static_cast<ACMPStatus>(ptr[offset++]);
        
        // control_data_length (2 bytes, network byte order)
        uint16_t net_control_length;
        memcpy(&net_control_length, ptr + offset, 2);
        control_data_length = ntohs(net_control_length);
        offset += 2;
        
        // stream_id (8 bytes, network byte order)
        uint64_t net_stream_id;
        memcpy(&net_stream_id, ptr + offset, 8);
        stream_id = ntohll(net_stream_id);
        offset += 8;
        
        // controller_entity_id (8 bytes, network byte order)
        uint64_t net_controller_id;
        memcpy(&net_controller_id, ptr + offset, 8);
        controller_entity_id = ntohll(net_controller_id);
        offset += 8;
        
        // talker_entity_id (8 bytes, network byte order)
        uint64_t net_talker_id;
        memcpy(&net_talker_id, ptr + offset, 8);
        talker_entity_id = ntohll(net_talker_id);
        offset += 8;
        
        // listener_entity_id (8 bytes, network byte order)
        uint64_t net_listener_id;
        memcpy(&net_listener_id, ptr + offset, 8);
        listener_entity_id = ntohll(net_listener_id);
        offset += 8;
        
        // talker_unique_id (2 bytes, network byte order)
        uint16_t net_talker_unique;
        memcpy(&net_talker_unique, ptr + offset, 2);
        talker_unique_id = ntohs(net_talker_unique);
        offset += 2;
        
        // listener_unique_id (2 bytes, network byte order)
        uint16_t net_listener_unique;
        memcpy(&net_listener_unique, ptr + offset, 2);
        listener_unique_id = ntohs(net_listener_unique);
        offset += 2;
        
        // stream_dest_mac (6 bytes)
        memcpy(stream_dest_mac.data(), ptr + offset, 6);
        offset += 6;
        
        // connection_count (2 bytes, network byte order)
        uint16_t net_connection_count;
        memcpy(&net_connection_count, ptr + offset, 2);
        connection_count = ntohs(net_connection_count);
        offset += 2;
        
        // sequence_id (2 bytes, network byte order)
        uint16_t net_sequence_id;
        memcpy(&net_sequence_id, ptr + offset, 2);
        sequence_id = ntohs(net_sequence_id);
        offset += 2;
        
        // flags (2 bytes, network byte order)
        uint16_t net_flags;
        memcpy(&net_flags, ptr + offset, 2);
        flags = static_cast<ACMPFlags>(ntohs(net_flags));
        offset += 2;
        
        // stream_vlan_id (2 bytes, network byte order)
        uint16_t net_vlan_id;
        memcpy(&net_vlan_id, ptr + offset, 2);
        stream_vlan_id = ntohs(net_vlan_id);
        offset += 2;
        
        // reserved (2 bytes, network byte order)
        uint16_t net_reserved;
        memcpy(&net_reserved, ptr + offset, 2);
        reserved = ntohs(net_reserved);
        offset += 2;

        return true;
    }

    bool ACMPMessage::is_valid() const {
        // Basic validation
        return (message_type <= GET_TX_CONNECTION_RESPONSE) && 
               (status <= NOT_SUPPORTED) &&
               (control_data_length == 44);
    }

} // namespace ACMP

// ==============================
// Common Structures Implementation
// ==============================

AVTPDUHeader::AVTPDUHeader() 
    : subtype(0xFA), sv_ver_mr_gv_tv_seq_tu(0x00), control_data_length(0), target_entity_id(0) {
}

// ==============================
// AVDECCEntity_2013 Implementation
// ==============================

AVDECCEntity_2013::AVDECCEntity_2013() 
    : entity_id(0), entity_model_id(0),
      entity_capabilities(ADP::EntityCapabilities::AEM_SUPPORTED),
      talker_stream_sources(0), talker_capabilities(ADP::TalkerCapabilities::IMPLEMENTED),
      listener_stream_sinks(0), listener_capabilities(ADP::ListenerCapabilities::IMPLEMENTED),
      controller_capabilities(ADP::ControllerCapabilities::IMPLEMENTED),
      available_index(0), association_id(0),
      entity_name("OpenAvnu IEEE 1722.1-2013 Entity"),
      firmware_version("1.0.0"),
      serial_number("OA2013001") {
}

ADP::AVDECCDiscoveryProtocolPDU AVDECCEntity_2013::create_adp_message(ADP::ADPMessageType type) const {
    ADP::AVDECCDiscoveryProtocolPDU adpdu;
    
    adpdu.set_message_type(type);
    adpdu.set_valid_time(62); // Default 2 minutes
    adpdu.set_entity_id(entity_id);
    adpdu.set_entity_model_id(entity_model_id);
    adpdu.set_entity_capabilities(entity_capabilities);
    adpdu.set_talker_info(talker_stream_sources, talker_capabilities);
    adpdu.set_listener_info(listener_stream_sinks, listener_capabilities);
    adpdu.set_controller_capabilities(controller_capabilities);
    adpdu.set_available_index(available_index);
    adpdu.set_association_id(association_id);
    
    adpdu.serialize();
    return adpdu;
}

bool AVDECCEntity_2013::update_from_adp(const ADP::AVDECCDiscoveryProtocolPDU& adpdu) {
    if (!adpdu.is_valid()) {
        return false;
    }
    
    // Update entity information from ADP message
    entity_id = adpdu.entity_id;
    entity_model_id = adpdu.entity_model_id;
    entity_capabilities = adpdu.entity_capabilities;
    talker_stream_sources = adpdu.talker_stream_sources;
    talker_capabilities = adpdu.talker_capabilities;
    listener_stream_sinks = adpdu.listener_stream_sinks;
    listener_capabilities = adpdu.listener_capabilities;
    controller_capabilities = adpdu.controller_capabilities;
    available_index = adpdu.available_index;
    association_id = adpdu.association_id;
    
    return true;
}

AEM::EntityDescriptor AVDECCEntity_2013::get_entity_descriptor() const {
    AEM::EntityDescriptor desc = {};
    
    desc.descriptor_type = AEM::DescriptorType::ENTITY;
    desc.descriptor_index = 0;
    desc.entity_id = entity_id;
    desc.entity_model_id = entity_model_id;
    desc.entity_capabilities = entity_capabilities;
    desc.talker_stream_sources = talker_stream_sources;
    desc.talker_capabilities = talker_capabilities;
    desc.listener_stream_sinks = listener_stream_sinks;
    desc.listener_capabilities = listener_capabilities;
    desc.controller_capabilities = controller_capabilities;
    desc.available_index = available_index;
    desc.association_id = association_id;
    
    // Copy strings safely
    AEM::safe_string_copy(desc.entity_name, entity_name.c_str(), sizeof(desc.entity_name));
    AEM::safe_string_copy(desc.firmware_version, firmware_version.c_str(), sizeof(desc.firmware_version));
    AEM::safe_string_copy(desc.serial_number, serial_number.c_str(), sizeof(desc.serial_number));
    
    desc.configurations_count = 1;
    desc.current_configuration = 0;
    
    return desc;
}

bool AVDECCEntity_2013::is_valid() const {
    return (entity_id != 0) && (!entity_name.empty());
}

std::string AVDECCEntity_2013::get_capabilities_string() const {
    std::stringstream ss;
    uint32_t entity_caps = static_cast<uint32_t>(entity_capabilities);
    uint32_t controller_caps = static_cast<uint32_t>(controller_capabilities);
    
    ss << "Entity Capabilities (0x" << std::hex << entity_caps << "): ";
    
    if (entity_caps & static_cast<uint32_t>(ADP::EntityCapabilities::AEM_SUPPORTED)) {
        ss << "AEM ";
    }
    if (entity_caps & static_cast<uint32_t>(ADP::EntityCapabilities::CLASS_A_SUPPORTED)) {
        ss << "ClassA ";
    }
    if (entity_caps & static_cast<uint32_t>(ADP::EntityCapabilities::CLASS_B_SUPPORTED)) {
        ss << "ClassB ";
    }
    if (entity_caps & static_cast<uint32_t>(ADP::EntityCapabilities::GPTP_SUPPORTED)) {
        ss << "gPTP ";
    }
    
    // Add controller capabilities if present
    if (controller_caps & static_cast<uint32_t>(ADP::ControllerCapabilities::IMPLEMENTED)) {
        ss << "Controller ";
    }
    
    return ss.str();
}
