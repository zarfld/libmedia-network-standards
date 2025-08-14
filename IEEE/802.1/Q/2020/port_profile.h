// Per-port ingress/egress profiles with CTAG/STAG table support (IEEE 802.1Q-2020)
#pragma once
#include <array>
#include <cstdint>
#include <utility>
#include <optional>
#include "qos.h"
#include "filtering.h"

namespace IEEE { namespace _802_1Q { namespace _2020 {

enum class TagTable : uint8_t { CTAG = 0, STAG = 1 };

struct DualPcpToTcMap {
    std::array<uint8_t, 8> ctag{ {0,1,2,3,4,5,6,7} };
    std::array<uint8_t, 8> stag{ {0,1,2,3,4,5,6,7} };
};

struct DualPriorityRegen {
    std::array<uint8_t, 8> ctag{ {0,1,2,3,4,5,6,7} };
    std::array<uint8_t, 8> stag{ {0,1,2,3,4,5,6,7} };
};

// Ingress profile: per-port PCP regeneration and PCP->TC maps per tag type
struct PortIngressProfile {
    DualPriorityRegen regen{};
    DualPcpToTcMap pcp2tc{};
    IngressRule rule{}; // optional allow/drop policy reuse

    // Apply ingress PCP regeneration and classification for a given tag table
    // Returns pair<pcp_after_regen, traffic_class>
    std::pair<uint8_t,uint8_t> classify(uint8_t ingress_pcp, TagTable table) const noexcept {
        uint8_t p = ingress_pcp < 8 ? ingress_pcp : 7;
        if (table == TagTable::CTAG)
            p = regen.ctag[p];
        else
            p = regen.stag[p];
        uint8_t tc = (table == TagTable::CTAG) ? pcp2tc.ctag[p] : pcp2tc.stag[p];
        return {p, static_cast<uint8_t>(tc % 8)};
    }
};

// Egress profile: per-port TC->PCP selection and default egress tagging per tag type
struct PortEgressProfile {
    // Only lowest num_tc are used; remaining entries ignored
    std::array<uint8_t, 8> tc_to_pcp_ctag{ {0,1,2,3,4,5,6,7} };
    std::array<uint8_t, 8> tc_to_pcp_stag{ {0,1,2,3,4,5,6,7} };
    EgressRule default_rule_ctag{}; // default VID/PCP override policy for CTAG
    EgressRule default_rule_stag{}; // default VID/PCP override policy for STAG

    TagHeader make_egress_header(uint8_t tc, TagTable table, uint8_t dei = 0) const {
        uint8_t pcp = (table == TagTable::CTAG) ? tc_to_pcp_ctag[tc % 8] : tc_to_pcp_stag[tc % 8];
        const auto& rule = (table == TagTable::CTAG) ? default_rule_ctag : default_rule_stag;
        TagHeader h = FilteringUtils::make_egress_tag(rule, pcp, dei);
        h.tpid = (table == TagTable::CTAG)
            ? static_cast<uint16_t>(EtherType::VLAN_TAGGED_CTAG)
            : static_cast<uint16_t>(EtherType::VLAN_TAGGED_STAG);
        return h;
    }
};

// Helpers to derive default per-port profiles from a QoSProfile
struct PortProfilesFactory {
    static PortIngressProfile make_ingress_from_qos(const QoSProfile& q) {
        PortIngressProfile p{};
        // Use QoSProfile's single-table values for both CTAG/STAG by default
        for (uint8_t i=0;i<8;++i) {
            p.regen.ctag[i] = q.regenerate_pcp(i);
            p.regen.stag[i] = q.regenerate_pcp(i);
            p.pcp2tc.ctag[i] = q.pcp_to_tc(i);
            p.pcp2tc.stag[i] = q.pcp_to_tc(i);
        }
        return p;
    }

    static PortEgressProfile make_egress_from_qos(const QoSProfile& q) {
        PortEgressProfile p{};
        const auto num_tc = q.num_traffic_classes();
        for (uint8_t tc=0; tc<8; ++tc) {
            uint8_t canonical = FilteringUtils::canonical_pcp_from_tc(static_cast<uint8_t>(tc % (num_tc?num_tc:1)), num_tc);
            p.tc_to_pcp_ctag[tc] = canonical;
            p.tc_to_pcp_stag[tc] = canonical;
        }
        // Default rules: priority-tagging (VID=0)
        p.default_rule_ctag.egress_vid = VLAN_ID_PRIORITY_TAG;
        p.default_rule_stag.egress_vid = VLAN_ID_PRIORITY_TAG;
        return p;
    }
};

}}} // namespace
