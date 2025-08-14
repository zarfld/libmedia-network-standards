# IEEE 802.1Q-2020 Helper Library

Lightweight C++ helpers for IEEE 802.1Q-2020 concepts used by media/AVB stacks: MAC utilities, VLAN tags (PCP/DEI/VID), stacked VLANs (Q-in-Q), and simple PCP→traffic-class mapping.

## Scope (What’s implemented)
- VLAN Tag representation and validation: PCP (0–7), DEI, VID (0–4094; 0=priority tag)
- Tag header read/write: TPID (0x8100 C-Tag, 0x88A8 S-Tag) + TCI
- Ethernet header builder with optional stacked VLANs (outer→inner)
- Ethernet frame parser extracting dst/src, VLAN stack, EtherType, payload offset
- MAC address helpers (broadcast/multicast detection, parse/format)
- PCP→traffic-class mapping for N TX queues (simple proportional mapping)

## Current scope
- VLAN/Tagging helpers, parsing, and Ethernet header assembly
- PCP→traffic-class helpers for N transmit queues (with default mappings)
- Ingress Priority Regeneration tables and canonical PCP↔TC helpers
- QoS profile scaffolding (traffic class configs, PCP→TC map, shaper/scheduler hooks)
- Filtering helpers (ingress allowlist + DEI-based drop, egress tag helper)
- Minimal FDB (host-side learning/lookup/aging; for tests/tools)

Note: A complete 802.1Q bridge includes filtering, learning/forwarding behaviors, MVRP/MSTP state machines, policing/shaping and scheduler models, and management – larger features we’re adding iteratively.

## Build & Link
Target name: `IEEE::802.1Q::2020` (static)

CMake usage:
```cmake
find_package(ieee_802_1q_2020 CONFIG QUIET) # if installed/exported
# or if building within the OpenAvnu superbuild:
target_link_libraries(your_target PRIVATE IEEE::802.1Q::2020)
```
Header:
```cpp
#include "IEEE_802_1_Q_2020.h"
```

## Quick examples
Build Ethernet header with Q-in-Q and payload EtherType:
```cpp
using namespace IEEE::_802_1Q::_2020;
MacAddress dst{}; MacAddress::try_parse("01:80:C2:00:00:0E", dst); // example
MacAddress src{}; MacAddress::try_parse("00:1B:63:84:45:E6", src);

std::vector<TagHeader> vlan{
    { static_cast<uint16_t>(EtherType::VLAN_TAGGED_STAG), VLANTag{3,0,100} },
    { static_cast<uint16_t>(EtherType::VLAN_TAGGED_CTAG), VLANTag{5,0,10} }
};

auto hdr = Utils::build_ethernet_header(dst, src, vlan, /*payload*/0x22F0);
```

Parse frame and extract VLANs:
```cpp
ParsedFrame pf{};
auto err = Utils::parse_ethernet_with_vlan(frame.data(), frame.size(), pf);
if (err == ParseError::Ok) {
    // pf.dst, pf.src, pf.vlan_stack, pf.ether_type, pf.payload_offset
}
```

Map PCP to a traffic class for N queues:
```cpp
uint8_t tc = Utils::pcp_to_traffic_class(/*pcp*/5, /*queues*/8); // -> 5
```

Basic QoSProfile mapping (identity or custom map):
```cpp
QoSProfile qos{4};
PcpToTcMap map{}; map.map = {0,0,1,1,2,2,3,3};
qos.set_pcp_to_tc(map);
auto tc = qos.pcp_to_tc(6); // -> 3

Default mapping and priority regeneration:
```cpp
auto qdef = QoSProfile::default_profile(4);
PriorityRegenerationTable regen{}; regen.map = {0,1,1,2,4,5,6,7};
qdef.set_priority_regeneration(regen);
auto [pcp_after, tc_after] = qdef.apply_ingress_pcp(3);
```
```

Minimal FDB usage (learning + aging for host-side tests):
```cpp
Fdb fdb; PortId p1{1};
std::array<uint8_t,6> mac{{0x00,0x01,0x02,0x03,0x04,0x05}};
fdb.learn(mac, p1, /*now_ms*/1000);
auto port = fdb.lookup(mac); // port->id == 1
fdb.age(/*now_ms*/5000, /*age_ms*/3000); // entry removed if not static

Filtering helpers (ingress + egress):
```cpp
IngressRule ing{};
ing.allow_vids = {100, 200};
ing.drop_eligibility_min_pcp = 6; // drop DEI=1 if PCP < 6
bool drop = FilteringUtils::ingress_drop(ing, VLANTag{5,1,200});

EgressRule eg{}; eg.egress_vid = 100; // set VID
auto tag = FilteringUtils::make_egress_tag(eg, /*pcp*/5);
```
```

## API highlights
- `struct VLANTag { uint8_t pcp; uint8_t dei; uint16_t vid; ... };`
  - `is_valid()`, `pack_*()`, `unpack_*()`
- `struct TagHeader { uint16_t tpid; VLANTag tci; ... };`
- `struct MacAddress { std::array<uint8_t,6> bytes; ... };`
- `namespace Utils { build_ethernet_header(...); parse_ethernet_with_vlan(...); pcp_to_traffic_class(...); }`

## Notes & Edge cases
- Priority-tagged frames (VID=0) are allowed if you pass `allow_priority_tag=true` to validation.
- VID=4095 is reserved and considered invalid.
- Parser treats 0x8100 and 0x88A8 as VLAN TPIDs and will consume a stack until a non-VLAN EtherType appears.
- PCP→TC mapping here is proportional and generic; platform NIC mappings may differ.

## Roadmap
- Ingress/Egress filtering rules and VLAN classification helpers
- Priority regeneration and canonical mappings per 802.1Q clauses
- Shaper/scheduler model interface compatible with OpenAvnu shaper daemon
- MVRP/MSTP integration points (state-machine hosting in higher layers)

## Related OpenAvnu components
- MRPD (SRP/MRP): `daemons/mrpd`
- Shaper (QoS): `daemons/shaper`

## Version
Library version 1.0.0, namespace `IEEE::_802_1Q::_2020`.
