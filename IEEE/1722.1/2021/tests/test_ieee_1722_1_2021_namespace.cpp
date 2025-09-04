/**
 * @file test_ieee_1722_1_2021_namespace.cpp
 * @brief Simple test to verify IEEE 1722.1-2021 namespace resolution
 */

#include "../core/ieee_1722_1_2021_base.h"
#include <iostream>

int main() {
    using namespace IEEE::_1722_1::_2021;
    
    // Test AEM namespace and types
    AEM::DescriptorType type = AEM::DescriptorType::DESCRIPTOR_ENTITY;
    
    // Test EntityCapabilities enum (in AEM namespace)
    uint32_t capabilities = AEM::EntityCapabilities::AEM_SUPPORTED | AEM::EntityCapabilities::CLASS_A_SUPPORTED;
    
    // Test AECP namespace and types  
    AECP::AECPMessageType msg_type = AECP::AECPMessageType::AEM_COMMAND;
    AECP::AEMCommandType cmd = AECP::AEMCommandType::READ_DESCRIPTOR;
    
    std::cout << "IEEE 1722.1-2021 namespace test passed!" << std::endl;
    std::cout << "AEM Descriptor Type: " << static_cast<int>(type) << std::endl;
    std::cout << "AEM Command Type: " << static_cast<int>(cmd) << std::endl;
    std::cout << "Entity Capabilities: 0x" << std::hex << capabilities << std::endl;
    std::cout << "AECP Message Type: " << static_cast<int>(msg_type) << std::endl;
    
    return 0;
}
