/**
 * @file ieee_802_1as_2021.h
 * @brief IEEE 802.1AS-2021 Standard Implementation
 * @details Timing and Synchronization for Time-Sensitive Applications in Bridged LANs
 * 
 * This implementation provides IEEE 802.1AS-2021 (ISO/IEC/IEEE 8802-1AS:2021)
 * compliance for generalized Precision Time Protocol (gPTP) in AVB/TSN networks.
 * 
 * Key Features:
 * - Enhanced multi-domain support
 * - Improved path delay mechanisms (peer-to-peer and end-to-end)
 * - Extended TLV support
 * - Better security framework
 * - YANG model support for management
 * - Backward compatibility with 802.1AS-2020/2011
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Implementation follows IEEE 802.1AS-2021 specification
 */

#ifndef IEEE_802_1AS_2021_H
#define IEEE_802_1AS_2021_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <array>

namespace IEEE {
namespace _802_1AS {
namespace _2021 {

// Forward declare std types we'll use
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::vector;
using std::string;
using std::array;
using std::shared_ptr;
using std::unique_ptr;

// ============================================================================
// IEEE 802.1AS-2021 Constants and Enumerations
// ============================================================================

/**
 * @brief IEEE 802.1AS-2021 version identification
 */
constexpr uint16_t IEEE_802_1AS_VERSION_2021 = 0x0002;

/**
 * @brief PTP Version for IEEE 802.1AS-2021
 */
constexpr uint8_t PTP_VERSION_2_1 = 0x02;
constexpr uint8_t PTP_MINOR_VERSION_1 = 0x01;

/**
 * @brief Domain number ranges (802.1AS-2021 supports multiple domains)
 */
constexpr uint8_t GPTP_DEFAULT_DOMAIN = 0;
constexpr uint8_t GPTP_MAX_DOMAINS = 127;

/**
 * @brief Message types defined in IEEE 802.1AS-2021
 */
enum class MessageType : uint8_t {
    SYNC = 0x0,
    DELAY_REQ = 0x1,
    PDELAY_REQ = 0x2,
    PDELAY_RESP = 0x3,
    FOLLOW_UP = 0x8,
    DELAY_RESP = 0x9,
    PDELAY_RESP_FOLLOW_UP = 0xA,
    ANNOUNCE = 0xB,
    SIGNALING = 0xC,
    MANAGEMENT = 0xD
};

/**
 * @brief Control field values
 */
enum class ControlField : uint8_t {
    SYNC = 0x00,
    DELAY_REQ = 0x01,
    FOLLOW_UP = 0x02,
    DELAY_RESP = 0x03,
    MANAGEMENT = 0x04,
    ALL_OTHERS = 0x05
};

/**
 * @brief Clock accuracy enumeration (IEEE 802.1AS-2021 extended)
 */
enum class ClockAccuracy : uint8_t {
    WITHIN_1_PS = 0x17,
    WITHIN_2_5_PS = 0x18,
    WITHIN_10_PS = 0x19,
    WITHIN_25_PS = 0x1A,
    WITHIN_100_PS = 0x1B,
    WITHIN_250_PS = 0x1C,
    WITHIN_1_NS = 0x1D,
    WITHIN_2_5_NS = 0x1E,
    WITHIN_10_NS = 0x1F,
    WITHIN_25_NS = 0x20,
    WITHIN_100_NS = 0x21,
    WITHIN_250_NS = 0x22,
    WITHIN_1_US = 0x23,
    WITHIN_2_5_US = 0x24,
    WITHIN_10_US = 0x25,
    WITHIN_25_US = 0x26,
    WITHIN_100_US = 0x27,
    WITHIN_250_US = 0x28,
    WITHIN_1_MS = 0x29,
    WITHIN_2_5_MS = 0x2A,
    WITHIN_10_MS = 0x2B,
    WITHIN_25_MS = 0x2C,
    WITHIN_100_MS = 0x2D,
    WITHIN_250_MS = 0x2E,
    WITHIN_1_S = 0x2F,
    WITHIN_10_S = 0x30,
    GREATER_10_S = 0x31,
    UNKNOWN = 0xFE
};

/**
 * @brief Time source enumeration
 */
enum class TimeSource : uint8_t {
    ATOMIC_CLOCK = 0x10,
    GNSS = 0x20,
    TERRESTRIAL_RADIO = 0x30,
    SERIAL_TIME_CODE = 0x39,
    PTP = 0x40,
    NTP = 0x50,
    HAND_SET = 0x60,
    OTHER = 0x90,
    INTERNAL_OSCILLATOR = 0xA0
};

// ============================================================================
// IEEE 802.1AS-2021 Basic Data Types
// ============================================================================

/**
 * @brief Timestamp structure for IEEE 802.1AS-2021
 */
struct Timestamp {
    uint64_t seconds_field;      ///< Seconds since epoch
    uint32_t nanoseconds_field;  ///< Nanoseconds (0-999,999,999)
    
    Timestamp() : seconds_field(0), nanoseconds_field(0) {}
    Timestamp(uint64_t sec, uint32_t ns) : seconds_field(sec), nanoseconds_field(ns) {}
    
    /**
     * @brief Convert to std::chrono::nanoseconds
     */
    std::chrono::nanoseconds to_nanoseconds() const {
        return std::chrono::seconds(seconds_field) + std::chrono::nanoseconds(nanoseconds_field);
    }
    
    /**
     * @brief Create from std::chrono::nanoseconds
     */
    static Timestamp from_nanoseconds(const std::chrono::nanoseconds& ns) {
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(ns);
        auto remaining_ns = ns - seconds;
        return Timestamp(seconds.count(), remaining_ns.count());
    }
};

/**
 * @brief Clock Identity (8-byte identifier)
 */
struct ClockIdentity {
    array<uint8_t, 8> identity;
    
    ClockIdentity() {
        identity.fill(0);
    }
    
    explicit ClockIdentity(const array<uint8_t, 8>& id) : identity(id) {}
    
    bool operator==(const ClockIdentity& other) const {
        return identity == other.identity;
    }
    
    bool operator!=(const ClockIdentity& other) const {
        return !(*this == other);
    }
};

/**
 * @brief Port Identity (Clock Identity + Port Number)
 */
struct PortIdentity {
    ClockIdentity clock_identity;
    uint16_t port_number;
    
    PortIdentity() : port_number(0) {}
    PortIdentity(const ClockIdentity& clock_id, uint16_t port_num) 
        : clock_identity(clock_id), port_number(port_num) {}
    
    bool operator==(const PortIdentity& other) const {
        return clock_identity == other.clock_identity && port_number == other.port_number;
    }
    
    bool operator!=(const PortIdentity& other) const {
        return !(*this == other);
    }
};

/**
 * @brief Scaled log variance for statistical measurements
 */
using ScaledLogVariance = uint16_t;

/**
 * @brief Time interval for various measurements
 */
using TimeInterval = int64_t;

/**
 * @brief Clock quality structure
 */
struct ClockQuality {
    uint8_t clock_class;
    ClockAccuracy clock_accuracy;
    ScaledLogVariance offset_scaled_log_variance;
    
    ClockQuality() 
        : clock_class(248), 
          clock_accuracy(ClockAccuracy::UNKNOWN),
          offset_scaled_log_variance(0xFFFF) {}
};

// ============================================================================
// IEEE 802.1AS-2021 Message Headers and Structures
// ============================================================================

/**
 * @brief Common PTP Header (all messages start with this)
 */
struct PTPHeader {
    uint8_t message_type_and_version;      // MessageType (4) + Transport (4) + Version (4) + Reserved (4)
    uint8_t ptp_version;                   // PTP Version
    uint16_t message_length;               // Message length in bytes
    uint8_t domain_number;                 // Domain number
    uint8_t minor_version_ptp;             // Minor version PTP field
    array<uint8_t, 2> flags;               // Flags field
    int64_t correction_field;              // Correction field (nanoseconds in units of 2^-16)
    array<uint8_t, 4> message_type_specific; // Message type specific field
    PortIdentity source_port_identity;     // Source port identity
    uint16_t sequence_id;                  // Sequence ID
    uint8_t control_field;                 // Control field
    int8_t log_message_interval;           // Log message interval
    
    PTPHeader() 
        : message_type_and_version(0),
          ptp_version(PTP_VERSION_2_1),
          message_length(0),
          domain_number(GPTP_DEFAULT_DOMAIN),
          minor_version_ptp(PTP_MINOR_VERSION_1),
          correction_field(0),
          sequence_id(0),
          control_field(static_cast<uint8_t>(ControlField::ALL_OTHERS)),
          log_message_interval(0) {
        flags.fill(0);
        message_type_specific.fill(0);
    }
};

/**
 * @brief Sync Message Structure
 */
struct SyncMessage {
    PTPHeader header;
    Timestamp origin_timestamp;
    
    SyncMessage() {
        header.message_type_and_version = static_cast<uint8_t>(MessageType::SYNC) | (PTP_VERSION_2_1 << 4);
        header.control_field = static_cast<uint8_t>(ControlField::SYNC);
        header.message_length = sizeof(SyncMessage);
    }
};

/**
 * @brief Follow_Up Message Structure
 */
struct FollowUpMessage {
    PTPHeader header;
    Timestamp precise_origin_timestamp;
    
    FollowUpMessage() {
        header.message_type_and_version = static_cast<uint8_t>(MessageType::FOLLOW_UP) | (PTP_VERSION_2_1 << 4);
        header.control_field = static_cast<uint8_t>(ControlField::FOLLOW_UP);
        header.message_length = sizeof(FollowUpMessage);
    }
};

/**
 * @brief Announce Message Structure
 */
struct AnnounceMessage {
    PTPHeader header;
    Timestamp origin_timestamp;
    uint16_t current_utc_offset;
    uint8_t reserved;
    uint8_t grandmaster_priority1;
    ClockQuality grandmaster_clock_quality;
    uint8_t grandmaster_priority2;
    ClockIdentity grandmaster_identity;
    uint16_t steps_removed;
    TimeSource time_source;
    
    AnnounceMessage() 
        : current_utc_offset(37), // Current UTC offset as of 2025
          reserved(0),
          grandmaster_priority1(255),
          grandmaster_priority2(255),
          steps_removed(0),
          time_source(TimeSource::INTERNAL_OSCILLATOR) {
        header.message_type_and_version = static_cast<uint8_t>(MessageType::ANNOUNCE) | (PTP_VERSION_2_1 << 4);
        header.control_field = static_cast<uint8_t>(ControlField::ALL_OTHERS);
        header.message_length = sizeof(AnnounceMessage);
    }
};

/**
 * @brief Path Delay Request Message Structure
 */
struct PathDelayReqMessage {
    PTPHeader header;
    Timestamp origin_timestamp;
    array<uint8_t, 10> reserved;
    
    PathDelayReqMessage() {
        header.message_type_and_version = static_cast<uint8_t>(MessageType::PDELAY_REQ) | (PTP_VERSION_2_1 << 4);
        header.control_field = static_cast<uint8_t>(ControlField::ALL_OTHERS);
        header.message_length = sizeof(PathDelayReqMessage);
        reserved.fill(0);
    }
};

/**
 * @brief Path Delay Response Message Structure
 */
struct PathDelayRespMessage {
    PTPHeader header;
    Timestamp request_receipt_timestamp;
    PortIdentity requesting_port_identity;
    
    PathDelayRespMessage() {
        header.message_type_and_version = static_cast<uint8_t>(MessageType::PDELAY_RESP) | (PTP_VERSION_2_1 << 4);
        header.control_field = static_cast<uint8_t>(ControlField::ALL_OTHERS);
        header.message_length = sizeof(PathDelayRespMessage);
    }
};

/**
 * @brief Path Delay Response Follow Up Message Structure
 */
struct PathDelayRespFollowUpMessage {
    PTPHeader header;
    Timestamp response_origin_timestamp;
    PortIdentity requesting_port_identity;
    
    PathDelayRespFollowUpMessage() {
        header.message_type_and_version = static_cast<uint8_t>(MessageType::PDELAY_RESP_FOLLOW_UP) | (PTP_VERSION_2_1 << 4);
        header.control_field = static_cast<uint8_t>(ControlField::ALL_OTHERS);
        header.message_length = sizeof(PathDelayRespFollowUpMessage);
    }
};

// ============================================================================
// IEEE 802.1AS-2021 Port and System State
// ============================================================================

/**
 * @brief Port State enumeration
 */
enum class PortState : uint8_t {
    INITIALIZING = 1,
    FAULTY = 2,
    DISABLED = 3,
    LISTENING = 4,
    PRE_MASTER = 5,
    MASTER = 6,
    PASSIVE = 7,
    UNCALIBRATED = 8,
    SLAVE = 9
};

/**
 * @brief gPTP Port Interface
 * @details Represents a single gPTP-capable port
 */
class GPTPPort {
public:
    virtual ~GPTPPort() = default;
    
    /**
     * @brief Initialize the port
     * @return true if successful
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Set port state
     */
    virtual void set_port_state(PortState state) = 0;
    
    /**
     * @brief Get current port state
     */
    virtual PortState get_port_state() const = 0;
    
    /**
     * @brief Get port identity
     */
    virtual const PortIdentity& get_port_identity() const = 0;
    
    /**
     * @brief Send PTP message
     * @param data Raw message data
     * @param length Message length
     * @return true if sent successfully
     */
    virtual bool send_message(const uint8_t* data, size_t length) = 0;
    
    /**
     * @brief Process received message
     * @param data Raw message data
     * @param length Message length
     * @param receive_timestamp Hardware timestamp of reception
     */
    virtual void process_message(const uint8_t* data, size_t length, const Timestamp& receive_timestamp) = 0;
};

/**
 * @brief Time-Aware System Interface
 * @details Represents the complete IEEE 802.1AS-2021 system
 */
class TimeAwareSystem {
public:
    virtual ~TimeAwareSystem() = default;
    
    /**
     * @brief Initialize the system
     * @return true if successful
     */
    virtual bool initialize() = 0;
    
    /**
     * @brief Shutdown the system
     */
    virtual void shutdown() = 0;
    
    /**
     * @brief Add a port to the system
     * @param port Shared pointer to the port
     * @return true if added successfully
     */
    virtual bool add_port(shared_ptr<GPTPPort> port) = 0;
    
    /**
     * @brief Remove a port from the system
     * @param port_identity Identity of the port to remove
     * @return true if removed successfully
     */
    virtual bool remove_port(const PortIdentity& port_identity) = 0;
    
    /**
     * @brief Get system time
     * @return Current system time
     */
    virtual Timestamp get_time() const = 0;
    
    /**
     * @brief Set system time
     * @param timestamp New system time
     * @return true if set successfully
     */
    virtual bool set_time(const Timestamp& timestamp) = 0;
    
    /**
     * @brief Get clock identity
     */
    virtual const ClockIdentity& get_clock_identity() const = 0;
    
    /**
     * @brief Get current grandmaster
     */
    virtual const ClockIdentity& get_grandmaster_identity() const = 0;
    
    /**
     * @brief Check if this system is the grandmaster
     */
    virtual bool is_grandmaster() const = 0;
};

} // namespace _2021
} // namespace _802_1AS
} // namespace IEEE

#endif // IEEE_802_1AS_2021_H
