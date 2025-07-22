/**
 * @file sync_state_machine.h
 * @brief IEEE 802.1AS-2021 Synchronization State Machines
 * @details Complete time synchronization state machines per IEEE 802.1AS-2021 Section 10
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Full implementation of IEEE 802.1AS-2021 sync state machines
 */

#ifndef IEEE_802_1AS_2021_SYNC_STATE_MACHINE_H
#define IEEE_802_1AS_2021_SYNC_STATE_MACHINE_H

#include "../core/ieee_802_1as_2021.h"
#include <chrono>
#include <functional>
#include <memory>
#include <deque>

namespace ieee_802_1as_2021 {

// Import types from IEEE::_802_1AS::_2021 namespace
using SyncMessage = IEEE::_802_1AS::_2021::SyncMessage;
using FollowUpMessage = IEEE::_802_1AS::_2021::FollowUpMessage;
using ClockIdentity = IEEE::_802_1AS::_2021::ClockIdentity;
using PortIdentity = IEEE::_802_1AS::_2021::PortIdentity;
using Timestamp = IEEE::_802_1AS::_2021::Timestamp;
using TimeInterval = IEEE::_802_1AS::_2021::TimeInterval;

/**
 * @brief Master Clock Transmission State Machine per IEEE 802.1AS-2021 Section 10.2.4
 * @details Handles Sync and Follow_Up message transmission for master ports
 */
class SyncTransmissionStateMachine {
public:
    /**
     * @brief Transmission states per IEEE 802.1AS-2021 Figure 10-11
     */
    enum class State {
        INITIALIZING,           ///< State machine initializing
        TRANSMIT_INIT,         ///< Initialize transmission parameters
        TRANSMIT_PERIODIC,     ///< Transmit periodic Sync messages
        TRANSMIT_SYNC,         ///< Transmitting Sync message
        WAITING_FOR_TIMESTAMP, ///< Waiting for Sync Tx timestamp
        TRANSMIT_FOLLOW_UP     ///< Transmitting Follow_Up message
    };

    /**
     * @brief Events per IEEE 802.1AS-2021 Section 10.2.4.2
     */
    enum class Event {
        SYNC_INTERVAL_TIMER_EXPIRED,    ///< Sync interval timer expired
        TX_TIMESTAMP_RECEIVED,          ///< Tx timestamp received for Sync
        TX_TIMESTAMP_TIMEOUT_EXPIRED,   ///< Tx timestamp timeout expired
        PORT_STATE_SELECTION_UPDATED,   ///< Port state selection updated
        SYNC_TRANSMISSION_ENABLED,      ///< Sync transmission enabled
        SYNC_TRANSMISSION_DISABLED,     ///< Sync transmission disabled
        INITIALIZE                      ///< Initialize event
    };

    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        uint16_t port_number;
        std::chrono::milliseconds sync_interval{125};        ///< 125ms (log2 = -3)
        std::chrono::milliseconds tx_timestamp_timeout{10};  ///< 10ms timeout
        bool two_step_clock = true;                         ///< Two-step clock (requires Follow_Up)
        uint8_t initial_log_sync_interval = -3;            ///< Initial log sync interval
        bool sync_locked_enabled = false;                   ///< Sync locked indication
        double clock_accuracy_degradation_threshold = 1e-6; ///< Clock accuracy threshold
    };

    /**
     * @brief Transmission statistics
     */
    struct TransmissionInfo {
        uint16_t sequence_id = 0;              ///< Current sequence ID
        Timestamp last_sync_tx_timestamp;     ///< Last Sync transmission timestamp
        Timestamp last_sync_origin_timestamp; ///< Last Sync origin timestamp
        std::chrono::steady_clock::time_point last_transmission;
        bool follow_up_pending = false;       ///< Follow_Up transmission pending
        bool timestamp_pending = false;       ///< Tx timestamp pending
    };

    /**
     * @brief Callbacks for hardware interface
     */
    using MessageTransmitCallback = std::function<bool(const SyncMessage&)>;
    using FollowUpTransmitCallback = std::function<bool(const FollowUpMessage&)>;
    using StateChangeCallback = std::function<void(State old_state, State new_state, Event trigger_event)>;
    using TimestampCallback = std::function<Timestamp()>; ///< Get current timestamp

    explicit SyncTransmissionStateMachine(const Configuration& config);
    ~SyncTransmissionStateMachine();

    // State machine control
    bool start();
    bool stop();
    void reset();

    // Event processing
    void process_event(Event event);
    void process_tx_timestamp(uint16_t sequence_id, const Timestamp& tx_timestamp);

    // State queries
    State get_current_state() const;
    bool is_transmitting() const;
    TransmissionInfo get_transmission_info() const;

    // Configuration
    void set_sync_interval(std::chrono::milliseconds interval);
    void set_two_step_clock(bool two_step);
    void enable_sync_locked_indication(bool enable);

    // Callbacks
    void set_message_transmit_callback(MessageTransmitCallback callback);
    void set_follow_up_transmit_callback(FollowUpTransmitCallback callback);
    void set_state_change_callback(StateChangeCallback callback);
    void set_timestamp_callback(TimestampCallback callback);

    // Timing control
    void tick(); ///< Called periodically for timer management

    // Statistics
    struct Statistics {
        uint64_t sync_messages_sent = 0;
        uint64_t follow_up_messages_sent = 0;
        uint64_t tx_timestamp_timeouts = 0;
        uint64_t sync_interval_adjustments = 0;
        std::chrono::steady_clock::time_point last_sync_transmission;
        double average_transmission_interval = 0.0;
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Clock Slave Synchronization State Machine per IEEE 802.1AS-2021 Section 10.2.5
 * @details Handles time synchronization for slave ports using PI controller
 */
class SlaveSynchronizationStateMachine {
public:
    /**
     * @brief Synchronization states per IEEE 802.1AS-2021 Figure 10-12
     */
    enum class State {
        INITIALIZING,          ///< State machine initializing
        UNCALIBRATED,         ///< Receiving sync but not synchronized
        SLAVE,                ///< Synchronized slave state
        GRAND_MASTER_PRESENT, ///< Grand master present but evaluating
        WAITING_FOR_FOLLOW_UP, ///< Waiting for Follow_Up message
        WAITING_FOR_SYNC      ///< Waiting for next Sync message
    };

    /**
     * @brief Events per IEEE 802.1AS-2021 Section 10.2.5.2
     */
    enum class Event {
        SYNC_RECEIVED,                    ///< Valid Sync message received
        FOLLOW_UP_RECEIVED,              ///< Valid Follow_Up message received
        SYNC_RECEIPT_TIMEOUT_EXPIRED,    ///< Sync receipt timeout expired
        QUALIFICATION_TIMEOUT_EXPIRED,   ///< Qualification timeout expired
        SELECTED_STATE_UPDATED,          ///< Port selected state updated
        CLOCK_MASTER_SYNC_OFFSET_UPDATED, ///< Clock offset updated
        INITIALIZE                       ///< Initialize event
    };

    /**
     * @brief PI Controller parameters for clock synchronization
     */
    struct PIControllerConfig {
        double kp = 0.7;                    ///< Proportional gain
        double ki = 0.3;                    ///< Integral gain
        double max_adjustment_ppb = 500.0;  ///< Maximum frequency adjustment (ppb)
        double min_adjustment_ppb = -500.0; ///< Minimum frequency adjustment (ppb)
        uint32_t sync_receipt_timeout_count = 3; ///< Sync receipt timeout multiplier
        std::chrono::milliseconds qualification_timeout{2000}; ///< Qualification timeout
    };

    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        uint16_t port_number;
        PIControllerConfig pi_controller;
        std::chrono::milliseconds follow_up_receipt_timeout{1000}; ///< Follow_Up timeout
        bool enable_offset_filtering = true;     ///< Enable offset measurement filtering
        uint8_t offset_filter_size = 8;          ///< Offset filter size
        bool enable_frequency_adjustment = true; ///< Enable frequency adjustment
        double sync_uncertainty_ns = 100.0;     ///< Sync uncertainty (nanoseconds)
        bool enable_phase_change_detection = true; ///< Enable phase change detection
    };

    /**
     * @brief Synchronization state information
     */
    struct SyncInfo {
        TimeInterval master_offset = 0;         ///< Current master offset (ns)
        double frequency_adjustment_ppb = 0.0;  ///< Current frequency adjustment (ppb)
        TimeInterval path_delay = 0;           ///< Path delay to master (ns)
        bool synchronized = false;              ///< Synchronization status
        uint16_t received_sync_sequence_id = 0; ///< Last received Sync sequence ID
        Timestamp last_sync_timestamp;         ///< Last sync timestamp
        std::chrono::steady_clock::time_point last_sync_received;
        double sync_receipt_rate = 0.0;        ///< Sync message reception rate
    };

    /**
     * @brief Callbacks for clock control interface
     */
    using ClockAdjustmentCallback = std::function<bool(double frequency_adjustment_ppb)>;
    using TimeAdjustmentCallback = std::function<bool(TimeInterval offset_adjustment_ns)>;
    using SyncInfoCallback = std::function<void(const SyncInfo&)>;
    using StateChangeCallback = std::function<void(State old_state, State new_state, Event trigger_event)>;

    explicit SlaveSynchronizationStateMachine(const Configuration& config);
    ~SlaveSynchronizationStateMachine();

    // State machine control
    bool start();
    bool stop();
    void reset();

    // Event processing
    void process_event(Event event);
    void process_sync_message(const SyncMessage& message, const Timestamp& receive_timestamp);
    void process_follow_up_message(const FollowUpMessage& message);

    // State queries
    State get_current_state() const;
    bool is_synchronized() const;
    SyncInfo get_sync_info() const;

    // Path delay integration
    void update_path_delay(TimeInterval path_delay_ns);

    // Configuration
    void set_pi_controller_parameters(const PIControllerConfig& config);
    void enable_offset_filtering(bool enable);
    void enable_frequency_adjustment(bool enable);

    // Callbacks
    void set_clock_adjustment_callback(ClockAdjustmentCallback callback);
    void set_time_adjustment_callback(TimeAdjustmentCallback callback);
    void set_sync_info_callback(SyncInfoCallback callback);
    void set_state_change_callback(StateChangeCallback callback);

    // Timing control
    void tick(); ///< Called periodically for timeout management

    // Statistics
    struct Statistics {
        uint64_t sync_messages_received = 0;
        uint64_t follow_up_messages_received = 0;
        uint64_t sync_receipt_timeouts = 0;
        uint64_t qualification_timeouts = 0;
        uint64_t clock_adjustments_made = 0;
        uint64_t time_adjustments_made = 0;
        TimeInterval max_offset_observed = 0;
        TimeInterval min_offset_observed = 0;
        double average_frequency_adjustment = 0.0;
        std::chrono::steady_clock::time_point last_synchronization;
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Clock Master Sync Offset State Machine per IEEE 802.1AS-2021 Section 10.2.6
 * @details Manages master clock offset calculations and adjustments
 */
class ClockMasterSyncOffsetStateMachine {
public:
    /**
     * @brief Offset calculation states
     */
    enum class State {
        INITIALIZING,        ///< State machine initializing
        UPDATE_OFFSET,       ///< Update offset calculation
        WAITING_FOR_SYNC,    ///< Waiting for Sync from upstream master
        CALCULATE_OFFSET,    ///< Calculate offset to upstream master
        PROPAGATE_OFFSET     ///< Propagate calculated offset
    };

    /**
     * @brief Events per IEEE 802.1AS-2021 Section 10.2.6.2
     */
    enum class Event {
        UPSTREAM_SYNC_RECEIVED,      ///< Sync received from upstream master
        OFFSET_CALCULATION_COMPLETE, ///< Offset calculation completed
        OFFSET_TIMEOUT_EXPIRED,      ///< Offset calculation timeout expired
        SELECTED_STATE_UPDATED,      ///< Port selected state updated
        INITIALIZE                   ///< Initialize event
    };

    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        uint16_t port_number;
        std::chrono::milliseconds offset_calculation_timeout{500}; ///< Offset calculation timeout
        bool enable_upstream_sync_monitoring = true;   ///< Monitor upstream sync quality
        double offset_threshold_ns = 50.0;            ///< Offset change threshold
        uint8_t offset_history_size = 16;             ///< Offset history for filtering
        bool enable_offset_prediction = false;         ///< Enable offset prediction
    };

    /**
     * @brief Master offset information
     */
    struct MasterOffsetInfo {
        TimeInterval upstream_offset = 0;      ///< Offset to upstream master (ns)
        TimeInterval cumulative_offset = 0;   ///< Cumulative offset from grand master (ns)
        double offset_stability = 0.0;        ///< Offset stability metric
        bool valid = false;                    ///< Offset validity
        uint16_t upstream_sync_sequence_id = 0; ///< Last upstream sync sequence ID
        std::chrono::steady_clock::time_point last_update;
    };

    /**
     * @brief Callbacks
     */
    using OffsetUpdateCallback = std::function<void(const MasterOffsetInfo&)>;
    using StateChangeCallback = std::function<void(State old_state, State new_state, Event trigger_event)>;

    explicit ClockMasterSyncOffsetStateMachine(const Configuration& config);
    ~ClockMasterSyncOffsetStateMachine();

    // State machine control
    bool start();
    bool stop();
    void reset();

    // Event processing
    void process_event(Event event);
    void process_upstream_sync(const SyncMessage& message, const Timestamp& receive_timestamp,
                              const FollowUpMessage* follow_up = nullptr);

    // State queries
    State get_current_state() const;
    MasterOffsetInfo get_master_offset_info() const;

    // Configuration
    void set_offset_calculation_timeout(std::chrono::milliseconds timeout);
    void enable_upstream_sync_monitoring(bool enable);
    void set_offset_threshold(double threshold_ns);

    // Callbacks
    void set_offset_update_callback(OffsetUpdateCallback callback);
    void set_state_change_callback(StateChangeCallback callback);

    // Timing control
    void tick(); ///< Called periodically for timeout management

    // Statistics
    struct Statistics {
        uint64_t upstream_sync_processed = 0;
        uint64_t offset_calculations_completed = 0;
        uint64_t offset_calculation_timeouts = 0;
        TimeInterval max_upstream_offset = 0;
        TimeInterval min_upstream_offset = 0;
        double average_offset_stability = 0.0;
        std::chrono::steady_clock::time_point last_offset_update;
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Announce Receipt Timeout State Machine per IEEE 802.1AS-2021 Section 10.2.3
 * @details Handles Announce message timeout detection and recovery
 */
class AnnounceReceiptTimeoutStateMachine {
public:
    /**
     * @brief Timeout states per IEEE 802.1AS-2021 Figure 10-10
     */
    enum class State {
        LISTENING,              ///< Listening for Announce messages
        ANNOUNCE_RECEIPT_TIMEOUT_EXPIRES, ///< Announce timeout expired
        WAITING_FOR_ANNOUNCE,   ///< Waiting for valid Announce
        RECOVERY                ///< Recovering from timeout
    };

    /**
     * @brief Events per IEEE 802.1AS-2021 Section 10.2.3.2
     */
    enum class Event {
        ANNOUNCE_RECEIVED,              ///< Valid Announce message received
        ANNOUNCE_RECEIPT_TIMEOUT_EXPIRED, ///< Announce receipt timeout expired
        RECOVERY_TIMEOUT_EXPIRED,       ///< Recovery timeout expired
        PORT_ROLE_SELECTION_UPDATED,    ///< Port role selection updated
        INITIALIZE                      ///< Initialize event
    };

    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        uint16_t port_number;
        uint8_t announce_receipt_timeout = 3;        ///< Announce receipt timeout multiplier
        std::chrono::milliseconds announce_interval{1000}; ///< Announce interval
        std::chrono::milliseconds recovery_timeout{5000};  ///< Recovery timeout
        bool enable_fast_recovery = true;           ///< Enable fast recovery mode
        uint8_t max_consecutive_timeouts = 5;       ///< Max consecutive timeouts before fault
    };

    /**
     * @brief Timeout information
     */
    struct TimeoutInfo {
        uint32_t consecutive_timeouts = 0;          ///< Consecutive timeout count
        uint32_t total_timeouts = 0;               ///< Total timeout count
        bool in_recovery = false;                  ///< Currently in recovery
        std::chrono::steady_clock::time_point last_announce_received;
        std::chrono::steady_clock::time_point last_timeout_occurred;
        double announce_reception_rate = 0.0;      ///< Announce reception rate
    };

    /**
     * @brief Callbacks
     */
    using TimeoutCallback = std::function<void(const TimeoutInfo&)>;
    using RecoveryCallback = std::function<void(bool recovery_success)>;
    using StateChangeCallback = std::function<void(State old_state, State new_state, Event trigger_event)>;

    explicit AnnounceReceiptTimeoutStateMachine(const Configuration& config);
    ~AnnounceReceiptTimeoutStateMachine();

    // State machine control
    bool start();
    bool stop();
    void reset();

    // Event processing
    void process_event(Event event);
    void process_announce_received();

    // State queries
    State get_current_state() const;
    TimeoutInfo get_timeout_info() const;
    bool is_in_timeout() const;

    // Configuration
    void set_announce_receipt_timeout(uint8_t timeout_multiplier);
    void set_announce_interval(std::chrono::milliseconds interval);
    void enable_fast_recovery(bool enable);

    // Callbacks
    void set_timeout_callback(TimeoutCallback callback);
    void set_recovery_callback(RecoveryCallback callback);
    void set_state_change_callback(StateChangeCallback callback);

    // Timing control
    void tick(); ///< Called periodically for timeout management

    // Statistics
    struct Statistics {
        uint64_t announce_messages_received = 0;
        uint64_t announce_receipt_timeouts = 0;
        uint64_t recovery_attempts = 0;
        uint64_t successful_recoveries = 0;
        std::chrono::steady_clock::time_point last_announce_received;
        double average_announce_interval = 0.0;
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

} // namespace ieee_802_1as_2021

#endif // IEEE_802_1AS_2021_SYNC_STATE_MACHINE_H
