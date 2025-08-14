#pragma once

/**
 * IEEE 802.1BA-2016: Audio Video Bridging (AVB) Systems – Library API
 *
 * Purpose: Lightweight, header-only compatible API surface for 3rd-party integrations
 * to reference AVB profiles, SR classes, priorities and basic compliance checks.
 *
 * Namespace convention follows existing standards in this repository:
 *   IEEE::_802_1BA::_2016
 *
 * Note: This library provides pragmatic defaults and helpers commonly used for AVB
 * deployments. It does not include any normative copy of the standard text.
 */

#include <cstdint>
#include <string>

namespace IEEE {
namespace _802_1BA {
namespace _2016 {

// Stream Reservation Classes (per AVB profiles)
// SpecRef: ISO/IEC/IEEE 8802-1BA:2016 – AVB System Profiles (SR classes); see sections describing Class A/B requirements.
enum class SRClass : uint8_t {
	ClassA = 0x01,
	ClassB = 0x02
};

// Common AVB Profile identifiers (subset for practical integration)
// SpecRef: ISO/IEC/IEEE 8802-1BA:2016 – Device/system profiles; map concrete device roles to profile configurations.
enum class Profile : uint8_t {
	DefaultAVB = 0,      // Typical AVB end-station/bridge defaults
	ProAudio = 1,        // Same defaults as DefaultAVB unless overridden by app
	Automotive = 2       // Placeholder for OEM-specific tuning
};

// Default profile parameters used for quick validation and mapping
struct ProfileDefaults {
	// SpecRef: ISO/IEC/IEEE 8802-1BA:2016 – Recommended QoS/latency goals for SR classes (PCP per 802.1Q; latency budgets per BA profile guidance).
	uint8_t pcp_class_a;           // IEEE 802.1Q PCP for Class A (commonly 3)
	uint8_t pcp_class_b;           // IEEE 802.1Q PCP for Class B (commonly 2)
	uint32_t max_latency_a_ms;     // Common deployment target (Class A, e.g., ~2 ms)
	uint32_t max_latency_b_ms;     // Common deployment target (Class B, e.g., ~50 ms)
};

// Stream provisioning parameters for validation/mapping
struct StreamParameters {
	// SpecRef: ISO/IEC/IEEE 8802-1BA:2016 – End-station provisioning parameters; VLAN usage, SR class selection, PCP mapping.
	uint16_t vlan_id{0};               // 0 means untagged, typical AVB uses a VLAN
	SRClass sr_class{SRClass::ClassA};
	uint8_t pcp{0};                    // Priority Code Point (0..7)
	uint32_t max_transit_time_ms{0};   // End-to-end budget requested by application
	uint32_t max_frame_size_bytes{0};  // Optional informational (e.g., 802.1Qav)
	uint32_t frames_per_interval{0};   // Optional informational (e.g., talker interval)
};

// Bitmask for required protocols/features per 802.1BA profiles
enum class RequiredProtocolFlags : uint32_t {
	None  = 0,
	gPTP  = 1u << 0,  // IEEE 802.1AS
	MSRP  = 1u << 1,  // Stream Reservation (802.1Q SRP/Qat)
	MAAP  = 1u << 2,  // IEEE 1722 MAAP
	AVTP  = 1u << 3,  // IEEE 1722
	Qav   = 1u << 4   // IEEE 802.1Q Credit-Based Shaper (FQTSS)
};

// Enable bitwise ops for RequiredProtocolFlags
inline RequiredProtocolFlags operator|(RequiredProtocolFlags a, RequiredProtocolFlags b) {
	return static_cast<RequiredProtocolFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline RequiredProtocolFlags& operator|=(RequiredProtocolFlags& a, RequiredProtocolFlags b) {
	a = a | b; return a;
}
inline RequiredProtocolFlags operator&(RequiredProtocolFlags a, RequiredProtocolFlags b) {
	return static_cast<RequiredProtocolFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}
inline bool Any(RequiredProtocolFlags f) { return static_cast<uint32_t>(f) != 0; }

// Profile definition mapping BA requirements to concrete defaults and protocols
struct ProfileDefinition {
	// SpecRef: ISO/IEC/IEEE 8802-1BA:2016 – Profile requirements: which protocols must be present and configured.
	Profile profile;
	ProfileDefaults defaults;     // PCP + latency defaults
	uint16_t default_vlan_id;     // Typical deployments reserve a VLAN for AVB
	RequiredProtocolFlags required_protocols; // gPTP, MSRP, MAAP, AVTP, Qav
};

// Retrieve pragmatic defaults for a given profile
// SpecRef: ISO/IEC/IEEE 8802-1BA:2016 – Profile recommendations and defaults for SR classes.
ProfileDefaults GetDefaults(Profile profile) noexcept;

// Helper defaults by SR Class
// SpecRef: 802.1Q QoS (PCP mapping) referenced by 802.1BA; latency budgets per BA.
uint8_t DefaultPCP(SRClass klass) noexcept;
uint32_t DefaultMaxLatencyMs(SRClass klass) noexcept;

// Compliance helper: verifies stream parameters meet or exceed conservative defaults
// (pcp matches default for class, and requested max_transit_time_ms <= default max latency)
// SpecRef: ISO/IEC/IEEE 8802-1BA:2016 – End-station compliance criteria for profile; this helper checks PCP/latency only.
// TODO [FullCompliance]: Extend to verify MSRP domain, SRP reservation state, MAAP address ownership, AVTP format, Qav CBS configured.
bool IsCompliant(const StreamParameters& params, const ProfileDefaults& def) noexcept;

// Retrieve a complete profile definition (requirements + defaults)
// SpecRef: ISO/IEC/IEEE 8802-1BA:2016 – System profile definition (dependencies and defaults).
ProfileDefinition GetProfileDefinition(Profile profile) noexcept;

// Validate that provided protocol set meets the profile requirements
// SpecRef: ISO/IEC/IEEE 8802-1BA:2016 – Conformance requires presence of referenced protocols.
bool ValidateProtocols(RequiredProtocolFlags provided, RequiredProtocolFlags required) noexcept;

// Friendly string helpers for logging/diagnostics
const char* ToString(SRClass klass) noexcept;
const char* ToString(Profile profile) noexcept;

} // namespace _2016
} // namespace _802_1BA
} // namespace IEEE

