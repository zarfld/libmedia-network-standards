/**
 * @file test_standards_build.cpp
 * @brief Simple test to verify Standards libraries build and link correctly
 */

#include <iostream>

// Test IEEE 802.1Q-2020 library
#include "../IEEE/802.1/Q/2020/IEEE_802_1_Q_2020.h"
#include "../IEEE/802.1/Q/2020/qos.h"
#include "../IEEE/802.1/Q/2020/port_profile.h"

using namespace IEEE::_802_1Q::_2020;

int main() {
    std::cout << "🧪 Testing Standards Library Build..." << std::endl;
    
    // Test IEEE 802.1Q-2020 functionality
    try {
        // Test QoS profile
        QoSProfile qos = QoSProfile::default_profile(8);
        uint8_t tc = qos.pcp_to_tc(3);
        std::cout << "✅ IEEE 802.1Q-2020: PCP 3 -> TC " << static_cast<int>(tc) << std::endl;
        
        // Test per-port profiles
        auto ingress_profile = PortProfilesFactory::make_ingress_from_qos(qos);
        auto result = ingress_profile.classify(3, TagTable::CTAG);
        std::cout << "✅ IEEE 802.1Q-2020: Port profiles working" << std::endl;
        
        // Test VLAN utilities
        uint8_t basic_tc = Utils::pcp_to_traffic_class(5, 8);
        std::cout << "✅ IEEE 802.1Q-2020: Utils working, PCP 5 -> TC " << static_cast<int>(basic_tc) << std::endl;
        
        std::cout << "✅ All Standards library tests passed!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Standards library test failed: " << e.what() << std::endl;
        return 1;
    }
}
