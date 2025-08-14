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
enum class SRClass : uint8_t {
	ClassA = 0x01,
	ClassB = 0x02
};

// Common AVB Profile identifiers (subset for practical integration)
enum class Profile : uint8_t {
	DefaultAVB = 0,      // Typical AVB end-station/bridge defaults
	ProAudio = 1,        // Same defaults as DefaultAVB unless overridden by app
	Automotive = 2       // Placeholder for OEM-specific tuning
};

// Default profile parameters used for quick validation and mapping
struct ProfileDefaults {
	uint8_t pcp_class_a;           // IEEE 802.1Q PCP for Class A (commonly 3)
	uint8_t pcp_class_b;           // IEEE 802.1Q PCP for Class B (commonly 2)
	uint32_t max_latency_a_ms;     // Common deployment target (Class A, e.g., ~2 ms)
	uint32_t max_latency_b_ms;     // Common deployment target (Class B, e.g., ~50 ms)
};

// Stream provisioning parameters for validation/mapping
struct StreamParameters {
	uint16_t vlan_id{0};               // 0 means untagged, typical AVB uses a VLAN
	SRClass sr_class{SRClass::ClassA};
	uint8_t pcp{0};                    // Priority Code Point (0..7)
	uint32_t max_transit_time_ms{0};   // End-to-end budget requested by application
	uint32_t max_frame_size_bytes{0};  // Optional informational (e.g., 802.1Qav)
	uint32_t frames_per_interval{0};   // Optional informational (e.g., talker interval)
};

// Retrieve pragmatic defaults for a given profile
ProfileDefaults GetDefaults(Profile profile) noexcept;

// Helper defaults by SR Class
uint8_t DefaultPCP(SRClass klass) noexcept;
uint32_t DefaultMaxLatencyMs(SRClass klass) noexcept;

// Compliance helper: verifies stream parameters meet or exceed conservative defaults
// (pcp matches default for class, and requested max_transit_time_ms <= default max latency)
bool IsCompliant(const StreamParameters& params, const ProfileDefaults& def) noexcept;

// Friendly string helpers for logging/diagnostics
const char* ToString(SRClass klass) noexcept;
const char* ToString(Profile profile) noexcept;

} // namespace _2016
} // namespace _802_1BA
} // namespace IEEE

