#include "fdb.h"

namespace IEEE { namespace _802_1Q { namespace _2020 {

void Fdb::learn(const std::array<uint8_t,6>& mac, PortId port, uint64_t now_ms, bool is_static) {
    auto& e = map_[mac];
    e.mac = mac;
    e.port = port;
    e.last_seen_ms = now_ms;
    if (is_static) e.static_entry = true; // static pin wins
}

std::optional<PortId> Fdb::lookup(const std::array<uint8_t,6>& mac) const {
    auto it = map_.find(mac);
    if (it == map_.end()) return std::nullopt;
    return it->second.port;
}

void Fdb::age(uint64_t now_ms, uint64_t age_ms) {
    for (auto it = map_.begin(); it != map_.end(); ) {
        const auto& e = it->second;
        if (!e.static_entry && now_ms - e.last_seen_ms > age_ms) it = map_.erase(it);
        else ++it;
    }
}

}}}
