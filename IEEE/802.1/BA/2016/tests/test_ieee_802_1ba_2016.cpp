#include <cassert>
#include <iostream>
#include "../IEEE_802_1_BA_21016.h"

using namespace IEEE::_802_1BA::_2016;

int main() {
    // Defaults by profile
    auto def = GetDefaults(Profile::DefaultAVB);
    assert(def.pcp_class_a == 3);
    assert(def.pcp_class_b == 2);

    // Defaults by class
    assert(DefaultPCP(SRClass::ClassA) == 3);
    assert(DefaultPCP(SRClass::ClassB) == 2);
    assert(DefaultMaxLatencyMs(SRClass::ClassA) == 2);
    assert(DefaultMaxLatencyMs(SRClass::ClassB) == 50);

    // Compliance check
    StreamParameters p{};
    p.vlan_id = 2;
    p.sr_class = SRClass::ClassA;
    p.pcp = 3; // matches class A
    p.max_transit_time_ms = 2; // within budget

    assert(IsCompliant(p, def));

    p.pcp = 2; // wrong pcp for Class A
    assert(!IsCompliant(p, def));

    std::cout << "IEEE 802.1BA-2016 basic tests passed (" << ToString(Profile::DefaultAVB)
              << ", " << ToString(SRClass::ClassA) << ")\n";
    return 0;
}
