/**
 * @file avtpdu.cpp  
 * @brief IEEE 1722-2016 AVTP Data Unit - Production Implementation
 *
 * Production-ready implementation of IEEE 1722-2016 Audio Video Transport Protocol
 * following the same quality standards as the IEEE 1722.1-2021 implementation.
 *
 * @version 1.0.0
 * @date 2025
 * @copyright OpenAvnu Project
 */

#include "avtpdu.h"
#include <cstring>

namespace IEEE {
namespace _1722 {
namespace _2016 {

// ============================
// AVTPDU Base Class Implementation
// ============================

AVTPDU::AVTPDU()
    : subtype_(Subtype::IEC_61883_IIDC)
    , stream_valid_(false)
    , version_(AVTP_VERSION)
    , media_clock_restart_(false)
    , gateway_valid_(false)
    , timestamp_valid_(false)
    , sequence_num_(0)
    , timestamp_uncertain_(false)
    , stream_id_(0)
    , avtp_timestamp_(0)
{
    payload_.reserve(AVTP_MAX_PAYLOAD_SIZE);
}

AVTPDU::AVTPDU(Subtype subtype, uint64_t stream_id)
    : subtype_(subtype)
    , stream_valid_(stream_id != 0)
    , version_(AVTP_VERSION)
    , media_clock_restart_(false)
    , gateway_valid_(false)
    , timestamp_valid_(false)
    , sequence_num_(0)
    , timestamp_uncertain_(false)
    , stream_id_(stream_id)
    , avtp_timestamp_(0)
{
    payload_.reserve(AVTP_MAX_PAYLOAD_SIZE);
}

size_t AVTPDU::serialize(uint8_t* buffer, size_t buffer_size) const {
    if (!buffer || buffer_size < get_total_size()) {
        return 0;
    }
    
    // Validate packet before serialization
    if (!is_valid()) {
        return 0;
    }
    
    size_t offset = 0;
    
    // Serialize common header
    serialize_common_header(buffer);
    offset += AVTP_COMMON_HEADER_SIZE;
    
    // Serialize subtype-specific data
    size_t subtype_data_size = serialize_subtype_specific_data(buffer + offset, buffer_size - offset);
    offset += subtype_data_size;
    
    // Serialize payload
    if (!payload_.empty() && (offset + payload_.size() <= buffer_size)) {
        std::memcpy(buffer + offset, payload_.data(), payload_.size());
        offset += payload_.size();
    }
    
    return offset;
}

bool AVTPDU::deserialize(const uint8_t* data, size_t length) {
    if (!data || length < AVTP_COMMON_HEADER_SIZE) {
        return false;
    }
    
    size_t offset = 0;
    
    // Deserialize common header
    if (!deserialize_common_header(data)) {
        return false;
    }
    offset += AVTP_COMMON_HEADER_SIZE;
    
    // Deserialize subtype-specific data
    size_t subtype_data_size = get_subtype_specific_data_size();
    if (offset + subtype_data_size > length) {
        return false;
    }
    
    if (subtype_data_size > 0) {
        if (!deserialize_subtype_specific_data(data + offset, subtype_data_size)) {
            return false;
        }
        offset += subtype_data_size;
    }
    
    // Deserialize payload
    if (offset < length) {
        size_t payload_size = length - offset;
        if (payload_size > AVTP_MAX_PAYLOAD_SIZE) {
            return false; // Payload too large
        }
        
        payload_.clear();
        payload_.resize(payload_size);
        std::memcpy(payload_.data(), data + offset, payload_size);
    } else {
        payload_.clear();
    }
    
    return is_valid();
}

void AVTPDU::set_payload(const std::vector<uint8_t>& payload) {
    if (payload.size() <= AVTP_MAX_PAYLOAD_SIZE) {
        payload_ = payload;
    }
}

void AVTPDU::set_payload(const uint8_t* data, size_t length) {
    if (!data || length == 0) {
        payload_.clear();
        return;
    }
    
    if (length <= AVTP_MAX_PAYLOAD_SIZE) {
        payload_.clear();
        payload_.resize(length);
        std::memcpy(payload_.data(), data, length);
    }
}

bool AVTPDU::is_valid() const {
    return validate_common_header() && is_valid_subtype(subtype_);
}

size_t AVTPDU::get_total_size() const {
    return AVTP_COMMON_HEADER_SIZE + get_subtype_specific_data_size() + payload_.size();
}

std::string AVTPDU::get_subtype_string() const {
    return subtype_to_string(subtype_);
}

size_t AVTPDU::serialize_subtype_specific_data(uint8_t* buffer, size_t buffer_size) const {
    // Base class has no subtype-specific data
    (void)buffer;      // Suppress unused parameter warning
    (void)buffer_size; // Suppress unused parameter warning
    return 0;
}

bool AVTPDU::deserialize_subtype_specific_data(const uint8_t* data, size_t length) {
    // Base class has no subtype-specific data
    (void)data;   // Suppress unused parameter warning
    (void)length; // Suppress unused parameter warning
    return true;
}

bool AVTPDU::validate_common_header() const {
    return version_ == AVTP_VERSION && 
           (stream_valid_ ? stream_id_ != 0 : true) &&
           is_valid_subtype(subtype_);
}

void AVTPDU::serialize_common_header(uint8_t* buffer) const {
    size_t offset = 0;
    
    // Byte 0: subtype
    buffer[offset++] = static_cast<uint8_t>(subtype_);
    
    // Byte 1: sv (1 bit) + version (3 bits) + mr (1 bit) + reserved (2 bits) + gv (1 bit) + tv (1 bit)
    buffer[offset] = 0;
    if (stream_valid_) buffer[offset] |= 0x80;           // sv bit
    buffer[offset] |= (version_ & 0x07) << 4;            // version (3 bits)
    if (media_clock_restart_) buffer[offset] |= 0x08;    // mr bit
    if (gateway_valid_) buffer[offset] |= 0x02;          // gv bit  
    if (timestamp_valid_) buffer[offset] |= 0x01;        // tv bit
    offset++;
    
    // Byte 2: sequence_num
    buffer[offset++] = sequence_num_;
    
    // Byte 3: reserved (7 bits) + tu (1 bit)
    buffer[offset] = 0;
    if (timestamp_uncertain_) buffer[offset] |= 0x01;    // tu bit
    offset++;
    
    // Bytes 4-11: stream_id (8 bytes, big endian)
    uint64_t stream_id_be = htobe64(stream_id_);
    std::memcpy(buffer + offset, &stream_id_be, 8);
    offset += 8;
    
    // Bytes 12-15: avtp_timestamp (4 bytes, big endian) 
    uint32_t timestamp_be = htobe32(avtp_timestamp_);
    std::memcpy(buffer + offset, &timestamp_be, 4);
}

bool AVTPDU::deserialize_common_header(const uint8_t* data) {
    size_t offset = 0;
    
    // Byte 0: subtype
    subtype_ = static_cast<Subtype>(data[offset++]);
    
    // Byte 1: sv + version + mr + reserved + gv + tv
    uint8_t byte1 = data[offset++];
    stream_valid_ = (byte1 & 0x80) != 0;
    version_ = (byte1 >> 4) & 0x07;
    media_clock_restart_ = (byte1 & 0x08) != 0;
    gateway_valid_ = (byte1 & 0x02) != 0;
    timestamp_valid_ = (byte1 & 0x01) != 0;
    
    // Byte 2: sequence_num
    sequence_num_ = data[offset++];
    
    // Byte 3: reserved + tu
    uint8_t byte3 = data[offset++];
    timestamp_uncertain_ = (byte3 & 0x01) != 0;
    
    // Bytes 4-11: stream_id (8 bytes, big endian)
    uint64_t stream_id_be;
    std::memcpy(&stream_id_be, data + offset, 8);
    stream_id_ = be64toh(stream_id_be);
    offset += 8;
    
    // Bytes 12-15: avtp_timestamp (4 bytes, big endian)
    uint32_t timestamp_be;
    std::memcpy(&timestamp_be, data + offset, 4);
    avtp_timestamp_ = be32toh(timestamp_be);
    
    return version_ == AVTP_VERSION && is_valid_subtype(subtype_);
}

// ============================
// AudioAVTPDU Implementation
// ============================

AudioAVTPDU::AudioAVTPDU()
    : AVTPDU(Subtype::AAF)
    , format_(AudioFormat::USER_SPECIFIED)
    , channels_(2)
    , bit_depth_(24)
    , sample_rate_(SampleRate::RATE_48000)
    , samples_per_frame_(6)
{
}

AudioAVTPDU::AudioAVTPDU(uint64_t stream_id, AudioFormat format)
    : AVTPDU(Subtype::AAF, stream_id)
    , format_(format)
    , channels_(2)
    , bit_depth_(24)
    , sample_rate_(SampleRate::RATE_48000)
    , samples_per_frame_(6)
{
}

void AudioAVTPDU::configure_audio(AudioFormat format, uint8_t channels, uint8_t bit_depth, SampleRate rate) {
    format_ = format;
    channels_ = (channels > 8) ? 8 : channels;  // Max 8 channels
    bit_depth_ = bit_depth;
    sample_rate_ = rate;
    
    // Calculate samples per frame based on sample rate (assume 8ms frame at 48kHz = 6 samples)
    switch (rate) {
        case SampleRate::RATE_48000:  samples_per_frame_ = 6; break;
        case SampleRate::RATE_96000:  samples_per_frame_ = 12; break;
        case SampleRate::RATE_192000: samples_per_frame_ = 24; break;
        default: samples_per_frame_ = 6; break;
    }
}

size_t AudioAVTPDU::get_audio_data_size() const {
    return channels_ * samples_per_frame_ * (bit_depth_ / 8);
}

bool AudioAVTPDU::is_audio_format_supported(AudioFormat format) const {
    return is_audio_format_valid(format);
}

size_t AudioAVTPDU::serialize_subtype_specific_data(uint8_t* buffer, size_t buffer_size) const {
    if (!buffer || buffer_size < 8) {
        return 0;
    }
    
    size_t offset = 0;
    
    // AAF-specific header (8 bytes)
    buffer[offset++] = static_cast<uint8_t>(format_);           // Format
    buffer[offset++] = channels_;                               // Number of channels
    buffer[offset++] = bit_depth_;                              // Bit depth
    buffer[offset++] = 0;                                       // Reserved
    
    // Sample rate (4 bytes, big endian)
    uint32_t rate_be = htobe32(static_cast<uint32_t>(sample_rate_));
    std::memcpy(buffer + offset, &rate_be, 4);
    offset += 4;
    
    return offset;
}

bool AudioAVTPDU::deserialize_subtype_specific_data(const uint8_t* data, size_t length) {
    if (!data || length < 8) {
        return false;
    }
    
    size_t offset = 0;
    
    // AAF-specific header (8 bytes)
    format_ = static_cast<AudioFormat>(data[offset++]);
    channels_ = data[offset++];
    bit_depth_ = data[offset++];
    offset++; // Skip reserved byte
    
    // Sample rate (4 bytes, big endian)
    uint32_t rate_be;
    std::memcpy(&rate_be, data + offset, 4);
    sample_rate_ = static_cast<SampleRate>(be32toh(rate_be));
    
    return is_audio_format_valid(format_) && channels_ > 0 && channels_ <= 8 && 
           (bit_depth_ == 16 || bit_depth_ == 24 || bit_depth_ == 32) &&
           is_sample_rate_valid(sample_rate_);
}

// ============================
// VideoAVTPDU Implementation  
// ============================

VideoAVTPDU::VideoAVTPDU()
    : AVTPDU(Subtype::CVF)
    , format_(VideoFormat::RFC4175_YCbCr_422_8)
    , width_(1920)
    , height_(1080)
    , frame_rate_(VideoFrameRate::RATE_30_FPS)
    , interlaced_(false)
{
}

VideoAVTPDU::VideoAVTPDU(uint64_t stream_id, VideoFormat format)
    : AVTPDU(Subtype::CVF, stream_id)
    , format_(format)
    , width_(1920)
    , height_(1080) 
    , frame_rate_(VideoFrameRate::RATE_30_FPS)
    , interlaced_(false)
{
}

void VideoAVTPDU::configure_video(VideoFormat format, uint16_t width, uint16_t height, VideoFrameRate rate) {
    format_ = format;
    width_ = width;
    height_ = height;
    frame_rate_ = rate;
}

size_t VideoAVTPDU::get_video_data_size() const {
    // Estimate based on format and resolution (simplified calculation)
    size_t pixels = width_ * height_;
    
    switch (format_) {
        case VideoFormat::RFC4175_YCbCr_422_8:  return pixels * 2;      // 2 bytes per pixel
        case VideoFormat::RFC4175_YCbCr_422_10: return pixels * 5 / 2;  // 2.5 bytes per pixel
        case VideoFormat::RFC4175_YCbCr_444_8:  return pixels * 3;      // 3 bytes per pixel
        case VideoFormat::RFC4175_RGB_8:        return pixels * 3;      // 3 bytes per pixel
        default:                                return pixels * 2;      // Default estimate
    }
}

bool VideoAVTPDU::is_video_format_supported(VideoFormat format) const {
    return is_video_format_valid(format);
}

size_t VideoAVTPDU::serialize_subtype_specific_data(uint8_t* buffer, size_t buffer_size) const {
    if (!buffer || buffer_size < 12) {
        return 0;
    }
    
    size_t offset = 0;
    
    // CVF-specific header (12 bytes)
    uint16_t format_be = htobe16(static_cast<uint16_t>(format_));
    std::memcpy(buffer + offset, &format_be, 2);
    offset += 2;
    
    uint16_t width_be = htobe16(width_);
    std::memcpy(buffer + offset, &width_be, 2);
    offset += 2;
    
    uint16_t height_be = htobe16(height_);
    std::memcpy(buffer + offset, &height_be, 2);
    offset += 2;
    
    uint16_t rate_be = htobe16(static_cast<uint16_t>(frame_rate_));
    std::memcpy(buffer + offset, &rate_be, 2);
    offset += 2;
    
    buffer[offset++] = interlaced_ ? 1 : 0;
    buffer[offset++] = 0; // Reserved
    buffer[offset++] = 0; // Reserved  
    buffer[offset++] = 0; // Reserved
    
    return offset;
}

bool VideoAVTPDU::deserialize_subtype_specific_data(const uint8_t* data, size_t length) {
    if (!data || length < 12) {
        return false;
    }
    
    size_t offset = 0;
    
    // CVF-specific header (12 bytes)
    uint16_t format_be;
    std::memcpy(&format_be, data + offset, 2);
    format_ = static_cast<VideoFormat>(be16toh(format_be));
    offset += 2;
    
    uint16_t width_be;
    std::memcpy(&width_be, data + offset, 2);
    width_ = be16toh(width_be);
    offset += 2;
    
    uint16_t height_be;
    std::memcpy(&height_be, data + offset, 2);
    height_ = be16toh(height_be);
    offset += 2;
    
    uint16_t rate_be;
    std::memcpy(&rate_be, data + offset, 2);
    frame_rate_ = static_cast<VideoFrameRate>(be16toh(rate_be));
    offset += 2;
    
    interlaced_ = data[offset] != 0;
    // Skip reserved bytes
    
    return is_video_format_valid(format_) && width_ > 0 && height_ > 0;
}

// ============================
// CRFAVTPDU Implementation
// ============================

CRFAVTPDU::CRFAVTPDU()
    : AVTPDU(Subtype::CRF)
    , crf_type_(CRFType::AUDIO_SAMPLE)
    , crf_data_(0)
    , crf_data_length_(4)
{
}

CRFAVTPDU::CRFAVTPDU(uint64_t stream_id, CRFType crf_type)
    : AVTPDU(Subtype::CRF, stream_id)
    , crf_type_(crf_type)
    , crf_data_(0)
    , crf_data_length_(4)
{
}

void CRFAVTPDU::configure_audio_crf(uint32_t sample_timestamp) {
    crf_type_ = CRFType::AUDIO_SAMPLE;
    crf_data_ = sample_timestamp;
    crf_data_length_ = 4;
}

void CRFAVTPDU::configure_video_crf(uint32_t frame_timestamp) {
    crf_type_ = CRFType::VIDEO_FRAME;
    crf_data_ = frame_timestamp;
    crf_data_length_ = 4;
}

bool CRFAVTPDU::is_crf_type_supported(CRFType type) const {
    return type == CRFType::AUDIO_SAMPLE || 
           type == CRFType::VIDEO_FRAME || 
           type == CRFType::MACHINE_CYCLE;
}

size_t CRFAVTPDU::serialize_subtype_specific_data(uint8_t* buffer, size_t buffer_size) const {
    if (!buffer || buffer_size < 8) {
        return 0;
    }
    
    size_t offset = 0;
    
    // CRF-specific header (8 bytes)
    buffer[offset++] = static_cast<uint8_t>(crf_type_);
    buffer[offset++] = 0; // Reserved
    
    uint16_t length_be = htobe16(crf_data_length_);
    std::memcpy(buffer + offset, &length_be, 2);
    offset += 2;
    
    uint32_t data_be = htobe32(crf_data_);
    std::memcpy(buffer + offset, &data_be, 4);
    offset += 4;
    
    return offset;
}

bool CRFAVTPDU::deserialize_subtype_specific_data(const uint8_t* data, size_t length) {
    if (!data || length < 8) {
        return false;
    }
    
    size_t offset = 0;
    
    // CRF-specific header (8 bytes)
    crf_type_ = static_cast<CRFType>(data[offset++]);
    offset++; // Skip reserved
    
    uint16_t length_be;
    std::memcpy(&length_be, data + offset, 2);
    crf_data_length_ = be16toh(length_be);
    offset += 2;
    
    uint32_t data_be;
    std::memcpy(&data_be, data + offset, 4);
    crf_data_ = be32toh(data_be);
    
    return is_crf_type_supported(crf_type_) && crf_data_length_ > 0;
}

// ============================
// Utility Functions Implementation
// ============================

bool is_valid_subtype(Subtype subtype) {
    switch (subtype) {
        case Subtype::IEC_61883_IIDC:
        case Subtype::MMA_STREAM:
        case Subtype::AAF:
        case Subtype::CVF:
        case Subtype::CRF:
        case Subtype::TSCF:
        case Subtype::SVF:
        case Subtype::RVF:
        case Subtype::AEF_CONTINUOUS:
        case Subtype::VSF_STREAM:
        case Subtype::EF_STREAM:
        case Subtype::NTSCF:
        case Subtype::ESCF:
        case Subtype::ADP:
        case Subtype::AECP:
        case Subtype::ACMP:
        case Subtype::MAAP:
        case Subtype::EF_CONTROL:
            return true;
        default:
            return false;
    }
}

std::string subtype_to_string(Subtype subtype) {
    switch (subtype) {
        case Subtype::IEC_61883_IIDC:    return "IEC_61883_IIDC";
        case Subtype::MMA_STREAM:        return "MMA_STREAM";
        case Subtype::AAF:               return "AAF";
        case Subtype::CVF:               return "CVF";
        case Subtype::CRF:               return "CRF";
        case Subtype::TSCF:              return "TSCF";
        case Subtype::SVF:               return "SVF";
        case Subtype::RVF:               return "RVF";
        case Subtype::AEF_CONTINUOUS:    return "AEF_CONTINUOUS";
        case Subtype::VSF_STREAM:        return "VSF_STREAM";
        case Subtype::EF_STREAM:         return "EF_STREAM";
        case Subtype::NTSCF:             return "NTSCF";
        case Subtype::ESCF:              return "ESCF";
        case Subtype::ADP:               return "ADP";
        case Subtype::AECP:              return "AECP";
        case Subtype::ACMP:              return "ACMP";
        case Subtype::MAAP:              return "MAAP";
        case Subtype::EF_CONTROL:        return "EF_CONTROL";
        default:                         return "UNKNOWN";
    }
}

Subtype string_to_subtype(const std::string& str) {
    if (str == "IEC_61883_IIDC") return Subtype::IEC_61883_IIDC;
    if (str == "MMA_STREAM") return Subtype::MMA_STREAM;
    if (str == "AAF") return Subtype::AAF;
    if (str == "CVF") return Subtype::CVF;
    if (str == "CRF") return Subtype::CRF;
    if (str == "TSCF") return Subtype::TSCF;
    if (str == "SVF") return Subtype::SVF;
    if (str == "RVF") return Subtype::RVF;
    if (str == "AEF_CONTINUOUS") return Subtype::AEF_CONTINUOUS;
    if (str == "VSF_STREAM") return Subtype::VSF_STREAM;
    if (str == "EF_STREAM") return Subtype::EF_STREAM;
    if (str == "NTSCF") return Subtype::NTSCF;
    if (str == "ESCF") return Subtype::ESCF;
    if (str == "ADP") return Subtype::ADP;
    if (str == "AECP") return Subtype::AECP;
    if (str == "ACMP") return Subtype::ACMP;
    if (str == "MAAP") return Subtype::MAAP;
    if (str == "EF_CONTROL") return Subtype::EF_CONTROL;
    return Subtype::IEC_61883_IIDC; // Default
}

bool is_audio_format_valid(AudioFormat format) {
    switch (format) {
        case AudioFormat::USER_SPECIFIED:
        case AudioFormat::IEC_61883_6:
        case AudioFormat::SAF:
        case AudioFormat::IEEE_FLOAT32:
        case AudioFormat::IEEE_FLOAT64:
            return true;
        default:
            return false;
    }
}

bool is_sample_rate_valid(SampleRate rate) {
    switch (rate) {
        case SampleRate::RATE_8000:
        case SampleRate::RATE_16000:
        case SampleRate::RATE_32000:
        case SampleRate::RATE_44100:
        case SampleRate::RATE_48000:
        case SampleRate::RATE_88200:
        case SampleRate::RATE_96000:
        case SampleRate::RATE_176400:
        case SampleRate::RATE_192000:
            return true;
        default:
            return false;
    }
}

bool is_video_format_valid(VideoFormat format) {
    switch (format) {
        case VideoFormat::RFC4175_YCbCr_422_8:
        case VideoFormat::RFC4175_YCbCr_422_10:
        case VideoFormat::RFC4175_YCbCr_444_8:
        case VideoFormat::RFC4175_YCbCr_444_10:
        case VideoFormat::RFC4175_RGB_8:
        case VideoFormat::RFC4175_RGB_10:
        case VideoFormat::H264:
        case VideoFormat::MJPEG:
            return true;
        default:
            return false;
    }
}

bool is_valid_avtp_packet(const uint8_t* data, size_t length) {
    if (!data || length < AVTP_COMMON_HEADER_SIZE) {
        return false;
    }
    
    // Check version field
    uint8_t version = (data[1] >> 4) & 0x07;
    if (version != AVTP_VERSION) {
        return false;
    }
    
    // Check subtype
    Subtype subtype = static_cast<Subtype>(data[0]);
    return is_valid_subtype(subtype);
}

size_t get_avtp_header_size(Subtype subtype) {
    switch (subtype) {
        case Subtype::AAF:  return AVTP_COMMON_HEADER_SIZE + 8;   // Audio format specific
        case Subtype::CVF:  return AVTP_COMMON_HEADER_SIZE + 12;  // Video format specific
        case Subtype::CRF:  return AVTP_COMMON_HEADER_SIZE + 8;   // CRF format specific
        default:            return AVTP_COMMON_HEADER_SIZE;        // Common header only
    }
}

size_t get_max_payload_size(Subtype subtype) {
    return AVTP_MAX_PAYLOAD_SIZE - (get_avtp_header_size(subtype) - AVTP_COMMON_HEADER_SIZE);
}

// Factory functions
std::unique_ptr<AVTPDU> create_avtpdu(Subtype subtype, uint64_t stream_id) {
    switch (subtype) {
        case Subtype::AAF:
            return std::make_unique<AudioAVTPDU>(stream_id);
        case Subtype::CVF:
            return std::make_unique<VideoAVTPDU>(stream_id);
        case Subtype::CRF:
            return std::make_unique<CRFAVTPDU>(stream_id);
        default:
            return std::make_unique<AVTPDU>(subtype, stream_id);
    }
}

std::unique_ptr<AudioAVTPDU> create_audio_avtpdu(uint64_t stream_id, AudioFormat format) {
    auto avtpdu = std::make_unique<AudioAVTPDU>(stream_id, format);
    return avtpdu;
}

std::unique_ptr<VideoAVTPDU> create_video_avtpdu(uint64_t stream_id, VideoFormat format) {
    auto avtpdu = std::make_unique<VideoAVTPDU>(stream_id, format);
    return avtpdu;
}

std::unique_ptr<CRFAVTPDU> create_crf_avtpdu(uint64_t stream_id, CRFAVTPDU::CRFType type) {
    auto avtpdu = std::make_unique<CRFAVTPDU>(stream_id, type);
    return avtpdu;
}

// ============================
// Milan Alliance Extensions
// ============================

namespace Milan {

bool is_milan_compatible_audio_format(AudioFormat format) {
    // Milan primarily supports high-quality PCM audio
    return format == AudioFormat::SAF || format == AudioFormat::IEC_61883_6;
}

bool is_milan_compatible_sample_rate(SampleRate rate) {
    // Milan supports standard professional audio sample rates
    return rate == MILAN_SAMPLE_RATE_48K || rate == MILAN_SAMPLE_RATE_96K ||
           rate == SampleRate::RATE_44100 || rate == SampleRate::RATE_192000;
}

void configure_milan_audio_stream(AudioAVTPDU& avtpdu, uint8_t channels) {
    // Configure for Milan-compliant audio streaming
    avtpdu.configure_audio(AudioFormat::SAF, channels, 24, MILAN_SAMPLE_RATE_48K);
    avtpdu.set_timestamp_valid(true);
    avtpdu.set_stream_valid(true);
}

} // namespace Milan

} // namespace _2016
} // namespace _1722
} // namespace IEEE
