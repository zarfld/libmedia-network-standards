#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <optional>
#include <unordered_map>

namespace IEEE { namespace _802_1Q { namespace _2020 {

// Very small forward database suitable for host-side learning tests
// NOT production-grade bridge FDB.

struct MacAddress; // forward decl; defined in IEEE_802_1_Q_2020.h

struct PortId { uint16_t id{0}; };

struct FdbEntry {
    std::array<uint8_t,6> mac{};
    PortId port{};
    uint64_t last_seen_ms{0};
    bool static_entry{false};
};

class Fdb {
public:
    // Learn/update entry
    void learn(const std::array<uint8_t,6>& mac, PortId port, uint64_t now_ms, bool is_static=false);
    // Lookup forwarding port
    std::optional<PortId> lookup(const std::array<uint8_t,6>& mac) const;
    // Aging (remove entries older than age_ms that are not static)
    void age(uint64_t now_ms, uint64_t age_ms);
    // Size
    size_t size() const noexcept { return map_.size(); }

private:
    struct KeyHash {
        size_t operator()(const std::array<uint8_t,6>& k) const noexcept {
            uint64_t v=0; for(int i=0;i<6;++i){ v = (v*131) ^ k[i]; } return static_cast<size_t>(v);
        }
    };
    std::unordered_map<std::array<uint8_t,6>, FdbEntry, KeyHash> map_;
};

}}} // namespace
