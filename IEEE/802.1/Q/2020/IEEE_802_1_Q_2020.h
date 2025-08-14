/**
 * @file IEEE_802_1_Q_2020.h
 * @brief IEEE 802.1Q-2020 Library – VLAN, Priority, and Tagging Helpers
 * @details ISO/IEC/IEEE 8802-1Q:2020 compliant primitives for Ethernet VLAN tagging
 *          and QoS utilities, designed for reuse by third-party components.
 *
 * Namespace pattern aligned with existing standards in this repository:
 *   namespace IEEE { namespace _802_1Q { namespace _2020 { ... }}}
 *
 * Scope (practical core):
 *  - VLAN Tag (PCP/DEI/VID) representation and validation
 *  - 802.1Q/802.1ad TPID constants (C-TAG/S-TAG)
 *  - Parsing/building stacked VLAN headers (Q-in-Q)
 *  - Basic PCP-to-traffic-class helpers
 *  - Minimal Ethernet framing helpers (MAC, EtherType)
 *
 * Note: This is a protocol-helper library; it does not implement bridge FDB,
 * MSTP/MVRP state machines, or management models. Those belong to higher layers.
 */

#ifndef IEEE_802_1_Q_2020_H
#define IEEE_802_1_Q_2020_H

#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <string>
#include <stdexcept>

namespace IEEE {
namespace _802_1Q {
namespace _2020 {

// ============================================================================
// Constants and basic types
// ============================================================================

/// Library semantic version (not the IEEE spec version)
constexpr uint32_t LIB_VERSION = 0x0001'0000; // 1.0.0

/// IEEE 802.1Q EtherType values
enum class EtherType : uint16_t {
	IPv4                 = 0x0800,
	ARP                  = 0x0806,
	IPv6                 = 0x86DD,
	VLAN_TAGGED_CTAG     = 0x8100, // C-Tag (Customer VLAN tag per 802.1Q)
	VLAN_TAGGED_STAG     = 0x88A8, // S-Tag (Provider/service VLAN tag per 802.1ad)
};

/// Max VLAN ID (0..4094 valid; 4095 reserved)
constexpr uint16_t VLAN_ID_MAX = 4094;

/// Special VID values per 802.1Q
constexpr uint16_t VLAN_ID_PRIORITY_TAG = 0;     // VID=0 means priority-tagged only
constexpr uint16_t VLAN_ID_RESERVED      = 4095; // 0xFFF reserved

/// Default PCP to traffic class mapping policy (identity mapping)
constexpr uint8_t PCP_MAX = 7; // 3 bits

// ============================================================================
// Endianness helpers (network byte order = big-endian)
// ============================================================================

inline constexpr uint16_t to_network_u16(uint16_t v) noexcept {
	return static_cast<uint16_t>((v >> 8) | (v << 8));
}

inline constexpr uint16_t from_network_u16(uint16_t v) noexcept {
	return to_network_u16(v);
}

// ============================================================================
// MAC address helper
// ============================================================================

struct MacAddress {
	std::array<uint8_t, 6> bytes{};

	constexpr MacAddress() = default;
	constexpr explicit MacAddress(const std::array<uint8_t, 6>& b) : bytes(b) {}

	static MacAddress broadcast();
	bool is_broadcast() const noexcept;
	bool is_multicast() const noexcept; // MSB of first octet set

	std::string to_string() const; // "aa:bb:cc:dd:ee:ff"
	static bool try_parse(const std::string& s, MacAddress& out);
};

// ============================================================================
// VLAN Tag (TCI) and Tag header
// ============================================================================

struct VLANTag {
	uint8_t  pcp = 0;   // Priority Code Point (3 bits)
	uint8_t  dei = 0;   // Drop Eligibility Indicator (1 bit)
	uint16_t vid = 0;   // VLAN Identifier (12 bits)

	// Validate field ranges per 802.1Q
	bool is_valid(bool allow_priority_tag = true) const noexcept;

	// Pack/Unpack the 16-bit TCI per 802.1Q (PCP:3 | DEI:1 | VID:12)
	uint16_t pack_host_order() const noexcept;              // host endian 0xPCP/DEI/VID
	uint16_t pack_network_order() const noexcept;           // big-endian over the wire
	static VLANTag unpack_host_order(uint16_t tci) noexcept;
	static VLANTag unpack_network_order(uint16_t net_tci) noexcept;
};

struct TagHeader {
	uint16_t tpid = static_cast<uint16_t>(EtherType::VLAN_TAGGED_CTAG);
	VLANTag  tci{};

	// Serialize/parse the 4-byte VLAN header
	void write_to(std::vector<uint8_t>& out) const;
	static bool parse_from(const uint8_t* data, size_t len, TagHeader& out);
};

// ============================================================================
// Ethernet frame parsing/building with optional VLAN stack (Q-in-Q)
// ============================================================================

struct ParsedFrame {
	MacAddress dst{};
	MacAddress src{};
	std::vector<TagHeader> vlan_stack; // outer-to-inner order
	uint16_t ether_type = 0;           // ether type of payload
	size_t payload_offset = 0;         // index into original frame where payload starts
};

enum class ParseError {
	Ok = 0,
	TooShort,
	Malformed,
};

namespace Utils {
	// Parse an Ethernet frame and collect VLAN stack.
	ParseError parse_ethernet_with_vlan(const uint8_t* frame, size_t length, ParsedFrame& out);

	// Build an Ethernet header with given VLAN stack (outer-to-inner) and payload EtherType.
	// Returns a vector containing: DST(6) SRC(6) [VLAN*] EtherType(2). Payload not included.
	std::vector<uint8_t> build_ethernet_header(const MacAddress& dst,
											   const MacAddress& src,
											   const std::vector<TagHeader>& vlan_stack,
											   uint16_t payload_ether_type);

	// Compute traffic class from PCP for a device with N queues (1..8). Default: clamp(pcp).
	uint8_t pcp_to_traffic_class(uint8_t pcp, uint8_t num_queues = 8) noexcept;
}

} // namespace _2020
} // namespace _802_1Q
} // namespace IEEE

#endif // IEEE_802_1_Q_2020_H

