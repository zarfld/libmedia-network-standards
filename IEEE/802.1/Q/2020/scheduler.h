// Scheduler/Shaper interfaces for IEEE 802.1Q QoS integration
#pragma once
#include <cstdint>

namespace IEEE { namespace _802_1Q { namespace _2020 {

class IShaper {
public:
    virtual ~IShaper() = default;
    virtual void configure_class(uint8_t tc,
                                 uint32_t idle_slope_kbps,
                                 uint32_t send_slope_kbps,
                                 uint32_t hi_credit_bytes,
                                 uint32_t lo_credit_bytes) = 0;
};

class IQueueScheduler {
public:
    virtual ~IQueueScheduler() = default;
    virtual void configure_num_classes(uint8_t num_tc) = 0;
};

}}} // namespace
