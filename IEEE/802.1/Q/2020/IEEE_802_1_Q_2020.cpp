/**
 * @file IEEE_802_1_Q_2020.cpp
 * @brief Implementation for IEEE 802.1Q-2020 helper library.
 */

#include "IEEE_802_1_Q_2020.h"

#include <cstdio>
#include <cctype>

namespace IEEE {
namespace _802_1Q {
namespace _2020 {

// ========================= MacAddress =========================

MacAddress MacAddress::broadcast() {
	MacAddress m;
	m.bytes.fill(0xFF);
	return m;
}

bool MacAddress::is_broadcast() const noexcept {
	for (auto b : bytes) if (b != 0xFF) return false; return true;
}

bool MacAddress::is_multicast() const noexcept {
	return (bytes[0] & 0x01) != 0;
}

std::string MacAddress::to_string() const {
	char buf[18];
	std::snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
				  bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
	return std::string(buf);
}

static inline int hexval(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
	return -1;
}

bool MacAddress::try_parse(const std::string& s, MacAddress& out) {
	// Accept forms: aa:bb:cc:dd:ee:ff (17 chars)
	if (s.size() != 17) return false;
	MacAddress m;
	for (int i = 0; i < 6; ++i) {
		int hi = hexval(s[i*3+0]);
		int lo = hexval(s[i*3+1]);
		if (hi < 0 || lo < 0) return false;
		m.bytes[i] = static_cast<uint8_t>((hi << 4) | lo);
		if (i < 5 && s[i*3+2] != ':') return false;
	}
	out = m;
	return true;
}

// ============================ VLANTag ============================

bool VLANTag::is_valid(bool allow_priority_tag) const noexcept {
	if (pcp > PCP_MAX) return false;
	if (dei > 1) return false;
	if (vid == VLAN_ID_RESERVED) return false;
	if (!allow_priority_tag && vid == VLAN_ID_PRIORITY_TAG) return false;
	if (vid > VLAN_ID_MAX) return false;
	return true;
}

uint16_t VLANTag::pack_host_order() const noexcept {
	uint16_t tci = 0;
	tci |= static_cast<uint16_t>((pcp & 0x7) << 13);
	tci |= static_cast<uint16_t>((dei & 0x1) << 12);
	tci |= static_cast<uint16_t>(vid & 0x0FFF);
	return tci;
}

uint16_t VLANTag::pack_network_order() const noexcept {
	return to_network_u16(pack_host_order());
}

/*static*/ VLANTag VLANTag::unpack_host_order(uint16_t tci) noexcept {
	VLANTag v{};
	v.pcp = static_cast<uint8_t>((tci >> 13) & 0x7);
	v.dei = static_cast<uint8_t>((tci >> 12) & 0x1);
	v.vid = static_cast<uint16_t>(tci & 0x0FFF);
	return v;
}

/*static*/ VLANTag VLANTag::unpack_network_order(uint16_t net_tci) noexcept {
	return unpack_host_order(from_network_u16(net_tci));
}

// ============================ TagHeader ============================

void TagHeader::write_to(std::vector<uint8_t>& out) const {
	const uint16_t net_tpid = to_network_u16(tpid);
	const uint16_t net_tci  = tci.pack_network_order();
	out.push_back(static_cast<uint8_t>(net_tpid >> 8));
	out.push_back(static_cast<uint8_t>(net_tpid & 0xFF));
	out.push_back(static_cast<uint8_t>(net_tci >> 8));
	out.push_back(static_cast<uint8_t>(net_tci & 0xFF));
}

/*static*/ bool TagHeader::parse_from(const uint8_t* data, size_t len, TagHeader& out) {
	if (!data || len < 4) return false;
	uint16_t tpid = static_cast<uint16_t>((data[0] << 8) | data[1]);
	uint16_t tci  = static_cast<uint16_t>((data[2] << 8) | data[3]);
	out.tpid = from_network_u16(tpid);
	out.tci  = VLANTag::unpack_network_order(tci);
	return true;
}

// ============================ Utils ============================

namespace Utils {

static inline bool is_vlan_tpid(uint16_t host_tpid) {
	return host_tpid == static_cast<uint16_t>(EtherType::VLAN_TAGGED_CTAG)
		|| host_tpid == static_cast<uint16_t>(EtherType::VLAN_TAGGED_STAG);
}

ParseError parse_ethernet_with_vlan(const uint8_t* frame, size_t length, ParsedFrame& out) {
	if (!frame || length < 14) return ParseError::TooShort;

	size_t idx = 0;
	for (int i = 0; i < 6; ++i) out.dst.bytes[i] = frame[idx++];
	for (int i = 0; i < 6; ++i) out.src.bytes[i] = frame[idx++];

	if (length - idx < 2) return ParseError::TooShort;
	uint16_t ether_or_tpid = static_cast<uint16_t>((frame[idx] << 8) | frame[idx+1]);
	uint16_t host_ether_or_tpid = from_network_u16(ether_or_tpid);
	idx += 2;

	out.vlan_stack.clear();

	// Parse stacked VLANs
	while (is_vlan_tpid(host_ether_or_tpid)) {
		if (length - idx < 2) return ParseError::TooShort;
		TagHeader tag{};
		tag.tpid = host_ether_or_tpid;
		uint16_t net_tci = static_cast<uint16_t>((frame[idx] << 8) | frame[idx+1]);
		idx += 2;
		tag.tci = VLANTag::unpack_network_order(net_tci);
		out.vlan_stack.push_back(tag);

		if (length - idx < 2) return ParseError::TooShort;
		ether_or_tpid = static_cast<uint16_t>((frame[idx] << 8) | frame[idx+1]);
		host_ether_or_tpid = from_network_u16(ether_or_tpid);
		idx += 2;
	}

	out.ether_type = host_ether_or_tpid;
	out.payload_offset = idx;
	if (out.payload_offset > length) return ParseError::Malformed;
	return ParseError::Ok;
}

std::vector<uint8_t> build_ethernet_header(const MacAddress& dst,
										   const MacAddress& src,
										   const std::vector<TagHeader>& vlan_stack,
										   uint16_t payload_ether_type) {
	std::vector<uint8_t> out;
	out.reserve(14 + vlan_stack.size() * 4);

	// DST, SRC
	out.insert(out.end(), dst.bytes.begin(), dst.bytes.end());
	out.insert(out.end(), src.bytes.begin(), src.bytes.end());

	// VLAN stack outer-to-inner
	for (const auto& tag : vlan_stack) {
		TagHeader th = tag;
		th.write_to(out);
	}

	// Payload EtherType
	uint16_t net_type = to_network_u16(payload_ether_type);
	out.push_back(static_cast<uint8_t>(net_type >> 8));
	out.push_back(static_cast<uint8_t>(net_type & 0xFF));

	return out;
}

uint8_t pcp_to_traffic_class(uint8_t pcp, uint8_t num_queues) noexcept {
	if (num_queues == 0) return 0;
	if (num_queues > 8) num_queues = 8;
	if (pcp > PCP_MAX) pcp = PCP_MAX;
	// Map 0..7 across 0..(N-1)
	// Simple proportional mapping: tc = floor(pcp * N / 8)
	uint8_t tc = static_cast<uint8_t>((static_cast<uint16_t>(pcp) * num_queues) / 8);
	if (tc >= num_queues) tc = static_cast<uint8_t>(num_queues - 1);
	return tc;
}

} // namespace Utils

} // namespace _2020
} // namespace _802_1Q
} // namespace IEEE

