/**
 * @file realtime_types_example.cpp
 * @brief Time-sensitive design example for IEEE 1588-2019 types
 * 
 * This example demonstrates how to use IEEE 1588-2019 types following
 * time-sensitive design principles suitable for predictable timing.
 * 
 * TIME-SENSITIVE DESIGN PRINCIPLES DEMONSTRATED:
 * - No dynamic memory allocation
 * - No exceptions or blocking calls  
 * - Deterministic execution time (O(1) operations)
 * - Error handling via result types
 * - Designed for time-critical applications
 * 
 * @copyright
 * This example demonstrates IEEE 1588-2019 standard compliance
 * with time-sensitive design principles.
 * 
 * @version 1.0.0
 * @date 2025-09-20
 */

#include "../include/IEEE/1588/PTP/2019/ieee1588_2019.hpp"
#include <iostream>
#include <cassert>

using namespace IEEE::_1588::PTP::_2019::Types;

/**
 * @brief Demonstrate time-sensitive PTP timestamp operations
 * @note All operations have bounded execution time - suitable for time-critical applications
 */
void demonstrateTimestampOperations() {
    std::cout << "\n=== Time-Sensitive PTP Timestamp Operations ===\n";
    
    // Create timestamp (O(1) operation)
    Timestamp ts;
    ts.seconds_high = 0;
    ts.seconds_low = 1695168000; // Example: Sept 2023
    ts.nanoseconds = 123456789;
    
    // Validate timestamp (O(1) check - deterministic)
    if (ts.isValid()) {
        std::cout << "Timestamp is valid\n";
        std::cout << "Total seconds: " << ts.getTotalSeconds() << "\n";
        std::cout << "Nanoseconds: " << ts.nanoseconds << "\n";
    } else {
        std::cout << "Invalid timestamp detected\n";
    }
    
    // Demonstrate 48-bit seconds handling (O(1) operations)
    UInteger64 total_seconds = 0x123456789ABCULL;
    ts.setTotalSeconds(total_seconds);
    assert(ts.getTotalSeconds() == total_seconds);
    std::cout << "48-bit seconds handling verified\n";
}

/**
 * @brief Demonstrate time-sensitive correction field operations
 */
void demonstrateCorrectionField() {
    std::cout << "\n=== Time-Sensitive Correction Field Operations ===\n";
    
    // All operations are constexpr and O(1) - deterministic
    constexpr double nanoseconds = 1234.5;
    constexpr auto correction = CorrectionField::fromNanoseconds(nanoseconds);
    constexpr double recovered = correction.toNanoseconds();
    
    std::cout << "Original: " << nanoseconds << " ns\n";
    std::cout << "Scaled value: " << correction.value << "\n";
    std::cout << "Recovered: " << recovered << " ns\n";
    
    // Verify precision (compile-time check possible)
    static_assert(correction.value != 0, "Correction field conversion failed");
}

/**
 * @brief Demonstrate time-sensitive port identity operations
 */
void demonstratePortIdentity() {
    std::cout << "\n=== Time-Sensitive Port Identity Operations ===\n";
    
    // Create port identities (O(1) operations)
    PortIdentity port1;
    port1.clock_identity = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    port1.port_number = 1;
    
    PortIdentity port2;
    port2.clock_identity = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    port2.port_number = 2;
    
    // Comparison operations are O(1) - deterministic
    if (port1 == port2) {
        std::cout << "Port identities are equal\n";
    } else {
        std::cout << "Port identities are different\n";
    }
    
    // Ordering for collections (O(1) comparison)
    if (port1 < port2) {
        std::cout << "port1 < port2\n";
    } else {
        std::cout << "port1 >= port2\n";
    }
}

/**
 * @brief Demonstrate deterministic error handling
 */
void demonstrateErrorHandling() {
    std::cout << "\n=== Deterministic Error Handling ===\n";
    
    // Function that might fail - returns result instead of throwing
    auto validateTimestamp = [](const Timestamp& ts) -> PTPResult<bool> {
        if (!ts.isValid()) {
            return makeError<bool>(PTPError::Invalid_Timestamp);
        }
        return makeSuccess(true);
    };
    
    // Test with valid timestamp
    Timestamp valid_ts{0, 1695168000, 123456789};
    auto result1 = validateTimestamp(valid_ts);
    if (result1.hasValue()) {
        std::cout << "Timestamp validation successful\n";
    } else {
        std::cout << "Timestamp validation failed: " 
                  << static_cast<int>(result1.getError()) << "\n";
    }
    
    // Test with invalid timestamp
    Timestamp invalid_ts{0, 1695168000, 2000000000}; // Invalid nanoseconds
    auto result2 = validateTimestamp(invalid_ts);
    if (result2.hasError()) {
        std::cout << "Invalid timestamp correctly detected, error code: "
                  << static_cast<int>(result2.getError()) << "\n";
    }
    
    // Safe value extraction with default
    bool is_valid = result2.getValueOr(false);
    std::cout << "Safe value extraction result: " << is_valid << "\n";
}

/**
 * @brief Demonstrate time-sensitive time interval operations
 */
void demonstrateTimeInterval() {
    std::cout << "\n=== Time-Sensitive Time Interval Operations ===\n";
    
    // All operations are constexpr and O(1)
    constexpr double interval_ns = 500.25;
    constexpr auto time_interval = TimeInterval::fromNanoseconds(interval_ns);
    constexpr double recovered_ns = time_interval.toNanoseconds();
    
    std::cout << "Original interval: " << interval_ns << " ns\n";
    std::cout << "Scaled representation: " << time_interval.scaled_nanoseconds << "\n";
    std::cout << "Recovered interval: " << recovered_ns << " ns\n";
}

/**
 * @brief Demonstrate enum usage in time-sensitive context
 */
void demonstrateEnums() {
    std::cout << "\n=== Time-Sensitive Enum Operations ===\n";
    
    // Enums are POD types with O(1) operations
    MessageType msg_type = MessageType::Sync;
    PortState port_state = PortState::Master;
    TimeSource time_source = TimeSource::GPS;
    
    std::cout << "Message type (Sync): " << static_cast<int>(msg_type) << "\n";
    std::cout << "Port state (Master): " << static_cast<int>(port_state) << "\n";
    std::cout << "Time source (GPS): " << static_cast<int>(time_source) << "\n";
    
    // Switch statements are O(1) and deterministic
    switch (msg_type) {
        case MessageType::Sync:
            std::cout << "Processing Sync message (time-critical path)\n";
            break;
        case MessageType::Announce:
            std::cout << "Processing Announce message\n";
            break;
        default:
            std::cout << "Unknown message type\n";
            break;
    }
}

int main() {
    std::cout << "IEEE 1588-2019 Time-Sensitive Design Example\n";
    std::cout << "============================================\n";
    
    // All demonstrations use time-sensitive design principles:
    // - No dynamic allocation
    // - No exceptions
    // - Bounded execution time
    // - Deterministic behavior
    
    demonstrateTimestampOperations();
    demonstrateCorrectionField();
    demonstratePortIdentity();
    demonstrateErrorHandling();
    demonstrateTimeInterval();
    demonstrateEnums();
    
    std::cout << "\n=== Time-Sensitive Design Summary ===\n";
    std::cout << "✓ All types are POD (Plain Old Data)\n";
    std::cout << "✓ No dynamic memory allocation\n";
    std::cout << "✓ No exceptions - error codes used instead\n";
    std::cout << "✓ All operations have O(1) bounded execution time\n";
    std::cout << "✓ No blocking calls or undefined timing behavior\n";
    std::cout << "✓ Designed for time-critical applications\n";
    std::cout << "✓ All critical operations are constexpr/noexcept\n";
    
    std::cout << "\n=== Constants (Compile-time determined) ===\n";
    std::cout << "Default domain: " << static_cast<int>(DEFAULT_DOMAIN) << "\n";
    std::cout << "Clock identity length: " << CLOCK_IDENTITY_LENGTH << " bytes\n";
    std::cout << "Port identity length: " << PORT_IDENTITY_LENGTH << " bytes\n";
    std::cout << "PTP Sync message length: " << PTP_SYNC_MESSAGE_LENGTH << " bytes\n";
    
    return 0;
}