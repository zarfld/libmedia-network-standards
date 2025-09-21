/**
 * @file types.hpp
 * @brief IEEE 1588-2019 PTP v2.1 fundamental data types and constants
 * 
 * This file defines all fundamental data types used throughout the IEEE 1588-2019
 * implementation, following the standard specifications exactly for maximum
 * interoperability and compliance.
 * 
 * DETERMINISTIC DESIGN:
 * - All types are POD (Plain Old Data) with deterministic memory layout
 * - No dynamic memory allocation
 * - All operations are bounded execution time
 * - No blocking calls or exceptions in critical paths
 * - Designed for time-sensitive applications
 * 
 * @copyright
 * This implementation follows IEEE 1588-2019 standard specifications.
 * All standard-defined types and constants are implemented according to
 * IEEE specifications.
 * 
 * @version 1.0.0
 * @date 2025-09-20
 */

#pragma once

#include <cstdint>
#include <array>

namespace IEEE {
namespace _1588 {
namespace PTP {
namespace _2019 {
namespace Types {

// Basic integer types as defined in IEEE 1588-2019
using UInteger4 = std::uint8_t;   ///< 4-bit unsigned integer (stored in uint8_t)
using UInteger8 = std::uint8_t;   ///< 8-bit unsigned integer
using UInteger16 = std::uint16_t; ///< 16-bit unsigned integer
using UInteger32 = std::uint32_t; ///< 32-bit unsigned integer
using UInteger48 = std::uint64_t; ///< 48-bit unsigned integer (stored in uint64_t)
using UInteger64 = std::uint64_t; ///< 64-bit unsigned integer

using Integer8 = std::int8_t;     ///< 8-bit signed integer
using Integer16 = std::int16_t;   ///< 16-bit signed integer
using Integer32 = std::int32_t;   ///< 32-bit signed integer
using Integer64 = std::int64_t;   ///< 64-bit signed integer

// Forward declarations
enum class PTPError : UInteger8;
template<typename T> class PTPResult;
struct TimeInterval;

// IEEE 1588-2019 specific types

/**
 * @brief Clock Identity - 8-byte unique identifier for each PTP clock
 * 
 * The ClockIdentity is derived from the IEEE EUI-64 identifier or
 * constructed according to IEEE 1588-2019 specifications.
 */
using ClockIdentity = std::array<std::uint8_t, 8>;

/**
 * @brief Port Number - identifies a specific port on a PTP clock
 */
using PortNumber = UInteger16;

/**
 * @brief Domain Number - identifies a PTP domain (0-127 for default domain)
 */
using DomainNumber = UInteger8;

/**
 * @brief Sequence ID - message sequence identifier
 */
using SequenceId = UInteger16;

/**
 * @brief Correction Field - time correction in units of nanoseconds * 2^16
 * 
 * Deterministic design: All operations are constexpr with bounded execution time.
 * No dynamic allocation or blocking calls.
 */
struct CorrectionField {
    UInteger64 value; ///< Correction value in scaled nanoseconds
    
    /**
     * @brief Default constructor
     */
    constexpr CorrectionField() noexcept : value(0) {}
    
    /**
     * @brief Value constructor
     * @param val Initial value
     */
    constexpr explicit CorrectionField(UInteger64 val) noexcept : value(val) {}
    
    /**
     * @brief Convert to nanoseconds
     * @return Correction value in nanoseconds
     * @note Deterministic: O(1) execution time, no blocking
     */
    constexpr double toNanoseconds() const noexcept {
        return static_cast<double>(value) / 65536.0;
    }
    
    /**
     * @brief Create from nanoseconds
     * @param ns Nanoseconds value
     * @return CorrectionField instance
     * @note Deterministic: O(1) execution time, no blocking
     */
    static constexpr CorrectionField fromNanoseconds(double ns) noexcept {
        return CorrectionField(static_cast<UInteger64>(ns * 65536.0));
    }
    
    /**
     * @brief Constructor from TimeInterval (for static_cast support)
     * @param interval TimeInterval value
     * @note Deterministic: O(1) execution time, defined after TimeInterval
     */
    explicit CorrectionField(const TimeInterval& interval) noexcept;
    
    /**
     * @brief Addition assignment operator
     * @param other CorrectionField to add
     * @return Reference to this CorrectionField
     * @note Deterministic: O(1) execution time, for clocks.cpp compatibility
     */
    constexpr CorrectionField& operator+=(const CorrectionField& other) noexcept {
        value += other.value;
        return *this;
    }
    
    /**
     * @brief Left shift operator for scaling
     * @param shift Number of bits to shift
     * @return New CorrectionField with shifted value
     * @note Deterministic: O(1) execution time, for clocks.cpp compatibility
     */
    constexpr CorrectionField operator<<(unsigned int shift) const noexcept {
        return CorrectionField(value << shift);
    }
};

/**
 * @brief PTP Timestamp - seconds and nanoseconds representation
 * 
 * Deterministic design: POD structure with deterministic operations.
 * Hardware abstraction layer will provide time conversions.
 */
struct Timestamp {
    UInteger48 seconds_high;    ///< Upper 16 bits of seconds (for future use)
    UInteger32 seconds_low;     ///< Lower 32 bits of seconds
    UInteger32 nanoseconds;     ///< Nanoseconds (0-999,999,999)
    
    /**
     * @brief Get total seconds (48-bit)
     * @return Total seconds value
     * @note Deterministic: O(1) execution time
     */
    constexpr UInteger64 getTotalSeconds() const noexcept {
        return (static_cast<UInteger64>(seconds_high) << 32) | seconds_low;
    }
    
    /**
     * @brief Set total seconds (48-bit)
     * @param total_seconds Total seconds value
     * @note Deterministic: O(1) execution time
     */
    constexpr void setTotalSeconds(UInteger64 total_seconds) noexcept {
        seconds_high = static_cast<UInteger48>(total_seconds >> 32);
        seconds_low = static_cast<UInteger32>(total_seconds & 0xFFFFFFFF);
    }
    
    /**
     * @brief Validate nanoseconds field
     * @return true if nanoseconds is valid (< 1,000,000,000)
     * @note Deterministic: O(1) execution time
     */
    constexpr bool isValid() const noexcept {
        return nanoseconds < 1000000000U;
    }
    
    /**
     * @brief Validate timestamp fields
     * @return PTPResult indicating success or error
     * @note Required for message validation compatibility
     */
    PTPResult<void> validate() const noexcept;
    
    /**
     * @brief Comparison operators for Timestamp
     * @note Deterministic: O(1) execution time, required for timeout calculations
     */
    constexpr bool operator<(const Timestamp& other) const noexcept {
        UInteger64 this_total = getTotalSeconds();
        UInteger64 other_total = other.getTotalSeconds();
        if (this_total != other_total) {
            return this_total < other_total;
        }
        return nanoseconds < other.nanoseconds;
    }
    
    constexpr bool operator>(const Timestamp& other) const noexcept {
        return other < *this;
    }
    
    constexpr bool operator<=(const Timestamp& other) const noexcept {
        return !(other < *this);
    }
    
    constexpr bool operator>=(const Timestamp& other) const noexcept {
        return !(*this < other);
    }
    
    constexpr bool operator==(const Timestamp& other) const noexcept {
        return getTotalSeconds() == other.getTotalSeconds() && 
               nanoseconds == other.nanoseconds;
    }
    
    constexpr bool operator!=(const Timestamp& other) const noexcept {
        return !(*this == other);
    }
    
    /**
     * @brief Arithmetic operators for Timestamp
     * @note Deterministic: O(1) execution time, required for time interval calculations
     */
    TimeInterval operator-(const Timestamp& other) const noexcept;
    
    /**
     * @brief Multiply timestamp by scalar (for timeout calculations)
     * @note Deterministic: O(1) execution time
     */
    Timestamp operator*(std::uint8_t multiplier) const noexcept;
};

/**
 * @brief Port Identity - combination of clock identity and port number
 * 
 * Deterministic design: POD structure with O(1) comparison operations.
 */
struct PortIdentity {
    ClockIdentity clock_identity; ///< Clock identifier
    PortNumber port_number;       ///< Port number
    
    /**
     * @brief Equality comparison
     * @note Deterministic: O(1) execution time
     */
    bool operator==(const PortIdentity& other) const noexcept {
        return clock_identity == other.clock_identity && 
               port_number == other.port_number;
    }
    
    /**
     * @brief Less-than comparison for ordering
     * @note Deterministic: O(1) execution time
     */
    bool operator<(const PortIdentity& other) const noexcept {
        if (clock_identity != other.clock_identity) {
            return clock_identity < other.clock_identity;
        }
        return port_number < other.port_number;
    }
    
    /**
     * @brief Validate port identity fields
     * @return PTPResult indicating success or error
     * @note Required for message validation compatibility
     */
    PTPResult<void> validate() const noexcept;
};

/**
 * @brief Clock Quality - describes the quality of a clock
 */
struct ClockQuality {
    UInteger8 clock_class;        ///< Clock class according to IEEE 1588-2019
    UInteger8 clock_accuracy;     ///< Clock accuracy enumeration
    UInteger16 offset_scaled_log_variance; ///< Allan variance measure
};

/**
 * @brief Time Interval - scaled nanoseconds for time intervals
 * 
 * Deterministic design: All operations are constexpr with bounded execution time.
 */
struct TimeInterval {
    Integer64 scaled_nanoseconds; ///< Time interval in units of 2^-16 nanoseconds
    
    /**
     * @brief Convert to nanoseconds
     * @note Deterministic: O(1) execution time, no blocking
     */
    constexpr double toNanoseconds() const noexcept {
        return static_cast<double>(scaled_nanoseconds) / 65536.0;
    }
    
    /**
     * @brief Create from nanoseconds
     * @note Deterministic: O(1) execution time, no blocking
     */
    static constexpr TimeInterval fromNanoseconds(double ns) noexcept {
        return {static_cast<Integer64>(ns * 65536.0)};
    }
};

//==============================================================================
// Timestamp operator implementations (defined after TimeInterval)
//==============================================================================

/**
 * @brief CorrectionField constructor from TimeInterval
 * @note Defined here after TimeInterval is complete
 */
inline CorrectionField::CorrectionField(const TimeInterval& interval) noexcept 
    : value(static_cast<UInteger64>(interval.scaled_nanoseconds)) {}

/**
 * @brief Timestamp arithmetic operator implementations
 * @note Defined here after TimeInterval is complete
 */
inline TimeInterval Timestamp::operator-(const Timestamp& other) const noexcept {
    // Calculate difference in nanoseconds, then convert to TimeInterval
    UInteger64 this_ns = getTotalSeconds() * 1000000000ULL + nanoseconds;
    UInteger64 other_ns = other.getTotalSeconds() * 1000000000ULL + other.nanoseconds;
    Integer64 diff_ns = static_cast<Integer64>(this_ns) - static_cast<Integer64>(other_ns);
    return TimeInterval{diff_ns * 65536}; // Convert to scaled nanoseconds (2^16 factor)
}

inline Timestamp Timestamp::operator*(std::uint8_t multiplier) const noexcept {
    UInteger64 total_ns = getTotalSeconds() * 1000000000ULL + nanoseconds;
    total_ns *= multiplier;
    
    Timestamp result;
    result.setTotalSeconds(total_ns / 1000000000ULL);
    result.nanoseconds = static_cast<UInteger32>(total_ns % 1000000000ULL);
    return result;
}

/**
 * @brief Log Message Interval - logarithmic representation of message intervals
 */
enum class LogMessageInterval : Integer8 {
    // Standard intervals (2^x seconds)
    Interval_1_128_Second = -7,  ///< 1/128 second (7.8125 ms)
    Interval_1_64_Second = -6,   ///< 1/64 second (15.625 ms)
    Interval_1_32_Second = -5,   ///< 1/32 second (31.25 ms)
    Interval_1_16_Second = -4,   ///< 1/16 second (62.5 ms)
    Interval_1_8_Second = -3,    ///< 1/8 second (125 ms)
    Interval_1_4_Second = -2,    ///< 1/4 second (250 ms)
    Interval_1_2_Second = -1,    ///< 1/2 second (500 ms)
    Interval_1_Second = 0,       ///< 1 second
    Interval_2_Seconds = 1,      ///< 2 seconds
    Interval_4_Seconds = 2,      ///< 4 seconds
    Interval_8_Seconds = 3,      ///< 8 seconds
    Interval_16_Seconds = 4,     ///< 16 seconds
    Interval_32_Seconds = 5,     ///< 32 seconds
    Interval_64_Seconds = 6      ///< 64 seconds
};

/**
 * @brief PTP Clock Types
 */
enum class ClockType : UInteger8 {
    Ordinary = 0x00,        ///< Ordinary Clock (OC)
    Boundary = 0x01,        ///< Boundary Clock (BC)
    P2P_Transparent = 0x02, ///< Peer-to-Peer Transparent Clock (P2P TC)
    E2E_Transparent = 0x03  ///< End-to-End Transparent Clock (E2E TC)
};

/**
 * @brief PTP Port States according to state machine
 */
enum class PortState : UInteger8 {
    Initializing = 0x01,    ///< Initializing state
    Faulty = 0x02,          ///< Faulty state
    Disabled = 0x03,        ///< Disabled state
    Listening = 0x04,       ///< Listening state
    PreMaster = 0x05,       ///< Pre-Master state
    Master = 0x06,          ///< Master state
    Passive = 0x07,         ///< Passive state
    Uncalibrated = 0x08,    ///< Uncalibrated state
    Slave = 0x09            ///< Slave state
};

/**
 * @brief PTP Message Types
 */
enum class MessageType : UInteger8 {
    // Event messages (requiring precise timestamps)
    Sync = 0x0,             ///< Sync message
    Delay_Req = 0x1,        ///< Delay Request message
    Pdelay_Req = 0x2,       ///< Peer Delay Request message
    Pdelay_Resp = 0x3,      ///< Peer Delay Response message
    
    // General messages (not requiring precise timestamps)
    Follow_Up = 0x8,        ///< Follow Up message
    Delay_Resp = 0x9,       ///< Delay Response message
    Pdelay_Resp_Follow_Up = 0xA, ///< Peer Delay Response Follow Up message
    Announce = 0xB,         ///< Announce message
    Signaling = 0xC,        ///< Signaling message
    Management = 0xD        ///< Management message
};

/**
 * @brief Network Transport Types
 */
enum class NetworkTransport : UInteger8 {
    UDP_IPv4 = 0x01,        ///< UDP over IPv4
    UDP_IPv6 = 0x02,        ///< UDP over IPv6
    IEEE_802_3 = 0x03,      ///< IEEE 802.3 Ethernet
    DeviceNet = 0x04,       ///< DeviceNet
    ControlNet = 0x05,      ///< ControlNet
    PROFINET = 0x06         ///< PROFINET
};

/**
 * @brief Time Source enumeration
 */
enum class TimeSource : UInteger8 {
    Atomic_Clock = 0x10,    ///< Atomic clock
    GPS = 0x20,             ///< Global Positioning System
    Terrestrial_Radio = 0x30, ///< Terrestrial radio
    PTP = 0x40,             ///< Another PTP clock
    NTP = 0x50,             ///< Network Time Protocol
    Hand_Set = 0x60,        ///< Manually set
    Other = 0x90,           ///< Other source
    Internal_Oscillator = 0xA0 ///< Internal oscillator
};

// Constants defined by IEEE 1588-2019

constexpr size_t CLOCK_IDENTITY_LENGTH = 8;
constexpr size_t CLOCK_IDENTITY_OFFSET = 0;
constexpr size_t PORT_NUMBER_OFFSET = 8;
constexpr size_t PORT_IDENTITY_LENGTH = 10;

constexpr UInteger16 PTP_SYNC_MESSAGE_LENGTH = 44;
constexpr UInteger16 PTP_ANNOUNCE_MESSAGE_LENGTH = 64;
constexpr UInteger16 PTP_DELAY_REQ_MESSAGE_LENGTH = 44;
constexpr UInteger16 PTP_DELAY_RESP_MESSAGE_LENGTH = 54;

// Default values
constexpr DomainNumber DEFAULT_DOMAIN = 0;
constexpr Integer8 DEFAULT_LOG_ANNOUNCE_INTERVAL = 1; // 2 seconds
constexpr Integer8 DEFAULT_LOG_SYNC_INTERVAL = 0;     // 1 second
constexpr Integer8 DEFAULT_LOG_MIN_DELAY_REQ_INTERVAL = 0; // 1 second

/**
 * @brief Deterministic error codes for PTP operations
 * 
 * Error handling without exceptions - suitable for time-sensitive applications.
 * All error conditions are represented as enumeration values with O(1) checking.
 */
enum class PTPError : UInteger8 {
    Success = 0x00,             ///< Operation completed successfully
    Invalid_Parameter = 0x01,   ///< Invalid input parameter
    Buffer_Overflow = 0x02,     ///< Buffer size exceeded
    Invalid_Timestamp = 0x03,   ///< Timestamp validation failed
    Clock_Identity_Error = 0x04, ///< Clock identity is invalid
    Port_Number_Error = 0x05,   ///< Port number out of range
    Domain_Error = 0x06,        ///< Domain number invalid
    Message_Parse_Error = 0x07, ///< Message parsing failed
    Hardware_Error = 0x08,      ///< Hardware abstraction layer error
    Timeout = 0x09,             ///< Operation timed out
    Resource_Unavailable = 0x0A, ///< Required resource not available
    State_Error = 0x0B,         ///< Invalid state for operation
    Memory_Error = 0x0C,        ///< Memory allocation/access error
    Network_Error = 0x0D,       ///< Network operation failed
    
    // Additional message validation errors (for message format validation)
    INVALID_VERSION = 0x10,      ///< Invalid PTP version in message
    INVALID_LENGTH = 0x11,       ///< Invalid message length
    INVALID_RESERVED_FIELD = 0x12, ///< Non-zero reserved field
    INVALID_CLOCK_CLASS = 0x13,  ///< Invalid clock class value
    INVALID_STEPS_REMOVED = 0x14, ///< Invalid steps removed value
    
    // Additional error codes for clocks.cpp compatibility
    UNSUPPORTED_MESSAGE = 0x15,  ///< Message type not supported
    INVALID_MESSAGE_SIZE = 0x16, ///< Message size validation failed
    INVALID_PORT = 0x17,         ///< Port number validation failed
    INVALID_TIMESTAMP = 0x18,    ///< Timestamp validation failed (alias for Invalid_Timestamp)
    INVALID_PARAMETER = 0x19,    ///< Parameter validation failed (alias for Invalid_Parameter)
    
    Unknown_Error = 0xFF        ///< Unknown or unspecified error
};

/**
 * @brief Deterministic result type for operations that may fail
 * 
 * Template class providing error handling without exceptions.
 * Suitable for time-sensitive systems with deterministic behavior.
 * 
 * @tparam T The type of value returned on success
 */
template<typename T>
class PTPResult {
private:
    union {
        T value_;           ///< Value when successful
        PTPError error_;    ///< Error code when failed
    };
    bool has_value_;       ///< True if contains value, false if contains error

public:
    /**
     * @brief Construct successful result
     * @param value The success value
     * @note Deterministic: O(1) construction, no dynamic allocation
     */
    constexpr explicit PTPResult(const T& value) noexcept 
        : value_(value), has_value_(true) {}
    
    /**
     * @brief Construct error result
     * @param error The error code
     * @note Deterministic: O(1) construction, no dynamic allocation
     */
    constexpr explicit PTPResult(PTPError error) noexcept 
        : error_(error), has_value_(false) {}
    
    /**
     * @brief Copy constructor
     * @note Deterministic: O(1) operation
     */
    constexpr PTPResult(const PTPResult& other) noexcept 
        : has_value_(other.has_value_) {
        if (has_value_) {
            value_ = other.value_;
        } else {
            error_ = other.error_;
        }
    }
    
    /**
     * @brief Check if result contains a value
     * @return true if successful, false if error
     * @note Deterministic: O(1) check
     */
    constexpr bool hasValue() const noexcept {
        return has_value_;
    }
    
    /**
     * @brief Check if result contains an error
     * @return true if error, false if successful
     * @note Deterministic: O(1) check
     */
    constexpr bool hasError() const noexcept {
        return !has_value_;
    }
    
    /**
     * @brief Get the value (only call if hasValue() returns true)
     * @return The success value
     * @note Deterministic: O(1) access, caller must check hasValue() first
     */
    constexpr const T& getValue() const noexcept {
        return value_;
    }
    
    /**
     * @brief Get the error code (only call if hasError() returns true)
     * @return The error code
     * @note Deterministic: O(1) access, caller must check hasError() first
     */
    constexpr PTPError getError() const noexcept {
        return error_;
    }
    
    /**
     * @brief Get value or return default if error
     * @param default_value Value to return on error
     * @return The value or default
     * @note Deterministic: O(1) operation
     */
    constexpr T getValueOr(const T& default_value) const noexcept {
        return has_value_ ? value_ : default_value;
    }
    
    /**
     * @brief Check if result is successful (alias for hasValue)
     * @return true if successful, false if error
     * @note Compatibility method for clock state machine API
     */
    constexpr bool is_success() const noexcept {
        return has_value_;
    }
    
    /**
     * @brief Get the value (alias for getValue)
     * @return The success value
     * @note Compatibility method for clock state machine API
     */
    constexpr const T& get_value() const noexcept {
        return value_;
    }
    
    /**
     * @brief Get the error (alias for getError)
     * @return The error code
     * @note Compatibility method for clock state machine API
     */
    constexpr PTPError get_error() const noexcept {
        return error_;
    }
    
    /**
     * @brief Create successful result (static method)
     * @param value The success value
     * @return Successful PTPResult
     * @note Compatibility method for clock state machine API
     */
    static constexpr PTPResult success(const T& value) noexcept {
        return PTPResult(value);
    }
    
    /**
     * @brief Create error result (static method)
     * @param error The error code
     * @return Error PTPResult
     * @note Compatibility method for clock state machine API
     */
    static constexpr PTPResult failure(PTPError error) noexcept {
        return PTPResult(error);
    }
    
    /**
     * @brief Create error result (alias for failure)
     * @param error The error code
     * @return Error PTPResult
     * @note Compatibility method for messages.hpp API
     */
    static constexpr PTPResult makeError(PTPError error) noexcept {
        return failure(error);
    }
};

/**
 * @brief Template specialization for void type
 * @details Special handling for operations that don't return values
 */
template<>
class PTPResult<void> {
private:
    PTPError error_;    ///< Error code when failed
    bool has_value_;    ///< True if successful, false if error

public:
    /**
     * @brief Construct successful result
     * @note Deterministic: O(1) construction, no dynamic allocation
     */
    constexpr PTPResult() noexcept 
        : error_(PTPError::Success), has_value_(true) {}
    
    /**
     * @brief Construct error result
     * @param error The error code
     * @note Deterministic: O(1) construction, no dynamic allocation
     */
    constexpr explicit PTPResult(PTPError error) noexcept 
        : error_(error), has_value_(false) {}
    
    /**
     * @brief Check if result is successful
     * @return true if successful, false if error
     * @note Deterministic: O(1) check
     */
    constexpr bool hasValue() const noexcept {
        return has_value_;
    }
    
    /**
     * @brief Check if result contains an error
     * @return true if error, false if successful
     * @note Deterministic: O(1) check
     */
    constexpr bool hasError() const noexcept {
        return !has_value_;
    }
    
    /**
     * @brief Get the error code (only call if hasError() returns true)
     * @return The error code
     * @note Deterministic: O(1) access, caller must check hasError() first
     */
    constexpr PTPError getError() const noexcept {
        return error_;
    }
    
    /**
     * @brief Check if result is successful (alias for hasValue)
     * @return true if successful, false if error
     * @note Compatibility method for clock state machine API
     */
    constexpr bool is_success() const noexcept {
        return has_value_;
    }
    
    /**
     * @brief Get the error (alias for getError)
     * @return The error code
     * @note Compatibility method for clock state machine API
     */
    constexpr PTPError get_error() const noexcept {
        return error_;
    }
    
    /**
     * @brief Create successful result (static method)
     * @return Successful PTPResult
     * @note Compatibility method for clock state machine API
     */
    static constexpr PTPResult success() noexcept {
        return PTPResult();
    }
    
    /**
     * @brief Create error result (static method)
     * @param error The error code
     * @return Error PTPResult
     * @note Compatibility method for clock state machine API
     */
    static constexpr PTPResult failure(PTPError error) noexcept {
        return PTPResult(error);
    }
    
    /**
     * @brief Create error result (alias for failure)
     * @param error The error code
     * @return Error PTPResult
     * @note Compatibility method for messages.hpp API
     */
    static constexpr PTPResult makeError(PTPError error) noexcept {
        return failure(error);
    }
};

/**
 * @brief Helper function to create successful result
 * @tparam T Value type
 * @param value The success value
 * @return PTPResult containing the value
 * @note Deterministic: O(1) construction
 */
template<typename T>
constexpr PTPResult<T> makeSuccess(const T& value) noexcept {
    return PTPResult<T>(value);
}

/**
 * @brief Helper function to create error result
 * @tparam T Value type
 * @param error The error code
 * @return PTPResult containing the error
 * @note Deterministic: O(1) construction
 */
template<typename T>
constexpr PTPResult<T> makeError(PTPError error) noexcept {
    return PTPResult<T>(error);
}

//==============================================================================
// Method definitions for types that need PTPResult
//==============================================================================

/**
 * @brief Validate timestamp fields
 * @return PTPResult indicating success or error
 * @note Required for message validation compatibility
 */
inline PTPResult<void> Timestamp::validate() const noexcept {
    if (!isValid()) {
        return PTPResult<void>::makeError(PTPError::Invalid_Timestamp);
    }
    return PTPResult<void>::success();
}

/**
 * @brief Validate port identity fields
 * @return PTPResult indicating success or error
 * @note Required for message validation compatibility
 */
inline PTPResult<void> PortIdentity::validate() const noexcept {
    if (port_number == 0) {
        return PTPResult<void>::makeError(PTPError::Port_Number_Error);
    }
    return PTPResult<void>::success();
}

} // namespace Types
} // namespace _2019
} // namespace PTP
} // namespace _1588
} // namespace IEEE