/**
 * @file time_sync_engine.h
 * @brief Time Synchronization Engine for IEEE 802.1AS-2021
 * @details Pure standard implementation with abstract hardware interfaces
 * 
 * This module provides the core timing and synchronization algorithms
 * defined in IEEE 802.1AS-2021. It uses abstract interfaces to interact
 * with hardware-specific timestamp and network functions.
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Pure implementation without hardware dependencies
 */

#ifndef TIME_SYNC_ENGINE_H
#define TIME_SYNC_ENGINE_H

#include "ieee_802_1as_2021_fixed.h"
#include <functional>

namespace IEEE {
namespace _802_1AS {
namespace _2021 {

// ============================================================================
// Hardware Abstraction Interfaces
// ============================================================================

/**
 * @brief Abstract hardware timestamp interface
 * @details This interface must be implemented by hardware-specific layers
 */
class HardwareTimestampInterface {
public:
    virtual ~HardwareTimestampInterface() = default;
    
    /**
     * @brief Capture hardware timestamp
     * @return Hardware timestamp
     */
    virtual Timestamp capture_timestamp() = 0;
    
    /**
     * @brief Get timestamp resolution in nanoseconds
     */
    virtual uint32_t get_timestamp_resolution() const = 0;
    
    /**
     * @brief Check if hardware timestamping is available
     */
    virtual bool is_hardware_timestamping_available() const = 0;
    
    /**
     * @brief Adjust system clock by offset
     * @param offset_ns Offset in nanoseconds (positive = advance clock)
     * @return true if adjustment successful
     */
    virtual bool adjust_clock(int64_t offset_ns) = 0;
    
    /**
     * @brief Set clock frequency adjustment
     * @param frequency_adjustment Parts per billion adjustment
     * @return true if adjustment successful
     */
    virtual bool set_frequency_adjustment(double frequency_adjustment) = 0;
};

/**
 * @brief Abstract network interface
 * @details This interface must be implemented by network-specific layers
 */
class NetworkInterface {
public:
    virtual ~NetworkInterface() = default;
    
    /**
     * @brief Send raw ethernet frame
     * @param data Frame data
     * @param length Frame length
     * @return true if sent successfully
     */
    virtual bool send_frame(const uint8_t* data, size_t length) = 0;
    
    /**
     * @brief Set frame receive callback
     * @param callback Function to call when frame is received
     */
    virtual void set_receive_callback(std::function<void(const uint8_t*, size_t, const Timestamp&)> callback) = 0;
    
    /**
     * @brief Get interface MAC address
     */
    virtual array<uint8_t, 6> get_mac_address() const = 0;
};

// ============================================================================
// Path Delay Measurement Engine
// ============================================================================

/**
 * @brief Path delay measurement results
 */
struct PathDelayMeasurement {
    Timestamp mean_path_delay;
    Timestamp path_delay_variation;
    bool valid;
    
    PathDelayMeasurement() : valid(false) {}
};

/**
 * @brief Path Delay Engine
 * @details Implements peer-to-peer path delay mechanism
 */
class PathDelayEngine {
public:
    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        std::chrono::milliseconds pdelay_req_interval{1000};  // Default 1 second
        uint32_t allowed_lost_responses{3};
        std::chrono::nanoseconds neighbor_rate_ratio_threshold{100000}; // 100 µs
        std::chrono::milliseconds measurement_timeout{5000};
        
        // PI Controller gains for path delay adjustment
        double proportional_gain{1.0};
        double integral_gain{0.1};
    };
    
    PathDelayEngine(shared_ptr<HardwareTimestampInterface> hw_interface,
                   shared_ptr<NetworkInterface> net_interface);
    
    virtual ~PathDelayEngine() = default;
    
    /**
     * @brief Initialize the path delay engine
     */
    bool initialize(const Configuration& config);
    
    /**
     * @brief Start path delay measurements
     */
    void start();
    
    /**
     * @brief Stop path delay measurements
     */
    void stop();
    
    /**
     * @brief Get current path delay measurement
     */
    PathDelayMeasurement get_path_delay() const;
    
    /**
     * @brief Process received path delay request
     */
    void process_pdelay_req(const PathDelayReqMessage& message, const Timestamp& receive_time);
    
    /**
     * @brief Process received path delay response
     */
    void process_pdelay_resp(const PathDelayRespMessage& message, const Timestamp& receive_time);
    
    /**
     * @brief Process received path delay response follow-up
     */
    void process_pdelay_resp_follow_up(const PathDelayRespFollowUpMessage& message);

private:
    class Implementation;
    unique_ptr<Implementation> impl_;
};

// ============================================================================
// Time Synchronization Engine
// ============================================================================

/**
 * @brief Synchronization status
 */
enum class SyncStatus {
    INITIALIZING,
    ACQUIRING_SYNC,
    SYNCHRONIZED,
    HOLDOVER,
    LOST_SYNC,
    ERROR
};

/**
 * @brief Time synchronization results
 */
struct SynchronizationState {
    SyncStatus status;
    Timestamp offset_from_master;
    double frequency_adjustment;
    Timestamp last_sync_time;
    uint32_t sync_sequence_id;
    ClockIdentity master_clock_id;
    bool valid;
    
    SynchronizationState() 
        : status(SyncStatus::INITIALIZING),
          frequency_adjustment(0.0),
          sync_sequence_id(0),
          valid(false) {}
};

/**
 * @brief Time Synchronization Engine
 * @details Implements IEEE 802.1AS-2021 time synchronization algorithms
 */
class TimeSynchronizationEngine {
public:
    /**
     * @brief Configuration parameters
     */
    struct Configuration {
        std::chrono::milliseconds sync_interval{125};  // Default 8 per second
        std::chrono::milliseconds announce_interval{1000};  // Default 1 per second
        std::chrono::seconds sync_receipt_timeout{3};
        uint32_t allowed_lost_responses{3};
        
        // PI Controller parameters for clock adjustment
        double proportional_gain{1.0};
        double integral_gain{0.1};
        double max_frequency_adjustment{100.0}; // ppm
        
        // Clock quality parameters
        uint8_t clock_class{248};
        ClockAccuracy clock_accuracy{ClockAccuracy::UNKNOWN};
        uint8_t priority1{255};
        uint8_t priority2{255};
    };
    
    TimeSynchronizationEngine(shared_ptr<HardwareTimestampInterface> hw_interface,
                            shared_ptr<NetworkInterface> net_interface,
                            shared_ptr<PathDelayEngine> path_delay_engine);
    
    virtual ~TimeSynchronizationEngine() = default;
    
    /**
     * @brief Initialize the synchronization engine
     */
    bool initialize(const Configuration& config, const ClockIdentity& clock_id);
    
    /**
     * @brief Start time synchronization
     */
    void start();
    
    /**
     * @brief Stop time synchronization
     */
    void stop();
    
    /**
     * @brief Get current synchronization state
     */
    SynchronizationState get_synchronization_state() const;
    
    /**
     * @brief Process received sync message
     */
    void process_sync_message(const SyncMessage& message, const Timestamp& receive_time);
    
    /**
     * @brief Process received follow-up message
     */
    void process_follow_up_message(const FollowUpMessage& message);
    
    /**
     * @brief Process received announce message
     */
    void process_announce_message(const AnnounceMessage& message, const Timestamp& receive_time);
    
    /**
     * @brief Check if acting as master
     */
    bool is_master() const;
    
    /**
     * @brief Force master role (for testing)
     */
    void force_master_role(bool enable = true);

private:
    class Implementation;
    unique_ptr<Implementation> impl_;
};

// ============================================================================
// Factory Functions
// ============================================================================

/**
 * @brief Create a time synchronization engine
 * @param hw_interface Hardware timestamp interface
 * @param net_interface Network interface
 * @return Unique pointer to synchronization engine
 */
unique_ptr<TimeSynchronizationEngine> create_time_sync_engine(
    shared_ptr<HardwareTimestampInterface> hw_interface,
    shared_ptr<NetworkInterface> net_interface);

/**
 * @brief Create a path delay engine
 * @param hw_interface Hardware timestamp interface
 * @param net_interface Network interface
 * @return Unique pointer to path delay engine
 */
unique_ptr<PathDelayEngine> create_path_delay_engine(
    shared_ptr<HardwareTimestampInterface> hw_interface,
    shared_ptr<NetworkInterface> net_interface);

} // namespace _2021
} // namespace _802_1AS
} // namespace IEEE

#endif // TIME_SYNC_ENGINE_H
