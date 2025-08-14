#include "IEEE_802_1_BA_21016.h"

namespace IEEE {
namespace _802_1BA {
namespace _2016 {

static constexpr ProfileDefaults kDefaults_DefaultAVB { 3, 2, 2u, 50u };
static constexpr ProfileDefaults kDefaults_ProAudio  { 3, 2, 2u, 50u };
static constexpr ProfileDefaults kDefaults_Automotive{ 3, 2, 2u, 50u };

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

