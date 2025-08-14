/**
 * @file 1722-2016.h
 * @brief IEEE 1722-2016 AVTP core definitions
 *
 * REVIEW: The brief/comment block was corrupted ("Heade// ... VTPDU"). Cleaned up for clarity.
 *         Ensure documentation follows IEEE 1722-2016 definitions precisely.
 */

#pragma once
#include <cstdint>
#include <array>
#include <string>

namespace IEEE {
namespace _1722 {
namespace _2016 {

// =============================
// AVTP Protocol Constants
// =============================
constexpr uint8_t AVTP_VERSION_2016 = 0x00;
constexpr size_t AVTPDU_MAX_SIZE = 1500;

// =============================
// AVTP Subtypes (per Table 5-1)
// =============================
enum class Subtype : uint8_t {
    IEC61883_IIDC = 0x00,
    IEC61883_6    = 0x02,
    MIDI          = 0x03,
    SMPTE         = 0x04,
    AVTP_AUDIO    = 0x22,
    AVTP_VIDEO    = 0x23,
    AVTP_CONTROL  = 0x24,
    AVTP_ANCILLARY= 0x25,
    AVTP_GENERIC  = 0x7E,
    EXPERIMENTAL  = 0x7F
};
struct AVTPDU {
    // AVTP Common Header (per 2016 spec)
    uint8_t subtype;              // AVTP subtype (audio, video, etc.)
    // REVIEW [IEEE 1722-2016, Clause 5 AVTPDU Common Header]: Flags (sv, mr, gv, tv, tu) are bitfields within specific octets.
    // Using separate bool fields loses on-wire layout and endianness. Represent as packed/bitfield or explicit byte layout.
    bool stream_valid;            // Stream ID valid flag (sv)
    uint8_t version;              // AVTP version (0x00 for 2016)
    bool mr;                      // Media clock restart
    bool gv;                      // Gateway valid
    bool tv;                      // Timestamp valid
    uint8_t sequence_num;         // Sequence number
    bool tu;                      // Timestamp uncertain
    uint8_t stream_id[8];         // Stream ID (8 bytes)
    uint32_t avtp_timestamp;      // AVTP timestamp
    uint16_t stream_data_length;  // Stream data length
    uint16_t format_specific_data; // Format-specific data (audio/video)
    std::array<uint8_t, 1500> payload; // Payload data
    // REVIEW [IEEE 1722-2016, Clause 5]: Max Ethernet payload is 1500 bytes including AVTPDU, not just data payload.
    // Consider limiting payload based on MTU minus header length, and document VLAN/SNAP cases.
    
    // Constructors
    AVTPDU();
    AVTPDU(const uint8_t* data, size_t length);
    
    // Serialization methods
    void serialize(uint8_t* buffer, size_t& length) const;
    bool deserialize(const uint8_t* data, size_t length);
    // REVIEW [IEEE 1722-2016, Clause 5]: Serialization must apply network byte order (big-endian) for multi-byte fields.
    // Ensure hton/ntoh conversions are applied for timestamp, lengths, format fields.
    
    // Utility methods
    static constexpr size_t get_header_size() { return 20; } // AVTP header size
    // REVIEW [IEEE 1722-2016, Clause 5]: Header size varies by subtype/format; 20 bytes constant may be incorrect for all cases.
    // Validate per-subtype header layout (e.g., IEC61883-6, AAF, CVF).
    bool is_valid() const { return version == AVTP_VERSION_2016; }
    // REVIEW: Validation should also check subtype-specific fixed bits, sv flag consistency, and stream_data_length bounds.
};

// =============================
// AVTP Stream Format (Audio/Video)
// =============================
enum class AudioFormat : uint16_t {
    IEC_61883_6 = 0x00,
    MILAN_PCM   = 0x01,
    AES67       = 0x02,
    SMPTE_ST2110_30 = 0x03
};
// REVIEW [Milan DCC 1.1a]: MILAN PCM imposes strict constraints (channel count, sample rate set, packing, timestamp rules).
// Ensure corresponding AVTP format fields and payload layout meet Milan Profile requirements.

enum class VideoFormat : uint16_t {
    IEC_61883_4 = 0x00,
    SMPTE_ST2110_20 = 0x01,
    H264        = 0x02,
    JPEG2000    = 0x03
};

enum class SampleRate : uint8_t {
    RATE_8KHZ   = 0x00,
    RATE_16KHZ  = 0x01,
    RATE_32KHZ  = 0x02,
    RATE_44_1KHZ = 0x03,
    RATE_48KHZ  = 0x04,
    RATE_88_2KHZ = 0x05,
    RATE_96KHZ  = 0x06,
    RATE_176_4KHZ = 0x07,
    RATE_192KHZ = 0x08
};

enum class FrameRate : uint8_t {
    RATE_24FPS  = 0x00,
    RATE_25FPS  = 0x01,
    RATE_30FPS  = 0x02,
    RATE_50FPS  = 0x03,
    RATE_60FPS  = 0x04
};

// =============================
// Audio-specific AVTPDU
// =============================
struct AudioAVTPDU : public AVTPDU {
    AudioFormat format;
    SampleRate nominal_sample_rate;
    uint8_t channels;
    uint8_t bit_depth;
    uint16_t samples_per_frame;
    
    AudioAVTPDU();
    void set_audio_format(AudioFormat fmt, uint8_t ch, uint8_t depth);
};

// =============================
// Video-specific AVTPDU
// =============================
struct VideoAVTPDU : public AVTPDU {
    VideoFormat format;
    FrameRate frame_rate;
    uint16_t width;
    uint16_t height;
    
    VideoAVTPDU();
    void set_video_format(VideoFormat fmt, uint16_t w, uint16_t h, FrameRate rate);
};

// =============================
// Utility Functions
// =============================
bool is_valid_avtp_packet(const uint8_t* data, size_t length);
size_t get_avtp_payload_offset(Subtype subtype);
std::string subtype_to_string(Subtype subtype);

// =============================
// Utility Functions (Header Only)
// =============================
inline bool is_valid_subtype(uint8_t subtype) {
    switch (static_cast<Subtype>(subtype)) {
        case Subtype::IEC61883_IIDC:
        case Subtype::IEC61883_6:
        case Subtype::MIDI:
        case Subtype::SMPTE:
        case Subtype::AVTP_AUDIO:
        case Subtype::AVTP_VIDEO:
        case Subtype::AVTP_CONTROL:
        case Subtype::AVTP_ANCILLARY:
        case Subtype::AVTP_GENERIC:
        case Subtype::EXPERIMENTAL:
            return true;
        default:
            return false;
    }
}

} // namespace _2016
} // namespace _1722
} // namespace IEEE