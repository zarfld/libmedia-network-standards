/**
 * @file test_core_descriptors.cpp
 * @brief Test compilation of IEEE 1722.1-2021 core descriptors
 */

#include "ieee_1722_1_2021_core.h"
#include <iostream>

int main() {
    using namespace IEEE::_1722_1::_2021;
    
    // Test EntityDescriptor
    AEM::EntityDescriptor entity_desc;
    entity_desc.entity_id = 0x0001020304050607ULL;
    entity_desc.entity_model_id = 0x00112233445566778ULL;
    entity_desc.entity_capabilities = AEM::EntityCapabilities::AEM_SUPPORTED | AEM::EntityCapabilities::CLASS_A_SUPPORTED;
    
    // Test ConfigurationDescriptor
    AEM::ConfigurationDescriptor config_desc;
    config_desc.object_name = "Test Configuration";
    
    // Test AudioUnitDescriptor  
    AEM::AudioUnitDescriptor audio_unit;
    audio_unit.object_name = "Test Audio Unit";
    audio_unit.clock_domain_index = 0;
    audio_unit.number_of_stream_input_ports = 1;
    audio_unit.number_of_stream_output_ports = 1;
    
    // Test StreamInputDescriptor
    AEM::StreamInputDescriptor stream_in;
    stream_in.object_name = "Test Stream Input";
    stream_in.clock_domain_index = 0;
    
    // Test StreamOutputDescriptor
    AEM::StreamOutputDescriptor stream_out;
    stream_out.object_name = "Test Stream Output";
    stream_out.clock_domain_index = 0;
    
    // Test AvbInterfaceDescriptor
    AEM::AvbInterfaceDescriptor avb_iface;
    avb_iface.object_name = "Test AVB Interface";
    avb_iface.mac_address = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    avb_iface.interface_flags = 0;
    avb_iface.clock_identity = 0x0011223344556677ULL;
    avb_iface.priority1 = 248;
    avb_iface.clock_class = 248;
    avb_iface.priority2 = 248;
    avb_iface.domain_number = 0;
    
    // Test ClockSourceDescriptor
    AEM::ClockSourceDescriptor clock_src;
    clock_src.object_name = "Test Clock Source";
    clock_src.clock_source_type = 0;
    
    // Test ClockDomainDescriptor
    AEM::ClockDomainDescriptor clock_domain;
    clock_domain.object_name = "Test Clock Domain";
    clock_domain.current_clock_source = 0;
    
    // Test StreamPortInputDescriptor
    AEM::StreamPortInputDescriptor stream_port_in;
    stream_port_in.clock_domain_index = 0;
    stream_port_in.port_flags = 0;
    stream_port_in.number_of_controls = 0;
    stream_port_in.base_control = 0;
    stream_port_in.number_of_clusters = 1;
    stream_port_in.base_cluster = 0;
    stream_port_in.number_of_maps = 1;
    stream_port_in.base_map = 0;
    
    std::cout << "✅ All IEEE 1722.1-2021 core descriptors compiled successfully!" << std::endl;
    std::cout << "Entity ID: 0x" << std::hex << entity_desc.entity_id << std::endl;
    std::cout << "Entity Model ID: 0x" << std::hex << entity_desc.entity_model_id << std::endl;
    std::cout << "Configuration: " << config_desc.object_name << std::endl;
    std::cout << "Audio Unit: " << audio_unit.object_name << std::endl;
    std::cout << "Stream Input: " << stream_in.object_name << std::endl;
    std::cout << "Stream Output: " << stream_out.object_name << std::endl;
    std::cout << "AVB Interface: " << avb_iface.object_name << std::endl;
    std::cout << "Clock Source: " << clock_src.object_name << std::endl;
    std::cout << "Clock Domain: " << clock_domain.object_name << std::endl;
    
    return 0;
}
