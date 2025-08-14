#include <cassert>
#include <vector>
#include <cstdint>
#include <iostream>

#include "IEEE_802_1_Q_2020.h"
#include "qos.h"
#include "filtering.h"
#include "port_profile.h"
#include "fdb.h"

using namespace IEEE::_802_1Q::_2020;

int main() {
    // Validate TCI pack/unpack
    VLANTag tag{}; tag.pcp = 5; tag.dei = 1; tag.vid = 123;
    assert(tag.is_valid());
    uint16_t tci = tag.pack_host_order();
    VLANTag back = VLANTag::unpack_host_order(tci);
    assert(back.pcp == 5 && back.dei == 1 && back.vid == 123);

    // Build single-tagged Ethernet header
    MacAddress dst = MacAddress::broadcast();
    MacAddress src{}; src.bytes = {0x00,0x11,0x22,0x33,0x44,0x55};

    TagHeader th{}; th.tpid = (uint16_t)EtherType::VLAN_TAGGED_CTAG; th.tci = tag;
    std::vector<TagHeader> stack{th};
    auto hdr = Utils::build_ethernet_header(dst, src, stack, (uint16_t)EtherType::IPv4);
    assert(hdr.size() == 14 + 4); // DST(6) SRC(6) TAG(4) EtherType(2) => 18? No, tag already includes TPID+TCI => 6+6+4+2 = 18

    // Parse it back
    ParsedFrame pf{};
    auto payload = std::vector<uint8_t>{0,0,0,0};
    std::vector<uint8_t> frame = hdr;
    frame.insert(frame.end(), payload.begin(), payload.end());

    auto err = Utils::parse_ethernet_with_vlan(frame.data(), frame.size(), pf);
    (void)err; // silence unused in Release
    assert(err == ParseError::Ok);
    assert(pf.vlan_stack.size() == 1);
    assert(pf.vlan_stack[0].tci.pcp == 5);
    assert(pf.vlan_stack[0].tci.dei == 1);
    assert(pf.vlan_stack[0].tci.vid == 123);

    // PCP->TC sanity
    assert(Utils::pcp_to_traffic_class(7, 8) == 7);
    assert(Utils::pcp_to_traffic_class(0, 8) == 0);

    // QoSProfile basic mapping
    QoSProfile qos{4};
    PcpToTcMap map{}; map.map = {0,0,1,1,2,2,3,3};
    qos.set_pcp_to_tc(map);
    assert(qos.num_traffic_classes() == 4);
    assert(qos.pcp_to_tc(0) == 0);
    assert(qos.pcp_to_tc(2) == 1);
    assert(qos.pcp_to_tc(4) == 2);
    assert(qos.pcp_to_tc(7) == 3);

    // Default profile and regeneration
    auto qdef = QoSProfile::default_profile(4);
    // default mapping approximately groups PCP 0-1->TC0, 2-3->TC1, 4-5->TC2, 6-7->TC3
    assert(qdef.pcp_to_tc(0) == 0);
    assert(qdef.pcp_to_tc(3) == 1);
    assert(qdef.pcp_to_tc(4) == 2);
    assert(qdef.pcp_to_tc(6) == 3);
    PriorityRegenerationTable regen{}; regen.map = {0,1,1,2,4,5,6,7};
    qdef.set_priority_regeneration(regen);
    auto pr = qdef.apply_ingress_pcp(2); // regen 2->1 then map
    assert(pr.first == 1 && pr.second == qdef.pcp_to_tc(1));

    // Canonical PCP from TC utility
    auto pcp_mid = FilteringUtils::canonical_pcp_from_tc(2, 4);
    (void)pcp_mid; // silence in Release
    assert(pcp_mid >= 4 && pcp_mid <= 5);

    // FDB learn/lookup/age
    Fdb fdb;
    std::array<uint8_t,6> m1{ {0x00,0x01,0x02,0x03,0x04,0x05} };
    std::array<uint8_t,6> m2{ {0x00,0x01,0x02,0x03,0x04,0x06} };
    PortId p1{1};
    PortId p2{2};
    fdb.learn(m1, p1, /*now_ms*/1000);
    fdb.learn(m2, p2, /*now_ms*/1000, /*static*/true);
    auto lp1 = fdb.lookup(m1); assert(lp1.has_value() && lp1->id == 1);
    auto lp2 = fdb.lookup(m2); assert(lp2.has_value() && lp2->id == 2);
    // Aging should remove m1 but keep static m2
    fdb.age(/*now_ms*/5000, /*age_ms*/3000);
    auto la1 = fdb.lookup(m1); assert(!la1.has_value());
    auto la2 = fdb.lookup(m2); assert(la2.has_value() && la2->id == 2);

    // Boundary aging: exactly at threshold should not remove
    fdb.learn(m1, p1, /*now_ms*/7000);
    fdb.age(/*now_ms*/10000, /*age_ms*/3000); // equal -> keep
    auto lb1 = fdb.lookup(m1); assert(lb1.has_value());

    // Multi-VLAN (Q-in-Q) build and parse
    VLANTag outer{}; outer.pcp=3; outer.dei=0; outer.vid=100;
    VLANTag inner{}; inner.pcp=5; inner.dei=1; inner.vid=200;
    TagHeader th_outer{}; th_outer.tpid=(uint16_t)EtherType::VLAN_TAGGED_STAG; th_outer.tci=outer;
    TagHeader th_inner{}; th_inner.tpid=(uint16_t)EtherType::VLAN_TAGGED_CTAG; th_inner.tci=inner;
    std::vector<TagHeader> qinq{th_outer, th_inner};
    auto hdr2 = Utils::build_ethernet_header(dst, src, qinq, (uint16_t)EtherType::IPv6);
    std::vector<uint8_t> frame2 = hdr2; frame2.insert(frame2.end(), payload.begin(), payload.end());
    ParsedFrame pf2{}; auto err2 = Utils::parse_ethernet_with_vlan(frame2.data(), frame2.size(), pf2);
    (void)err2; // silence in Release
    assert(err2 == ParseError::Ok);
    assert(pf2.vlan_stack.size() == 2);
    assert(pf2.vlan_stack[0].tci.vid == 100 && pf2.vlan_stack[1].tci.vid == 200);
    // Ingress filtering: drop if DEI=1 and PCP < threshold
    IngressRule ing{}; ing.drop_eligibility_min_pcp = 6;
    assert(FilteringUtils::ingress_drop(ing, inner) == true);
    assert(FilteringUtils::ingress_drop(ing, outer) == false);

    // Egress helper: override PCP
    EgressRule eg{}; eg.egress_vid = 300; eg.pcp_override = 7;
    auto tag_out = FilteringUtils::make_egress_tag(eg, /*pcp*/5);
    assert(tag_out.tci.vid == 300 && tag_out.tci.pcp == 7);

    // Per-port CTAG/STAG profile classification and egress
    auto ingress_prof = PortProfilesFactory::make_ingress_from_qos(qdef);
    auto egress_prof  = PortProfilesFactory::make_egress_from_qos(qdef);
    // Customize STAG table to map PCP 7->TC 0
    ingress_prof.pcp2tc.stag[7] = 0;
    auto cls_ctag = ingress_prof.classify(/*pcp*/6, TagTable::CTAG);
    auto cls_stag = ingress_prof.classify(/*pcp*/7, TagTable::STAG);
    assert(cls_ctag.second == qdef.pcp_to_tc(6));
    assert(cls_stag.second == 0);
    // Build egress headers for CTAG/STAG
    auto eh_ctag = egress_prof.make_egress_header(/*tc*/cls_ctag.second, TagTable::CTAG);
    auto eh_stag = egress_prof.make_egress_header(/*tc*/cls_stag.second, TagTable::STAG);
    assert(eh_ctag.tpid == (uint16_t)EtherType::VLAN_TAGGED_CTAG);
    assert(eh_stag.tpid == (uint16_t)EtherType::VLAN_TAGGED_STAG);

    std::cout << "IEEE 802.1Q-2020 basic tests passed\n";
    return 0;
}
