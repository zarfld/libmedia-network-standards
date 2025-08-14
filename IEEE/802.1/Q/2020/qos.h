#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <utility>

namespace IEEE { namespace _802_1Q { namespace _2020 {

// Minimal QoS primitives toward 802.1Q sections: traffic classes, priority mapping,
// and per-class shaper placeholders (token bucket model), to be implemented fully.

struct TrafficClassConfig {
    // IdleSlope/SendSlope are in kbps for SR classes; not enforced yet.
    uint32_t idle_slope_kbps{0};
    uint32_t send_slope_kbps{0};
    uint32_t hi_credit_bytes{0};
    uint32_t lo_credit_bytes{0};
};

struct PcpToTcMap {
    // Map PCP 0..7 to traffic class index 0..N-1
    std::array<uint8_t, 8> map{ {0,1,2,3,4,5,6,7} };
};

struct PriorityRegenerationTable {
    // Ingress PCP regeneration (per-port), default identity mapping.
    std::array<uint8_t, 8> map{ {0,1,2,3,4,5,6,7} };
};

// Forward-declare shaper/scheduler interfaces (defined in scheduler.h)
class IShaper;
class IQueueScheduler;

class QoSProfile {
public:
    // N traffic classes (1..8)
    explicit QoSProfile(uint8_t num_tc = 8) : num_tc_(num_tc ? (num_tc>8?8:num_tc) : 1) {
        tc_cfg_.resize(num_tc_);
    }

    static PcpToTcMap make_default_pcp_to_tc(uint8_t num_tc) {
        if (num_tc == 0) num_tc = 1; if (num_tc > 8) num_tc = 8;
        PcpToTcMap m{};
        for (uint8_t p=0; p<8; ++p) {
            m.map[p] = static_cast<uint8_t>((static_cast<uint16_t>(p) * num_tc) / 8);
            if (m.map[p] >= num_tc) m.map[p] = static_cast<uint8_t>(num_tc-1);
        }
        return m;
    }

    static QoSProfile default_profile(uint8_t num_tc) {
        QoSProfile q{num_tc};
        q.set_pcp_to_tc(make_default_pcp_to_tc(num_tc));
        return q;
    }

    uint8_t num_traffic_classes() const noexcept { return num_tc_; }

    void set_pcp_to_tc(const PcpToTcMap& m) { pcp2tc_ = m; }
    uint8_t pcp_to_tc(uint8_t pcp) const noexcept {
        return pcp < 8 ? (pcp2tc_.map[pcp] % num_tc_) : (pcp2tc_.map[7] % num_tc_);
    }

    void set_priority_regeneration(const PriorityRegenerationTable& t) { regen_ = t; }
    uint8_t regenerate_pcp(uint8_t ingress_pcp) const noexcept {
        return ingress_pcp < 8 ? regen_.map[ingress_pcp] : regen_.map[7];
    }

    // Apply ingress PCP regeneration followed by PCP→TC mapping
    // Returns pair<pcp_after_regen, traffic_class>
    std::pair<uint8_t,uint8_t> apply_ingress_pcp(uint8_t ingress_pcp) const noexcept {
        uint8_t p = regenerate_pcp(ingress_pcp);
        return {p, pcp_to_tc(p)};
    }

    TrafficClassConfig& tc(uint8_t idx) { return tc_cfg_.at(idx % num_tc_); }
    const TrafficClassConfig& tc(uint8_t idx) const { return tc_cfg_.at(idx % num_tc_); }

    // Hooks to export config to a shaper/scheduler implementation
    void apply_to_shaper(IShaper& shaper) const;   // defined in qos.cpp
    void apply_to_scheduler(IQueueScheduler& sched) const; // defined in qos.cpp

private:
    uint8_t num_tc_;
    PcpToTcMap pcp2tc_{};
    PriorityRegenerationTable regen_{};
    std::vector<TrafficClassConfig> tc_cfg_{};
};

}}} // namespace
