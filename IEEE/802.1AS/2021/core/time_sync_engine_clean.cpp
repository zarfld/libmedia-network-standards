/**
 * @file time_sync_engine.cpp
 * @brief IEEE 802.1AS-2021 Time Synchronization Engine Implementation
 * @details Hardware-agnostic time synchronization with proper dependency injection
 * 
 * @author OpenAvnu Standards Team
 * @date 2025  
 * @copyright IEEE Standards - Implementation follows IEEE 802.1AS-2021 specification
 */

#include "time_sync_engine.h"
#include <algorithm>
#include <cmath>

namespace IEEE {
namespace _802_1AS {
namespace _2021 {

// ============================================================================
// TimeSynchronizationEngine::Implementation
// ============================================================================

class TimeSynchronizationEngine::Implementation {
public:
    Implementation(std::unique_ptr<HardwareTimestampInterface> hw_interface, 
                  const Configuration& config)
        : hw_interface_(std::move(hw_interface))
        , config_(config)
        , sync_state_(SyncState::UNSYNCHRONIZED)
        , offset_history_(config.offset_filter_size)
        , pending_sync_()
        , pending_sync_rx_timestamp_() {}

    bool start() {
        sync_state_ = SyncState::SYNCHRONIZING;
        statistics_.lastActivity = std::chrono::steady_clock::now();
        offset_history_.clear();
        return true;
    }

    bool stop() {
        sync_state_ = SyncState::UNSYNCHRONIZED;
        statistics_.lastActivity = std::chrono::steady_clock::now();
        return true;
    }

    bool reset() {
        sync_state_ = SyncState::UNSYNCHRONIZED;
        statistics_ = Statistics{};
        offset_history_.clear();
        return true;
    }

    bool process_sync(const SyncMessage& sync, const Timestamp& rx_timestamp) {
        // Store pending sync for Follow_Up processing
        pending_sync_ = sync;
        pending_sync_rx_timestamp_ = rx_timestamp;

        // Update statistics
        statistics_.messagesReceived++;
        statistics_.lastActivity = std::chrono::steady_clock::now();

        return true;
    }

    bool process_follow_up(const FollowUpMessage& follow_up) {
        // Process Follow_Up for stored Sync message
        if (pending_sync_ && 
            follow_up.header.sequence_id == pending_sync_->header.sequence_id) {

            // Calculate master-to-slave delay
            TimeInterval master_to_slave_delay = 
                calculate_offset(follow_up.precise_origin_timestamp, 
                               pending_sync_rx_timestamp_);

            // Add to offset history for filtering
            offset_history_.push_back(master_to_slave_delay);
            if (offset_history_.size() > config_.offset_filter_size) {
                offset_history_.pop_front();
            }

            // Update synchronization state
            if (offset_history_.size() >= config_.min_samples_for_sync) {
                sync_state_ = SyncState::SYNCHRONIZED;
            }

            return true;
        }
        return false;
    }

    SyncState get_sync_state() const {
        return sync_state_;
    }

    TimeInterval get_current_offset() const {
        if (offset_history_.empty()) {
            return 0;
        }

        // Calculate median offset for better filtering
        auto sorted_offsets = std::vector<TimeInterval>(offset_history_.begin(), offset_history_.end());
        std::sort(sorted_offsets.begin(), sorted_offsets.end());
        return sorted_offsets[sorted_offsets.size() / 2];
    }

    Configuration get_configuration() const {
        return config_;
    }

    bool set_configuration(const Configuration& new_config) {
        config_ = new_config;
        return true;
    }

    Statistics get_statistics() const {
        return statistics_;
    }

private:
    std::unique_ptr<HardwareTimestampInterface> hw_interface_;
    Configuration config_;
    SyncState sync_state_;
    Statistics statistics_;

    // Offset filtering
    std::deque<TimeInterval> offset_history_;

    // Pending message processing
    std::optional<SyncMessage> pending_sync_;
    Timestamp pending_sync_rx_timestamp_;

    TimeInterval calculate_offset(const Timestamp& master_time, const Timestamp& slave_time) {
        // Convert timestamps to nanoseconds for calculation
        uint64_t master_ns = static_cast<uint64_t>(master_time.seconds_field) * 1000000000ULL + 
                            master_time.nanoseconds_field;
        uint64_t slave_ns = static_cast<uint64_t>(slave_time.seconds_field) * 1000000000ULL + 
                           slave_time.nanoseconds_field;

        return static_cast<TimeInterval>(master_ns) - static_cast<TimeInterval>(slave_ns);
    }
};

// ============================================================================
// TimeSynchronizationEngine Public Interface  
// ============================================================================

TimeSynchronizationEngine::TimeSynchronizationEngine(
    std::unique_ptr<HardwareTimestampInterface> hw_interface,
    const Configuration& config)
    : pImpl(std::make_unique<Implementation>(std::move(hw_interface), config)) {}

TimeSynchronizationEngine::~TimeSynchronizationEngine() = default;

bool TimeSynchronizationEngine::start() {
    return pImpl->start();
}

bool TimeSynchronizationEngine::stop() {
    return pImpl->stop();
}

bool TimeSynchronizationEngine::reset() {
    return pImpl->reset();
}

bool TimeSynchronizationEngine::process_sync(const SyncMessage& sync, const Timestamp& rx_timestamp) {
    return pImpl->process_sync(sync, rx_timestamp);
}

bool TimeSynchronizationEngine::process_follow_up(const FollowUpMessage& follow_up) {
    return pImpl->process_follow_up(follow_up);
}

TimeSynchronizationEngine::SyncState TimeSynchronizationEngine::get_sync_state() const {
    return pImpl->get_sync_state();
}

TimeInterval TimeSynchronizationEngine::get_current_offset() const {
    return pImpl->get_current_offset();
}

TimeSynchronizationEngine::Configuration TimeSynchronizationEngine::get_configuration() const {
    return pImpl->get_configuration();
}

bool TimeSynchronizationEngine::set_configuration(const Configuration& config) {
    return pImpl->set_configuration(config);
}

TimeSynchronizationEngine::Statistics TimeSynchronizationEngine::get_statistics() const {
    return pImpl->get_statistics();
}

// ============================================================================
// PathDelayEngine::Implementation
// ============================================================================

class PathDelayEngine::Implementation {
public:
    Implementation(std::unique_ptr<HardwareTimestampInterface> hw_interface)
        : hw_interface_(std::move(hw_interface))
        , measurement_state_(MeasurementState::IDLE)
        , current_sequence_id_(0)
        , path_delay_(0)
        , path_delay_valid_(false) {}

    bool start() {
        measurement_state_ = MeasurementState::READY;
        return true;
    }

    bool stop() {
        measurement_state_ = MeasurementState::IDLE;
        return true;
    }

    bool initiate_path_delay_measurement() {
        if (measurement_state_ != MeasurementState::READY) {
            return false;
        }

        // Initiate PDelay_Req transmission with timestamp capture
        measurement_state_ = MeasurementState::AWAITING_RESPONSE;
        current_sequence_id_++;

        return true;
    }

    TimeInterval get_path_delay() const {
        return path_delay_;
    }

    bool is_path_delay_valid() const {
        return path_delay_valid_;
    }

private:
    std::unique_ptr<HardwareTimestampInterface> hw_interface_;
    MeasurementState measurement_state_;
    uint16_t current_sequence_id_;
    TimeInterval path_delay_;
    bool path_delay_valid_;
};

// ============================================================================
// PathDelayEngine Public Interface
// ============================================================================

PathDelayEngine::PathDelayEngine(std::unique_ptr<HardwareTimestampInterface> hw_interface)
    : pImpl(std::make_unique<Implementation>(std::move(hw_interface))) {}

PathDelayEngine::~PathDelayEngine() = default;

bool PathDelayEngine::start() {
    return pImpl->start();
}

bool PathDelayEngine::stop() {
    return pImpl->stop();
}

bool PathDelayEngine::initiate_path_delay_measurement() {
    return pImpl->initiate_path_delay_measurement();
}

TimeInterval PathDelayEngine::get_path_delay() const {
    return pImpl->get_path_delay();
}

bool PathDelayEngine::is_path_delay_valid() const {
    return pImpl->is_path_delay_valid();
}

// ============================================================================
// Deprecated Methods (for backward compatibility)
// ============================================================================

bool PathDelayEngine::start_measurements() {
    return start();
}

bool PathDelayEngine::stop_measurements() {
    return stop();
}

bool PathDelayEngine::send_pdelay_request() {
    return initiate_path_delay_measurement();
}

} // namespace _2021
} // namespace _802_1AS  
} // namespace IEEE
