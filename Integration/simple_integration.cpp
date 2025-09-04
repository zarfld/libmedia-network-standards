/**
 * @file simple_integration.cpp
 * @brief Simple IEEE Standards Integration Example
 * 
 * This simplified integration example demonstrates basic IEEE 802.1Q-2020 
 * functionality without the complex 1722.1 library dependencies.
 */

#include "../IEEE/802.1/Q/2020/IEEE_802_1_Q_2020.h"
#include "../IEEE/802.1/Q/2020/qos.h"
#include "../IEEE/802.1/Q/2020/port_profile.h"
#include <iostream>
#include <vector>

using namespace IEEE::_802_1Q::_2020;

/**
 * @brief Simple integration test showing IEEE 802.1Q-2020 functionality
 */
void test_simple_integration() {
    std::cout << "🔧 Testing IEEE 802.1Q-2020 Simple Integration..." << std::endl;
    
    // Test basic VLAN parsing
    std::vector<uint8_t> sample_frame = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB,  // Destination MAC
        0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67,  // Source MAC
        0x81, 0x00,                           // VLAN TPID
        0x20, 0x64,                           // PCP=1, DEI=0, VID=100
        0x08, 0x00                            // EtherType (IPv4)
    };
    
    ParsedFrame frame;
    ParseError result = Utils::parse_ethernet_with_vlan(sample_frame.data(), sample_frame.size(), frame);
    if (result == ParseError::Ok && !frame.vlan_stack.empty()) {
        const auto& vlan_tag = frame.vlan_stack[0].tci;
        std::cout << "✅ VLAN Tag parsed: VID=" << vlan_tag.vid 
                  << ", PCP=" << static_cast<int>(vlan_tag.pcp) << std::endl;
    }
    
    // Test QoS profile
    QoSProfile qos = QoSProfile::default_profile(8);
    uint8_t tc = qos.pcp_to_tc(3);
    std::cout << "✅ PCP 3 -> TC " << static_cast<int>(tc) << std::endl;
    
    // Test per-port profiles
    auto ingress_profile = PortProfilesFactory::make_ingress_from_qos(qos);
    auto result_pair = ingress_profile.classify(3, TagTable::CTAG);
    uint8_t pcp_regen = result_pair.first;
    uint8_t traffic_class = result_pair.second;
    std::cout << "✅ Port ingress: PCP 3 -> Regen PCP " << static_cast<int>(pcp_regen) 
              << ", TC " << static_cast<int>(traffic_class) << std::endl;
    
    auto egress_profile = PortProfilesFactory::make_egress_from_qos(qos);
    TagHeader egress_tag = egress_profile.make_egress_header(2, TagTable::STAG, 0);
    std::cout << "✅ Port egress: TC 2 -> STAG TPID=0x" << std::hex << egress_tag.tpid 
              << ", PCP=" << std::dec << static_cast<int>(egress_tag.tci.pcp) << std::endl;
}

/**
 * @brief Main integration test function
 */
int main() {
    std::cout << "🚀 IEEE Standards Simple Integration Test" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    try {
        test_simple_integration();
        std::cout << "✅ All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
