/**
 * @file time_sync_engine.h
 * @brief IEEE 802.1AS-2021 Time Synchronization Engine
 * @details Real time synchronization algorithms with hardware timestamping integration
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Complete IEEE 802.1AS-2021 time sync implementation
 */

#ifndef IEEE_802_1AS_2021_TIME_SYNC_ENGINE_H
#define IEEE_802_1AS_2021_TIME_SYNC_ENGINE_H

#include "../core/ieee_802_1as_2021.h"
#include <chrono>
#include <memory>
#include <functional>

namespace IEEE {
namespace _802_1 {
namespace AS {
namespace _2021 {

/**
 * @brief Hardware Timestamp Interface
 * @details Real hardware timestamping integration with Intel HAL
 */
class HardwareTimestampInterface {
public:
    /**
     * @brief Timestamp capture result
     */
    struct TimestampCapture {
        Timestamp timestamp;
        uint16_t sequence_id;
        bool valid;
        std::chrono::steady_clock::time_point capture_time;
    };

    virtual ~HardwareTimestampInterface() = default;

    // Hardware timestamp capture
    virtual bool capture_tx_timestamp(uint16_t sequence_id, TimestampCapture& capture) = 0;
    virtual bool capture_rx_timestamp(uint16_t sequence_id, TimestampCapture& capture) = 0;
    
    // Hardware clock adjustment
    virtual bool adjust_clock_frequency(int32_t ppb_adjustment) = 0;
    virtual bool adjust_clock_phase(TimeInterval phase_adjustment) = 0;
    virtual bool set_clock_time(const Timestamp& time) = 0;
    virtual bool get_clock_time(Timestamp& time) = 0;
    
    // Hardware capabilities
    virtual bool supports_one_step() const = 0;
    virtual bool supports_two_step() const = 0;
    virtual TimeInterval get_timestamp_accuracy() const = 0;
};

/**
 * @brief Time Synchronization Engine
 * @details Complete IEEE 802.1AS-2021 time synchronization implementation with hardware abstraction
 * 
 * NOTE: This engine requires a HardwareTimestampInterface implementation to be provided.
 * Hardware-specific implementations (Intel HAL, etc.) should be provided by integration layers.
 */
class TimeSynchronizationEngine {
public:
    /**
     * @brief Synchronization State
     */
    enum class SyncState {
        UNSYNCHRONIZED,     // Not synchronized
        SYNCHRONIZING,      // In process of synchronization
        SYNCHRONIZED,       // Synchronized and tracking
        HOLDOVER           // Maintaining time without master
    };

    /**
     * @brief Synchronization Configuration
     */
    struct Configuration {
        TimeInterval max_offset_threshold;        // 1ms max offset
        TimeInterval min_offset_threshold;        // 100ns min offset
        double frequency_adjustment_ratio;        // PPB adjustment ratio
        double proportional_gain;                 // PI controller P gain
        double integral_gain;                     // PI controller I gain
        uint32_t sync_receipt_timeout;           // Number of intervals
        uint32_t calibration_period;             // Calibration cycles
        bool enable_one_step;                    // Use one-step if available
        bool enable_frequency_adjustment;         // Enable freq adjustment
        bool enable_phase_adjustment;            // Enable phase adjustment
        
        // Default constructor
        Configuration() 
            : max_offset_threshold(1000000)
            , min_offset_threshold(100)
            , frequency_adjustment_ratio(1.0e-9)
            , proportional_gain(1.0)
            , integral_gain(0.1)
            , sync_receipt_timeout(3)
            , calibration_period(16)
            , enable_one_step(false)
            , enable_frequency_adjustment(true)
            , enable_phase_adjustment(true) {}
    };

    /**
     * @brief Synchronization Statistics
     */
    struct Statistics {
        uint64_t sync_messages_processed{0};
        uint64_t follow_up_messages_processed{0};
        uint64_t frequency_adjustments{0};
        uint64_t phase_adjustments{0};
        TimeInterval current_offset{0};
        TimeInterval mean_path_delay{0};
        int32_t current_frequency_adjustment{0};
        SyncState current_state{SyncState::UNSYNCHRONIZED};
        ::std::chrono::steady_clock::time_point last_sync_time;
        ::std::chrono::steady_clock::time_point last_adjustment_time;
    };

    /**
     * @brief Constructor
     */
    explicit TimeSynchronizationEngine(::std::unique_ptr<HardwareTimestampInterface> hw_interface,
                                     const Configuration& config = Configuration());

    /**
     * @brief Destructor
     */
    ~TimeSynchronizationEngine();

    // Synchronization control
    bool start();
    bool stop();
    bool reset_synchronization();
    
    // Convenience methods for compatibility with legacy APIs
    bool start_synchronization() { return start(); }
    bool stop_synchronization() { return stop(); }

    // Message processing
    bool process_sync_message(const SyncMessage& sync, const Timestamp& rx_timestamp);
    bool process_follow_up_message(const FollowUpMessage& follow_up);

    // State queries
    SyncState get_sync_state() const;
    TimeInterval get_current_offset() const;
    TimeInterval get_mean_path_delay() const;
    bool is_synchronized() const;

    // Configuration
    bool set_configuration(const Configuration& config);
    Configuration get_configuration() const;

    // IEEE 802.1AS-2021 State Machines (Figures 10-3 through 10-9)
    bool process_announce_message(const AnnounceMessage& announce, const Timestamp& rx_timestamp);
    bool process_pdelay_req_message(const PDelayReqMessage& pdelay_req, const Timestamp& rx_timestamp);
    bool process_pdelay_resp_message(const PDelayRespMessage& pdelay_resp, const Timestamp& rx_timestamp);
    
    // State Machine States
    enum class SiteSyncSyncState {
        INITIALIZING,
        SENDING_SYNC,
        SYNC_RECEIPT_TIMEOUT
    };
    
    enum class PortSyncSyncReceiveState {
        DISCARD,
        RECEIVED_SYNC,
        RECEIVED_FOLLOWUP
    };
    
    enum class ClockSlaveState {
        INITIALIZING,
        SEND_SYNC_INDICATION,
        SEND_FOLLOWUP_INDICATION
    };
    
    enum class ClockMasterSyncSendState {
        INITIALIZING,
        SEND_SYNC_MESSAGE,
        SEND_FOLLOWUP_MESSAGE
    };
    
    // State Machine Status Queries
    SiteSyncSyncState get_site_sync_state() const;
    PortSyncSyncReceiveState get_port_sync_receive_state() const;
    ClockSlaveState get_clock_slave_state() const;
    ClockMasterSyncSendState get_clock_master_send_state() const;

    // Statistics
    Statistics get_statistics() const;
    void reset_statistics();

    // Hardware interface
    HardwareTimestampInterface* get_hardware_interface() const;

private:
    class Implementation;
    ::std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Path Delay Measurement Engine
 * @details Real P2P and E2E path delay calculation per IEEE 802.1AS-2021 Section 11
 */
class PathDelayEngine {
public:
    /**
     * @brief Path Delay Mechanism Type
     */
    enum class MechanismType {
        PEER_TO_PEER,       // P2P mechanism (IEEE 802.1AS default)
        END_TO_END,         // E2E mechanism (compatibility)
        DISABLED            // No path delay measurement
    };

    /**
     * @brief Path Delay Configuration
     */
    struct Configuration {
        MechanismType mechanism;       // P2P mechanism (IEEE 802.1AS default)
        ::std::chrono::milliseconds pdelay_interval;    // PDelay request interval
        ::std::chrono::milliseconds pdelay_timeout;     // PDelay response timeout
        uint8_t pdelay_allowed_lost;                     // Allowed lost PDelay responses
        TimeInterval neighbor_prop_delay_threshold; // 800µs threshold
        bool enable_asymmetry_correction;             // Asymmetry correction
        uint8_t filter_weight;                          // Exponential filter weight
        
        // Default constructor
        Configuration()
            : mechanism(MechanismType::PEER_TO_PEER)
            , pdelay_interval(1000)
            , pdelay_timeout(5000)
            , pdelay_allowed_lost(3)
            , neighbor_prop_delay_threshold(800000)
            , enable_asymmetry_correction(true)
            , filter_weight(8) {}
    };

    /**
     * @brief Path Delay Measurement Result
     */
    struct PathDelayMeasurement {
        TimeInterval mean_path_delay{0};
        TimeInterval path_delay_variation{0};
        TimeInterval asymmetry_correction{0};
        bool is_valid{false};
        bool is_calibrated{false};
        uint32_t measurement_count{0};
        ::std::chrono::steady_clock::time_point last_measurement;
    };

    /**
     * @brief Constructor
     */
    explicit PathDelayEngine(::std::unique_ptr<HardwareTimestampInterface> hw_interface,
                           const Configuration& config = Configuration());

    /**
     * @brief Destructor
     */
    ~PathDelayEngine();

    // Path delay operations
    bool start();
    bool stop();
    bool initiate_path_delay_measurement();

    // DEPRECATED METHOD NAMES - Use renamed methods for consistency
    [[deprecated("Use start() instead - renamed for consistency with other engines")]]
    bool start_measurements() { return start(); }
    
    [[deprecated("Use stop() instead - renamed for consistency with other engines")]]
    bool stop_measurements() { return stop(); }
    
    [[deprecated("Use initiate_path_delay_measurement() instead - clearer name following IEEE 802.1AS-2021 terminology")]]
    bool send_pdelay_request() { return initiate_path_delay_measurement(); }

    // Message processing
    bool process_pdelay_request(const PDelayReqMessage& request, const Timestamp& rx_timestamp);
    bool process_pdelay_response(const PDelayRespMessage& response, const Timestamp& rx_timestamp);
    bool process_pdelay_response_follow_up(const PDelayRespFollowUpMessage& follow_up);

    // Results
    PathDelayMeasurement get_current_measurement() const;
    TimeInterval get_mean_path_delay() const;
    bool is_measurement_valid() const;
    bool is_neighbor_rate_ratio_valid() const;

    // Configuration
    bool set_configuration(const Configuration& config);
    Configuration get_configuration() const;

    // Calibration
    bool start_calibration();
    bool stop_calibration();
    bool is_calibrated() const;

    // DEPRECATED COMPATIBILITY CLASS - ARCHITECTURAL VIOLATIONS
    /**
     * @brief Deprecated Implementation with Intel HAL violations
     * @deprecated Use dependency injection with HardwareTimestampInterface instead
     * @details This class contains direct Intel HAL calls that violate Standards layer architecture.
     * It will be removed after migration to proper hardware abstraction pattern.
     */
    class [[deprecated("Violates Standards layer architecture - use dependency injection with HardwareTimestampInterface instead")]] 
    DeprecatedImplementation {
    public:
        // Constructor
        [[deprecated("Use TimeSynchronizationEngine(std::unique_ptr<HardwareTimestampInterface>) instead")]]
        DeprecatedImplementation(const std::string& interface_name);
        
        // Destructor
        ~DeprecatedImplementation();
        
        // Deprecated hardware access methods - use HardwareTimestampInterface instead
        [[deprecated("Standards layer must be hardware-agnostic. Hardware cleanup should be handled by injected HardwareTimestampInterface implementation. This function will be removed after migration to proper dependency injection pattern.")]]
        void shutdown_hal();
        
        [[deprecated("Standards layer must use injected HardwareTimestampInterface for hardware abstraction. Direct intel_hal_get_tx_timestamp calls violate architectural separation. This function will be removed after migration to proper dependency injection pattern.")]]
        bool capture_tx_timestamp(uint16_t sequence_id, HardwareTimestampInterface::TimestampCapture& capture);
        
        [[deprecated("Standards layer must use injected HardwareTimestampInterface for hardware abstraction. Direct intel_hal_get_rx_timestamp calls violate architectural separation. This function will be removed after migration to proper dependency injection pattern.")]]
        bool capture_rx_timestamp(uint16_t sequence_id, HardwareTimestampInterface::TimestampCapture& capture);
        
        // Add other deprecated methods as needed
        
    private:
        std::string interface_name_;
        bool hal_initialized_;
        void* device_handle_;  // Intel HAL device handle - architectural violation
    };

private:
    class Implementation;
    ::std::unique_ptr<Implementation> pImpl;
};

} // namespace _2021
} // namespace AS
} // namespace _802_1  
} // namespace IEEE

#endif // IEEE_802_1AS_2021_TIME_SYNC_ENGINE_H
