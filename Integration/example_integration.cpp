#include "ieee_1722_1_2021_library.h"
#include <iostream>
#include <vector>

using namespace IEEE::_1722_1::_2021;

/**
 * @brief Example ATDECC Entity implementation showing integration with OpenAvnu
 * 
 * This example demonstrates how the IEEE 1722.1-2021 implementation can be
 * integrated into an OpenAvnu-based ATDECC entity.
 */
class ATDECCEntity {
private:
    uint64_t entity_id_;
    uint64_t entity_model_id_;
    std::string entity_name_;
    ADPDU::ATDECCDiscoveryProtocolPDU discovery_pdu_;
    AEM::EntityDescriptor entity_descriptor_;
    
public:
    ATDECCEntity(uint64_t entity_id, uint64_t model_id, const std::string& name) 
        : entity_id_(entity_id), entity_model_id_(model_id), entity_name_(name) {
        initialize();
    }
    
    void initialize() {
        // Configure discovery PDU
        discovery_pdu_.set_entity_id(entity_id_);
        discovery_pdu_.set_entity_model_id(entity_model_id_);
        
        // Set entity capabilities for a typical audio device
        EntityCapabilities caps = static_cast<EntityCapabilities>(
            static_cast<uint32_t>(EntityCapabilities::AEM_SUPPORTED) |
            static_cast<uint32_t>(EntityCapabilities::CLASS_A_SUPPORTED) |
            static_cast<uint32_t>(EntityCapabilities::CLASS_B_SUPPORTED) |
            static_cast<uint32_t>(EntityCapabilities::GPTP_SUPPORTED)
        );
        discovery_pdu_.set_entity_capabilities(caps);
        
        // Configure as audio talker and listener
        TalkerCapabilities talker_caps = static_cast<TalkerCapabilities>(
            static_cast<uint16_t>(TalkerCapabilities::AUDIO_SOURCE) |
            static_cast<uint16_t>(TalkerCapabilities::IMPLEMENTED)
        );
        discovery_pdu_.set_talker_info(8, talker_caps); // 8 output streams
        
        ListenerCapabilities listener_caps = static_cast<ListenerCapabilities>(
            static_cast<uint16_t>(ListenerCapabilities::AUDIO_SINK) |
            static_cast<uint16_t>(ListenerCapabilities::IMPLEMENTED)
        );
        discovery_pdu_.set_listener_info(8, listener_caps); // 8 input streams
        
        // Not a controller
        discovery_pdu_.set_controller_capabilities(ControllerCapabilities::None);
        
        // Configure entity descriptor
        entity_descriptor_.entity_id = entity_id_;
        entity_descriptor_.entity_model_id = entity_model_id_;
        entity_descriptor_.entity_capabilities = caps;
        entity_descriptor_.current_configuration = 0;
        
        AEM::safe_string_copy(entity_descriptor_.entity_name, 
                             entity_name_.c_str(), 
                             sizeof(entity_descriptor_.entity_name));
        AEM::safe_string_copy(entity_descriptor_.firmware_version, 
                             "OpenAvnu-1.0", 
                             sizeof(entity_descriptor_.firmware_version));
        AEM::safe_string_copy(entity_descriptor_.serial_number, 
                             "OA001234", 
                             sizeof(entity_descriptor_.serial_number));
    }
    
    /**
     * @brief Send entity available announcement
     * 
     * In a real implementation, this would send the PDU over the network
     * using the OpenAvnu networking stack.
     */
    void announce_available() {
        discovery_pdu_.set_message_type(ADP_ENTITY_AVAILABLE);
        discovery_pdu_.set_valid_time(62); // 124 seconds (62 * 2)
        discovery_pdu_.serialize();
        
        std::cout << "Announcing entity available:" << std::endl;
        std::cout << "  Entity ID: 0x" << std::hex << entity_id_ << std::endl;
        std::cout << "  Name: " << entity_name_ << std::endl;
        std::cout << "  PDU Size: " << std::dec << discovery_pdu_.get_size() << " bytes" << std::endl;
        
        // In real implementation:
        // openavnu_send_multicast(AVDECC_MULTICAST_MAC, discovery_pdu_.get_raw_octets(), discovery_pdu_.get_size());
    }
    
    /**
     * @brief Send entity departing announcement
     */
    void announce_departing() {
        discovery_pdu_.set_message_type(ADP_ENTITY_DEPARTING);
        discovery_pdu_.set_valid_time(0); // Departing messages have valid_time = 0
        discovery_pdu_.serialize();
        
        std::cout << "Announcing entity departing:" << std::endl;
        std::cout << "  Entity ID: 0x" << std::hex << entity_id_ << std::endl;
        
        // In real implementation:
        // openavnu_send_multicast(AVDECC_MULTICAST_MAC, discovery_pdu_.get_raw_octets(), discovery_pdu_.get_size());
    }
    
    /**
     * @brief Handle discovery request
     */
    void handle_discovery_request() {
        std::cout << "Received discovery request - responding with entity available" << std::endl;
        announce_available();
    }
    
    /**
     * @brief Get entity descriptor for AEM read operations
     */
    const AEM::EntityDescriptor& get_entity_descriptor() const {
        return entity_descriptor_;
    }
    
    /**
     * @brief Simulate handling an incoming ADPDU
     */
    static void handle_incoming_adpdu(const uint8_t* data, size_t length) {
        ADPDU::ATDECCDiscoveryProtocolPDU incoming_pdu(data, length);
        
        if (!incoming_pdu.is_valid()) {
            std::cout << "Received invalid ADPDU" << std::endl;
            return;
        }
        
        std::cout << "Received ADPDU:" << std::endl;
        std::cout << "  Message Type: " << static_cast<int>(incoming_pdu.message_type) << std::endl;
        std::cout << "  Entity ID: 0x" << std::hex << incoming_pdu.entity_id << std::endl;
        
        switch (incoming_pdu.message_type) {
            case ADP_ENTITY_AVAILABLE:
                std::cout << "  -> Entity Available from 0x" << std::hex << incoming_pdu.entity_id << std::endl;
                break;
            case ADP_ENTITY_DEPARTING:
                std::cout << "  -> Entity Departing: 0x" << std::hex << incoming_pdu.entity_id << std::endl;
                break;
            case ADP_ENTITY_DISCOVERY_REQUEST:
                std::cout << "  -> Discovery Request received" << std::endl;
                break;
        }
    }
};

/**
 * @brief Example ATDECC Controller implementation
 */
class ATDECCController {
private:
    std::vector<uint64_t> discovered_entities_;
    
public:
    ATDECCController() {
        std::cout << "ATDECC Controller started" << std::endl;
    }
    
    /**
     * @brief Send discovery request to find all entities
     */
    void send_discovery_request() {
        ADPDU::ATDECCDiscoveryProtocolPDU discovery_pdu;
        
        discovery_pdu.set_entity_id(0); // 0 means "discover all entities"
        discovery_pdu.set_message_type(ADP_ENTITY_DISCOVERY_REQUEST);
        discovery_pdu.set_controller_capabilities(ControllerCapabilities::IMPLEMENTED);
        discovery_pdu.serialize();
        
        std::cout << "Sending discovery request to find all entities" << std::endl;
        
        // In real implementation:
        // openavnu_send_multicast(AVDECC_MULTICAST_MAC, discovery_pdu.get_raw_octets(), discovery_pdu.get_size());
    }
    
    void add_discovered_entity(uint64_t entity_id) {
        discovered_entities_.push_back(entity_id);
        std::cout << "Added entity 0x" << std::hex << entity_id << " to discovered list" << std::endl;
    }
    
    void show_discovered_entities() const {
        std::cout << "Discovered entities (" << discovered_entities_.size() << "):" << std::endl;
        for (uint64_t entity_id : discovered_entities_) {
            std::cout << "  - 0x" << std::hex << entity_id << std::endl;
        }
    }
};

/**
 * @brief Demonstration of ATDECC protocol interaction
 */
int main() {
    std::cout << "=== OpenAvnu ATDECC Integration Example ===" << std::endl << std::endl;
    
    // Create some ATDECC entities
    ATDECCEntity audio_interface(0x0001020304050607ULL, 0x1001020304050607ULL, "OpenAvnu Audio Interface");
    ATDECCEntity mixer(0x0008090A0B0C0D0EULL, 0x1008090A0B0C0D0EULL, "OpenAvnu Digital Mixer");
    
    // Create a controller
    ATDECCController controller;
    
    std::cout << "=== Scenario 1: Entity Startup ===" << std::endl;
    audio_interface.announce_available();
    mixer.announce_available();
    std::cout << std::endl;
    
    std::cout << "=== Scenario 2: Controller Discovery ===" << std::endl;
    controller.send_discovery_request();
    
    // Entities would respond to discovery request
    audio_interface.handle_discovery_request();
    mixer.handle_discovery_request();
    
    // Controller would receive these responses
    controller.add_discovered_entity(0x0001020304050607ULL);
    controller.add_discovered_entity(0x0008090A0B0C0D0EULL);
    controller.show_discovered_entities();
    std::cout << std::endl;
    
    std::cout << "=== Scenario 3: Entity Information ===" << std::endl;
    const auto& audio_desc = audio_interface.get_entity_descriptor();
    std::cout << "Audio Interface Details:" << std::endl;
    std::cout << "  Name: " << audio_desc.entity_name << std::endl;
    std::cout << "  Firmware: " << audio_desc.firmware_version << std::endl;
    std::cout << "  Serial: " << audio_desc.serial_number << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Scenario 4: Simulated Network Traffic ===" << std::endl;
    
    // Simulate receiving an entity available announcement
    ADPDU::ATDECCDiscoveryProtocolPDU test_pdu;
    test_pdu.set_entity_id(0xAABBCCDDEEFF0011ULL);
    test_pdu.set_entity_model_id(0x1122334455667788ULL);
    test_pdu.set_message_type(ADP_ENTITY_AVAILABLE);
    test_pdu.serialize();
    
    ATDECCEntity::handle_incoming_adpdu(test_pdu.get_raw_octets(), test_pdu.get_size());
    std::cout << std::endl;
    
    std::cout << "=== Scenario 5: Entity Shutdown ===" << std::endl;
    audio_interface.announce_departing();
    mixer.announce_departing();
    std::cout << std::endl;
    
    std::cout << "=== Integration Points with OpenAvnu ===" << std::endl;
    std::cout << "✓ Network Layer: Use OpenAvnu's AVB networking stack" << std::endl;
    std::cout << "✓ Timing: Integrate with gPTP time synchronization" << std::endl;
    std::cout << "✓ Stream Management: Connect to IEEE 1722 streaming" << std::endl;
    std::cout << "✓ QoS: Leverage IEEE 802.1Q traffic shaping" << std::endl;
    std::cout << "✓ Discovery: Use multicast for entity discovery" << std::endl;
    std::cout << "✓ Control: Implement full AECP for device control" << std::endl;
    
    return 0;
}
