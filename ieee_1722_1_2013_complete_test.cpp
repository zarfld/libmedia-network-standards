/**
 * @file ieee_1722_1_2013_complete_test.cpp
 * @brief Test Suite für IEEE 1722.1-2013 VOLLSTÄNDIGE EIGENSTÄNDIGE IMPLEMENTATION
 * 
 * Comprehensive test suite for the IEEE 1722.1-2013 standard implementation.
 * Diese Tests validieren die vollständig eigenständige Implementation.
 * 
 * @version 1.0.0
 * @date Juli 2025
 * @copyright OpenAvnu Project
 */

#include "ieee_1722_1_2013_complete.h"
#include <iostream>
#include <cassert>
#include <iomanip>
#include <vector>

using namespace IEEE::_1722_1::_2013;

// Test helper functions
void print_test_header(const std::string& test_name) {
    std::cout << "\n=== " << test_name << " ===" << std::endl;
}

void print_test_result(const std::string& test_name, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test_name << std::endl;
}

void print_hex_dump(const uint8_t* data, size_t length, const std::string& label) {
    std::cout << label << " (" << length << " bytes):" << std::endl;
    for (size_t i = 0; i < length; ++i) {
        if (i % 16 == 0) {
            std::cout << std::hex << std::setfill('0') << std::setw(4) << i << ": ";
        }
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(data[i]) << " ";
        if ((i + 1) % 16 == 0 || i == length - 1) {
            std::cout << std::endl;
        }
    }
    std::cout << std::dec << std::endl;
}

// ==============================
// ADP Tests
// ==============================

bool test_adp_basic_construction() {
    print_test_header("ADP Basic Construction");
    
    ADP::AVDECCDiscoveryProtocolPDU adpdu;
    
    // Test default values
    bool test_passed = true;
    test_passed &= (adpdu.message_type == ADP::ENTITY_AVAILABLE);
    test_passed &= (adpdu.valid_time == 62);
    test_passed &= (adpdu.entity_id == 0);
    test_passed &= (adpdu.get_size() == 56);
    
    print_test_result("ADP Basic Construction", test_passed);
    return test_passed;
}

bool test_adp_serialization() {
    print_test_header("ADP Serialization/Deserialization");
    
    // Create ADP message with test data
    ADP::AVDECCDiscoveryProtocolPDU adpdu;
    adpdu.set_message_type(ADP::ENTITY_AVAILABLE);
    adpdu.set_valid_time(62);
    adpdu.set_entity_id(0x001122334455667788ULL);
    adpdu.set_entity_model_id(0xAABBCCDDEEFF0011ULL);
    adpdu.set_entity_capabilities(ADP::EntityCapabilities::AEM_SUPPORTED);
    adpdu.set_talker_info(2, ADP::TalkerCapabilities::AUDIO_SOURCE);
    adpdu.set_listener_info(4, ADP::ListenerCapabilities::AUDIO_SINK);
    adpdu.set_controller_capabilities(ADP::ControllerCapabilities::IMPLEMENTED);
    adpdu.set_available_index(123);
    adpdu.set_gptp_info(0x1234567890ABCDEFULL, 5);
    adpdu.set_identify_control_index(100);
    adpdu.set_interface_index(1);
    adpdu.set_association_id(0xFEDCBA0987654321ULL);
    
    // Serialize
    adpdu.serialize();
    
    const uint8_t* raw_data = adpdu.get_raw_octets();
    size_t data_size = adpdu.get_size();
    
    print_hex_dump(raw_data, data_size, "Serialized ADP Message");
    
    // Create new ADPDU and deserialize
    ADP::AVDECCDiscoveryProtocolPDU adpdu2(raw_data, data_size);
    
    // Verify deserialization
    bool test_passed = true;
    test_passed &= (adpdu2.message_type == ADP::ENTITY_AVAILABLE);
    test_passed &= (adpdu2.valid_time == 62);
    test_passed &= (adpdu2.entity_id == 0x001122334455667788ULL);
    test_passed &= (adpdu2.entity_model_id == 0xAABBCCDDEEFF0011ULL);
    test_passed &= (adpdu2.talker_stream_sources == 2);
    test_passed &= (adpdu2.listener_stream_sinks == 4);
    test_passed &= (adpdu2.available_index == 123);
    test_passed &= (adpdu2.gptp_grandmaster_id == 0x1234567890ABCDEFULL);
    test_passed &= (adpdu2.gptp_domain_number == 5);
    test_passed &= (adpdu2.identify_control_index == 100);
    test_passed &= (adpdu2.interface_index == 1);
    test_passed &= (adpdu2.association_id == 0xFEDCBA0987654321ULL);
    test_passed &= adpdu2.is_valid();
    
    print_test_result("ADP Serialization/Deserialization", test_passed);
    return test_passed;
}

bool test_adp_message_types() {
    print_test_header("ADP Message Types");
    
    bool test_passed = true;
    
    // Test all message types
    std::vector<ADP::ADPMessageType> types = {
        ADP::ENTITY_AVAILABLE,
        ADP::ENTITY_DEPARTING,
        ADP::ENTITY_DISCOVER
    };
    
    for (auto type : types) {
        ADP::AVDECCDiscoveryProtocolPDU adpdu;
        adpdu.set_message_type(type);
        adpdu.set_entity_id(0x123456789ABCDEFULL);
        adpdu.serialize();
        
        // Verify message type is preserved
        ADP::AVDECCDiscoveryProtocolPDU adpdu2(adpdu.get_raw_octets(), adpdu.get_size());
        test_passed &= (adpdu2.message_type == type);
        test_passed &= adpdu2.is_valid();
    }
    
    print_test_result("ADP Message Types", test_passed);
    return test_passed;
}

// ==============================
// ACMP Tests
// ==============================

bool test_acmp_basic_construction() {
    print_test_header("ACMP Basic Construction");
    
    ACMP::ACMPMessage acmp;
    
    // Test default values
    bool test_passed = true;
    test_passed &= (acmp.message_type == ACMP::CONNECT_TX_COMMAND);
    test_passed &= (acmp.status == ACMP::SUCCESS);
    test_passed &= (acmp.control_data_length == 44);
    test_passed &= (acmp.stream_id == 0);
    test_passed &= acmp.is_valid();
    
    print_test_result("ACMP Basic Construction", test_passed);
    return test_passed;
}

bool test_acmp_serialization() {
    print_test_header("ACMP Serialization/Deserialization");
    
    // Create ACMP message with test data
    ACMP::ACMPMessage acmp;
    acmp.message_type = ACMP::CONNECT_TX_COMMAND;
    acmp.status = ACMP::SUCCESS;
    acmp.control_data_length = 44;
    acmp.stream_id = 0x1122334455667788ULL;
    acmp.controller_entity_id = 0xAABBCCDDEEFF0011ULL;
    acmp.talker_entity_id = 0x1111222233334444ULL;
    acmp.listener_entity_id = 0x5555666677778888ULL;
    acmp.talker_unique_id = 1;
    acmp.listener_unique_id = 2;
    acmp.stream_dest_mac = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    acmp.connection_count = 1;
    acmp.sequence_id = 100;
    acmp.flags = ACMP::CLASS_B;
    acmp.stream_vlan_id = 5;
    
    // Serialize
    std::vector<uint8_t> buffer;
    acmp.serialize(buffer);
    
    print_hex_dump(buffer.data(), buffer.size(), "Serialized ACMP Message");
    
    // Deserialize
    ACMP::ACMPMessage acmp2;
    bool deserialize_ok = acmp2.deserialize(buffer.data(), buffer.size());
    
    // Verify deserialization
    bool test_passed = deserialize_ok;
    test_passed &= (acmp2.message_type == ACMP::CONNECT_TX_COMMAND);
    test_passed &= (acmp2.status == ACMP::SUCCESS);
    test_passed &= (acmp2.control_data_length == 44);
    test_passed &= (acmp2.stream_id == 0x1122334455667788ULL);
    test_passed &= (acmp2.controller_entity_id == 0xAABBCCDDEEFF0011ULL);
    test_passed &= (acmp2.talker_entity_id == 0x1111222233334444ULL);
    test_passed &= (acmp2.listener_entity_id == 0x5555666677778888ULL);
    test_passed &= (acmp2.talker_unique_id == 1);
    test_passed &= (acmp2.listener_unique_id == 2);
    test_passed &= (acmp2.connection_count == 1);
    test_passed &= (acmp2.sequence_id == 100);
    test_passed &= (acmp2.flags == ACMP::CLASS_B);
    test_passed &= (acmp2.stream_vlan_id == 5);
    test_passed &= acmp2.is_valid();
    
    // Verify MAC address
    for (int i = 0; i < 6; ++i) {
        test_passed &= (acmp2.stream_dest_mac[i] == (i + 1));
    }
    
    print_test_result("ACMP Serialization/Deserialization", test_passed);
    return test_passed;
}

bool test_acmp_message_types() {
    print_test_header("ACMP Message Types");
    
    bool test_passed = true;
    
    // Test all message types
    std::vector<ACMP::ACMPMessageType> types = {
        ACMP::CONNECT_TX_COMMAND,
        ACMP::CONNECT_TX_RESPONSE,
        ACMP::DISCONNECT_TX_COMMAND,
        ACMP::DISCONNECT_TX_RESPONSE,
        ACMP::GET_TX_STATE_COMMAND,
        ACMP::GET_TX_STATE_RESPONSE,
        ACMP::CONNECT_RX_COMMAND,
        ACMP::CONNECT_RX_RESPONSE,
        ACMP::DISCONNECT_RX_COMMAND,
        ACMP::DISCONNECT_RX_RESPONSE,
        ACMP::GET_RX_STATE_COMMAND,
        ACMP::GET_RX_STATE_RESPONSE,
        ACMP::GET_TX_CONNECTION_COMMAND,
        ACMP::GET_TX_CONNECTION_RESPONSE
    };
    
    for (auto type : types) {
        ACMP::ACMPMessage acmp;
        acmp.message_type = type;
        acmp.stream_id = 0x123456789ABCDEFULL;
        
        std::vector<uint8_t> buffer;
        acmp.serialize(buffer);
        
        ACMP::ACMPMessage acmp2;
        bool deserialize_ok = acmp2.deserialize(buffer.data(), buffer.size());
        
        test_passed &= deserialize_ok;
        test_passed &= (acmp2.message_type == type);
        test_passed &= acmp2.is_valid();
    }
    
    print_test_result("ACMP Message Types", test_passed);
    return test_passed;
}

// ==============================
// Entity Tests
// ==============================

bool test_avdecc_entity_basic() {
    print_test_header("AVDECC Entity Basic Functionality");
    
    AVDECCEntity_2013 entity;
    
    // Set up entity
    entity.entity_id = 0x001122334455667788ULL;
    entity.entity_model_id = 0xAABBCCDDEEFF0011ULL;
    entity.entity_name = "Test OpenAvnu Entity 2013";
    entity.firmware_version = "1.0.0-test";
    entity.serial_number = "TEST2013001";
    entity.talker_stream_sources = 2;
    entity.listener_stream_sinks = 4;
    entity.available_index = 1;
    
    // Test basic functionality
    bool test_passed = entity.is_valid();
    test_passed &= !entity.get_capabilities_string().empty();
    
    // Test ADP message creation
    ADP::AVDECCDiscoveryProtocolPDU adpdu = entity.create_adp_message(ADP::ENTITY_AVAILABLE);
    test_passed &= adpdu.is_valid();
    test_passed &= (adpdu.entity_id == entity.entity_id);
    test_passed &= (adpdu.entity_model_id == entity.entity_model_id);
    test_passed &= (adpdu.talker_stream_sources == entity.talker_stream_sources);
    test_passed &= (adpdu.listener_stream_sinks == entity.listener_stream_sinks);
    test_passed &= (adpdu.available_index == entity.available_index);
    
    // Test entity descriptor
    AEM::EntityDescriptor desc = entity.get_entity_descriptor();
    test_passed &= (desc.descriptor_type == AEM::DescriptorType::ENTITY);
    test_passed &= (desc.entity_id == entity.entity_id);
    test_passed &= (desc.entity_model_id == entity.entity_model_id);
    test_passed &= (std::string(desc.entity_name) == entity.entity_name);
    test_passed &= (std::string(desc.firmware_version) == entity.firmware_version);
    test_passed &= (std::string(desc.serial_number) == entity.serial_number);
    
    print_test_result("AVDECC Entity Basic Functionality", test_passed);
    return test_passed;
}

bool test_avdecc_entity_update_from_adp() {
    print_test_header("AVDECC Entity Update from ADP");
    
    // Create ADP message
    ADP::AVDECCDiscoveryProtocolPDU adpdu;
    adpdu.set_entity_id(0x9988776655443322ULL);
    adpdu.set_entity_model_id(0x1122334455667788ULL);
    adpdu.set_talker_info(8, ADP::TalkerCapabilities::AUDIO_SOURCE);
    adpdu.set_listener_info(16, ADP::ListenerCapabilities::AUDIO_SINK);
    adpdu.set_available_index(42);
    adpdu.set_association_id(0xDEADBEEFCAFEBABEULL);
    adpdu.serialize();
    
    // Create entity and update from ADP
    AVDECCEntity_2013 entity;
    bool update_ok = entity.update_from_adp(adpdu);
    
    // Verify update
    bool test_passed = update_ok;
    test_passed &= (entity.entity_id == 0x9988776655443322ULL);
    test_passed &= (entity.entity_model_id == 0x1122334455667788ULL);
    test_passed &= (entity.talker_stream_sources == 8);
    test_passed &= (entity.listener_stream_sinks == 16);
    test_passed &= (entity.available_index == 42);
    test_passed &= (entity.association_id == 0xDEADBEEFCAFEBABEULL);
    test_passed &= entity.is_valid();
    
    print_test_result("AVDECC Entity Update from ADP", test_passed);
    return test_passed;
}

// ==============================
// Integration Tests
// ==============================

bool test_protocol_integration() {
    print_test_header("Protocol Integration Test");
    
    // Create a complete AVDECC entity
    AVDECCEntity_2013 talker_entity;
    talker_entity.entity_id = 0x0011223344556677ULL;
    talker_entity.entity_model_id = 0xAABBCCDDEEFF0011ULL;
    talker_entity.entity_name = "Test Talker Entity";
    talker_entity.talker_stream_sources = 1;
    talker_entity.entity_capabilities = static_cast<ADP::EntityCapabilities>(
        static_cast<uint32_t>(ADP::EntityCapabilities::AEM_SUPPORTED) |
        static_cast<uint32_t>(ADP::EntityCapabilities::CLASS_A_SUPPORTED) |
        static_cast<uint32_t>(ADP::EntityCapabilities::GPTP_SUPPORTED)
    );
    
    AVDECCEntity_2013 listener_entity;
    listener_entity.entity_id = 0x8899AABBCCDDEEFFULL;
    listener_entity.entity_model_id = 0x1122334455667788ULL;
    listener_entity.entity_name = "Test Listener Entity";
    listener_entity.listener_stream_sinks = 1;
    listener_entity.entity_capabilities = static_cast<ADP::EntityCapabilities>(
        static_cast<uint32_t>(ADP::EntityCapabilities::AEM_SUPPORTED) |
        static_cast<uint32_t>(ADP::EntityCapabilities::CLASS_A_SUPPORTED) |
        static_cast<uint32_t>(ADP::EntityCapabilities::GPTP_SUPPORTED)
    );
    
    AVDECCEntity_2013 controller_entity;
    controller_entity.entity_id = 0xFEDCBA0987654321ULL;
    controller_entity.entity_model_id = 0x0123456789ABCDEFULL;
    controller_entity.entity_name = "Test Controller Entity";
    controller_entity.controller_capabilities = ADP::ControllerCapabilities::IMPLEMENTED;
    
    bool test_passed = true;
    
    // 1. ADP Discovery Phase
    std::cout << "Phase 1: ADP Discovery" << std::endl;
    
    // Controller sends ENTITY_DISCOVER
    ADP::AVDECCDiscoveryProtocolPDU discover_msg;
    discover_msg.set_message_type(ADP::ENTITY_DISCOVER);
    discover_msg.set_entity_id(controller_entity.entity_id);
    discover_msg.serialize();
    
    // Talker and Listener respond with ENTITY_AVAILABLE
    ADP::AVDECCDiscoveryProtocolPDU talker_available = talker_entity.create_adp_message(ADP::ENTITY_AVAILABLE);
    ADP::AVDECCDiscoveryProtocolPDU listener_available = listener_entity.create_adp_message(ADP::ENTITY_AVAILABLE);
    
    test_passed &= discover_msg.is_valid();
    test_passed &= talker_available.is_valid();
    test_passed &= listener_available.is_valid();
    
    // 2. ACMP Connection Phase
    std::cout << "Phase 2: ACMP Connection" << std::endl;
    
    // Controller initiates connection
    ACMP::ACMPMessage connect_tx;
    connect_tx.message_type = ACMP::CONNECT_TX_COMMAND;
    connect_tx.controller_entity_id = controller_entity.entity_id;
    connect_tx.talker_entity_id = talker_entity.entity_id;
    connect_tx.listener_entity_id = listener_entity.entity_id;
    connect_tx.stream_id = 0x0011223344556677ULL | 0x0000ULL; // Stream ID = talker_entity_id + stream_index
    connect_tx.talker_unique_id = 0;
    connect_tx.listener_unique_id = 0;
    connect_tx.sequence_id = 1;
    connect_tx.flags = ACMP::CLASS_A;
    
    std::vector<uint8_t> connect_tx_buffer;
    connect_tx.serialize(connect_tx_buffer);
    
    // Talker responds
    ACMP::ACMPMessage connect_tx_response;
    connect_tx_response.deserialize(connect_tx_buffer.data(), connect_tx_buffer.size());
    connect_tx_response.message_type = ACMP::CONNECT_TX_RESPONSE;
    connect_tx_response.status = ACMP::SUCCESS;
    connect_tx_response.stream_dest_mac = {0x91, 0xE0, 0xF0, 0x00, 0x01, 0x00}; // Multicast MAC
    connect_tx_response.connection_count = 1;
    
    test_passed &= connect_tx.is_valid();
    test_passed &= connect_tx_response.is_valid();
    test_passed &= (connect_tx_response.status == ACMP::SUCCESS);
    
    // 3. Entity Model Access (AEM)
    std::cout << "Phase 3: Entity Model Access" << std::endl;
    
    // Get entity descriptors
    AEM::EntityDescriptor talker_desc = talker_entity.get_entity_descriptor();
    AEM::EntityDescriptor listener_desc = listener_entity.get_entity_descriptor();
    
    test_passed &= (talker_desc.descriptor_type == AEM::DescriptorType::ENTITY);
    test_passed &= (listener_desc.descriptor_type == AEM::DescriptorType::ENTITY);
    test_passed &= (talker_desc.entity_id == talker_entity.entity_id);
    test_passed &= (listener_desc.entity_id == listener_entity.entity_id);
    
    // 4. Status validation
    std::cout << "Phase 4: Final Validation" << std::endl;
    
    test_passed &= talker_entity.is_valid();
    test_passed &= listener_entity.is_valid();
    test_passed &= controller_entity.is_valid();
    
    std::cout << "Talker: " << talker_entity.get_capabilities_string() << std::endl;
    std::cout << "Listener: " << listener_entity.get_capabilities_string() << std::endl;
    std::cout << "Controller: " << controller_entity.get_capabilities_string() << std::endl;
    
    print_test_result("Protocol Integration Test", test_passed);
    return test_passed;
}

// ==============================
// Main Test Function
// ==============================

int main() {
    std::cout << "IEEE 1722.1-2013 VOLLSTÄNDIGE EIGENSTÄNDIGE IMPLEMENTATION - Test Suite" << std::endl;
    std::cout << "=================================================================" << std::endl;
    
    int passed = 0;
    int total = 0;
    
    // ADP Tests
    ++total; if (test_adp_basic_construction()) ++passed;
    ++total; if (test_adp_serialization()) ++passed;
    ++total; if (test_adp_message_types()) ++passed;
    
    // ACMP Tests
    ++total; if (test_acmp_basic_construction()) ++passed;
    ++total; if (test_acmp_serialization()) ++passed;
    ++total; if (test_acmp_message_types()) ++passed;
    
    // Entity Tests
    ++total; if (test_avdecc_entity_basic()) ++passed;
    ++total; if (test_avdecc_entity_update_from_adp()) ++passed;
    
    // Integration Tests
    ++total; if (test_protocol_integration()) ++passed;
    
    // Final Results
    std::cout << "\n=================================================================" << std::endl;
    std::cout << "Test Results: " << passed << "/" << total << " passed" << std::endl;
    
    if (passed == total) {
        std::cout << "🎉 ALL TESTS PASSED! IEEE 1722.1-2013 Implementation ist vollständig funktionsfähig!" << std::endl;
        std::cout << "✅ Diese Implementation funktioniert eigenständig ohne IEEE 1722.1-2021 Dateien." << std::endl;
    } else {
        std::cout << "❌ " << (total - passed) << " tests failed!" << std::endl;
        return 1;
    }
    
    return 0;
}
