# IEEE 802.1BA-2016 Gap Analysis and Spec References

This file tracks coverage of ISO/IEC/IEEE 8802-1BA:2016 in the `ieee_802_1ba_2016` library and identifies remaining work for full conformance. Clause numbers are referenced at a high level to avoid quoting the standard.

## Summary
- Scope: 802.1BA defines AVB system profiles and references dependent standards (802.1AS, 802.1Q SRP/Qav, 1722, 1722 MAAP).
- Provided here: Profile selection/defaults, protocol requirement flags, simple compliance checks (PCP/latency), developer guide.
- Not provided here: The referenced protocols themselves (covered elsewhere in this repo), and end-to-end runtime verification that they’re all active and configured per profile.

## Code map → Spec anchors
- `SRClass`, `Profile`, `ProfileDefaults`, `StreamParameters` (IEEE_802_1_BA_21016.h)
  - SpecRef: 802.1BA system profiles; SR classes A/B; guidance on PCP/latency budgets.
- `RequiredProtocolFlags` and bitwise helpers (IEEE_802_1_BA_21016.h)
  - SpecRef: 802.1BA mandated services for AVB systems: 802.1AS, SRP/MSRP, 1722/MAAP, 802.1Qav.
- `GetDefaults(Profile)` (IEEE_802_1_BA_21016.cpp)
  - SpecRef: profile default expectations; values are pragmatic defaults to be tuned per deployment.
- `GetProfileDefinition(Profile)` (IEEE_802_1_BA_21016.cpp)
  - SpecRef: maps profile to required protocols and default VLAN usage.
- `IsCompliant(...)` (IEEE_802_1_BA_21016.cpp)
  - SpecRef: end-station profile conformance constraints; PCP must match SR class, latency within budget.
  - TODO: extend checks to verify active gPTP, SRP reservations, MAAP, AVTP format, and Qav CBS.
- `ValidateProtocols(...)` (IEEE_802_1_BA_21016.cpp)
  - SpecRef: BA requires presence of referenced protocols; this checks provided vs required flags.

## Gaps to close for full BA-2016 compliance
1. Protocol runtime verification hooks
   - 802.1AS domain lock/sync state (time base available)
   - MSRP/SRP declarations/reservations status per stream
   - MAAP allocated destination address for multicast streams
   - AVTP talker/listener configuration and stream timing
   - Qav/CBS shaper configured on egress ports used by streams
2. Profile/role coverage
   - Enumerate device roles (talker, listener, bridge) and attach role-specific checks
   - Map BA profiles to 802.1Q PCP/traffic class and SRP domain numbers
3. Telemetry and diagnostics
   - Structured status reporting (per-profile conformance report)
   - Logging of non-conforming parameters and missing services
4. Conformance tests
   - Integrate with unified testing to assert BA conformance across real hardware

## Next steps
- Add a `ProfileRole` enum and extend `ProfileDefinition` with role-specific constraints.
- Wire gPTP/MSRP/MAAP/AVTP/Qav verifiers using existing modules in this repo.
- Provide CLI/example to print BA conformance report for a given NIC/domain.
