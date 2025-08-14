// IEEE 802.1Q filtering helpers: ingress/egress rules and classification aids
#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <optional>
#include "IEEE_802_1_Q_2020.h"
#include "qos.h"

namespace IEEE { namespace _802_1Q { namespace _2020 {

struct IngressRule {
    // Allow list of VLAN IDs; empty means allow all
    std::vector<uint16_t> allow_vids;
    // Drop if DEI=1 and PCP below threshold (optional policy)
    std::optional<uint8_t> drop_eligibility_min_pcp;
};

struct EgressRule {
    // Egress tagging policy: which VLAN to tag, or priority-tag (VID=0)
    uint16_t egress_vid{VLAN_ID_PRIORITY_TAG};
    // Optional PCP override
    std::optional<uint8_t> pcp_override;
};

namespace FilteringUtils {
    inline bool vid_allowed(const IngressRule& r, uint16_t vid) {
        if (r.allow_vids.empty()) return true;
        for (auto v : r.allow_vids) if (v == vid) return true;
        return false;
    }

    inline bool ingress_drop(const IngressRule& r, const VLANTag& tci) {
        if (!vid_allowed(r, tci.vid)) return true;
        if (r.drop_eligibility_min_pcp.has_value() && tci.dei) {
            return tci.pcp < r.drop_eligibility_min_pcp.value();
        }
        return false;
    }

    inline uint8_t canonical_pcp_from_tc(uint8_t tc, uint8_t num_tc) {
        if (num_tc == 0) num_tc = 1; if (num_tc > 8) num_tc = 8;
        if (tc >= num_tc) tc = static_cast<uint8_t>(num_tc-1);
        // map tc back to center of its PCP bucket
        uint8_t start = static_cast<uint8_t>((tc * 8) / num_tc);
        uint8_t end   = static_cast<uint8_t>(((tc+1) * 8) / num_tc);
        if (end <= start) end = static_cast<uint8_t>(start+1);
        return static_cast<uint8_t>((start + end - 1) / 2); // midpoint bias high
    }

    inline uint8_t tc_from_pcp_with_profile(const QoSProfile& q, uint8_t pcp) {
        return q.pcp_to_tc(pcp);
    }

    inline TagHeader make_egress_tag(const EgressRule& rule, uint8_t pcp, uint8_t dei = 0) {
        TagHeader h{};
        h.tpid = static_cast<uint16_t>(EtherType::VLAN_TAGGED_CTAG);
        h.tci.pcp = rule.pcp_override.has_value() ? rule.pcp_override.value() : pcp;
        h.tci.dei = dei ? 1 : 0;
        h.tci.vid = rule.egress_vid;
        return h;
    }
}

}}} // namespace
