/**
 * @file test_core_descriptors.cpp
 * @brief Test compilation of IEEE 1722.1-2021 core descriptors
 */

#include "ieee_1722_1_2021_core.h"
#include <iostream>
#include <cstring>

int main() {
    using namespace IEEE::_1722_1::_2021;
    
    // Test EntityDescriptor
    AEM::EntityDescriptor entity_desc = {};
    entity_desc.entity_id = 0x0001020304050607ULL;
    entity_desc.entity_model_id = 0x00112233445566778ULL;
    entity_desc.entity_capabilities = EntityCapabilities::AEM_SUPPORTED | EntityCapabilities::CLASS_A_SUPPORTED;
    
    // Test ConfigurationDescriptor
    AEM::ConfigurationDescriptor config_desc = {};
    std::strcpy(config_desc.object_name, "Main Configuration");
    
    // Test AudioUnitDescriptor
    AEM::AudioUnitDescriptor audio_unit = {};
    std::strcpy(audio_unit.object_name, "Audio Processing Unit");
    audio_unit.clock_domain_index = 0;
    audio_unit.number_of_stream_input_ports = 2;
    audio_unit.number_of_stream_output_ports = 2;
    
    // Test StreamInputDescriptor
    AEM::StreamInputDescriptor stream_in = {};
    std::strcpy(stream_in.object_name, "Input Stream");
    stream_in.current_format = 0x02F000020001ULL; // Example format
    
    // Test StreamOutputDescriptor  
    AEM::StreamOutputDescriptor stream_out = {};
    std::strcpy(stream_out.object_name, "Output Stream");
    stream_out.current_format = 0x02F000020001ULL; // Example format
    
    // Test AvbInterfaceDescriptor
    AEM::AvbInterfaceDescriptor avb_iface = {};
    std::strcpy(avb_iface.object_name, "AVB Interface");
    avb_iface.mac_address[0] = 0x00;
    avb_iface.mac_address[1] = 0x11;
    avb_iface.mac_address[2] = 0x22;
    avb_iface.mac_address[3] = 0x33;
    avb_iface.mac_address[4] = 0x44;
    avb_iface.mac_address[5] = 0x55;
    
    // Test ClockSourceDescriptor
    AEM::ClockSourceDescriptor clock_src = {};
    std::strcpy(clock_src.object_name, "Internal Clock");
    clock_src.clock_source_type = 0x0000; // Internal
    
    // Test ClockDomainDescriptor
    AEM::ClockDomainDescriptor clock_domain = {};
    std::strcpy(clock_domain.object_name, "Audio Clock Domain");
    clock_domain.clock_source_index = 0;
    
    // Test new descriptors - StreamPortInputDescriptor
    AEM::StreamPortInputDescriptor stream_port_in = {};
    std::strcpy(stream_port_in.object_name, "Input Stream Port");
    stream_port_in.clock_domain_index = 0;
    stream_port_in.port_flags = static_cast<uint16_t>(AEM::StreamPortFlags::ClockSyncSource);
    stream_port_in.number_of_clusters = 1;
    stream_port_in.number_of_maps = 1;
    
    // Test StreamPortOutputDescriptor
    AEM::StreamPortOutputDescriptor stream_port_out = {};
    std::strcpy(stream_port_out.object_name, "Output Stream Port");
    stream_port_out.clock_domain_index = 0;
    stream_port_out.port_flags = static_cast<uint16_t>(AEM::StreamPortFlags::AsyncSampleRateConv);
    stream_port_out.number_of_clusters = 2;
    stream_port_out.number_of_maps = 2;
    
    // Test AudioClusterDescriptor
    AEM::AudioClusterDescriptor audio_cluster = {};
    std::strcpy(audio_cluster.object_name, "Stereo Audio Cluster");
    audio_cluster.signal_type = AEM::SIGNAL_TYPE_AUDIO_CLUSTER;
    audio_cluster.signal_index = 0;
    audio_cluster.path_latency = 1000;  // 1 microsecond
    audio_cluster.block_latency = 500;  // 0.5 microsecond
    audio_cluster.channel_count = 2;    // Stereo
    audio_cluster.format = AEM::AUDIO_FORMAT_AAF;
    
    // Test AudioMapDescriptor
    AEM::AudioMapDescriptor audio_map_desc = {};
    std::strcpy(audio_map_desc.object_name, "Default Audio Map");
    audio_map_desc.number_of_mappings = 2;
    
    // Test ControlDescriptor
    AEM::ControlDescriptor control_desc = {};
    std::strcpy(control_desc.object_name, "Master Volume");
    control_desc.control_latency = 100;  // 100ns
    control_desc.control_value_type = static_cast<uint16_t>(AEM::ControlValueType::LinearFloat);
    control_desc.control_type = AEM::CONTROL_TYPE_GAIN;
    control_desc.signal_type = AEM::SIGNAL_TYPE_AUDIO_CLUSTER;
    
    // Test SignalSelectorDescriptor
    AEM::SignalSelectorDescriptor signal_selector = {};
    std::strcpy(signal_selector.object_name, "Input Source Selector");
    signal_selector.current_signal_type = AEM::SIGNAL_TYPE_AUDIO_CLUSTER;
    signal_selector.current_signal_index = 0;
    signal_selector.default_signal_type = AEM::SIGNAL_TYPE_AUDIO_CLUSTER;
    signal_selector.default_signal_index = 0;
    signal_selector.number_of_sources = 4;
    
    // Test MixerDescriptor
    AEM::MixerDescriptor mixer_desc = {};
    std::strcpy(mixer_desc.object_name, "Main Audio Mixer");
    mixer_desc.mixer_flags = static_cast<uint16_t>(AEM::MixerFlags::LinearGain | AEM::MixerFlags::MatrixControl);
    mixer_desc.number_of_sources = 8;
    mixer_desc.number_of_destinations = 2;
    
    // Test MatrixDescriptor
    AEM::MatrixDescriptor matrix_desc = {};
    std::strcpy(matrix_desc.object_name, "8x2 Audio Matrix");
    matrix_desc.matrix_flags = static_cast<uint16_t>(AEM::MatrixFlags::LinearGain);
    matrix_desc.width = 8;  // 8 inputs
    matrix_desc.height = 2; // 2 outputs
    matrix_desc.number_of_sources = 8;
    matrix_desc.number_of_destinations = 2;
    
    // Test LocaleDescriptor
    AEM::LocaleDescriptor locale_desc = {};
    std::strcpy(locale_desc.locale_identifier, "en-US");
    locale_desc.number_of_strings = 1;
    locale_desc.base_strings = 0;
    
    // Test dynamic model structures
    AEM::StreamInfo stream_info = {};
    stream_info.stream_info_flags = AEM::StreamInfoFlags::Connected | AEM::StreamInfoFlags::StreamFormatValid;
    stream_info.stream_format = 0x02F000020001ULL;
    stream_info.stream_id = 0x112233445566778AULL;
    
    AEM::AvbInfo avb_info = {};
    avb_info.gptp_grandmaster_id = 0x0011223344556677ULL;
    avb_info.gptp_domain_number = 0;
    avb_info.flags = AEM::AvbInfoFlags::ASCapable | AEM::AvbInfoFlags::GPTPEnabled;
    
    // Test audio mapping
    AEM::AudioMapping audio_map = {};
    audio_map.stream_index = 0;
    audio_map.stream_channel = 0;
    audio_map.cluster_offset = 0;
    audio_map.cluster_channel = 0;
    
    // Test state management
    AEM::AcquireState acquire_state = AEM::AcquireState::NotAcquired;
    AEM::LockState lock_state = AEM::LockState::NotLocked;
    AEM::EnumerationStep enum_step = AEM::EnumerationStep::GetStaticModel;
    
    // Test discovery state
    AEM::DiscoveryState discovery = {};
    discovery.is_online = true;
    discovery.available_index = 1;
    discovery.enumeration_steps = AEM::EnumerationStep::GetStaticModel;
    
    // Test command status
    AEM::AemCommandStatus cmd_status = AEM::AemCommandStatus::Success;
    
    // Test inflight command
    AEM::InflightCommand inflight = {};
    inflight.sequence_id = 1234;
    inflight.retry_count = 0;
    inflight.max_retries = 3;
    inflight.timeout_ms = 5000;
    inflight.target_entity_id = entity_desc.entity_id;
    
    std::cout << "✅ IEEE 1722.1-2021 Core Descriptors compilation test passed!" << std::endl;
    std::cout << "Entity ID: 0x" << std::hex << entity_desc.entity_id << std::endl;
    std::cout << "Audio Unit Ports: " << std::dec << audio_unit.number_of_stream_input_ports 
              << " in, " << audio_unit.number_of_stream_output_ports << " out" << std::endl;
    std::cout << "Stream connected: " << ((stream_info.stream_info_flags & AEM::StreamInfoFlags::Connected) != AEM::StreamInfoFlags::None ? "Yes" : "No") << std::endl;
    std::cout << "Entity online: " << (discovery.is_online ? "Yes" : "No") << std::endl;
    
    // Test new descriptor functionality
    std::cout << "\n🔧 Additional Descriptor Tests:" << std::endl;
    std::cout << "Audio Cluster: " << audio_cluster.channel_count << " channels, " 
              << audio_cluster.path_latency << "ns latency" << std::endl;
    std::cout << "Stream Port In clusters: " << stream_port_in.number_of_clusters << std::endl;
    std::cout << "Stream Port Out clusters: " << stream_port_out.number_of_clusters << std::endl;
    std::cout << "Control type: " << (control_desc.control_type == AEM::CONTROL_TYPE_GAIN ? "Gain Control" : "Other") << std::endl;
    std::cout << "Mixer sources: " << mixer_desc.number_of_sources << " -> " << mixer_desc.number_of_destinations << " destinations" << std::endl;
    std::cout << "Matrix size: " << matrix_desc.width << "x" << matrix_desc.height << std::endl;
    std::cout << "Locale: " << locale_desc.locale_identifier << std::endl;
    
    // Test bitwise operations on new flag types
    AEM::StreamPortFlags port_flags = AEM::StreamPortFlags::ClockSyncSource | AEM::StreamPortFlags::AsyncSampleRateConv;
    bool has_clock_sync = (port_flags & AEM::StreamPortFlags::ClockSyncSource) != AEM::StreamPortFlags::None;
    std::cout << "Port has clock sync: " << (has_clock_sync ? "Yes" : "No") << std::endl;
    
    AEM::MixerFlags mixer_flags = AEM::MixerFlags::LinearGain | AEM::MixerFlags::MatrixControl;
    bool has_matrix_control = (mixer_flags & AEM::MixerFlags::MatrixControl) != AEM::MixerFlags::None;
    std::cout << "Mixer has matrix control: " << (has_matrix_control ? "Yes" : "No") << std::endl;
    
    return 0;
}
