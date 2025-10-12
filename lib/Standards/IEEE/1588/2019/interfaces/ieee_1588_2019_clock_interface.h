/**
 * @file ieee_1588_2019_clock_interface.h
 * @brief IEEE 1588-2019 specific clock interface for PTP implementation
 * @namespace IEEE::_1588::_2019::interfaces
 * 
 * Extends Common::interfaces::ClockInterface with IEEE 1588-2019 specific
 * clock control and synchronization capabilities required for PTP.
 * 
 * @see IEEE 1588-2019, Section 7.6 "Local clock"
 * @see ADR-003: IEEE 1588-2019 Architecture Implementation Strategy
 */

#ifndef IEEE_1588_2019_CLOCK_INTERFACE_H
#define IEEE_1588_2019_CLOCK_INTERFACE_H

#include "../../../Common/interfaces/clock_interface.h"
#include <cstdint>

namespace IEEE {
namespace _1588 {
namespace _2019 {
namespace interfaces {

/**
 * @brief IEEE 1588-2019 clock properties for PTP implementation
 */
struct PTPClockProperties {
    uint8_t clock_class;           ///< IEEE 1588-2019 Section 7.6.2.4
    uint8_t clock_accuracy;        ///< IEEE 1588-2019 Section 7.6.2.5
    uint16_t offset_scaled_log_variance; ///< IEEE 1588-2019 Section 7.6.2.6
    uint8_t priority1;             ///< IEEE 1588-2019 Section 7.6.2.1
    uint8_t priority2;             ///< IEEE 1588-2019 Section 7.6.2.2
    uint64_t clock_identity;       ///< IEEE 1588-2019 Section 7.5.2.2.2
    uint16_t time_source;          ///< IEEE 1588-2019 Section 7.6.2.7
};

/**
 * @brief IEEE 1588-2019 specific clock interface for PTP
 * 
 * Extends Common clock interface with PTP-specific clock control
 * and synchronization capabilities required by IEEE 1588-2019.
 */
class IEEE1588ClockInterface : public Common::interfaces::ClockInterface {
public:
    /**
     * @brief Adjust clock frequency for PTP synchronization
     * 
     * Performs fine frequency adjustment required for IEEE 1588-2019
     * PTP synchronization. Frequency adjustment is cumulative and
     * should support sub-ppm precision.
     *
     * @param frequency_ppb Frequency adjustment in parts per billion (ppb)
     *                     Positive values increase frequency, negative decrease
     * @return 0 on success, negative error code on failure
     * 
     * @see IEEE 1588-2019, Section 11.2.2 "Frequency adjustment"
     */
    virtual int adjust_frequency_ppb(double frequency_ppb) = 0;

    /**
     * @brief Step adjust clock time for large corrections
     * 
     * Performs step time adjustment for large time corrections that
     * exceed the slewing capability. Used for initial synchronization
     * or when offset exceeds threshold.
     *
     * @param time_adjustment_ns Time adjustment in nanoseconds
     *                          Positive values advance clock, negative retard
     * @return 0 on success, negative error code on failure
     * 
     * @see IEEE 1588-2019, Section 11.2.1 "Time adjustment"
     */
    virtual int step_adjust_time_ns(int64_t time_adjustment_ns) = 0;

    /**
     * @brief Set clock time with high precision
     * 
     * Sets the local clock time with nanosecond precision.
     * Used for initial clock synchronization in IEEE 1588-2019 PTP.
     *
     * @param time_ns Time in nanoseconds since PTP epoch (1970-01-01 TAI)
     * @return 0 on success, negative error code on failure
     * 
     * @see IEEE 1588-2019, Section 7.2.1 "Time representation"
     */
    virtual int set_time_ns(uint64_t time_ns) = 0;

    /**
     * @brief Get current clock time with high precision
     * 
     * Returns the current local clock time with nanosecond precision.
     * Critical for IEEE 1588-2019 timestamp generation.
     *
     * @return Current time in nanoseconds since PTP epoch, 0 on error
     * 
     * @see IEEE 1588-2019, Section 7.2.1 "Time representation"
     */
    virtual uint64_t get_time_ns() = 0;

    /**
     * @brief Get PTP clock properties for BMCA
     * 
     * Returns IEEE 1588-2019 clock properties used by Best Master Clock
     * Algorithm (BMCA) for clock selection and comparison.
     *
     * @param properties Output buffer for PTP clock properties
     * @return 0 on success, negative error code on failure
     * 
     * @see IEEE 1588-2019, Section 9.3 "Best master clock algorithm"
     */
    virtual int get_ptp_clock_properties(PTPClockProperties* properties) = 0;

    /**
     * @brief Set PTP clock properties
     * 
     * Configures IEEE 1588-2019 clock properties used for clock
     * identification and BMCA dataset comparison.
     *
     * @param properties PTP clock properties to configure
     * @return 0 on success, negative error code on failure
     * 
     * @see IEEE 1588-2019, Section 7.6 "Local clock"
     */
    virtual int set_ptp_clock_properties(const PTPClockProperties* properties) = 0;

    /**
     * @brief Get clock synchronization state
     * 
     * Returns the current IEEE 1588-2019 clock synchronization state
     * for determining clock reliability and accuracy.
     *
     * @return Clock state: 0=FREERUN, 1=HOLDOVER, 2=LOCKED, negative=ERROR
     * 
     * @see IEEE 1588-2019, Section 9.2.2 "Port state definitions"
     */
    virtual int get_synchronization_state() = 0;

    /**
     * @brief Enable or disable clock discipline
     * 
     * Enables or disables automatic clock discipline based on
     * IEEE 1588-2019 PTP synchronization messages.
     *
     * @param enable True to enable discipline, false to disable
     * @return 0 on success, negative error code on failure
     */
    virtual int enable_discipline(bool enable) = 0;

    /**
     * @brief Get clock statistics for monitoring
     * 
     * Provides IEEE 1588-2019 clock performance statistics for
     * monitoring and debugging synchronization quality.
     */
    struct PTPClockStatistics {
        double mean_offset_ns;      ///< Mean time offset from master (ns)
        double offset_variance_ns2; ///< Offset variance (ns²)
        double frequency_offset_ppb; ///< Frequency offset from master (ppb) 
        uint64_t sync_count;        ///< Number of Sync messages processed
        uint64_t step_adjustments;  ///< Number of step time adjustments
        uint64_t frequency_adjustments; ///< Number of frequency adjustments
    };

    /**
     * @brief Get PTP clock statistics
     * 
     * @param stats Output buffer for clock statistics
     * @return 0 on success, negative error code on failure
     */
    virtual int get_clock_statistics(PTPClockStatistics* stats) = 0;
};

} // namespace interfaces
} // namespace _2019
} // namespace _1588  
} // namespace IEEE

#endif // IEEE_1588_2019_CLOCK_INTERFACE_H