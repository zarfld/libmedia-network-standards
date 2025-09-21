/**
 * @file messages.hpp
 * @brief IEEE 1588-2019 PTP Message Format Structures
 * 
 * Implements PTP message formats according to Section 13 of IEEE 1588-2019
 * with time-sensitive design principles for deterministic execution.
 * 
 * Design Characteristics:
 * - All structures are POD (Plain Old Data) types for predictable memory layout
 * - Constexpr operations for compile-time computation where possible
 * - No dynamic allocation - all sizes known at compile time
 * - Deterministic serialization/deserialization with O(1) complexity
 * - Network byte order handling for cross-platform compatibility
 * - Hardware timestamp integration points identified
 * 
 * @note Based on IEEE 1588-2019 Section 13 "PTP message formats"
 * @copyright Standards Repository - Open Source Implementation
 */

#pragma once

#include "types.hpp"
#include <cstdint>
#include <array>

// Network byte order conversion functions
#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif

namespace IEEE {
namespace _1588 {
namespace PTP {
namespace _2019 {

// Import types from types.hpp
using namespace Types;

// Import all the types we need
using Types::UInteger8;
using Types::UInteger16;
using Types::UInteger32;
using Types::UInteger64;
using Types::Integer8;
using Types::ClockIdentity;
using Types::PortNumber;
using Types::PortIdentity;
using Types::CorrectionField;
using Types::Timestamp;
using Types::PTPError;
using Types::PTPResult;
using Types::MessageType;

// Message-specific types that extend base types
// Note: Base types (CorrectionField, PortIdentity, etc.) are imported from types.hpp

//==============================================================================
// PTP Flag Field Constants (Section 13.3.2.6)
//==============================================================================

namespace Flags {
    constexpr std::uint16_t ALTERNATE_MASTER       = 0x0100;
    constexpr std::uint16_t TWO_STEP               = 0x0200;
    constexpr std::uint16_t UNICAST                = 0x0400;
    constexpr std::uint16_t PROFILE_SPECIFIC_1     = 0x2000;
    constexpr std::uint16_t PROFILE_SPECIFIC_2     = 0x4000;
    constexpr std::uint16_t SECURITY               = 0x8000;
    
    // Leap second flags
    constexpr std::uint16_t LI_61                  = 0x0001;
    constexpr std::uint16_t LI_59                  = 0x0002;
    constexpr std::uint16_t CURRENT_UTC_OFFSET_VALID = 0x0004;
    constexpr std::uint16_t PTP_TIMESCALE          = 0x0008;
    constexpr std::uint16_t TIME_TRACEABLE         = 0x0010;
    constexpr std::uint16_t FREQUENCY_TRACEABLE    = 0x0020;
}

//==============================================================================
// Common PTP Message Header (Section 13.3)
//==============================================================================

/**
 * @brief Common PTP Message Header - present in all PTP messages
 * 
 * Fixed 34-byte header structure as specified in IEEE 1588-2019 Section 13.3.
 * Designed for deterministic parsing with O(1) field access.
 * 
 * @note Network byte order (big-endian) for all multi-byte fields
 * @note Hardware timestamping occurs at specific points during transmission/reception
 */
struct [[gnu::packed]] CommonHeader {
    // Byte 0: Transport specific (4 bits) + Message type (4 bits)
    std::uint8_t transport_messageType;
    
    // Byte 1: Reserved (4 bits) + Version (4 bits)  
    std::uint8_t reserved_version;
    
    // Bytes 2-3: Message length (network byte order)
    std::uint16_t messageLength;
    
    // Byte 4: Domain number
    std::uint8_t domainNumber;
    
    // Byte 5: Minor version PTP
    std::uint8_t minorVersionPTP;
    
    // Bytes 6-7: Flags (network byte order)
    std::uint16_t flagField;
    
    // Bytes 8-15: Correction field (network byte order)
    CorrectionField correctionField;
    
    // Bytes 16-19: Message type specific (reserved)
    std::uint32_t messageTypeSpecific;
    
    // Bytes 20-29: Source port identity
    PortIdentity sourcePortIdentity;
    
    // Bytes 30-31: Sequence ID (network byte order)
    std::uint16_t sequenceId;
    
    // Byte 32: Control field (deprecated in v2, set to 0xFF)
    std::uint8_t controlField;
    
    // Byte 33: Mean log message interval
    std::int8_t logMessageInterval;
    
    //--------------------------------------------------------------------------
    // Time-sensitive design accessors with compile-time optimization
    //--------------------------------------------------------------------------
    
    /**
     * @brief Extract message type with zero-cost abstraction
     * @return MessageType enum value
     * @note Constexpr for compile-time optimization
     */
    constexpr MessageType getMessageType() const noexcept {
        return static_cast<MessageType>(transport_messageType & 0x0F);
    }
    
    /**
     * @brief Set message type with deterministic execution
     * @param type MessageType to set
     * @note Preserves transport specific field (upper 4 bits)
     */
    constexpr void setMessageType(MessageType type) noexcept {
        transport_messageType = (transport_messageType & 0xF0) | 
                               (static_cast<std::uint8_t>(type) & 0x0F);
    }
    
    /**
     * @brief Extract PTP version with compile-time optimization
     * @return PTP version (should be 2 for IEEE 1588-2019)
     */
    constexpr std::uint8_t getVersion() const noexcept {
        return reserved_version & 0x0F;
    }
    
    /**
     * @brief Set PTP version (typically 2 for IEEE 1588-2019)
     * @param version PTP version to set
     */
    constexpr void setVersion(std::uint8_t version) noexcept {
        reserved_version = (reserved_version & 0xF0) | (version & 0x0F);
    }
    
    /**
     * @brief Check if two-step flag is set
     * @return true if two-step mode, false if one-step mode
     */
    constexpr bool isTwoStep() const noexcept {
        return (ntohs(flagField) & Flags::TWO_STEP) != 0;
    }
    
    /**
     * @brief Check if this is an event message requiring timestamping
     * @return true if event message, false if general message
     * @note Event messages: Sync, Delay_Req, Pdelay_Req, Pdelay_Resp
     */
    constexpr bool isEventMessage() const noexcept {
        const auto type = getMessageType();
        return (type == MessageType::Sync) ||
               (type == MessageType::Delay_Req) ||
               (type == MessageType::Pdelay_Req) ||
               (type == MessageType::Pdelay_Resp);
    }
    
    /**
     * @brief Validate header consistency with deterministic checks
     * @return PTPResult with validation status
     * @note All validation checks have bounded execution time
     */
    PTPResult<void> validate() const noexcept {
        // Version check
        if (getVersion() != 2) {
            return PTPResult<void>::makeError(PTPError::INVALID_VERSION);
        }
        
        // Message length bounds check
        const auto msgLen = ntohs(messageLength);
        if (msgLen < sizeof(CommonHeader) || msgLen > 1500) {
            return PTPResult<void>::makeError(PTPError::INVALID_LENGTH);
        }
        
        // Reserved fields should be zero
        if ((reserved_version & 0xF0) != 0) {
            return PTPResult<void>::makeError(PTPError::INVALID_RESERVED_FIELD);
        }
        
        return PTPResult<void>::success();
    }
};

static_assert(sizeof(CommonHeader) == 34, "CommonHeader must be exactly 34 bytes");
static_assert(std::is_trivially_copyable_v<CommonHeader>, "CommonHeader must be POD type");

//==============================================================================
// Announce Message Body (Section 13.5)
//==============================================================================

/**
 * @brief Announce message body for Best Master Clock Algorithm
 * 
 * Contains clock quality and identity information for master selection.
 * Follows IEEE 1588-2019 Section 13.5 format specification.
 */
struct [[gnu::packed]] AnnounceBody {
    // Bytes 34-43: Origin timestamp (when announce was sent)
    Timestamp originTimestamp;
    
    // Bytes 44-45: Current UTC offset (network byte order)
    std::int16_t currentUtcOffset;
    
    // Byte 46: Reserved
    std::uint8_t reserved;
    
    // Byte 47: Grandmaster priority 1
    std::uint8_t grandmasterPriority1;
    
    // Bytes 48-51: Grandmaster clock quality
    std::uint8_t grandmasterClockClass;
    std::uint8_t grandmasterClockAccuracy;
    std::uint16_t grandmasterClockVariance; // network byte order
    
    // Byte 52: Grandmaster priority 2  
    std::uint8_t grandmasterPriority2;
    
    // Bytes 53-60: Grandmaster identity
    ClockIdentity grandmasterIdentity;
    
    // Bytes 61-62: Steps removed (network byte order)
    std::uint16_t stepsRemoved;
    
    // Byte 63: Time source
    std::uint8_t timeSource;
    
    /**
     * @brief Validate announce message fields
     * @return PTPResult with validation status
     */
    PTPResult<void> validate() const noexcept {
        // Clock class range check (IEEE 1588-2019 Table 5)
        if (grandmasterClockClass > 255) {
            return PTPResult<void>::makeError(PTPError::INVALID_CLOCK_CLASS);
        }
        
        // Steps removed sanity check
        const auto steps = ntohs(stepsRemoved);
        if (steps > 255) {
            return PTPResult<void>::makeError(PTPError::INVALID_STEPS_REMOVED);
        }
        
        return PTPResult<void>::success();
    }
};

// Note: AnnounceBody size varies based on Timestamp implementation - will be fixed in future update

//==============================================================================
// Sync Message Body (Section 13.6)
//==============================================================================

/**
 * @brief Sync message body - minimal structure for time distribution
 * 
 * Used in one-step mode or followed by Follow_Up in two-step mode.
 */
struct [[gnu::packed]] SyncBody {
    // Bytes 34-43: Origin timestamp 
    Timestamp originTimestamp;
    
    /**
     * @brief Validate sync message timestamp
     * @return PTPResult with validation status
     */
    PTPResult<void> validate() const noexcept {
        return originTimestamp.validate();
    }
};

static_assert(sizeof(SyncBody) == 10, "SyncBody must be exactly 10 bytes");

//==============================================================================
// Follow_Up Message Body (Section 13.7)
//==============================================================================

/**
 * @brief Follow_Up message body for two-step time distribution
 * 
 * Contains precise timestamp of previously sent Sync message.
 */
struct [[gnu::packed]] FollowUpBody {
    // Bytes 34-43: Precise origin timestamp of associated Sync
    Timestamp preciseOriginTimestamp;
    
    /**
     * @brief Validate follow-up message timestamp
     * @return PTPResult with validation status
     */
    PTPResult<void> validate() const noexcept {
        return preciseOriginTimestamp.validate();
    }
};

static_assert(sizeof(FollowUpBody) == 10, "FollowUpBody must be exactly 10 bytes");

//==============================================================================
// Delay_Req Message Body (Section 13.6)
//==============================================================================

/**
 * @brief Delay_Req message body - end-to-end delay measurement
 * 
 * Minimal message for delay request-response mechanism.
 */
struct [[gnu::packed]] DelayReqBody {
    // Bytes 34-43: Origin timestamp (set to zero, filled by Follow_Up)
    Timestamp originTimestamp;
    
    /**
     * @brief Validate delay request message
     * @return PTPResult with validation status
     */
    PTPResult<void> validate() const noexcept {
        // Origin timestamp typically zero for delay request
        return PTPResult<void>::success();
    }
};

static_assert(sizeof(DelayReqBody) == 10, "DelayReqBody must be exactly 10 bytes");

//==============================================================================
// Delay_Resp Message Body (Section 13.8)
//==============================================================================

/**
 * @brief Delay_Resp message body - end-to-end delay measurement response
 * 
 * Contains receive timestamp of corresponding Delay_Req message.
 */
struct [[gnu::packed]] DelayRespBody {
    // Bytes 34-43: Receive timestamp of Delay_Req
    Timestamp receiveTimestamp;
    
    // Bytes 44-53: Requesting port identity
    PortIdentity requestingPortIdentity;
    
    /**
     * @brief Validate delay response message
     * @return PTPResult with validation status
     */
    PTPResult<void> validate() const noexcept {
        auto result = receiveTimestamp.validate();
        if (!result.isSuccess()) {
            return result;
        }
        
        return requestingPortIdentity.validate();
    }
};

static_assert(sizeof(DelayRespBody) == 20, "DelayRespBody must be exactly 20 bytes");

//==============================================================================
// Pdelay_Req Message Body (Section 13.9)
//==============================================================================

/**
 * @brief Pdelay_Req message body - peer-to-peer delay measurement
 * 
 * Used for direct link delay measurement between peers.
 */
struct [[gnu::packed]] PdelayReqBody {
    // Bytes 34-43: Origin timestamp (typically zero, filled by hardware)
    Timestamp originTimestamp;
    
    // Bytes 44-53: Reserved (10 bytes)
    std::array<std::uint8_t, 10> reserved;
    
    /**
     * @brief Validate peer delay request message
     * @return PTPResult with validation status
     */
    PTPResult<void> validate() const noexcept {
        // Reserved field should be zero
        for (const auto byte : reserved) {
            if (byte != 0) {
                return PTPResult<void>::makeError(PTPError::INVALID_RESERVED_FIELD);
            }
        }
        
        return PTPResult<void>::success();
    }
};

static_assert(sizeof(PdelayReqBody) == 20, "PdelayReqBody must be exactly 20 bytes");

//==============================================================================
// Pdelay_Resp Message Body (Section 13.10)
//==============================================================================

/**
 * @brief Pdelay_Resp message body - peer-to-peer delay measurement response
 * 
 * Contains receive timestamp of corresponding Pdelay_Req message.
 */
struct [[gnu::packed]] PdelayRespBody {
    // Bytes 34-43: Request receive timestamp
    Timestamp requestReceiveTimestamp;
    
    // Bytes 44-53: Requesting port identity
    PortIdentity requestingPortIdentity;
    
    /**
     * @brief Validate peer delay response message
     * @return PTPResult with validation status
     */
    PTPResult<void> validate() const noexcept {
        auto result = requestReceiveTimestamp.validate();
        if (!result.isSuccess()) {
            return result;
        }
        
        return requestingPortIdentity.validate();
    }
};

static_assert(sizeof(PdelayRespBody) == 20, "PdelayRespBody must be exactly 20 bytes");

//==============================================================================
// Pdelay_Resp_Follow_Up Message Body (Section 13.11)
//==============================================================================

/**
 * @brief Pdelay_Resp_Follow_Up message body - precise peer delay response
 * 
 * Contains precise transmit timestamp of corresponding Pdelay_Resp message.
 */
struct [[gnu::packed]] PdelayRespFollowUpBody {
    // Bytes 34-43: Response origin timestamp
    Timestamp responseOriginTimestamp;
    
    // Bytes 44-53: Requesting port identity
    PortIdentity requestingPortIdentity;
    
    /**
     * @brief Validate peer delay response follow-up message
     * @return PTPResult with validation status  
     */
    PTPResult<void> validate() const noexcept {
        auto result = responseOriginTimestamp.validate();
        if (!result.isSuccess()) {
            return result;
        }
        
        return requestingPortIdentity.validate();
    }
};

static_assert(sizeof(PdelayRespFollowUpBody) == 20, "PdelayRespFollowUpBody must be exactly 20 bytes");

//==============================================================================
// Complete PTP Message Templates
//==============================================================================

/**
 * @brief Complete PTP Message template combining header and body
 * 
 * Template-based approach for type-safe message handling with
 * compile-time size validation and deterministic memory layout.
 * 
 * @tparam BodyType Specific message body type
 */
template<typename BodyType>
struct [[gnu::packed]] PTPMessage {
    CommonHeader header;
    BodyType body;
    
    /**
     * @brief Validate complete message structure
     * @return PTPResult with validation status
     * @note Validates both header and body with bounded execution time
     */
    PTPResult<void> validate() const noexcept {
        auto headerResult = header.validate();
        if (!headerResult.isSuccess()) {
            return headerResult;
        }
        
        return body.validate();
    }
    
    /**
     * @brief Get total message size
     * @return Message size in bytes
     * @note Constexpr for compile-time computation
     */
    static constexpr std::size_t getMessageSize() noexcept {
        return sizeof(CommonHeader) + sizeof(BodyType);
    }
    
    /**
     * @brief Initialize message with proper defaults
     * @param msgType Message type to set
     * @param domain PTP domain number
     * @param sourcePort Source port identity
     */
    void initialize(MessageType msgType, std::uint8_t domain, 
                   const PortIdentity& sourcePort) noexcept {
        // Initialize header with IEEE 1588-2019 defaults
        header = {};
        header.setMessageType(msgType);
        header.setVersion(2);  // IEEE 1588-2019 is version 2
        header.messageLength = htons(static_cast<std::uint16_t>(getMessageSize()));
        header.domainNumber = domain;
        header.minorVersionPTP = 1;  // IEEE 1588-2019 minor version
        header.sourcePortIdentity = sourcePort;
        header.controlField = 0xFF;  // Deprecated in v2
        
        // Initialize body to zero
        body = {};
    }
};

//==============================================================================
// Specific Message Type Aliases for Type Safety
//==============================================================================

using AnnounceMessage = PTPMessage<AnnounceBody>;
using SyncMessage = PTPMessage<SyncBody>;
using FollowUpMessage = PTPMessage<FollowUpBody>;
using DelayReqMessage = PTPMessage<DelayReqBody>;
using DelayRespMessage = PTPMessage<DelayRespBody>;
using PdelayReqMessage = PTPMessage<PdelayReqBody>;
using PdelayRespMessage = PTPMessage<PdelayRespBody>;
using PdelayRespFollowUpMessage = PTPMessage<PdelayRespFollowUpBody>;

// Compile-time size verification for all message types
static_assert(AnnounceMessage::getMessageSize() == 64);
static_assert(SyncMessage::getMessageSize() == 44);
static_assert(FollowUpMessage::getMessageSize() == 44);
static_assert(DelayReqMessage::getMessageSize() == 44);
static_assert(DelayRespMessage::getMessageSize() == 54);
static_assert(PdelayReqMessage::getMessageSize() == 54);
static_assert(PdelayRespMessage::getMessageSize() == 54);
static_assert(PdelayRespFollowUpMessage::getMessageSize() == 54);

//==============================================================================
// Validation method implementations
//==============================================================================

inline PTPResult<void> Timestamp::validate() const noexcept {
    if (!isValid()) {
        return PTPResult<void>::makeError(PTPError::INVALID_LENGTH);
    }
    return PTPResult<void>::success();
}

inline PTPResult<void> PortIdentity::validate() const noexcept {
    if (!isValid()) {
        return PTPResult<void>::makeError(PTPError::INVALID_LENGTH);
    }
    return PTPResult<void>::success();
}

} // namespace _2019
} // namespace PTP
} // namespace _1588
} // namespace IEEE