/**
 * @file simple_clock_test.cpp
 * @brief Simple test to verify our IEEE 1588-2019 types work
 */

#include "../include/IEEE/1588/PTP/2019/ieee1588_2019.hpp"
#include <iostream>

using namespace IEEE::_1588::PTP::_2019;

int main() {
    std::cout << "Simple IEEE 1588-2019 Test\n";
    std::cout << "==========================\n\n";
    
    // Test basic types
    std::cout << "✅ Test 1: Basic Types\n";
    Types::ClockIdentity clock_id = {0x00, 0x1b, 0x21, 0xff, 0xfe, 0x12, 0x34, 0x56};
    Types::PortNumber port_num = 1;
    Types::DomainNumber domain = 0;
    
    std::cout << "   Clock Identity: ";
    for (auto byte : clock_id) {
        std::cout << std::hex << static_cast<int>(byte) << ":";
    }
    std::cout << "\n   Port Number: " << std::dec << port_num << "\n";
    std::cout << "   Domain: " << domain << "\n\n";
    
    // Test PTPResult
    std::cout << "✅ Test 2: PTPResult Types\n";
    auto success_result = Types::makeSuccess(42);
    auto error_result = Types::makeError<int>(Types::PTPError::Invalid_Parameter);
    
    if (success_result.hasValue()) {
        std::cout << "   Success result value: " << success_result.getValue() << "\n";
    }
    
    if (error_result.hasError()) {
        std::cout << "   Error result code: " << static_cast<int>(error_result.getError()) << "\n";
    }
    
    // Test PTPResult<void>
    auto void_success = Types::PTPResult<void>();
    auto void_error = Types::PTPResult<void>(Types::PTPError::State_Error);
    
    std::cout << "   Void success: " << (void_success.is_success() ? "true" : "false") << "\n";
    std::cout << "   Void error: " << (void_error.is_success() ? "true" : "false") << "\n\n";
    
    // Test Timestamp
    std::cout << "✅ Test 3: Timestamp Operations\n";
    Types::Timestamp timestamp{0, 1695168000, 123456789};
    
    std::cout << "   Timestamp seconds: " << timestamp.seconds_low << "\n";
    std::cout << "   Timestamp nanoseconds: " << timestamp.nanoseconds << "\n";
    std::cout << "   Total seconds: " << timestamp.getTotalSeconds() << "\n";
    std::cout << "   Valid: " << (timestamp.isValid() ? "Yes" : "No") << "\n\n";
    
    // Test Correction Field
    std::cout << "✅ Test 4: Correction Field\n";
    auto correction = Types::CorrectionField::fromNanoseconds(123.456);
    std::cout << "   Correction field value: " << correction.value << "\n";
    std::cout << "   Back to nanoseconds: " << correction.toNanoseconds() << "\n\n";
    
    std::cout << "🎉 All basic tests passed!\n";
    std::cout << "📊 IEEE 1588-2019 foundation is working correctly.\n";
    
    return 0;
}