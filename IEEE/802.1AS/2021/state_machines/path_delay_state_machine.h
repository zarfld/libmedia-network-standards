/**
 * @file path_delay_state_machine.h
 * @brief IEEE 802.1AS-2021 Path Delay State Machine Implementation
 * @details Complete peer-to-peer path delay mechanism per IEEE 802.1AS-2021 Section 11
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Complete implementation of IEEE 802.1AS-2021 path delay state machines
 */

#ifndef IEEE_802_1AS_2021_PATH_DELAY_STATE_MACHINE_H
#define IEEE_802_1AS_2021_PATH_DELAY_STATE_MACHINE_H

#include "../core/ieee_802_1as_2021.h"
#include <chrono>
#include <functional>
#include <memory>
#include <map>

namespace ieee_802_1as_2021 {

// Import types from IEEE::_802_1AS::_2021 namespace
using PDelayReqMessage = IEEE::_802_1AS::_2021::PDelayReqMessage;
using PDelayRespMessage = IEEE::_802_1AS::_2021::PDelayRespMessage;
using PDelayRespFollowUpMessage = IEEE::_802_1AS::_2021::PDelayRespFollowUpMessage;
using ClockIdentity = IEEE::_802_1AS::_2021::ClockIdentity;
using PortIdentity = IEEE::_802_1AS::_2021::PortIdentity;
using Timestamp = IEEE::_802_1AS::_2021::Timestamp;
using TimeInterval = IEEE::_802_1AS::_2021::TimeInterval;

/**
 * @brief Path Delay Initiator State Machine per IEEE 802.1AS-2021 Section 11.2.15
 * @details Handles P2P path delay request transmission and response processing
 */
class PathDelayInitiatorStateMachine {
public:
    /**
     * @brief Initiator states per IEEE 802.1AS-2021 Figure 11-8
     */
    enum class State {
        NOT_ENABLED,        ///< Path delay mechanism not enabled
        INITIAL_SEND_PDELAY_REQ, ///< Initial state, send first PDelay_Req
        SEND_PDELAY_REQ,    ///< Send periodic PDelay_Req messages
        WAITING_FOR_PDELAY_RESP, ///< Waiting for PDelay_Resp message
        WAITING_FOR_PDELAY_RESP_FOLLOW_UP ///< Waiting for PDelay_Resp_Follow_Up
    };

    /**
     * @brief Events per IEEE 802.1AS-2021 Section 11.2.15.2
     */
    enum class Event {
        PDELAY_INTERVAL_TIMER_EXPIRED, ///< PDelay interval timer expired
        PDELAY_RESP_RECEIPT_TIMEOUT_EXPIRED, ///< PDelay response receipt timeout
        PDELAY_RESP_RECEIVED,         ///< Valid PDelay_Resp received
        PDELAY_RESP_FOLLOW_UP_RECEIVED, ///< Valid PDelay_Resp_Follow_Up received
        PORT_OPER_CHANGED,            ///< Port operational status changed
        ASYMMETRY_MEASUREMENT_MODE_CHANGED, ///< Asymmetry measurement mode changed
        INITIALIZE                    ///< Initialize event
    };

    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        uint16_t port_number;
        std::chrono::milliseconds pdelay_request_interval{1000}; ///< 1 second default
        std::chrono::milliseconds pdelay_resp_receipt_timeout{10000}; ///< 10 second default
        bool compute_neighbor_rate_ratio = true;
        bool compute_neighbor_prop_delay = true;
        int64_t neighbor_prop_delay_thresh = 800000; ///< 800μs threshold
        uint8_t allowed_lost_responses = 3; ///< Consecutive lost responses threshold
        bool enable_multiple_pdelay_resp_detection = true; ///< Milan profile requirement
    };

    /**
     * @brief Path delay measurement result
     */
    struct PathDelayResult {
        TimeInterval propagation_delay;    ///< One-way propagation delay (ns)
        double neighbor_rate_ratio;       ///< Neighbor clock rate ratio
        bool valid = false;               ///< Result validity
        uint16_t sequence_id;             ///< Sequence ID of measurement
        std::chrono::steady_clock::time_point timestamp; ///< Measurement timestamp
    };

    /**
     * @brief Multiple PDelay Response Detection (Milan Profile requirement)
     */
    struct MultiplePDelayRespInfo {
        std::map<ClockIdentity, uint16_t> response_counts; ///< Responses per clock identity
        uint16_t consecutive_multiple_resp_count = 0;      ///< Consecutive occurrences
        bool cessation_active = false;                     ///< Transmission cessation active
        std::chrono::steady_clock::time_point cessation_start; ///< Cessation start time
    };

    /**
     * @brief Callbacks for hardware interface
     */
    using PathDelayResultCallback = std::function<void(const PathDelayResult&)>;
    using MessageTransmitCallback = std::function<bool(const PDelayReqMessage&)>;
    using StateChangeCallback = std::function<void(State old_state, State new_state, Event trigger_event)>;

    explicit PathDelayInitiatorStateMachine(const Configuration& config);
    ~PathDelayInitiatorStateMachine();

    // State machine control
    bool start();
    bool stop();
    void reset();

    // Event processing
    void process_event(Event event);
    void process_pdelay_resp(const PDelayRespMessage& message, const Timestamp& receive_timestamp);
    void process_pdelay_resp_follow_up(const PDelayRespFollowUpMessage& message);

    // State queries
    State get_current_state() const;
    bool is_enabled() const;
    bool is_measuring() const;

    // Results and statistics
    PathDelayResult get_last_result() const;
    MultiplePDelayRespInfo get_multiple_resp_info() const;

    // Configuration
    void set_pdelay_request_interval(std::chrono::milliseconds interval);
    void set_neighbor_prop_delay_thresh(int64_t threshold_ns);
    void enable_multiple_resp_detection(bool enable);

    // Callbacks
    void set_path_delay_result_callback(PathDelayResultCallback callback);
    void set_message_transmit_callback(MessageTransmitCallback callback);
    void set_state_change_callback(StateChangeCallback callback);

    // Timing control
    void tick(); ///< Called periodically for timeout handling

    // Statistics
    struct Statistics {
        uint64_t pdelay_req_sent = 0;
        uint64_t pdelay_resp_received = 0;
        uint64_t pdelay_resp_follow_up_received = 0;
        uint64_t response_timeouts = 0;
        uint64_t invalid_responses = 0;
        uint64_t successful_measurements = 0;
        uint64_t multiple_resp_events = 0;
        uint64_t cessation_periods = 0;
        std::chrono::steady_clock::time_point last_successful_measurement;
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Path Delay Responder State Machine per IEEE 802.1AS-2021 Section 11.2.16
 * @details Handles P2P path delay response generation and follow-up transmission
 */
class PathDelayResponderStateMachine {
public:
    /**
     * @brief Responder states per IEEE 802.1AS-2021 Figure 11-9
     */
    enum class State {
        NOT_ENABLED,        ///< Path delay mechanism not enabled
        WAITING_FOR_PDELAY_REQ, ///< Waiting for PDelay_Req messages
        SEND_PDELAY_RESP,   ///< Send PDelay_Resp message
        WAITING_FOR_TIMESTAMP ///< Waiting for Tx timestamp for follow-up
    };

    /**
     * @brief Events per IEEE 802.1AS-2021 Section 11.2.16.2
     */
    enum class Event {
        PDELAY_REQ_RECEIVED,      ///< Valid PDelay_Req received
        TX_TIMESTAMP_RECEIVED,    ///< Tx timestamp received for response
        PORT_OPER_CHANGED,        ///< Port operational status changed
        INITIALIZE                ///< Initialize event
    };

    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        uint16_t port_number;
        std::chrono::milliseconds tx_timestamp_timeout{100}; ///< Tx timestamp timeout
        bool enable_response_validation = true;     ///< Validate incoming requests
        bool enable_follow_up_transmission = true;  ///< Always send follow-up messages
    };

    /**
     * @brief Response generation result
     */
    struct ResponseInfo {
        uint16_t sequence_id;           ///< Request sequence ID
        PortIdentity requesting_port;   ///< Requesting port identity
        Timestamp request_rx_timestamp; ///< Request receive timestamp
        Timestamp response_tx_timestamp; ///< Response transmit timestamp
        bool valid = false;             ///< Response validity
    };

    /**
     * @brief Callbacks for hardware interface
     */
    using MessageTransmitCallback = std::function<bool(const PDelayRespMessage&)>;
    using FollowUpTransmitCallback = std::function<bool(const PDelayRespFollowUpMessage&)>;
    using StateChangeCallback = std::function<void(State old_state, State new_state, Event trigger_event)>;

    explicit PathDelayResponderStateMachine(const Configuration& config);
    ~PathDelayResponderStateMachine();

    // State machine control
    bool start();
    bool stop();
    void reset();

    // Event processing
    void process_event(Event event);
    void process_pdelay_req(const PDelayReqMessage& message, const Timestamp& receive_timestamp);
    void process_tx_timestamp(uint16_t sequence_id, const Timestamp& tx_timestamp);

    // State queries
    State get_current_state() const;
    bool is_enabled() const;

    // Configuration
    void set_tx_timestamp_timeout(std::chrono::milliseconds timeout);
    void enable_response_validation(bool enable);

    // Callbacks
    void set_message_transmit_callback(MessageTransmitCallback callback);
    void set_follow_up_transmit_callback(FollowUpTransmitCallback callback);
    void set_state_change_callback(StateChangeCallback callback);

    // Timing control
    void tick(); ///< Called periodically for timeout handling

    // Statistics
    struct Statistics {
        uint64_t pdelay_req_received = 0;
        uint64_t pdelay_resp_sent = 0;
        uint64_t pdelay_resp_follow_up_sent = 0;
        uint64_t invalid_requests = 0;
        uint64_t tx_timestamp_timeouts = 0;
        std::chrono::steady_clock::time_point last_response_sent;
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Link Delay Entity per IEEE 802.1AS-2021 Section 11.2.17
 * @details Coordinates path delay measurements and maintains link delay information
 */
class LinkDelayEntity {
public:
    /**
     * @brief Link delay computation states
     */
    enum class State {
        UPDATE_TIME,        ///< Update time intervals
        UPDATE_LINK_DELAY,  ///< Update link delay value
        COMPUTE_PROP_TIME,  ///< Compute propagation time
        UPDATE_PROP_TIME,   ///< Update propagation time
        COMPUTE_MEAN_PROP_DELAY ///< Compute mean propagation delay
    };

    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        uint16_t port_number;
        bool compute_neighbor_rate_ratio = true;
        bool compute_neighbor_prop_delay = true;
        int64_t neighbor_prop_delay_thresh = 800000; ///< 800μs default
        uint8_t filter_size = 8; ///< Moving average filter size
        double rate_ratio_valid_range_min = 0.998; ///< Minimum valid rate ratio
        double rate_ratio_valid_range_max = 1.002; ///< Maximum valid rate ratio
    };

    /**
     * @brief Link delay information
     */
    struct LinkDelayInfo {
        TimeInterval mean_link_delay = 0;      ///< Mean link delay (ns)
        TimeInterval link_delay = 0;           ///< Current link delay (ns)
        double neighbor_rate_ratio = 1.0;     ///< Neighbor rate ratio
        bool as_capable = false;               ///< AS capable determination
        bool valid = false;                    ///< Information validity
        std::chrono::steady_clock::time_point last_update; ///< Last update time
    };

    /**
     * @brief Callbacks
     */
    using LinkDelayUpdateCallback = std::function<void(const LinkDelayInfo&)>;

    explicit LinkDelayEntity(const Configuration& config);
    ~LinkDelayEntity();

    // Control
    void reset();
    void update_path_delay_measurement(const PathDelayInitiatorStateMachine::PathDelayResult& result);

    // Information retrieval
    LinkDelayInfo get_link_delay_info() const;
    bool is_as_capable() const;
    TimeInterval get_mean_link_delay() const;
    double get_neighbor_rate_ratio() const;

    // Configuration
    void set_neighbor_prop_delay_thresh(int64_t threshold_ns);
    void set_filter_size(uint8_t size);
    void set_rate_ratio_valid_range(double min_ratio, double max_ratio);

    // Callbacks
    void set_link_delay_update_callback(LinkDelayUpdateCallback callback);

    // Statistics
    struct Statistics {
        uint64_t measurements_processed = 0;
        uint64_t valid_measurements = 0;
        uint64_t threshold_violations = 0;
        uint64_t rate_ratio_violations = 0;
        TimeInterval min_link_delay = INT64_MAX;
        TimeInterval max_link_delay = INT64_MIN;
        double mean_rate_ratio = 1.0;
        std::chrono::steady_clock::time_point last_valid_measurement;
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

} // namespace ieee_802_1as_2021

#endif // IEEE_802_1AS_2021_PATH_DELAY_STATE_MACHINE_H
