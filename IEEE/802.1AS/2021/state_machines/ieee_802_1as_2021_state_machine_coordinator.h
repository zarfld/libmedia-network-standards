/**
 * @file ieee_802_1as_2021_state_machine_coordinator.h
 * @brief IEEE 802.1AS-2021 Complete State Machine Coordinator
 * @details Comprehensive state machine coordination covering all edge cases per IEEE 802.1AS-2021
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Complete state machine implementation with all edge cases
 */

#ifndef IEEE_802_1AS_2021_STATE_MACHINE_COORDINATOR_H
#define IEEE_802_1AS_2021_STATE_MACHINE_COORDINATOR_H

#include "port_state_machine.h"
#include "path_delay_state_machine.h"
#include "sync_state_machine.h"
#include "announce_state_machine.h"
#include "../core/ieee_802_1as_2021.h"
#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

namespace ieee_802_1as_2021 {

/**
 * @brief Complete IEEE 802.1AS-2021 State Machine Coordinator
 * @details Coordinates all state machines to handle every edge case defined in IEEE 802.1AS-2021
 * 
 * This coordinator implements:
 * - Complete port state management per Section 10.3
 * - Path delay mechanisms per Section 11.2
 * - Sync/Follow_Up handling per Section 11.3
 * - Announce/BMCA per Section 11.4
 * - All timeout and recovery mechanisms
 * - Milan profile requirements
 * - Edge case handling for network failures, asymmetry, etc.
 */
class StateManager802_1AS_2021 {
public:
    /**
     * @brief Manager states covering all IEEE 802.1AS-2021 scenarios
     */
    enum class ManagerState {
        UNINITIALIZED,          ///< Manager not initialized
        INITIALIZING,           ///< Manager initializing all components
        FAULT_DETECTION,        ///< Detecting and handling faults
        LISTENING,              ///< Listening for network activity
        PRE_MASTER,            ///< Preparing master operation
        MASTER_ACTIVE,         ///< Active master operation
        SLAVE_SYNCHRONIZING,   ///< Synchronizing as slave
        SLAVE_SYNCHRONIZED,    ///< Synchronized slave operation
        PASSIVE_MONITORING,    ///< Passive monitoring mode
        ASYMMETRY_MEASUREMENT, ///< Measuring path asymmetry
        RECOVERY_MODE,         ///< Recovering from failures
        MILAN_COMPLIANCE_CHECK ///< Checking Milan profile compliance
    };

    /**
     * @brief Events that can trigger state machine transitions
     */
    enum class ManagerEvent {
        // Initialization events
        INITIALIZE_REQUEST,
        INITIALIZATION_COMPLETE,
        INITIALIZATION_FAILED,
        
        // Network events
        LINK_UP,
        LINK_DOWN,
        NETWORK_TOPOLOGY_CHANGED,
        
        // Message events
        ANNOUNCE_RECEIVED,
        SYNC_RECEIVED,
        FOLLOW_UP_RECEIVED,
        PDELAY_REQ_RECEIVED,
        PDELAY_RESP_RECEIVED,
        PDELAY_RESP_FOLLOW_UP_RECEIVED,
        
        // Timeout events
        ANNOUNCE_RECEIPT_TIMEOUT,
        SYNC_RECEIPT_TIMEOUT,
        PDELAY_TIMEOUT,
        QUALIFICATION_TIMEOUT,
        
        // BMCA events
        BMCA_DECISION_CHANGED,
        BETTER_MASTER_DISCOVERED,
        MASTER_CLOCK_FAILED,
        
        // Milan profile events
        MULTIPLE_PDELAY_RESP_DETECTED,
        PDELAY_CESSATION_REQUIRED,
        
        // Error conditions
        HARDWARE_FAULT_DETECTED,
        TIMESTAMP_FAULT,
        MESSAGE_VALIDATION_FAILED,
        CLOCK_ACCURACY_DEGRADED,
        
        // Recovery events
        FAULT_CLEARED,
        MANUAL_RECOVERY_TRIGGERED,
        AUTOMATIC_RECOVERY_TRIGGERED
    };

    /**
     * @brief Comprehensive configuration for all state machines
     */
    struct Configuration {
        // Basic port configuration
        uint16_t port_number;
        IEEE::_802_1AS::_2021::PortIdentity port_identity;
        IEEE::_802_1AS::_2021::ClockIdentity clock_identity;
        
        // Timing intervals (IEEE 802.1AS-2021 defaults)
        std::chrono::milliseconds announce_interval{1000};      // 1 second
        std::chrono::milliseconds sync_interval{125};           // 125 ms
        std::chrono::milliseconds pdelay_req_interval{1000};    // 1 second
        
        // Timeout multipliers
        uint8_t announce_receipt_timeout = 3;
        uint8_t sync_receipt_timeout = 3;
        uint8_t pdelay_receipt_timeout = 3;
        
        // Clock parameters
        IEEE::_802_1AS::_2021::ClockQuality local_clock_quality;
        uint8_t priority1 = 248;
        uint8_t priority2 = 248;
        uint8_t domain_number = 0;
        
        // Advanced features
        bool enable_milan_profile = true;               ///< Enable Milan profile compliance
        bool enable_path_asymmetry_measurement = false; ///< Enable asymmetry measurement
        bool enable_alternate_timescale = false;        ///< Enable alternate timescale
        bool enable_frequency_traceable = false;        ///< Enable frequency traceability
        bool enable_time_traceable = false;             ///< Enable time traceability
        
        // Fault tolerance
        uint32_t max_consecutive_timeouts = 5;          ///< Max timeouts before fault
        std::chrono::milliseconds fault_recovery_timeout{10000}; // 10 seconds
        bool enable_automatic_recovery = true;          ///< Enable automatic fault recovery
        
        // Performance tuning
        bool enable_fast_convergence = true;            ///< Enable fast network convergence
        bool enable_redundant_paths = false;            ///< Enable redundant path support
        uint8_t max_foreign_masters = 16;              ///< Maximum foreign masters to track
    };

    /**
     * @brief Complete system status information
     */
    struct SystemStatus {
        ManagerState manager_state;
        
        // Port state information
        PortStateMachine::State port_state;
        BMCAStateMachine::PortRole port_role;
        
        // Synchronization status
        bool is_synchronized = false;
        IEEE::_802_1AS::_2021::TimeInterval offset_from_master = 0;
        double frequency_adjustment_ppb = 0.0;
        IEEE::_802_1AS::_2021::TimeInterval path_delay = 0;
        
        // Master information
        IEEE::_802_1AS::_2021::ClockIdentity grandmaster_identity;
        IEEE::_802_1AS::_2021::PortIdentity master_port_identity;
        uint16_t steps_removed = 0;
        
        // Network topology
        uint32_t active_foreign_masters = 0;
        bool network_topology_stable = false;
        
        // Milan profile status
        bool milan_compliant = false;
        bool multiple_pdelay_resp_detected = false;
        bool pdelay_cessation_active = false;
        
        // Fault status
        uint32_t active_faults = 0;
        bool in_recovery_mode = false;
        std::vector<std::string> fault_descriptions;
        
        // Performance metrics
        double sync_accuracy_ns = 0.0;
        double max_time_interval_error_ns = 0.0;
        uint32_t message_rate_pps = 0;
        
        // Timestamps
        std::chrono::steady_clock::time_point last_update;
        std::chrono::steady_clock::time_point last_sync_received;
        std::chrono::steady_clock::time_point last_announce_received;
    };

    /**
     * @brief Edge case detection and handling information
     */
    struct EdgeCaseStatus {
        // Timing edge cases
        bool clock_jump_detected = false;
        bool frequency_step_detected = false;
        bool asymmetric_path_detected = false;
        bool variable_path_delay_detected = false;
        
        // Network edge cases
        bool network_congestion_detected = false;
        bool duplicate_announce_detected = false;
        bool announce_sequence_error = false;
        bool sync_sequence_error = false;
        
        // Hardware edge cases
        bool timestamp_uncertainty_high = false;
        bool hardware_timestamp_fault = false;
        bool clock_servo_instability = false;
        
        // Milan profile edge cases
        bool milan_cessation_required = false;
        uint32_t multiple_response_count = 0;
        bool milan_profile_violation = false;
        
        // Recovery status
        bool recovery_in_progress = false;
        std::chrono::steady_clock::time_point recovery_started;
        uint32_t recovery_attempts = 0;
    };

    /**
     * @brief Comprehensive statistics covering all state machines
     */
    struct Statistics {
        // Message statistics
        uint64_t announce_tx = 0, announce_rx = 0;
        uint64_t sync_tx = 0, sync_rx = 0;
        uint64_t follow_up_tx = 0, follow_up_rx = 0;
        uint64_t pdelay_req_tx = 0, pdelay_req_rx = 0;
        uint64_t pdelay_resp_tx = 0, pdelay_resp_rx = 0;
        uint64_t pdelay_resp_follow_up_tx = 0, pdelay_resp_follow_up_rx = 0;
        
        // Error statistics
        uint64_t message_validation_errors = 0;
        uint64_t timestamp_errors = 0;
        uint64_t timeout_events = 0;
        uint64_t fault_events = 0;
        uint64_t recovery_events = 0;
        
        // State transition statistics
        uint64_t manager_state_transitions = 0;
        uint64_t port_state_transitions = 0;
        uint64_t bmca_decisions = 0;
        
        // Performance statistics
        IEEE::_802_1AS::_2021::TimeInterval max_offset_observed = 0;
        IEEE::_802_1AS::_2021::TimeInterval min_offset_observed = 0;
        double max_frequency_adjustment = 0.0;
        double min_frequency_adjustment = 0.0;
        
        // Timing statistics
        std::chrono::steady_clock::time_point manager_start_time;
        std::chrono::steady_clock::time_point last_sync_time;
        std::chrono::milliseconds uptime{0};
        std::chrono::milliseconds synchronized_time{0};
    };

    /**
     * @brief Callbacks for system integration
     */
    using StatusUpdateCallback = std::function<void(const SystemStatus&)>;
    using EdgeCaseCallback = std::function<void(const EdgeCaseStatus&)>;
    using FaultCallback = std::function<void(const std::string& fault_description, bool critical)>;
    using RecoveryCallback = std::function<void(bool recovery_successful, const std::string& details)>;
    using MessageCallback = std::function<void(const std::string& message_type, const std::vector<uint8_t>& data)>;
    
    // Hardware interface callbacks
    using TimestampCallback = std::function<IEEE::_802_1AS::_2021::Timestamp()>;
    using ClockAdjustmentCallback = std::function<bool(double frequency_adjustment_ppb)>;
    using TimeAdjustmentCallback = std::function<bool(IEEE::_802_1AS::_2021::TimeInterval offset_ns)>;

    explicit StateManager802_1AS_2021(const Configuration& config);
    ~StateManager802_1AS_2021();

    // ============================================================================
    // System Control and Lifecycle Management
    // ============================================================================

    /**
     * @brief Initialize all state machines and start operation
     * @return true if initialization successful
     */
    bool initialize();

    /**
     * @brief Start all state machine operations
     * @return true if start successful
     */
    bool start();

    /**
     * @brief Stop all state machine operations
     * @return true if stop successful
     */
    bool stop();

    /**
     * @brief Reset all state machines to initial state
     */
    void reset();

    /**
     * @brief Shutdown and cleanup all resources
     */
    void shutdown();

    // ============================================================================
    // Event Processing and State Management
    // ============================================================================

    /**
     * @brief Process external events
     * @param event Event to process
     * @param event_data Optional event data
     */
    void process_event(ManagerEvent event, const std::vector<uint8_t>& event_data = {});

    /**
     * @brief Process received IEEE 802.1AS-2021 message
     * @param message_data Raw message bytes
     * @param receive_timestamp Receive timestamp
     * @return true if message processed successfully
     */
    bool process_received_message(const std::vector<uint8_t>& message_data,
                                 const IEEE::_802_1AS::_2021::Timestamp& receive_timestamp);

    /**
     * @brief Periodic tick for all state machines
     * @details Must be called regularly (e.g., every 10ms) for proper operation
     */
    void tick();

    // ============================================================================
    // Status and Information Queries
    // ============================================================================

    /**
     * @brief Get current complete system status
     */
    SystemStatus get_system_status() const;

    /**
     * @brief Get current edge case status
     */
    EdgeCaseStatus get_edge_case_status() const;

    /**
     * @brief Get comprehensive statistics
     */
    Statistics get_statistics() const;

    /**
     * @brief Check if system is fully synchronized
     */
    bool is_synchronized() const;

    /**
     * @brief Check if system is operating as master
     */
    bool is_master() const;

    /**
     * @brief Check if system is operating as slave
     */
    bool is_slave() const;

    /**
     * @brief Check Milan profile compliance status
     */
    bool is_milan_compliant() const;

    // ============================================================================
    // Configuration and Control
    // ============================================================================

    /**
     * @brief Update configuration parameters
     * @param new_config New configuration
     * @return true if update successful
     */
    bool update_configuration(const Configuration& new_config);

    /**
     * @brief Enable/disable Milan profile compliance
     * @param enable Enable Milan profile
     */
    void enable_milan_profile(bool enable);

    /**
     * @brief Force BMCA reselection
     */
    void force_bmca_reselection();

    /**
     * @brief Trigger manual recovery from faults
     */
    void trigger_manual_recovery();

    /**
     * @brief Set administrative port state
     * @param enabled True to enable port, false to disable
     */
    void set_port_enabled(bool enabled);

    // ============================================================================
    // Callback Registration
    // ============================================================================

    void set_status_update_callback(StatusUpdateCallback callback);
    void set_edge_case_callback(EdgeCaseCallback callback);
    void set_fault_callback(FaultCallback callback);
    void set_recovery_callback(RecoveryCallback callback);
    void set_message_callback(MessageCallback callback);

    // Hardware interface callbacks
    void set_timestamp_callback(TimestampCallback callback);
    void set_clock_adjustment_callback(ClockAdjustmentCallback callback);
    void set_time_adjustment_callback(TimeAdjustmentCallback callback);

    // ============================================================================
    // Advanced Features and Edge Case Handling
    // ============================================================================

    /**
     * @brief Handle detected clock jump
     * @param jump_magnitude_ns Magnitude of clock jump in nanoseconds
     */
    void handle_clock_jump(IEEE::_802_1AS::_2021::TimeInterval jump_magnitude_ns);

    /**
     * @brief Handle network congestion detection
     * @param congestion_level Congestion level (0.0 = none, 1.0 = severe)
     */
    void handle_network_congestion(double congestion_level);

    /**
     * @brief Handle asymmetric path detection
     * @param master_to_slave_delay Path delay master to slave
     * @param slave_to_master_delay Path delay slave to master
     */
    void handle_asymmetric_path(IEEE::_802_1AS::_2021::TimeInterval master_to_slave_delay,
                               IEEE::_802_1AS::_2021::TimeInterval slave_to_master_delay);

    /**
     * @brief Handle Milan profile violation
     * @param violation_type Type of violation
     * @param violation_data Additional violation data
     */
    void handle_milan_violation(const std::string& violation_type,
                               const std::vector<uint8_t>& violation_data = {});

    /**
     * @brief Enable debug logging for specific state machine
     * @param state_machine State machine name
     * @param enable Enable debug logging
     */
    void enable_debug_logging(const std::string& state_machine, bool enable);

    /**
     * @brief Export current state to JSON for diagnostics
     * @return JSON representation of current state
     */
    std::string export_state_to_json() const;

    /**
     * @brief Validate IEEE 802.1AS-2021 compliance
     * @return Compliance report
     */
    std::string validate_ieee_compliance() const;

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Utility class for IEEE 802.1AS-2021 message validation
 */
class MessageValidator {
public:
    /**
     * @brief Validation result
     */
    struct ValidationResult {
        bool valid = false;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::string message_type;
        uint16_t sequence_id = 0;
        IEEE::_802_1AS::_2021::PortIdentity source_port;
    };

    /**
     * @brief Validate IEEE 802.1AS-2021 message
     * @param message_data Raw message bytes
     * @return Validation result
     */
    static ValidationResult validate_message(const std::vector<uint8_t>& message_data);

    /**
     * @brief Validate Announce message specific fields
     */
    static ValidationResult validate_announce_message(const IEEE::_802_1AS::_2021::AnnounceMessage& message);

    /**
     * @brief Validate Sync message specific fields
     */
    static ValidationResult validate_sync_message(const IEEE::_802_1AS::_2021::SyncMessage& message);

    /**
     * @brief Validate PDelay message sequence
     */
    static ValidationResult validate_pdelay_sequence(
        const IEEE::_802_1AS::_2021::PDelayReqMessage* req,
        const IEEE::_802_1AS::_2021::PDelayRespMessage* resp,
        const IEEE::_802_1AS::_2021::PDelayRespFollowUpMessage* follow_up);
};

/**
 * @brief Factory for creating pre-configured state managers
 */
class StateManagerFactory {
public:
    /**
     * @brief Create state manager for Milan profile compliance
     */
    static std::unique_ptr<StateManager802_1AS_2021> create_milan_compliant_manager(
        uint16_t port_number,
        const IEEE::_802_1AS::_2021::ClockIdentity& clock_identity);

    /**
     * @brief Create state manager for automotive applications
     */
    static std::unique_ptr<StateManager802_1AS_2021> create_automotive_manager(
        uint16_t port_number,
        const IEEE::_802_1AS::_2021::ClockIdentity& clock_identity);

    /**
     * @brief Create state manager for professional audio/video
     */
    static std::unique_ptr<StateManager802_1AS_2021> create_professional_av_manager(
        uint16_t port_number,
        const IEEE::_802_1AS::_2021::ClockIdentity& clock_identity);

    /**
     * @brief Create state manager with custom configuration
     */
    static std::unique_ptr<StateManager802_1AS_2021> create_custom_manager(
        const StateManager802_1AS_2021::Configuration& config);
};

} // namespace ieee_802_1as_2021

#endif // IEEE_802_1AS_2021_STATE_MACHINE_COORDINATOR_H
