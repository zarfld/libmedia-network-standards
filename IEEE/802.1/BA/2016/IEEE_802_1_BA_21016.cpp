#include "IEEE_802_1_BA_21016.h"

namespace IEEE {
namespace _802_1BA {
namespace _2016 {

static constexpr ProfileDefaults kDefaults_DefaultAVB { 3, 2, 2u, 50u };
static constexpr ProfileDefaults kDefaults_ProAudio  { 3, 2, 2u, 50u };
static constexpr ProfileDefaults kDefaults_Automotive{ 3, 2, 2u, 50u };
// SpecRef: ISO/IEC/IEEE 8802-1BA:2016 – These are pragmatic defaults; tune as per deployment guidance.

ProfileDefaults GetDefaults(Profile profile) noexcept {
	switch (profile) {
		case Profile::DefaultAVB: return kDefaults_DefaultAVB;
		case Profile::ProAudio:   return kDefaults_ProAudio;
		case Profile::Automotive: return kDefaults_Automotive;
		default:                  return kDefaults_DefaultAVB;
	}
}

uint8_t DefaultPCP(SRClass klass) noexcept {
	return (klass == SRClass::ClassA) ? 3 : 2;
}

uint32_t DefaultMaxLatencyMs(SRClass klass) noexcept {
	return (klass == SRClass::ClassA) ? 2u : 50u;
}

ProfileDefinition GetProfileDefinition(Profile profile) noexcept {
	ProfileDefinition defn{};
	defn.profile = profile;
	defn.defaults = GetDefaults(profile);
	defn.default_vlan_id = 2; // Typical demo VLAN; projects may override
	// SpecRef: ISO/IEC/IEEE 8802-1BA:2016 – Required services for AVB system profiles:
	// - gPTP (802.1AS), MSRP/SRP (802.1Q), MAAP (1722), AVTP (1722), Qav CBS (802.1Q)
	defn.required_protocols = RequiredProtocolFlags::gPTP |
							  RequiredProtocolFlags::MSRP |
							  RequiredProtocolFlags::MAAP |
							  RequiredProtocolFlags::AVTP |
							  RequiredProtocolFlags::Qav;
	return defn;
}

bool ValidateProtocols(RequiredProtocolFlags provided, RequiredProtocolFlags required) noexcept {
	return (static_cast<uint32_t>(provided & required) == static_cast<uint32_t>(required));
}

bool IsCompliant(const StreamParameters& params, const ProfileDefaults& def) noexcept {
	// PCP check must align with SR class default
	const auto expected_pcp = (params.sr_class == SRClass::ClassA) ? def.pcp_class_a : def.pcp_class_b;
	if (params.pcp != expected_pcp) {
		return false;
	}
	// Transit time must be within conservative budget for class
	const auto max_allowed = (params.sr_class == SRClass::ClassA) ? def.max_latency_a_ms : def.max_latency_b_ms;
	if (params.max_transit_time_ms == 0 || params.max_transit_time_ms > max_allowed) {
		return false;
	}
	// TODO [FullCompliance][BA-2016]:
	// - Verify gPTP domain lock and sync state (802.1AS)
	// - Verify MSRP/SRP talker/listener declarations and reservations (802.1Q SRP)
	// - Verify MAAP address allocation state for multicast destination (1722)
	// - Verify AVTP stream format and timing (1722)
	// - Verify Qav CBS configuration on egress port (802.1Q)
	return true;
}

const char* ToString(SRClass klass) noexcept {
	switch (klass) {
		case SRClass::ClassA: return "ClassA";
		case SRClass::ClassB: return "ClassB";
		default: return "Unknown";
	}
}

const char* ToString(Profile profile) noexcept {
	switch (profile) {
		case Profile::DefaultAVB: return "DefaultAVB";
		case Profile::ProAudio:   return "ProAudio";
		case Profile::Automotive: return "Automotive";
		default: return "Unknown";
	}
}

} // namespace _2016
} // namespace _802_1BA
} // namespace IEEE

