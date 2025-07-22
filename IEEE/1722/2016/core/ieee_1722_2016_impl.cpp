// IEEE 1722-2016 AVTP Implementation
// Implementation of AVTP protocol classes defined in 1722-2016.h

#include "ieee_1722_2016.h"
#include <cstring>
#include <algorithm>

namespace IEEE {
namespace _1722 {
namespace _2016 {

// AVTPDU Implementation
AVTPDU::AVTPDU() {
    subtype = 0;
    stream_valid = false;
    version = AVTP_VERSION_2016;
    mr = false;
    gv = false;
    tv = false;
    sequence_num = 0;
    tu = false;
    memset(stream_id, 0, sizeof(stream_id));
    avtp_timestamp = 0;
    stream_data_length = 0;
    format_specific_data = 0;
    payload.fill(0);
}

AVTPDU::AVTPDU(const uint8_t* data, size_t length) : AVTPDU() {
    if (data && length >= get_header_size()) {
        deserialize(data, length);
    }
}

void AVTPDU::serialize(uint8_t* buffer, size_t& length) const {
    if (!buffer) return;
    
    size_t total_size = get_header_size() + stream_data_length;
    if (length < total_size) return;
    
    // Pack AVTP header according to IEEE 1722-2016
    buffer[0] = subtype;
    buffer[1] = (stream_valid ? 0x80 : 0) | (version << 4) | (mr ? 0x08 : 0) | 
                (gv ? 0x04 : 0) | (tv ? 0x02 : 0) | (tu ? 0x01 : 0);
    buffer[2] = sequence_num;
    buffer[3] = 0; // Reserved
    
    // Stream ID (8 bytes)
    memcpy(buffer + 4, stream_id, 8);
    
    // Timestamp and length
    buffer[12] = (avtp_timestamp >> 24) & 0xFF;
    buffer[13] = (avtp_timestamp >> 16) & 0xFF;
    buffer[14] = (avtp_timestamp >> 8) & 0xFF;
    buffer[15] = avtp_timestamp & 0xFF;
    
    buffer[16] = (stream_data_length >> 8) & 0xFF;
    buffer[17] = stream_data_length & 0xFF;
    buffer[18] = (format_specific_data >> 8) & 0xFF;
    buffer[19] = format_specific_data & 0xFF;
    
    // Copy payload
    if (stream_data_length > 0) {
        memcpy(buffer + get_header_size(), payload.data(), stream_data_length);
    }
    
    length = total_size;
}

bool AVTPDU::deserialize(const uint8_t* data, size_t length) {
    if (!data || length < get_header_size()) return false;
    
    // Parse AVTP header
    subtype = data[0];
    stream_valid = (data[1] & 0x80) != 0;
    version = (data[1] >> 4) & 0x07;
    mr = (data[1] & 0x08) != 0;
    gv = (data[1] & 0x04) != 0;
    tv = (data[1] & 0x02) != 0;
    tu = (data[1] & 0x01) != 0;
    
    sequence_num = data[2];
    
    // Stream ID
    memcpy(stream_id, data + 4, 8);
    
    // Timestamp and length
    avtp_timestamp = (data[12] << 24) | (data[13] << 16) | (data[14] << 8) | data[15];
    stream_data_length = (data[16] << 8) | data[17];
    format_specific_data = (data[18] << 8) | data[19];
    
    // Copy payload if available
    if (stream_data_length > 0 && length >= get_header_size() + stream_data_length) {
        size_t payload_size = std::min(static_cast<size_t>(stream_data_length), payload.size());
        memcpy(payload.data(), data + get_header_size(), payload_size);
    }
    
    return is_valid();
}

// AudioAVTPDU Implementation
AudioAVTPDU::AudioAVTPDU() : AVTPDU() {
    subtype = static_cast<uint8_t>(Subtype::AVTP_AUDIO);
    format = AudioFormat::MILAN_PCM;
    nominal_sample_rate = SampleRate::RATE_48KHZ;
    channels = 2;
    bit_depth = 24;
    samples_per_frame = 6;
}

void AudioAVTPDU::set_audio_format(AudioFormat fmt, uint8_t ch, uint8_t depth) {
    format = fmt;
    channels = ch;
    bit_depth = depth;
    
    // Update format-specific data in AVTPDU header
    format_specific_data = (static_cast<uint16_t>(format) << 8) | 
                          ((channels & 0x0F) << 4) | 
                          (bit_depth & 0x0F);
}

} // namespace _2016
} // namespace _1722
} // namespace IEEE
