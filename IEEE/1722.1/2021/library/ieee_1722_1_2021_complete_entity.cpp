/**
 * @file ieee_1722_1_2021_complete_entity.cpp
 * @brief Implementation of the complete IEEE 1722.1-2021 AVDECC Entity
 */

#include "ieee_1722_1_2021_complete_entity.h"
#include <cstring>
#include <algorithm>

namespace IEEE {
namespace _1722_1 {
namespace _2021 {

// Constructor
IEEE17221_2021CompleteEntity::IEEE17221_2021CompleteEntity(uint64_t entity_id) {
    // Initialize entity descriptor with basic values
    std::memset(&entity_descriptor_, 0, sizeof(entity_descriptor_));
    entity_descriptor_.descriptor_type = AEM::DescriptorType::ENTITY;
    entity_descriptor_.descriptor_index = 0;
    entity_descriptor_.entity_id = entity_id;
    entity_descriptor_.entity_model_id = 0x0000000000000001ULL;
    entity_descriptor_.entity_capabilities = 0x00000001; // AEM_SUPPORTED
    entity_descriptor_.talker_stream_sources = 1;
    entity_descriptor_.talker_capabilities = 0x0001; // IMPLEMENTED
    entity_descriptor_.listener_stream_sinks = 1;
    entity_descriptor_.listener_capabilities = 0x0001; // IMPLEMENTED
    entity_descriptor_.controller_capabilities = 0x0001; // IMPLEMENTED
    entity_descriptor_.available_index = 1001;
    entity_descriptor_.association_id = 0;
    
    // Set entity name
    const char* name = "OpenAvnu IEEE 1722.1-2021 Complete Entity";
    size_t name_len = std::min(strlen(name), static_cast<size_t>(64));
    std::memcpy(entity_descriptor_.entity_name, name, name_len);
}

// Main command processing interface
AECP::ATDECCEnumerationControlProtocolPDU IEEE17221_2021CompleteEntity::process_command(
    const AECP::ATDECCEnumerationControlProtocolPDU& command) {
    
    AECP::ATDECCEnumerationControlProtocolPDU response = command;
    response.status = AECP::AEM_Status::SUCCESS;
    
    switch (command.command_type) {
        case AECP::AEM_Command_type::ACQUIRE_ENTITY:
            return handle_acquire_entity(command);
        case AECP::AEM_Command_type::LOCK_ENTITY:
            return handle_lock_entity(command);
        case AECP::AEM_Command_type::ENTITY_AVAILABLE:
            return handle_entity_available(command);
        case AECP::AEM_Command_type::CONTROLLER_AVAILABLE:
            return handle_controller_available(command);
        case AECP::AEM_Command_type::READ_DESCRIPTOR:
            return handle_read_descriptor(command);
        case AECP::AEM_Command_type::WRITE_DESCRIPTOR:
            return handle_write_descriptor(command);
        case AECP::AEM_Command_type::SET_CONFIGURATION:
            return handle_set_configuration(command);
        case AECP::AEM_Command_type::GET_CONFIGURATION:
            return handle_get_configuration(command);
        default:
            response.status = AECP::AEM_Status::NOT_IMPLEMENTED;
            break;
    }
    
    return response;
}

// Basic Entity Management Commands
AECP::ATDECCEnumerationControlProtocolPDU IEEE17221_2021CompleteEntity::handle_acquire_entity(
    const AECP::ATDECCEnumerationControlProtocolPDU& command) {
    
    AECP::ATDECCEnumerationControlProtocolPDU response = command;
    
    if (acquire_owner_id_ == 0 || acquire_owner_id_ == command.controller_entity_id) {
        acquire_owner_id_ = command.controller_entity_id;
        response.status = AECP::AEM_Status::SUCCESS;
    } else {
        response.status = AECP::AEM_Status::ENTITY_ACQUIRED;
    }
    
    return response;
}

AECP::ATDECCEnumerationControlProtocolPDU IEEE17221_2021CompleteEntity::handle_lock_entity(
    const AECP::ATDECCEnumerationControlProtocolPDU& command) {
    
    AECP::ATDECCEnumerationControlProtocolPDU response = command;
    
    if (lock_owner_id_ == 0 || lock_owner_id_ == command.controller_entity_id) {
        lock_owner_id_ = command.controller_entity_id;
        response.status = AECP::AEM_Status::SUCCESS;
    } else {
        response.status = AECP::AEM_Status::ENTITY_LOCKED;
    }
    
    return response;
}

AECP::ATDECCEnumerationControlProtocolPDU IEEE17221_2021CompleteEntity::handle_entity_available(
    const AECP::ATDECCEnumerationControlProtocolPDU& command) {
    
    AECP::ATDECCEnumerationControlProtocolPDU response = command;
    response.status = AECP::AEM_Status::SUCCESS;
    
    // Entity is always available
    return response;
}

AECP::ATDECCEnumerationControlProtocolPDU IEEE17221_2021CompleteEntity::handle_controller_available(
    const AECP::ATDECCEnumerationControlProtocolPDU& command) {
    
    AECP::ATDECCEnumerationControlProtocolPDU response = command;
    response.status = AECP::AEM_Status::SUCCESS;
    
    // Controller availability acknowledged
    return response;
}

// Descriptor Management Commands
AECP::ATDECCEnumerationControlProtocolPDU IEEE17221_2021CompleteEntity::handle_read_descriptor(
    const AECP::ATDECCEnumerationControlProtocolPDU& command) {
    
    AECP::ATDECCEnumerationControlProtocolPDU response = command;
    
    // Extract descriptor type and index from command
    if (command.command_specific_length >= 4) {
        uint16_t descriptor_type = (command.command_specific_data[0] << 8) | command.command_specific_data[1];
        uint16_t descriptor_index = (command.command_specific_data[2] << 8) | command.command_specific_data[3];
        
        if (descriptor_type == static_cast<uint16_t>(AEM::DescriptorType::ENTITY) && descriptor_index == 0) {
            // Return entity descriptor
            response.status = AECP::AEM_Status::SUCCESS;
            response.command_specific_length = sizeof(entity_descriptor_) + 4;
            std::memcpy(response.command_specific_data, &descriptor_type, 2);
            std::memcpy(response.command_specific_data + 2, &descriptor_index, 2);
            std::memcpy(response.command_specific_data + 4, &entity_descriptor_, sizeof(entity_descriptor_));
        } else {
            response.status = AECP::AEM_Status::NO_SUCH_DESCRIPTOR;
        }
    } else {
        response.status = AECP::AEM_Status::BAD_ARGUMENTS;
    }
    
    return response;
}

AECP::ATDECCEnumerationControlProtocolPDU IEEE17221_2021CompleteEntity::handle_write_descriptor(
    const AECP::ATDECCEnumerationControlProtocolPDU& command) {
    
    AECP::ATDECCEnumerationControlProtocolPDU response = command;
    response.status = AECP::AEM_Status::NOT_SUPPORTED;
    
    // Most descriptors are read-only
    return response;
}

AECP::ATDECCEnumerationControlProtocolPDU IEEE17221_2021CompleteEntity::handle_set_configuration(
    const AECP::ATDECCEnumerationControlProtocolPDU& command) {
    
    AECP::ATDECCEnumerationControlProtocolPDU response = command;
    
    if (command.command_specific_length >= 2) {
        uint16_t config_index = (command.command_specific_data[0] << 8) | command.command_specific_data[1];
        
        if (configuration_descriptors_.find(config_index) != configuration_descriptors_.end()) {
            current_configuration_ = config_index;
            response.status = AECP::AEM_Status::SUCCESS;
        } else {
            response.status = AECP::AEM_Status::NO_SUCH_DESCRIPTOR;
        }
    } else {
        response.status = AECP::AEM_Status::BAD_ARGUMENTS;
    }
    
    return response;
}

AECP::ATDECCEnumerationControlProtocolPDU IEEE17221_2021CompleteEntity::handle_get_configuration(
    const AECP::ATDECCEnumerationControlProtocolPDU& command) {
    
    AECP::ATDECCEnumerationControlProtocolPDU response = command;
    response.status = AECP::AEM_Status::SUCCESS;
    response.command_specific_length = 2;
    response.command_specific_data[0] = (current_configuration_ >> 8) & 0xFF;
    response.command_specific_data[1] = current_configuration_ & 0xFF;
    
    return response;
}

// Helper methods for entity configuration
void IEEE17221_2021CompleteEntity::set_entity_name(const std::string& name) {
    std::memset(entity_descriptor_.entity_name, 0, sizeof(entity_descriptor_.entity_name));
    size_t copy_len = std::min(name.length(), static_cast<size_t>(64));
    std::memcpy(entity_descriptor_.entity_name, name.c_str(), copy_len);
}

void IEEE17221_2021CompleteEntity::add_configuration_descriptor(uint16_t config_index, const AEM::ConfigurationDescriptor& config) {
    configuration_descriptors_[config_index] = config;
}

void IEEE17221_2021CompleteEntity::add_stream_input_descriptor(uint16_t config_index, uint16_t stream_index, const AEM::StreamInputDescriptor& stream) {
    stream_input_descriptors_[std::make_pair(config_index, stream_index)] = stream;
}

void IEEE17221_2021CompleteEntity::add_stream_output_descriptor(uint16_t config_index, uint16_t stream_index, const AEM::StreamOutputDescriptor& stream) {
    stream_output_descriptors_[std::make_pair(config_index, stream_index)] = stream;
}

void IEEE17221_2021CompleteEntity::add_avb_interface_descriptor(uint16_t config_index, uint16_t interface_index, const AEM::AvbInterfaceDescriptor& avb_interface) {
    avb_interface_descriptors_[std::make_pair(config_index, interface_index)] = avb_interface;
}

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
