/**
 * @file milan_1_2_2023_test.cpp
 * @brief Test suite for Milan v1.2-2023 implementation
 */

#include "AVnu_Milan_1_2-2023_complete.h"
#include <iostream>
#include <cassert>
#include <iomanip>

using namespace AVnu::Milan::_1_2_2023;

class MilanTestSuite {
private:
    int tests_passed = 0;
    int tests_failed = 0;
    
    void log_test_start(const std::string& test_name) {
        std::cout << "🧪 " << test_name << " ... ";
    }
    
    void log_test_result(bool passed, const std::string& error = "") {
        if (passed) {
            std::cout << "✅ PASS" << std::endl;
            tests_passed++;
        } else {
            std::cout << "❌ FAIL";
            if (!error.empty()) {
                std::cout << " - " << error;
            }
            std::cout << std::endl;
            tests_failed++;
        }
    }
    
public:
    void test_milan_constants() {
        log_test_start("Milan Constants");
        
        bool valid = (MILAN_MAJOR_VERSION == 1) &&
                    (MILAN_MINOR_VERSION == 2) &&
                    (MILAN_MVU_PROTOCOL_ID == 0x90E0F000FE00ULL);
        
        log_test_result(valid);
    }
    
    void test_get_milan_info_command() {
        log_test_start("GetMilanInfoCommand Serialization");
        
        try {
            GetMilanInfoCommand cmd;
            std::vector<uint8_t> data;
            
            bool valid = cmd.serialize(data) && !data.empty();
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_milan_paad_entity() {
        log_test_start("MilanPAADEntity Creation");
        
        try {
            uint64_t entity_id = 0x123456789ABCDEF0ULL;
            uint64_t model_id = 0x001122334455667ULL;
            
            MilanPAADEntity entity(entity_id, model_id);
            
            bool valid = (entity.get_system_unique_id() == entity_id) &&
                        (entity.get_milan_capabilities() == static_cast<uint32_t>(MilanCapabilityFlags::BASELINE_INTEROPERABILITY));
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_milan_stream_formats() {
        log_test_start("Milan Stream Format Validation");
        
        try {
            // Test known valid Milan formats
            bool valid = MilanStreamFormatValidator::is_valid_milan_format(
                static_cast<uint64_t>(MilanBaseAudioFormat::AAF_PCM_24BIT_48KHZ_2CH));
            
            valid &= MilanStreamFormatValidator::is_valid_milan_format(
                static_cast<uint64_t>(MilanBaseAudioFormat::AAF_PCM_32BIT_48KHZ_8CH));
            
            // Test invalid format
            valid &= !MilanStreamFormatValidator::is_valid_milan_format(0x1234567890ABCDEFULL);
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_milan_capabilities() {
        log_test_start("Milan Capability Management");
        
        try {
            MilanPAADEntity entity(0x123456789ABCDEF0ULL, 0x001122334455667ULL);
            
            // Test setting capabilities
            uint32_t capabilities = static_cast<uint32_t>(MilanCapabilityFlags::BASELINE_INTEROPERABILITY) |
                                  static_cast<uint32_t>(MilanCapabilityFlags::MEDIA_CLOCKING);
            
            entity.set_milan_capabilities(capabilities);
            bool valid = (entity.get_milan_capabilities() == capabilities);
            
            // Test setting features
            uint16_t features = static_cast<uint16_t>(MilanProtocolFeatures::MILAN_BASELINE) |
                               static_cast<uint16_t>(MilanProtocolFeatures::MEDIA_CLOCK_MANAGEMENT);
            
            entity.set_milan_features(features);
            valid &= (entity.get_milan_features() == features);
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_system_unique_id() {
        log_test_start("System Unique ID Management");
        
        try {
            MilanPAADEntity entity(0x123456789ABCDEF0ULL, 0x001122334455667ULL);
            
            uint64_t new_uid = 0xFEDCBA0987654321ULL;
            entity.set_system_unique_id(new_uid);
            
            bool valid = (entity.get_system_unique_id() == new_uid);
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_milan_version_info() {
        log_test_start("Milan Version Information");
        
        try {
            uint32_t version_info = MilanPAADEntity::get_milan_version_info();
            std::string version_string = MilanPAADEntity::get_milan_version_string();
            
            bool valid = (version_info == MILAN_VERSION_INFO) &&
                        (!version_string.empty());
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_media_clock_reference() {
        log_test_start("Media Clock Reference Management");
        
        try {
            MilanPAADEntity entity(0x123456789ABCDEF0ULL, 0x001122334455667ULL);
            
            MediaClockReferenceInfo ref_info;
            ref_info.reference_type = MediaClockReferenceType::STREAM_INPUT;
            ref_info.reference_descriptor_index = 1;
            ref_info.nominal_sample_rate = 48000;
            ref_info.locked_status = 1;
            
            bool valid = entity.add_media_clock_reference(0, ref_info);
            
            MediaClockReferenceInfo retrieved_info;
            valid &= entity.get_media_clock_reference(0, retrieved_info);
            valid &= (retrieved_info.reference_type == MediaClockReferenceType::STREAM_INPUT);
            valid &= (retrieved_info.nominal_sample_rate == 48000);
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_redundant_streams() {
        log_test_start("Redundant Stream Management");
        
        try {
            MilanPAADEntity entity(0x123456789ABCDEF0ULL, 0x001122334455667ULL);
            
            RedundantStreamConfig config;
            config.primary_stream_id = 0x1111222233334444ULL;
            config.secondary_stream_id = 0x5555666677778888ULL;
            config.switchover_timeout_ms = 50;
            
            bool valid = entity.add_redundant_stream(config);
            
            auto streams = entity.get_redundant_streams();
            valid &= (streams.size() == 1);
            valid &= (streams[0].primary_stream_id == 0x1111222233334444ULL);
            
            valid &= entity.remove_redundant_stream(0x1111222233334444ULL);
            streams = entity.get_redundant_streams();
            valid &= (streams.size() == 0);
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_milan_protocol_stack_manager() {
        log_test_start("Milan Protocol Stack Manager");
        
        try {
            MilanProtocolStackManager manager;
            
            bool valid = manager.initialize(0x123456789ABCDEF0ULL, 0x001122334455667ULL);
            valid &= manager.is_initialized();

            valid &= manager.enable_milan_mode(
                static_cast<uint32_t>(MilanCapabilityFlags::BASELINE_INTEROPERABILITY),
                static_cast<uint16_t>(MilanProtocolFeatures::MILAN_BASELINE)
            );
            valid &= manager.is_milan_mode_enabled();

            auto* entity = manager.get_milan_entity();
            valid &= (entity != nullptr);
            
            // Set device name to make entity compliant
            if (entity) {
                entity->set_device_name("Test Milan Protocol Stack Manager");
            }

            valid &= manager.validate_milan_setup();

            valid &= manager.shutdown();
            valid &= !manager.is_initialized();

            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_milan_compliance_validation() {
        log_test_start("Milan Compliance Validation");
        
        try {
            MilanPAADEntity entity(0x123456789ABCDEF0ULL, 0x001122334455667ULL);
            
            // Initially should not be compliant (no device name)
            bool valid = !entity.validate_milan_compliance();
            
            // Set device name
            entity.set_device_name("Milan Test Device");
            
            // Now should be compliant
            valid &= entity.validate_milan_compliance();
            
            auto issues = entity.get_compliance_issues();
            valid &= issues.empty();
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void test_milan_utilities() {
        log_test_start("Milan Utility Functions");
        
        try {
            using namespace MilanUtils;
            
            bool valid = !get_milan_version_string().empty();
            valid &= (get_milan_version_info() == MILAN_VERSION_INFO);
            valid &= is_milan_version_compatible(1, 2);
            valid &= !is_milan_version_compatible(2, 0);
            
            uint32_t capabilities = static_cast<uint32_t>(MilanCapabilityFlags::BASELINE_INTEROPERABILITY) |
                                  static_cast<uint32_t>(MilanCapabilityFlags::MEDIA_CLOCKING);
            
            valid &= has_milan_capability(capabilities, MilanCapabilityFlags::BASELINE_INTEROPERABILITY);
            valid &= has_milan_capability(capabilities, MilanCapabilityFlags::MEDIA_CLOCKING);
            valid &= !has_milan_capability(capabilities, MilanCapabilityFlags::SEAMLESS_REDUNDANCY);
            
            std::string cap_str = capabilities_to_string(capabilities);
            valid &= !cap_str.empty();
            
            uint16_t features = static_cast<uint16_t>(MilanProtocolFeatures::MILAN_BASELINE);
            std::string feat_str = features_to_string(features);
            valid &= !feat_str.empty();
            
            valid &= is_milan_stream_format(static_cast<uint64_t>(MilanBaseAudioFormat::AAF_PCM_24BIT_48KHZ_2CH));
            valid &= !is_milan_stream_format(0x1234567890ABCDEFULL);
            
            auto tools = get_professional_tool_compatibility();
            valid &= !tools.empty();
            
            log_test_result(valid);
        } catch (const std::exception& e) {
            log_test_result(false, e.what());
        }
    }
    
    void run_all_tests() {
        std::cout << "========================================" << std::endl;
        std::cout << "Milan v1.2-2023 Implementation Test Suite" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Testing AVnu Alliance Milan Specification v1.2 (November 2023)" << std::endl;
        std::cout << "Professional Audio AVB Device (PAAD) compliance validation" << std::endl;
        
        test_milan_constants();
        test_get_milan_info_command();
        test_milan_paad_entity();
        test_milan_stream_formats();
        test_milan_capabilities();
        test_system_unique_id();
        test_milan_version_info();
        test_media_clock_reference();
        test_redundant_streams();
        test_milan_protocol_stack_manager();
        test_milan_compliance_validation();
        test_milan_utilities();
        
        std::cout << "========================================" << std::endl;
        std::cout << "TEST RESULTS SUMMARY" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "✅ Tests Passed: " << tests_passed << std::endl;
        std::cout << "❌ Tests Failed: " << tests_failed << std::endl;
        std::cout << "📊 Pass Rate: " << std::fixed << std::setprecision(1) 
                  << (100.0 * tests_passed / (tests_passed + tests_failed)) << "%" << std::endl;
        
        if (tests_failed == 0) {
            std::cout << "🎉 ALL TESTS PASSED! Milan v1.2-2023 implementation is functional!" << std::endl;
        } else {
            std::cout << "⚠️  Some tests failed. Milan implementation needs attention." << std::endl;
        }
    }
};

int main() {
    MilanTestSuite test_suite;
    test_suite.run_all_tests();
    return 0;
}
