// Windows shaper adapter: concrete IShaper/IQueueScheduler using OpenAvnu shaper daemon APIs
#pragma once
#ifdef _WIN32

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>
#include "scheduler.h"

// Try to include the Windows shaper C API if available
#if defined(__has_include)
#  if __has_include("../../../daemons/shaper/windows/shaper_windows.h")
#    define OPENAVNU_HAVE_WINDOWS_SHAPER_API 1
extern "C" {
#    include "../../../daemons/shaper/windows/shaper_windows.h"
}
#  elif __has_include("daemons/shaper/windows/shaper_windows.h")
#    define OPENAVNU_HAVE_WINDOWS_SHAPER_API 1
extern "C" {
#    include "daemons/shaper/windows/shaper_windows.h"
}
#  endif
#endif

namespace IEEE { namespace _802_1Q { namespace _2020 {

#if OPENAVNU_HAVE_WINDOWS_SHAPER_API
class WindowsShaperAdapter : public IShaper, public IQueueScheduler {
public:
    explicit WindowsShaperAdapter(std::string ifname = "") : iface_(std::move(ifname)), configured_tc_(0), initialized_(false) {}
    ~WindowsShaperAdapter() override {
        if (initialized_) {
            shaper_windows_cleanup();
        }
    }

    bool initialize(uint32_t link_kbps = 1000000) {
        shaper_windows_config_t cfg{};
        if (!iface_.empty()) {
            strncpy(cfg.interface_name, iface_.c_str(), sizeof(cfg.interface_name)-1);
        }
        cfg.bandwidth_kbps = link_kbps;
        cfg.tsn_enabled = false;
        cfg.qos_enabled = true;
        cfg.intel_hal_available = false;
        int rc = shaper_windows_init(&cfg);
        initialized_ = (rc == 0);
        return initialized_;
    }

    // IQueueScheduler
    void configure_num_classes(uint8_t num_tc) override {
        configured_tc_ = num_tc ? (num_tc>8?8:num_tc) : 1;
        // No explicit API needed here; stored for class configuration
    }

    // IShaper
    void configure_class(uint8_t tc,
                         uint32_t idle_slope_kbps,
                         uint32_t /*send_slope_kbps*/,
                         uint32_t /*hi_credit_bytes*/,
                         uint32_t /*lo_credit_bytes*/) override {
        if (!initialized_) return;
        // Map tc to Windows QoS priority (0..7)
        uint8_t priority = static_cast<uint8_t>(tc % 8);
        // Use idle_slope as an approximation for bandwidth in kbps
        shaper_windows_configure_class(priority, idle_slope_kbps, /*burst*/ 1500u * 8u);
    }

    bool apply() {
        if (!initialized_) return false;
        return shaper_windows_apply_configuration() == 0;
    }

private:
    std::string iface_;
    uint8_t configured_tc_;
    bool initialized_;
};
#else
// Fallback no-op adapter: compiles without daemon headers; calls do nothing
class WindowsShaperAdapter : public IShaper, public IQueueScheduler {
public:
    explicit WindowsShaperAdapter(std::string /*ifname*/ = "") {}
    ~WindowsShaperAdapter() override = default;
    bool initialize(uint32_t /*link_kbps*/ = 1000000) { return false; }
    void configure_num_classes(uint8_t /*num_tc*/) override {}
    void configure_class(uint8_t /*tc*/, uint32_t /*idle_slope_kbps*/, uint32_t /*send_slope_kbps*/,
                         uint32_t /*hi_credit_bytes*/, uint32_t /*lo_credit_bytes*/) override {}
    bool apply() { return false; }
};
#endif

}}} // namespace

#endif // _WIN32
