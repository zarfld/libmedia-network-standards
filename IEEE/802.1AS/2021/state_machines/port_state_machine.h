/**
 * @file port_state_machine.h
 * @brief IEEE 802.1AS-2021 Port State Machine Implementation
 * @details Real state machine logic per IEEE 802.1AS-2021 Section 10.2
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Complete implementation of IEEE 802.1AS-2021 port state machines
 */

#ifndef IEEE_802_1AS_2021_PORT_STATE_MACHINE_H
#define IEEE_802_1AS_2021_PORT_STATE_MACHINE_H

#include "../core/ieee_802_1as_2021.h"
#include <chrono>
#include <functional>
#include <memory>

namespace IEEE {
namespace _802_1 {
namespace AS {
namespace _2021 {

// Forward declarations for clean naming
// (These types are defined globally in ieee_802_1as_2021.h without namespace)
class BMCAEngine;
class TimeAwareSystem;

/**
 * @brief Port State Machine per IEEE 802.1AS-2021 Section 10.2
 * @details Implements the complete port state machine logic including:
 *          - BMCA (Best Master Clock Algorithm)
 *          - Port state transitions (INITIALIZING -> FAULTY -> DISABLED -> LISTENING -> PRE_MASTER -> MASTER/SLAVE)
 *          - Announce receipt timeout handling
 *          - Sync receipt timeout handling
 */
class PortStateMachine {
public:
    /**
     * @brief Port states per IEEE 802.1AS-2021 Table 10-2
     */
    enum class State {
        INITIALIZING,    ///< Port is initializing
        FAULTY,         ///< Port has detected a fault
        DISABLED,       ///< Port is administratively disabled
        LISTENING,      ///< Port is listening for Announce messages
        PRE_MASTER,     ///< Port is preparing to become Master
        MASTER,         ///< Port is in Master state
        PASSIVE,        ///< Port is in Passive state (not participating in BMCA)
        UNCALIBRATED,   ///< Port is Slave but not yet calibrated
        SLAVE           ///< Port is in Slave state and synchronized
    };

    /**
     * @brief Events that trigger state transitions per IEEE 802.1AS-2021 Section 10.2.2
     */
    enum class Event {
        POWERUP,                    ///< Port power-up
        INITIALIZE,                 ///< Initialize event
        FAULT_CLEARED,              ///< Fault condition cleared
        DESIGNATED_ENABLED,         ///< Port designated as enabled
        DESIGNATED_DISABLED,        ///< Port designated as disabled
        ANNOUNCE_RECEIPT_TIMEOUT,   ///< Announce receipt timeout (4 * announceInterval)
        SYNC_RECEIPT_TIMEOUT,       ///< Sync receipt timeout
        ANNOUNCE_RECEIVED,          ///< Valid Announce message received
        SYNC_RECEIVED,             ///< Valid Sync message received
        BETTER_MASTER_DETECTED,     ///< Better master clock detected via BMCA
        WORSE_MASTER_DETECTED,      ///< Current master is no longer the best
        RECOMMENDED_STATE_CHANGE    ///< Recommended state change from system
    };

    /**
     * @brief State machine configuration per IEEE 802.1AS-2021
     */
    struct Configuration {
        uint16_t port_number;                    ///< Port number
        bool as_capable = true;                  ///< AS capability flag
        uint8_t announce_receipt_timeout = 3;    ///< Announce receipt timeout multiplier
        uint8_t sync_receipt_timeout = 3;        ///< Sync receipt timeout multiplier
        std::chrono::milliseconds announce_interval{1000};  ///< Announce interval (log base 2)
        std::chrono::milliseconds sync_interval{125};       ///< Sync interval (log base 2)
        uint8_t domain_number = 0;               ///< gPTP domain number
        bool neighbor_prop_delay_thresh_valid = false; ///< Neighbor propagation delay threshold validity
        int64_t neighbor_prop_delay_thresh = 800000;   ///< Neighbor propagation delay threshold (ns)
    };

    /**
     * @brief State change callback function type
     */
    using StateChangeCallback = std::function<void(State old_state, State new_state, Event trigger_event)>;
    
    /**
     * @brief Message transmission callback function type
     */
    using MessageTransmitCallback = std::function<bool(const AnnounceMessage&)>;
    using SyncTransmitCallback = std::function<bool(const SyncMessage&)>;

    explicit PortStateMachine(const Configuration& config);
    ~PortStateMachine();

    // State machine control
    bool start();
    bool stop();
    void reset();

    // Event processing
    void process_event(Event event);
    void process_announce_message(const AnnounceMessage& message);
    void process_sync_message(const SyncMessage& message);
    void process_follow_up_message(const FollowUpMessage& message);

    // State queries
    State get_current_state() const;
    bool is_master() const;
    bool is_slave() const;
    bool is_synchronized() const;

    // BMCA support
    void set_local_clock_identity(const ClockIdentity& identity);
    void set_local_clock_quality(const ClockQuality& quality);
    void set_local_priority1(uint8_t priority);
    void set_local_priority2(uint8_t priority);

    // Callbacks
    void set_state_change_callback(StateChangeCallback callback);
    void set_announce_transmit_callback(MessageTransmitCallback callback);
    void set_sync_transmit_callback(SyncTransmitCallback callback);

    // Timing control
    void tick(); ///< Called periodically for timeout handling

    // Statistics and diagnostics
    struct Statistics {
        uint64_t state_transitions = 0;
        uint64_t announce_messages_received = 0;
        uint64_t announce_messages_transmitted = 0;
        uint64_t sync_messages_received = 0;
        uint64_t sync_messages_transmitted = 0;
        uint64_t announce_receipt_timeouts = 0;
        uint64_t sync_receipt_timeouts = 0;
        uint64_t bmca_evaluations = 0;
        std::chrono::steady_clock::time_point last_announce_rx;
        std::chrono::steady_clock::time_point last_sync_rx;
    };

    Statistics get_statistics() const;
    void clear_statistics();

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief BMCA (Best Master Clock Algorithm) Implementation per IEEE 802.1AS-2021 Section 10.3
 * @details Complete implementation of the Best Master Clock Algorithm including:
 *          - Data set comparison algorithm (IEEE 1588-2019 Section 9.3.2.5)
 *          - State decision algorithm (IEEE 1588-2019 Section 9.3.3)
 *          - gPTP profile specific modifications (IEEE 802.1AS-2021 Section 10.3.2)
 */
class BMCAEngine {
public:
    /**
     * @brief BMCA comparison result
     */
    enum class ComparisonResult {
        A_BETTER_THAN_B,    ///< Dataset A is better than dataset B
        B_BETTER_THAN_A,    ///< Dataset B is better than dataset A
        A_BETTER_BY_TOPOLOGY, ///< A is better due to topology (steps removed)
        B_BETTER_BY_TOPOLOGY, ///< B is better due to topology (steps removed)
        ERROR_1,            ///< Error condition 1 (same clock identity)
        ERROR_2             ///< Error condition 2 (invalid comparison)
    };

    /**
     * @brief Foreign master dataset per IEEE 802.1AS-2021 Section 10.3.8
     */
    struct ForeignMasterDataset {
        ClockIdentity grandmaster_identity;
        ClockQuality grandmaster_clock_quality;
        uint8_t grandmaster_priority1;
        uint8_t grandmaster_priority2;
        uint16_t steps_removed;
        PortIdentity source_port_identity;
        std::chrono::steady_clock::time_point last_announce_time;
        bool valid = false;
    };

    explicit BMCAEngine(uint16_t port_number);
    ~BMCAEngine();

    // BMCA algorithm implementation
    ComparisonResult compare_datasets(const ForeignMasterDataset& dataset_a, 
                                    const ForeignMasterDataset& dataset_b) const;
    
    PortStateMachine::State recommended_state(const std::vector<ForeignMasterDataset>& foreign_masters,
                                            const ForeignMasterDataset& local_dataset) const;

    // Foreign master management
    void update_foreign_master(const AnnounceMessage& announce_msg);
    void age_out_foreign_masters(std::chrono::seconds timeout);
    std::vector<ForeignMasterDataset> get_valid_foreign_masters() const;
    
    // Local dataset management
    void set_local_dataset(const ForeignMasterDataset& dataset);
    ForeignMasterDataset get_local_dataset() const;

    // Best master selection
    ForeignMasterDataset select_best_master() const;
    bool has_valid_best_master() const;

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

} // namespace _2021
} // namespace AS
} // namespace _802_1
} // namespace IEEE

#endif // IEEE_802_1AS_2021_PORT_STATE_MACHINE_H
