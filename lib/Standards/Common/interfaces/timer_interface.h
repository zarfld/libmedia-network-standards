#pragma once

/**
 * @file timer_interface.h
 * @brief Hardware-agnostic timing interface for all IEEE standards
 * @details Provides unified precision timing abstraction for nanosecond-accurate protocols
 * 
 * This interface MUST be used by ALL IEEE standards requiring precision timing.
 * Direct timing hardware calls are FORBIDDEN in lib/Standards/ namespaces.
 * 
 * @note Part of Phase 00 Cross-Standard Architecture Foundation
 * @see 03-architecture/phase00-cross-standard-architecture-foundation.md
 */

#include <cstdint>
#include <functional>

namespace Common {
namespace interfaces {

/**
 * @brief Hardware-agnostic timing interface for precision IEEE protocols
 * 
 * Provides unified abstraction for precision timing required by:
 * - IEEE 1588 (PTPv2 nanosecond timestamp precision)
 * - IEEE 802.1AS (gPTP ±80ns synchronization accuracy)
 * - IEEE 1722 (AVTP presentation time scheduling)
 * - IEEE 1722.1 (AVDECC command timeout handling)
 */
class TimerInterface {
public:
    virtual ~TimerInterface() = default;

    /**
     * @brief Get current time in nanoseconds since epoch
     * @return Nanosecond timestamp
     * 
     * CRITICAL: Must provide nanosecond precision for IEEE 1588 PTP
     * and IEEE 802.1AS gPTP synchronization accuracy requirements.
     * All IEEE timing protocols depend on this precision.
     */
    virtual uint64_t get_time_ns() = 0;

    /**
     * @brief Set periodic timer with callback
     * @param interval_us Timer interval in microseconds
     * @param callback Function to call on timer expiry
     * @return Timer ID on success, negative error code on failure
     * 
     * Used by:
     * - IEEE 1588 PTP for sync/announce message intervals
     * - IEEE 802.1AS gPTP for pdelay measurement intervals
     * - IEEE 1722.1 AVDECC for command timeout handling
     */
    virtual int set_timer(uint32_t interval_us, std::function<void()> callback) = 0;

    /**
     * @brief Cancel previously set timer
     * @param timer_id Timer ID returned by set_timer()
     * @return 0 on success, negative error code on failure
     */
    virtual int cancel_timer(int timer_id) = 0;

    /**
     * @brief Get hardware timestamp from network packet reception
     * @param timestamp_ns Output: hardware timestamp in nanoseconds
     * @return 0 on success, negative error code on failure
     * 
     * CRITICAL: Required for IEEE 802.1AS gPTP path delay measurement
     * accuracy. Hardware timestamping provides ±80ns accuracy needed
     * for Milan professional audio compliance.
     */
    virtual int get_hardware_timestamp(uint64_t* timestamp_ns) = 0;

    /**
     * @brief Enable/disable hardware timestamping
     * @param enabled true to enable hardware timestamps, false to disable
     * @return 0 on success, negative error code on failure
     * 
     * MUST be enabled for IEEE 802.1AS gPTP operation.
     * Software-only timestamps cannot meet ±80ns accuracy requirement.
     */
    virtual int enable_hardware_timestamping(bool enabled) = 0;

    /**
     * @brief Set one-shot timer with callback
     * @param delay_us Delay in microseconds before callback
     * @param callback Function to call after delay
     * @return Timer ID on success, negative error code on failure
     * 
     * Used by IEEE 1722 AVTP for presentation time scheduling
     * and IEEE 1722.1 AVDECC for response timeout handling.
     */
    virtual int set_oneshot_timer(uint32_t delay_us, std::function<void()> callback) = 0;

    /**
     * @brief Get timer resolution in nanoseconds
     * @return Timer resolution (smaller values = higher precision)
     * 
     * Used to validate timer precision meets IEEE standard requirements.
     * IEEE 802.1AS requires sub-microsecond timer resolution.
     */
    virtual uint32_t get_timer_resolution_ns() = 0;

    /**
     * @brief Sleep with nanosecond precision
     * @param duration_ns Sleep duration in nanoseconds
     * @return 0 on success, negative error code on failure
     * 
     * Used for precise timing delays in IEEE protocol implementations.
     * Provides higher precision than standard sleep functions.
     */
    virtual int nanosleep(uint64_t duration_ns) = 0;

    /**
     * @brief Get monotonic time (unaffected by system clock adjustments)
     * @return Monotonic nanosecond timestamp
     * 
     * Used for interval calculations that must not be affected by
     * IEEE 1588 PTP clock adjustments or system time changes.
     */
    virtual uint64_t get_monotonic_time_ns() = 0;
};

} // namespace interfaces
} // namespace Common