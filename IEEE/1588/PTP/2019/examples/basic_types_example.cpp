/**
 * @file basic_types_example.cpp
 * @brief Basic usage example for IEEE 1588-2019 types
 * 
 * This example demonstrates the fundamental data types and their usage
 * in the IEEE 1588-2019 PTP implementation.
 * 
 * @copyright
 * This example demonstrates IEEE 1588-2019 standard compliance.
 * 
 * @version 1.0.0
 * @date 2025-09-20
 */

#include <IEEE/1588/PTP/2019/ieee1588_2019.hpp>
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "IEEE 1588-2019 PTP v2.1 Basic Types Example\n";
    std::cout << "============================================\n\n";

    // Demonstrate ClockIdentity usage
    IEEE::_1588::PTP::_2019::Types::ClockIdentity clock_id = {
        0x00, 0x1B, 0x21, 0xFF, 0xFE, 0x12, 0x34, 0x56
    };
    
    std::cout << "Clock Identity: ";
    for (size_t i = 0; i < clock_id.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') 
                  << static_cast<unsigned>(clock_id[i]);
        if (i < clock_id.size() - 1) std::cout << ":";
    }
    std::cout << std::dec << "\n\n";

    // Demonstrate PortIdentity usage
    IEEE::_1588::PTP::_2019::Types::PortIdentity port_id = {clock_id, 1};
    std::cout << "Port Number: " << port_id.port_number << "\n\n";

    // Demonstrate Timestamp usage
    IEEE::_1588::PTP::_2019::Types::Timestamp ptp_timestamp;
    ptp_timestamp.seconds_high = 0;
    ptp_timestamp.seconds_low = 1695168000; // Sept 2023
    ptp_timestamp.nanoseconds = 123456789;
    
    std::cout << "PTP Timestamp:\n";
    std::cout << "  Seconds: " << ptp_timestamp.seconds_low << "\n";
    std::cout << "  Nanoseconds: " << ptp_timestamp.nanoseconds << "\n";
    std::cout << "  Total seconds (48-bit): " << ptp_timestamp.getTotalSeconds() << "\n";
    std::cout << "  Valid: " << (ptp_timestamp.isValid() ? "Yes" : "No") << "\n\n";

    // Demonstrate CorrectionField usage
    IEEE::_1588::PTP::_2019::Types::CorrectionField correction = 
        IEEE::_1588::PTP::_2019::Types::CorrectionField::fromNanoseconds(123.456);
    
    std::cout << "Correction Field:\n";
    std::cout << "  Raw value: " << correction.value << "\n";
    std::cout << "  Nanoseconds: " << correction.toNanoseconds() << "\n\n";

    // Demonstrate TimeInterval usage
    IEEE::_1588::PTP::_2019::Types::TimeInterval interval = 
        IEEE::_1588::PTP::_2019::Types::TimeInterval::fromNanoseconds(1000.0);
    
    std::cout << "Time Interval:\n";
    std::cout << "  Raw value: " << interval.scaled_nanoseconds << "\n";
    std::cout << "  Nanoseconds: " << interval.toNanoseconds() << "\n\n";

    // Demonstrate enumerations
    std::cout << "Message Types:\n";
    std::cout << "  Sync: " << static_cast<int>(IEEE::_1588::PTP::_2019::Types::MessageType::Sync) << "\n";
    std::cout << "  Announce: " << static_cast<int>(IEEE::_1588::PTP::_2019::Types::MessageType::Announce) << "\n";
    std::cout << "  Delay_Req: " << static_cast<int>(IEEE::_1588::PTP::_2019::Types::MessageType::Delay_Req) << "\n\n";

    std::cout << "Port States:\n";
    std::cout << "  Initializing: " << static_cast<int>(IEEE::_1588::PTP::_2019::Types::PortState::Initializing) << "\n";
    std::cout << "  Master: " << static_cast<int>(IEEE::_1588::PTP::_2019::Types::PortState::Master) << "\n";
    std::cout << "  Slave: " << static_cast<int>(IEEE::_1588::PTP::_2019::Types::PortState::Slave) << "\n\n";

    // Demonstrate ClockQuality
    IEEE::_1588::PTP::_2019::Types::ClockQuality quality = {
        6,      // clock_class (application-specific time, traceable to UTC)
        254,    // clock_accuracy (unknown)
        0xFFFF  // offset_scaled_log_variance (not specified)
    };
    
    std::cout << "Clock Quality:\n";
    std::cout << "  Class: " << static_cast<int>(quality.clock_class) << "\n";
    std::cout << "  Accuracy: " << static_cast<int>(quality.clock_accuracy) << "\n";
    std::cout << "  Scaled Log Variance: 0x" << std::hex << quality.offset_scaled_log_variance << std::dec << "\n\n";

    std::cout << "IEEE 1588-2019 types demonstration completed successfully!\n";
    
    return 0;
}