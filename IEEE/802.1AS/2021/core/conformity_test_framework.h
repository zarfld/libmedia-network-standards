/**
 * @file conformity_test_framework.h
 * @brief IEEE 802.1AS-2021 Conformity Testing Framework
 * @details Comprehensive test suite for validating IEEE 802.1AS-2021 compliance
 * 
 * This framework provides systematic testing according to:
 * - IEEE 802.1AS-2021 Clauses 11.2-11.5: Conformance requirements
 * - IEEE 802.1AS-2021 Annex A: Implementation conformance statement (ICS)
 * - IEEE 802.1AS-2021 Annex B: Performance requirements validation
 * - IEEE 802.1AS-2021 Clause 10: Protocol operation testing
 * 
 * Test Categories:
 * 1. Message Format Compliance (Table 10-7)
 * 2. State Machine Behavior (Figures 10-3 to 10-9)  
 * 3. Timing Requirements (Annex B)
 * 4. Protocol Interoperability
 * 5. Error Handling and Recovery
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Test framework follows IEEE 802.1AS-2021 specification
 */

#ifndef IEEE_802_1AS_2021_CONFORMITY_TEST_FRAMEWORK_H
#define IEEE_802_1AS_2021_CONFORMITY_TEST_FRAMEWORK_H

#include "../core/ieee_802_1as_2021.h"
#include "../core/time_sync_engine.h"
#include "../core/performance_measurement.h"
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <memory>

namespace IEEE {
namespace _802_1 {
namespace AS {
namespace _2021 {
namespace Testing {

/**
 * @brief Test result enumeration
 */
enum class TestResult {
    PASS,
    FAIL,
    NOT_APPLICABLE,
    INCONCLUSIVE,
    ERROR
};

/**
 * @brief Test execution status
 */
enum class TestStatus {
    NOT_STARTED,
    RUNNING,
    COMPLETED,
    FAILED,
    SKIPPED
};

/**
 * @brief Individual test case result
 */
struct TestCaseResult {
    std::string test_id;                    // Unique test identifier
    std::string test_name;                  // Human-readable test name
    std::string test_description;           // Detailed test description
    TestResult result;                      // Test outcome
    TestStatus status;                      // Execution status
    std::string failure_reason;             // Reason for failure (if applicable)
    std::chrono::duration<double> execution_time; // Test execution time
    std::vector<std::string> details;       // Additional test details
    std::string ieee_reference;             // IEEE 802.1AS-2021 clause reference
    
    TestCaseResult() : result(TestResult::NOT_APPLICABLE), status(TestStatus::NOT_STARTED) {}
};

/**
 * @brief Test suite result summary
 */
struct TestSuiteResult {
    std::string suite_name;                 // Test suite name
    std::string ieee_clause_reference;      // IEEE clause being tested
    std::vector<TestCaseResult> test_cases; // Individual test results
    uint32_t tests_passed;                  // Number of passed tests
    uint32_t tests_failed;                  // Number of failed tests
    uint32_t tests_not_applicable;          // Number of N/A tests
    uint32_t tests_inconclusive;            // Number of inconclusive tests
    std::chrono::duration<double> total_execution_time; // Total suite execution time
    bool overall_compliance;                // True if all applicable tests pass
    
    TestSuiteResult() : tests_passed(0), tests_failed(0), tests_not_applicable(0), 
                       tests_inconclusive(0), overall_compliance(false) {}
};

/**
 * @brief Complete conformity test report
 */
struct ConformityTestReport {
    std::string device_under_test;          // Device/implementation being tested
    std::string test_date;                  // Test execution date
    std::string test_version;               // Test framework version
    std::string ieee_standard_version;      // IEEE 802.1AS-2021 version
    
    std::vector<TestSuiteResult> suite_results; // All test suite results
    
    // Summary statistics
    uint32_t total_tests;                   // Total number of tests
    uint32_t total_passed;                  // Total passed tests
    uint32_t total_failed;                  // Total failed tests
    uint32_t total_not_applicable;          // Total N/A tests
    uint32_t total_inconclusive;            // Total inconclusive tests
    
    bool ieee_802_1as_2021_compliant;      // Overall compliance status
    std::string compliance_summary;         // Human-readable summary
    std::vector<std::string> non_compliance_issues; // List of compliance issues
    
    ConformityTestReport() : total_tests(0), total_passed(0), total_failed(0),
                            total_not_applicable(0), total_inconclusive(0),
                            ieee_802_1as_2021_compliant(false) {}
};

/**
 * @brief Main conformity test framework class
 */
class ConformityTestFramework {
public:
    /**
     * @brief Constructor
     */
    explicit ConformityTestFramework();
    
    /**
     * @brief Destructor
     */
    ~ConformityTestFramework();

    // Test Suite Execution Methods
    
    /**
     * @brief Execute all conformity tests
     * @return Complete conformity test report
     */
    ConformityTestReport execute_all_tests();
    
    /**
     * @brief Execute specific test suite
     * @param suite_name Name of the test suite to execute
     * @return Test suite result
     */
    TestSuiteResult execute_test_suite(const std::string& suite_name);
    
    /**
     * @brief Execute individual test case
     * @param test_id Unique test identifier
     * @return Test case result
     */
    TestCaseResult execute_test_case(const std::string& test_id);

    // IEEE 802.1AS-2021 Specific Test Suites
    
    /**
     * @brief Test message format compliance (IEEE 802.1AS-2021 Table 10-7)
     * Validates PTP message header format, field sizes, endianness
     */
    TestSuiteResult test_message_format_compliance();
    
    /**
     * @brief Test state machine behavior (IEEE 802.1AS-2021 Figures 10-3 to 10-9)
     * Validates state transitions, event handling, timing
     */
    TestSuiteResult test_state_machine_behavior();
    
    /**
     * @brief Test timing requirements (IEEE 802.1AS-2021 Annex B)
     * Validates performance requirements, accuracy, jitter, wander
     */
    TestSuiteResult test_timing_requirements();
    
    /**
     * @brief Test protocol interoperability (IEEE 802.1AS-2021 Clause 11)
     * Validates interoperability with other 802.1AS implementations
     */
    TestSuiteResult test_protocol_interoperability();
    
    /**
     * @brief Test error handling and recovery (IEEE 802.1AS-2021 Clause 10.6)
     * Validates error detection, reporting, and recovery mechanisms
     */
    TestSuiteResult test_error_handling_recovery();
    
    /**
     * @brief Test BMCA implementation (IEEE 802.1AS-2021 Clause 10.3)
     * Validates Best Master Clock Algorithm behavior
     */
    TestSuiteResult test_bmca_implementation();
    
    /**
     * @brief Test path delay mechanisms (IEEE 802.1AS-2021 Clause 10.2)
     * Validates peer delay and end-to-end delay mechanisms
     */
    TestSuiteResult test_path_delay_mechanisms();

    // Configuration and Utilities
    
    /**
     * @brief Set device under test information
     */
    void set_device_under_test(const std::string& device_info);
    
    /**
     * @brief Set time sync engine for testing
     */
    void set_time_sync_engine(std::shared_ptr<TimeSynchronizationEngine> engine);
    
    /**
     * @brief Set performance measurement framework
     */
    void set_performance_measurement(std::shared_ptr<PerformanceMeasurement> perf);
    
    /**
     * @brief Enable/disable specific test categories
     */
    void enable_test_category(const std::string& category, bool enable);
    
    /**
     * @brief Set test execution timeout
     */
    void set_test_timeout(std::chrono::seconds timeout);
    
    /**
     * @brief Generate compliance report in various formats
     */
    std::string generate_report_xml(const ConformityTestReport& report);
    std::string generate_report_json(const ConformityTestReport& report);
    std::string generate_report_text(const ConformityTestReport& report);
    
    /**
     * @brief Validate Implementation Conformance Statement (ICS)
     * IEEE 802.1AS-2021 Annex A
     */
    TestSuiteResult validate_implementation_conformance_statement();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Specific test case implementations
 */
namespace TestCases {

/**
 * @brief Message Format Tests (Table 10-7)
 */
namespace MessageFormat {
    TestCaseResult test_announce_message_format();
    TestCaseResult test_sync_message_format();
    TestCaseResult test_follow_up_message_format();
    TestCaseResult test_pdelay_req_message_format();
    TestCaseResult test_pdelay_resp_message_format();
    TestCaseResult test_pdelay_resp_follow_up_format();
    TestCaseResult test_signaling_message_format();
    TestCaseResult test_management_message_format();
    TestCaseResult test_header_endianness_compliance();
    TestCaseResult test_tlv_format_compliance();
}

/**
 * @brief State Machine Tests (Figures 10-3 to 10-9)
 */
namespace StateMachine {
    TestCaseResult test_site_sync_sync_transitions();
    TestCaseResult test_port_sync_sync_receive_behavior();
    TestCaseResult test_clock_slave_sync_behavior();
    TestCaseResult test_clock_master_sync_send_behavior();
    TestCaseResult test_port_announce_information_sm();
    TestCaseResult test_port_announce_receive_sm();
    TestCaseResult test_port_announce_transmit_sm();
    TestCaseResult test_link_delay_interval_setting_sm();
    TestCaseResult test_md_pdelay_req_sm();
    TestCaseResult test_md_pdelay_resp_sm();
}

/**
 * @brief Timing Requirements Tests (Annex B)
 */
namespace TimingRequirements {
    TestCaseResult test_frequency_accuracy_compliance();
    TestCaseResult test_time_granularity_compliance();
    TestCaseResult test_jitter_generation_compliance();
    TestCaseResult test_wander_generation_compliance();
    TestCaseResult test_residence_time_compliance();
    TestCaseResult test_pdelay_turnaround_compliance();
    TestCaseResult test_rate_ratio_measurement_compliance();
    TestCaseResult test_end_to_end_accuracy_compliance();
    TestCaseResult test_tdev_mask_compliance();
    TestCaseResult test_mtie_compliance();
}

/**
 * @brief Protocol Interoperability Tests
 */
namespace Interoperability {
    TestCaseResult test_multiple_domain_support();
    TestCaseResult test_backwards_compatibility_802_1as_2020();
    TestCaseResult test_backwards_compatibility_802_1as_2011();
    TestCaseResult test_cross_vendor_interoperability();
    TestCaseResult test_mixed_hop_count_scenarios();
    TestCaseResult test_network_topology_changes();
}

/**
 * @brief Error Handling Tests
 */
namespace ErrorHandling {
    TestCaseResult test_clock_failure_detection();
    TestCaseResult test_network_partition_handling();
    TestCaseResult test_invalid_message_handling();
    TestCaseResult test_timeout_recovery();
    TestCaseResult test_frequency_drift_compensation();
    TestCaseResult test_leap_second_handling();
}

} // namespace TestCases

/**
 * @brief Test utilities and helper functions
 */
namespace TestUtilities {
    /**
     * @brief Create test messages with specific format violations
     */
    std::vector<uint8_t> create_malformed_announce_message();
    std::vector<uint8_t> create_malformed_sync_message();
    
    /**
     * @brief Simulate network conditions for testing
     */
    void simulate_network_delay(std::chrono::nanoseconds delay);
    void simulate_packet_loss(double loss_rate);
    void simulate_frequency_drift(double ppm_drift);
    
    /**
     * @brief Time measurement utilities
     */
    std::vector<Timestamp> capture_timestamp_sequence(uint32_t count, std::chrono::milliseconds interval);
    bool validate_timestamp_accuracy(const std::vector<Timestamp>& expected, const std::vector<Timestamp>& actual, TimeInterval tolerance);
    
    /**
     * @brief Statistical analysis utilities
     */
    double calculate_measurement_uncertainty(const std::vector<double>& measurements);
    bool is_within_confidence_interval(double value, double mean, double std_dev, double confidence_level);
}

} // namespace Testing
} // namespace _2021
} // namespace AS
} // namespace _802_1
} // namespace IEEE

#endif // IEEE_802_1AS_2021_CONFORMITY_TEST_FRAMEWORK_H
