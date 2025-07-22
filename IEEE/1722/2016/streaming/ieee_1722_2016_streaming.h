/**
 * @file ieee_1722_2016_streaming.h
 * @brief IEEE 1722-2016 Standard Headers
 * 
 * Complete AVTPDU structure, stream formats, and protocol constants for IEEE 1722-2016.
 */

#ifndef IEEE_1722_2016_STREAMING_H
#define IEEE_1722_2016_STREAMING_H

#include <cstdint>
#include <array>
#include <string>

namespace avtp_protocol {
namespace ieee_1722_2016 {

// =============================
// AVTP Protocol Constants
// =============================
constexpr uint8_t AVTP_VERSION_2016 = 0x00;
constexpr size_t AVTPDU_MAX_SIZE = 1500;

// =============================
// AVTP Subtypes (per IEEE 1722-2016 Table 6)
// =============================
enum class Subtype : uint8_t {
    // Official IEEE 1722-2016 Subtype Values (Table 6)
    IEC61883_IIDC = 0x00,       // IEC 61883/IIDC format
    MMA_STREAM = 0x01,          // MMA streams (MIDI)
    AAF = 0x02,                 // AVTP Audio Format
    CVF = 0x03,                 // Compressed Video Format
    CRF = 0x04,                 // Clock Reference Format
    TSCF = 0x05,                // Time-Synchronous Control Format
    SVF = 0x06,                 // SDI Video Format
    RVF = 0x07,                 // Raw Video Format
    // 0x08 to 0x6D Reserved
    AEF_CONTINUOUS = 0x6E,      // AES Encrypted Format Continuous
    VSF_STREAM = 0x6F,          // Vendor Specific Format Stream
    // 0x70 to 0x7E Reserved
    EF_STREAM = 0x7F,           // Experimental Format Stream
    // 0x80 to 0x81 Reserved
    NTSCF = 0x82,               // Non-Time-Synchronous Control Format
    // 0x83 to 0xEB Reserved
    ESCF = 0xEC,                // ECC Signed Control Format
    EECF = 0xED,                // ECC Encrypted Control Format
    AEF_DISCRETE = 0xEE,        // AES Encrypted Format Discrete
    // 0xEF to 0xF9 Reserved
    ADP = 0xFA,                 // AVDECC Discovery Protocol
    AECP = 0xFB,                // AVDECC Enumeration Control Protocol
    ACMP = 0xFC,                // AVDECC Connection Management Protocol
    // 0xFD Reserved
    MAAP = 0xFE,                // MAAP Protocol
    EF_CONTROL = 0xFF,          // Experimental Format Control
    
    // Legacy aliases for backward compatibility
    AVTP_AUDIO = AAF,           // Alias for AAF
    AVTP_VIDEO = CVF,           // Alias for CVF
    AVTP_CONTROL = NTSCF,       // Alias for NTSCF
    AVTP_SDI = SVF,             // Alias for SVF
    AVTP_RVF = RVF,             // Direct mapping
    AVTP_TSCF = TSCF,           // Direct mapping
    AVTP_ANCILLARY = 0x25,      // Custom extension for ancillary data
    AVTP_AEF = AEF_CONTINUOUS,  // Alias for AES Encrypted Format
    AVTP_GENERIC = VSF_STREAM,  // Alias for Vendor Specific
    EXPERIMENTAL = EF_STREAM,   // Alias for Experimental Stream
    
    // Extended formats (beyond official standard)
    MIDI = MMA_STREAM,          // Alias for MMA_STREAM
    IEC61883_6 = 0x02,          // Legacy - now mapped to AAF
    CRF_AUDIO = 0x06,           // Legacy - now mapped to SVF
    CRF_VIDEO = 0x07,           // Legacy - now mapped to RVF
    AES_ENCRYPTED = AEF_CONTINUOUS // Alias for AES Encrypted
};
struct AVTPDU {
    // AVTP Common Header (per 2016 spec)
    uint8_t subtype;              // AVTP subtype (audio, video, etc.)
    bool stream_valid;            // Stream ID valid flag (sv)
    uint8_t version;              // AVTP version (should be 0x00 for 2016)
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
    
    // Constructors
    AVTPDU();
    AVTPDU(const uint8_t* data, size_t length);
    
    // Serialization methods
    void serialize(uint8_t* buffer, size_t& length) const;
    bool deserialize(const uint8_t* data, size_t length);
    
    // Utility methods
    static constexpr size_t get_header_size() { return 20; } // AVTP header size
    bool is_valid() const { return version == AVTP_VERSION_2016; }
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
// Enhanced Compressed Video Format (CVF) AVTPDU
// =============================
enum class CVFFormat : uint8_t {
    RFC_MJPEG = 0x00,       // RFC 2435 MJPEG
    H264      = 0x01,       // RFC 6184 H.264
    JPEG2000  = 0x02        // RFC 5371 JPEG 2000
};

struct CVFAVTPDU : public VideoAVTPDU {
    CVFFormat cvf_format;
    bool m_bit;                     // Marker bit (RFC compliance)
    bool evt;                       // Event indicator
    uint32_t presentation_timestamp; // Presentation timestamp (PTS)
    uint16_t h264_timestamp;        // H.264 specific timestamp
    uint8_t h264_ptv;              // H.264 PTV (Presentation Time Valid)
    uint8_t h264_fragment_offset;   // H.264 fragment offset
    
    // H.264 specific fields
    struct H264Info {
        uint8_t nal_unit_type;      // NAL unit type
        bool start_of_frame;        // Start of frame indicator  
        bool end_of_frame;          // End of frame indicator
        uint8_t temporal_id;        // Temporal layer ID
        uint16_t sequence_number;   // H.264 sequence number
    } h264_info;
    
    // JPEG2000 specific fields
    struct JPEG2000Info {
        uint32_t tile_number;       // Tile number
        bool main_header;           // Main header flag
        bool tile_header;           // Tile part header flag
        uint8_t priority;           // Quality layer priority
        uint16_t progression_order; // Progression order
    } jpeg2000_info;
    
    CVFAVTPDU();
    void set_h264_format(uint16_t w, uint16_t h, FrameRate rate);
    void set_jpeg2000_format(uint16_t w, uint16_t h, FrameRate rate);
    void set_mjpeg_format(uint16_t w, uint16_t h, FrameRate rate);
    bool is_fragment_start() const;
    bool is_fragment_end() const;
};

// =============================
// Clock Reference Format (CRF) AVTPDU
// =============================
enum class CRFType : uint8_t {
    USER_SPECIFIED = 0x00,
    AUDIO_SAMPLE = 0x01,
    VIDEO_FRAME = 0x02,
    VIDEO_LINE = 0x03,
    MACHINE_CYCLE = 0x04
};

struct CRFAVTPDU : public AVTPDU {
    CRFType crf_type;
    uint64_t crf_timestamp;         // CRF timestamp value
    uint16_t crf_data_length;       // Length of CRF data
    uint32_t timestamp_interval;    // Timestamp interval
    uint8_t base_frequency;         // Base frequency indicator
    uint16_t pull;                  // Pull multiplier/divisor
    
    CRFAVTPDU();
    void set_crf_audio_sample(uint32_t sample_rate);
    void set_crf_video_frame(FrameRate frame_rate);
    bool is_valid_crf() const;
};

// =============================
// AVTP Control Format (ACF) AVTPDU
// =============================
enum class ControlFormat : uint8_t {
    NON_TIME_SYNC = 0x00,
    TIME_SYNC = 0x01
};

enum class ACFMessageType : uint8_t {
    USER_SPECIFIED = 0x00,
    CAN_BRIEF = 0x02,
    CAN_FULL = 0x03,
    LIN = 0x04,
    MOST = 0x05,
    FLEXRAY = 0x06,
    SENSOR_BRIEF = 0x09,
    SENSOR_DETAILED = 0x0A,
    AECP = 0x7C,
    ADP = 0x7D,
    ACMP = 0x7E,
    VENDOR_DEFINED = 0x7F
};

struct ControlAVTPDU : public AVTPDU {
    ControlFormat control_format;
    ACFMessageType message_type;
    uint16_t control_data_length;
    uint32_t message_timestamp;     // For time-synchronous control
    std::array<uint8_t, 1472> control_data; // Control message payload
    
    ControlAVTPDU();
    void set_control_message(ControlFormat fmt, ACFMessageType type, const uint8_t* data, uint16_t length);
    bool is_time_sync() const { return control_format == ControlFormat::TIME_SYNC; }
};

// =============================
// SDI Video Format AVTPDU
// =============================
enum class SDIFormat : uint8_t {
    SD_525_59_94 = 0x00,        // SMPTE ST 259 525/59.94
    SD_625_50 = 0x01,           // SMPTE ST 259 625/50
    HD_720P_50 = 0x02,          // SMPTE ST 296 720p/50
    HD_720P_59_94 = 0x03,       // SMPTE ST 296 720p/59.94
    HD_1080I_50 = 0x04,         // SMPTE ST 274 1080i/50
    HD_1080I_59_94 = 0x05,      // SMPTE ST 274 1080i/59.94
    HD_1080P_24 = 0x06,         // SMPTE ST 274 1080p/24
    HD_1080P_25 = 0x07,         // SMPTE ST 274 1080p/25
    HD_1080P_30 = 0x08,         // SMPTE ST 274 1080p/30
    UHD_2160P_24 = 0x09,        // SMPTE ST 425 2160p/24
    UHD_2160P_25 = 0x0A,        // SMPTE ST 425 2160p/25
    UHD_2160P_30 = 0x0B         // SMPTE ST 425 2160p/30
};

struct SDIAVTPDU : public AVTPDU {
    SDIFormat sdi_format;
    bool active_video_only;         // Active video only flag
    uint16_t line_number;           // Current line number
    uint16_t line_offset;           // Offset within line
    uint32_t field_identification;  // Field and frame identification
    
    SDIAVTPDU();
    void set_sdi_format(SDIFormat fmt);
    bool is_valid_sdi() const;
};

// =============================
// Raw Video Format (RVF) AVTPDU  
// =============================
enum class PixelFormat : uint8_t {
    YUV420 = 0x00,              // YUV 4:2:0
    YUV422 = 0x01,              // YUV 4:2:2
    YUV444 = 0x02,              // YUV 4:4:4
    RGB24 = 0x03,               // RGB 24-bit
    RGB32 = 0x04,               // RGB 32-bit (RGBA)
    RGBA = 0x05,                // RGBA
    YUV420P10 = 0x06,           // YUV 4:2:0 10-bit
    YUV422P10 = 0x07,           // YUV 4:2:2 10-bit
    BAYER_RGGB = 0x08,          // Bayer RGGB
    BAYER_GRBG = 0x09,          // Bayer GRBG
    BAYER_GBRG = 0x0A,          // Bayer GBRG
    BAYER_BGGR = 0x0B           // Bayer BGGR
};

enum class ColorSpace : uint8_t {
    BT601 = 0x00,               // ITU-R BT.601
    BT709 = 0x01,               // ITU-R BT.709  
    BT2020 = 0x02,              // ITU-R BT.2020
    SRGB = 0x03,                // sRGB
    DCI_P3 = 0x04,              // DCI-P3
    ADOBE_RGB = 0x05            // Adobe RGB
};

struct RVFAVTPDU : public AVTPDU {
    PixelFormat pixel_format;
    ColorSpace color_space;
    uint16_t width;
    uint16_t height;
    uint8_t bit_depth;              // Bit depth per component
    uint16_t line_stride;           // Bytes per line
    uint32_t frame_size;            // Total frame size in bytes
    bool progressive;               // Progressive vs interlaced
    bool top_field_first;           // Field order for interlaced
    
    RVFAVTPDU();
    void set_raw_video_format(uint16_t w, uint16_t h, PixelFormat fmt, uint8_t depth);
    size_t calculate_frame_size() const;
    bool is_valid_raw_video() const;
};

// =============================
// AES Encrypted Format AVTPDU
// =============================
enum class EncryptionMode : uint8_t {
    AES_128_CTR = 0x00,         // AES-128 Counter Mode
    AES_256_CTR = 0x01,         // AES-256 Counter Mode
    AES_128_CBC = 0x02,         // AES-128 Cipher Block Chaining
    AES_256_CBC = 0x03,         // AES-256 Cipher Block Chaining
    AES_128_GCM = 0x04,         // AES-128 Galois/Counter Mode
    AES_256_GCM = 0x05          // AES-256 Galois/Counter Mode
};

struct AESInfo {
    EncryptionMode mode;
    uint8_t key_id[16];             // Key identifier
    uint8_t initialization_vector[16]; // IV for encryption
    uint16_t auth_tag_length;       // Authentication tag length (for GCM)
    bool authenticated;             // Whether packet is authenticated
    
    AESInfo();
    void generate_iv();
    bool is_gcm_mode() const;
};

struct AESAVTPDU : public AVTPDU {
    AESInfo aes_info;
    Subtype encrypted_subtype;      // Original subtype before encryption
    uint32_t encrypted_data_length; // Length of encrypted payload
    
    AESAVTPDU();
    void set_encryption(EncryptionMode mode, const uint8_t* key_id);
    bool decrypt_to_avtpdu(AVTPDU& output) const;
    bool is_valid_encrypted() const;
};

// =============================
// MIDI Transport Format AVTPDU
// =============================
enum class MIDIFormat : uint8_t {
    MIDI_1_0 = 0x00,            // Standard MIDI 1.0
    MIDI_2_0 = 0x01,            // MIDI 2.0 (future)
    MPE = 0x02,                 // MIDI Polyphonic Expression
    SYSEX = 0x03                // System Exclusive
};

struct MIDIAVTPDU : public AVTPDU {
    MIDIFormat midi_format;
    uint8_t midi_channel;           // MIDI channel (0-15)
    uint16_t midi_data_length;      // Length of MIDI data
    uint32_t midi_timestamp;        // MIDI event timestamp
    bool running_status;            // Running status optimization
    std::array<uint8_t, 1472> midi_data; // MIDI message payload
    
    MIDIAVTPDU();
    void set_midi_message(uint8_t channel, const uint8_t* data, uint16_t length);
    void add_note_on(uint8_t channel, uint8_t note, uint8_t velocity);
    void add_note_off(uint8_t channel, uint8_t note, uint8_t velocity);
    void add_control_change(uint8_t channel, uint8_t controller, uint8_t value);
    bool is_valid_midi() const;
};

// =============================
// Ancillary Data Format AVTPDU
// =============================
enum class AncillaryDataType : uint8_t {
    CAPTIONS_CEA_608 = 0x00,    // CEA-608 Closed Captions
    CAPTIONS_CEA_708 = 0x01,    // CEA-708 Closed Captions
    TIMECODE_LTC = 0x02,        // Linear Timecode
    TIMECODE_VITC = 0x03,       // Vertical Interval Timecode
    AFD = 0x04,                 // Active Format Description
    BAR_DATA = 0x05,            // Bar Data
    PAN_SCAN = 0x06,            // Pan and Scan
    ANSI_SCTE_104 = 0x07,       // ANSI/SCTE 104 Messages
    DVB_SCTE_35 = 0x08,         // DVB/SCTE-35 Splice Info
    KLV_METADATA = 0x09,        // KLV Metadata (SMPTE 336M)
    USER_DEFINED = 0xFF         // User-defined ancillary data
};

struct AncillaryAVTPDU : public AVTPDU {
    AncillaryDataType anc_type;
    uint16_t did;                   // Data Identifier
    uint16_t sdid;                  // Secondary Data Identifier
    uint16_t data_count;            // Data count
    uint16_t line_number;           // Video line number
    uint16_t horizontal_offset;     // Horizontal offset
    bool c_not_y;                   // C (chroma) not Y (luma) flag
    std::array<uint8_t, 1456> anc_data; // Ancillary data payload
    
    AncillaryAVTPDU();
    void set_ancillary_data(AncillaryDataType type, uint16_t did, uint16_t sdid, 
                           const uint8_t* data, uint16_t length);
    void set_cea608_captions(const uint8_t* caption_data, uint16_t length);
    void set_timecode_ltc(uint32_t hours, uint32_t minutes, uint32_t seconds, uint32_t frames);
    bool is_valid_ancillary() const;
};

// =============================
// IEC 61883 IIDC Format AVTPDU
// =============================
enum class IIDCFormat : uint8_t {
    DV_25 = 0x00,               // DV 25 Mbps
    DV_50 = 0x01,               // DV 50 Mbps  
    DV_100 = 0x02,              // DV 100 Mbps
    DVCPRO_25 = 0x03,           // DVCPRO 25
    DVCPRO_50 = 0x04,           // DVCPRO 50
    DVCPRO_HD = 0x05,           // DVCPRO HD
    HDV = 0x06,                 // HDV
    MPEG2_TS = 0x07             // MPEG-2 Transport Stream
};

struct IEC61883IIDCAVTPDU : public AVTPDU {
    IIDCFormat iidc_format;
    uint8_t tag;                    // IEEE 1394 tag field
    uint8_t channel;                // IEEE 1394 channel
    uint8_t tcode;                  // IEEE 1394 transaction code
    uint8_t sy;                     // IEEE 1394 synchronization field
    uint16_t data_length;           // IEC 61883 data length
    std::array<uint8_t, 1472> iidc_data; // IEC 61883 payload
    
    IEC61883IIDCAVTPDU();
    void set_iidc_format(IIDCFormat fmt, uint8_t ch);
    bool is_valid_iidc() const;
};

// =============================
// IEC 61883-6 Audio Format AVTPDU
// =============================
enum class IEC61883_6_Format : uint8_t {
    AM824 = 0x00,               // AM824 Audio
    MULTI_BIT_LINEAR_AUDIO = 0x40, // Multi-bit linear audio
    MIDI_CONFORMANT = 0x80,     // MIDI conformant
    RESERVED = 0xFF             // Reserved
};

struct IEC61883_6_AVTPDU : public AVTPDU {
    IEC61883_6_Format format;
    uint8_t tag;                    // IEEE 1394 tag field
    uint8_t channel;                // IEEE 1394 channel
    uint8_t tcode;                  // IEEE 1394 transaction code
    uint8_t sy;                     // IEEE 1394 synchronization field
    uint8_t dbs;                    // Data block size
    uint8_t fn;                     // Fraction number
    uint8_t qpc;                    // Quadlet padding count
    uint8_t sph;                    // Source packet header
    uint16_t dbc;                   // Data block continuity
    std::array<uint8_t, 1464> audio_data; // IEC 61883-6 audio payload
    
    IEC61883_6_AVTPDU();
    void set_iec61883_6_format(IEC61883_6_Format fmt, uint8_t ch, uint8_t data_block_size);
    bool is_valid_iec61883_6() const;
};

// =============================
// Time-Synchronous Control Format (TSCF) AVTPDU
// =============================
enum class TSCFMessageType : uint8_t {
    USER_SPECIFIED = 0x00,
    CAN_BRIEF = 0x02,
    CAN_FULL = 0x03,
    LIN = 0x04,
    MOST = 0x05,
    FLEXRAY = 0x06,
    IEEE1722_1_AECP = 0x7C,
    IEEE1722_1_ADP = 0x7D,
    IEEE1722_1_ACMP = 0x7E,
    VENDOR_DEFINED = 0x7F
};

struct TSCFAVTPDU : public AVTPDU {
    TSCFMessageType message_type;
    uint16_t tscf_data_length;      // TSCF data length
    uint32_t stream_data_time;      // Stream data time
    bool fs;                        // Frame sync flag
    uint8_t tu;                     // Time uncertain
    std::array<uint8_t, 1468> tscf_data; // TSCF payload
    
    TSCFAVTPDU();
    void set_tscf_message(TSCFMessageType type, const uint8_t* data, uint16_t length);
    void set_can_message(uint32_t can_id, const uint8_t* data, uint8_t dlc);
    void set_flexray_message(uint16_t slot_id, uint8_t cycle, const uint8_t* data, uint8_t length);
    bool is_valid_tscf() const;
};

// =============================
// Protocol Utilities
// =============================

// IEEE 1722-2016 Stream ID utilities
struct StreamID {
    uint64_t value;
    
    StreamID() : value(0) {}
    explicit StreamID(uint64_t id) : value(id) {}
    
    // Extract EUI-48 (first 48 bits)
    uint64_t get_eui48() const { return (value >> 16) & 0xFFFFFFFFFFFFULL; }
    
    // Extract Unique ID (last 16 bits)  
    uint16_t get_unique_id() const { return value & 0xFFFF; }
    
    // Create from EUI-48 and unique ID
    static StreamID create(uint64_t eui48, uint16_t unique_id) {
        return StreamID((eui48 << 16) | unique_id);
    }
    
    bool operator==(const StreamID& other) const { return value == other.value; }
    bool operator!=(const StreamID& other) const { return value != other.value; }
};

// IEEE 1722-2016 Protocol version validation
constexpr uint8_t IEEE_1722_2016_VERSION = 0;

inline bool is_valid_1722_2016_packet(const AVTPDU& pdu) {
    return pdu.version == IEEE_1722_2016_VERSION && 
           static_cast<uint8_t>(pdu.subtype) <= static_cast<uint8_t>(Subtype::AVTP_AEF);
}

// =============================
// Utility Functions  
// =============================
bool is_valid_avtp_packet(const uint8_t* data, size_t length);
size_t get_avtp_payload_offset(Subtype subtype);
std::string subtype_to_string(Subtype subtype);

// Header-only utility functions
inline bool is_valid_subtype(uint8_t subtype) {
    switch (static_cast<Subtype>(subtype)) {
        // Official IEEE 1722-2016 subtypes (Table 6)
        case Subtype::IEC61883_IIDC:    // 0x00
        case Subtype::MMA_STREAM:       // 0x01 (MIDI)
        case Subtype::AAF:              // 0x02 (AVTP Audio Format)
        case Subtype::CVF:              // 0x03 (Compressed Video Format)
        case Subtype::CRF:              // 0x04 (Clock Reference Format)
        case Subtype::TSCF:             // 0x05 (Time-Synchronous Control Format)
        case Subtype::SVF:              // 0x06 (SDI Video Format)
        case Subtype::RVF:              // 0x07 (Raw Video Format)
        case Subtype::AEF_CONTINUOUS:   // 0x6E (AES Encrypted Format)
        case Subtype::VSF_STREAM:       // 0x6F (Vendor Specific Format)
        case Subtype::EF_STREAM:        // 0x7F (Experimental Format Stream)
        case Subtype::NTSCF:            // 0x82 (Non-Time-Synchronous Control Format)
        case Subtype::ESCF:             // 0xEC (ECC Signed Control Format)
        case Subtype::EECF:             // 0xED (ECC Encrypted Control Format)
        case Subtype::AEF_DISCRETE:     // 0xEE (AES Encrypted Format Discrete)
        case Subtype::ADP:              // 0xFA (AVDECC Discovery Protocol)
        case Subtype::AECP:             // 0xFB (AVDECC Enumeration Control Protocol)
        case Subtype::ACMP:             // 0xFC (AVDECC Connection Management Protocol)
        case Subtype::MAAP:             // 0xFE (MAAP Protocol)
        case Subtype::EF_CONTROL:       // 0xFF (Experimental Format Control)
        // Legacy extensions (custom subtypes for backward compatibility)
        case Subtype::AVTP_ANCILLARY:   // 0x25 (Custom extension)
            return true;
        default:
            return false;
    }
}

// String conversion for debugging/logging
inline std::string subtype_to_string(Subtype subtype) {
    switch (subtype) {
        case Subtype::IEC61883_IIDC: return "IEC61883_IIDC (0x00)";
        case Subtype::MMA_STREAM: return "MMA_STREAM (0x01) - MIDI";
        case Subtype::AAF: return "AAF (0x02) - AVTP Audio Format";
        case Subtype::CVF: return "CVF (0x03) - Compressed Video Format";
        case Subtype::CRF: return "CRF (0x04) - Clock Reference Format";
        case Subtype::TSCF: return "TSCF (0x05) - Time-Synchronous Control Format";
        case Subtype::SVF: return "SVF (0x06) - SDI Video Format";
        case Subtype::RVF: return "RVF (0x07) - Raw Video Format";
        case Subtype::AEF_CONTINUOUS: return "AEF_CONTINUOUS (0x6E) - AES Encrypted Format";
        case Subtype::VSF_STREAM: return "VSF_STREAM (0x6F) - Vendor Specific Format";
        case Subtype::EF_STREAM: return "EF_STREAM (0x7F) - Experimental Format Stream";
        case Subtype::NTSCF: return "NTSCF (0x82) - Non-Time-Synchronous Control Format";
        case Subtype::ESCF: return "ESCF (0xEC) - ECC Signed Control Format";
        case Subtype::EECF: return "EECF (0xED) - ECC Encrypted Control Format";
        case Subtype::AEF_DISCRETE: return "AEF_DISCRETE (0xEE) - AES Encrypted Format Discrete";
        case Subtype::ADP: return "ADP (0xFA) - AVDECC Discovery Protocol";
        case Subtype::AECP: return "AECP (0xFB) - AVDECC Enumeration Control Protocol";
        case Subtype::ACMP: return "ACMP (0xFC) - AVDECC Connection Management Protocol";
        case Subtype::MAAP: return "MAAP (0xFE) - MAAP Protocol";
        case Subtype::EF_CONTROL: return "EF_CONTROL (0xFF) - Experimental Format Control";
        case Subtype::AVTP_ANCILLARY: return "AVTP_ANCILLARY (0x25) - Custom Extension";
        default: return "UNKNOWN_SUBTYPE (0x" + std::to_string(static_cast<uint8_t>(subtype)) + ")";
    }
}

} // namespace ieee_1722_2016
} // namespace avtp_protocol

#endif // IEEE_1722_2016_STREAMING_H