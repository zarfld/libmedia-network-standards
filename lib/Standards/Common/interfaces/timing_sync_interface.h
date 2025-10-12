#pragma once

/**
 * @file timing_sync_interface.h  
 * @brief Cross-standard timing synchronization coordination interface
 * @details Provides IEEE 802.1AS → IEEE 1722 timing coordination abstraction
 * 
 * This interface enables IEEE 802.1AS gPTP to provide synchronized timing
 * to IEEE 1722 AVTP and IEEE 1722.1 AVDECC protocols without tight coupling.
 * 
 * @note Part of Phase 00 Cross-Standard Architecture Foundation
 * @see 03-architecture/phase00-cross-standard-architecture-foundation.md
 */

#include <cstdint>

namespace Common {
namespace interfaces {

/**
 * @brief Cross-standard timing synchronization coordination interface
 * 
 * Enables timing coordination between IEEE standards:
 * - IEEE 802.1AS gPTP provides synchronized time to higher layers
 * - IEEE 1722 AVTP consumes synchronized time for presentation timing
 * - IEEE 1722.1 AVDECC uses synchronized time for timestamping
 */
class TimingSyncInterface {
public:
    virtual ~TimingSyncInterface() = default;

    /**
     * @brief Get current synchronized time from IEEE 802.1AS gPTP
     * @return Synchronized time in nanoseconds, 0 if not synchronized
     * 
     * CRITICAL: This is the time synchronized across the IEEE 802.1AS
     * network domain. Used by IEEE 1722 AVTP for presentation time
     * calculations and stream synchronization.
     */
    virtual uint64_t get_synchronized_time() = 0;

    /**
     * @brief Check if timing is synchronized with network master  
     * @return true if synchronized to network master, false otherwise
     * 
     * Used by IEEE 1722 AVTP and IEEE 1722.1 AVDECC to determine
     * if timing information is valid for time-sensitive operations.
     */
    virtual bool is_synchronized() = 0;

    /**
     * @brief Get synchronization accuracy in nanoseconds
     * @return Synchronization accuracy in nanoseconds
     * 
     * Used by higher layer protocols to determine timing quality.
     * IEEE 802.1AS gPTP should provide ±80ns accuracy for Milan compliance.
     */
    virtual int32_t get_sync_accuracy_ns() = 0;

    /**
     * @brief Get grandmaster clock identity
     * @param clock_identity Output: 8-byte grandmaster clock identity
     * @return 0 on success, negative error code on failure
     * 
     * Used by IEEE 1722.1 AVDECC for network topology discovery
     * and timing domain identification.
     */
    virtual int get_grandmaster_identity(uint8_t clock_identity[8]) = 0;

    /**
     * @brief Get current path delay to grandmaster
     * @return Path delay in nanoseconds, 0 if unknown
     * 
     * Used by IEEE 1722 AVTP for accurate presentation time
     * calculations accounting for network propagation delay.
     */
    virtual uint64_t get_path_delay_ns() = 0;

    /**
     * @brief Get synchronization domain number
     * @return IEEE 802.1AS domain number (0-255)
     * 
     * Used for multi-domain timing coordination where different
     * IEEE standards may operate in different timing domains.
     */
    virtual uint8_t get_domain_number() = 0;

    /**
     * @brief Get timing synchronization status details
     * @param master_offset_ns Output: offset from master in nanoseconds
     * @param frequency_adjustment_ppb Output: current frequency adjustment
     * @param sync_interval_ms Output: sync message interval in milliseconds
     * @return 0 on success, negative error code on failure
     * 
     * Provides detailed timing status for IEEE protocol coordination
     * and network diagnostics.
     */
    virtual int get_sync_status(int64_t* master_offset_ns,
                               int32_t* frequency_adjustment_ppb,
                               uint32_t* sync_interval_ms) = 0;

    /**
     * @brief Register for timing synchronization state change notifications
     * @param callback Function to call on synchronization state changes
     * @return 0 on success, negative error code on failure
     * 
     * Used by IEEE 1722 AVTP and IEEE 1722.1 AVDECC to react to
     * timing synchronization loss or recovery events.
     */
    virtual int register_sync_callback(std::function<void(bool synchronized)> callback) = 0;

    /**
     * @brief Convert synchronized time to presentation time for AVTP
     * @param delay_ns Desired delay from current synchronized time  
     * @return Future presentation time in nanoseconds
     * 
     * Utility function for IEEE 1722 AVTP stream timing calculations.
     * Accounts for network delays and synchronization accuracy.
     */
    virtual uint64_t calculate_presentation_time(uint32_t delay_ns) = 0;

    /**
     * @brief Validate if timing meets IEEE standards requirements
     * @param required_accuracy_ns Required synchronization accuracy
     * @return true if timing meets requirements, false otherwise
     * 
     * Used by IEEE 1722 AVTP and IEEE 1722.1 AVDECC to validate
     * timing quality before enabling time-sensitive operations.
     */
    virtual bool validate_timing_requirements(uint32_t required_accuracy_ns) = 0;
};

} // namespace interfaces  
} // namespace Common