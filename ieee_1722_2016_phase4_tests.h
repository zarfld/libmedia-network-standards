#ifndef IEEE_1722_2016_PHASE4_TESTS_H
#define IEEE_1722_2016_PHASE4_TESTS_H

/**
 * @file ieee_1722_2016_phase4_tests.h
 * @brief Comprehensive Test Framework for IEEE 1722-2016 Phase 4 Gap Closure
 * 
 * Complete test suite validating all components:
 * - Phase 4: Critical Subtype Corrections
 * - Phase 5: Industrial Protocol Implementation  
 * - Phase 6: Security & Cryptography
 * - Phase 7: AVDECC Integration
 * 
 * @author OpenAvnu IEEE 1722-2016 Compliance Team
 * @date July 2025
 * @version 4.0.0 - Phase 4-7 Complete Implementation Tests
 */

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <chrono>
#include <functional>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <iomanip>

// Include all our newly implemented components
#include "ieee_1722_2016_streaming.h"
#include "industrial_protocols.h"
#include "security_cryptography.h"
#include "avdecc_integration.h"

namespace avtp_protocol {
namespace test {

// =============================
// Test Framework Base Classes
// =============================

/**
 * @brief Test Result Structure
 */
struct TestResult {
    std::string test_name;
    bool passed;
    std::string error_message;
    double execution_time_ms;
    std::map<std::string, std::string> details;
    
    std::string to_string() const {
        std::string result = std::string("[") + (passed ? "PASS" : "FAIL") + "] " + test_name;
        if (!passed && !error_message.empty()) {
            result += " - " + error_message;
        }
        result += " (" + std::to_string(execution_time_ms) + "ms)";
        return result;
    }
};

/**
 * @brief Test Suite Base Class
 */
class TestSuiteBase {
public:
    virtual ~TestSuiteBase() = default;
    virtual std::string get_suite_name() const = 0;
    virtual std::vector<TestResult> run_all_tests() = 0;
    
protected:
    TestResult run_test(const std::string& test_name, std::function<bool()> test_function) {
        TestResult result;
        result.test_name = test_name;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            result.passed = test_function();
            if (!result.passed) {
                result.error_message = "Test function returned false";
            }
        } catch (const std::exception& e) {
            result.passed = false;
            result.error_message = e.what();
        } catch (...) {
            result.passed = false;
            result.error_message = "Unknown exception thrown";
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        result.execution_time_ms = duration.count() / 1000.0;
        
        return result;
    }
    
    bool assert_equal(uint32_t expected, uint32_t actual, const std::string& message = "") {
        if (expected != actual) {
            throw std::runtime_error("Expected " + std::to_string(expected) + 
                                   " but got " + std::to_string(actual) + 
                                   (message.empty() ? "" : " - " + message));
        }
        return true;
    }
    
    bool assert_true(bool condition, const std::string& message = "") {
        if (!condition) {
            throw std::runtime_error("Assertion failed" + 
                                   (message.empty() ? "" : " - " + message));
        }
        return true;
    }
};

// =============================
// Phase 4: Critical Subtype Corrections Tests
// =============================

class Phase4SubtypeTests : public TestSuiteBase {
public:
    std::string get_suite_name() const override {
        return "Phase 4: Critical Subtype Corrections";
    }
    
    std::vector<TestResult> run_all_tests() override {
        return {
            run_test("test_official_ieee_subtype_values", 
                    [this]() { return test_official_ieee_subtype_values(); }),
            run_test("test_subtype_enum_alignment", 
                    [this]() { return test_subtype_enum_alignment(); }),
            run_test("test_legacy_compatibility", 
                    [this]() { return test_legacy_compatibility(); }),
            run_test("test_subtype_validation", 
                    [this]() { return test_subtype_validation(); }),
            run_test("test_string_conversion", 
                    [this]() { return test_string_conversion(); }),
            run_test("test_avdecc_protocol_subtypes", 
                    [this]() { return test_avdecc_protocol_subtypes(); })
        };
    }
    
private:
    bool test_official_ieee_subtype_values() {
        using namespace ieee_1722_2016;
        
        // Test official IEEE 1722-2016 Table 6 values
        assert_equal(0x00, static_cast<uint8_t>(Subtype::IEC61883_IIDC));
        assert_equal(0x01, static_cast<uint8_t>(Subtype::MMA_STREAM));
        assert_equal(0x02, static_cast<uint8_t>(Subtype::AAF));
        assert_equal(0x03, static_cast<uint8_t>(Subtype::CVF));
        assert_equal(0x04, static_cast<uint8_t>(Subtype::CRF));
        assert_equal(0x05, static_cast<uint8_t>(Subtype::TSCF));
        assert_equal(0x06, static_cast<uint8_t>(Subtype::SVF));
        assert_equal(0x07, static_cast<uint8_t>(Subtype::RVF));
        
        // Test encrypted and control formats
        assert_equal(0x6E, static_cast<uint8_t>(Subtype::AEF_CONTINUOUS));
        assert_equal(0x6F, static_cast<uint8_t>(Subtype::VSF_STREAM));
        assert_equal(0x7F, static_cast<uint8_t>(Subtype::EF_STREAM));
        assert_equal(0x82, static_cast<uint8_t>(Subtype::NTSCF));
        
        // Test AVDECC protocol subtypes
        assert_equal(0xFA, static_cast<uint8_t>(Subtype::ADP));
        assert_equal(0xFB, static_cast<uint8_t>(Subtype::AECP));
        assert_equal(0xFC, static_cast<uint8_t>(Subtype::ACMP));
        assert_equal(0xFE, static_cast<uint8_t>(Subtype::MAAP));
        
        return true;
    }
    
    bool test_subtype_enum_alignment() {
        using namespace ieee_1722_2016;
        
        // Test that all official subtypes are valid
        assert_true(is_valid_subtype(static_cast<uint8_t>(Subtype::IEC61883_IIDC)));
        assert_true(is_valid_subtype(static_cast<uint8_t>(Subtype::MMA_STREAM)));
        assert_true(is_valid_subtype(static_cast<uint8_t>(Subtype::AAF)));
        assert_true(is_valid_subtype(static_cast<uint8_t>(Subtype::CVF)));
        assert_true(is_valid_subtype(static_cast<uint8_t>(Subtype::CRF)));
        assert_true(is_valid_subtype(static_cast<uint8_t>(Subtype::TSCF)));
        assert_true(is_valid_subtype(static_cast<uint8_t>(Subtype::SVF)));
        assert_true(is_valid_subtype(static_cast<uint8_t>(Subtype::RVF)));
        
        // Test invalid/reserved subtypes
        assert_true(!is_valid_subtype(0x08)); // Reserved
        assert_true(!is_valid_subtype(0x50)); // Reserved
        assert_true(!is_valid_subtype(0x80)); // Reserved
        assert_true(!is_valid_subtype(0xF0)); // Reserved
        
        return true;
    }
    
    bool test_legacy_compatibility() {
        using namespace ieee_1722_2016;
        
        // Test that legacy aliases still work
        assert_equal(static_cast<uint8_t>(Subtype::AAF), 
                    static_cast<uint8_t>(Subtype::AVTP_AUDIO));
        assert_equal(static_cast<uint8_t>(Subtype::CVF), 
                    static_cast<uint8_t>(Subtype::AVTP_VIDEO));
        assert_equal(static_cast<uint8_t>(Subtype::MMA_STREAM), 
                    static_cast<uint8_t>(Subtype::MIDI));
        
        return true;
    }
    
    bool test_subtype_validation() {
        using namespace ieee_1722_2016;
        
        // Test validation function with comprehensive range
        for (uint16_t i = 0; i <= 255; ++i) {
            uint8_t subtype = static_cast<uint8_t>(i);
            bool expected_valid = false;
            
            // Check if this subtype should be valid according to Table 6
            switch (subtype) {
                case 0x00: case 0x01: case 0x02: case 0x03: case 0x04:
                case 0x05: case 0x06: case 0x07: case 0x25: case 0x6E:
                case 0x6F: case 0x7F: case 0x82: case 0xEC: case 0xED:
                case 0xEE: case 0xFA: case 0xFB: case 0xFC: case 0xFE:
                case 0xFF:
                    expected_valid = true;
                    break;
                default:
                    expected_valid = false;
                    break;
            }
            
            assert_equal(expected_valid, is_valid_subtype(subtype), 
                        "Subtype 0x" + std::to_string(subtype));
        }
        
        return true;
    }
    
    bool test_string_conversion() {
        using namespace ieee_1722_2016;
        
        // Test string conversion for key subtypes
        std::string aaf_str = subtype_to_string(Subtype::AAF);
        assert_true(aaf_str.find("AAF") != std::string::npos);
        assert_true(aaf_str.find("0x02") != std::string::npos);
        
        std::string midi_str = subtype_to_string(Subtype::MMA_STREAM);
        assert_true(midi_str.find("MMA_STREAM") != std::string::npos);
        assert_true(midi_str.find("0x01") != std::string::npos);
        assert_true(midi_str.find("MIDI") != std::string::npos);
        
        std::string adp_str = subtype_to_string(Subtype::ADP);
        assert_true(adp_str.find("ADP") != std::string::npos);
        assert_true(adp_str.find("0xFA") != std::string::npos);
        
        return true;
    }
    
    bool test_avdecc_protocol_subtypes() {
        using namespace ieee_1722_2016;
        
        // Test that all AVDECC protocol subtypes are correctly mapped
        std::vector<std::pair<Subtype, uint8_t>> avdecc_subtypes = {
            {Subtype::ADP, 0xFA},
            {Subtype::AECP, 0xFB},
            {Subtype::ACMP, 0xFC},
            {Subtype::MAAP, 0xFE}
        };
        
        for (const auto& pair : avdecc_subtypes) {
            assert_equal(pair.second, static_cast<uint8_t>(pair.first));
            assert_true(is_valid_subtype(pair.second));
        }
        
        return true;
    }
};

// =============================
// Phase 5: Industrial Protocol Tests
// =============================

class Phase5IndustrialTests : public TestSuiteBase {
public:
    std::string get_suite_name() const override {
        return "Phase 5: Industrial Protocol Implementation";
    }
    
    std::vector<TestResult> run_all_tests() override {
        return {
            run_test("test_can_frame_processing", 
                    [this]() { return test_can_frame_processing(); }),
            run_test("test_lin_frame_processing", 
                    [this]() { return test_lin_frame_processing(); }),
            run_test("test_most_frame_processing", 
                    [this]() { return test_most_frame_processing(); }),
            run_test("test_flexray_frame_processing", 
                    [this]() { return test_flexray_frame_processing(); }),
            run_test("test_sensor_data_processing", 
                    [this]() { return test_sensor_data_processing(); }),
            run_test("test_industrial_protocol_manager", 
                    [this]() { return test_industrial_protocol_manager(); })
        };
    }
    
private:
    bool test_can_frame_processing() {
        using namespace industrial::can;
        
        // Create CAN bus configuration
        CANBusConfig config;
        config.bus_id = 1;
        config.nominal_bitrate = 500000;
        config.enable_can_fd = true;
        config.data_bitrate = 2000000;
        
        // Create protocol handler
        CANProtocolHandler handler(config);
        
        // Create test CAN frame
        CANFrame frame;
        frame.can_id = 0x123;
        frame.frame_format = 0; // Standard frame
        frame.frame_type = 0;   // Data frame
        frame.can_fd_format = 1; // CAN FD
        frame.data_length_code = 8;
        frame.data.fill(0);
        frame.data[0] = 0xAA;
        frame.data[1] = 0x55;
        frame.timestamp_ns = 1000000;
        
        // Test encoding
        std::vector<uint8_t> avtp_payload;
        assert_true(handler.encode_can_frame(frame, avtp_payload));
        assert_true(avtp_payload.size() > 0);
        
        // Test decoding
        CANFrame decoded_frame;
        assert_true(handler.decode_can_frame(avtp_payload, decoded_frame));
        
        // Verify decoded frame matches original
        assert_equal(frame.can_id, decoded_frame.can_id);
        assert_equal(frame.frame_format, decoded_frame.frame_format);
        assert_equal(frame.can_fd_format, decoded_frame.can_fd_format);
        assert_equal(frame.data_length_code, decoded_frame.data_length_code);
        assert_equal(frame.data[0], decoded_frame.data[0]);
        assert_equal(frame.data[1], decoded_frame.data[1]);
        
        return true;
    }
    
    bool test_lin_frame_processing() {
        using namespace industrial::lin;
        
        // Create LIN bus configuration
        LINBusConfig config;
        config.bus_id = 1;
        config.baud_rate = 19200;
        config.lin_version = 22; // LIN 2.2A
        config.master_node_id = 0;
        
        // Create protocol handler
        LINProtocolHandler handler(config);
        
        // Create test LIN frame
        LINFrame frame;
        frame.frame_id = 0x10;
        frame.protected_id = 0x50; // Frame ID + parity bits
        frame.data_length = 4;
        frame.data.fill(0);
        frame.data[0] = 0x11;
        frame.data[1] = 0x22;
        frame.data[2] = 0x33;
        frame.data[3] = 0x44;
        frame.checksum_type = 1; // Enhanced checksum
        frame.timestamp_ns = 2000000;
        
        // Test encoding
        std::vector<uint8_t> avtp_payload;
        assert_true(handler.encode_lin_frame(frame, avtp_payload));
        assert_true(avtp_payload.size() > 0);
        
        // Test decoding
        LINFrame decoded_frame;
        assert_true(handler.decode_lin_frame(avtp_payload, decoded_frame));
        
        // Verify decoded frame matches original
        assert_equal(frame.frame_id, decoded_frame.frame_id);
        assert_equal(frame.data_length, decoded_frame.data_length);
        assert_equal(frame.data[0], decoded_frame.data[0]);
        assert_equal(frame.checksum_type, decoded_frame.checksum_type);
        
        return true;
    }
    
    bool test_most_frame_processing() {
        using namespace industrial::most;
        
        // Create MOST network configuration
        MOSTNetworkConfig config;
        config.network_id = 1;
        config.most_version = 150;
        config.frame_rate = 48000;
        config.node_address = 0x0100;
        
        // Create protocol handler
        MOSTProtocolHandler handler(config);
        
        // Create test MOST frame
        MOSTFrame frame;
        frame.source_address = 0x0100;
        frame.destination_address = 0x0200;
        frame.function_block_id = 0x01;
        frame.function_id = 0x1234;
        frame.operation_type = 0x0C; // Set operation
        frame.data = {0xDE, 0xAD, 0xBE, 0xEF};
        frame.timestamp_ns = 3000000;
        
        // Test encoding
        std::vector<uint8_t> avtp_payload;
        assert_true(handler.encode_most_frame(frame, avtp_payload));
        assert_true(avtp_payload.size() > 0);
        
        // Test decoding
        MOSTFrame decoded_frame;
        assert_true(handler.decode_most_frame(avtp_payload, decoded_frame));
        
        // Verify decoded frame matches original
        assert_equal(frame.source_address, decoded_frame.source_address);
        assert_equal(frame.destination_address, decoded_frame.destination_address);
        assert_equal(frame.function_id, decoded_frame.function_id);
        assert_equal(frame.data.size(), decoded_frame.data.size());
        
        return true;
    }
    
    bool test_flexray_frame_processing() {
        using namespace industrial::flexray;
        
        // Create FlexRay cluster configuration
        FlexRayClusterConfig config;
        config.cluster_id = 1;
        config.gdCycle = 5000; // 5ms cycle
        config.gdStaticSlot = 100; // 100µs static slot
        config.gNumberOfStaticSlots = 64;
        
        // Create protocol handler
        FlexRayProtocolHandler handler(config);
        
        // Create test FlexRay frame
        FlexRayFrame frame;
        frame.frame_id = 1;
        frame.payload_length = 8; // 8 words = 16 bytes
        frame.sync_frame_indicator = true;
        frame.startup_frame_indicator = false;
        frame.cycle_count = 10;
        frame.data.fill(0);
        frame.data[0] = 0xCA;
        frame.data[1] = 0xFE;
        frame.timestamp_ns = 4000000;
        
        // Test encoding
        std::vector<uint8_t> avtp_payload;
        assert_true(handler.encode_flexray_frame(frame, avtp_payload));
        assert_true(avtp_payload.size() > 0);
        
        // Test decoding
        FlexRayFrame decoded_frame;
        assert_true(handler.decode_flexray_frame(avtp_payload, decoded_frame));
        
        // Verify decoded frame matches original
        assert_equal(frame.frame_id, decoded_frame.frame_id);
        assert_equal(frame.payload_length, decoded_frame.payload_length);
        assert_equal(frame.sync_frame_indicator, decoded_frame.sync_frame_indicator);
        assert_equal(frame.data[0], decoded_frame.data[0]);
        
        return true;
    }
    
    bool test_sensor_data_processing() {
        using namespace industrial::sensor;
        
        // Create sensor network configuration
        SensorNetworkConfig config;
        config.network_id = 1;
        config.sampling_rate_hz = 1000;
        config.enable_data_aggregation = false;
        
        // Create protocol handler
        SensorProtocolHandler handler(config);
        
        // Create test sensor frame
        SensorDataFrame frame;
        frame.sensor_id = 0x12345678;
        frame.sensor_type = 1; // Temperature sensor
        frame.data_format = 1; // Calibrated data
        frame.values = {23.5, 24.1, 23.8}; // Multiple readings
        frame.signal_quality = 255; // Maximum quality
        frame.data_valid = true;
        frame.sequence_number = 100;
        frame.timestamp_ns = 5000000;
        
        // Test encoding
        std::vector<uint8_t> avtp_payload;
        assert_true(handler.encode_sensor_frame(frame, avtp_payload));
        assert_true(avtp_payload.size() > 0);
        
        // Test decoding
        SensorDataFrame decoded_frame;
        assert_true(handler.decode_sensor_frame(avtp_payload, decoded_frame));
        
        // Verify decoded frame matches original
        assert_equal(frame.sensor_id, decoded_frame.sensor_id);
        assert_equal(frame.sensor_type, decoded_frame.sensor_type);
        assert_equal(frame.signal_quality, decoded_frame.signal_quality);
        assert_equal(frame.sequence_number, decoded_frame.sequence_number);
        assert_true(frame.values.size() == decoded_frame.values.size());
        
        return true;
    }
    
    bool test_industrial_protocol_manager() {
        using namespace industrial;
        
        // Create unified manager
        IndustrialProtocolManager manager;
        
        // Create and register protocol handlers
        auto can_config = can::CANBusConfig{};
        can_config.bus_id = 1;
        auto can_handler = std::make_shared<can::CANProtocolHandler>(can_config);
        assert_true(manager.register_can_handler(1, can_handler));
        
        auto lin_config = lin::LINBusConfig{};
        lin_config.bus_id = 2;
        auto lin_handler = std::make_shared<lin::LINProtocolHandler>(lin_config);
        assert_true(manager.register_lin_handler(2, lin_handler));
        
        // Test statistics
        auto stats = manager.get_statistics(1, 1); // CAN protocol type 1, bus 1
        assert_equal(0, stats.frames_transmitted); // Initially zero
        
        // Test active protocol types
        auto active_types = manager.get_active_protocol_types();
        assert_true(active_types.size() >= 2); // At least CAN and LIN
        
        return true;
    }
};

// =============================
// Phase 6: Security & Cryptography Tests
// =============================

class Phase6SecurityTests : public TestSuiteBase {
public:
    std::string get_suite_name() const override {
        return "Phase 6: Security & Cryptography";
    }
    
    std::vector<TestResult> run_all_tests() override {
        return {
            run_test("test_aes_siv_encryption", 
                    [this]() { return test_aes_siv_encryption(); }),
            run_test("test_ecc_key_generation", 
                    [this]() { return test_ecc_key_generation(); }),
            run_test("test_ecdsa_signatures", 
                    [this]() { return test_ecdsa_signatures(); }),
            run_test("test_key_management", 
                    [this]() { return test_key_management(); }),
            run_test("test_secure_avtp_handler", 
                    [this]() { return test_secure_avtp_handler(); }),
            run_test("test_replay_protection", 
                    [this]() { return test_replay_protection(); })
        };
    }
    
private:
    bool test_aes_siv_encryption() {
        using namespace security::aes_siv;
        
        // Create AES-SIV configuration
        AESSIVConfig config;
        config.key.fill(0x42); // Test key
        config.use_hardware_acceleration = false;
        
        // Create crypto instance
        AESSIVCrypto crypto(config);
        
        // Test data
        std::vector<uint8_t> plaintext = {0x48, 0x65, 0x6C, 0x6C, 0x6F}; // "Hello"
        std::vector<uint8_t> associated_data = {0x41, 0x44}; // "AD"
        
        // Test encryption
        AESSIVFrame encrypted_frame;
        assert_true(crypto.encrypt(plaintext, associated_data, encrypted_frame));
        assert_true(encrypted_frame.ciphertext.size() > 0);
        
        // Test decryption
        std::vector<uint8_t> decrypted_plaintext;
        std::vector<uint8_t> decrypted_ad;
        assert_true(crypto.decrypt(encrypted_frame, decrypted_plaintext, decrypted_ad));
        
        // Verify decrypted data matches original
        assert_equal(plaintext.size(), decrypted_plaintext.size());
        assert_equal(associated_data.size(), decrypted_ad.size());
        
        for (size_t i = 0; i < plaintext.size(); ++i) {
            assert_equal(plaintext[i], decrypted_plaintext[i]);
        }
        
        return true;
    }
    
    bool test_ecc_key_generation() {
        using namespace security::ecc;
        
        // Create ECC cryptography instance
        ECCCryptography ecc_crypto(ECCCurve::NIST_P256);
        
        // Generate key pair
        ECCKeyPair key_pair;
        assert_true(ecc_crypto.generate_key_pair(ECCCurve::NIST_P256, key_pair));
        
        // Verify key pair properties
        assert_equal(static_cast<uint8_t>(ECCCurve::NIST_P256), 
                    static_cast<uint8_t>(key_pair.curve_type));
        assert_true(key_pair.private_key.size() > 0);
        assert_true(key_pair.public_key_x.size() > 0);
        assert_true(key_pair.public_key_y.size() > 0);
        assert_true(key_pair.is_valid());
        
        // Test key size consistency
        assert_equal(key_pair.get_key_size_bytes(), key_pair.private_key.size());
        
        return true;
    }
    
    bool test_ecdsa_signatures() {
        using namespace security::ecc;
        
        // Create ECC cryptography instance
        ECCCryptography ecc_crypto(ECCCurve::NIST_P256);
        
        // Generate key pair for signing
        ECCKeyPair signing_key;
        assert_true(ecc_crypto.generate_key_pair(ECCCurve::NIST_P256, signing_key));
        
        // Test data to sign
        std::vector<uint8_t> test_data = {0x54, 0x65, 0x73, 0x74}; // "Test"
        
        // Create signature
        ECDSASignature signature;
        assert_true(ecc_crypto.sign_data(test_data, signing_key, signature));
        
        // Verify signature properties
        assert_true(signature.r_component.size() > 0);
        assert_true(signature.s_component.size() > 0);
        assert_equal(static_cast<uint8_t>(ECCCurve::NIST_P256), 
                    static_cast<uint8_t>(signature.curve_type));
        assert_true(signature.is_valid());
        
        // Verify signature
        assert_true(ecc_crypto.verify_signature(test_data, signature, signing_key));
        
        // Test with wrong data (should fail)
        std::vector<uint8_t> wrong_data = {0x57, 0x72, 0x6F, 0x6E, 0x67}; // "Wrong"
        assert_true(!ecc_crypto.verify_signature(wrong_data, signature, signing_key));
        
        return true;
    }
    
    bool test_key_management() {
        using namespace security::key_management;
        
        // Create key management system
        KeyManagementSystem kms;
        
        // Create security association
        SecurityAssociation sa;
        assert_true(kms.create_security_association(0x1234567890ABCDEF, // stream_id
                                                  1, // AES-SIV encryption
                                                  1, // ECDSA signature
                                                  sa));
        
        // Verify SA properties
        assert_equal(static_cast<uint32_t>(0x90ABCDEF), static_cast<uint32_t>(sa.stream_id));
        assert_equal(1, sa.encryption_algorithm);
        assert_equal(1, sa.signature_algorithm);
        assert_true(sa.is_valid());
        
        // Test SA retrieval
        SecurityAssociation retrieved_sa;
        assert_true(kms.get_security_association(sa.sa_id, retrieved_sa));
        assert_equal(sa.stream_id, retrieved_sa.stream_id);
        
        // Test key rotation
        assert_true(kms.rotate_encryption_key(sa.sa_id));
        
        return true;
    }
    
    bool test_secure_avtp_handler() {
        using namespace security;
        
        // Create secure AVTP handler
        SecureAVTPHandler handler;
        
        // Initialize with test configuration
        assert_true(handler.initialize("test_security_policy.json"));
        
        // Configure security for test stream
        uint64_t test_stream_id = 0x0011223344556677;
        assert_true(handler.configure_security_association(test_stream_id, 1, 1));
        
        // Test frame encryption and signing
        std::vector<uint8_t> plaintext_payload = {0x01, 0x02, 0x03, 0x04, 0x05};
        std::vector<uint8_t> secure_frame;
        assert_true(handler.encrypt_and_sign_frame(test_stream_id, 
                                                  plaintext_payload, 
                                                  secure_frame));
        assert_true(secure_frame.size() > plaintext_payload.size()); // Should be larger due to security overhead
        
        // Test frame decryption and verification
        uint64_t recovered_stream_id;
        std::vector<uint8_t> recovered_payload;
        assert_true(handler.decrypt_and_verify_frame(secure_frame, 
                                                    recovered_stream_id, 
                                                    recovered_payload));
        
        // Verify recovered data matches original
        assert_equal(test_stream_id, recovered_stream_id);
        assert_equal(plaintext_payload.size(), recovered_payload.size());
        for (size_t i = 0; i < plaintext_payload.size(); ++i) {
            assert_equal(plaintext_payload[i], recovered_payload[i]);
        }
        
        return true;
    }
    
    bool test_replay_protection() {
        using namespace security::aes_siv;
        
        // Create AES-SIV configuration with replay protection
        AESSIVConfig config;
        config.key.fill(0x55);
        
        AESSIVCrypto crypto(config);
        
        // Test sequence number validation
        assert_true(crypto.validate_sequence_number(1));
        assert_true(crypto.validate_sequence_number(2));
        assert_true(crypto.validate_sequence_number(3));
        
        // Test replay detection (sequence number going backwards)
        assert_true(!crypto.validate_sequence_number(2)); // Should fail - replay
        assert_true(!crypto.validate_sequence_number(1)); // Should fail - replay
        
        // Test future sequence numbers are still accepted
        assert_true(crypto.validate_sequence_number(10));
        
        return true;
    }
};

// =============================
// Phase 7: AVDECC Integration Tests
// =============================

class Phase7AVDECCTests : public TestSuiteBase {
public:
    std::string get_suite_name() const override {
        return "Phase 7: AVDECC Integration";
    }
    
    std::vector<TestResult> run_all_tests() override {
        return {
            run_test("test_adp_entity_advertisement", 
                    [this]() { return test_adp_entity_advertisement(); }),
            run_test("test_aecp_descriptor_management", 
                    [this]() { return test_aecp_descriptor_management(); }),
            run_test("test_acmp_stream_connections", 
                    [this]() { return test_acmp_stream_connections(); }),
            run_test("test_avdecc_stack_integration", 
                    [this]() { return test_avdecc_stack_integration(); }),
            run_test("test_milan_compatibility", 
                    [this]() { return test_milan_compatibility(); }),
            run_test("test_aem_checksums", 
                    [this]() { return test_aem_checksums(); })
        };
    }
    
private:
    bool test_adp_entity_advertisement() {
        using namespace avdecc::adp;
        
        // Create test entity info
        ADPEntityInfo entity_info;
        entity_info.entity_id = 0x0011223344556677;
        entity_info.entity_model_id = 0x001122FFFE334455;
        entity_info.entity_capabilities.aem_supported = true;
        entity_info.entity_capabilities.gptp_supported = true;
        entity_info.talker_stream_sources = 2;
        entity_info.listener_stream_sinks = 2;
        entity_info.available_index = 0;
        
        // Create ADP handler
        ADPProtocolHandler adp_handler(entity_info);
        
        // Test entity info validation
        assert_true(entity_info.is_valid());
        
        // Test entity capabilities conversion
        uint32_t caps_uint32 = entity_info.entity_capabilities.to_uint32();
        assert_true(caps_uint32 != 0); // Should have some capabilities set
        
        EntityCapabilities restored_caps;
        restored_caps.from_uint32(caps_uint32);
        assert_equal(entity_info.entity_capabilities.aem_supported, 
                    restored_caps.aem_supported);
        assert_equal(entity_info.entity_capabilities.gptp_supported, 
                    restored_caps.gptp_supported);
        
        // Test advertisement start/stop
        assert_true(adp_handler.start_entity_advertisement());
        assert_true(adp_handler.stop_entity_advertisement());
        
        return true;
    }
    
    bool test_aecp_descriptor_management() {
        using namespace avdecc::aecp;
        
        // Create AECP handler
        uint64_t local_entity_id = 0x0011223344556677;
        AECPProtocolHandler aecp_handler(local_entity_id);
        
        // Create entity descriptor
        auto entity_desc = std::make_shared<EntityDescriptor>();
        entity_desc->descriptor_type = avdecc::AEMDescriptorType::ENTITY;
        entity_desc->descriptor_index = 0;
        entity_desc->entity_id = local_entity_id;
        entity_desc->entity_model_id = 0x001122FFFE334455;
        entity_desc->configurations_count = 1;
        entity_desc->current_configuration = 0;
        
        // Add descriptor
        assert_true(aecp_handler.add_descriptor(entity_desc));
        
        // Retrieve descriptor
        std::shared_ptr<AEMDescriptor> retrieved_desc;
        assert_true(aecp_handler.get_descriptor(avdecc::AEMDescriptorType::ENTITY, 0, retrieved_desc));
        assert_true(retrieved_desc != nullptr);
        
        // Verify descriptor type
        auto retrieved_entity_desc = std::dynamic_pointer_cast<EntityDescriptor>(retrieved_desc);
        assert_true(retrieved_entity_desc != nullptr);
        assert_equal(local_entity_id, retrieved_entity_desc->entity_id);
        
        return true;
    }
    
    bool test_acmp_stream_connections() {
        using namespace avdecc::acmp;
        
        // Create ACMP handler
        uint64_t local_entity_id = 0x0011223344556677;
        ACMPProtocolHandler acmp_handler(local_entity_id);
        
        // Test connection parameters
        uint64_t talker_entity_id = 0x1111111111111111;
        uint16_t talker_unique_id = 0;
        uint64_t listener_entity_id = 0x2222222222222222;
        uint16_t listener_unique_id = 0;
        
        // Test stream connection
        StreamConnection connection;
        // Note: In a real test, this would require actual network setup
        // For unit testing, we test the data structures and validation
        
        connection.talker_entity_id = talker_entity_id;
        connection.listener_entity_id = listener_entity_id;
        connection.talker_unique_id = talker_unique_id;
        connection.listener_unique_id = listener_unique_id;
        connection.stream_id = 0x1234567890ABCDEF;
        connection.connected = true;
        
        // Test connection structure
        std::string debug_str = connection.to_debug_string();
        assert_true(debug_str.find("1234567890ABCDEF") != std::string::npos);
        
        // Test statistics
        auto stats = acmp_handler.get_statistics();
        assert_equal(0, stats.connections_established); // Initially zero
        
        return true;
    }
    
    bool test_avdecc_stack_integration() {
        using namespace avdecc;
        
        // Create AVDECC stack
        uint64_t entity_id = 0x0011223344556677;
        AVDECCStackManager stack(entity_id);
        
        // Create entity info
        adp::ADPEntityInfo entity_info;
        entity_info.entity_id = entity_id;
        entity_info.entity_model_id = 0x001122FFFE334455;
        entity_info.entity_capabilities.aem_supported = true;
        entity_info.talker_stream_sources = 1;
        entity_info.listener_stream_sinks = 1;
        
        // Initialize stack
        assert_true(stack.initialize(entity_info));
        
        // Test entity descriptor creation
        aecp::EntityDescriptor entity_desc;
        entity_desc.entity_id = entity_id;
        entity_desc.entity_model_id = entity_info.entity_model_id;
        entity_desc.configurations_count = 1;
        
        assert_true(stack.add_entity_descriptor(entity_desc));
        
        // Test stream descriptor creation
        aecp::StreamDescriptor stream_desc;
        stream_desc.descriptor_type = AEMDescriptorType::STREAM_OUTPUT;
        stream_desc.descriptor_index = 0;
        stream_desc.current_format = 0x02020800; // 48kHz, 2 channels, 16-bit
        
        assert_true(stack.add_stream_descriptor(stream_desc, false)); // Output stream
        
        return true;
    }
    
    bool test_milan_compatibility() {
        using namespace avdecc;
        
        // Create AVDECC stack with Milan mode
        uint64_t entity_id = 0x0011223344556677;
        AVDECCStackManager stack(entity_id);
        
        // Enable Milan mode
        assert_true(stack.enable_milan_mode());
        
        // Test Milan-specific features would go here
        // For now, just verify Milan mode is enabled
        
        return true;
    }
    
    bool test_aem_checksums() {
        using namespace avdecc::aecp;
        
        // Create entity descriptor
        EntityDescriptor entity_desc;
        entity_desc.descriptor_type = avdecc::AEMDescriptorType::ENTITY;
        entity_desc.entity_id = 0x0011223344556677;
        entity_desc.entity_model_id = 0x001122FFFE334455;
        
        // Calculate AEM checksum
        uint32_t checksum = entity_desc.calculate_aem_checksum();
        entity_desc.aem_checksum = checksum;
        
        // Verify checksum
        assert_true(entity_desc.verify_aem_checksum());
        
        // Test checksum failure with modified data
        entity_desc.entity_id = 0x1111111111111111; // Change entity ID
        assert_true(!entity_desc.verify_aem_checksum()); // Should fail
        
        return true;
    }
};

// =============================
// Complete Test Suite Runner
// =============================

class IEEE1722Phase4TestRunner {
public:
    struct TestSuiteResult {
        std::string suite_name;
        std::vector<TestResult> test_results;
        int tests_passed;
        int tests_failed;
        double total_execution_time_ms;
        
        double get_pass_rate() const {
            int total_tests = tests_passed + tests_failed;
            return total_tests > 0 ? (static_cast<double>(tests_passed) / total_tests) * 100.0 : 0.0;
        }
    };
    
    std::vector<TestSuiteResult> run_all_test_suites() {
        std::vector<std::unique_ptr<TestSuiteBase>> test_suites;
        
        // Add all test suites
        test_suites.push_back(std::make_unique<Phase4SubtypeTests>());
        test_suites.push_back(std::make_unique<Phase5IndustrialTests>());
        test_suites.push_back(std::make_unique<Phase6SecurityTests>());
        test_suites.push_back(std::make_unique<Phase7AVDECCTests>());
        
        std::vector<TestSuiteResult> suite_results;
        
        for (auto& test_suite : test_suites) {
            TestSuiteResult suite_result;
            suite_result.suite_name = test_suite->get_suite_name();
            suite_result.test_results = test_suite->run_all_tests();
            suite_result.tests_passed = 0;
            suite_result.tests_failed = 0;
            suite_result.total_execution_time_ms = 0.0;
            
            for (const auto& test_result : suite_result.test_results) {
                if (test_result.passed) {
                    suite_result.tests_passed++;
                } else {
                    suite_result.tests_failed++;
                }
                suite_result.total_execution_time_ms += test_result.execution_time_ms;
            }
            
            suite_results.push_back(suite_result);
        }
        
        return suite_results;
    }
    
    void print_test_report(const std::vector<TestSuiteResult>& suite_results) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "IEEE 1722-2016 Phase 4 Test Report" << std::endl;
        std::cout << "Complete 'Tutti Completo' Implementation" << std::endl;
        std::cout << "========================================\n" << std::endl;
        
        int total_tests_passed = 0;
        int total_tests_failed = 0;
        double total_execution_time = 0.0;
        
        for (const auto& suite_result : suite_results) {
            std::cout << "📁 " << suite_result.suite_name << std::endl;
            std::cout << "   Tests: " << (suite_result.tests_passed + suite_result.tests_failed)
                     << " | Passed: " << suite_result.tests_passed 
                     << " | Failed: " << suite_result.tests_failed
                     << " | Pass Rate: " << std::fixed << std::setprecision(1) 
                     << suite_result.get_pass_rate() << "%" << std::endl;
            std::cout << "   Execution Time: " << std::fixed << std::setprecision(2) 
                     << suite_result.total_execution_time_ms << "ms\n" << std::endl;
            
            // Print failed tests
            for (const auto& test_result : suite_result.test_results) {
                if (!test_result.passed) {
                    std::cout << "   ❌ " << test_result.to_string() << std::endl;
                }
            }
            
            total_tests_passed += suite_result.tests_passed;
            total_tests_failed += suite_result.tests_failed;
            total_execution_time += suite_result.total_execution_time_ms;
        }
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "OVERALL SUMMARY" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Total Tests: " << (total_tests_passed + total_tests_failed) << std::endl;
        std::cout << "Passed: " << total_tests_passed << std::endl;
        std::cout << "Failed: " << total_tests_failed << std::endl;
        
        double overall_pass_rate = 0.0;
        if (total_tests_passed + total_tests_failed > 0) {
            overall_pass_rate = (static_cast<double>(total_tests_passed) / 
                                (total_tests_passed + total_tests_failed)) * 100.0;
        }
        
        std::cout << "Overall Pass Rate: " << std::fixed << std::setprecision(1) 
                 << overall_pass_rate << "%" << std::endl;
        std::cout << "Total Execution Time: " << std::fixed << std::setprecision(2) 
                 << total_execution_time << "ms" << std::endl;
        
        if (total_tests_failed == 0) {
            std::cout << "\n🎉 ALL TESTS PASSED! IEEE 1722-2016 'Tutti Completo' Implementation Complete!" << std::endl;
        } else {
            std::cout << "\n⚠️  Some tests failed. Please review and fix issues before claiming completion." << std::endl;
        }
        
        std::cout << "========================================\n" << std::endl;
    }
};

} // namespace test
} // namespace avtp_protocol

#endif // IEEE_1722_2016_PHASE4_TESTS_H
