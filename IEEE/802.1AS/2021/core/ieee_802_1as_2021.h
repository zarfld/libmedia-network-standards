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

// Windows SDK compatibility fixes (moved to implementation files only)
#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
#endif

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <array>

namespace IEEE {
namespace _802_1 {
namespace AS {
namespace _2021 {

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
// REVIEW [IEEE 802.1AS-2021, 10.2]: Version/minor handling is contextual; ensure Announce and Sync PDUs encode version fields per Table/Clause.

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
 * @brief Transport mechanisms (802.1AS-2021 expanded support)
 */
enum class TransportMechanism : uint8_t {
    ETHERNET = 0x00,
    IEEE_802_11 = 0x01,
    CSN = 0x02,          // Coordinated Shared Networks
    TIME_SYNC_INTERFACE = 0x03
};

/**
 * @brief Path delay mechanisms (802.1AS-2021 enhanced)
 */
enum class PathDelayMechanism : uint8_t {
    E2E = 0x01,          // End-to-End (new in 2021)
    P2P = 0x02,          // Peer-to-Peer (traditional)
    DISABLED = 0xFE
};

/**
 * @brief Clock identity type (IEEE 1588 / 802.1AS format)
 */
using ClockIdentity = std::array<uint8_t, 8>;

/**
 * @brief Port identity structure
 */
struct PortIdentity {
    ClockIdentity clock_identity;
    uint16_t port_number;
    
    PortIdentity() : port_number(0) {
        clock_identity.fill(0);
    }
};

/**
 * @brief Time interval (nanoseconds, signed 64-bit)
 */
using TimeInterval = int64_t;

/**
 * @brief Helper for 48-bit unsigned integer (IEEE 1588 timestamp seconds field)
 */
struct uint48_t {
    uint64_t value : 48;
    
    uint48_t(uint64_t val = 0) : value(val & 0xFFFFFFFFFFFFULL) {}
    operator uint64_t() const { return value; }
    uint48_t& operator=(uint64_t val) { value = val & 0xFFFFFFFFFFFFULL; return *this; }
};

/**
 * @brief Timestamp structure (802.1AS-2021 precision)
 */
struct Timestamp {
    uint48_t seconds_field;      // 48-bit seconds
    uint32_t nanoseconds_field;  // 32-bit nanoseconds
    
    Timestamp() : seconds_field(0), nanoseconds_field(0) {}
    Timestamp(uint64_t sec, uint32_t nsec) : seconds_field(sec), nanoseconds_field(nsec) {}
    
    // Convert to nanoseconds (for arithmetic)
    uint64_t to_nanoseconds() const {
        return static_cast<uint64_t>(seconds_field) * 1000000000ULL + nanoseconds_field;
    }
    
    // Create from nanoseconds
    static Timestamp from_nanoseconds(uint64_t ns) {
        return Timestamp(ns / 1000000000ULL, ns % 1000000000ULL);
    }
    
    // Arithmetic operators
    Timestamp operator-(const Timestamp& other) const {
        uint64_t this_ns = to_nanoseconds();
        uint64_t other_ns = other.to_nanoseconds();
        if (this_ns >= other_ns) {
            return from_nanoseconds(this_ns - other_ns);
        } else {
            return from_nanoseconds(0); // Avoid negative timestamps
        }
    }
    
    Timestamp operator+(const Timestamp& other) const {
        return from_nanoseconds(to_nanoseconds() + other.to_nanoseconds());
    }
    
    bool operator<(const Timestamp& other) const {
        return to_nanoseconds() < other.to_nanoseconds();
    }
    
    bool operator>(const Timestamp& other) const {
        return to_nanoseconds() > other.to_nanoseconds();
    }
    
    bool operator==(const Timestamp& other) const {
        return to_nanoseconds() == other.to_nanoseconds();
    }
};

// ============================================================================
// IEEE 802.1AS-2021 Clock Quality Structure
// ============================================================================

/**
 * @brief Clock quality definition per IEEE 802.1AS-2021
 */
struct ClockQuality {
    uint8_t clock_class;                    ///< Clock class (0-255)
    uint8_t clock_accuracy;                 ///< Clock accuracy enumeration
    uint16_t offset_scaled_log_variance;    ///< Scaled log variance
    
    ClockQuality() 
        : clock_class(248)           // gPTP default
        , clock_accuracy(0xFE)       // Unknown accuracy
        , offset_scaled_log_variance(0x4E5D) {}
};

// ============================================================================
// IEEE 802.1AS-2021 Message Headers and Structures
// ============================================================================

/**
 * @brief Common PTP header for all 802.1AS-2021 messages
 */
struct PTPHeader {
    uint8_t message_type : 4;
    uint8_t transport_specific : 4;
    uint8_t version_ptp;
    uint16_t message_length;
    uint8_t domain_number;
    uint8_t minor_version_ptp;
    uint16_t flags;
    int64_t correction_field;
    uint32_t message_type_specific;
    PortIdentity source_port_identity;
    uint16_t sequence_id;
    uint8_t control_field;
    int8_t log_message_interval;
    
    PTPHeader();
};

/**
 * @brief Announce message body (802.1AS-2021)
 */
struct AnnounceMessage {
    PTPHeader header;
    Timestamp origin_timestamp;
    int16_t current_utc_offset;
    uint8_t priority1;
    ClockQuality grandmaster_clock_quality;
    ClockIdentity grandmaster_identity;
    uint16_t steps_removed;
    uint8_t time_source;
    uint8_t priority2;  // Added missing priority2 field
    
    AnnounceMessage();
};

/**
 * @brief Sync message body (802.1AS-2021)
 */
struct SyncMessage {
    PTPHeader header;
    Timestamp origin_timestamp;
    
    SyncMessage();
};

/**
 * @brief Follow_Up message body (802.1AS-2021)
 */
struct FollowUpMessage {
    PTPHeader header;
    Timestamp precise_origin_timestamp;
    
    FollowUpMessage();
};

/**
 * @brief PDelay_Req message body (802.1AS-2021)
 */
struct PDelayReqMessage {
    PTPHeader header;
    Timestamp origin_timestamp;
    
    PDelayReqMessage();
};

/**
 * @brief PDelay_Resp message body (802.1AS-2021)
 */
struct PDelayRespMessage {
    PTPHeader header;
    Timestamp request_receipt_timestamp;
    PortIdentity requesting_port_identity;
    
    PDelayRespMessage();
};

/**
 * @brief PDelay_Resp_Follow_Up message body (802.1AS-2021)
 */
struct PDelayRespFollowUpMessage {
    PTPHeader header;
    Timestamp response_origin_timestamp;
    PortIdentity requesting_port_identity;
    
    PDelayRespFollowUpMessage();
};

// ============================================================================
// IEEE 802.1AS-2021 Extended Data Structures
// ============================================================================

/**
 * @brief Milan profile extensions per IEEE 802.1AS-2021
 */
struct MilanProfileData {
    bool multiplePDelayRespDetected = false;     ///< Multiple PDelay responses detected
    uint32_t multiplePDelayRespCount = 0;        ///< Count of multiple responses
    std::chrono::steady_clock::time_point lastMultipleDetection;
    bool pDelayTxCessation = false;             ///< PDelay transmission cessation
    uint32_t consecutivePDelayTimeouts = 0;     ///< Consecutive PDelay timeouts
};

/**
 * @brief Statistics collection structure
 */
struct StatisticsData {
    uint64_t messagesTransmitted = 0;
    uint64_t messagesReceived = 0;
    uint64_t errorsDetected = 0;
    uint64_t timeoutsOccurred = 0;
    std::chrono::steady_clock::time_point lastActivity;
};

// ============================================================================
// IEEE 802.1AS-2021 TLV Support (Enhanced in 2021)
// ============================================================================

/**
 * @brief Base TLV structure for 802.1AS-2021 extensions
 */
struct TLV {
    uint16_t tlv_type;
    uint16_t length_field;
    std::vector<uint8_t> value;
    
    TLV(uint16_t type = 0, uint16_t length = 0) 
        : tlv_type(type), length_field(length) {}
};

/**
 * @brief Organization Extension TLV (802.1AS-2021)
 */
struct OrganizationExtensionTLV : public TLV {
    std::array<uint8_t, 3> organization_id;
    std::array<uint8_t, 3> organization_sub_type;
    
    OrganizationExtensionTLV();
};

// ============================================================================
// IEEE 802.1AS-2021 Time-Aware System Interface
// ============================================================================

/**
 * @brief Time-Aware System representation per 802.1AS-2021
 */
class TimeAwareSystem {
public:
    TimeAwareSystem();
    virtual ~TimeAwareSystem(); // Declared here, defined in cpp file
    
    // System initialization and configuration
    bool initialize(uint8_t domain_number = GPTP_DEFAULT_DOMAIN);
    bool shutdown();
    
    // Multi-domain support (enhanced in 802.1AS-2021)
    bool add_domain(uint8_t domain_number);
    bool remove_domain(uint8_t domain_number);
    std::vector<uint8_t> get_active_domains() const;
    
    // Time synchronization interface
    bool get_time(Timestamp& current_time, uint8_t domain = GPTP_DEFAULT_DOMAIN) const;
    bool set_time(const Timestamp& time, uint8_t domain = GPTP_DEFAULT_DOMAIN);
    
    // Clock quality and performance monitoring
    ClockQuality get_clock_quality(uint8_t domain = GPTP_DEFAULT_DOMAIN) const;
    bool is_synchronized(uint8_t domain = GPTP_DEFAULT_DOMAIN) const;
    TimeInterval get_offset_from_master(uint8_t domain = GPTP_DEFAULT_DOMAIN) const;
    
    // Path delay mechanism selection (802.1AS-2021 enhancement)
    bool set_path_delay_mechanism(PathDelayMechanism mechanism, uint8_t domain = GPTP_DEFAULT_DOMAIN);
    PathDelayMechanism get_path_delay_mechanism(uint8_t domain = GPTP_DEFAULT_DOMAIN) const;
    
    // Transport mechanism support
    bool add_transport_mechanism(TransportMechanism transport);
    std::vector<TransportMechanism> get_supported_transports() const;
    
private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

// ============================================================================
// IEEE 802.1AS-2021 Port Interface
// ============================================================================

/**
 * @brief gPTP Port representation per 802.1AS-2021
 */
class GPTPPort {
public:
    enum class PortState {
        INITIALIZING,
        FAULTY,
        DISABLED,
        LISTENING,
        PRE_MASTER,
        MASTER,
        PASSIVE,
        UNCALIBRATED,
        SLAVE
    };
    
    GPTPPort(uint16_t port_number);
    virtual ~GPTPPort(); // Declared here, defined in cpp file
    
    // Port state management
    PortState get_port_state() const;
    bool set_port_state(PortState state);
    
    // Port identity
    PortIdentity get_port_identity() const;
    
    // Message transmission and reception
    virtual bool send_sync(const SyncMessage& message) = 0;
    virtual bool send_announce(const AnnounceMessage& message) = 0;
    virtual bool send_pdelay_req(const PDelayReqMessage& message) = 0;
    virtual bool send_pdelay_resp(const PDelayRespMessage& message) = 0;
    virtual bool send_follow_up(const FollowUpMessage& message) = 0;
    virtual bool send_pdelay_resp_follow_up(const PDelayRespFollowUpMessage& message) = 0;
    
    // Timestamping interface (802.1AS-2021 precision requirements)
    virtual bool get_tx_timestamp(Timestamp& timestamp, uint16_t sequence_id) = 0;
    virtual bool get_rx_timestamp(Timestamp& timestamp, uint16_t sequence_id) = 0;
    
    // Path delay measurement
    TimeInterval get_mean_path_delay() const;
    bool is_path_delay_valid() const;
    
private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

// ============================================================================
// IEEE 802.1AS-2021 Clock Interface
// ============================================================================

/**
 * @brief IEEE 1588 Clock implementation for 802.1AS-2021
 */
class IEEE1588Clock {
public:
    IEEE1588Clock(const ClockIdentity& identity);
    virtual ~IEEE1588Clock(); // Declared here, defined in cpp file
    
    // Clock identity and properties
    ClockIdentity get_clock_identity() const;
    ClockQuality get_clock_quality() const;
    bool set_clock_quality(const ClockQuality& quality);
    
    // Time management
    virtual bool get_time(Timestamp& time) const = 0;
    virtual bool set_time(const Timestamp& time) = 0;
    virtual bool adjust_frequency(int32_t ppb) = 0;  // parts per billion
    virtual bool adjust_phase(TimeInterval offset) = 0;
    // REVIEW [IEEE 802.1AS-2021, 10.2.12 Servo behavior]: Interfaces should define constraints and monotonicity/error handling for phase/frequency adjustments.
    
    // Master selection (BMCA - Best Master Clock Algorithm)
    bool is_better_than(const AnnounceMessage& announce) const;
    // REVIEW [IEEE 802.1AS-2021, 10.3 BMCA]: Provide explicit tie-break evaluation order (priority1, class, accuracy, variance, priority2, identity).
    // The current single helper name is ambiguous; document or expose comparison steps to avoid misinterpretation.
    uint8_t get_priority1() const;
    uint8_t get_priority2() const;
    void set_priority1(uint8_t priority);
    void set_priority2(uint8_t priority);
    
    // Synchronization state
    bool is_master() const;
    bool is_slave() const;
    TimeInterval get_offset_from_master() const;
    void set_offset_from_master(TimeInterval offset);
    
private:
    // Forward declaration only - definition in cpp file
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

// ============================================================================
// IEEE 802.1AS-2021 Utilities and Helper Functions
// ============================================================================

namespace Utils {
    /**
     * @brief Serialize PTP message to network byte order
     */
    std::vector<uint8_t> serialize_message(const PTPHeader& header, const void* message_body, size_t body_size);
    
    /**
     * @brief Deserialize PTP message from network byte order
     */
    bool deserialize_message(const std::vector<uint8_t>& data, PTPHeader& header, void* message_body, size_t body_size);
    
    /**
     * @brief Calculate correction field for path delay
     */
    int64_t calculate_correction_field(TimeInterval path_delay, TimeInterval residence_time = 0);
    
    /**
     * @brief Validate IEEE 802.1AS-2021 message format
     */
    bool validate_message_format(const std::vector<uint8_t>& message);
    
    /**
     * @brief Convert timestamp to nanoseconds since epoch
     */
    uint64_t timestamp_to_nanoseconds(const Timestamp& ts);
    
    /**
     * @brief Convert nanoseconds to timestamp structure
     */
    Timestamp nanoseconds_to_timestamp(uint64_t nanoseconds);
    
    /**
     * @brief Generate unique clock identity (802.1AS-2021 requirements)
     */
    ClockIdentity generate_clock_identity();
    
    /**
     * @brief Validate clock identity format
     */
    bool validate_clock_identity(const ClockIdentity& identity);
}

} // namespace _2021
} // namespace AS
} // namespace _802_1
} // namespace IEEE

#endif // IEEE_802_1AS_2021_H
