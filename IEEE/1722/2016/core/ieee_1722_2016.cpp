/**
 * @file 1722-2016.cpp
 * @brief IEEE 1722-2016 Standard Implementation
 * 
 * Implementation of IEEE Standard for a Transport Protocol for Time-Sensitive 
 * Applications in Bridged Local Area Networks (2016 version).
 * 
 * @version 1.0.0
 * @date 2025
 * @copyright OpenAvnu Project
 */

#include "ieee_1722_2016.h"
#include <cstring>
#include <algorithm>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
#endif

namespace IEEE {
namespace _1722 {
namespace _2016 {

// ==============================
// AVTPDU Implementation
// ==============================

AVTPDU::AVTPDU() 
    : subtype(0x00)
    , stream_valid(false)
    , version(AVTP_VERSION_2016)
    , mr(false)
    , gv(false)
    , tv(false)
    , sequence_num(0)
    , tu(false)
    , stream_data_length(0)
    , avtp_timestamp(0)
{
    memset(stream_id, 0, sizeof(stream_id));
    payload.fill(0);
}

AVTPDU::AVTPDU(const uint8_t* data, size_t length) 
    : AVTPDU()
{
    deserialize(data, length);
}

void AVTPDU::serialize(uint8_t* buffer, size_t& length) const {
    if (!buffer || length < get_header_size()) {
        length = 0;
        return;
    }
    
    size_t offset = 0;
    
    // Byte 0: subtype
    buffer[offset++] = subtype;
    
    // Byte 1: version (4 bits), stream_valid (1 bit), mr (1 bit), gv (1 bit), tv (1 bit)
    buffer[offset] = (version & 0x0F) << 4;
    if (stream_valid) buffer[offset] |= 0x08;
    if (mr) buffer[offset] |= 0x04;
    if (gv) buffer[offset] |= 0x02;
    if (tv) buffer[offset] |= 0x01;
    offset++;
    
    // Byte 2: sequence_num
    buffer[offset++] = sequence_num;
    
    // Byte 3: reserved (7 bits), tu (1 bit)
    buffer[offset] = 0x00;
    if (tu) buffer[offset] |= 0x01;
    offset++;
    
    // Bytes 4-11: stream_id (8 bytes)
    memcpy(buffer + offset, stream_id, 8);
    offset += 8;
    
    // Bytes 12-15: avtp_timestamp (4 bytes, network byte order)
    uint32_t timestamp_be = htonl(avtp_timestamp);
    memcpy(buffer + offset, &timestamp_be, 4);
    offset += 4;
    
    // Bytes 16-17: stream_data_length (2 bytes, network byte order)
    uint16_t length_be = htons(stream_data_length);
    memcpy(buffer + offset, &length_be, 2);
    offset += 2;
    
    // Bytes 18-19: format_specific_data (2 bytes, network byte order)
    uint16_t format_be = htons(format_specific_data);
    memcpy(buffer + offset, &format_be, 2);
    offset += 2;
    
    // Payload data
    size_t max_payload = (payload.size() < (length - offset)) ? payload.size() : (length - offset);
    size_t payload_size = (static_cast<size_t>(stream_data_length) < max_payload) ? 
                         static_cast<size_t>(stream_data_length) : max_payload;
    if (payload_size > 0) {
        memcpy(buffer + offset, payload.data(), payload_size);
        offset += payload_size;
    }
    
    length = offset;
}

bool AVTPDU::deserialize(const uint8_t* data, size_t length) {
    if (!data || length < get_header_size()) {
        return false;
    }
    
    size_t offset = 0;
    
    // Byte 0: subtype
    subtype = data[offset++];
    
    // Byte 1: version, stream_valid, mr, gv, tv
    uint8_t byte1 = data[offset++];
    version = (byte1 >> 4) & 0x0F;
    stream_valid = (byte1 & 0x08) != 0;
    mr = (byte1 & 0x04) != 0;
    gv = (byte1 & 0x02) != 0;
    tv = (byte1 & 0x01) != 0;
    
    // Byte 2: sequence_num
    sequence_num = data[offset++];
    
    // Byte 3: reserved, tu
    uint8_t byte3 = data[offset++];
    tu = (byte3 & 0x01) != 0;
    
    // Bytes 4-11: stream_id
    memcpy(stream_id, data + offset, 8);
    offset += 8;
    
    // Bytes 12-15: avtp_timestamp
    uint32_t timestamp_be;
    memcpy(&timestamp_be, data + offset, 4);
    avtp_timestamp = ntohl(timestamp_be);
    offset += 4;
    
    // Bytes 16-17: stream_data_length
    uint16_t length_be;
    memcpy(&length_be, data + offset, 2);
    stream_data_length = ntohs(length_be);
    offset += 2;
    
    // Bytes 18-19: format_specific_data
    uint16_t format_be;
    memcpy(&format_be, data + offset, 2);
    format_specific_data = ntohs(format_be);
    offset += 2;
    
    // Payload data
    size_t remaining = length - offset;
    size_t max_remaining = (remaining < payload.size()) ? remaining : payload.size();
    size_t payload_size = (static_cast<size_t>(stream_data_length) < max_remaining) ? 
                         static_cast<size_t>(stream_data_length) : max_remaining;
    
    if (payload_size > 0) {
        memcpy(payload.data(), data + offset, payload_size);
    }
    
    return true;
}

// ==============================
// AudioAVTPDU Implementation
// ==============================

AudioAVTPDU::AudioAVTPDU() 
    : AVTPDU()
    , format(AudioFormat::IEC_61883_6)
    , nominal_sample_rate(SampleRate::RATE_48KHZ)
    , channels(0)
    , bit_depth(16)
    , samples_per_frame(0)
{
    subtype = static_cast<uint8_t>(Subtype::AVTP_AUDIO);
}

void AudioAVTPDU::set_audio_format(AudioFormat fmt, uint8_t ch, uint8_t depth) {
    format = fmt;
    channels = ch;
    bit_depth = depth;
    
    // Update format_specific_data based on audio format
    format_specific_data = (static_cast<uint16_t>(format) << 8) | 
                          ((channels & 0x0F) << 4) | 
                          ((bit_depth == 24 ? 1 : 0) << 3);
}

// ==============================
// VideoAVTPDU Implementation
// ==============================

VideoAVTPDU::VideoAVTPDU() 
    : AVTPDU()
    , format(VideoFormat::IEC_61883_4)
    , frame_rate(FrameRate::RATE_30FPS)
    , width(0)
    , height(0)
{
    subtype = static_cast<uint8_t>(Subtype::AVTP_VIDEO);
}

void VideoAVTPDU::set_video_format(VideoFormat fmt, uint16_t w, uint16_t h, FrameRate rate) {
    format = fmt;
    width = w;
    height = h;
    frame_rate = rate;
    
    // Update format_specific_data based on video format
    format_specific_data = (static_cast<uint16_t>(format) << 8) | 
                          (static_cast<uint16_t>(frame_rate) & 0xFF);
}

// ==============================
// Utility Functions
// ==============================

bool is_valid_avtp_packet(const uint8_t* data, size_t length) {
    if (!data || length < AVTPDU::get_header_size()) {
        return false;
    }
    
    // Check version
    uint8_t version = (data[1] >> 4) & 0x0F;
    if (version != AVTP_VERSION_2016) {
        return false;
    }
    
    // Check subtype
    uint8_t subtype = data[0];
    return is_valid_subtype(subtype);
}

size_t get_avtp_payload_offset(Subtype subtype) {
    switch (subtype) {
        case Subtype::AVTP_AUDIO:
        case Subtype::AVTP_VIDEO:
            return AVTPDU::get_header_size() + 4; // Additional format-specific headers
        case Subtype::IEC61883_6:
            return AVTPDU::get_header_size() + 8; // IEC 61883-6 specific headers
        default:
            return AVTPDU::get_header_size();
    }
}

std::string subtype_to_string(Subtype subtype) {
    switch (subtype) {
        case Subtype::IEC61883_IIDC: return "IEC 61883 IIDC";
        case Subtype::IEC61883_6: return "IEC 61883-6";
        case Subtype::MIDI: return "MIDI";
        case Subtype::SMPTE: return "SMPTE";
        case Subtype::AVTP_AUDIO: return "AVTP Audio";
        case Subtype::AVTP_VIDEO: return "AVTP Video";
        case Subtype::AVTP_CONTROL: return "AVTP Control";
        case Subtype::AVTP_ANCILLARY: return "AVTP Ancillary";
        case Subtype::AVTP_GENERIC: return "AVTP Generic";
        case Subtype::EXPERIMENTAL: return "Experimental";
        default: return "Unknown";
    }
}

} // namespace _2016
} // namespace _1722
} // namespace IEEE
