/**
 * IEEE 1722-2016 AVTP Protocol Implementation - Simple Working Version
 */

#include "ieee_1722_2016_streaming.h"
#include <sstream>

namespace avtp_protocol {
namespace ieee_1722_2016 {

// Simple implementation that compiles
size_t get_avtp_payload_offset(Subtype subtype) {
    switch (subtype) {
        case Subtype::IEC61883_IIDC: return 32;
        case Subtype::MMA_STREAM: return 28;
        case Subtype::AAF: return 28;
        case Subtype::CVF: return 28;
        case Subtype::CRF: return 32;
        case Subtype::TSCF: return 28;
        case Subtype::SVF: return 28;
        case Subtype::RVF: return 36;
        default: return 24;
    }
}

std::string subtype_to_string(Subtype subtype) {
    switch (subtype) {
        case Subtype::IEC61883_IIDC: return "IEC 61883/IIDC";
        case Subtype::MMA_STREAM: return "MMA Stream";
        case Subtype::AAF: return "AVTP Audio Format";
        case Subtype::CVF: return "Compressed Video Format";
        case Subtype::CRF: return "Clock Reference Format";
        case Subtype::TSCF: return "Time-Synchronous Control Format";
        case Subtype::SVF: return "SDI Video Format";
        case Subtype::RVF: return "Raw Video Format";
        case Subtype::ADP: return "AVDECC Discovery Protocol";
        case Subtype::AECP: return "AVDECC Enumeration Control Protocol";
        case Subtype::ACMP: return "AVDECC Connection Management Protocol";
        case Subtype::MAAP: return "MAAP Protocol";
        case Subtype::EF_CONTROL: return "Experimental Format Control";
        default: {
            std::ostringstream oss;
            oss << "Unknown (" << static_cast<int>(subtype) << ")";
            return oss.str();
        }
    }
}

bool is_valid_subtype(Subtype subtype) {
    return static_cast<uint8_t>(subtype) <= 0xFF;
}

uint32_t calculate_crc32(const uint8_t* data, size_t length) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int k = 0; k < 8; k++) {
            crc = (crc >> 1) ^ (0xEDB88320 & (-(int)(crc & 1)));
        }
    }
    return ~crc;
}

} // namespace ieee_1722_2016
} // namespace avtp_protocol
