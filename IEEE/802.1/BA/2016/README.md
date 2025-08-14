# IEEE 802.1BA-2016 (AVB Systems) – Developer Guide

This library provides a lightweight API for using IEEE 802.1BA-2016 AVB System profiles in applications and integrations. It follows the namespace style used throughout this repo.

- Namespace: `IEEE::_802_1BA::_2016`
- Target: `ieee_802_1ba_2016` (static library)

## What it contains
- SR classes and PCP defaults (`SRClass`, `DefaultPCP`, `DefaultMaxLatencyMs`)
- Profile defaults (`GetDefaults(Profile)`)
- Full profile definition including required protocols (`GetProfileDefinition`)
- Stream parameter validation (`IsCompliant`)
- Protocol requirement validation (`ValidateProtocols`)

## Quick start
```cpp
#include <IEEE_802_1_BA_21016.h>
using namespace IEEE::_802_1BA::_2016;

int main() {
  // Choose a profile
  auto profile = Profile::DefaultAVB;
  auto def = GetDefaults(profile);
  auto defn = GetProfileDefinition(profile);

  // Protocol readiness (example: all BA-required services enabled)
  RequiredProtocolFlags provided = RequiredProtocolFlags::gPTP |
                                   RequiredProtocolFlags::MSRP |
                                   RequiredProtocolFlags::MAAP |
                                   RequiredProtocolFlags::AVTP |
                                   RequiredProtocolFlags::Qav;
  if (!ValidateProtocols(provided, defn.required_protocols)) {
    // initialize/start missing services
  }

  // Validate stream parameters
  StreamParameters p{};
  p.vlan_id = defn.default_vlan_id;
  p.sr_class = SRClass::ClassA;
  p.pcp = def.defaults.pcp_class_a;     // PCP must match class default
  p.max_transit_time_ms = def.defaults.max_latency_a_ms; // within budget

  if (!IsCompliant(p, def)) {
    // adjust parameters or reject configuration
  }
}
```

## Integration notes
- 802.1BA profiles expect the following services at runtime:
  - 802.1AS (gPTP) time sync
  - MSRP (SRP) for reservations
  - MAAP for multicast address allocation
  - AVTP (1722) for transport
  - 802.1Qav (credit-based shaping)
- Use `GetProfileDefinition(profile)` to retrieve `required_protocols` and enforce them before streaming.

## Status
- This module provides the application-facing pieces to select profiles, validate parameters, and ensure dependent services are present.
- It does not implement the dependent protocols (gPTP, MSRP/MAAP, AVTP, Qav) here; those live in their respective modules in this repository and must be integrated/started as required by your app.

