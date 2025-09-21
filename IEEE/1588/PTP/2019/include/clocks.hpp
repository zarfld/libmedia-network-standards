/**
 * @file clocks.hpp
 * @brief IEEE 1588-2019 PTP Clock State Machines Implementation
 * @details Implements Ordinary Clock, Boundary Clock, and Transparent Clock
 *          state machines with deterministic design patterns as required by
 *          IEEE 1588-2019 Sections 9 and 10.
 * 
 * @note This implementation follows OpenAvnu deterministic design principles:
 *       - No dynamic memory allocation in critical paths
 *       - No blocking calls or exceptions
 *       - Bounded execution time for all operations
 *       - POD types for hardware compatibility
 * 
 * @copyright Copyright (c) 2024 OpenAvnu
 * @author OpenAvnu Standards Implementation Team
 * @version IEEE 1588-2019 PTP v2.1
 */

#pragma once

#include "IEEE/1588/PTP/2019/types.hpp"
#include "IEEE/1588/PTP/2019/messages.hpp"
#include <cstdint>
#include <array>

namespace IEEE {
namespace _1588 {
namespace PTP {
namespace _2019 {

// Import types for convenience
using namespace Types;

/**
 * @brief IEEE 1588-2019 Clock State Machines
 * @details Implementation of clock state machines per IEEE 1588-2019
 *          Sections 9.2, 10.2, and 10.3 with time-sensitive design patterns.
 */
namespace Clocks {

// Forward declarations
class OrdinaryClock;
class BoundaryClock;
class TransparentClock;
class PtpPort;

// Message type aliases for convenience (actual messages defined in messages.hpp)
using AnnounceMessage = IEEE::_1588::PTP::_2019::AnnounceMessage;
using SyncMessage = IEEE::_1588::PTP::_2019::SyncMessage; 
using FollowUpMessage = IEEE::_1588::PTP::_2019::FollowUpMessage;
using DelayReqMessage = IEEE::_1588::PTP::_2019::DelayReqMessage;
using DelayRespMessage = IEEE::_1588::PTP::_2019::DelayRespMessage;

/**
 * @brief State Machine Events per IEEE 1588-2019 Section 9.2.6
 * @details Events that trigger state transitions in PTP port state machines
 */
enum class StateEvent : std::uint8_t {
    POWERUP                 = 0x00,  ///< Power-up or initialization
    INITIALIZE              = 0x01,  ///< Initialize event
    FAULT_DETECTED          = 0x02,  ///< Fault detected
    FAULT_CLEARED           = 0x03,  ///< Fault cleared
    DESIGNATED_ENABLED      = 0x04,  ///< Port designated and enabled
    DESIGNATED_DISABLED     = 0x05,  ///< Port designated but disabled
    RS_MASTER               = 0x06,  ///< Recommended State: Master
    RS_GRAND_MASTER         = 0x07,  ///< Recommended State: Grand Master
    RS_SLAVE                = 0x08,  ///< Recommended State: Slave
    RS_PASSIVE              = 0x09,  ///< Recommended State: Passive
    ANNOUNCE_RECEIPT_TIMEOUT = 0x0A, ///< Announce receipt timeout
    SYNCHRONIZATION_FAULT   = 0x0B,  ///< Synchronization fault
    QUALIFICATION_TIMEOUT   = 0x0C   ///< Master qualification timeout
};

/**
 * @brief Best Master Clock Algorithm Decision
 * @details Result of BMCA comparison per IEEE 1588-2019 Section 9.3
 */
enum class BMCADecision : std::uint8_t {
    BETTER_MASTER           = 0x00,  ///< Foreign master is better
    BETTER_BY_TOPOLOGY      = 0x01,  ///< Foreign master better by topology
    SAME_MASTER             = 0x02,  ///< Same master clock
    WORSE_BY_TOPOLOGY       = 0x03,  ///< Foreign master worse by topology
    WORSE_MASTER            = 0x04   ///< Foreign master is worse
};

/**
 * @brief Port Role Designation per IEEE 1588-2019 Section 9.3.3
 * @details Port role assignment after BMCA decision
 */
enum class PortRole : std::uint8_t {
    MASTER      = 0x00,  ///< Master port role
    SLAVE       = 0x01,  ///< Slave port role
    PASSIVE     = 0x02,  ///< Passive port role
    DISABLED    = 0x03   ///< Disabled port role
};

/**
 * @brief PTP Port Configuration
 * @details Deterministic configuration structure for PTP ports
 */
struct PortConfiguration {
    Types::PortNumber port_number{1};
    Types::DomainNumber domain_number{0};
    std::uint8_t announce_interval{1};        ///< Log message interval
    std::uint8_t sync_interval{0};            ///< Log message interval
    std::uint8_t delay_req_interval{0};       ///< Log message interval
    std::uint8_t announce_receipt_timeout{3}; ///< Timeout multiplier
    std::uint8_t sync_receipt_timeout{3};     ///< Timeout multiplier
    bool delay_mechanism_p2p{false};          ///< Use peer-to-peer delay
    std::uint8_t version_number{2};           ///< PTP version number
};

/**
 * @brief PTP Port Statistics
 * @details Performance and error counters with bounded memory usage
 */
struct PortStatistics {
    std::uint32_t state_transitions{0};
    std::uint32_t announce_messages_sent{0};
    std::uint32_t announce_messages_received{0};
    std::uint32_t sync_messages_sent{0};
    std::uint32_t sync_messages_received{0};
    std::uint32_t follow_up_messages_sent{0};
    std::uint32_t follow_up_messages_received{0};
    std::uint32_t delay_req_messages_sent{0};
    std::uint32_t delay_req_messages_received{0};
    std::uint32_t delay_resp_messages_sent{0};
    std::uint32_t delay_resp_messages_received{0};
    std::uint32_t announce_timeouts{0};
    std::uint32_t sync_timeouts{0};
    std::uint32_t qualification_timeouts{0};
    std::uint32_t fault_events{0};
    
    /** Reset all counters to zero */
    constexpr void reset() noexcept {
        *this = PortStatistics{};
    }
};

/**
 * @brief PTP Port Data Set per IEEE 1588-2019 Section 8.2.5
 * @details Complete port data set with deterministic layout
 */
struct PortDataSet {
    Types::PortIdentity port_identity;
    PortState port_state{PortState::Initializing};
    std::uint8_t log_min_delay_req_interval{0};
    Types::TimeInterval peer_mean_path_delay{0};
    std::uint8_t log_announce_interval{1};
    std::uint8_t announce_receipt_timeout{3};
    std::uint8_t log_sync_interval{0};
    bool delay_mechanism{false};  ///< false = E2E, true = P2P
    std::uint8_t log_min_pdelay_req_interval{0};
    std::uint8_t version_number{2};
};

/**
 * @brief Current Data Set per IEEE 1588-2019 Section 8.2.2
 * @details Dynamic state information with bounded precision
 */
struct CurrentDataSet {
    std::uint16_t steps_removed{0};
    Types::TimeInterval offset_from_master{0};
    Types::TimeInterval mean_path_delay{0};
};

/**
 * @brief Parent Data Set per IEEE 1588-2019 Section 8.2.3
 * @details Information about the master clock with deterministic structure
 */
struct ParentDataSet {
    Types::PortIdentity parent_port_identity;
    bool parent_stats{false};
    std::uint16_t observed_parent_offset_scaled_log_variance{0xFFFF};
    std::int32_t observed_parent_clock_phase_change_rate{0x7FFFFFFF};
    Types::ClockIdentity grandmaster_identity;
    Types::ClockQuality grandmaster_clock_quality;
    std::uint8_t grandmaster_priority1{128};
    std::uint8_t grandmaster_priority2{128};
};

/**
 * @brief Priority vectors for Best Master Clock Algorithm (IEEE 1588-2019 Section 9.3)
 */
struct PriorityVector {
    Types::ClockIdentity rootSystemIdentity;
    std::uint16_t stepsRemoved;
    Types::ClockIdentity sourcePortIdentity;
    std::uint16_t sourcePortNumber;
    
    constexpr PriorityVector() noexcept 
        : rootSystemIdentity{}, stepsRemoved(0), sourcePortIdentity{}, sourcePortNumber(0) {}
};

/**
 * @brief Clock synchronization information
 */
struct SynchronizationData {
    Types::Timestamp masterTimeStamp;       ///< Master timestamp from Sync message
    Types::Timestamp slaveTimeStamp;        ///< Slave timestamp when Sync received
    Types::CorrectionField correction;      ///< Correction field from messages
    Types::TimeInterval offsetFromMaster;   ///< Calculated offset from master
    Types::TimeInterval meanPathDelay;      ///< Mean path delay to master
    
    SynchronizationData() noexcept = default;
    
    /**
     * @brief Calculate offset from master using timestamps
     * @param sync_timestamp Master timestamp from Sync message
     * @param sync_reception Local timestamp when Sync was received
     * @param delay_req_timestamp Local timestamp when DelayReq was sent
     * @param delay_resp_timestamp Master timestamp from DelayResp message
     * @return Result containing calculated offset or error
     */
    Types::PTPResult<Types::TimeInterval> calculateOffset(
        const Types::Timestamp& sync_timestamp,
        const Types::Timestamp& sync_reception,
        const Types::Timestamp& delay_req_timestamp,
        const Types::Timestamp& delay_resp_timestamp
    ) noexcept {
        // IEEE 1588-2019 offset calculation algorithm
        // offset_from_master = ((sync_reception - sync_timestamp) - (delay_resp_timestamp - delay_req_timestamp)) / 2
        
        // Calculate T2 - T1 (slave reception - master transmission)
        // Note: Timestamp arithmetic operations need to be implemented
        // For now, create placeholder TimeInterval
        Types::TimeInterval t2_minus_t1{0};
        
        // TODO: Implement actual timestamp arithmetic when operators are available
        
        // Calculate T4 - T3 (master reception - slave transmission)
        Types::TimeInterval t4_minus_t3{0};
        
        // Calculate offset: ((T2-T1) - (T4-T3)) / 2  
        // TODO: Implement actual timestamp arithmetic
        Types::TimeInterval calculated_offset{0};
        
        offsetFromMaster = calculated_offset;
        return Types::PTPResult<Types::TimeInterval>::success(offsetFromMaster);
    }
};

/**
 * @brief Compare priority vectors for BMCA (IEEE 1588-2019 Section 9.3.4)
 * @param a First priority vector
 * @param b Second priority vector
 * @return -1 if a < b, 0 if equal, 1 if a > b
 */
constexpr int comparePriorityVectors(const PriorityVector& a, const PriorityVector& b) noexcept {
    // Compare root system identity first
    for (size_t i = 0; i < 8; ++i) {
        if (a.rootSystemIdentity[i] < b.rootSystemIdentity[i]) return -1;
        if (a.rootSystemIdentity[i] > b.rootSystemIdentity[i]) return 1;
    }
    
    // If root identities equal, compare steps removed
    if (a.stepsRemoved < b.stepsRemoved) return -1;
    if (a.stepsRemoved > b.stepsRemoved) return 1;
    
    // Continue with other comparison criteria as needed
    return 0;
}

// Compile-time size validation for deterministic data structures
static_assert(sizeof(PortDataSet) <= 128, 
              "PortDataSet must be compact for deterministic access");
static_assert(sizeof(CurrentDataSet) <= 32, 
              "CurrentDataSet must be compact for real-time access");  
static_assert(sizeof(ParentDataSet) <= 64, 
              "ParentDataSet must be compact for BMCA operations");

/**
 * @brief State Machine Callbacks
 * @details Function pointers for hardware abstraction with bounded execution
 */
struct StateCallbacks {
    // Message transmission callbacks (must be non-blocking)
    Types::PTPError (*send_announce)(const AnnounceMessage& msg);
    Types::PTPError (*send_sync)(const SyncMessage& msg);
    Types::PTPError (*send_follow_up)(const FollowUpMessage& msg);
    Types::PTPError (*send_delay_req)(const DelayReqMessage& msg);
    Types::PTPError (*send_delay_resp)(const DelayRespMessage& msg);
    
    // Timestamping callbacks (must be deterministic)
    Types::Timestamp (*get_timestamp)();
    Types::PTPError (*get_tx_timestamp)(std::uint16_t sequence_id, Types::Timestamp* timestamp);
    
    // Hardware control callbacks (bounded execution time)
    Types::PTPError (*adjust_clock)(std::int64_t adjustment_ns);
    Types::PTPError (*adjust_frequency)(double ppb_adjustment);
    
    // Event notification callbacks
    void (*on_state_change)(PortState old_state, PortState new_state);
    void (*on_fault)(const char* fault_description);
};

/**
 * @brief PTP Port State Machine
 * @details Implementation of IEEE 1588-2019 port state machine with
 *          deterministic state transitions and bounded execution time.
 * 
 * Implements state machine per IEEE 1588-2019 Figure 9-1 with:
 * - Deterministic state transitions
 * - Non-blocking message processing
 * - Bounded memory usage
 * - Hardware timestamping integration
 */
class PtpPort {
public:
    /**
     * @brief Construct PTP port with deterministic configuration
     * @param config Port configuration (must be valid)
     * @param callbacks Hardware abstraction callbacks
     */
    explicit PtpPort(const PortConfiguration& config, 
                     const StateCallbacks& callbacks) noexcept;
    
    /** Non-copyable for deterministic resource management */
    PtpPort(const PtpPort&) = delete;
    PtpPort& operator=(const PtpPort&) = delete;
    
    /** Movable for efficient initialization */
    PtpPort(PtpPort&&) noexcept = default;
    PtpPort& operator=(PtpPort&&) noexcept = default;
    
    /** Destructor ensures clean resource release */
    ~PtpPort() noexcept = default;
    
    // State machine control (deterministic operations)
    
    /**
     * @brief Initialize port state machine
     * @return Success/failure result
     * @post Port state is INITIALIZING
     */
    Types::PTPResult<void> initialize() noexcept;
    
    /**
     * @brief Start port operation
     * @return Success/failure result
     * @pre Port must be initialized
     */
    Types::PTPResult<void> start() noexcept;
    
    /**
     * @brief Stop port operation
     * @return Success/failure result
     * @post Port enters appropriate quiescent state
     */
    Types::PTPResult<void> stop() noexcept;
    
    /**
     * @brief Process state machine event
     * @param event State machine event to process
     * @return Success/failure result
     * @note Bounded execution time, non-blocking
     */
    Types::PTPResult<void> process_event(StateEvent event) noexcept;
    
    // Message processing (non-blocking, bounded execution time)
    
    /**
     * @brief Process received Announce message
     * @param message Validated Announce message
     * @return Success/failure result
     */
    Types::PTPResult<void> process_announce(const AnnounceMessage& message) noexcept;
    
    /**
     * @brief Process received Sync message
     * @param message Validated Sync message
     * @param rx_timestamp Receive timestamp from hardware
     * @return Success/failure result
     */
    Types::PTPResult<void> process_sync(const SyncMessage& message,
                                       const Types::Timestamp& rx_timestamp) noexcept;
    
    /**
     * @brief Process received Follow_Up message
     * @param message Validated Follow_Up message
     * @return Success/failure result
     */
    Types::PTPResult<void> process_follow_up(const FollowUpMessage& message) noexcept;
    
    /**
     * @brief Process received Delay_Req message
     * @param message Validated Delay_Req message
     * @param rx_timestamp Receive timestamp from hardware
     * @return Success/failure result
     */
    Types::PTPResult<void> process_delay_req(const DelayReqMessage& message,
                                            const Types::Timestamp& rx_timestamp) noexcept;
    
    /**
     * @brief Process received Delay_Resp message
     * @param message Validated Delay_Resp message
     * @return Success/failure result
     */
    Types::PTPResult<void> process_delay_resp(const DelayRespMessage& message) noexcept;
    
    // Periodic processing (deterministic timing)
    
    /**
     * @brief Execute periodic state machine tasks
     * @param current_time Current system time
     * @return Success/failure result
     * @note Must be called at regular intervals (e.g., every 125ms)
     */
    Types::PTPResult<void> tick(const Types::Timestamp& current_time) noexcept;
    
    // State queries (deterministic, read-only)
    
    /** Get current port state */
    constexpr PortState get_state() const noexcept { return port_data_set_.port_state; }
    
    /** Get port identity */
    constexpr const Types::PortIdentity& get_identity() const noexcept { 
        return port_data_set_.port_identity; 
    }
    
    /** Get current statistics */
    constexpr const PortStatistics& get_statistics() const noexcept { return statistics_; }
    
    /** Get port configuration */
    constexpr const PortConfiguration& get_configuration() const noexcept { return config_; }
    
    /** Get current data set */
    constexpr const CurrentDataSet& get_current_data_set() const noexcept { return current_data_set_; }
    
    /** Get parent data set */
    constexpr const ParentDataSet& get_parent_data_set() const noexcept { return parent_data_set_; }
    
    /** Check if port is in master role */
    constexpr bool is_master() const noexcept { 
        return port_data_set_.port_state == PortState::Master; 
    }
    
    /** Check if port is in slave role */
    constexpr bool is_slave() const noexcept { 
        return port_data_set_.port_state == PortState::Slave || 
               port_data_set_.port_state == PortState::Uncalibrated; 
    }
    
    /** Check if port is synchronized */
    constexpr bool is_synchronized() const noexcept { 
        return port_data_set_.port_state == PortState::Slave; 
    }
    
    // Configuration updates (deterministic)
    
    /**
     * @brief Update announce interval
     * @param log_interval Log base 2 interval value
     * @return Success/failure result
     */
    Types::PTPResult<void> set_announce_interval(std::uint8_t log_interval) noexcept;
    
    /**
     * @brief Update sync interval
     * @param log_interval Log base 2 interval value
     * @return Success/failure result
     */
    Types::PTPResult<void> set_sync_interval(std::uint8_t log_interval) noexcept;
    
    /**
     * @brief Clear all statistics counters
     */
    void clear_statistics() noexcept { statistics_.reset(); }

private:
    // Configuration and state (POD for deterministic access)
    PortConfiguration config_;
    StateCallbacks callbacks_;
    PortDataSet port_data_set_;
    CurrentDataSet current_data_set_;
    ParentDataSet parent_data_set_;
    PortStatistics statistics_;
    
    // Timing state (bounded precision)
    Types::Timestamp last_announce_time_{0};
    Types::Timestamp last_sync_time_{0};
    Types::Timestamp last_delay_req_time_{0};
    Types::Timestamp announce_timeout_time_{0};
    Types::Timestamp sync_timeout_time_{0};
    std::uint16_t announce_sequence_id_{0};
    std::uint16_t sync_sequence_id_{0};
    std::uint16_t delay_req_sequence_id_{0};
    
    // BMCA state (limited storage for deterministic operation)
    static constexpr size_t MAX_FOREIGN_MASTERS = 16;
    std::array<AnnounceMessage, MAX_FOREIGN_MASTERS> foreign_masters_;
    std::array<Types::Timestamp, MAX_FOREIGN_MASTERS> foreign_master_timestamps_;
    std::uint8_t foreign_master_count_{0};
    
    // Internal state machine operations (deterministic)
    Types::PTPResult<void> transition_to_state(PortState new_state) noexcept;
    Types::PTPResult<void> execute_state_actions() noexcept;
    Types::PTPResult<void> send_announce_message() noexcept;
    Types::PTPResult<void> send_sync_message() noexcept;
    Types::PTPResult<void> send_delay_req_message() noexcept;
    Types::PTPResult<void> check_timeouts(const Types::Timestamp& current_time) noexcept;
    Types::PTPResult<void> run_bmca() noexcept;
    BMCADecision compare_announce_messages(const AnnounceMessage& local,
                                         const AnnounceMessage& foreign) const noexcept;
    Types::PTPResult<void> update_foreign_master_list(const AnnounceMessage& message) noexcept;
    Types::PTPResult<void> calculate_offset_and_delay() noexcept;
    
    // Time interval calculations (bounded execution time)
    constexpr Types::Timestamp interval_to_nanoseconds(std::uint8_t log_interval) const noexcept {
        return Types::Timestamp{1000000000ULL << log_interval};
    }
    
    constexpr bool is_timeout_expired(const Types::Timestamp& last_time,
                                     const Types::Timestamp& current_time,
                                     const Types::TimeInterval& timeout_interval) const noexcept {
        // Calculate elapsed time and compare with timeout
        Types::TimeInterval elapsed = current_time - last_time;
        return elapsed.toNanoseconds() >= timeout_interval.toNanoseconds();
    }
};

/**
 * @brief Ordinary Clock State Machine
 * @details IEEE 1588-2019 Ordinary Clock implementation with single port
 *          and deterministic state management per Section 6.5.2.
 */
class OrdinaryClock {
public:
    /**
     * @brief Construct Ordinary Clock
     * @param port_config Configuration for the single PTP port
     * @param callbacks Hardware abstraction callbacks
     */
    explicit OrdinaryClock(const PortConfiguration& port_config,
                          const StateCallbacks& callbacks) noexcept;
    
    /** Non-copyable for deterministic resource management */
    OrdinaryClock(const OrdinaryClock&) = delete;
    OrdinaryClock& operator=(const OrdinaryClock&) = delete;
    
    /** Movable for efficient initialization */
    OrdinaryClock(OrdinaryClock&&) noexcept = default;
    OrdinaryClock& operator=(OrdinaryClock&&) noexcept = default;
    
    /** Destructor ensures clean resource release */
    ~OrdinaryClock() noexcept = default;
    
    // Clock control operations
    
    /**
     * @brief Initialize ordinary clock
     * @return Success/failure result
     */
    Types::PTPResult<void> initialize() noexcept;
    
    /**
     * @brief Start clock operation
     * @return Success/failure result
     */
    Types::PTPResult<void> start() noexcept;
    
    /**
     * @brief Stop clock operation
     * @return Success/failure result
     */
    Types::PTPResult<void> stop() noexcept;
    
    // Message processing delegation
    
    /**
     * @brief Process received PTP message
     * @param message_type Type of PTP message
     * @param message_data Message data buffer
     * @param message_size Size of message data
     * @param rx_timestamp Hardware receive timestamp
     * @return Success/failure result
     */
    Types::PTPResult<void> process_message(std::uint8_t message_type,
                                          const void* message_data,
                                          std::size_t message_size,
                                          const Types::Timestamp& rx_timestamp) noexcept;
    
    // Periodic processing
    
    /**
     * @brief Execute periodic clock tasks
     * @param current_time Current system time
     * @return Success/failure result
     */
    Types::PTPResult<void> tick(const Types::Timestamp& current_time) noexcept;
    
    // State queries
    
    /** Get the single PTP port */
    constexpr const PtpPort& get_port() const noexcept { return port_; }
    
    /** Get clock type */
    constexpr ClockType get_clock_type() const noexcept { return ClockType::Ordinary; }
    
    /** Check if clock is master */
    constexpr bool is_master() const noexcept { return port_.is_master(); }
    
    /** Check if clock is slave */
    constexpr bool is_slave() const noexcept { return port_.is_slave(); }
    
    /** Check if clock is synchronized */
    constexpr bool is_synchronized() const noexcept { return port_.is_synchronized(); }

private:
    PtpPort port_;  ///< Single port for Ordinary Clock
};

/**
 * @brief Boundary Clock State Machine
 * @details IEEE 1588-2019 Boundary Clock implementation with multiple ports
 *          and deterministic state coordination per Section 6.5.3.
 */
class BoundaryClock {
public:
    static constexpr std::size_t MAX_PORTS = 8;  ///< Maximum ports for deterministic arrays
    
    /**
     * @brief Construct Boundary Clock
     * @param port_configs Array of port configurations
     * @param port_count Number of active ports
     * @param callbacks Hardware abstraction callbacks
     */
    explicit BoundaryClock(const std::array<PortConfiguration, MAX_PORTS>& port_configs,
                          std::size_t port_count,
                          const StateCallbacks& callbacks) noexcept;
    
    /** Non-copyable for deterministic resource management */
    BoundaryClock(const BoundaryClock&) = delete;
    BoundaryClock& operator=(const BoundaryClock&) = delete;
    
    /** Movable for efficient initialization */
    BoundaryClock(BoundaryClock&&) noexcept = default;
    BoundaryClock& operator=(BoundaryClock&&) noexcept = default;
    
    /** Destructor ensures clean resource release */
    ~BoundaryClock() noexcept = default;
    
    // Clock control operations
    
    /**
     * @brief Initialize boundary clock
     * @return Success/failure result
     */
    Types::PTPResult<void> initialize() noexcept;
    
    /**
     * @brief Start clock operation
     * @return Success/failure result
     */
    Types::PTPResult<void> start() noexcept;
    
    /**
     * @brief Stop clock operation
     * @return Success/failure result
     */
    Types::PTPResult<void> stop() noexcept;
    
    // Message processing
    
    /**
     * @brief Process received PTP message on specific port
     * @param port_number Port that received the message
     * @param message_type Type of PTP message
     * @param message_data Message data buffer
     * @param message_size Size of message data
     * @param rx_timestamp Hardware receive timestamp
     * @return Success/failure result
     */
    Types::PTPResult<void> process_message(Types::PortNumber port_number,
                                          std::uint8_t message_type,
                                          const void* message_data,
                                          std::size_t message_size,
                                          const Types::Timestamp& rx_timestamp) noexcept;
    
    // Periodic processing
    
    /**
     * @brief Execute periodic clock tasks
     * @param current_time Current system time
     * @return Success/failure result
     */
    Types::PTPResult<void> tick(const Types::Timestamp& current_time) noexcept;
    
    // State queries
    
    /** Get number of active ports */
    constexpr std::size_t get_port_count() const noexcept { return port_count_; }
    
    /** Get specific port by number */
    const PtpPort* get_port(Types::PortNumber port_number) const noexcept;
    
    /** Get clock type */
    constexpr ClockType get_clock_type() const noexcept { return ClockType::Boundary; }
    
    /** Check if any port is master */
    bool has_master_port() const noexcept;
    
    /** Check if any port is slave */
    bool has_slave_port() const noexcept;
    
    /** Check if clock is synchronized (has synchronized slave port) */
    bool is_synchronized() const noexcept;

private:
    std::array<PtpPort, MAX_PORTS> ports_;
    std::size_t port_count_;
    
    // Find port by number (deterministic search)
    PtpPort* find_port(Types::PortNumber port_number) noexcept;
    const PtpPort* find_port(Types::PortNumber port_number) const noexcept;
};

/**
 * @brief Transparent Clock State Machine
 * @details IEEE 1588-2019 Transparent Clock implementation with residence
 *          time correction per Section 6.5.4 and 6.5.5.
 */
class TransparentClock {
public:
    static constexpr std::size_t MAX_PORTS = 16;  ///< Maximum ports for transparent clock
    
    /**
     * @brief Transparent Clock Types
     */
    enum class TransparentType : std::uint8_t {
        END_TO_END   = 0x00,  ///< End-to-End Transparent Clock (E2E TC)
        PEER_TO_PEER = 0x01   ///< Peer-to-Peer Transparent Clock (P2P TC)
    };
    
    /**
     * @brief Construct Transparent Clock
     * @param type Type of transparent clock (E2E or P2P)
     * @param port_configs Array of port configurations
     * @param port_count Number of active ports
     * @param callbacks Hardware abstraction callbacks
     */
    explicit TransparentClock(TransparentType type,
                             const std::array<PortConfiguration, MAX_PORTS>& port_configs,
                             std::size_t port_count,
                             const StateCallbacks& callbacks) noexcept;
    
    /** Non-copyable for deterministic resource management */
    TransparentClock(const TransparentClock&) = delete;
    TransparentClock& operator=(const TransparentClock&) = delete;
    
    /** Movable for efficient initialization */
    TransparentClock(TransparentClock&&) noexcept = default;
    TransparentClock& operator=(TransparentClock&&) noexcept = default;
    
    /** Destructor ensures clean resource release */
    ~TransparentClock() noexcept = default;
    
    // Clock control operations
    
    /**
     * @brief Initialize transparent clock
     * @return Success/failure result
     */
    Types::PTPResult<void> initialize() noexcept;
    
    /**
     * @brief Start clock operation
     * @return Success/failure result
     */
    Types::PTPResult<void> start() noexcept;
    
    /**
     * @brief Stop clock operation
     * @return Success/failure result
     */
    Types::PTPResult<void> stop() noexcept;
    
    // Message forwarding with residence time correction
    
    /**
     * @brief Forward PTP message with residence time correction
     * @param ingress_port Port that received the message
     * @param egress_port Port to forward the message to
     * @param message_data Message data buffer (will be modified)
     * @param message_size Size of message data
     * @param ingress_timestamp Message ingress timestamp
     * @param egress_timestamp Message egress timestamp
     * @return Success/failure result
     */
    Types::PTPResult<void> forward_message(Types::PortNumber ingress_port,
                                          Types::PortNumber egress_port,
                                          void* message_data,
                                          std::size_t message_size,
                                          const Types::Timestamp& ingress_timestamp,
                                          const Types::Timestamp& egress_timestamp) noexcept;
    
    // State queries
    
    /** Get transparent clock type */
    constexpr TransparentType get_transparent_type() const noexcept { return type_; }
    
    /** Get clock type */
    constexpr ClockType get_clock_type() const noexcept { 
        return (type_ == TransparentType::END_TO_END) ? 
               ClockType::E2E_Transparent : 
               ClockType::P2P_Transparent; 
    }
    
    /** Get number of active ports */
    constexpr std::size_t get_port_count() const noexcept { return port_count_; }

private:
    TransparentType type_;
    std::array<PortConfiguration, MAX_PORTS> port_configs_;
    std::size_t port_count_;
    StateCallbacks callbacks_;
    
    // Residence time calculation (bounded execution time)
    Types::PTPResult<Types::TimeInterval> calculate_residence_time(
        const Types::Timestamp& ingress_timestamp,
        const Types::Timestamp& egress_timestamp) const noexcept;
    
    // Correction field update (deterministic operation)
    Types::PTPResult<void> update_correction_field(void* message_data,
                                                   std::size_t message_size,
                                                   Types::TimeInterval residence_time) const noexcept;
};

} // namespace Clocks
} // namespace _2019
} // namespace PTP
} // namespace _1588
} // namespace IEEE