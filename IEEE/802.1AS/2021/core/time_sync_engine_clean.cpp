/**
 * @file time_sync_engine_clean.cpp
 * @brief Pure IEEE 802.1AS-2021 Time Synchronization Implementation
 * @details Standard algorithms without hardware dependencies
 * 
 * This implementation provides pure IEEE 802.1AS-2021 synchronization algorithms
 * using abstract hardware interfaces only. No direct hardware dependencies.
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Pure implementation
 */

#include "time_sync_engine_fixed.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cmath>
#include <algorithm>

namespace IEEE {
namespace _802_1AS {
namespace _2021 {

// ============================================================================
// Path Delay Engine Implementation
// ============================================================================

class PathDelayEngine::Implementation {
public:
    Implementation(shared_ptr<HardwareTimestampInterface> hw_interface,
                  shared_ptr<NetworkInterface> net_interface)
        : hw_interface_(hw_interface)
        , net_interface_(net_interface)
        , running_(false)
        , sequence_id_(0)
        , measurement_valid_(false) {}
    
    ~Implementation() {
        stop();
    }
    
    bool initialize(const Configuration& config) {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
        
        if (!hw_interface_ || !net_interface_) {
            return false;
        }
        
        if (!hw_interface_->is_hardware_timestamping_available()) {
            return false;
        }
        
        return true;
    }
    
    void start() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (running_) return;
        
        running_ = true;
        measurement_thread_ = std::thread(&Implementation::measurement_loop, this);
    }
    
    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!running_) return;
            running_ = false;
        }
        
        condition_.notify_all();
        
        if (measurement_thread_.joinable()) {
            measurement_thread_.join();
        }
    }
    
    PathDelayMeasurement get_path_delay() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return current_measurement_;
    }
    
    void process_pdelay_req(const PathDelayReqMessage& message, const Timestamp& receive_time) {
        // Send path delay response
        PathDelayRespMessage response;
        response.header.sequence_id = message.header.sequence_id;
        response.header.source_port_identity = local_port_identity_;
        response.request_receipt_timestamp = receive_time;
        response.requesting_port_identity = message.header.source_port_identity;
        
        // Send response immediately
        net_interface_->send_frame(reinterpret_cast<const uint8_t*>(&response), sizeof(response));
        
        // Capture precise transmission timestamp for follow-up
        Timestamp tx_time = hw_interface_->capture_timestamp();
        
        // Send follow-up with precise timestamp
        PathDelayRespFollowUpMessage follow_up;
        follow_up.header.sequence_id = message.header.sequence_id;
        follow_up.header.source_port_identity = local_port_identity_;
        follow_up.response_origin_timestamp = tx_time;
        follow_up.requesting_port_identity = message.header.source_port_identity;
        
        net_interface_->send_frame(reinterpret_cast<const uint8_t*>(&follow_up), sizeof(follow_up));
    }
    
    void process_pdelay_resp(const PathDelayRespMessage& message, const Timestamp& receive_time) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find matching pending request
        auto it = std::find_if(pending_requests_.begin(), pending_requests_.end(),
            [&](const PendingRequest& req) {
                return req.sequence_id == message.header.sequence_id;
            });
        
        if (it != pending_requests_.end()) {
            it->t2 = message.request_receipt_timestamp;
            it->t3 = receive_time;
            it->response_received = true;
        }
    }
    
    void process_pdelay_resp_follow_up(const PathDelayRespFollowUpMessage& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Find matching pending request
        auto it = std::find_if(pending_requests_.begin(), pending_requests_.end(),
            [&](const PendingRequest& req) {
                return req.sequence_id == message.header.sequence_id;
            });
        
        if (it != pending_requests_.end()) {
            it->t4 = message.response_origin_timestamp;
            it->follow_up_received = true;
            
            // Calculate path delay if we have all timestamps
            if (it->response_received && it->follow_up_received) {
                calculate_path_delay(*it);
                pending_requests_.erase(it);
            }
        }
    }
    
private:
    struct PendingRequest {
        uint16_t sequence_id;
        Timestamp t1; // Request transmission time
        Timestamp t2; // Request reception time (from response)
        Timestamp t3; // Response reception time
        Timestamp t4; // Response transmission time (from follow-up)
        bool response_received = false;
        bool follow_up_received = false;
        std::chrono::steady_clock::time_point sent_time;
    };
    
    void measurement_loop() {
        while (running_) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (condition_.wait_for(lock, config_.pdelay_req_interval, [this] { return !running_; })) {
                break; // Shutting down
            }
            
            if (!running_) break;
            
            send_pdelay_request();
            cleanup_expired_requests();
        }
    }
    
    void send_pdelay_request() {
        // Create path delay request
        PathDelayReqMessage request;
        request.header.sequence_id = ++sequence_id_;
        request.header.source_port_identity = local_port_identity_;
        
        // Record transmission time
        Timestamp tx_time = hw_interface_->capture_timestamp();
        request.origin_timestamp = tx_time;
        
        // Send the request
        if (net_interface_->send_frame(reinterpret_cast<const uint8_t*>(&request), sizeof(request))) {
            // Track this request
            PendingRequest pending;
            pending.sequence_id = request.header.sequence_id;
            pending.t1 = tx_time;
            pending.sent_time = std::chrono::steady_clock::now();
            
            pending_requests_.push_back(pending);
        }
    }
    
    void cleanup_expired_requests() {
        auto now = std::chrono::steady_clock::now();
        pending_requests_.erase(
            std::remove_if(pending_requests_.begin(), pending_requests_.end(),
                [&](const PendingRequest& req) {
                    return (now - req.sent_time) > config_.measurement_timeout;
                }),
            pending_requests_.end());
    }
    
    void calculate_path_delay(const PendingRequest& request) {
        // IEEE 802.1AS-2021 path delay calculation:
        // pathDelay = ((t3 - t1) - (t4 - t2)) / 2
        
        auto t1_ns = request.t1.to_nanoseconds();
        auto t2_ns = request.t2.to_nanoseconds();
        auto t3_ns = request.t3.to_nanoseconds();
        auto t4_ns = request.t4.to_nanoseconds();
        
        auto delay_ns = ((t3_ns - t1_ns) - (t4_ns - t2_ns)) / 2;
        
        // Update current measurement
        current_measurement_.mean_path_delay = Timestamp::from_nanoseconds(delay_ns);
        current_measurement_.valid = true;
        measurement_valid_ = true;
        
        // Simple variation calculation (could be enhanced with proper statistics)
        if (!path_delay_history_.empty()) {
            auto last_delay = path_delay_history_.back();
            auto variation = std::abs(delay_ns.count() - last_delay.count());
            current_measurement_.path_delay_variation = Timestamp::from_nanoseconds(std::chrono::nanoseconds(variation));
        }
        
        // Keep history for statistics
        path_delay_history_.push_back(delay_ns);
        if (path_delay_history_.size() > 10) {
            path_delay_history_.erase(path_delay_history_.begin());
        }
    }
    
    shared_ptr<HardwareTimestampInterface> hw_interface_;
    shared_ptr<NetworkInterface> net_interface_;
    Configuration config_;
    PortIdentity local_port_identity_;
    
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> running_;
    std::thread measurement_thread_;
    
    uint16_t sequence_id_;
    vector<PendingRequest> pending_requests_;
    PathDelayMeasurement current_measurement_;
    bool measurement_valid_;
    vector<std::chrono::nanoseconds> path_delay_history_;
};

// ============================================================================
// Time Synchronization Engine Implementation
// ============================================================================

class TimeSynchronizationEngine::Implementation {
public:
    Implementation(shared_ptr<HardwareTimestampInterface> hw_interface,
                  shared_ptr<NetworkInterface> net_interface,
                  shared_ptr<PathDelayEngine> path_delay_engine)
        : hw_interface_(hw_interface)
        , net_interface_(net_interface)
        , path_delay_engine_(path_delay_engine)
        , running_(false)
        , is_master_(false)
        , force_master_(false) {}
    
    ~Implementation() {
        stop();
    }
    
    bool initialize(const Configuration& config, const ClockIdentity& clock_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        config_ = config;
        clock_identity_ = clock_id;
        
        if (!hw_interface_ || !net_interface_ || !path_delay_engine_) {
            return false;
        }
        
        // Initialize synchronization state
        sync_state_.status = SyncStatus::INITIALIZING;
        sync_state_.master_clock_id = clock_id;
        
        return true;
    }
    
    void start() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (running_) return;
        
        running_ = true;
        sync_state_.status = SyncStatus::ACQUIRING_SYNC;
        
        sync_thread_ = std::thread(&Implementation::synchronization_loop, this);
    }
    
    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!running_) return;
            running_ = false;
        }
        
        condition_.notify_all();
        
        if (sync_thread_.joinable()) {
            sync_thread_.join();
        }
    }
    
    SynchronizationState get_synchronization_state() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return sync_state_;
    }
    
    void process_sync_message(const SyncMessage& message, const Timestamp& receive_time) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Store sync information for follow-up processing
        pending_sync_.sync_message = message;
        pending_sync_.receive_time = receive_time;
        pending_sync_.sync_received = true;
        pending_sync_.sequence_id = message.header.sequence_id;
    }
    
    void process_follow_up_message(const FollowUpMessage& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check if this matches our pending sync
        if (pending_sync_.sync_received && 
            pending_sync_.sequence_id == message.header.sequence_id) {
            
            pending_sync_.precise_origin_timestamp = message.precise_origin_timestamp;
            pending_sync_.follow_up_received = true;
            
            // Now we can calculate time offset
            calculate_time_offset();
        }
    }
    
    void process_announce_message(const AnnounceMessage& message, const Timestamp& receive_time) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Best Master Clock Algorithm (BMCA) would go here
        // For simplicity, accept any master for now
        if (!is_master_ && !force_master_) {
            sync_state_.master_clock_id = message.header.source_port_identity.clock_identity;
            last_announce_time_ = std::chrono::steady_clock::now();
        }
    }
    
    bool is_master() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return is_master_;
    }
    
    void force_master_role(bool enable) {
        std::lock_guard<std::mutex> lock(mutex_);
        force_master_ = enable;
        is_master_ = enable;
        
        if (enable) {
            sync_state_.status = SyncStatus::SYNCHRONIZED;
            sync_state_.master_clock_id = clock_identity_;
        }
    }
    
private:
    struct PendingSync {
        SyncMessage sync_message;
        Timestamp receive_time;
        Timestamp precise_origin_timestamp;
        uint16_t sequence_id = 0;
        bool sync_received = false;
        bool follow_up_received = false;
    };
    
    void synchronization_loop() {
        while (running_) {
            std::unique_lock<std::mutex> lock(mutex_);
            
            if (is_master_ || force_master_) {
                // Send sync messages as master
                send_sync_message();
                condition_.wait_for(lock, config_.sync_interval, [this] { return !running_; });
            } else {
                // Wait for sync messages as slave
                condition_.wait_for(lock, config_.sync_receipt_timeout, [this] { return !running_; });
                check_sync_timeout();
            }
        }
    }
    
    void send_sync_message() {
        // Create sync message
        SyncMessage sync_msg;
        sync_msg.header.sequence_id = ++sync_sequence_id_;
        sync_msg.header.domain_number = 0; // Default domain
        
        // Get transmission timestamp
        Timestamp tx_time = hw_interface_->capture_timestamp();
        sync_msg.origin_timestamp = tx_time;
        
        // Send sync message
        net_interface_->send_frame(reinterpret_cast<const uint8_t*>(&sync_msg), sizeof(sync_msg));
        
        // Send follow-up with precise timestamp
        FollowUpMessage follow_up;
        follow_up.header.sequence_id = sync_msg.header.sequence_id;
        follow_up.precise_origin_timestamp = tx_time;
        
        net_interface_->send_frame(reinterpret_cast<const uint8_t*>(&follow_up), sizeof(follow_up));
    }
    
    void calculate_time_offset() {
        if (!pending_sync_.sync_received || !pending_sync_.follow_up_received) {
            return;
        }
        
        // Get path delay from path delay engine
        auto path_delay_measurement = path_delay_engine_->get_path_delay();
        if (!path_delay_measurement.valid) {
            return; // Can't calculate offset without path delay
        }
        
        // Calculate offset: offset = t2 - t1 - pathDelay
        auto t1 = pending_sync_.precise_origin_timestamp.to_nanoseconds();
        auto t2 = pending_sync_.receive_time.to_nanoseconds();
        auto path_delay = path_delay_measurement.mean_path_delay.to_nanoseconds();
        
        auto offset_ns = t2 - t1 - path_delay;
        
        // Update synchronization state
        sync_state_.offset_from_master = Timestamp::from_nanoseconds(offset_ns);
        sync_state_.last_sync_time = pending_sync_.receive_time;
        sync_state_.sync_sequence_id = pending_sync_.sequence_id;
        sync_state_.status = SyncStatus::SYNCHRONIZED;
        sync_state_.valid = true;
        
        // Apply PI controller for clock adjustment
        apply_clock_adjustment(offset_ns);
        
        // Clear pending sync
        pending_sync_ = PendingSync();
    }
    
    void apply_clock_adjustment(const std::chrono::nanoseconds& offset) {
        // Simple PI controller
        static double integral_error = 0.0;
        
        double error = static_cast<double>(offset.count());
        integral_error += error;
        
        // Calculate adjustment
        double proportional = config_.proportional_gain * error;
        double integral = config_.integral_gain * integral_error;
        double adjustment = proportional + integral;
        
        // Clamp adjustment
        adjustment = std::max(-config_.max_frequency_adjustment * 1e9, 
                            std::min(config_.max_frequency_adjustment * 1e9, adjustment));
        
        sync_state_.frequency_adjustment = adjustment / 1e9; // Convert to ppm
        
        // Apply adjustment to hardware
        if (std::abs(offset.count()) > 1000000) { // > 1ms, step adjustment
            hw_interface_->adjust_clock(offset.count());
        } else { // < 1ms, frequency adjustment
            hw_interface_->set_frequency_adjustment(sync_state_.frequency_adjustment);
        }
    }
    
    void check_sync_timeout() {
        auto now = std::chrono::steady_clock::now();
        if (now - last_sync_time_ > config_.sync_receipt_timeout) {
            sync_state_.status = SyncStatus::LOST_SYNC;
            sync_state_.valid = false;
        }
    }
    
    shared_ptr<HardwareTimestampInterface> hw_interface_;
    shared_ptr<NetworkInterface> net_interface_;
    shared_ptr<PathDelayEngine> path_delay_engine_;
    Configuration config_;
    ClockIdentity clock_identity_;
    
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> running_;
    std::thread sync_thread_;
    
    bool is_master_;
    bool force_master_;
    SynchronizationState sync_state_;
    PendingSync pending_sync_;
    
    uint16_t sync_sequence_id_ = 0;
    std::chrono::steady_clock::time_point last_sync_time_;
    std::chrono::steady_clock::time_point last_announce_time_;
};

// ============================================================================
// Public Interface Implementation
// ============================================================================

PathDelayEngine::PathDelayEngine(shared_ptr<HardwareTimestampInterface> hw_interface,
                               shared_ptr<NetworkInterface> net_interface)
    : impl_(std::make_unique<Implementation>(hw_interface, net_interface)) {
}

bool PathDelayEngine::initialize(const Configuration& config) {
    return impl_->initialize(config);
}

void PathDelayEngine::start() {
    impl_->start();
}

void PathDelayEngine::stop() {
    impl_->stop();
}

PathDelayMeasurement PathDelayEngine::get_path_delay() const {
    return impl_->get_path_delay();
}

void PathDelayEngine::process_pdelay_req(const PathDelayReqMessage& message, const Timestamp& receive_time) {
    impl_->process_pdelay_req(message, receive_time);
}

void PathDelayEngine::process_pdelay_resp(const PathDelayRespMessage& message, const Timestamp& receive_time) {
    impl_->process_pdelay_resp(message, receive_time);
}

void PathDelayEngine::process_pdelay_resp_follow_up(const PathDelayRespFollowUpMessage& message) {
    impl_->process_pdelay_resp_follow_up(message);
}

TimeSynchronizationEngine::TimeSynchronizationEngine(shared_ptr<HardwareTimestampInterface> hw_interface,
                                                    shared_ptr<NetworkInterface> net_interface,
                                                    shared_ptr<PathDelayEngine> path_delay_engine)
    : impl_(std::make_unique<Implementation>(hw_interface, net_interface, path_delay_engine)) {
}

bool TimeSynchronizationEngine::initialize(const Configuration& config, const ClockIdentity& clock_id) {
    return impl_->initialize(config, clock_id);
}

void TimeSynchronizationEngine::start() {
    impl_->start();
}

void TimeSynchronizationEngine::stop() {
    impl_->stop();
}

SynchronizationState TimeSynchronizationEngine::get_synchronization_state() const {
    return impl_->get_synchronization_state();
}

void TimeSynchronizationEngine::process_sync_message(const SyncMessage& message, const Timestamp& receive_time) {
    impl_->process_sync_message(message, receive_time);
}

void TimeSynchronizationEngine::process_follow_up_message(const FollowUpMessage& message) {
    impl_->process_follow_up_message(message);
}

void TimeSynchronizationEngine::process_announce_message(const AnnounceMessage& message, const Timestamp& receive_time) {
    impl_->process_announce_message(message, receive_time);
}

bool TimeSynchronizationEngine::is_master() const {
    return impl_->is_master();
}

void TimeSynchronizationEngine::force_master_role(bool enable) {
    impl_->force_master_role(enable);
}

// ============================================================================
// Factory Functions
// ============================================================================

unique_ptr<TimeSynchronizationEngine> create_time_sync_engine(
    shared_ptr<HardwareTimestampInterface> hw_interface,
    shared_ptr<NetworkInterface> net_interface) {
    
    auto path_delay_engine = std::make_shared<PathDelayEngine>(hw_interface, net_interface);
    return std::make_unique<TimeSynchronizationEngine>(hw_interface, net_interface, path_delay_engine);
}

unique_ptr<PathDelayEngine> create_path_delay_engine(
    shared_ptr<HardwareTimestampInterface> hw_interface,
    shared_ptr<NetworkInterface> net_interface) {
    
    return std::make_unique<PathDelayEngine>(hw_interface, net_interface);
}

} // namespace _2021
} // namespace _802_1AS
} // namespace IEEE
