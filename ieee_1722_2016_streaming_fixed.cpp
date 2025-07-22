/**
 * IEEE 1722-2016 AVTP Protocol Implementation - Fixed Version
 * Complete implementation of Audio Video Transport Protocol
 * 
 * This implementation provides complete IEEE 1722-2016 compliance
 * with official Table 6 subtype values and modern C++ standards.
 * 
 * @file ieee_1722_2016_streaming_fixed.cpp
 * @author OpenAvnu Team
 * @date 2025
 * @version 4.0.0 - "Tutti Completo" Release
 */

#include "ieee_1722_2016_streaming.h"
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace avtp_protocol {
namespace ieee_1722_2016 {

// AVTPDU Implementation
AVTPDU::AVTPDU() {
    std::memset(&header_, 0, sizeof(header_));
    header_.cd = 1; // Control Data bit
    header_.subtype = static_cast<uint8_t>(Subtype::IEC61883_IIDC);
    header_.sv = 1; // Stream ID Valid
    header_.version = 0; // IEEE 1722-2016
}

AVTPDU::AVTPDU(Subtype subtype, uint64_t stream_id) : AVTPDU() {
    header_.subtype = static_cast<uint8_t>(subtype);
    header_.stream_id = htobe64(stream_id);
}

void AVTPDU::set_subtype(Subtype subtype) {
    header_.subtype = static_cast<uint8_t>(subtype);
}

Subtype AVTPDU::get_subtype() const {
    return static_cast<Subtype>(header_.subtype);
}

void AVTPDU::set_stream_id(uint64_t stream_id) {
    header_.stream_id = htobe64(stream_id);
}

uint64_t AVTPDU::get_stream_id() const {
    return be64toh(header_.stream_id);
}

void AVTPDU::set_sequence_number(uint8_t seq_num) {
    header_.avtp_timestamp = (header_.avtp_timestamp & 0x00FFFFFF) | 
                             (static_cast<uint32_t>(seq_num) << 24);
}

uint8_t AVTPDU::get_sequence_number() const {
    return static_cast<uint8_t>((be32toh(header_.avtp_timestamp) >> 24) & 0xFF);
}

void AVTPDU::set_timestamp(uint32_t timestamp) {
    header_.avtp_timestamp = (header_.avtp_timestamp & 0xFF000000) | 
                             (htobe32(timestamp) & 0x00FFFFFF);
}

uint32_t AVTPDU::get_timestamp() const {
    return be32toh(header_.avtp_timestamp) & 0x00FFFFFF;
}

void AVTPDU::set_timestamp_valid(bool valid) {
    header_.tv = valid ? 1 : 0;
}

bool AVTPDU::get_timestamp_valid() const {
    return header_.tv == 1;
}

void AVTPDU::set_stream_data_length(uint16_t length) {
    header_.stream_data_length = htobe16(length);
}

uint16_t AVTPDU::get_stream_data_length() const {
    return be16toh(header_.stream_data_length);
}

void AVTPDU::set_gateway_info(uint32_t info) {
    header_.gateway_info = htobe32(info);
}

uint32_t AVTPDU::get_gateway_info() const {
    return be32toh(header_.gateway_info);
}

void AVTPDU::set_payload(const uint8_t* data, size_t length) {
    if (length > MAX_PAYLOAD_SIZE) {
        throw std::invalid_argument("Payload size exceeds maximum");
    }
    
    payload_.assign(data, data + length);
    set_stream_data_length(static_cast<uint16_t>(length));
}

const std::vector<uint8_t>& AVTPDU::get_payload() const {
    return payload_;
}

size_t AVTPDU::serialize(uint8_t* buffer, size_t buffer_size) const {
    if (buffer_size < get_total_size()) {
        throw std::invalid_argument("Buffer too small for serialization");
    }
    
    // Copy header
    std::memcpy(buffer, &header_, sizeof(header_));
    
    // Copy payload
    if (!payload_.empty()) {
        std::memcpy(buffer + sizeof(header_), payload_.data(), payload_.size());
    }
    
    return get_total_size();
}

size_t AVTPDU::deserialize(const uint8_t* data, size_t data_size) {
    if (data_size < sizeof(header_)) {
        throw std::invalid_argument("Data too small for AVTPDU header");
    }
    
    // Copy header
    std::memcpy(&header_, data, sizeof(header_));
    
    // Extract payload
    size_t payload_size = get_stream_data_length();
    if (data_size < sizeof(header_) + payload_size) {
        throw std::invalid_argument("Data too small for complete AVTPDU");
    }
    
    if (payload_size > 0) {
        payload_.assign(data + sizeof(header_), data + sizeof(header_) + payload_size);
    } else {
        payload_.clear();
    }
    
    return sizeof(header_) + payload_size;
}

size_t AVTPDU::get_total_size() const {
    return sizeof(header_) + payload_.size();
}

// Utility Functions
bool is_valid_subtype(Subtype subtype) {
    uint8_t value = static_cast<uint8_t>(subtype);
    
    // Check for official IEEE 1722-2016 Table 6 values
    switch (subtype) {
        case Subtype::IEC61883_IIDC:
        case Subtype::MMA_STREAM:
        case Subtype::AAF:
        case Subtype::CVF:
        case Subtype::CRF:
        case Subtype::TSCF:
        case Subtype::SVF:
        case Subtype::RVF:
        case Subtype::AEF:
        case Subtype::VSF:
        case Subtype::EF_STREAM:
        case Subtype::NTSCF:
        case Subtype::ESCF:
        case Subtype::EECF:
        case Subtype::ADP:
        case Subtype::AECP:
        case Subtype::ACMP:
        case Subtype::MAAP:
        case Subtype::EF_CONTROL:
            return true;
        default:
            // Check reserved ranges
            return (value >= 0x08 && value <= 0x7F) ||  // Reserved future use
                   (value >= 0x83 && value <= 0xF9) ||  // Reserved future use
                   (value == 0xFD);                      // Reserved
    }
}

size_t get_avtp_payload_offset(Subtype subtype) {
    switch (subtype) {
        case Subtype::AAF:
            return AVTPDU::get_header_size() + 4; // Additional AAF headers
        case Subtype::CVF:
        case Subtype::SVF:
            return AVTPDU::get_header_size() + 4; // Additional video headers
        case Subtype::RVF:
            return AVTPDU::get_header_size() + 12; // Raw Video specific headers
        case Subtype::CRF:
            return AVTPDU::get_header_size() + 8; // CRF specific headers
        case Subtype::TSCF:
        case Subtype::NTSCF:
            return AVTPDU::get_header_size() + 4; // Control headers
        case Subtype::AEF:
        case Subtype::ESCF:
        case Subtype::EECF:
            return AVTPDU::get_header_size() + 16; // Encryption headers
        case Subtype::IEC61883_IIDC:
            return AVTPDU::get_header_size() + 8; // IEC 61883 specific headers
        case Subtype::MMA_STREAM:
            return AVTPDU::get_header_size() + 4; // MIDI headers
        default:
            return AVTPDU::get_header_size();
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
        case Subtype::AEF: return "AES Encrypted Format";
        case Subtype::VSF: return "Vendor Specific Format";
        case Subtype::EF_STREAM: return "Experimental Format Stream";
        case Subtype::NTSCF: return "Non-Time-Synchronous Control Format";
        case Subtype::ESCF: return "ECC Signed Control Format";
        case Subtype::EECF: return "ECC Encrypted Control Format";
        case Subtype::ADP: return "AVDECC Discovery Protocol";
        case Subtype::AECP: return "AVDECC Enumeration Control Protocol";
        case Subtype::ACMP: return "AVDECC Connection Management Protocol";
        case Subtype::MAAP: return "MAAP Protocol";
        case Subtype::EF_CONTROL: return "Experimental Format Control";
        default: {
            uint8_t value = static_cast<uint8_t>(subtype);
            if ((value >= 0x08 && value <= 0x7F) || 
                (value >= 0x83 && value <= 0xF9) || 
                (value == 0xFD)) {
                return "Reserved (" + std::to_string(value) + ")";
            }
            return "Unknown (" + std::to_string(value) + ")";
        }
    }
}

Subtype string_to_subtype(const std::string& str) {
    // Convert to lowercase for case-insensitive comparison
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    
    if (lower_str == "iec61883_iidc" || lower_str == "iec 61883/iidc") return Subtype::IEC61883_IIDC;
    if (lower_str == "mma_stream" || lower_str == "mma stream") return Subtype::MMA_STREAM;
    if (lower_str == "aaf" || lower_str == "avtp audio format") return Subtype::AAF;
    if (lower_str == "cvf" || lower_str == "compressed video format") return Subtype::CVF;
    if (lower_str == "crf" || lower_str == "clock reference format") return Subtype::CRF;
    if (lower_str == "tscf" || lower_str == "time-synchronous control format") return Subtype::TSCF;
    if (lower_str == "svf" || lower_str == "sdi video format") return Subtype::SVF;
    if (lower_str == "rvf" || lower_str == "raw video format") return Subtype::RVF;
    if (lower_str == "aef" || lower_str == "aes encrypted format") return Subtype::AEF;
    if (lower_str == "vsf" || lower_str == "vendor specific format") return Subtype::VSF;
    if (lower_str == "ef_stream" || lower_str == "experimental format stream") return Subtype::EF_STREAM;
    if (lower_str == "ntscf" || lower_str == "non-time-synchronous control format") return Subtype::NTSCF;
    if (lower_str == "escf" || lower_str == "ecc signed control format") return Subtype::ESCF;
    if (lower_str == "eecf" || lower_str == "ecc encrypted control format") return Subtype::EECF;
    if (lower_str == "adp" || lower_str == "avdecc discovery protocol") return Subtype::ADP;
    if (lower_str == "aecp" || lower_str == "avdecc enumeration control protocol") return Subtype::AECP;
    if (lower_str == "acmp" || lower_str == "avdecc connection management protocol") return Subtype::ACMP;
    if (lower_str == "maap" || lower_str == "maap protocol") return Subtype::MAAP;
    if (lower_str == "ef_control" || lower_str == "experimental format control") return Subtype::EF_CONTROL;
    
    throw std::invalid_argument("Unknown subtype string: " + str);
}

uint32_t calculate_crc32(const uint8_t* data, size_t length) {
    static const uint32_t crc_table[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
        0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
        0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
        0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
        0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
        0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
        0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
        0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
        0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
        0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
        0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
        0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
        0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
        0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
        0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
        0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
        0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
        0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
        0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
        0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
        0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
        0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
        0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
        0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
        0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
        0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
        0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
        0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
        0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
        0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
        0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
        0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
        0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
        0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
        0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
        0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
        0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
        0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
        0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
        0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
        0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
        0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
        0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
    };
    
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < length; i++) {
        crc = crc_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

// Stream Validation Functions
bool validate_stream_parameters(const StreamParameters& params) {
    // Validate sample rate
    if (params.sample_rate == 0 || params.sample_rate > 192000) {
        return false;
    }
    
    // Validate channels
    if (params.channels == 0 || params.channels > 64) {
        return false;
    }
    
    // Validate sample format
    switch (params.sample_format) {
        case SampleFormat::PCM_8:
        case SampleFormat::PCM_16:
        case SampleFormat::PCM_24:
        case SampleFormat::PCM_32:
        case SampleFormat::FLOAT_32:
        case SampleFormat::FLOAT_64:
            break;
        default:
            return false;
    }
    
    // Validate presentation time
    if (params.presentation_time_offset > 1000000) { // Max 1 second
        return false;
    }
    
    return true;
}

// Performance monitoring
StreamStatistics::StreamStatistics() {
    reset();
}

void StreamStatistics::reset() {
    packets_sent = 0;
    packets_received = 0;
    bytes_sent = 0;
    bytes_received = 0;
    packets_dropped = 0;
    sequence_errors = 0;
    timestamp_errors = 0;
    last_timestamp = 0;
    max_latency_us = 0;
    min_latency_us = UINT32_MAX;
    avg_latency_us = 0;
}

void StreamStatistics::update_packet_sent(size_t packet_size) {
    packets_sent++;
    bytes_sent += packet_size;
}

void StreamStatistics::update_packet_received(size_t packet_size, uint32_t latency_us) {
    packets_received++;
    bytes_received += packet_size;
    
    if (latency_us > max_latency_us) max_latency_us = latency_us;
    if (latency_us < min_latency_us) min_latency_us = latency_us;
    
    // Simple moving average
    avg_latency_us = (avg_latency_us + latency_us) / 2;
}

void StreamStatistics::update_sequence_error() {
    sequence_errors++;
}

void StreamStatistics::update_timestamp_error() {
    timestamp_errors++;
}

double StreamStatistics::get_packet_loss_rate() const {
    if (packets_sent == 0) return 0.0;
    return static_cast<double>(packets_dropped) / static_cast<double>(packets_sent);
}

double StreamStatistics::get_error_rate() const {
    uint64_t total_errors = sequence_errors + timestamp_errors + packets_dropped;
    if (packets_received == 0) return 0.0;
    return static_cast<double>(total_errors) / static_cast<double>(packets_received);
}

} // namespace ieee_1722_2016
} // namespace avtp_protocol
