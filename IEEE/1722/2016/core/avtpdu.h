/**
 * @file avtpdu.h
 * @brief IEEE 1722-2016 AVTP Data Unit - Production Implementation
 * 
 * Clean, production-ready implementation of IEEE 1722-2016 Audio Video Transport Protocol
 * Data Units. This implementation follows the same architectural patterns as the successful
 * IEEE 1722.1-2021 implementation in this project.
 * 
 * Key Design Principles:
 * - Unified IEEE::_1722::_2016 namespace (consistent with 1722.1-2021)
 * - Cross-platform compatibility (Windows/Linux/macOS)
 * - Comprehensive error handling and validation
 * - Hardware timestamping integration ready
 * - Milan Alliance compatibility
 * 
 * @version 1.0.0
 * @date 2025
 * @copyright OpenAvnu Project
 */

#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <memory>

// Platform abstraction for endianness
#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
    #define htobe16(x) htons(x)
    #define htobe32(x) htonl(x)
    #define htobe64(x) _byteswap_uint64(htonll(x))
    #define be16toh(x) ntohs(x)  
    #define be32toh(x) ntohl(x)
    #define be64toh(x) ntohll(_byteswap_uint64(x))
    
    // Windows lacks htonll/ntohll in some versions
    #ifndef htonll
    #define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
    #define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))
    #endif
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #ifdef __APPLE__
        #include <libkern/OSByteOrder.h>
        #define htobe16(x) OSSwapHostToBigInt16(x)
        #define htobe32(x) OSSwapHostToBigInt32(x) 
        #define htobe64(x) OSSwapHostToBigInt64(x)
        #define be16toh(x) OSSwapBigToHostInt16(x)
        #define be32toh(x) OSSwapBigToHostInt32(x)
        #define be64toh(x) OSSwapBigToHostInt64(x)
    #else
        #include <endian.h>
    #endif
#endif

namespace IEEE {
namespace _1722 {
namespace _2016 {

// ============================
// AVTP Protocol Constants  
// ============================

constexpr uint8_t AVTP_VERSION = 0x00;            // IEEE 1722-2016 version
constexpr size_t AVTP_COMMON_HEADER_SIZE = 16;    // Common AVTP header size (includes timestamp)
constexpr size_t AVTP_MAX_PAYLOAD_SIZE = 1472;    // Maximum payload size (1500 - 28 for IP/UDP)
constexpr size_t AVTP_MAX_PACKET_SIZE = 1500;     // Maximum AVTP packet size

// ============================
// AVTP Subtypes (IEEE 1722-2016 Table 5-1)
// ============================

enum class Subtype : uint8_t {
    // Media Subtypes
    IEC_61883_IIDC    = 0x00,    // IEC 61883/IIDC formats
    MMA_STREAM        = 0x01,    // MIDI Media Adaptation streams  
    AAF               = 0x02,    // AVTP Audio Format
    CVF               = 0x03,    // Compressed Video Format
    CRF               = 0x04,    // Clock Reference Format
    TSCF              = 0x05,    // Time-Synchronous Control Format
    SVF               = 0x06,    // SDI Video Format
    RVF               = 0x07,    // Raw Video Format
    
    // Experimental/Vendor Subtypes
    AEF_CONTINUOUS    = 0x6E,    // AES Encrypted Format (continuous)
    VSF_STREAM        = 0x6F,    // Vendor Specific Format (stream)
    EF_STREAM         = 0x7E,    // Experimental Format (stream)  
    
    // Control Subtypes (from IEEE 1722.1)
    NTSCF             = 0x82,    // Non-Time-Synchronous Control Format
    ESCF              = 0x83,    // Embedded SMPTE Control Format
    ADP               = 0xFA,    // AVDECC Discovery Protocol
    AECP              = 0xFB,    // AVDECC Enumeration and Control Protocol
    ACMP              = 0xFC,    // AVDECC Connection Management Protocol
    
    // Vendor/Experimental  
    MAAP              = 0xFE,    // Multicast Address Allocation Protocol
    EF_CONTROL        = 0xFF     // Experimental Format (control)
};

// ============================
// Audio Format Definitions
// ============================

enum class AudioFormat : uint16_t {
    USER_SPECIFIED    = 0x00,    // User specified format
    IEC_61883_6       = 0x01,    // IEC 61883-6 standard
    SAF               = 0x02,    // Simple Audio Format
    IEEE_FLOAT32      = 0x03,    // IEEE 32-bit floating point
    IEEE_FLOAT64      = 0x04,    // IEEE 64-bit floating point
};

enum class SampleRate : uint32_t {
    RATE_8000   = 8000,      // 8 kHz
    RATE_16000  = 16000,     // 16 kHz  
    RATE_32000  = 32000,     // 32 kHz
    RATE_44100  = 44100,     // 44.1 kHz
    RATE_48000  = 48000,     // 48 kHz (most common)
    RATE_88200  = 88200,     // 88.2 kHz
    RATE_96000  = 96000,     // 96 kHz
    RATE_176400 = 176400,    // 176.4 kHz
    RATE_192000 = 192000     // 192 kHz (professional)
};

// ============================
// Video Format Definitions  
// ============================

enum class VideoFormat : uint16_t {
    RFC4175_YCbCr_422_8   = 0x00,    // RFC 4175 YCbCr-4:2:2 8-bit
    RFC4175_YCbCr_422_10  = 0x01,    // RFC 4175 YCbCr-4:2:2 10-bit
    RFC4175_YCbCr_444_8   = 0x02,    // RFC 4175 YCbCr-4:4:4 8-bit
    RFC4175_YCbCr_444_10  = 0x03,    // RFC 4175 YCbCr-4:4:4 10-bit
    RFC4175_RGB_8         = 0x04,    // RFC 4175 RGB 8-bit
    RFC4175_RGB_10        = 0x05,    // RFC 4175 RGB 10-bit
    H264                  = 0x06,    // H.264/AVC
    MJPEG                 = 0x07     // Motion JPEG
};

enum class VideoFrameRate : uint16_t {
    RATE_24_FPS     = 24,        // 24 fps (film)
    RATE_25_FPS     = 25,        // 25 fps (PAL)
    RATE_30_FPS     = 30,        // 30 fps (NTSC) 
    RATE_50_FPS     = 50,        // 50 fps (PAL progressive)
    RATE_60_FPS     = 60,        // 60 fps (NTSC progressive)
    RATE_120_FPS    = 120        // 120 fps (high frame rate)
};

// ============================
// AVTP Data Unit Base Class
// ============================

class AVTPDU {
public:
    // Construction/Destruction
    AVTPDU();
    AVTPDU(Subtype subtype, uint64_t stream_id = 0);
    virtual ~AVTPDU() = default;
    
    // Copy/Move semantics
    AVTPDU(const AVTPDU& other) = default;
    AVTPDU& operator=(const AVTPDU& other) = default;
    AVTPDU(AVTPDU&& other) noexcept = default;
    AVTPDU& operator=(AVTPDU&& other) noexcept = default;
    
    // Serialization/Deserialization
    virtual size_t serialize(uint8_t* buffer, size_t buffer_size) const;
    virtual bool deserialize(const uint8_t* data, size_t length);
    
    // Header field accessors
    Subtype get_subtype() const { return subtype_; }
    void set_subtype(Subtype subtype) { subtype_ = subtype; }
    
    bool get_stream_valid() const { return stream_valid_; }
    void set_stream_valid(bool valid) { stream_valid_ = valid; }
    
    uint8_t get_version() const { return version_; }
    
    bool get_media_clock_restart() const { return media_clock_restart_; }
    void set_media_clock_restart(bool restart) { media_clock_restart_ = restart; }
    
    bool get_gateway_valid() const { return gateway_valid_; }
    void set_gateway_valid(bool valid) { gateway_valid_ = valid; }
    
    bool get_timestamp_valid() const { return timestamp_valid_; }
    void set_timestamp_valid(bool valid) { timestamp_valid_ = valid; }
    
    uint8_t get_sequence_num() const { return sequence_num_; }
    void set_sequence_num(uint8_t seq) { sequence_num_ = seq; }
    
    bool get_timestamp_uncertain() const { return timestamp_uncertain_; }
    void set_timestamp_uncertain(bool uncertain) { timestamp_uncertain_ = uncertain; }
    
    uint64_t get_stream_id() const { return stream_id_; }
    void set_stream_id(uint64_t id) { stream_id_ = id; }
    
    uint32_t get_avtp_timestamp() const { return avtp_timestamp_; }
    void set_avtp_timestamp(uint32_t timestamp) { avtp_timestamp_ = timestamp; }
    
    // Payload management  
    const std::vector<uint8_t>& get_payload() const { return payload_; }
    void set_payload(const std::vector<uint8_t>& payload);
    void set_payload(const uint8_t* data, size_t length);
    size_t get_payload_size() const { return payload_.size(); }
    
    // Utility methods
    virtual bool is_valid() const;
    virtual size_t get_total_size() const;
    std::string get_subtype_string() const;
    
protected:
    // Subclass serialization hooks
    virtual size_t serialize_subtype_specific_data(uint8_t* buffer, size_t buffer_size) const;
    virtual bool deserialize_subtype_specific_data(const uint8_t* data, size_t length);
    virtual size_t get_subtype_specific_data_size() const { return 0; }
    
private:
    // AVTP Common Header Fields (IEEE 1722-2016 Section 5.3)
    Subtype subtype_;                    // AVTP subtype
    bool stream_valid_;                  // Stream ID valid flag (sv)
    uint8_t version_;                    // AVTP version (0x00 for 2016)
    bool media_clock_restart_;           // Media clock restart (mr)
    bool gateway_valid_;                 // Gateway valid flag (gv)  
    bool timestamp_valid_;               // Timestamp valid flag (tv)
    uint8_t sequence_num_;               // AVTP sequence number
    bool timestamp_uncertain_;           // Timestamp uncertain flag (tu)
    uint64_t stream_id_;                 // AVTP stream identifier (8 bytes)
    uint32_t avtp_timestamp_;            // AVTP timestamp (4 bytes)
    
    // Payload data
    std::vector<uint8_t> payload_;       // AVTP payload data
    
    // Validation helpers
    bool validate_common_header() const;
    void serialize_common_header(uint8_t* buffer) const;
    bool deserialize_common_header(const uint8_t* data);
};

// ============================
// Audio AVTP Data Unit
// ============================

class AudioAVTPDU : public AVTPDU {
public:
    AudioAVTPDU();
    AudioAVTPDU(uint64_t stream_id, AudioFormat format = AudioFormat::USER_SPECIFIED);
    
    // Audio-specific fields
    AudioFormat get_format() const { return format_; }
    void set_format(AudioFormat format) { format_ = format; }
    
    uint8_t get_channels() const { return channels_; }
    void set_channels(uint8_t channels) { channels_ = channels; }
    
    uint8_t get_bit_depth() const { return bit_depth_; }
    void set_bit_depth(uint8_t depth) { bit_depth_ = depth; }
    
    SampleRate get_sample_rate() const { return sample_rate_; }
    void set_sample_rate(SampleRate rate) { sample_rate_ = rate; }
    
    uint16_t get_samples_per_frame() const { return samples_per_frame_; }
    void set_samples_per_frame(uint16_t samples) { samples_per_frame_ = samples; }
    
    // Convenience methods
    void configure_audio(AudioFormat format, uint8_t channels, uint8_t bit_depth, SampleRate rate);
    size_t get_audio_data_size() const;
    bool is_audio_format_supported(AudioFormat format) const;
    
protected:
    size_t serialize_subtype_specific_data(uint8_t* buffer, size_t buffer_size) const override;
    bool deserialize_subtype_specific_data(const uint8_t* data, size_t length) override;
    size_t get_subtype_specific_data_size() const override { return 8; } // Audio-specific header
    
private:
    AudioFormat format_;          // Audio format type
    uint8_t channels_;           // Number of audio channels
    uint8_t bit_depth_;          // Bit depth per sample
    SampleRate sample_rate_;     // Sample rate in Hz
    uint16_t samples_per_frame_; // Number of samples per AVTP frame
};

// ============================
// Video AVTP Data Unit  
// ============================

class VideoAVTPDU : public AVTPDU {
public:
    VideoAVTPDU();
    VideoAVTPDU(uint64_t stream_id, VideoFormat format = VideoFormat::RFC4175_YCbCr_422_8);
    
    // Video-specific fields
    VideoFormat get_format() const { return format_; }
    void set_format(VideoFormat format) { format_ = format; }
    
    uint16_t get_width() const { return width_; }
    void set_width(uint16_t width) { width_ = width; }
    
    uint16_t get_height() const { return height_; }
    void set_height(uint16_t height) { height_ = height; }
    
    VideoFrameRate get_frame_rate() const { return frame_rate_; }
    void set_frame_rate(VideoFrameRate rate) { frame_rate_ = rate; }
    
    bool get_interlaced() const { return interlaced_; }
    void set_interlaced(bool interlaced) { interlaced_ = interlaced; }
    
    // Convenience methods
    void configure_video(VideoFormat format, uint16_t width, uint16_t height, VideoFrameRate rate);
    size_t get_video_data_size() const;
    bool is_video_format_supported(VideoFormat format) const;
    
protected:
    size_t serialize_subtype_specific_data(uint8_t* buffer, size_t buffer_size) const override;
    bool deserialize_subtype_specific_data(const uint8_t* data, size_t length) override;
    size_t get_subtype_specific_data_size() const override { return 12; } // Video-specific header
    
private:
    VideoFormat format_;         // Video format type
    uint16_t width_;            // Video width in pixels
    uint16_t height_;           // Video height in pixels  
    VideoFrameRate frame_rate_; // Video frame rate
    bool interlaced_;           // Interlaced vs progressive
};

// ============================
// Clock Reference Format AVTP Data Unit
// ============================

class CRFAVTPDU : public AVTPDU {
public:
    enum class CRFType : uint8_t {
        AUDIO_SAMPLE    = 0x00,    // Audio sample clock reference
        VIDEO_FRAME     = 0x01,    // Video frame clock reference
        MACHINE_CYCLE   = 0x02     // Machine cycle clock reference
    };
    
    CRFAVTPDU();
    CRFAVTPDU(uint64_t stream_id, CRFType crf_type = CRFType::AUDIO_SAMPLE);
    
    // CRF-specific fields
    CRFType get_crf_type() const { return crf_type_; }
    void set_crf_type(CRFType type) { crf_type_ = type; }
    
    uint32_t get_crf_data() const { return crf_data_; }
    void set_crf_data(uint32_t data) { crf_data_ = data; }
    
    uint16_t get_crf_data_length() const { return crf_data_length_; }
    void set_crf_data_length(uint16_t length) { crf_data_length_ = length; }
    
    // Convenience methods
    void configure_audio_crf(uint32_t sample_timestamp);
    void configure_video_crf(uint32_t frame_timestamp);
    bool is_crf_type_supported(CRFType type) const;
    
protected:
    size_t serialize_subtype_specific_data(uint8_t* buffer, size_t buffer_size) const override;
    bool deserialize_subtype_specific_data(const uint8_t* data, size_t length) override; 
    size_t get_subtype_specific_data_size() const override { return 8; } // CRF-specific header
    
private:
    CRFType crf_type_;           // Clock reference type
    uint32_t crf_data_;          // Clock reference data
    uint16_t crf_data_length_;   // Length of CRF data
};

// ============================
// Utility Functions
// ============================

// Subtype validation and string conversion
bool is_valid_subtype(Subtype subtype);
std::string subtype_to_string(Subtype subtype);
Subtype string_to_subtype(const std::string& str);

// Format validation
bool is_audio_format_valid(AudioFormat format);
bool is_sample_rate_valid(SampleRate rate);
bool is_video_format_valid(VideoFormat format);

// AVTP packet validation  
bool is_valid_avtp_packet(const uint8_t* data, size_t length);
size_t get_avtp_header_size(Subtype subtype);
size_t get_max_payload_size(Subtype subtype);

// Factory functions for creating AVTP packets
std::unique_ptr<AVTPDU> create_avtpdu(Subtype subtype, uint64_t stream_id = 0);
std::unique_ptr<AudioAVTPDU> create_audio_avtpdu(uint64_t stream_id, AudioFormat format);
std::unique_ptr<VideoAVTPDU> create_video_avtpdu(uint64_t stream_id, VideoFormat format);
std::unique_ptr<CRFAVTPDU> create_crf_avtpdu(uint64_t stream_id, CRFAVTPDU::CRFType type);

// Milan Alliance Extensions (professional audio)
namespace Milan {
    // Milan-specific audio formats and sample rates
    constexpr SampleRate MILAN_SAMPLE_RATE_48K = SampleRate::RATE_48000;
    constexpr SampleRate MILAN_SAMPLE_RATE_96K = SampleRate::RATE_96000;
    
    // Milan clock domain validation
    bool is_milan_compatible_audio_format(AudioFormat format);
    bool is_milan_compatible_sample_rate(SampleRate rate);
    
    // Milan stream configuration helpers
    void configure_milan_audio_stream(AudioAVTPDU& avtpdu, uint8_t channels);
} // namespace Milan

} // namespace _2016
} // namespace _1722  
} // namespace IEEE
