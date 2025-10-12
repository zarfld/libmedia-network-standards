#pragma once

/**
 * @file clock_interface.h
 * @brief Hardware-agnostic system clock interface for IEEE timing protocols
 * @details Provides unified clock management abstraction for PTP synchronization
 * 
 * This interface MUST be used by IEEE 1588 PTP and IEEE 802.1AS gPTP
 * for system clock synchronization. Direct clock hardware calls are 
 * FORBIDDEN in lib/Standards/ namespaces.
 * 
 * @note Part of Phase 00 Cross-Standard Architecture Foundation
 * @see 03-architecture/phase00-cross-standard-architecture-foundation.md
 */

#include <cstdint>

namespace Common {
namespace interfaces {

/**
 * @brief Hardware-agnostic system clock interface for IEEE timing protocols
 * 
 * Provides unified abstraction for system clock management required by:
 * - IEEE 1588 (PTPv2 system clock synchronization)
 * - IEEE 802.1AS (gPTP clock servo algorithms)
 */
class ClockInterface {
public:
    virtual ~ClockInterface() = default;

    /**
     * @brief Get current system time in nanoseconds since epoch
     * @return System time in nanoseconds
     * 
     * CRITICAL: Must provide the time that will be synchronized
     * by IEEE 1588 PTP clock servo. This is the "system clock"
     * that applications use for timestamping.
     */
    virtual uint64_t get_system_time_ns() = 0;

    /**
     * @brief Set system time in nanoseconds since epoch
     * @param time_ns New system time in nanoseconds
     * @return 0 on success, negative error code on failure
     * 
     * Used by IEEE 1588 PTP clock servo for coarse clock corrections
     * when offset from master exceeds adjustment threshold.
     */
    virtual int set_system_time_ns(uint64_t time_ns) = 0;

    /**
     * @brief Adjust system clock frequency
     * @param ppb_adjustment Frequency adjustment in parts per billion (ppb)
     *                      Positive values speed up clock, negative slow down
     * @return 0 on success, negative error code on failure
     * 
     * CRITICAL: Used by IEEE 1588 PTP and IEEE 802.1AS clock servo
     * algorithms for fine frequency synchronization. Must support
     * at least ±100 ppm adjustment range for IEEE compliance.
     */
    virtual int adjust_clock_frequency(int32_t ppb_adjustment) = 0;

    /**
     * @brief Get current clock frequency adjustment
     * @return Current frequency adjustment in parts per billion (ppb)
     * 
     * Used by clock servo algorithms to read current adjustment
     * for proportional-integral (PI) control calculations.
     */
    virtual int32_t get_clock_frequency_adjustment() = 0;

    /**
     * @brief Step adjust system clock by offset
     * @param offset_ns Offset to add to system clock in nanoseconds  
     * @return 0 on success, negative error code on failure
     * 
     * Used by IEEE 1588 PTP for small clock corrections that don't
     * require frequency adjustment. Provides smooth corrections.
     */
    virtual int step_clock(int64_t offset_ns) = 0;

    /**
     * @brief Get clock capabilities and limitations
     * @param max_frequency_adjustment_ppb Output: maximum frequency adjustment
     * @param resolution_ns Output: clock resolution in nanoseconds
     * @param stability_ppb Output: clock stability in ppb
     * @return 0 on success, negative error code on failure
     * 
     * Used by IEEE 1588 PTP and IEEE 802.1AS to determine if hardware
     * meets timing accuracy requirements for synchronization.
     */
    virtual int get_clock_capabilities(int32_t* max_frequency_adjustment_ppb,
                                      uint32_t* resolution_ns,
                                      uint32_t* stability_ppb) = 0;

    /**
     * @brief Get clock status and synchronization state
     * @param synchronized Output: true if clock is synchronized to master
     * @param offset_from_master_ns Output: current offset from master in ns
     * @param frequency_adjustment_ppb Output: current frequency adjustment
     * @return 0 on success, negative error code on failure
     * 
     * Used by higher layer protocols to determine timing quality
     * and synchronization health for IEEE standards integration.
     */
    virtual int get_clock_status(bool* synchronized, 
                                int64_t* offset_from_master_ns,
                                int32_t* frequency_adjustment_ppb) = 0;

    /**
     * @brief Enable/disable clock disciplining 
     * @param enabled true to enable PTP clock disciplining, false to disable
     * @return 0 on success, negative error code on failure
     * 
     * Used to control whether IEEE 1588/802.1AS synchronization
     * actively disciplines the system clock or runs in monitor mode.
     */
    virtual int set_clock_disciplining(bool enabled) = 0;

    /**
     * @brief Get clock accuracy class per IEEE 1588
     * @return Clock accuracy class (IEEE 1588 Table 5)
     * 
     * Used for IEEE 1588 PTP announce message generation
     * and Best Master Clock Algorithm (BMCA) calculations.
     */
    virtual uint8_t get_clock_accuracy() = 0;

    /**
     * @brief Get clock class per IEEE 1588  
     * @return Clock class (IEEE 1588 Table 5)
     * 
     * Used for IEEE 1588 PTP announce message generation
     * and Best Master Clock Algorithm (BMCA) calculations.
     */
    virtual uint8_t get_clock_class() = 0;
};

} // namespace interfaces
} // namespace Common