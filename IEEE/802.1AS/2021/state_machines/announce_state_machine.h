/**
 * @file announce_state_machine.h
 * @brief IEEE 802.1AS-2021 Announce State Machines
 * @details Complete Announce message handling and BMCA state machines per IEEE 802.1AS-2021 Section 10
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Full implementation of IEEE 802.1AS-2021 Announce handling
 */

#ifndef IEEE_802_1AS_2021_ANNOUNCE_STATE_MACHINE_H
#define IEEE_802_1AS_2021_ANNOUNCE_STATE_MACHINE_H

#include "../core/ieee_802_1as_2021.h"
#include <chrono>
#include <functional>
#include <memory>
#include <vector>
#include <map>

namespace ieee_802_1as_2021 {

// Import types from IEEE::_802_1AS::_2021 namespace
using AnnounceMessage = IEEE::_802_1AS::_2021::AnnounceMessage;
using ClockIdentity = IEEE::_802_1AS::_2021::ClockIdentity;
using PortIdentity = IEEE::_802_1AS::_2021::PortIdentity;
using ClockQuality = IEEE::_802_1AS::_2021::ClockQuality;
using Timestamp = IEEE::_802_1AS::_2021::Timestamp;

/**
 * @brief Announce Transmission State Machine per IEEE 802.1AS-2021 Section 10.1.4
 * @details Handles periodic Announce message transmission for master ports
 */
class AnnounceTransmissionStateMachine {
public:
    /**
     * @brief Transmission states per IEEE 802.1AS-2021 Figure 10-5
     */
    enum class State {
        INITIALIZING,           ///< State machine initializing
        TRANSMIT_INIT,         ///< Initialize transmission parameters
        TRANSMIT_PERIODIC,     ///< Transmit periodic Announce messages
        TRANSMIT_ANNOUNCE,     ///< Transmitting Announce message
        UPDATE_ANNOUNCE_INFO   ///< Update Announce information
    };

    /**
     * @brief Events per IEEE 802.1AS-2021 Section 10.1.4.2
     */
    enum class Event {
        ANNOUNCE_INTERVAL_TIMER_EXPIRED,    ///< Announce interval timer expired
        SELECTED_STATE_UPDATED,             ///< Port selected state updated
        ANNOUNCE_TRANSMISSION_ENABLED,      ///< Announce transmission enabled
        ANNOUNCE_TRANSMISSION_DISABLED,     ///< Announce transmission disabled
        MASTER_INFO_UPDATED,               ///< Master clock information updated
        INITIALIZE                         ///< Initialize event
    };

    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        uint16_t port_number;
        std::chrono::milliseconds announce_interval{1000};        ///< 1000ms (log2 = 0)
        int8_t initial_log_announce_interval = 0;               ///< Initial log announce interval
        uint8_t priority1 = 248;                               ///< Default priority1
        uint8_t priority2 = 248;                               ///< Default priority2
        ClockQuality clock_quality;                            ///< Local clock quality
        ClockIdentity clock_identity;                          ///< Local clock identity
        uint8_t time_source = 0xA0;                           ///< Internal oscillator
        bool announce_current_utc_offset_valid = false;       ///< UTC offset valid flag
    };

    /**
     * @brief Master clock information
     */
    struct MasterInfo {
        ClockIdentity grandmaster_identity;    ///< Grand master clock identity
        ClockQuality grandmaster_quality;      ///< Grand master clock quality
        uint16_t steps_removed = 0;           ///< Steps removed from grand master
        uint8_t priority1 = 248;              ///< Priority1 value
        uint8_t priority2 = 248;              ///< Priority2 value
        uint8_t time_source = 0xA0;           ///< Time source
        int16_t current_utc_offset = 37;      ///< Current UTC offset (seconds)
        bool utc_offset_valid = false;        ///< UTC offset validity
        Timestamp last_update;                ///< Last update timestamp
    };

    /**
     * @brief Callbacks for message transmission
     */
    using MessageTransmitCallback = std::function<bool(const AnnounceMessage&)>;
    using StateChangeCallback = std::function<void(State old_state, State new_state, Event trigger_event)>;
    using MasterInfoUpdateCallback = std::function<void(const MasterInfo&)>;

    explicit AnnounceTransmissionStateMachine(const Configuration& config);
    ~AnnounceTransmissionStateMachine();

    // State machine control
    bool start();
    bool stop();
    void reset();

    // Event processing
    void process_event(Event event);

    // State queries
    State get_current_state() const;
    bool is_transmitting() const;
    MasterInfo get_master_info() const;

    // Configuration updates
    void update_master_info(const MasterInfo& info);
    void set_announce_interval(std::chrono::milliseconds interval);
    void set_clock_quality(const ClockQuality& quality);
    void set_priorities(uint8_t priority1, uint8_t priority2);

    // Callbacks
    void set_message_transmit_callback(MessageTransmitCallback callback);
    void set_state_change_callback(StateChangeCallback callback);
    void set_master_info_update_callback(MasterInfoUpdateCallback callback);

    // Timing control
    void tick(); ///< Called periodically for timer management

    // Statistics
    struct Statistics {
        uint64_t announce_messages_sent = 0;
        uint64_t master_info_updates = 0;
        uint64_t announce_interval_changes = 0;
        std::chrono::steady_clock::time_point last_announce_transmission;
        double average_transmission_interval = 0.0;
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Announce Reception State Machine per IEEE 802.1AS-2021 Section 10.1.5
 * @details Handles Announce message reception and validation
 */
class AnnounceReceptionStateMachine {
public:
    /**
     * @brief Reception states per IEEE 802.1AS-2021 Figure 10-6
     */
    enum class State {
        INITIALIZING,          ///< State machine initializing
        LISTENING,            ///< Listening for Announce messages
        RECEIVE_ANNOUNCE,     ///< Processing received Announce message
        UPDATE_ANNOUNCE_INFO  ///< Update Announce information from message
    };

    /**
     * @brief Events per IEEE 802.1AS-2021 Section 10.1.5.2
     */
    enum class Event {
        ANNOUNCE_MESSAGE_RECEIVED,      ///< Valid Announce message received
        ANNOUNCE_MESSAGE_TIMEOUT,       ///< Announce message timeout
        FOREIGN_MASTER_INFO_UPDATED,   ///< Foreign master info updated
        SELECTED_STATE_UPDATED,         ///< Port selected state updated
        INITIALIZE                      ///< Initialize event
    };

    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        uint16_t port_number;
        uint8_t announce_receipt_timeout = 3;              ///< Announce receipt timeout multiplier
        std::chrono::milliseconds announce_interval{1000}; ///< Expected announce interval
        bool enable_foreign_master_tracking = true;        ///< Track foreign masters
        uint8_t max_foreign_masters = 16;                 ///< Maximum foreign masters to track
        bool enable_announce_validation = true;            ///< Enable message validation
        bool enable_steps_removed_check = true;           ///< Check steps removed field
        uint16_t max_steps_removed = 255;                 ///< Maximum allowed steps removed
    };

    /**
     * @brief Foreign master information
     */
    struct ForeignMasterInfo {
        PortIdentity port_identity;         ///< Foreign master port identity
        ClockIdentity clock_identity;       ///< Foreign master clock identity
        ClockQuality clock_quality;         ///< Foreign master clock quality
        uint8_t priority1;                  ///< Priority1 value
        uint8_t priority2;                  ///< Priority2 value
        uint16_t steps_removed;             ///< Steps removed from grand master
        uint8_t time_source;               ///< Time source
        int16_t current_utc_offset;        ///< UTC offset
        bool utc_offset_valid;             ///< UTC offset validity
        uint16_t announce_count = 0;        ///< Announce messages received
        std::chrono::steady_clock::time_point last_announce_received;
        std::chrono::steady_clock::time_point first_announce_received;
        bool qualified = false;             ///< Master qualification status
        
        // Comparison operator for BMCA
        bool operator<(const ForeignMasterInfo& other) const;
        bool is_better_than(const ForeignMasterInfo& other) const;
    };

    /**
     * @brief Reception information
     */
    struct ReceptionInfo {
        uint32_t total_announces_received = 0;
        uint32_t valid_announces_received = 0;
        uint32_t invalid_announces_received = 0;
        uint32_t foreign_masters_discovered = 0;
        std::chrono::steady_clock::time_point last_announce_received;
        double announce_reception_rate = 0.0;
        std::map<PortIdentity, ForeignMasterInfo> foreign_masters;
    };

    /**
     * @brief Callbacks
     */
    using AnnounceReceivedCallback = std::function<void(const AnnounceMessage&, const Timestamp&)>;
    using ForeignMasterDiscoveredCallback = std::function<void(const ForeignMasterInfo&)>;
    using ForeignMasterQualifiedCallback = std::function<void(const ForeignMasterInfo&)>;
    using StateChangeCallback = std::function<void(State old_state, State new_state, Event trigger_event)>;

    explicit AnnounceReceptionStateMachine(const Configuration& config);
    ~AnnounceReceptionStateMachine();

    // State machine control
    bool start();
    bool stop();
    void reset();

    // Event processing
    void process_event(Event event);
    void process_announce_message(const AnnounceMessage& message, const Timestamp& receive_timestamp);

    // State queries
    State get_current_state() const;
    ReceptionInfo get_reception_info() const;
    std::vector<ForeignMasterInfo> get_qualified_foreign_masters() const;

    // Foreign master queries
    bool has_foreign_master(const PortIdentity& port_id) const;
    ForeignMasterInfo get_foreign_master(const PortIdentity& port_id) const;
    ForeignMasterInfo get_best_foreign_master() const;

    // Configuration
    void set_announce_receipt_timeout(uint8_t timeout_multiplier);
    void set_max_foreign_masters(uint8_t max_masters);
    void enable_foreign_master_tracking(bool enable);

    // Callbacks
    void set_announce_received_callback(AnnounceReceivedCallback callback);
    void set_foreign_master_discovered_callback(ForeignMasterDiscoveredCallback callback);
    void set_foreign_master_qualified_callback(ForeignMasterQualifiedCallback callback);
    void set_state_change_callback(StateChangeCallback callback);

    // Timing control
    void tick(); ///< Called periodically for timeout management

    // Statistics
    struct Statistics {
        uint64_t announce_messages_received = 0;
        uint64_t announce_messages_validated = 0;
        uint64_t announce_messages_rejected = 0;
        uint64_t foreign_masters_discovered = 0;
        uint64_t foreign_masters_qualified = 0;
        uint64_t foreign_masters_expired = 0;
        std::chrono::steady_clock::time_point last_announce_received;
        double average_announce_interval = 0.0;
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Best Master Clock Algorithm State Machine per IEEE 802.1AS-2021 Section 10.3.5
 * @details Implements complete BMCA with IEEE 802.1AS-2021 extensions
 */
class BMCAStateMachine {
public:
    /**
     * @brief BMCA states per IEEE 802.1AS-2021 Figure 10-13
     */
    enum class State {
        INITIALIZING,           ///< State machine initializing
        DESIGNATION_SELECTION,  ///< Performing designation selection
        MASTER_SELECTION,       ///< Performing master selection
        STATE_SELECTION,        ///< Performing state selection
        UPDATE_STATE_MACHINES  ///< Update related state machines
    };

    /**
     * @brief Events per IEEE 802.1AS-2021 Section 10.3.5.2
     */
    enum class Event {
        FOREIGN_MASTER_INFO_UPDATED,    ///< Foreign master information updated
        LOCAL_CLOCK_INFO_UPDATED,       ///< Local clock information updated
        PORT_STATE_INFO_UPDATED,        ///< Port state information updated
        RESELECT_TIMER_EXPIRED,         ///< Reselect timer expired
        QUALIFICATION_TIMEOUT_EXPIRED,   ///< Qualification timeout expired
        FORCED_RESELECTION,             ///< Forced reselection event
        INITIALIZE                      ///< Initialize event
    };

    /**
     * @brief Port roles per IEEE 802.1AS-2021 Section 10.3
     */
    enum class PortRole {
        INITIALIZING,    ///< Port is initializing
        FAULTY,         ///< Port is faulty
        DISABLED,       ///< Port is disabled
        LISTENING,      ///< Port is listening
        PRE_MASTER,     ///< Port is pre-master
        MASTER,         ///< Port is master
        PASSIVE,        ///< Port is passive
        UNCALIBRATED,   ///< Port is uncalibrated
        SLAVE           ///< Port is slave
    };

    /**
     * @brief BMCA decision information
     */
    struct BMCADecision {
        PortIdentity selected_master_port;     ///< Selected master port
        ClockIdentity selected_grandmaster;    ///< Selected grandmaster
        PortRole recommended_role;             ///< Recommended port role
        uint16_t steps_removed;               ///< Steps from grandmaster
        bool local_clock_is_best = false;     ///< Local clock is best
        bool role_changed = false;            ///< Port role changed
        std::chrono::steady_clock::time_point decision_time;
    };

    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        uint16_t port_number;
        ClockIdentity local_clock_identity;
        ClockQuality local_clock_quality;
        uint8_t local_priority1 = 248;
        uint8_t local_priority2 = 248;
        std::chrono::milliseconds reselect_interval{1000};     ///< Reselect interval
        std::chrono::milliseconds qualification_timeout{2000}; ///< Qualification timeout
        bool enable_alternate_time_offset_indicator = false;   ///< ATOI support
        bool enable_path_trace = false;                       ///< Path trace support
        uint16_t max_steps_removed = 255;                    ///< Maximum steps removed
    };

    /**
     * @brief System information for BMCA
     */
    struct SystemInfo {
        ClockIdentity clock_identity;
        ClockQuality clock_quality;
        uint8_t priority1;
        uint8_t priority2;
        PortIdentity port_identity;
        uint16_t steps_removed;
        uint8_t time_source;
        
        // Comparison methods for BMCA
        int compare_dataset(const SystemInfo& other) const;
        bool is_better_than(const SystemInfo& other) const;
    };

    /**
     * @brief Callbacks
     */
    using BMCADecisionCallback = std::function<void(const BMCADecision&)>;
    using PortRoleChangeCallback = std::function<void(uint16_t port_number, PortRole old_role, PortRole new_role)>;
    using StateChangeCallback = std::function<void(State old_state, State new_state, Event trigger_event)>;
    using ReselectionCallback = std::function<void(bool forced_reselection)>;

    explicit BMCAStateMachine(const Configuration& config);
    ~BMCAStateMachine();

    // State machine control
    bool start();
    bool stop();
    void reset();

    // Event processing
    void process_event(Event event);
    void update_foreign_master_info(const std::vector<AnnounceReceptionStateMachine::ForeignMasterInfo>& masters);
    void update_local_clock_info(const SystemInfo& local_info);

    // State queries
    State get_current_state() const;
    BMCADecision get_current_decision() const;
    PortRole get_recommended_port_role() const;
    SystemInfo get_selected_master_info() const;

    // BMCA operations
    void force_reselection();
    bool is_local_clock_best() const;
    SystemInfo run_bmca_comparison(const std::vector<SystemInfo>& candidates) const;

    // Configuration
    void set_local_clock_info(const SystemInfo& info);
    void set_reselect_interval(std::chrono::milliseconds interval);
    void enable_path_trace(bool enable);

    // Callbacks
    void set_bmca_decision_callback(BMCADecisionCallback callback);
    void set_port_role_change_callback(PortRoleChangeCallback callback);
    void set_state_change_callback(StateChangeCallback callback);
    void set_reselection_callback(ReselectionCallback callback);

    // Timing control
    void tick(); ///< Called periodically for timer management

    // Statistics
    struct Statistics {
        uint64_t bmca_decisions_made = 0;
        uint64_t port_role_changes = 0;
        uint64_t forced_reselections = 0;
        uint64_t qualification_timeouts = 0;
        uint64_t foreign_masters_evaluated = 0;
        std::chrono::steady_clock::time_point last_decision;
        std::chrono::milliseconds average_decision_time{0};
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief State Decision Algorithm per IEEE 802.1AS-2021 Section 10.3.12
 * @details Determines port states based on BMCA decisions and network topology
 */
class StateDecisionAlgorithm {
public:
    /**
     * @brief Port states per IEEE 802.1AS-2021 Section 10.3
     */
    enum class PortState {
        INITIALIZING,    ///< Port initializing
        FAULTY,         ///< Port faulty
        DISABLED,       ///< Port administratively disabled
        LISTENING,      ///< Port listening (learning topology)
        PRE_MASTER,     ///< Port preparing to be master
        MASTER,         ///< Port is master
        PASSIVE,        ///< Port is passive
        UNCALIBRATED,   ///< Port uncalibrated slave
        SLAVE           ///< Port is synchronized slave
    };

    /**
     * @brief State decision parameters
     */
    struct StateDecisionInfo {
        BMCAStateMachine::PortRole recommended_role;   ///< BMCA recommended role
        bool port_enabled = true;                     ///< Port administrative state
        bool asymmetry_measurement_mode = false;     ///< Asymmetry measurement mode
        bool sync_locked = false;                     ///< Sync locked to master
        bool pdelay_allowed = true;                   ///< Path delay allowed
        uint32_t consecutive_sync_timeouts = 0;       ///< Sync timeout count
        uint32_t consecutive_announce_timeouts = 0;   ///< Announce timeout count
        bool link_up = true;                          ///< Physical link status
    };

    /**
     * @brief State transition result
     */
    struct StateTransition {
        PortState old_state;        ///< Previous port state
        PortState new_state;        ///< New port state
        bool state_changed;         ///< Whether state changed
        std::string reason;         ///< Reason for transition
        std::chrono::steady_clock::time_point transition_time;
    };

    /**
     * @brief Callbacks
     */
    using StateTransitionCallback = std::function<void(const StateTransition&)>;
    using PortStateCallback = std::function<void(uint16_t port_number, PortState new_state)>;

    explicit StateDecisionAlgorithm(uint16_t port_number);
    ~StateDecisionAlgorithm();

    // State decision operations
    PortState determine_port_state(const StateDecisionInfo& info);
    StateTransition update_port_state(const StateDecisionInfo& info);

    // State queries
    PortState get_current_state() const;
    bool is_master_state() const;
    bool is_slave_state() const;
    bool is_passive_state() const;
    bool allows_sync_transmission() const;
    bool allows_announce_transmission() const;
    bool allows_pdelay_transmission() const;

    // State control
    void force_state_transition(PortState target_state, const std::string& reason = "");
    void set_port_enabled(bool enabled);
    void set_link_status(bool link_up);

    // Callbacks
    void set_state_transition_callback(StateTransitionCallback callback);
    void set_port_state_callback(PortStateCallback callback);

    // Statistics
    struct Statistics {
        uint64_t state_transitions = 0;
        uint64_t forced_transitions = 0;
        std::map<PortState, uint64_t> state_durations_ms;
        std::chrono::steady_clock::time_point last_transition;
        std::vector<StateTransition> recent_transitions; ///< Last 10 transitions
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

} // namespace ieee_802_1as_2021

#endif // IEEE_802_1AS_2021_ANNOUNCE_STATE_MACHINE_H
