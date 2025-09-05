/**
 * @file message_validation.h
 * @brief IEEE 802.1AS-2021 Message Validation Enhancement
 * @details Strict Table 10-7 PTP header format compliance with bit-field validation
 * 
 * This implementation provides comprehensive message validation according to:
 * - IEEE 802.1AS-2021 Table 10-7: Message format specification
 * - IEEE 802.1AS-2021 Clause 11.4.2: Message format requirements
 * - IEEE 1588-2019 Clause 13: PTP message formats
 * - IEEE 802.1AS-2021 Annex F: Implementation guidelines
 * 
 * Features:
 * - Strict bit-field layout validation
 * - Endianness handling (network byte order)
 * - Field range validation
 * - Message type specific validation
 * - TLV format validation
 * - Comprehensive error reporting
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Implementation follows IEEE 802.1AS-2021 Table 10-7
 */

#ifndef IEEE_802_1AS_2021_MESSAGE_VALIDATION_H
#define IEEE_802_1AS_2021_MESSAGE_VALIDATION_H

#include "../core/ieee_802_1as_2021.h"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>

namespace IEEE {
namespace _802_1 {
namespace AS {
namespace _2021 {

/**
 * @brief Message validation result codes
 */
enum class ValidationResult {
    VALID,                          // Message is fully compliant
    INVALID_HEADER_LENGTH,          // Header length mismatch
    INVALID_MESSAGE_TYPE,           // Unknown or invalid message type
    INVALID_VERSION,                // Unsupported PTP version
    INVALID_DOMAIN,                 // Invalid domain number
    INVALID_FLAGS,                  // Invalid flag combination
    INVALID_CORRECTION_FIELD,       // Correction field out of range
    INVALID_SOURCE_PORT_IDENTITY,   // Malformed source port identity
    INVALID_SEQUENCE_ID,            // Invalid sequence ID
    INVALID_CONTROL_FIELD,          // Invalid control field
    INVALID_LOG_MESSAGE_INTERVAL,   // Invalid log message interval
    INVALID_TIMESTAMP,              // Invalid timestamp format
    INVALID_TLV_FORMAT,             // Malformed TLV
    INVALID_MESSAGE_LENGTH,         // Message length inconsistency
    INVALID_CHECKSUM,               // Message checksum failure
    ENDIANNESS_ERROR,               // Incorrect byte order
    FIELD_ALIGNMENT_ERROR,          // Field boundary violation
    RESERVED_FIELD_VIOLATION        // Reserved field contains non-zero value
};

/**
 * @brief Validation error details
 */
struct ValidationError {
    ValidationResult error_code;     // Error type
    std::string error_message;       // Human-readable error description
    uint32_t byte_offset;           // Byte offset where error occurred
    uint32_t bit_offset;            // Bit offset within byte (if applicable)
    std::string field_name;         // Name of the problematic field
    std::vector<uint8_t> expected_value; // Expected value (if applicable)
    std::vector<uint8_t> actual_value;   // Actual value found
    std::string ieee_reference;     // IEEE 802.1AS-2021 clause reference
    
    ValidationError() : error_code(ValidationResult::VALID), byte_offset(0), bit_offset(0) {}
};

/**
 * @brief Complete validation report
 */
struct ValidationReport {
    bool is_valid;                   // Overall validation result
    std::vector<ValidationError> errors; // List of validation errors
    std::vector<ValidationError> warnings; // List of warnings (non-fatal)
    std::string message_type_name;   // Human-readable message type
    uint32_t message_length;         // Total message length
    uint8_t message_type;            // PTP message type
    std::string validation_summary;  // Summary of validation results
    std::chrono::steady_clock::time_point validation_time; // When validation was performed
    
    ValidationReport() : is_valid(false), message_length(0), message_type(0xFF) {}
};

/**
 * @brief Enhanced message validator class
 */
class MessageValidator {
public:
    /**
     * @brief Constructor
     */
    explicit MessageValidator();
    
    /**
     * @brief Destructor
     */
    ~MessageValidator();
    
    // Primary validation methods
    
    /**
     * @brief Validate complete PTP message
     * @param message_data Raw message bytes
     * @return Complete validation report
     */
    ValidationReport validate_message(const std::vector<uint8_t>& message_data);
    
    /**
     * @brief Validate PTP header only (first 34 bytes)
     * @param header_data PTP header bytes
     * @return Validation report for header
     */
    ValidationReport validate_header(const std::vector<uint8_t>& header_data);
    
    /**
     * @brief Validate specific message type
     * @param message_data Complete message bytes
     * @param expected_type Expected PTP message type
     * @return Validation report
     */
    ValidationReport validate_message_type(const std::vector<uint8_t>& message_data, uint8_t expected_type);
    
    // Message-specific validation methods (IEEE 802.1AS-2021 Table 10-7)
    
    /**
     * @brief Validate Announce message format
     */
    ValidationReport validate_announce_message(const std::vector<uint8_t>& message_data);
    
    /**
     * @brief Validate Sync message format
     */
    ValidationReport validate_sync_message(const std::vector<uint8_t>& message_data);
    
    /**
     * @brief Validate Follow_Up message format
     */
    ValidationReport validate_follow_up_message(const std::vector<uint8_t>& message_data);
    
    /**
     * @brief Validate Pdelay_Req message format
     */
    ValidationReport validate_pdelay_req_message(const std::vector<uint8_t>& message_data);
    
    /**
     * @brief Validate Pdelay_Resp message format
     */
    ValidationReport validate_pdelay_resp_message(const std::vector<uint8_t>& message_data);
    
    /**
     * @brief Validate Pdelay_Resp_Follow_Up message format
     */
    ValidationReport validate_pdelay_resp_follow_up_message(const std::vector<uint8_t>& message_data);
    
    /**
     * @brief Validate Signaling message format
     */
    ValidationReport validate_signaling_message(const std::vector<uint8_t>& message_data);
    
    /**
     * @brief Validate Management message format
     */
    ValidationReport validate_management_message(const std::vector<uint8_t>& message_data);
    
    // Field-specific validation methods
    
    /**
     * @brief Validate PTP header fields (Table 10-7 compliance)
     */
    std::vector<ValidationError> validate_header_fields(const std::vector<uint8_t>& header_data);
    
    /**
     * @brief Validate timestamp format
     */
    ValidationError validate_timestamp_field(const std::vector<uint8_t>& timestamp_data, uint32_t offset);
    
    /**
     * @brief Validate PortIdentity format
     */
    ValidationError validate_port_identity_field(const std::vector<uint8_t>& port_id_data, uint32_t offset);
    
    /**
     * @brief Validate TLV format
     */
    std::vector<ValidationError> validate_tlv_format(const std::vector<uint8_t>& tlv_data, uint32_t offset);
    
    /**
     * @brief Validate correction field format and range
     */
    ValidationError validate_correction_field(const std::vector<uint8_t>& message_data, uint32_t offset);
    
    /**
     * @brief Validate flags field for 802.1AS compliance
     */
    ValidationError validate_flags_field(const std::vector<uint8_t>& message_data, uint32_t offset, uint8_t message_type);
    
    // Endianness and bit-field validation
    
    /**
     * @brief Validate network byte order compliance
     */
    std::vector<ValidationError> validate_endianness(const std::vector<uint8_t>& message_data);
    
    /**
     * @brief Validate reserved field compliance (must be zero)
     */
    std::vector<ValidationError> validate_reserved_fields(const std::vector<uint8_t>& message_data, uint8_t message_type);
    
    /**
     * @brief Validate field boundary alignment
     */
    std::vector<ValidationError> validate_field_alignment(const std::vector<uint8_t>& message_data);
    
    // Configuration methods
    
    /**
     * @brief Enable/disable strict validation mode
     * @param strict If true, warnings become errors
     */
    void set_strict_mode(bool strict);
    
    /**
     * @brief Set IEEE 802.1AS domain filter
     * @param allowed_domains List of allowed domain numbers
     */
    void set_allowed_domains(const std::vector<uint8_t>& allowed_domains);
    
    /**
     * @brief Enable validation of specific message types only
     * @param message_types List of message types to validate
     */
    void set_message_type_filter(const std::vector<uint8_t>& message_types);
    
    /**
     * @brief Set custom validation rules
     * @param rules Custom validation rule callbacks
     */
    void set_custom_validation_rules(const std::map<std::string, std::function<ValidationError(const std::vector<uint8_t>&)>>& rules);
    
    // Utility methods
    
    /**
     * @brief Generate human-readable validation report
     */
    std::string generate_validation_summary(const ValidationReport& report);
    
    /**
     * @brief Check if message type is supported by IEEE 802.1AS-2021
     */
    bool is_supported_message_type(uint8_t message_type);
    
    /**
     * @brief Get expected message length for message type
     */
    uint32_t get_expected_message_length(uint8_t message_type);
    
    /**
     * @brief Extract field value with endianness handling
     */
    template<typename T>
    T extract_field(const std::vector<uint8_t>& data, uint32_t offset);
    
    /**
     * @brief Convert validation result to string
     */
    static std::string validation_result_to_string(ValidationResult result);

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Message format constants from IEEE 802.1AS-2021 Table 10-7
 */
namespace MessageFormat {
    // PTP Message Types (IEEE 802.1AS-2021 Table 10-7)
    constexpr uint8_t SYNC = 0x0;
    constexpr uint8_t DELAY_REQ = 0x1;
    constexpr uint8_t PDELAY_REQ = 0x2;
    constexpr uint8_t PDELAY_RESP = 0x3;
    constexpr uint8_t FOLLOW_UP = 0x8;
    constexpr uint8_t DELAY_RESP = 0x9;
    constexpr uint8_t PDELAY_RESP_FOLLOW_UP = 0xA;
    constexpr uint8_t ANNOUNCE = 0xB;
    constexpr uint8_t SIGNALING = 0xC;
    constexpr uint8_t MANAGEMENT = 0xD;
    
    // Message Lengths (bytes)
    constexpr uint32_t PTP_HEADER_LENGTH = 34;
    constexpr uint32_t SYNC_MESSAGE_LENGTH = 44;
    constexpr uint32_t FOLLOW_UP_MESSAGE_LENGTH = 44;
    constexpr uint32_t PDELAY_REQ_MESSAGE_LENGTH = 54;
    constexpr uint32_t PDELAY_RESP_MESSAGE_LENGTH = 54;
    constexpr uint32_t PDELAY_RESP_FOLLOW_UP_MESSAGE_LENGTH = 54;
    constexpr uint32_t ANNOUNCE_MESSAGE_LENGTH = 64;
    constexpr uint32_t SIGNALING_MESSAGE_LENGTH = 44; // Minimum, plus TLVs
    constexpr uint32_t MANAGEMENT_MESSAGE_LENGTH = 48; // Minimum, plus management TLV
    
    // PTP Version
    constexpr uint8_t PTP_VERSION = 2;
    
    // IEEE 802.1AS Specific Values
    constexpr uint8_t TRANSPORT_SPECIFIC_802_1AS = 0x1;
    constexpr uint8_t DEFAULT_DOMAIN_NUMBER = 0;
    
    // Field Offsets in PTP Header (Table 10-7)
    constexpr uint32_t OFFSET_TRANSPORT_SPECIFIC = 0;  // 4 bits
    constexpr uint32_t OFFSET_MESSAGE_TYPE = 0;        // 4 bits
    constexpr uint32_t OFFSET_RESERVED_1 = 1;          // 4 bits
    constexpr uint32_t OFFSET_VERSION_PTP = 1;         // 4 bits
    constexpr uint32_t OFFSET_MESSAGE_LENGTH = 2;      // 16 bits
    constexpr uint32_t OFFSET_DOMAIN_NUMBER = 4;       // 8 bits
    constexpr uint32_t OFFSET_RESERVED_2 = 5;          // 8 bits
    constexpr uint32_t OFFSET_FLAGS = 6;               // 16 bits
    constexpr uint32_t OFFSET_CORRECTION_FIELD = 8;    // 64 bits
    constexpr uint32_t OFFSET_RESERVED_3 = 16;         // 32 bits
    constexpr uint32_t OFFSET_SOURCE_PORT_IDENTITY = 20; // 80 bits (10 bytes)
    constexpr uint32_t OFFSET_SEQUENCE_ID = 30;        // 16 bits
    constexpr uint32_t OFFSET_CONTROL_FIELD = 32;      // 8 bits
    constexpr uint32_t OFFSET_LOG_MESSAGE_INTERVAL = 33; // 8 bits
    
    // Flag field bit positions (IEEE 802.1AS-2021)
    constexpr uint16_t FLAG_LEAP61 = 0x0001;
    constexpr uint16_t FLAG_LEAP59 = 0x0002;
    constexpr uint16_t FLAG_CURRENT_UTC_OFFSET_VALID = 0x0004;
    constexpr uint16_t FLAG_PTP_TIMESCALE = 0x0008;
    constexpr uint16_t FLAG_TIME_TRACEABLE = 0x0010;
    constexpr uint16_t FLAG_FREQUENCY_TRACEABLE = 0x0020;
    
    // Control field values per message type
    constexpr uint8_t CONTROL_SYNC = 0x00;
    constexpr uint8_t CONTROL_DELAY_REQ = 0x01;
    constexpr uint8_t CONTROL_FOLLOW_UP = 0x02;
    constexpr uint8_t CONTROL_DELAY_RESP = 0x03;
    constexpr uint8_t CONTROL_MANAGEMENT = 0x04;
    constexpr uint8_t CONTROL_OTHER = 0x05;
}

/**
 * @brief TLV validation utilities
 */
namespace TLVValidation {
    /**
     * @brief TLV Type values for IEEE 802.1AS-2021
     */
    constexpr uint16_t TLV_MANAGEMENT = 0x0001;
    constexpr uint16_t TLV_MANAGEMENT_ERROR_STATUS = 0x0002;
    constexpr uint16_t TLV_ORGANIZATION_EXTENSION = 0x0003;
    constexpr uint16_t TLV_PATH_TRACE = 0x0008;
    constexpr uint16_t TLV_ALTERNATE_TIME_OFFSET_INDICATOR = 0x0009;
    
    /**
     * @brief Validate TLV header format
     */
    ValidationError validate_tlv_header(const std::vector<uint8_t>& tlv_data, uint32_t offset);
    
    /**
     * @brief Validate Organization Extension TLV
     */
    ValidationError validate_organization_extension_tlv(const std::vector<uint8_t>& tlv_data, uint32_t offset);
}

} // namespace _2021
} // namespace AS
} // namespace _802_1
} // namespace IEEE

#endif // IEEE_802_1AS_2021_MESSAGE_VALIDATION_H
