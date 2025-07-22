/**
 * @file ieee_802_1as_2021_edge_case_tests.h
 * @brief IEEE 802.1AS-2021 Edge Case Test Framework
 * @details Comprehensive test framework covering all IEEE 802.1AS-2021 edge cases and gap closure
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Complete edge case validation for IEEE 802.1AS-2021
 */

#ifndef IEEE_802_1AS_2021_EDGE_CASE_TESTS_H
#define IEEE_802_1AS_2021_EDGE_CASE_TESTS_H

#include "../state_machines/ieee_802_1as_2021_state_machine_coordinator.h"
#include "../core/ieee_802_1as_2021.h"
#include <functional>
#include <vector>
#include <string>
#include <chrono>
#include <memory>
#include <map>

namespace ieee_802_1as_2021 {
namespace testing {

/**
 * @brief Comprehensive Edge Case Test Framework
 * @details Tests every edge case defined in IEEE 802.1AS-2021 standard
 * 
 * This test framework validates:
 * - All timeout scenarios (Section 10.2.3, 10.2.5, 11.2.17)
 * - Path delay edge cases (Section 11.2.15, 11.2.16)
 * - BMCA edge cases (Section 10.3.5)
 * - Milan profile compliance (Section 16)
 * - Network failure recovery
 * - Clock accuracy degradation
 * - Asymmetric path handling
 * - Multiple PDelay response detection
 * - And every other edge case in the standard
 */
class EdgeCaseTestFramework {
public:
    /**
     * @brief Test categories covering all IEEE 802.1AS-2021 edge cases
     */
    enum class TestCategory {
        // Basic Protocol Tests
        INITIALIZATION_EDGE_CASES,      ///< Power-up, reset, initialization failures
        MESSAGE_VALIDATION_EDGE_CASES,  ///< Malformed messages, validation failures
        SEQUENCE_ERROR_EDGE_CASES,      ///< Sequence number errors, duplicates
        
        // Timeout Edge Cases (Critical for stability)
        ANNOUNCE_RECEIPT_TIMEOUT,       ///< Section 10.2.3 - Announce timeout scenarios
        SYNC_RECEIPT_TIMEOUT,          ///< Section 10.2.5 - Sync timeout scenarios  
        PDELAY_RECEIPT_TIMEOUT,        ///< Section 11.2.17 - PDelay timeout scenarios
        QUALIFICATION_TIMEOUT,         ///< Section 10.3.5 - BMCA qualification timeout
        
        // Path Delay Edge Cases (Section 11.2)
        PDELAY_ASYMMETRIC_PATHS,       ///< Asymmetric path delay scenarios
        PDELAY_VARIABLE_DELAY,         ///< Variable path delay scenarios
        PDELAY_ZERO_DELAY,             ///< Zero or negative path delay
        PDELAY_INFINITE_DELAY,         ///< Infinite or excessive path delay
        PDELAY_MEASUREMENT_ERRORS,     ///< Path delay measurement errors
        
        // BMCA Edge Cases (Section 10.3)
        BMCA_TIE_BREAKING,             ///< Identical clock priorities
        BMCA_CLOCK_IDENTITY_CONFLICTS, ///< Clock identity conflicts
        BMCA_SIMULTANEOUS_MASTERS,     ///< Multiple masters claiming same priority
        BMCA_RAPID_TOPOLOGY_CHANGES,   ///< Rapid network topology changes
        BMCA_FOREIGN_MASTER_EXPIRY,    ///< Foreign master timeout scenarios
        
        // Synchronization Edge Cases (Section 10.2)
        SYNC_CLOCK_JUMP_DETECTION,     ///< Clock jump detection and recovery
        SYNC_FREQUENCY_STEP_DETECTION, ///< Frequency step detection
        SYNC_SERVO_INSTABILITY,        ///< Clock servo instability
        SYNC_OFFSET_OVERFLOW,          ///< Offset calculation overflow
        SYNC_LOCKED_DETECTION,         ///< Sync locked detection edge cases
        
        // Network Edge Cases
        NETWORK_CONGESTION,            ///< Network congestion scenarios
        NETWORK_PACKET_LOSS,           ///< Packet loss scenarios
        NETWORK_DUPLICATE_PACKETS,     ///< Duplicate packet scenarios
        NETWORK_OUT_OF_ORDER_PACKETS,  ///< Out-of-order packet scenarios
        NETWORK_LINK_FLAPPING,         ///< Link up/down flapping
        
        // Hardware Edge Cases
        HARDWARE_TIMESTAMP_FAULTS,     ///< Hardware timestamping faults
        HARDWARE_CLOCK_FAULTS,         ///< Hardware clock faults
        HARDWARE_REGISTER_FAULTS,      ///< Hardware register access faults
        HARDWARE_INTERRUPT_FAULTS,     ///< Hardware interrupt issues
        
        // Milan Profile Edge Cases (Section 16)
        MILAN_MULTIPLE_PDELAY_RESP,    ///< Multiple PDelay response detection
        MILAN_PDELAY_CESSATION,        ///< PDelay transmission cessation
        MILAN_COMPLIANCE_VIOLATIONS,   ///< Milan profile violations
        MILAN_INTEROPERABILITY,        ///< Milan interoperability scenarios
        
        // Power and Environmental Edge Cases
        POWER_SUPPLY_VARIATIONS,       ///< Power supply variation effects
        TEMPERATURE_VARIATIONS,        ///< Temperature variation effects
        ELECTROMAGNETIC_INTERFERENCE,  ///< EMI effects on timing
        
        // Advanced Protocol Features Edge Cases
        ALTERNATE_TIMESCALE,           ///< Alternate timescale edge cases
        PATH_TRACE,                    ///< Path trace edge cases
        FREQUENCY_TRACEABLE,           ///< Frequency traceability edge cases
        TIME_TRACEABLE,                ///< Time traceability edge cases
        
        // Recovery and Fault Tolerance Edge Cases
        AUTOMATIC_RECOVERY,            ///< Automatic fault recovery scenarios
        MANUAL_RECOVERY,               ///< Manual recovery scenarios
        CASCADING_FAILURES,            ///< Cascading failure scenarios
        PARTIAL_NETWORK_FAILURES       ///< Partial network failure scenarios
    };

    /**
     * @brief Test result for individual edge case
     */
    struct TestResult {
        TestCategory category;
        std::string test_name;
        bool passed = false;
        std::string description;
        std::vector<std::string> violations_found;
        std::vector<std::string> gaps_identified;
        std::chrono::milliseconds test_duration{0};
        std::string detailed_log;
        
        // IEEE 802.1AS-2021 compliance specific results
        bool ieee_compliant = false;
        std::vector<std::string> ieee_section_violations;
        std::string compliance_level; // "FULL", "PARTIAL", "NON_COMPLIANT"
    };

    /**
     * @brief Comprehensive test suite results
     */
    struct TestSuiteResults {
        uint32_t total_tests_run = 0;
        uint32_t tests_passed = 0;
        uint32_t tests_failed = 0;
        uint32_t ieee_compliant_tests = 0;
        std::vector<TestResult> all_results;
        std::vector<std::string> remaining_gaps;
        std::vector<std::string> critical_issues;
        std::chrono::milliseconds total_test_time{0};
        bool overall_ieee_compliance = false;
        std::string compliance_report;
    };

    /**
     * @brief Test callbacks for validation
     */
    using TestProgressCallback = std::function<void(const std::string& test_name, double progress)>;
    using TestResultCallback = std::function<void(const TestResult& result)>;
    using GapIdentifiedCallback = std::function<void(const std::string& gap_description, const std::string& ieee_section)>;

    explicit EdgeCaseTestFramework();
    ~EdgeCaseTestFramework();

    // ============================================================================
    // Test Execution
    // ============================================================================

    /**
     * @brief Run all edge case tests
     * @return Complete test suite results
     */
    TestSuiteResults run_all_edge_case_tests();

    /**
     * @brief Run tests for specific category
     * @param category Test category to run
     * @return Test results for category
     */
    std::vector<TestResult> run_category_tests(TestCategory category);

    /**
     * @brief Run specific edge case test
     * @param test_name Name of specific test
     * @return Test result
     */
    TestResult run_specific_test(const std::string& test_name);

    // ============================================================================
    // Timeout Edge Case Tests (Critical)
    // ============================================================================

    /**
     * @brief Test Announce receipt timeout edge cases per IEEE 802.1AS-2021 Section 10.2.3
     * @details Tests all timeout scenarios including:
     * - Single announce timeout
     * - Multiple consecutive timeouts
     * - Timeout during master clock change
     * - Timeout with network congestion
     * - Recovery from timeout
     */
    TestResult test_announce_receipt_timeout_edge_cases();

    /**
     * @brief Test Sync receipt timeout edge cases per IEEE 802.1AS-2021 Section 10.2.5
     * @details Tests sync timeout scenarios including:
     * - Sync timeout in synchronized state
     * - Sync timeout during frequency adjustment
     * - Multiple sync timeouts
     * - Timeout recovery with offset correction
     */
    TestResult test_sync_receipt_timeout_edge_cases();

    /**
     * @brief Test PDelay timeout edge cases per IEEE 802.1AS-2021 Section 11.2.17
     * @details Tests path delay timeout scenarios including:
     * - PDelay request timeout
     * - PDelay response timeout
     * - Milan profile cessation requirements
     */
    TestResult test_pdelay_timeout_edge_cases();

    // ============================================================================
    // Path Delay Edge Case Tests
    // ============================================================================

    /**
     * @brief Test asymmetric path delay scenarios
     * @details Tests per IEEE 802.1AS-2021 Section 11.2.15:
     * - Asymmetric physical paths
     * - Asymmetric processing delays
     * - Asymmetric network congestion
     * - Asymmetry measurement and correction
     */
    TestResult test_asymmetric_path_delay_edge_cases();

    /**
     * @brief Test variable path delay scenarios
     * @details Tests dynamic path delay changes:
     * - Network load variations
     * - Temperature effects on cable delay
     * - Switch processing variations
     * - Path delay filtering requirements
     */
    TestResult test_variable_path_delay_edge_cases();

    /**
     * @brief Test path delay measurement edge cases
     * @details Tests measurement accuracy scenarios:
     * - Zero path delay scenarios
     * - Negative path delay detection
     * - Path delay overflow conditions
     * - Measurement precision limits
     */
    TestResult test_path_delay_measurement_edge_cases();

    // ============================================================================
    // BMCA Edge Case Tests
    // ============================================================================

    /**
     * @brief Test BMCA tie-breaking scenarios per IEEE 802.1AS-2021 Section 10.3.5
     * @details Tests complex BMCA decisions:
     * - Identical priorities and clock quality
     * - Clock identity tie-breaking
     * - Steps removed comparison
     * - Port identity tie-breaking
     */
    TestResult test_bmca_tie_breaking_edge_cases();

    /**
     * @brief Test simultaneous master scenarios
     * @details Tests multiple masters claiming best clock:
     * - Network partition recovery
     * - Simultaneous startup
     * - Master clock failure scenarios
     */
    TestResult test_simultaneous_master_edge_cases();

    /**
     * @brief Test rapid topology change scenarios
     * @details Tests network instability:
     * - Rapid link up/down events
     * - Multiple simultaneous topology changes
     * - BMCA convergence time
     */
    TestResult test_rapid_topology_change_edge_cases();

    // ============================================================================
    // Milan Profile Edge Case Tests
    // ============================================================================

    /**
     * @brief Test Milan multiple PDelay response detection per IEEE 802.1AS-2021 Section 16
     * @details Tests Milan profile requirements:
     * - Multiple response detection algorithm
     * - Response cessation trigger conditions
     * - Recovery after cessation
     */
    TestResult test_milan_multiple_pdelay_response_edge_cases();

    /**
     * @brief Test Milan PDelay cessation scenarios
     * @details Tests cessation behavior:
     * - Cessation trigger conditions
     * - Cessation duration
     * - Recovery conditions
     * - Interoperability with non-Milan devices
     */
    TestResult test_milan_pdelay_cessation_edge_cases();

    /**
     * @brief Test Milan compliance violations
     * @details Tests detection of Milan violations:
     * - Non-compliant device detection
     * - Mixed Milan/non-Milan networks
     * - Compliance reporting
     */
    TestResult test_milan_compliance_violation_edge_cases();

    // ============================================================================
    // Clock and Synchronization Edge Case Tests
    // ============================================================================

    /**
     * @brief Test clock jump detection and recovery
     * @details Tests large time changes:
     * - Forward time jumps
     * - Backward time jumps
     * - Recovery procedures
     */
    TestResult test_clock_jump_edge_cases();

    /**
     * @brief Test frequency step detection
     * @details Tests frequency change scenarios:
     * - Sudden frequency changes
     * - Frequency step detection algorithms
     * - Recovery procedures
     */
    TestResult test_frequency_step_edge_cases();

    /**
     * @brief Test clock servo instability
     * @details Tests servo control edge cases:
     * - Oscillatory behavior
     * - Servo saturation
     * - Instability detection
     */
    TestResult test_clock_servo_instability_edge_cases();

    // ============================================================================
    // Network Edge Case Tests
    // ============================================================================

    /**
     * @brief Test network congestion scenarios
     * @details Tests network load effects:
     * - Variable message delays
     * - Packet loss scenarios
     * - Congestion detection
     */
    TestResult test_network_congestion_edge_cases();

    /**
     * @brief Test duplicate packet scenarios
     * @details Tests duplicate message handling:
     * - Duplicate detection
     * - Sequence number validation
     * - Impact on synchronization
     */
    TestResult test_duplicate_packet_edge_cases();

    /**
     * @brief Test out-of-order packet scenarios
     * @details Tests packet reordering:
     * - Message sequence validation
     * - Reordering detection
     * - Recovery procedures
     */
    TestResult test_out_of_order_packet_edge_cases();

    // ============================================================================
    // Hardware Edge Case Tests
    // ============================================================================

    /**
     * @brief Test hardware timestamp fault scenarios
     * @details Tests hardware failure modes:
     * - Timestamp unavailable
     * - Timestamp inaccuracy
     * - Hardware fault detection
     */
    TestResult test_hardware_timestamp_fault_edge_cases();

    /**
     * @brief Test hardware clock fault scenarios
     * @details Tests clock hardware failures:
     * - Clock source failures
     * - Frequency source instability
     * - Hardware fault recovery
     */
    TestResult test_hardware_clock_fault_edge_cases();

    // ============================================================================
    // Recovery and Fault Tolerance Edge Case Tests
    // ============================================================================

    /**
     * @brief Test automatic recovery scenarios
     * @details Tests automatic fault recovery:
     * - Recovery trigger conditions
     * - Recovery procedures
     * - Recovery success validation
     */
    TestResult test_automatic_recovery_edge_cases();

    /**
     * @brief Test cascading failure scenarios
     * @details Tests complex failure modes:
     * - Multiple simultaneous failures
     * - Failure propagation
     * - System resilience
     */
    TestResult test_cascading_failure_edge_cases();

    // ============================================================================
    // IEEE 802.1AS-2021 Gap Analysis
    // ============================================================================

    /**
     * @brief Identify remaining gaps in IEEE 802.1AS-2021 implementation
     * @return List of identified gaps with IEEE section references
     */
    std::vector<std::string> identify_remaining_implementation_gaps();

    /**
     * @brief Validate complete IEEE 802.1AS-2021 compliance
     * @return Detailed compliance report
     */
    std::string validate_complete_ieee_compliance();

    /**
     * @brief Generate comprehensive edge case coverage report
     * @return Coverage report showing all tested edge cases
     */
    std::string generate_edge_case_coverage_report();

    // ============================================================================
    // Configuration and Callbacks
    // ============================================================================

    void set_test_progress_callback(TestProgressCallback callback);
    void set_test_result_callback(TestResultCallback callback);
    void set_gap_identified_callback(GapIdentifiedCallback callback);

    /**
     * @brief Set test timeout for individual tests
     * @param timeout Maximum time per test
     */
    void set_test_timeout(std::chrono::milliseconds timeout);

    /**
     * @brief Enable detailed logging for specific test categories
     * @param category Category to enable logging for
     * @param enable Enable/disable logging
     */
    void enable_detailed_logging(TestCategory category, bool enable);

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Test utilities for creating edge case scenarios
 */
class EdgeCaseTestUtilities {
public:
    /**
     * @brief Create simulated network congestion
     * @param congestion_level Congestion level (0.0-1.0)
     * @param duration Duration of congestion
     */
    static void simulate_network_congestion(double congestion_level, std::chrono::milliseconds duration);

    /**
     * @brief Create simulated clock jump
     * @param jump_magnitude_ns Magnitude of clock jump in nanoseconds
     * @param direction Forward (true) or backward (false)
     */
    static void simulate_clock_jump(int64_t jump_magnitude_ns, bool forward);

    /**
     * @brief Create simulated hardware fault
     * @param fault_type Type of hardware fault
     * @param duration Duration of fault
     */
    static void simulate_hardware_fault(const std::string& fault_type, std::chrono::milliseconds duration);

    /**
     * @brief Create simulated message corruption
     * @param corruption_rate Message corruption rate (0.0-1.0)
     */
    static void simulate_message_corruption(double corruption_rate);

    /**
     * @brief Validate IEEE 802.1AS-2021 message format
     * @param message_data Raw message bytes
     * @return Validation result with detailed errors
     */
    static MessageValidator::ValidationResult validate_ieee_message_format(const std::vector<uint8_t>& message_data);
};

/**
 * @brief Automated test runner for continuous validation
 */
class ContinuousEdgeCaseValidator {
public:
    /**
     * @brief Configuration for continuous testing
     */
    struct Configuration {
        std::chrono::minutes test_interval{60};          ///< Run tests every hour
        std::vector<EdgeCaseTestFramework::TestCategory> enabled_categories;
        bool stop_on_failure = false;                   ///< Stop testing on first failure
        bool generate_reports = true;                   ///< Generate test reports
        std::string report_directory = "./test_reports"; ///< Report output directory
    };

    explicit ContinuousEdgeCaseValidator(const Configuration& config);
    ~ContinuousEdgeCaseValidator();

    /**
     * @brief Start continuous edge case validation
     * @return true if started successfully
     */
    bool start_continuous_validation();

    /**
     * @brief Stop continuous validation
     */
    void stop_continuous_validation();

    /**
     * @brief Get latest test results
     */
    EdgeCaseTestFramework::TestSuiteResults get_latest_results() const;

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

} // namespace testing
} // namespace ieee_802_1as_2021

#endif // IEEE_802_1AS_2021_EDGE_CASE_TESTS_H
