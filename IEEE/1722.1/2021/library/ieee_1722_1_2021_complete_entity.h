#pragma once

#include "1722_1-2021_clean.h"
#include <map>
#include <functional>
#include <vector>
#include <memory>

namespace IEEE {
namespace _1722_1 {
namespace _2021 {

/**
 * @class IEEE17221_2021CompleteEntity
 * @brief Complete IEEE 1722.1-2021 AVDECC Entity Implementation
 * 
 * This class provides a complete implementation of all 75 AEM commands
 * defined in IEEE 1722.1-2021, addressing the implementation gap identified
 * in the existing OpenAvnu AVDECC entities.
 */
class IEEE17221_2021CompleteEntity {
public:
    // Constructor
    IEEE17221_2021CompleteEntity(uint64_t entity_id = 0x0000000000000001ULL);
    virtual ~IEEE17221_2021CompleteEntity() = default;

    // Main command processing interface
    AECP::ATDECCEnumerationControlProtocolPDU process_command(
        const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Entity configuration
    void set_entity_id(uint64_t id) { entity_descriptor_.entity_id = id; }
    void set_entity_model_id(uint64_t model_id) { entity_descriptor_.entity_model_id = model_id; }
    void set_entity_name(const std::string& name);
    
    // Descriptor management
    void add_configuration_descriptor(uint16_t config_index, const AEM::ConfigurationDescriptor& config);
    void add_stream_input_descriptor(uint16_t config_index, uint16_t stream_index, const AEM::StreamInputDescriptor& stream);
    void add_stream_output_descriptor(uint16_t config_index, uint16_t stream_index, const AEM::StreamOutputDescriptor& stream);
    void add_avb_interface_descriptor(uint16_t config_index, uint16_t interface_index, const AEM::AvbInterfaceDescriptor& avb_interface);
    
    // Status and capability getters
    uint64_t get_entity_id() const { return entity_descriptor_.entity_id; }
    uint16_t get_current_configuration() const { return current_configuration_; }
    uint32_t get_available_index() const { return available_index_; }

private:
    // Entity state
    AEM::EntityDescriptor entity_descriptor_;
    uint16_t current_configuration_ = 0;
    uint32_t available_index_ = 1001;
    uint64_t acquire_owner_id_ = 0;
    uint64_t lock_owner_id_ = 0;
    
    // Descriptor storage
    std::map<uint16_t, AEM::ConfigurationDescriptor> configuration_descriptors_;
    std::map<std::pair<uint16_t, uint16_t>, AEM::StreamInputDescriptor> stream_input_descriptors_;
    std::map<std::pair<uint16_t, uint16_t>, AEM::StreamOutputDescriptor> stream_output_descriptors_;
    std::map<std::pair<uint16_t, uint16_t>, AEM::AvbInterfaceDescriptor> avb_interface_descriptors_;
    std::map<std::pair<uint16_t, uint16_t>, AEM::AudioUnitDescriptor> audio_unit_descriptors_;
    std::map<std::pair<uint16_t, uint16_t>, AEM::ClockSourceDescriptor> clock_source_descriptors_;
    std::map<std::pair<uint16_t, uint16_t>, AEM::ControlDescriptor> control_descriptors_;
    
    // Command handlers - Basic Entity Management (0x0000-0x0003)
    AECP::ATDECCEnumerationControlProtocolPDU handle_acquire_entity(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_lock_entity(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_entity_available(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_controller_available(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Descriptor Management (0x0004-0x0007)
    AECP::ATDECCEnumerationControlProtocolPDU handle_read_descriptor(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_write_descriptor(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_configuration(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_configuration(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Stream Format Management (0x0008-0x000D)
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_stream_format(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_stream_format(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_video_format(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_video_format(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_sensor_format(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_sensor_format(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Stream Information Management (0x000E-0x000F)
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_stream_info(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_stream_info(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Name Management (0x0010-0x0011)
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_name(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_name(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Association Management (0x0012-0x0013)
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_association_id(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_association_id(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Audio System Management (0x0014-0x0017)
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_sampling_rate(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_sampling_rate(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_clock_source(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_clock_source(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Control System Management (0x0018-0x0021)
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_control(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_control(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_increment_control(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_decrement_control(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_signal_selector(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_signal_selector(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_mixer(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_mixer(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_set_matrix(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_matrix(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Streaming Control (0x0022-0x0023)
    AECP::ATDECCEnumerationControlProtocolPDU handle_start_streaming(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_stop_streaming(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Notification Management (0x0024-0x0026)
    AECP::ATDECCEnumerationControlProtocolPDU handle_register_unsolicited_notification(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_deregister_unsolicited_notification(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_identify_notification(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Network Information (0x0027-0x0029)
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_avb_info(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_as_path(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_counters(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // System Management (0x002A)
    AECP::ATDECCEnumerationControlProtocolPDU handle_reboot(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Audio Mapping (0x002B-0x002D)
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_audio_map(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_add_audio_mappings(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_remove_audio_mappings(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Video Mapping (0x002E-0x0030)
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_video_map(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_add_video_mappings(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_remove_video_mappings(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Sensor Mapping (0x0031-0x0033)
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_sensor_map(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_add_sensor_mappings(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_remove_sensor_mappings(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Operation Management (0x0034-0x0036)
    AECP::ATDECCEnumerationControlProtocolPDU handle_start_operation(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_abort_operation(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_operation_status(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Authentication & Security (0x0037-0x004A) - 20 commands
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_add_key(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_delete_key(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_get_key_list(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_get_key(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_add_key_to_chain(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_delete_key_from_chain(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_get_keychain_list(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_get_identity(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_add_certificate_to_chain(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_delete_certificate_from_chain(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_get_certificate_chain(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_get_certificate(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_add_crl(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_delete_crl(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_get_crl_list(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_auth_get_crl(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_authenticate(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_deauthenticate(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_enable_transport_security(const AECP::ATDECCEnumerationControlProtocolPDU& command);
    AECP::ATDECCEnumerationControlProtocolPDU handle_disable_transport_security(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Milan Extension (0x004B)
    AECP::ATDECCEnumerationControlProtocolPDU handle_get_dynamic_info(const AECP::ATDECCEnumerationControlProtocolPDU& command);

    // Utility methods
    AECP::ATDECCEnumerationControlProtocolPDU create_error_response(
        const AECP::ATDECCEnumerationControlProtocolPDU& command, 
        AECP::AECP_Status status);
    
    bool is_entity_acquired(uint64_t controller_id) const;
    bool is_entity_locked(uint64_t controller_id) const;
    
    void increment_available_index();

    // Command dispatch table
    using CommandHandler = std::function<AECP::ATDECCEnumerationControlProtocolPDU(
        const AECP::ATDECCEnumerationControlProtocolPDU&)>;
    std::map<AECP::AEM_Command_type, CommandHandler> command_handlers_;
    
    void initialize_command_handlers();
};

} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
