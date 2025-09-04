/**
 * @file 1722_1-2013.cpp
 * @brief IEEE 1722.1-2013 Standard Implementation
 * 
 * Implementation of IEEE Standard for Device Discovery, Connection Management, 
 * and Control Protocol for IEEE 1722™ Based Devices (2013 version).
 * 
 * @version 1.0.0
 * @date 2025
 * @copyright OpenAvnu Project
 */

#include "ieee_1722_1_2013_legacy.h"

namespace IEEE {
namespace _1722_1 {
namespace _2013 {
namespace ADP {

    // ==============================
    // AVDECCDiscoveryProtocolPDU Implementation
    // ==============================

    AVDECCDiscoveryProtocolPDU::AVDECCDiscoveryProtocolPDU() 
        : message_type(ENTITY_AVAILABLE)
        , valid_time(31)  // Default 62 seconds (31 * 2)
        , entity_id(0)
        , entity_model_id(0)
        , entity_capabilities(EntityCapabilities::AEM_SUPPORTED)
        , talker_stream_sources(0)
        , talker_capabilities(TalkerCapabilities::IMPLEMENTED)
        , listener_stream_sinks(0)
        , listener_capabilities(ListenerCapabilities::IMPLEMENTED)
        , controller_capabilities(ControllerCapabilities::IMPLEMENTED)
        , available_index(0)
        , gptp_grandmaster_id(0)
        , gptp_domain_number(0)
        , identify_control_index(0)
        , interface_index(0)
        , association_id(0)
        , reserved1(0)
        , is_valid_(true)
    {
        // Initialize reserved fields
        memset(reserved0, 0, sizeof(reserved0));
        
        // Pre-allocate space for serialized data (56 bytes for 2013 ADPDU)
        raw_octets_.resize(56);
    }

    AVDECCDiscoveryProtocolPDU::AVDECCDiscoveryProtocolPDU(const uint8_t* data, size_t length) 
        : AVDECCDiscoveryProtocolPDU()
    {
        is_valid_ = deserialize(data, length);
    }

    void AVDECCDiscoveryProtocolPDU::serialize() {
        // Ensure we have the correct size for IEEE 1722.1-2013 ADPDU (56 bytes)
        raw_octets_.resize(56);
        uint8_t* data = raw_octets_.data();
        
        // Serialize all fields in network byte order
        size_t offset = 0;
        
        // entity_model_id (8 bytes)
        uint64_t entity_model_id_be = htonll(entity_model_id);
        memcpy(data + offset, &entity_model_id_be, 8);
        offset += 8;
        
        // entity_capabilities (4 bytes)
        uint32_t entity_caps_be = htonl(static_cast<uint32_t>(entity_capabilities));
        memcpy(data + offset, &entity_caps_be, 4);
        offset += 4;
        
        // talker_stream_sources (2 bytes)
        uint16_t talker_sources_be = htons(talker_stream_sources);
        memcpy(data + offset, &talker_sources_be, 2);
        offset += 2;
        
        // talker_capabilities (2 bytes)
        uint16_t talker_caps_be = htons(static_cast<uint16_t>(talker_capabilities));
        memcpy(data + offset, &talker_caps_be, 2);
        offset += 2;
        
        // listener_stream_sinks (2 bytes)
        uint16_t listener_sinks_be = htons(listener_stream_sinks);
        memcpy(data + offset, &listener_sinks_be, 2);
        offset += 2;
        
        // listener_capabilities (2 bytes)
        uint16_t listener_caps_be = htons(static_cast<uint16_t>(listener_capabilities));
        memcpy(data + offset, &listener_caps_be, 2);
        offset += 2;
        
        // controller_capabilities (4 bytes)
        uint32_t controller_caps_be = htonl(static_cast<uint32_t>(controller_capabilities));
        memcpy(data + offset, &controller_caps_be, 4);
        offset += 4;
        
        // available_index (4 bytes)
        uint32_t avail_index_be = htonl(available_index);
        memcpy(data + offset, &avail_index_be, 4);
        offset += 4;
        
        // gptp_grandmaster_id (8 bytes)
        uint64_t gptp_gm_id_be = htonll(gptp_grandmaster_id);
        memcpy(data + offset, &gptp_gm_id_be, 8);
        offset += 8;
        
        // gptp_domain_number (1 byte)
        data[offset] = gptp_domain_number;
        offset += 1;
        
        // reserved0 (3 bytes)
        memset(data + offset, 0, 3);
        offset += 3;
        
        // identify_control_index (2 bytes)
        uint16_t identify_idx_be = htons(identify_control_index);
        memcpy(data + offset, &identify_idx_be, 2);
        offset += 2;
        
        // interface_index (2 bytes)
        uint16_t interface_idx_be = htons(interface_index);
        memcpy(data + offset, &interface_idx_be, 2);
        offset += 2;
        
        // association_id (8 bytes)
        uint64_t assoc_id_be = htonll(association_id);
        memcpy(data + offset, &assoc_id_be, 8);
        offset += 8;
        
        // reserved1 (4 bytes)
        uint32_t reserved1_be = htonl(reserved1);
        memcpy(data + offset, &reserved1_be, 4);
        offset += 4;
        
        is_valid_ = true;
    }

    bool AVDECCDiscoveryProtocolPDU::deserialize(const uint8_t* data, size_t length) {
        if (!data || length < 56) {
            is_valid_ = false;
            return false;
        }
        
        // Copy raw data
        raw_octets_.resize(length < 56 ? length : 56);
        memcpy(raw_octets_.data(), data, raw_octets_.size());
        
        // Deserialize all fields from network byte order
        size_t offset = 0;
        
        // entity_model_id (8 bytes)
        uint64_t entity_model_id_be;
        memcpy(&entity_model_id_be, data + offset, 8);
        entity_model_id = ntohll(entity_model_id_be);
        offset += 8;
        
        // entity_capabilities (4 bytes)
        uint32_t entity_caps_be;
        memcpy(&entity_caps_be, data + offset, 4);
        entity_capabilities = static_cast<EntityCapabilities>(ntohl(entity_caps_be));
        offset += 4;
        
        // talker_stream_sources (2 bytes)
        uint16_t talker_sources_be;
        memcpy(&talker_sources_be, data + offset, 2);
        talker_stream_sources = ntohs(talker_sources_be);
        offset += 2;
        
        // talker_capabilities (2 bytes)
        uint16_t talker_caps_be;
        memcpy(&talker_caps_be, data + offset, 2);
        talker_capabilities = static_cast<TalkerCapabilities>(ntohs(talker_caps_be));
        offset += 2;
        
        // listener_stream_sinks (2 bytes)
        uint16_t listener_sinks_be;
        memcpy(&listener_sinks_be, data + offset, 2);
        listener_stream_sinks = ntohs(listener_sinks_be);
        offset += 2;
        
        // listener_capabilities (2 bytes)
        uint16_t listener_caps_be;
        memcpy(&listener_caps_be, data + offset, 2);
        listener_capabilities = static_cast<ListenerCapabilities>(ntohs(listener_caps_be));
        offset += 2;
        
        // controller_capabilities (4 bytes)
        uint32_t controller_caps_be;
        memcpy(&controller_caps_be, data + offset, 4);
        controller_capabilities = static_cast<ControllerCapabilities>(ntohl(controller_caps_be));
        offset += 4;
        
        // available_index (4 bytes)
        uint32_t avail_index_be;
        memcpy(&avail_index_be, data + offset, 4);
        available_index = ntohl(avail_index_be);
        offset += 4;
        
        // gptp_grandmaster_id (8 bytes)
        uint64_t gptp_gm_id_be;
        memcpy(&gptp_gm_id_be, data + offset, 8);
        gptp_grandmaster_id = ntohll(gptp_gm_id_be);
        offset += 8;
        
        // gptp_domain_number (1 byte)
        gptp_domain_number = data[offset];
        offset += 1;
        
        // reserved0 (3 bytes) - skip
        memcpy(reserved0, data + offset, 3);
        offset += 3;
        
        // identify_control_index (2 bytes)
        uint16_t identify_idx_be;
        memcpy(&identify_idx_be, data + offset, 2);
        identify_control_index = ntohs(identify_idx_be);
        offset += 2;
        
        // interface_index (2 bytes)
        uint16_t interface_idx_be;
        memcpy(&interface_idx_be, data + offset, 2);
        interface_index = ntohs(interface_idx_be);
        offset += 2;
        
        // association_id (8 bytes)
        uint64_t assoc_id_be;
        memcpy(&assoc_id_be, data + offset, 8);
        association_id = ntohll(assoc_id_be);
        offset += 8;
        
        // reserved1 (4 bytes)
        uint32_t reserved1_be;
        memcpy(&reserved1_be, data + offset, 4);
        reserved1 = ntohl(reserved1_be);
        offset += 4;
        
        is_valid_ = true;
        return true;
    }

    // ==============================
} // namespace ADP
} // namespace _2013
} // namespace _1722_1
} // namespace IEEE
