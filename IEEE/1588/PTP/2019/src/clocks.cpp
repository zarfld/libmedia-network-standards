/**
 * @file clocks.cpp
 * @brief IEEE 1588-2019 PTP Clock State Machines Implementation
 * @details Implementation of Ordinary Clock, Boundary Clock, and Transparent Clock
 *          state machines with deterministic design patterns.
 * 
 * @note This implementation follows OpenAvnu deterministic design principles:
 *       - No dynamic memory allocation in critical paths
 *       - No blocking calls or exceptions
 *       - Bounded execution time for all operations
 *       - Hardware timestamping integration
 * 
 * @copyright Copyright (c) 2024 OpenAvnu
 * @author OpenAvnu Standards Implementation Team
 * @version IEEE 1588-2019 PTP v2.1
 */

#include "../include/clocks.hpp"
#include <cstring>
#include <algorithm>

namespace IEEE {
namespace _1588 {
namespace PTP {
namespace _2019 {
namespace Clocks {

// ============================================================================
// PtpPort Implementation
// ============================================================================

PtpPort::PtpPort(const PortConfiguration& config, 
                 const StateCallbacks& callbacks) noexcept 
    : config_(config)
    , callbacks_(callbacks) {
    
    // Initialize port data set per IEEE 1588-2019 Section 8.2.5
    port_data_set_.port_identity.port_number = config.port_number;
    // Clock identity will be set by parent clock
    port_data_set_.port_state = PortState::Initializing;
    port_data_set_.log_min_delay_req_interval = config.delay_req_interval;
    port_data_set_.peer_mean_path_delay = Types::TimeInterval{0};
    port_data_set_.log_announce_interval = config.announce_interval;
    port_data_set_.announce_receipt_timeout = config.announce_receipt_timeout;
    port_data_set_.log_sync_interval = config.sync_interval;
    port_data_set_.delay_mechanism = config.delay_mechanism_p2p;
    port_data_set_.log_min_pdelay_req_interval = 0;
    port_data_set_.version_number = config.version_number;
    
    // Initialize current data set per IEEE 1588-2019 Section 8.2.2
    current_data_set_.steps_removed = 0;
    current_data_set_.offset_from_master = Types::TimeInterval{0};
    current_data_set_.mean_path_delay = Types::TimeInterval{0};
    
    // Initialize parent data set per IEEE 1588-2019 Section 8.2.3
    parent_data_set_.parent_port_identity.port_number = 0;
    parent_data_set_.parent_port_identity.clock_identity.fill(0);
    parent_data_set_.parent_stats = false;
    parent_data_set_.observed_parent_offset_scaled_log_variance = 0xFFFF;
    parent_data_set_.observed_parent_clock_phase_change_rate = 0x7FFFFFFF;
    parent_data_set_.grandmaster_identity.fill(0);
    parent_data_set_.grandmaster_clock_quality.clock_class = 248;
    parent_data_set_.grandmaster_clock_quality.clock_accuracy = 0xFE;
    parent_data_set_.grandmaster_clock_quality.offset_scaled_log_variance = 0xFFFF;
    parent_data_set_.grandmaster_priority1 = 128;
    parent_data_set_.grandmaster_priority2 = 128;
    
    // Initialize foreign master list
    foreign_masters_.fill(AnnounceMessage{});
    foreign_master_timestamps_.fill(Types::Timestamp{0});
    foreign_master_count_ = 0;
}

Types::PTPResult<void> PtpPort::initialize() noexcept {
    // Reset all state to initial values
    port_data_set_.port_state = PortState::Initializing;
    statistics_.reset();
    foreign_master_count_ = 0;
    
    // Reset timing state
    last_announce_time_ = Types::Timestamp{0};
    last_sync_time_ = Types::Timestamp{0};
    last_delay_req_time_ = Types::Timestamp{0};
    announce_timeout_time_ = Types::Timestamp{0};
    sync_timeout_time_ = Types::Timestamp{0};
    
    // Reset sequence IDs
    announce_sequence_id_ = 0;
    sync_sequence_id_ = 0;
    delay_req_sequence_id_ = 0;
    
    return Types::PTPResult<void>{};
}

Types::PTPResult<void> PtpPort::start() noexcept {
    if (port_data_set_.port_state != PortState::Initializing) {
        return Types::PTPResult<void>{Types::PTPError::State_Error};
    }
    
    // Transition to LISTENING state per IEEE 1588-2019 Section 9.2.5
    return transition_to_state(PortState::Listening);
}

Types::PTPResult<void> PtpPort::stop() noexcept {
    // Transition to DISABLED state
    return transition_to_state(PortState::Disabled);
}

Types::PTPResult<void> PtpPort::process_event(StateEvent event) noexcept {
    PortState current_state = port_data_set_.port_state;
    PortState new_state = current_state;
    
    // State machine transitions per IEEE 1588-2019 Figure 9-1
    switch (current_state) {
    case PortState::Initializing:
        switch (event) {
        case StateEvent::INITIALIZE:
            new_state = PortState::Listening;
            break;
        case StateEvent::FAULT_DETECTED:
            new_state = PortState::Faulty;
            break;
        case StateEvent::DESIGNATED_DISABLED:
            new_state = PortState::Disabled;
            break;
        default:
            break;
        }
        break;
        
    case PortState::Faulty:
        switch (event) {
        case StateEvent::FAULT_CLEARED:
            new_state = PortState::Initializing;
            break;
        default:
            break;
        }
        break;
        
    case PortState::Disabled:
        switch (event) {
        case StateEvent::DESIGNATED_ENABLED:
            new_state = PortState::Listening;
            break;
        default:
            break;
        }
        break;
        
    case PortState::Listening:
        switch (event) {
        case StateEvent::RS_MASTER:
        case StateEvent::RS_GRAND_MASTER:
            new_state = PortState::PreMaster;
            break;
        case StateEvent::RS_SLAVE:
            new_state = PortState::Uncalibrated;
            break;
        case StateEvent::RS_PASSIVE:
            new_state = PortState::Passive;
            break;
        case StateEvent::FAULT_DETECTED:
            new_state = PortState::Faulty;
            break;
        case StateEvent::DESIGNATED_DISABLED:
            new_state = PortState::Disabled;
            break;
        default:
            break;
        }
        break;
        
    case PortState::PreMaster:
        switch (event) {
        case StateEvent::QUALIFICATION_TIMEOUT:
            new_state = PortState::Master;
            break;
        case StateEvent::RS_SLAVE:
            new_state = PortState::Uncalibrated;
            break;
        case StateEvent::RS_PASSIVE:
            new_state = PortState::Passive;
            break;
        default:
            break;
        }
        break;
        
    case PortState::Master:
        switch (event) {
        case StateEvent::RS_SLAVE:
            new_state = PortState::Uncalibrated;
            break;
        case StateEvent::RS_PASSIVE:
            new_state = PortState::Passive;
            break;
        default:
            break;
        }
        break;
        
    case PortState::Passive:
        switch (event) {
        case StateEvent::RS_MASTER:
        case StateEvent::RS_GRAND_MASTER:
            new_state = PortState::PreMaster;
            break;
        case StateEvent::RS_SLAVE:
            new_state = PortState::Uncalibrated;
            break;
        default:
            break;
        }
        break;
        
    case PortState::Uncalibrated:
        switch (event) {
        case StateEvent::RS_MASTER:
        case StateEvent::RS_GRAND_MASTER:
            new_state = PortState::PreMaster;
            break;
        case StateEvent::RS_PASSIVE:
            new_state = PortState::Passive;
            break;
        case StateEvent::SYNCHRONIZATION_FAULT:
            new_state = PortState::Listening;
            break;
        case StateEvent::ANNOUNCE_RECEIPT_TIMEOUT:
            new_state = PortState::Listening;
            break;
        default:
            // Check if synchronization is achieved (implementation-specific)
            // For now, transition to SLAVE after receiving valid Sync+Follow_Up
            break;
        }
        break;
        
    case PortState::Slave:
        switch (event) {
        case StateEvent::RS_MASTER:
        case StateEvent::RS_GRAND_MASTER:
            new_state = PortState::PreMaster;
            break;
        case StateEvent::RS_PASSIVE:
            new_state = PortState::Passive;
            break;
        case StateEvent::SYNCHRONIZATION_FAULT:
            new_state = PortState::Uncalibrated;
            break;
        case StateEvent::ANNOUNCE_RECEIPT_TIMEOUT:
            new_state = PortState::Listening;
            break;
        default:
            break;
        }
        break;
    }
    
    // Apply state transition if changed
    if (new_state != current_state) {
        return transition_to_state(new_state);
    }
    
    return Types::PTPResult<void>{};
}

Types::PTPResult<void> PtpPort::transition_to_state(PortState new_state) noexcept {
    PortState old_state = port_data_set_.port_state;
    port_data_set_.port_state = new_state;
    statistics_.state_transitions++;
    
    // State exit actions
    switch (old_state) {
    case PortState::Master:
        // Stop transmitting Announce and Sync messages
        break;
    case PortState::Slave:
    case PortState::Uncalibrated:
        // Stop requesting delay measurements
        break;
    default:
        break;
    }
    
    // State entry actions per IEEE 1588-2019 Section 9.2.5
    switch (new_state) {
    case PortState::Initializing:
        // Clear foreign master list
        foreign_master_count_ = 0;
        break;
        
    case PortState::Listening:
        // Start listening for Announce messages
        // Reset announce timeout
        announce_timeout_time_ = Types::Timestamp{0};
        break;
        
    case PortState::PreMaster:
        // Start qualification timeout (implementation-specific duration)
        // Typically 2 * announce interval
        break;
        
    case PortState::Master:
        // Start transmitting Announce and Sync messages
        last_announce_time_ = Types::Timestamp{0};
        last_sync_time_ = Types::Timestamp{0};
        break;
        
    case PortState::Slave:
    case PortState::Uncalibrated:
        // Start requesting delay measurements
        last_delay_req_time_ = Types::Timestamp{0};
        break;
        
    case PortState::Faulty:
        statistics_.fault_events++;
        if (callbacks_.on_fault) {
            callbacks_.on_fault("Port entered FAULTY state");
        }
        break;
        
    default:
        break;
    }
    
    // Notify state change
    if (callbacks_.on_state_change) {
        callbacks_.on_state_change(old_state, new_state);
    }
    
    return Types::PTPResult<void>{};
}

Types::PTPResult<void> PtpPort::process_announce(const AnnounceMessage& message) noexcept {
    statistics_.announce_messages_received++;
    
    // Update foreign master list
    auto result = update_foreign_master_list(message);
    if (!result.hasValue()) {
        return result;
    }
    
    // Run BMCA if in appropriate state
    if (port_data_set_.port_state == PortState::Listening ||
        port_data_set_.port_state == PortState::PreMaster ||
        port_data_set_.port_state == PortState::Master ||
        port_data_set_.port_state == PortState::Passive ||
        port_data_set_.port_state == PortState::Uncalibrated ||
        port_data_set_.port_state == PortState::Slave) {
        
        return run_bmca();
    }
    
    return Types::PTPResult<void>{};
}

Types::PTPResult<void> PtpPort::process_sync(const SyncMessage& message,
                                            const Types::Timestamp& rx_timestamp) noexcept {
    statistics_.sync_messages_received++;
    
    // Only process in slave states
    if (port_data_set_.port_state != PortState::Uncalibrated &&
        port_data_set_.port_state != PortState::Slave) {
        return Types::PTPResult<void>{};
    }
    
    // Store sync information for offset calculation
    // This is a simplified implementation - full implementation would
    // handle two-step vs one-step clocks, correction fields, etc.
    
    return Types::PTPResult<void>{};
}

Types::PTPResult<void> PtpPort::process_follow_up(const FollowUpMessage& message) noexcept {
    statistics_.follow_up_messages_received++;
    
    // Only process in slave states
    if (port_data_set_.port_state != PortState::Uncalibrated &&
        port_data_set_.port_state != PortState::Slave) {
        return Types::PTPResult<void>::success();
    }
    
    // Calculate offset from master using sync and follow-up timestamps
    auto result = calculate_offset_and_delay();
    if (!result.is_success()) {
        return result;
    }
    
    // Check if we can transition from UNCALIBRATED to SLAVE
    if (port_data_set_.port_state == PortState::Uncalibrated) {
        // Simplified synchronization check - in practice this would
        // involve more sophisticated algorithms
        return transition_to_state(PortState::Slave);
    }
    
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> PtpPort::process_delay_req(const DelayReqMessage& message,
                                                 const Types::Timestamp& rx_timestamp) noexcept {
    statistics_.delay_req_messages_received++;
    
    // Only respond if we are master
    if (port_data_set_.port_state != PortState::Master) {
        return Types::PTPResult<void>::success();
    }
    
    // Send Delay_Resp message
    DelayRespMessage response;
    response.header.setMessageType(MessageType::Delay_Resp);
    response.header.reserved_version = (response.header.reserved_version & 0xF0) | (port_data_set_.version_number & 0x0F);
    response.header.messageLength = sizeof(DelayRespMessage);
    response.header.domainNumber = config_.domain_number;
    response.header.sequenceId = message.header.sequenceId;
    response.header.sourcePortIdentity = port_data_set_.port_identity;
    response.body.receiveTimestamp = rx_timestamp;
    response.body.requestingPortIdentity = message.header.sourcePortIdentity;
    
    if (callbacks_.send_delay_resp) {
        auto result = callbacks_.send_delay_resp(response);
        if (result == Types::PTPError::Success) {
            statistics_.delay_resp_messages_sent++;
            return Types::PTPResult<void>{};
        }
        return Types::PTPResult<void>(result);
    }
    
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> PtpPort::process_delay_resp(const DelayRespMessage& message) noexcept {
    statistics_.delay_resp_messages_received++;
    
    // Only process if we are slave and this response is for us
    if (port_data_set_.port_state != PortState::Uncalibrated &&
        port_data_set_.port_state != PortState::Slave) {
        return Types::PTPResult<void>::success();
    }
    
    // Check if this response matches our request
    if (message.body.requestingPortIdentity.port_number != port_data_set_.port_identity.port_number ||
        std::memcmp(message.body.requestingPortIdentity.clock_identity.data(),
                    port_data_set_.port_identity.clock_identity.data(),
                    Types::CLOCK_IDENTITY_LENGTH) != 0) {
        return Types::PTPResult<void>::success();
    }
    
    // Calculate path delay using delay request/response timestamps
    return calculate_offset_and_delay();
}

Types::PTPResult<void> PtpPort::tick(const Types::Timestamp& current_time) noexcept {
    // Check for timeouts
    auto result = check_timeouts(current_time);
    if (!result.is_success()) {
        return result;
    }
    
    // Execute state-specific actions
    return execute_state_actions();
}

Types::PTPResult<void> PtpPort::execute_state_actions() noexcept {
    switch (port_data_set_.port_state) {
    case PortState::Master:
        {
            // Send periodic Announce messages
            auto announce_result = send_announce_message();
            if (!announce_result.is_success()) {
                return announce_result;
            }
            
            // Send periodic Sync messages
            return send_sync_message();
        }
        
    case PortState::Slave:
    case PortState::Uncalibrated:
        // Send periodic Delay_Req messages (if using E2E delay mechanism)
        if (!port_data_set_.delay_mechanism) {
            return send_delay_req_message();
        }
        break;
        
    default:
        break;
    }
    
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> PtpPort::send_announce_message() noexcept {
    if (!callbacks_.send_announce) {
        return Types::PTPResult<void>::failure(Types::PTPError::Resource_Unavailable);
    }
    
    AnnounceMessage message;
    message.header.setMessageType(MessageType::Announce);
    message.header.setVersion(port_data_set_.version_number);
    message.header.messageLength = sizeof(AnnounceMessage);
    message.header.domainNumber = config_.domain_number;
    message.header.sequenceId = announce_sequence_id_++;
    message.header.sourcePortIdentity = port_data_set_.port_identity;
    
    // Fill announce body with current data
    message.body.originTimestamp = callbacks_.get_timestamp ? callbacks_.get_timestamp() : Types::Timestamp{0};
    message.body.currentUtcOffset = 37; // Current TAI-UTC offset
    message.body.grandmasterPriority1 = parent_data_set_.grandmaster_priority1;
    message.body.grandmasterClockClass = parent_data_set_.grandmaster_clock_quality.clock_class;
    message.body.grandmasterClockAccuracy = parent_data_set_.grandmaster_clock_quality.clock_accuracy;
    message.body.grandmasterClockVariance = parent_data_set_.grandmaster_clock_quality.offset_scaled_log_variance;
    message.body.grandmasterPriority2 = parent_data_set_.grandmaster_priority2;
    message.body.grandmasterIdentity = parent_data_set_.grandmaster_identity;
    message.body.stepsRemoved = current_data_set_.steps_removed;
    message.body.timeSource = static_cast<std::uint8_t>(Types::TimeSource::Internal_Oscillator);
    
    auto error = callbacks_.send_announce(message);
    if (error == Types::PTPError::Success) {
        statistics_.announce_messages_sent++;
        last_announce_time_ = callbacks_.get_timestamp ? callbacks_.get_timestamp() : Types::Timestamp{0};
        return Types::PTPResult<void>::success();
    }
    
    return Types::PTPResult<void>::failure(error);
}

Types::PTPResult<void> PtpPort::send_sync_message() noexcept {
    if (!callbacks_.send_sync) {
        return Types::PTPResult<void>::failure(Types::PTPError::Resource_Unavailable);
    }
    
    SyncMessage message;
    message.header.setMessageType(MessageType::Sync);
    message.header.setVersion(port_data_set_.version_number);
    message.header.messageLength = sizeof(SyncMessage);
    message.header.domainNumber = config_.domain_number;
    message.header.sequenceId = sync_sequence_id_++;
    message.header.sourcePortIdentity = port_data_set_.port_identity;
    
    // Origin timestamp will be filled by hardware or follow-up message
    message.body.originTimestamp = Types::Timestamp{0};
    
    auto error = callbacks_.send_sync(message);
    if (error == Types::PTPError::Success) {
        statistics_.sync_messages_sent++;
        last_sync_time_ = callbacks_.get_timestamp ? callbacks_.get_timestamp() : Types::Timestamp{0};
        return Types::PTPResult<void>::success();
    }
    
    return Types::PTPResult<void>::failure(error);
}

Types::PTPResult<void> PtpPort::send_delay_req_message() noexcept {
    if (!callbacks_.send_delay_req) {
        return Types::PTPResult<void>::failure(Types::PTPError::Resource_Unavailable);
    }
    
    DelayReqMessage message;
    message.header.setMessageType(MessageType::Delay_Req);
    message.header.setVersion(port_data_set_.version_number);
    message.header.messageLength = sizeof(DelayReqMessage);
    message.header.domainNumber = config_.domain_number;
    message.header.sequenceId = delay_req_sequence_id_++;
    message.header.sourcePortIdentity = port_data_set_.port_identity;
    
    // Origin timestamp will be filled by hardware
    message.body.originTimestamp = Types::Timestamp{0};
    
    auto error = callbacks_.send_delay_req(message);
    if (error == Types::PTPError::Success) {
        statistics_.delay_req_messages_sent++;
        last_delay_req_time_ = callbacks_.get_timestamp ? callbacks_.get_timestamp() : Types::Timestamp{0};
        return Types::PTPResult<void>::success();
    }
    
    return Types::PTPResult<void>::failure(error);
}

Types::PTPResult<void> PtpPort::check_timeouts(const Types::Timestamp& current_time) noexcept {
    // Check announce receipt timeout
    if (port_data_set_.port_state == PortState::Slave ||
        port_data_set_.port_state == PortState::Uncalibrated) {
        
        Types::Timestamp announce_timeout = interval_to_nanoseconds(port_data_set_.log_announce_interval) *
                                           port_data_set_.announce_receipt_timeout;
        
        if (is_timeout_expired(last_announce_time_, current_time, announce_timeout)) {
            statistics_.announce_timeouts++;
            return process_event(StateEvent::ANNOUNCE_RECEIPT_TIMEOUT);
        }
    }
    
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> PtpPort::run_bmca() noexcept {
    // Simplified BMCA implementation
    // Full implementation would follow IEEE 1588-2019 Section 9.3
    
    if (foreign_master_count_ == 0) {
        // No foreign masters, remain in current state or become master
        return Types::PTPResult<void>::success();
    }
    
    // For now, just select first foreign master as best
    // In practice, this would involve complete dataset comparison
    if (port_data_set_.port_state == PortState::Listening) {
        return process_event(StateEvent::RS_SLAVE);
    }
    
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> PtpPort::update_foreign_master_list(const AnnounceMessage& message) noexcept {
    // Find existing entry for this clock
    for (std::uint8_t i = 0; i < foreign_master_count_; ++i) {
        if (std::memcmp(foreign_masters_[i].header.sourcePortIdentity.clock_identity.data(),
                        message.header.sourcePortIdentity.clock_identity.data(),
                        Types::CLOCK_IDENTITY_LENGTH) == 0 &&
            foreign_masters_[i].header.sourcePortIdentity.port_number ==
            message.header.sourcePortIdentity.port_number) {
            
            // Update existing entry
            foreign_masters_[i] = message;
            foreign_master_timestamps_[i] = callbacks_.get_timestamp ? callbacks_.get_timestamp() : Types::Timestamp{0};
            return Types::PTPResult<void>::success();
        }
    }
    
    // Add new entry if space available
    if (foreign_master_count_ < MAX_FOREIGN_MASTERS) {
        foreign_masters_[foreign_master_count_] = message;
        foreign_master_timestamps_[foreign_master_count_] = callbacks_.get_timestamp ? callbacks_.get_timestamp() : Types::Timestamp{0};
        foreign_master_count_++;
        return Types::PTPResult<void>::success();
    }
    
    // Foreign master list is full - could implement aging policy here
    return Types::PTPResult<void>::failure(Types::PTPError::Resource_Unavailable);
}

Types::PTPResult<void> PtpPort::calculate_offset_and_delay() noexcept {
    // Simplified implementation - real implementation would use
    // proper timestamp correlation and filtering algorithms
    
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> PtpPort::set_announce_interval(std::uint8_t log_interval) noexcept {
    if (log_interval > 4) { // Maximum 16 seconds per IEEE 1588-2019
        return Types::PTPResult<void>::failure(Types::PTPError::Invalid_Parameter);
    }
    
    port_data_set_.log_announce_interval = log_interval;
    config_.announce_interval = log_interval;
    
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> PtpPort::set_sync_interval(std::uint8_t log_interval) noexcept {
    if (log_interval > 4) { // Maximum 16 seconds per IEEE 1588-2019
        return Types::PTPResult<void>::failure(Types::PTPError::Invalid_Parameter);
    }
    
    port_data_set_.log_sync_interval = log_interval;
    config_.sync_interval = log_interval;
    
    return Types::PTPResult<void>::success();
}

// ============================================================================
// OrdinaryClock Implementation
// ============================================================================

OrdinaryClock::OrdinaryClock(const PortConfiguration& port_config,
                            const StateCallbacks& callbacks) noexcept 
    : port_(port_config, callbacks) {
}

Types::PTPResult<void> OrdinaryClock::initialize() noexcept {
    return port_.initialize();
}

Types::PTPResult<void> OrdinaryClock::start() noexcept {
    return port_.start();
}

Types::PTPResult<void> OrdinaryClock::stop() noexcept {
    return port_.stop();
}

Types::PTPResult<void> OrdinaryClock::process_message(std::uint8_t message_type,
                                                     const void* message_data,
                                                     std::size_t message_size,
                                                     const Types::Timestamp& rx_timestamp) noexcept {
    // Parse message and delegate to port
    // This is a simplified implementation - full implementation would
    // include proper message parsing and validation
    
    switch (static_cast<MessageType>(message_type)) {
    case MessageType::Announce:
        if (message_size >= sizeof(AnnounceMessage)) {
            const auto* announce_msg = static_cast<const AnnounceMessage*>(message_data);
            return port_.process_announce(*announce_msg);
        }
        break;
        
    case MessageType::Sync:
        if (message_size >= sizeof(SyncMessage)) {
            const auto* sync_msg = static_cast<const SyncMessage*>(message_data);
            return port_.process_sync(*sync_msg, rx_timestamp);
        }
        break;
        
    case MessageType::Follow_Up:
        if (message_size >= sizeof(FollowUpMessage)) {
            const auto* follow_up_msg = static_cast<const FollowUpMessage*>(message_data);
            return port_.process_follow_up(*follow_up_msg);
        }
        break;
        
    case MessageType::Delay_Req:
        if (message_size >= sizeof(DelayReqMessage)) {
            const auto* delay_req_msg = static_cast<const DelayReqMessage*>(message_data);
            return port_.process_delay_req(*delay_req_msg, rx_timestamp);
        }
        break;
        
    case MessageType::Delay_Resp:
        if (message_size >= sizeof(DelayRespMessage)) {
            const auto* delay_resp_msg = static_cast<const DelayRespMessage*>(message_data);
            return port_.process_delay_resp(*delay_resp_msg);
        }
        break;
        
    default:
        return Types::PTPResult<void>::failure(Types::PTPError::UNSUPPORTED_MESSAGE);
    }
    
    return Types::PTPResult<void>::failure(Types::PTPError::INVALID_MESSAGE_SIZE);
}

Types::PTPResult<void> OrdinaryClock::tick(const Types::Timestamp& current_time) noexcept {
    return port_.tick(current_time);
}

// ============================================================================
// BoundaryClock Implementation
// ============================================================================

BoundaryClock::BoundaryClock(const std::array<PortConfiguration, MAX_PORTS>& port_configs,
                            std::size_t port_count,
                            const StateCallbacks& callbacks) noexcept 
    : port_count_(static_cast<std::size_t>(port_count) < MAX_PORTS ? port_count : MAX_PORTS) {
    
    // Initialize all active ports
    for (std::size_t i = 0; i < port_count_; ++i) {
        ports_[i] = PtpPort(port_configs[i], callbacks);
    }
}

Types::PTPResult<void> BoundaryClock::initialize() noexcept {
    for (std::size_t i = 0; i < port_count_; ++i) {
        auto result = ports_[i].initialize();
        if (!result.is_success()) {
            return result;
        }
    }
    
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> BoundaryClock::start() noexcept {
    for (std::size_t i = 0; i < port_count_; ++i) {
        auto result = ports_[i].start();
        if (!result.is_success()) {
            return result;
        }
    }
    
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> BoundaryClock::stop() noexcept {
    for (std::size_t i = 0; i < port_count_; ++i) {
        auto result = ports_[i].stop();
        if (!result.is_success()) {
            return result;
        }
    }
    
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> BoundaryClock::process_message(Types::PortNumber port_number,
                                                     std::uint8_t message_type,
                                                     const void* message_data,
                                                     std::size_t message_size,
                                                     const Types::Timestamp& rx_timestamp) noexcept {
    PtpPort* port = find_port(port_number);
    if (!port) {
        return Types::PTPResult<void>::failure(Types::PTPError::INVALID_PORT);
    }
    
    // Delegate to the specific port (similar to OrdinaryClock implementation)
    // This is simplified - full implementation would include message parsing
    
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> BoundaryClock::tick(const Types::Timestamp& current_time) noexcept {
    for (std::size_t i = 0; i < port_count_; ++i) {
        auto result = ports_[i].tick(current_time);
        if (!result.is_success()) {
            return result;
        }
    }
    
    return Types::PTPResult<void>::success();
}

const PtpPort* BoundaryClock::get_port(Types::PortNumber port_number) const noexcept {
    return find_port(port_number);
}

bool BoundaryClock::has_master_port() const noexcept {
    for (std::size_t i = 0; i < port_count_; ++i) {
        if (ports_[i].is_master()) {
            return true;
        }
    }
    return false;
}

bool BoundaryClock::has_slave_port() const noexcept {
    for (std::size_t i = 0; i < port_count_; ++i) {
        if (ports_[i].is_slave()) {
            return true;
        }
    }
    return false;
}

bool BoundaryClock::is_synchronized() const noexcept {
    for (std::size_t i = 0; i < port_count_; ++i) {
        if (ports_[i].is_synchronized()) {
            return true;
        }
    }
    return false;
}

PtpPort* BoundaryClock::find_port(Types::PortNumber port_number) noexcept {
    for (std::size_t i = 0; i < port_count_; ++i) {
        if (ports_[i].get_identity().port_number == port_number) {
            return &ports_[i];
        }
    }
    return nullptr;
}

const PtpPort* BoundaryClock::find_port(Types::PortNumber port_number) const noexcept {
    for (std::size_t i = 0; i < port_count_; ++i) {
        if (ports_[i].get_identity().port_number == port_number) {
            return &ports_[i];
        }
    }
    return nullptr;
}

// ============================================================================
// TransparentClock Implementation
// ============================================================================

TransparentClock::TransparentClock(TransparentType type,
                                  const std::array<PortConfiguration, MAX_PORTS>& port_configs,
                                  std::size_t port_count,
                                  const StateCallbacks& callbacks) noexcept 
    : type_(type)
    , port_count_(static_cast<std::size_t>(port_count) < MAX_PORTS ? port_count : MAX_PORTS)
    , callbacks_(callbacks) {
    
    // Copy port configurations
    for (std::size_t i = 0; i < port_count_; ++i) {
        port_configs_[i] = port_configs[i];
    }
}

Types::PTPResult<void> TransparentClock::initialize() noexcept {
    // Transparent clocks don't have full port state machines
    // They primarily forward messages with residence time correction
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> TransparentClock::start() noexcept {
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> TransparentClock::stop() noexcept {
    return Types::PTPResult<void>::success();
}

Types::PTPResult<void> TransparentClock::forward_message(Types::PortNumber ingress_port,
                                                        Types::PortNumber egress_port,
                                                        void* message_data,
                                                        std::size_t message_size,
                                                        const Types::Timestamp& ingress_timestamp,
                                                        const Types::Timestamp& egress_timestamp) noexcept {
    // Calculate residence time
    auto residence_time_result = calculate_residence_time(ingress_timestamp, egress_timestamp);
    if (!residence_time_result.is_success()) {
        return Types::PTPResult<void>::failure(residence_time_result.get_error());
    }
    
    // Update correction field in message
    return update_correction_field(message_data, message_size, residence_time_result.get_value());
}

Types::PTPResult<Types::TimeInterval> TransparentClock::calculate_residence_time(
    const Types::Timestamp& ingress_timestamp,
    const Types::Timestamp& egress_timestamp) const noexcept {
    
    if (egress_timestamp < ingress_timestamp) {
        return Types::PTPResult<Types::TimeInterval>::failure(Types::PTPError::INVALID_TIMESTAMP);
    }
    
    Types::TimeInterval residence_time = egress_timestamp - ingress_timestamp;
    return Types::PTPResult<Types::TimeInterval>::success(residence_time);
}

Types::PTPResult<void> TransparentClock::update_correction_field(void* message_data,
                                                                std::size_t message_size,
                                                                Types::TimeInterval residence_time) const noexcept {
    if (!message_data || message_size < sizeof(CommonHeader)) {
        return Types::PTPResult<void>::failure(Types::PTPError::INVALID_PARAMETER);
    }
    
    // Cast to common header to access correction field
    auto* header = static_cast<CommonHeader*>(message_data);
    
    // Add residence time to correction field (scaled by 2^16 per IEEE 1588-2019)
    Types::CorrectionField scaled_residence_time = static_cast<Types::CorrectionField>(residence_time) << 16;
    header->correctionField += scaled_residence_time;
    
    return Types::PTPResult<void>::success();
}

} // namespace Clocks
} // namespace _2019
} // namespace PTP
} // namespace _1588
} // namespace IEEE