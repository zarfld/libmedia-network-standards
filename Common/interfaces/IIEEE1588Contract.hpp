/**
 * @file IIEEE1588Contract.hpp
 * @brief Abstract interface contract for IEEE 1588 Precision Time Protocol
 * 
 * This interface defines the stable contract that any IEEE 1588 implementation
 * must provide to be compatible with dependent standards (IEEE 802.1AS, IEEE 1722, etc.)
 * 
 * Design Principles:
 * - Version-agnostic: Compatible with IEEE 1588-2019, 2008, 2002
 * - Minimal contract: Only essential operations required by dependent standards
 * - Capability detection: Runtime query for version-specific features
 * - Zero overhead: Pure virtual with no runtime penalty when inlined
 * - Time-sensitive safe: No dynamic allocation, exceptions, or blocking
 * 
 * @copyright Copyright (C) 2025 OpenAvnu Project
 * @license BSD-3-Clause
 */

#ifndef STANDARDS_INTERFACES_IIEEE1588CONTRACT_HPP
#define STANDARDS_INTERFACES_IIEEE1588CONTRACT_HPP

#include <cstdint>
#include <cstddef>

namespace Standards {
namespace Interfaces {

/**
 * @class IIEEE1588Contract
 * @brief Abstract interface for IEEE 1588 Precision Time Protocol
 * 
 * This interface provides a stable contract across IEEE 1588 versions.
 * Dependent standards link against this interface, not concrete implementations.
 * 
 * Version Support Matrix:
 * - IEEE 1588-2002: Basic sync, BMCA
 * - IEEE 1588-2008: + Peer delay, transparent/boundary clocks
 * - IEEE 1588-2019: + Security, multi-domain, enhanced accuracy
 * 
 * Usage Example:
 * @code
 * IIEEE1588Contract* ptp = GetIEEE1588Implementation();
 * 
 * // Check version compatibility
 * if (ptp->hasCapability(IIEEE1588Contract::CAP_BASIC_SYNC)) {
 *     IIEEE1588Contract::Timestamp now;
 *     ptp->getCurrentTime(now);
 * }
 * @endcode
 */
class IIEEE1588Contract {
public:
    virtual ~IIEEE1588Contract() = default;
    
    // ========== Version and Capability Detection ==========
    
    /**
     * @brief Get the IEEE 1588 version implemented
     * @return Version string (e.g., "2019", "2008", "2002")
     * @note Thread-safe, no side effects
     */
    virtual const char* getVersion() const noexcept = 0;
    
    /**
     * @brief Query if specific capability is supported
     * @param capability Capability bitmask (see Capability enum)
     * @return true if ALL specified capabilities supported, false otherwise
     * @note Thread-safe, O(1) complexity
     */
    virtual bool hasCapability(uint32_t capability) const noexcept = 0;
    
    /**
     * @enum Capability
     * @brief Feature capability flags for runtime detection
     * 
     * Allows dependent standards to detect and adapt to available features.
     * Capabilities are grouped by IEEE 1588 version introduction.
     */
    enum Capability : uint32_t {
        // ===== Core Capabilities (All Versions) =====
        
        /// Basic sync message support (IEEE 1588-2002+)
        CAP_BASIC_SYNC           = 0x00000001,
        
        /// Delay request/response mechanism (IEEE 1588-2002+)
        CAP_DELAY_MECHANISM      = 0x00000002,
        
        /// Best Master Clock Algorithm (IEEE 1588-2002+)
        CAP_BEST_MASTER_CLOCK    = 0x00000004,
        
        /// Follow-up message support (IEEE 1588-2002+)
        CAP_FOLLOW_UP            = 0x00000008,
        
        // ===== IEEE 1588-2008 Capabilities =====
        
        /// Peer-to-peer delay mechanism (IEEE 1588-2008+)
        CAP_PEER_DELAY           = 0x00000010,
        
        /// Transparent clock support (IEEE 1588-2008+)
        CAP_TRANSPARENT_CLOCK    = 0x00000020,
        
        /// Boundary clock support (IEEE 1588-2008+)
        CAP_BOUNDARY_CLOCK       = 0x00000040,
        
        /// Management message support (IEEE 1588-2008+)
        CAP_MANAGEMENT           = 0x00000080,
        
        // ===== IEEE 1588-2019 Capabilities =====
        
        /// Security mechanisms (IEEE 1588-2019+)
        CAP_SECURITY             = 0x00000100,
        
        /// Multi-domain support (IEEE 1588-2019+)
        CAP_MULTI_DOMAIN         = 0x00000200,
        
        /// Enhanced calibration and accuracy (IEEE 1588-2019+)
        CAP_ENHANCED_ACCURACY    = 0x00000400,
        
        /// Common Mean Link Delay Service (IEEE 1588-2019+)
        CAP_CMLDS                = 0x00000800,
        
        /// Alternate timescales (IEEE 1588-2019+)
        CAP_ALTERNATE_TIMESCALE  = 0x00001000,
        
        // ===== Convenience Masks =====
        
        /// All IEEE 1588-2002 capabilities
        CAP_IEEE1588_2002 = CAP_BASIC_SYNC | CAP_DELAY_MECHANISM | 
                           CAP_BEST_MASTER_CLOCK | CAP_FOLLOW_UP,
        
        /// All IEEE 1588-2008 capabilities (includes 2002)
        CAP_IEEE1588_2008 = CAP_IEEE1588_2002 | CAP_PEER_DELAY | 
                           CAP_TRANSPARENT_CLOCK | CAP_BOUNDARY_CLOCK | 
                           CAP_MANAGEMENT,
        
        /// All IEEE 1588-2019 capabilities (includes 2008)
        CAP_IEEE1588_2019 = CAP_IEEE1588_2008 | CAP_SECURITY | 
                           CAP_MULTI_DOMAIN | CAP_ENHANCED_ACCURACY | 
                           CAP_CMLDS | CAP_ALTERNATE_TIMESCALE
    };
    
    // ========== Core Data Structures ==========
    
    /**
     * @struct Timestamp
     * @brief PTP timestamp structure (common across all versions)
     * 
     * Represents time as seconds + nanoseconds since PTP epoch (1970-01-01 00:00:00 TAI).
     * This format is consistent across IEEE 1588-2002, 2008, and 2019.
     */
    struct Timestamp {
        uint64_t seconds;        ///< Seconds since PTP epoch (1970-01-01 TAI)
        uint32_t nanoseconds;    ///< Nanoseconds (0-999,999,999)
        
        /// Default constructor (zero timestamp)
        constexpr Timestamp() noexcept : seconds(0), nanoseconds(0) {}
        
        /// Explicit constructor
        constexpr Timestamp(uint64_t s, uint32_t ns) noexcept 
            : seconds(s), nanoseconds(ns) {}
        
        /// Compare timestamps for equality
        constexpr bool operator==(const Timestamp& other) const noexcept {
            return seconds == other.seconds && nanoseconds == other.nanoseconds;
        }
        
        /// Compare timestamps for inequality
        constexpr bool operator!=(const Timestamp& other) const noexcept {
            return !(*this == other);
        }
    };
    
    /**
     * @struct ClockIdentity
     * @brief Unique clock identifier (8 bytes, standard across versions)
     * 
     * Typically derived from IEEE EUI-64 or MAC address.
     * Format: [manufacturer OUI (3 bytes)][0xFF][0xFE][device ID (3 bytes)]
     */
    struct ClockIdentity {
        uint8_t id[8];  ///< 8-byte unique identifier
        
        /// Default constructor (zero identity)
        constexpr ClockIdentity() noexcept : id{0} {}
        
        /// Compare identities for equality
        constexpr bool operator==(const ClockIdentity& other) const noexcept {
            for (size_t i = 0; i < 8; ++i) {
                if (id[i] != other.id[i]) return false;
            }
            return true;
        }
    };
    
    /**
     * @struct PortIdentity
     * @brief Port identity combining clock identity and port number
     */
    struct PortIdentity {
        ClockIdentity clock_identity;  ///< Parent clock identity
        uint16_t port_number;          ///< Port number (1-based)
        
        constexpr PortIdentity() noexcept : clock_identity(), port_number(0) {}
    };
    
    /**
     * @struct ClockQuality
     * @brief Clock quality metrics (compatible across versions)
     * 
     * Describes the quality and accuracy characteristics of a PTP clock.
     * Used in Best Master Clock Algorithm (BMCA) for master selection.
     */
    struct ClockQuality {
        /// Clock class (Section 7.6.2.2)
        /// Values: 6=primary reference, 52=holdover, 187=degraded, 255=slave-only
        uint8_t clock_class;
        
        /// Clock accuracy enumeration (Section 7.6.2.3)
        /// Values: 0x20=25ns, 0x21=100ns, 0x22=250ns, 0x23=1us, etc.
        uint8_t clock_accuracy;
        
        /// Offset scaled log variance (Section 7.6.2.4)
        /// Allan variance characterization of clock stability
        uint16_t offset_scaled_log_variance;
        
        constexpr ClockQuality() noexcept 
            : clock_class(255), clock_accuracy(0xFE), 
              offset_scaled_log_variance(0xFFFF) {}
    };
    
    // ========== Core Timing Operations ==========
    
    /**
     * @brief Get current PTP time
     * @param[out] timestamp Output timestamp structure
     * @return 0 on success, negative error code on failure
     * @note Time-sensitive safe: no dynamic allocation, deterministic timing
     */
    virtual int getCurrentTime(Timestamp& timestamp) const noexcept = 0;
    
    /**
     * @brief Get local clock identity
     * @param[out] clock_id Output clock identity
     * @return 0 on success, negative error code on failure
     */
    virtual int getClockIdentity(ClockIdentity& clock_id) const noexcept = 0;
    
    /**
     * @brief Get local port identity for specified port
     * @param port_number Port number (1-based, 0 for default)
     * @param[out] port_id Output port identity
     * @return 0 on success, negative error code on failure
     */
    virtual int getPortIdentity(uint16_t port_number, 
                                PortIdentity& port_id) const noexcept = 0;
    
    // ========== Clock Quality Assessment ==========
    
    /**
     * @brief Get current clock quality metrics
     * @param[out] quality Output clock quality structure
     * @return 0 on success, negative error code on failure
     * @note Used by dependent standards for quality assessment
     */
    virtual int getClockQuality(ClockQuality& quality) const noexcept = 0;
    
    /**
     * @brief Get priority1 value (used in BMCA)
     * @return Priority1 value (0-255, lower is better)
     */
    virtual uint8_t getPriority1() const noexcept = 0;
    
    /**
     * @brief Get priority2 value (used in BMCA)
     * @return Priority2 value (0-255, lower is better)
     */
    virtual uint8_t getPriority2() const noexcept = 0;
    
    // ========== Synchronization State ==========
    
    /**
     * @enum SyncState
     * @brief Port synchronization states (IEEE 1588 Section 9.2.5)
     */
    enum SyncState : uint8_t {
        SYNC_STATE_INITIALIZING = 0,  ///< Initial power-up state
        SYNC_STATE_FAULTY       = 1,  ///< Fault detected
        SYNC_STATE_DISABLED     = 2,  ///< Port disabled
        SYNC_STATE_LISTENING    = 3,  ///< Listening for announce
        SYNC_STATE_PRE_MASTER   = 4,  ///< Pre-master state
        SYNC_STATE_MASTER       = 5,  ///< Master state (sending sync)
        SYNC_STATE_PASSIVE      = 6,  ///< Passive (not best master)
        SYNC_STATE_UNCALIBRATED = 7,  ///< Uncalibrated slave
        SYNC_STATE_SLAVE        = 8   ///< Synchronized slave
    };
    
    /**
     * @brief Get current synchronization state
     * @param port_number Port number (0 for default port)
     * @return Current sync state
     */
    virtual SyncState getSyncState(uint16_t port_number = 0) const noexcept = 0;
    
    /**
     * @brief Get offset from master (if in slave state)
     * @param port_number Port number (0 for default port)
     * @return Offset in nanoseconds (0 if not synchronized)
     * @note Positive = local clock ahead, negative = local clock behind
     */
    virtual int64_t getOffsetFromMaster(uint16_t port_number = 0) const noexcept = 0;
    
    /**
     * @brief Get mean path delay to master
     * @param port_number Port number (0 for default port)
     * @return Path delay in nanoseconds (0 if unknown)
     */
    virtual uint64_t getMeanPathDelay(uint16_t port_number = 0) const noexcept = 0;
    
    // ========== Error Codes ==========
    
    enum ErrorCode : int {
        SUCCESS              = 0,    ///< Operation successful
        ERROR_NOT_INITIALIZED = -1,   ///< PTP not initialized
        ERROR_INVALID_PORT   = -2,   ///< Invalid port number
        ERROR_NOT_SUPPORTED  = -3,   ///< Operation not supported by version
        ERROR_NO_MASTER      = -4,   ///< Not synchronized to master
        ERROR_HARDWARE       = -5,   ///< Hardware error
        ERROR_TIMEOUT        = -6    ///< Operation timeout
    };
};

} // namespace Interfaces
} // namespace Standards

#endif // STANDARDS_INTERFACES_IIEEE1588CONTRACT_HPP
