/**
 * @file message_validation.cpp
 * @brief IEEE 802.1AS-2021 Message Validation Enhancement Implementation
 * @details Implementation of strict Table 10-7 compliance validation
 */

#include "message_validation.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace IEEE {
namespace _802_1 {
namespace AS {
namespace _2021 {

/**
 * @brief Implementation class for message validator
 */
class MessageValidator::Implementation {
public:
    // Validation configuration
    bool strict_mode = false;
    std::vector<uint8_t> allowed_domains = {0}; // Default: domain 0 only
    std::vector<uint8_t> allowed_message_types = {
        MessageFormat::SYNC, MessageFormat::FOLLOW_UP, MessageFormat::PDELAY_REQ,
        MessageFormat::PDELAY_RESP, MessageFormat::PDELAY_RESP_FOLLOW_UP,
        MessageFormat::ANNOUNCE, MessageFormat::SIGNALING, MessageFormat::MANAGEMENT
    };
    std::map<std::string, std::function<ValidationError(const std::vector<uint8_t>&)>> custom_rules;
    
    Implementation() = default;
    ~Implementation() = default;
    
    // Field extraction utilities with endianness handling
    uint16_t extract_uint16(const std::vector<uint8_t>& data, uint32_t offset) {
        if (offset + 1 >= data.size()) return 0;
        return (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
    }
    
    uint32_t extract_uint32(const std::vector<uint8_t>& data, uint32_t offset) {
        if (offset + 3 >= data.size()) return 0;
        return (static_cast<uint32_t>(data[offset]) << 24) |
               (static_cast<uint32_t>(data[offset + 1]) << 16) |
               (static_cast<uint32_t>(data[offset + 2]) << 8) |
               data[offset + 3];
    }
    
    uint64_t extract_uint64(const std::vector<uint8_t>& data, uint32_t offset) {
        if (offset + 7 >= data.size()) return 0;
        return (static_cast<uint64_t>(data[offset]) << 56) |
               (static_cast<uint64_t>(data[offset + 1]) << 48) |
               (static_cast<uint64_t>(data[offset + 2]) << 40) |
               (static_cast<uint64_t>(data[offset + 3]) << 32) |
               (static_cast<uint64_t>(data[offset + 4]) << 24) |
               (static_cast<uint64_t>(data[offset + 5]) << 16) |
               (static_cast<uint64_t>(data[offset + 6]) << 8) |
               data[offset + 7];
    }
    
    // Field validation utilities
    ValidationError create_error(ValidationResult code, const std::string& message, 
                                uint32_t byte_offset, const std::string& field_name,
                                const std::string& ieee_ref = "IEEE 802.1AS-2021 Table 10-7") {
        ValidationError error;
        error.error_code = code;
        error.error_message = message;
        error.byte_offset = byte_offset;
        error.field_name = field_name;
        error.ieee_reference = ieee_ref;
        return error;
    }
    
    bool is_reserved_field_zero(const std::vector<uint8_t>& data, uint32_t offset, uint32_t length) {
        for (uint32_t i = 0; i < length && offset + i < data.size(); ++i) {
            if (data[offset + i] != 0) return false;
        }
        return true;
    }
    
    std::string bytes_to_hex_string(const std::vector<uint8_t>& bytes, uint32_t offset, uint32_t length) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (uint32_t i = 0; i < length && offset + i < bytes.size(); ++i) {
            ss << std::setw(2) << static_cast<unsigned>(bytes[offset + i]);
            if (i < length - 1) ss << " ";
        }
        return ss.str();
    }
};

MessageValidator::MessageValidator() 
    : pImpl(std::make_unique<Implementation>()) {
}

MessageValidator::~MessageValidator() = default;

ValidationReport MessageValidator::validate_message(const std::vector<uint8_t>& message_data) {
    ValidationReport report;
    report.validation_time = std::chrono::steady_clock::now();
    report.message_length = static_cast<uint32_t>(message_data.size());
    
    // Basic length check
    if (message_data.size() < MessageFormat::PTP_HEADER_LENGTH) {
        ValidationError error = pImpl->create_error(
            ValidationResult::INVALID_HEADER_LENGTH,
            "Message too short for PTP header",
            0, "message_length"
        );
        report.errors.push_back(error);
        report.is_valid = false;
        return report;
    }
    
    // Extract message type for specific validation
    uint8_t message_type = message_data[MessageFormat::OFFSET_MESSAGE_TYPE] & 0x0F;
    report.message_type = message_type;
    
    // Validate header first
    ValidationReport header_report = validate_header(message_data);
    report.errors.insert(report.errors.end(), header_report.errors.begin(), header_report.errors.end());
    report.warnings.insert(report.warnings.end(), header_report.warnings.begin(), header_report.warnings.end());
    
    // Message type specific validation
    ValidationReport type_specific_report;
    switch (message_type) {
        case MessageFormat::ANNOUNCE:
            type_specific_report = validate_announce_message(message_data);
            report.message_type_name = "Announce";
            break;
        case MessageFormat::SYNC:
            type_specific_report = validate_sync_message(message_data);
            report.message_type_name = "Sync";
            break;
        case MessageFormat::FOLLOW_UP:
            type_specific_report = validate_follow_up_message(message_data);
            report.message_type_name = "Follow_Up";
            break;
        case MessageFormat::PDELAY_REQ:
            type_specific_report = validate_pdelay_req_message(message_data);
            report.message_type_name = "Pdelay_Req";
            break;
        case MessageFormat::PDELAY_RESP:
            type_specific_report = validate_pdelay_resp_message(message_data);
            report.message_type_name = "Pdelay_Resp";
            break;
        case MessageFormat::PDELAY_RESP_FOLLOW_UP:
            type_specific_report = validate_pdelay_resp_follow_up_message(message_data);
            report.message_type_name = "Pdelay_Resp_Follow_Up";
            break;
        case MessageFormat::SIGNALING:
            type_specific_report = validate_signaling_message(message_data);
            report.message_type_name = "Signaling";
            break;
        case MessageFormat::MANAGEMENT:
            type_specific_report = validate_management_message(message_data);
            report.message_type_name = "Management";
            break;
        default:
            ValidationError error = pImpl->create_error(
                ValidationResult::INVALID_MESSAGE_TYPE,
                "Unknown or unsupported message type: " + std::to_string(message_type),
                MessageFormat::OFFSET_MESSAGE_TYPE, "messageType"
            );
            report.errors.push_back(error);
            report.message_type_name = "Unknown";
            break;
    }
    
    // Merge type-specific results
    report.errors.insert(report.errors.end(), type_specific_report.errors.begin(), type_specific_report.errors.end());
    report.warnings.insert(report.warnings.end(), type_specific_report.warnings.begin(), type_specific_report.warnings.end());
    
    // Overall validation result
    report.is_valid = report.errors.empty();
    
    // Generate validation summary
    std::stringstream summary;
    summary << "IEEE 802.1AS-2021 Message Validation Report\n";
    summary << "Message Type: " << report.message_type_name << " (0x" << std::hex << static_cast<unsigned>(message_type) << ")\n";
    summary << "Message Length: " << report.message_length << " bytes\n";
    summary << "Validation Result: " << (report.is_valid ? "VALID" : "INVALID") << "\n";
    summary << "Errors: " << report.errors.size() << "\n";
    summary << "Warnings: " << report.warnings.size() << "\n";
    
    if (!report.errors.empty()) {
        summary << "\nValidation Errors:\n";
        for (const auto& error : report.errors) {
            summary << "- " << error.error_message << " (at byte " << error.byte_offset << ")\n";
        }
    }
    
    report.validation_summary = summary.str();
    
    return report;
}

ValidationReport MessageValidator::validate_header(const std::vector<uint8_t>& header_data) {
    ValidationReport report;
    report.validation_time = std::chrono::steady_clock::now();
    
    if (header_data.size() < MessageFormat::PTP_HEADER_LENGTH) {
        ValidationError error = pImpl->create_error(
            ValidationResult::INVALID_HEADER_LENGTH,
            "Header data too short",
            0, "header_length"
        );
        report.errors.push_back(error);
        report.is_valid = false;
        return report;
    }
    
    // Validate all header fields
    std::vector<ValidationError> header_errors = validate_header_fields(header_data);
    report.errors.insert(report.errors.end(), header_errors.begin(), header_errors.end());
    
    // Validate endianness
    std::vector<ValidationError> endian_errors = validate_endianness(header_data);
    report.errors.insert(report.errors.end(), endian_errors.begin(), endian_errors.end());
    
    // Validate reserved fields
    uint8_t message_type = header_data[MessageFormat::OFFSET_MESSAGE_TYPE] & 0x0F;
    std::vector<ValidationError> reserved_errors = validate_reserved_fields(header_data, message_type);
    report.errors.insert(report.errors.end(), reserved_errors.begin(), reserved_errors.end());
    
    report.is_valid = report.errors.empty();
    return report;
}

std::vector<ValidationError> MessageValidator::validate_header_fields(const std::vector<uint8_t>& header_data) {
    std::vector<ValidationError> errors;
    
    // Transport Specific field (4 bits, should be 0x1 for 802.1AS)
    uint8_t transport_specific = (header_data[0] >> 4) & 0x0F;
    if (transport_specific != MessageFormat::TRANSPORT_SPECIFIC_802_1AS) {
        errors.push_back(pImpl->create_error(
            ValidationResult::INVALID_FLAGS,
            "Transport Specific field should be 0x1 for IEEE 802.1AS",
            MessageFormat::OFFSET_TRANSPORT_SPECIFIC, "transportSpecific"
        ));
    }
    
    // Message Type (4 bits)
    uint8_t message_type = header_data[0] & 0x0F;
    if (!is_supported_message_type(message_type)) {
        errors.push_back(pImpl->create_error(
            ValidationResult::INVALID_MESSAGE_TYPE,
            "Unsupported message type for IEEE 802.1AS-2021",
            MessageFormat::OFFSET_MESSAGE_TYPE, "messageType"
        ));
    }
    
    // Reserved field 1 (4 bits, must be 0)
    uint8_t reserved1 = (header_data[1] >> 4) & 0x0F;
    if (reserved1 != 0) {
        errors.push_back(pImpl->create_error(
            ValidationResult::RESERVED_FIELD_VIOLATION,
            "Reserved field 1 must be zero",
            MessageFormat::OFFSET_RESERVED_1, "reserved1"
        ));
    }
    
    // Version PTP (4 bits, must be 2)
    uint8_t version = header_data[1] & 0x0F;
    if (version != MessageFormat::PTP_VERSION) {
        errors.push_back(pImpl->create_error(
            ValidationResult::INVALID_VERSION,
            "PTP version must be 2 for IEEE 802.1AS-2021",
            MessageFormat::OFFSET_VERSION_PTP, "versionPTP"
        ));
    }
    
    // Message Length (16 bits)
    uint16_t message_length = pImpl->extract_uint16(header_data, MessageFormat::OFFSET_MESSAGE_LENGTH);
    uint32_t expected_length = get_expected_message_length(message_type);
    if (expected_length != 0 && message_length < expected_length) {
        errors.push_back(pImpl->create_error(
            ValidationResult::INVALID_MESSAGE_LENGTH,
            "Message length too short for message type",
            MessageFormat::OFFSET_MESSAGE_LENGTH, "messageLength"
        ));
    }
    
    // Domain Number (8 bits)
    uint8_t domain = header_data[MessageFormat::OFFSET_DOMAIN_NUMBER];
    if (std::find(pImpl->allowed_domains.begin(), pImpl->allowed_domains.end(), domain) == pImpl->allowed_domains.end()) {
        errors.push_back(pImpl->create_error(
            ValidationResult::INVALID_DOMAIN,
            "Domain number not in allowed list",
            MessageFormat::OFFSET_DOMAIN_NUMBER, "domainNumber"
        ));
    }
    
    // Reserved field 2 (8 bits, must be 0)
    if (header_data[MessageFormat::OFFSET_RESERVED_2] != 0) {
        errors.push_back(pImpl->create_error(
            ValidationResult::RESERVED_FIELD_VIOLATION,
            "Reserved field 2 must be zero",
            MessageFormat::OFFSET_RESERVED_2, "reserved2"
        ));
    }
    
    // Flags field validation
    ValidationError flags_error = validate_flags_field(header_data, MessageFormat::OFFSET_FLAGS, message_type);
    if (flags_error.error_code != ValidationResult::VALID) {
        errors.push_back(flags_error);
    }
    
    // Correction field validation
    ValidationError correction_error = validate_correction_field(header_data, MessageFormat::OFFSET_CORRECTION_FIELD);
    if (correction_error.error_code != ValidationResult::VALID) {
        errors.push_back(correction_error);
    }
    
    // Reserved field 3 (32 bits, must be 0)
    if (!pImpl->is_reserved_field_zero(header_data, MessageFormat::OFFSET_RESERVED_3, 4)) {
        errors.push_back(pImpl->create_error(
            ValidationResult::RESERVED_FIELD_VIOLATION,
            "Reserved field 3 must be zero",
            MessageFormat::OFFSET_RESERVED_3, "reserved3"
        ));
    }
    
    // Source Port Identity validation
    ValidationError port_id_error = validate_port_identity_field(header_data, MessageFormat::OFFSET_SOURCE_PORT_IDENTITY);
    if (port_id_error.error_code != ValidationResult::VALID) {
        errors.push_back(port_id_error);
    }
    
    return errors;
}

ValidationError MessageValidator::validate_flags_field(const std::vector<uint8_t>& message_data, uint32_t offset, uint8_t message_type) {
    if (offset + 1 >= message_data.size()) {
        return pImpl->create_error(ValidationResult::INVALID_FLAGS, "Insufficient data for flags field", offset, "flags");
    }
    
    uint16_t flags = pImpl->extract_uint16(message_data, offset);
    
    // IEEE 802.1AS-2021 specific flag requirements
    switch (message_type) {
        case MessageFormat::ANNOUNCE:
            // Announce messages should have PTP_TIMESCALE flag set
            if (!(flags & MessageFormat::FLAG_PTP_TIMESCALE)) {
                return pImpl->create_error(
                    ValidationResult::INVALID_FLAGS,
                    "Announce message should have PTP_TIMESCALE flag set",
                    offset, "flags"
                );
            }
            break;
            
        case MessageFormat::SYNC:
        case MessageFormat::FOLLOW_UP:
            // Sync and Follow_Up messages should not have leap second flags
            if (flags & (MessageFormat::FLAG_LEAP61 | MessageFormat::FLAG_LEAP59)) {
                return pImpl->create_error(
                    ValidationResult::INVALID_FLAGS,
                    "Sync/Follow_Up messages should not have leap second flags",
                    offset, "flags"
                );
            }
            break;
    }
    
    return ValidationError(); // Valid
}

ValidationError MessageValidator::validate_correction_field(const std::vector<uint8_t>& message_data, uint32_t offset) {
    if (offset + 7 >= message_data.size()) {
        return pImpl->create_error(ValidationResult::INVALID_CORRECTION_FIELD, "Insufficient data for correction field", offset, "correctionField");
    }
    
    uint64_t correction = pImpl->extract_uint64(message_data, offset);
    
    // IEEE 802.1AS-2021 correction field is in nanoseconds (1/65536 ns units)
    // Check for reasonable range (not more than 1 second)
    const uint64_t MAX_CORRECTION_NS = 1000000000ULL * 65536; // 1 second in 1/65536 ns units
    
    if (correction > MAX_CORRECTION_NS) {
        return pImpl->create_error(
            ValidationResult::INVALID_CORRECTION_FIELD,
            "Correction field value exceeds reasonable range",
            offset, "correctionField"
        );
    }
    
    return ValidationError(); // Valid
}

ValidationError MessageValidator::validate_port_identity_field(const std::vector<uint8_t>& port_id_data, uint32_t offset) {
    if (offset + 9 >= port_id_data.size()) {
        return pImpl->create_error(ValidationResult::INVALID_SOURCE_PORT_IDENTITY, "Insufficient data for port identity", offset, "sourcePortIdentity");
    }
    
    // Clock Identity (8 bytes) - check for all zeros (invalid)
    bool all_zeros = true;
    for (uint32_t i = 0; i < 8; ++i) {
        if (port_id_data[offset + i] != 0) {
            all_zeros = false;
            break;
        }
    }
    
    if (all_zeros) {
        return pImpl->create_error(
            ValidationResult::INVALID_SOURCE_PORT_IDENTITY,
            "Clock identity cannot be all zeros",
            offset, "clockIdentity"
        );
    }
    
    // Port Number (2 bytes) - check for valid range
    uint16_t port_number = pImpl->extract_uint16(port_id_data, offset + 8);
    if (port_number == 0) {
        return pImpl->create_error(
            ValidationResult::INVALID_SOURCE_PORT_IDENTITY,
            "Port number cannot be zero",
            offset + 8, "portNumber"
        );
    }
    
    return ValidationError(); // Valid
}

std::vector<ValidationError> MessageValidator::validate_endianness(const std::vector<uint8_t>& message_data) {
    std::vector<ValidationError> errors;
    
    // IEEE 802.1AS-2021 uses network byte order (big-endian)
    // Check key fields for proper endianness
    
    if (message_data.size() >= 4) {
        // Message Length field should be in network byte order
        uint16_t message_length_be = pImpl->extract_uint16(message_data, MessageFormat::OFFSET_MESSAGE_LENGTH);
        uint16_t message_length_le = (message_data[MessageFormat::OFFSET_MESSAGE_LENGTH + 1] << 8) | 
                                     message_data[MessageFormat::OFFSET_MESSAGE_LENGTH];
        
        // If little-endian interpretation gives a more reasonable length, flag as error
        if (message_length_le < message_data.size() && message_length_be > message_data.size() * 2) {
            errors.push_back(pImpl->create_error(
                ValidationResult::ENDIANNESS_ERROR,
                "Message length field appears to be in little-endian format",
                MessageFormat::OFFSET_MESSAGE_LENGTH, "messageLength"
            ));
        }
    }
    
    return errors;
}

std::vector<ValidationError> MessageValidator::validate_reserved_fields(const std::vector<uint8_t>& message_data, uint8_t message_type) {
    std::vector<ValidationError> errors;
    
    // Check all reserved fields are zero according to Table 10-7
    
    // Reserved1 (4 bits at offset 1, upper nibble)
    if ((message_data[1] & 0xF0) != 0) {
        errors.push_back(pImpl->create_error(
            ValidationResult::RESERVED_FIELD_VIOLATION,
            "Reserved field 1 must be zero",
            1, "reserved1"
        ));
    }
    
    // Reserved2 (8 bits at offset 5)
    if (message_data.size() > 5 && message_data[5] != 0) {
        errors.push_back(pImpl->create_error(
            ValidationResult::RESERVED_FIELD_VIOLATION,
            "Reserved field 2 must be zero",
            5, "reserved2"
        ));
    }
    
    // Reserved3 (32 bits at offset 16-19)
    if (message_data.size() > 19) {
        for (uint32_t i = 16; i < 20; ++i) {
            if (message_data[i] != 0) {
                errors.push_back(pImpl->create_error(
                    ValidationResult::RESERVED_FIELD_VIOLATION,
                    "Reserved field 3 must be zero",
                    i, "reserved3"
                ));
                break; // Only report once
            }
        }
    }
    
    return errors;
}

// Message-specific validation implementations
ValidationReport MessageValidator::validate_announce_message(const std::vector<uint8_t>& message_data) {
    ValidationReport report;
    
    if (message_data.size() < MessageFormat::ANNOUNCE_MESSAGE_LENGTH) {
        ValidationError error = pImpl->create_error(
            ValidationResult::INVALID_MESSAGE_LENGTH,
            "Announce message too short",
            0, "messageLength"
        );
        report.errors.push_back(error);
        report.is_valid = false;
        return report;
    }
    
    // Validate timestamp at offset 34
    ValidationError timestamp_error = validate_timestamp_field(message_data, 34);
    if (timestamp_error.error_code != ValidationResult::VALID) {
        report.errors.push_back(timestamp_error);
    }
    
    // Additional Announce-specific validations would go here
    // (currentUtcOffset, grandmasterPriority1, grandmasterClockQuality, etc.)
    
    report.is_valid = report.errors.empty();
    return report;
}

ValidationReport MessageValidator::validate_sync_message(const std::vector<uint8_t>& message_data) {
    ValidationReport report;
    
    if (message_data.size() < MessageFormat::SYNC_MESSAGE_LENGTH) {
        ValidationError error = pImpl->create_error(
            ValidationResult::INVALID_MESSAGE_LENGTH,
            "Sync message too short",
            0, "messageLength"
        );
        report.errors.push_back(error);
        report.is_valid = false;
        return report;
    }
    
    // Validate origin timestamp at offset 34
    ValidationError timestamp_error = validate_timestamp_field(message_data, 34);
    if (timestamp_error.error_code != ValidationResult::VALID) {
        report.errors.push_back(timestamp_error);
    }
    
    report.is_valid = report.errors.empty();
    return report;
}

ValidationError MessageValidator::validate_timestamp_field(const std::vector<uint8_t>& timestamp_data, uint32_t offset) {
    if (offset + 9 >= timestamp_data.size()) {
        return pImpl->create_error(ValidationResult::INVALID_TIMESTAMP, "Insufficient data for timestamp", offset, "timestamp");
    }
    
    // Timestamp format: 48-bit seconds field + 32-bit nanoseconds field
    uint64_t seconds = 0;
    for (int i = 0; i < 6; ++i) {
        seconds = (seconds << 8) | timestamp_data[offset + i];
    }
    
    uint32_t nanoseconds = pImpl->extract_uint32(timestamp_data, offset + 6);
    
    // Validate nanoseconds field (must be < 1,000,000,000)
    if (nanoseconds >= 1000000000U) {
        return pImpl->create_error(
            ValidationResult::INVALID_TIMESTAMP,
            "Nanoseconds field must be less than 1,000,000,000",
            offset + 6, "nanoseconds"
        );
    }
    
    return ValidationError(); // Valid
}

// Utility methods
bool MessageValidator::is_supported_message_type(uint8_t message_type) {
    return std::find(pImpl->allowed_message_types.begin(), pImpl->allowed_message_types.end(), message_type) 
           != pImpl->allowed_message_types.end();
}

uint32_t MessageValidator::get_expected_message_length(uint8_t message_type) {
    switch (message_type) {
        case MessageFormat::SYNC: return MessageFormat::SYNC_MESSAGE_LENGTH;
        case MessageFormat::FOLLOW_UP: return MessageFormat::FOLLOW_UP_MESSAGE_LENGTH;
        case MessageFormat::PDELAY_REQ: return MessageFormat::PDELAY_REQ_MESSAGE_LENGTH;
        case MessageFormat::PDELAY_RESP: return MessageFormat::PDELAY_RESP_MESSAGE_LENGTH;
        case MessageFormat::PDELAY_RESP_FOLLOW_UP: return MessageFormat::PDELAY_RESP_FOLLOW_UP_MESSAGE_LENGTH;
        case MessageFormat::ANNOUNCE: return MessageFormat::ANNOUNCE_MESSAGE_LENGTH;
        case MessageFormat::SIGNALING: return MessageFormat::SIGNALING_MESSAGE_LENGTH; // Minimum
        case MessageFormat::MANAGEMENT: return MessageFormat::MANAGEMENT_MESSAGE_LENGTH; // Minimum
        default: return 0; // Unknown
    }
}

std::string MessageValidator::validation_result_to_string(ValidationResult result) {
    switch (result) {
        case ValidationResult::VALID: return "VALID";
        case ValidationResult::INVALID_HEADER_LENGTH: return "INVALID_HEADER_LENGTH";
        case ValidationResult::INVALID_MESSAGE_TYPE: return "INVALID_MESSAGE_TYPE";
        case ValidationResult::INVALID_VERSION: return "INVALID_VERSION";
        case ValidationResult::INVALID_DOMAIN: return "INVALID_DOMAIN";
        case ValidationResult::INVALID_FLAGS: return "INVALID_FLAGS";
        case ValidationResult::INVALID_CORRECTION_FIELD: return "INVALID_CORRECTION_FIELD";
        case ValidationResult::INVALID_SOURCE_PORT_IDENTITY: return "INVALID_SOURCE_PORT_IDENTITY";
        case ValidationResult::INVALID_SEQUENCE_ID: return "INVALID_SEQUENCE_ID";
        case ValidationResult::INVALID_CONTROL_FIELD: return "INVALID_CONTROL_FIELD";
        case ValidationResult::INVALID_LOG_MESSAGE_INTERVAL: return "INVALID_LOG_MESSAGE_INTERVAL";
        case ValidationResult::INVALID_TIMESTAMP: return "INVALID_TIMESTAMP";
        case ValidationResult::INVALID_TLV_FORMAT: return "INVALID_TLV_FORMAT";
        case ValidationResult::INVALID_MESSAGE_LENGTH: return "INVALID_MESSAGE_LENGTH";
        case ValidationResult::INVALID_CHECKSUM: return "INVALID_CHECKSUM";
        case ValidationResult::ENDIANNESS_ERROR: return "ENDIANNESS_ERROR";
        case ValidationResult::FIELD_ALIGNMENT_ERROR: return "FIELD_ALIGNMENT_ERROR";
        case ValidationResult::RESERVED_FIELD_VIOLATION: return "RESERVED_FIELD_VIOLATION";
        default: return "UNKNOWN_ERROR";
    }
}

// Configuration methods
void MessageValidator::set_strict_mode(bool strict) {
    pImpl->strict_mode = strict;
}

void MessageValidator::set_allowed_domains(const std::vector<uint8_t>& allowed_domains) {
    pImpl->allowed_domains = allowed_domains;
}

void MessageValidator::set_message_type_filter(const std::vector<uint8_t>& message_types) {
    pImpl->allowed_message_types = message_types;
}

// Placeholder implementations for additional methods
ValidationReport MessageValidator::validate_message_type(const std::vector<uint8_t>& message_data, uint8_t expected_type) {
    ValidationReport report = validate_message(message_data);
    if (report.message_type != expected_type) {
        ValidationError error = pImpl->create_error(
            ValidationResult::INVALID_MESSAGE_TYPE,
            "Message type does not match expected type",
            0, "messageType"
        );
        report.errors.push_back(error);
        report.is_valid = false;
    }
    return report;
}

ValidationReport MessageValidator::validate_follow_up_message(const std::vector<uint8_t>& message_data) {
    // Similar to sync message validation
    return validate_sync_message(message_data);
}

ValidationReport MessageValidator::validate_pdelay_req_message(const std::vector<uint8_t>& message_data) {
    ValidationReport report;
    if (message_data.size() < MessageFormat::PDELAY_REQ_MESSAGE_LENGTH) {
        ValidationError error = pImpl->create_error(ValidationResult::INVALID_MESSAGE_LENGTH, "Pdelay_Req message too short", 0, "messageLength");
        report.errors.push_back(error);
    }
    report.is_valid = report.errors.empty();
    return report;
}

ValidationReport MessageValidator::validate_pdelay_resp_message(const std::vector<uint8_t>& message_data) {
    ValidationReport report;
    if (message_data.size() < MessageFormat::PDELAY_RESP_MESSAGE_LENGTH) {
        ValidationError error = pImpl->create_error(ValidationResult::INVALID_MESSAGE_LENGTH, "Pdelay_Resp message too short", 0, "messageLength");
        report.errors.push_back(error);
    }
    report.is_valid = report.errors.empty();
    return report;
}

ValidationReport MessageValidator::validate_pdelay_resp_follow_up_message(const std::vector<uint8_t>& message_data) {
    ValidationReport report;
    if (message_data.size() < MessageFormat::PDELAY_RESP_FOLLOW_UP_MESSAGE_LENGTH) {
        ValidationError error = pImpl->create_error(ValidationResult::INVALID_MESSAGE_LENGTH, "Pdelay_Resp_Follow_Up message too short", 0, "messageLength");
        report.errors.push_back(error);
    }
    report.is_valid = report.errors.empty();
    return report;
}

ValidationReport MessageValidator::validate_signaling_message(const std::vector<uint8_t>& message_data) {
    ValidationReport report;
    if (message_data.size() < MessageFormat::SIGNALING_MESSAGE_LENGTH) {
        ValidationError error = pImpl->create_error(ValidationResult::INVALID_MESSAGE_LENGTH, "Signaling message too short", 0, "messageLength");
        report.errors.push_back(error);
    }
    report.is_valid = report.errors.empty();
    return report;
}

ValidationReport MessageValidator::validate_management_message(const std::vector<uint8_t>& message_data) {
    ValidationReport report;
    if (message_data.size() < MessageFormat::MANAGEMENT_MESSAGE_LENGTH) {
        ValidationError error = pImpl->create_error(ValidationResult::INVALID_MESSAGE_LENGTH, "Management message too short", 0, "messageLength");
        report.errors.push_back(error);
    }
    report.is_valid = report.errors.empty();
    return report;
}

std::vector<ValidationError> MessageValidator::validate_tlv_format(const std::vector<uint8_t>& tlv_data, uint32_t offset) {
    // TLV validation implementation would go here
    return std::vector<ValidationError>();
}

std::vector<ValidationError> MessageValidator::validate_field_alignment(const std::vector<uint8_t>& message_data) {
    // Field alignment validation would go here
    return std::vector<ValidationError>();
}

void MessageValidator::set_custom_validation_rules(const std::map<std::string, std::function<ValidationError(const std::vector<uint8_t>&)>>& rules) {
    pImpl->custom_rules = rules;
}

std::string MessageValidator::generate_validation_summary(const ValidationReport& report) {
    return report.validation_summary;
}

template<>
uint16_t MessageValidator::extract_field<uint16_t>(const std::vector<uint8_t>& data, uint32_t offset) {
    return pImpl->extract_uint16(data, offset);
}

template<>
uint32_t MessageValidator::extract_field<uint32_t>(const std::vector<uint8_t>& data, uint32_t offset) {
    return pImpl->extract_uint32(data, offset);
}

template<>
uint64_t MessageValidator::extract_field<uint64_t>(const std::vector<uint8_t>& data, uint32_t offset) {
    return pImpl->extract_uint64(data, offset);
}

// TLV validation utilities
namespace TLVValidation {
    ValidationError validate_tlv_header(const std::vector<uint8_t>& tlv_data, uint32_t offset) {
        ValidationError error;
        if (offset + 3 >= tlv_data.size()) {
            error.error_code = ValidationResult::INVALID_TLV_FORMAT;
            error.error_message = "TLV header too short";
            error.byte_offset = offset;
            error.field_name = "tlvHeader";
        }
        return error;
    }
    
    ValidationError validate_organization_extension_tlv(const std::vector<uint8_t>& tlv_data, uint32_t offset) {
        ValidationError error;
        // Organization Extension TLV validation would be implemented here
        return error;
    }
}

} // namespace _2021
} // namespace AS
} // namespace _802_1
} // namespace IEEE
