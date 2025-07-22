/**
 * @file ieee_1722_2016_production_tests.cpp
 * @brief IEEE 1722-2016 AVTP Production Test Suite
 *
 * Comprehensive test suite for IEEE 1722-2016 implementation following the same
 * rigorous testing standards as the IEEE 1722.1-2021 implementation.
 *
 * Test Coverage:
 * - AVTPDU serialization/deserialization
 * - Audio/Video/CRF format support  
 * - Cross-platform compatibility
 * - Milan Alliance extensions
 * - Error handling and validation
 * - Performance benchmarks
 *
 * @version 1.0.0  
 * @date 2025
 * @copyright OpenAvnu Project
 */

#include "../core/avtpdu.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <cassert>
#include <iomanip>

using namespace IEEE::_1722::_2016;

// ============================
// Test Framework
// ============================

class IEEE1722_2016_TestFramework {
private:
    int tests_passed_;
    int tests_failed_;
    std::string current_test_;
    
    void log_test_start(const std::string& test_name) {
        current_test_ = test_name;
        std::cout << "🧪 " << test_name << " ... ";
        std::cout.flush();
    }
    
    void log_test_result(bool passed, const std::string& error_msg = "") {
        if (passed) {
            tests_passed_++;
            std::cout << "✅ PASS" << std::endl;
        } else {
            tests_failed_++;
            std::cout << "❌ FAIL";
            if (!error_msg.empty()) {
                std::cout << " - " << error_msg;
            }
            std::cout << std::endl;
        }
    }
    
public:
    IEEE1722_2016_TestFramework() : tests_passed_(0), tests_failed_(0) {}
    
    void run_all_tests() {
        print_header();
        
        // Core functionality tests
        test_avtpdu_basic_operations();
        test_avtpdu_serialization();
        test_avtpdu_deserialization();
        test_subtype_validation();
        
        // Audio format tests
        test_audio_avtpdu_creation();
        test_audio_avtpdu_configuration();
        test_audio_avtpdu_serialization();
        test_audio_format_validation();
        
        // Video format tests
        test_video_avtpdu_creation();
        test_video_avtpdu_configuration(); 
        test_video_avtpdu_serialization();
        test_video_format_validation();
        
        // CRF format tests
        test_crf_avtpdu_creation();
        test_crf_avtpdu_configuration();
        test_crf_avtpdu_serialization();
        
        // Utility function tests
        test_utility_functions();
        test_factory_functions();
        
        // Milan extensions tests
        test_milan_extensions();
        
        // Cross-platform compatibility tests
        test_cross_platform_endianness();
        
        // Performance tests
        test_serialization_performance();
        test_deserialization_performance();
        
        // Error handling tests
        test_error_handling();
        
        print_summary();
    }
    
private:
    void print_header() {
        std::cout << "\n";
        std::cout << "========================================\n";
        std::cout << "IEEE 1722-2016 AVTP Production Test Suite\n";
        std::cout << "========================================\n";
        std::cout << "Testing IEEE 1722-2016 Audio Video Transport Protocol implementation\n";
        std::cout << "Target: Production-ready quality matching IEEE 1722.1-2021 standard\n\n";
    }
    
    void print_summary() {
        int total_tests = tests_passed_ + tests_failed_;
        double pass_rate = (total_tests > 0) ? (double)tests_passed_ / total_tests * 100.0 : 0.0;
        
        std::cout << "\n========================================\n";
        std::cout << "TEST RESULTS SUMMARY\n";
        std::cout << "========================================\n";
        std::cout << "✅ Tests Passed: " << tests_passed_ << "\n";
        std::cout << "❌ Tests Failed: " << tests_failed_ << "\n";
        std::cout << "📊 Pass Rate: " << std::fixed << std::setprecision(1) << pass_rate << "%\n";
        std::cout << "\n";
        
        if (tests_failed_ == 0) {
            std::cout << "🎉 ALL TESTS PASSED! IEEE 1722-2016 implementation is PRODUCTION READY!\n";
        } else {
            std::cout << "⚠️  Some tests failed. Implementation needs fixes before production deployment.\n";
        }
        std::cout << "\n";
    }
    
    // ============================
    // Core AVTPDU Tests
    // ============================
    
    void test_avtpdu_basic_operations() {
        log_test_start("AVTPDU Basic Operations");
        
        try {
            // Test default construction
            AVTPDU avtpdu;
            bool valid = avtpdu.get_version() == AVTP_VERSION &&
                        avtpdu.get_subtype() == Subtype::IEC_61883_IIDC &&
                        avtpdu.get_stream_id() == 0 &&
                        !avtpdu.get_stream_valid();
            
            // Test parameterized construction
            AVTPDU avtpdu2(Subtype::AAF, 0x123456789ABCDEF0ULL);
            valid &= avtpdu2.get_subtype() == Subtype::AAF &&
                    avtpdu2.get_stream_id() == 0x123456789ABCDEF0ULL &&
                    avtpdu2.get_stream_valid();
            
            // Test field setters/getters
            avtpdu.set_sequence_num(42);
            avtpdu.set_timestamp_valid(true);
            avtpdu.set_avtp_timestamp(0x12345678);
            
            valid &= avtpdu.get_sequence_num() == 42 &&
                    avtpdu.get_timestamp_valid() &&
                    avtpdu.get_avtp_timestamp() == 0x12345678;
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_avtpdu_serialization() {
        log_test_start("AVTPDU Serialization");
        
        try {
            AVTPDU avtpdu(Subtype::AAF, 0x123456789ABCDEF0ULL);
            avtpdu.set_sequence_num(42);
            avtpdu.set_timestamp_valid(true);
            avtpdu.set_avtp_timestamp(0x12345678);
            
            // Add payload
            std::vector<uint8_t> payload = {0x01, 0x02, 0x03, 0x04, 0x05};
            avtpdu.set_payload(payload);
            
            // Serialize
            std::vector<uint8_t> buffer(1500);
            size_t serialized_size = avtpdu.serialize(buffer.data(), buffer.size());
            
            bool valid = serialized_size > AVTP_COMMON_HEADER_SIZE &&
                        serialized_size == avtpdu.get_total_size() &&
                        buffer[0] == static_cast<uint8_t>(Subtype::AAF) &&
                        buffer[2] == 42; // Sequence number
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_avtpdu_deserialization() {
        log_test_start("AVTPDU Deserialization");
        
        try {
            // Create and serialize an Audio AVTPDU (since AAF has subtype-specific data)
            AudioAVTPDU original(0x123456789ABCDEF0ULL, AudioFormat::SAF);
            original.set_sequence_num(123);
            original.set_timestamp_valid(true);
            original.set_avtp_timestamp(0x87654321);
            original.configure_audio(AudioFormat::SAF, 2, 24, SampleRate::RATE_48000);
            
            std::vector<uint8_t> payload = {0xAA, 0xBB, 0xCC, 0xDD};
            original.set_payload(payload);
            
            std::vector<uint8_t> buffer(1500);
            size_t size = original.serialize(buffer.data(), buffer.size());
            
            // Deserialize into new AudioAVTPDU
            AudioAVTPDU deserialized;
            bool success = deserialized.deserialize(buffer.data(), size);
            
            // Verify all fields match step by step
            bool valid = success;
            if (success) {
                valid &= deserialized.get_subtype() == Subtype::AAF;
                valid &= deserialized.get_stream_id() == 0x123456789ABCDEF0ULL;
                valid &= deserialized.get_sequence_num() == 123;
                valid &= deserialized.get_timestamp_valid();
                valid &= deserialized.get_avtp_timestamp() == 0x87654321;
                valid &= deserialized.get_format() == AudioFormat::SAF;
                valid &= deserialized.get_channels() == 2;
                valid &= deserialized.get_payload() == payload;
            }
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_subtype_validation() {
        log_test_start("Subtype Validation");
        
        try {
            // Test valid subtypes
            bool valid = is_valid_subtype(Subtype::AAF) &&
                        is_valid_subtype(Subtype::CVF) &&
                        is_valid_subtype(Subtype::CRF) &&
                        is_valid_subtype(Subtype::ADP) &&
                        is_valid_subtype(Subtype::AECP);
            
            // Test string conversion
            valid &= subtype_to_string(Subtype::AAF) == "AAF" &&
                    subtype_to_string(Subtype::CVF) == "CVF" &&
                    subtype_to_string(Subtype::CRF) == "CRF";
            
            // Test reverse conversion
            valid &= string_to_subtype("AAF") == Subtype::AAF &&
                    string_to_subtype("CVF") == Subtype::CVF;
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    // ============================
    // Audio AVTPDU Tests
    // ============================
    
    void test_audio_avtpdu_creation() {
        log_test_start("Audio AVTPDU Creation");
        
        try {
            // Test default construction
            AudioAVTPDU audio1;
            bool valid = audio1.get_subtype() == Subtype::AAF &&
                        audio1.get_format() == AudioFormat::USER_SPECIFIED &&
                        audio1.get_channels() == 2 &&
                        audio1.get_bit_depth() == 24 &&
                        audio1.get_sample_rate() == SampleRate::RATE_48000;
            
            // Test parameterized construction
            AudioAVTPDU audio2(0x123456789ABCDEF0ULL, AudioFormat::SAF);
            valid &= audio2.get_stream_id() == 0x123456789ABCDEF0ULL &&
                    audio2.get_format() == AudioFormat::SAF &&
                    audio2.get_stream_valid();
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_audio_avtpdu_configuration() {
        log_test_start("Audio AVTPDU Configuration");
        
        try {
            AudioAVTPDU audio(0x123456789ABCDEF0ULL);
            audio.configure_audio(AudioFormat::SAF, 8, 24, SampleRate::RATE_96000);
            
            bool valid = audio.get_format() == AudioFormat::SAF &&
                        audio.get_channels() == 8 &&
                        audio.get_bit_depth() == 24 &&
                        audio.get_sample_rate() == SampleRate::RATE_96000 &&
                        audio.get_samples_per_frame() == 12; // Expected for 96kHz
            
            // Test channel limit (max 8)
            audio.configure_audio(AudioFormat::SAF, 16, 24, SampleRate::RATE_48000);
            valid &= audio.get_channels() == 8; // Should be clamped to 8
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_audio_avtpdu_serialization() {
        log_test_start("Audio AVTPDU Serialization");
        
        try {
            AudioAVTPDU audio(0x123456789ABCDEF0ULL, AudioFormat::SAF);
            audio.configure_audio(AudioFormat::SAF, 2, 24, SampleRate::RATE_48000);
            
            // Add audio payload
            std::vector<uint8_t> audio_data(144); // 2 channels * 6 samples * 3 bytes
            for (size_t i = 0; i < audio_data.size(); ++i) {
                audio_data[i] = static_cast<uint8_t>(i & 0xFF);
            }
            audio.set_payload(audio_data);
            
            // Serialize
            std::vector<uint8_t> buffer(1500);
            size_t size = audio.serialize(buffer.data(), buffer.size());
            
            // Deserialize to verify
            AudioAVTPDU deserialized;
            bool success = deserialized.deserialize(buffer.data(), size);
            
            bool valid = success &&
                        deserialized.get_format() == AudioFormat::SAF &&
                        deserialized.get_channels() == 2 &&
                        deserialized.get_bit_depth() == 24 &&
                        deserialized.get_sample_rate() == SampleRate::RATE_48000 &&
                        deserialized.get_payload() == audio_data;
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_audio_format_validation() {
        log_test_start("Audio Format Validation");
        
        try {
            bool valid = is_audio_format_valid(AudioFormat::USER_SPECIFIED) &&
                        is_audio_format_valid(AudioFormat::IEC_61883_6) &&
                        is_audio_format_valid(AudioFormat::SAF) &&
                        is_audio_format_valid(AudioFormat::IEEE_FLOAT32);
            
            AudioAVTPDU audio;
            valid &= audio.is_audio_format_supported(AudioFormat::SAF) &&
                    audio.is_audio_format_supported(AudioFormat::IEC_61883_6);
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    // ============================
    // Video AVTPDU Tests
    // ============================
    
    void test_video_avtpdu_creation() {
        log_test_start("Video AVTPDU Creation");
        
        try {
            VideoAVTPDU video1;
            bool valid = video1.get_subtype() == Subtype::CVF &&
                        video1.get_format() == VideoFormat::RFC4175_YCbCr_422_8 &&
                        video1.get_width() == 1920 &&
                        video1.get_height() == 1080 &&
                        video1.get_frame_rate() == VideoFrameRate::RATE_30_FPS;
            
            VideoAVTPDU video2(0x123456789ABCDEF0ULL, VideoFormat::H264);
            valid &= video2.get_format() == VideoFormat::H264 &&
                    video2.get_stream_id() == 0x123456789ABCDEF0ULL;
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_video_avtpdu_configuration() {
        log_test_start("Video AVTPDU Configuration");
        
        try {
            VideoAVTPDU video(0x123456789ABCDEF0ULL);
            video.configure_video(VideoFormat::RFC4175_RGB_8, 3840, 2160, VideoFrameRate::RATE_60_FPS);
            
            bool valid = video.get_format() == VideoFormat::RFC4175_RGB_8 &&
                        video.get_width() == 3840 &&
                        video.get_height() == 2160 &&
                        video.get_frame_rate() == VideoFrameRate::RATE_60_FPS;
            
            // Test video data size calculation
            size_t expected_size = 3840 * 2160 * 3; // RGB 8-bit
            valid &= video.get_video_data_size() == expected_size;
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_video_avtpdu_serialization() {
        log_test_start("Video AVTPDU Serialization");
        
        try {
            VideoAVTPDU video(0x123456789ABCDEF0ULL, VideoFormat::H264);
            video.configure_video(VideoFormat::H264, 1920, 1080, VideoFrameRate::RATE_25_FPS);
            video.set_interlaced(false);
            
            // Add video payload
            std::vector<uint8_t> video_data(1024);
            for (size_t i = 0; i < video_data.size(); ++i) {
                video_data[i] = static_cast<uint8_t>(i & 0xFF);
            }
            video.set_payload(video_data);
            
            // Serialize/deserialize
            std::vector<uint8_t> buffer(1500);
            size_t size = video.serialize(buffer.data(), buffer.size());
            
            VideoAVTPDU deserialized;
            bool success = deserialized.deserialize(buffer.data(), size);
            
            bool valid = success &&
                        deserialized.get_format() == VideoFormat::H264 &&
                        deserialized.get_width() == 1920 &&
                        deserialized.get_height() == 1080 &&
                        deserialized.get_frame_rate() == VideoFrameRate::RATE_25_FPS &&
                        !deserialized.get_interlaced() &&
                        deserialized.get_payload() == video_data;
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_video_format_validation() {
        log_test_start("Video Format Validation");
        
        try {
            bool valid = is_video_format_valid(VideoFormat::RFC4175_YCbCr_422_8) &&
                        is_video_format_valid(VideoFormat::H264) &&
                        is_video_format_valid(VideoFormat::MJPEG);
            
            VideoAVTPDU video;
            valid &= video.is_video_format_supported(VideoFormat::RFC4175_RGB_8);
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    // ============================
    // CRF AVTPDU Tests
    // ============================
    
    void test_crf_avtpdu_creation() {
        log_test_start("CRF AVTPDU Creation");
        
        try {
            CRFAVTPDU crf1;
            bool valid = crf1.get_subtype() == Subtype::CRF &&
                        crf1.get_crf_type() == CRFAVTPDU::CRFType::AUDIO_SAMPLE &&
                        crf1.get_crf_data() == 0 &&
                        crf1.get_crf_data_length() == 4;
            
            CRFAVTPDU crf2(0x123456789ABCDEF0ULL, CRFAVTPDU::CRFType::VIDEO_FRAME);
            valid &= crf2.get_crf_type() == CRFAVTPDU::CRFType::VIDEO_FRAME &&
                    crf2.get_stream_id() == 0x123456789ABCDEF0ULL;
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_crf_avtpdu_configuration() {
        log_test_start("CRF AVTPDU Configuration");
        
        try {
            CRFAVTPDU crf(0x123456789ABCDEF0ULL);
            
            // Test audio CRF configuration
            crf.configure_audio_crf(0x12345678);
            bool valid = crf.get_crf_type() == CRFAVTPDU::CRFType::AUDIO_SAMPLE &&
                        crf.get_crf_data() == 0x12345678;
            
            // Test video CRF configuration
            crf.configure_video_crf(0x87654321);
            valid &= crf.get_crf_type() == CRFAVTPDU::CRFType::VIDEO_FRAME &&
                    crf.get_crf_data() == 0x87654321;
            
            // Test type support validation
            valid &= crf.is_crf_type_supported(CRFAVTPDU::CRFType::AUDIO_SAMPLE) &&
                    crf.is_crf_type_supported(CRFAVTPDU::CRFType::MACHINE_CYCLE);
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_crf_avtpdu_serialization() {
        log_test_start("CRF AVTPDU Serialization");
        
        try {
            CRFAVTPDU crf(0x123456789ABCDEF0ULL, CRFAVTPDU::CRFType::AUDIO_SAMPLE);
            crf.configure_audio_crf(0x12345678);
            
            // Serialize/deserialize
            std::vector<uint8_t> buffer(1500);
            size_t size = crf.serialize(buffer.data(), buffer.size());
            
            CRFAVTPDU deserialized;
            bool success = deserialized.deserialize(buffer.data(), size);
            
            bool valid = success &&
                        deserialized.get_crf_type() == CRFAVTPDU::CRFType::AUDIO_SAMPLE &&
                        deserialized.get_crf_data() == 0x12345678 &&
                        deserialized.get_crf_data_length() == 4;
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    // ============================
    // Utility and Factory Tests
    // ============================
    
    void test_utility_functions() {
        log_test_start("Utility Functions");
        
        try {
            // Test packet validation
            std::vector<uint8_t> valid_packet(20, 0);
            valid_packet[0] = static_cast<uint8_t>(Subtype::AAF); // Valid subtype
            valid_packet[1] = 0x00; // Version 0
            bool valid = is_valid_avtp_packet(valid_packet.data(), valid_packet.size());
            
            // Test header size calculation
            valid &= get_avtp_header_size(Subtype::AAF) == 24 &&  // 16 + 8 for audio
                    get_avtp_header_size(Subtype::CVF) == 28 &&   // 16 + 12 for video
                    get_avtp_header_size(Subtype::CRF) == 24;     // 16 + 8 for CRF
            
            // Test payload size calculation
            valid &= get_max_payload_size(Subtype::AAF) == (AVTP_MAX_PAYLOAD_SIZE - 8) &&
                    get_max_payload_size(Subtype::CVF) == (AVTP_MAX_PAYLOAD_SIZE - 12);
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_factory_functions() {
        log_test_start("Factory Functions");
        
        try {
            // Test generic AVTPDU factory
            auto avtpdu = create_avtpdu(Subtype::AAF, 0x123456789ABCDEF0ULL);
            bool valid = avtpdu != nullptr &&
                        avtpdu->get_subtype() == Subtype::AAF;
            
            // Test audio AVTPDU factory
            auto audio = create_audio_avtpdu(0x123456789ABCDEF0ULL, AudioFormat::SAF);
            valid &= audio != nullptr &&
                    audio->get_format() == AudioFormat::SAF;
            
            // Test video AVTPDU factory
            auto video = create_video_avtpdu(0x123456789ABCDEF0ULL, VideoFormat::H264);
            valid &= video != nullptr &&
                    video->get_format() == VideoFormat::H264;
            
            // Test CRF AVTPDU factory
            auto crf = create_crf_avtpdu(0x123456789ABCDEF0ULL, CRFAVTPDU::CRFType::AUDIO_SAMPLE);
            valid &= crf != nullptr &&
                    crf->get_crf_type() == CRFAVTPDU::CRFType::AUDIO_SAMPLE;
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    // ============================
    // Milan Extensions Tests
    // ============================
    
    void test_milan_extensions() {
        log_test_start("Milan Alliance Extensions");
        
        try {
            // Test Milan format compatibility
            bool valid = Milan::is_milan_compatible_audio_format(AudioFormat::SAF) &&
                        Milan::is_milan_compatible_audio_format(AudioFormat::IEC_61883_6);
            
            // Test Milan sample rate compatibility
            valid &= Milan::is_milan_compatible_sample_rate(Milan::MILAN_SAMPLE_RATE_48K) &&
                    Milan::is_milan_compatible_sample_rate(Milan::MILAN_SAMPLE_RATE_96K);
            
            // Test Milan stream configuration
            AudioAVTPDU audio(0x123456789ABCDEF0ULL);
            Milan::configure_milan_audio_stream(audio, 8);
            
            valid &= audio.get_format() == AudioFormat::SAF &&
                    audio.get_channels() == 8 &&
                    audio.get_bit_depth() == 24 &&
                    audio.get_sample_rate() == Milan::MILAN_SAMPLE_RATE_48K &&
                    audio.get_timestamp_valid() &&
                    audio.get_stream_valid();
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    // ============================
    // Cross-Platform Tests
    // ============================
    
    void test_cross_platform_endianness() {
        log_test_start("Cross-Platform Endianness");
        
        try {
            // Test endianness conversion functions work correctly
            uint16_t val16 = 0x1234;
            uint32_t val32 = 0x12345678;
            uint64_t val64 = 0x123456789ABCDEF0ULL;
            
            // Convert to big endian and back
            uint16_t be16 = htobe16(val16);
            uint32_t be32 = htobe32(val32);
            uint64_t be64 = htobe64(val64);
            
            bool valid = be16toh(be16) == val16 &&
                        be32toh(be32) == val32 &&
                        be64toh(be64) == val64;
            
            // Test that serialization/deserialization works with different endianness
            AVTPDU avtpdu(Subtype::AAF, val64);
            avtpdu.set_avtp_timestamp(val32);
            
            std::vector<uint8_t> buffer(100);
            size_t size = avtpdu.serialize(buffer.data(), buffer.size());
            
            AVTPDU deserialized;
            bool success = deserialized.deserialize(buffer.data(), size);
            
            valid &= success &&
                    deserialized.get_stream_id() == val64 &&
                    deserialized.get_avtp_timestamp() == val32;
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    // ============================
    // Performance Tests
    // ============================
    
    void test_serialization_performance() {
        log_test_start("Serialization Performance");
        
        try {
            AudioAVTPDU audio(0x123456789ABCDEF0ULL, AudioFormat::SAF);
            audio.configure_audio(AudioFormat::SAF, 8, 24, SampleRate::RATE_48000);
            
            // Create maximum size payload
            std::vector<uint8_t> payload(AVTP_MAX_PAYLOAD_SIZE - 8, 0xAA);
            audio.set_payload(payload);
            
            std::vector<uint8_t> buffer(1500);
            
            // Measure serialization performance
            const int iterations = 10000;
            auto start = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < iterations; ++i) {
                audio.serialize(buffer.data(), buffer.size());
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            double avg_time_us = static_cast<double>(duration.count()) / iterations;
            bool valid = avg_time_us < 50.0; // Should be under 50 microseconds per serialization
            
            if (valid) {
                std::cout << "⏱️  Avg: " << std::fixed << std::setprecision(1) << avg_time_us << "μs ";
            }
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_deserialization_performance() {
        log_test_start("Deserialization Performance");
        
        try {
            // Create serialized packet
            AudioAVTPDU audio(0x123456789ABCDEF0ULL, AudioFormat::SAF);
            std::vector<uint8_t> payload(1000, 0xBB);
            audio.set_payload(payload);
            
            std::vector<uint8_t> buffer(1500);
            size_t size = audio.serialize(buffer.data(), buffer.size());
            
            // Measure deserialization performance
            const int iterations = 10000;
            auto start = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < iterations; ++i) {
                AudioAVTPDU deserialized;
                deserialized.deserialize(buffer.data(), size);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            
            double avg_time_us = static_cast<double>(duration.count()) / iterations;
            bool valid = avg_time_us < 75.0; // Should be under 75 microseconds per deserialization
            
            if (valid) {
                std::cout << "⏱️  Avg: " << std::fixed << std::setprecision(1) << avg_time_us << "μs ";
            }
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    // ============================
    // Error Handling Tests
    // ============================
    
    void test_error_handling() {
        log_test_start("Error Handling");
        
        try {
            bool valid = true;
            
            // Test invalid serialization buffer
            AVTPDU avtpdu;
            size_t result = avtpdu.serialize(nullptr, 100);
            valid &= result == 0;
            
            result = avtpdu.serialize(reinterpret_cast<uint8_t*>(1), 5); // Too small
            valid &= result == 0;
            
            // Test invalid deserialization data
            AVTPDU test_avtpdu;
            bool success = test_avtpdu.deserialize(nullptr, 100);
            valid &= !success;
            
            // Test packet too small
            std::vector<uint8_t> small_buffer(5);
            success = test_avtpdu.deserialize(small_buffer.data(), small_buffer.size());
            valid &= !success;
            
            // Test invalid version
            std::vector<uint8_t> invalid_version(20, 0);
            invalid_version[1] = 0x10; // Invalid version
            success = test_avtpdu.deserialize(invalid_version.data(), invalid_version.size());
            valid &= !success;
            
            // Test payload too large
            std::vector<uint8_t> large_payload(AVTP_MAX_PAYLOAD_SIZE + 100, 0xFF);
            avtpdu.set_payload(large_payload);
            valid &= avtpdu.get_payload_size() != large_payload.size(); // Should not accept oversized payload
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
};

// ============================
// Main Test Runner
// ============================

int main() {
    IEEE1722_2016_TestFramework test_framework;
    test_framework.run_all_tests();
    return 0;
}
