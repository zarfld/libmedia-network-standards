/**
 * @file performance_measurement.h
 * @brief IEEE 802.1AS-2021 Annex B Performance Measurement Framework
 * @details Validates LocalClock requirements, timing accuracy, and protocol compliance
 * 
 * This implementation provides comprehensive performance validation according to:
 * - Annex B.1: LocalClock requirements (frequency accuracy, granularity, noise)
 * - Annex B.2: PTP Instance requirements (residence time, turnaround time, rate ratio)
 * - Annex B.3: End-to-end time-synchronization performance (1μs accuracy)
 * - Annex B.4: Jitter and wander performance (TDEV/ADEV calculations)
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Implementation follows IEEE 802.1AS-2021 Annex B
 */

#ifndef IEEE_802_1AS_2021_PERFORMANCE_MEASUREMENT_H
#define IEEE_802_1AS_2021_PERFORMANCE_MEASUREMENT_H

#include "../core/ieee_802_1as_2021.h"
#include <chrono>
#include <vector>
#include <deque>
#include <memory>
#include <cmath>
#include <functional>

namespace IEEE {
namespace _802_1 {
namespace AS {
namespace _2021 {

/**
 * @brief Performance Measurement Framework
 * @details Validates IEEE 802.1AS-2021 Annex B requirements
 */
class PerformanceMeasurement {
public:
    /**
     * @brief B.1.1 LocalClock Frequency Accuracy Requirements
     * Fractional frequency offset relative to TAI shall be within ±100 ppm
     */
    struct FrequencyAccuracyMeasurement {
        double frequency_offset_ppm;        // Measured frequency offset in ppm
        bool meets_requirement;              // True if within ±100 ppm
        std::chrono::steady_clock::time_point measurement_time;
        
        FrequencyAccuracyMeasurement() : frequency_offset_ppm(0.0), meets_requirement(false) {}
    };

    /**
     * @brief B.1.2 Time Measurement Granularity Requirements
     * Granularity <= 40/(1-0.0001) ns ≈ 40.004 ns
     */
    struct TimeMeasurementGranularity {
        double granularity_ns;               // Measured granularity in nanoseconds
        bool meets_requirement;              // True if <= 40.004 ns
        uint64_t measurement_samples;        // Number of samples used
        
        TimeMeasurementGranularity() : granularity_ns(0.0), meets_requirement(false), measurement_samples(0) {}
    };

    /**
     * @brief B.1.3 Noise Generation Requirements
     * Jitter and wander performance measurements
     */
    struct NoiseGenerationMeasurement {
        // B.1.3.1 Jitter Generation
        double jitter_peak_to_peak_ns;      // Peak-to-peak jitter over 60s
        bool jitter_meets_requirement;      // True if <= 2 ns peak-to-peak
        
        // B.1.3.2 Wander Generation (TDEV)
        std::vector<double> tdev_values;     // TDEV measurements at different τ
        std::vector<double> adev_values;     // ADEV measurements at different τ
        std::vector<double> ptpdev_values;   // PTPDEV measurements at different τ
        std::vector<double> observation_intervals; // τ values in seconds
        bool wander_meets_requirement;       // True if within TDEV mask
        
        NoiseGenerationMeasurement() : jitter_peak_to_peak_ns(0.0), jitter_meets_requirement(false), 
                                      wander_meets_requirement(false) {}
    };

    /**
     * @brief B.2 PTP Instance Requirements
     */
    struct PTPInstancePerformance {
        // B.2.2 Residence Time (should be <= 10 ms)
        TimeInterval residence_time_ns;      // Measured residence time
        bool residence_time_meets_requirement;
        
        // B.2.3 Pdelay Turnaround Time (should be <= 10 ms)
        TimeInterval pdelay_turnaround_ns;   // Measured turnaround time
        bool pdelay_turnaround_meets_requirement;
        
        // B.2.4 Rate Ratio Measurement Error (shall not exceed 0.1 ppm)
        double rate_ratio_error_ppm;        // Measured rate ratio error
        bool rate_ratio_meets_requirement;
        
        PTPInstancePerformance() : residence_time_ns(0), residence_time_meets_requirement(false),
                                  pdelay_turnaround_ns(0), pdelay_turnaround_meets_requirement(false),
                                  rate_ratio_error_ppm(0.0), rate_ratio_meets_requirement(false) {}
    };

    /**
     * @brief B.3 End-to-End Time-Synchronization Performance
     * Any two PTP Instances separated by ≤6 hops synchronized within 1μs peak-to-peak
     */
    struct EndToEndPerformance {
        TimeInterval synchronization_error_ns; // Measured sync error
        uint32_t hop_count;                   // Number of hops in path
        bool meets_requirement;               // True if <= 1μs for ≤6 hops
        std::chrono::steady_clock::time_point measurement_time;
        
        EndToEndPerformance() : synchronization_error_ns(0), hop_count(0), meets_requirement(false) {}
    };

    /**
     * @brief B.4 Jitter and Wander Performance
     * MTIE requirements for different endpoint filter bandwidths
     */
    struct JitterWanderPerformance {
        double endpoint_filter_bandwidth_hz;  // Filter bandwidth
        double measured_mtie_ns;              // Measured MTIE
        double required_mtie_ns;              // Required MTIE from standard
        double measured_jitter_pp_ns;         // Peak-to-peak jitter
        double required_jitter_pp_ns;         // Required jitter limit
        bool mtie_meets_requirement;
        bool jitter_meets_requirement;
        
        JitterWanderPerformance() : endpoint_filter_bandwidth_hz(0.0), measured_mtie_ns(0.0), 
                                   required_mtie_ns(0.0), measured_jitter_pp_ns(0.0), 
                                   required_jitter_pp_ns(0.0), mtie_meets_requirement(false),
                                   jitter_meets_requirement(false) {}
    };

    /**
     * @brief Complete Performance Report
     */
    struct PerformanceReport {
        FrequencyAccuracyMeasurement frequency_accuracy;
        TimeMeasurementGranularity time_granularity;
        NoiseGenerationMeasurement noise_generation;
        PTPInstancePerformance ptp_instance;
        EndToEndPerformance end_to_end;
        JitterWanderPerformance jitter_wander;
        
        bool overall_compliance;              // True if all requirements met
        std::chrono::steady_clock::time_point report_time;
        std::string compliance_summary;       // Human-readable summary
    };

    /**
     * @brief Constructor
     */
    explicit PerformanceMeasurement();

    /**
     * @brief Destructor
     */
    ~PerformanceMeasurement();

    // B.1 LocalClock Requirements
    FrequencyAccuracyMeasurement measure_frequency_accuracy(
        const std::vector<Timestamp>& reference_times,
        const std::vector<Timestamp>& local_times);
    
    TimeMeasurementGranularity measure_time_granularity(
        std::function<Timestamp()> time_source, 
        uint32_t sample_count = 10000);
    
    NoiseGenerationMeasurement measure_noise_generation(
        const std::vector<Timestamp>& time_samples,
        double measurement_duration_seconds = 120.0);

    // B.2 PTP Instance Requirements  
    PTPInstancePerformance measure_ptp_instance_performance(
        TimeInterval residence_time,
        TimeInterval pdelay_turnaround,
        double rate_ratio_error);

    // B.3 End-to-End Performance
    EndToEndPerformance measure_end_to_end_performance(
        const std::vector<Timestamp>& master_times,
        const std::vector<Timestamp>& slave_times,
        uint32_t hop_count);

    // B.4 Jitter and Wander Performance
    JitterWanderPerformance measure_jitter_wander_performance(
        const std::vector<Timestamp>& time_samples,
        double filter_bandwidth_hz);

    // Comprehensive performance validation
    PerformanceReport generate_performance_report();
    bool validate_ieee_802_1as_compliance();
    
    // TDEV/ADEV/PTPDEV calculations (B.1.3.2)
    std::vector<double> calculate_tdev(const std::vector<double>& phase_errors, 
                                      const std::vector<double>& observation_intervals);
    std::vector<double> calculate_adev(const std::vector<double>& phase_errors, 
                                      const std::vector<double>& observation_intervals);
    std::vector<double> calculate_ptpdev(const std::vector<double>& phase_errors, 
                                        const std::vector<double>& observation_intervals);

    // Utility methods
    void reset_measurements();
    void set_measurement_duration(double seconds);
    void enable_continuous_monitoring(bool enable);

private:
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
};

/**
 * @brief Performance Validation Constants from IEEE 802.1AS-2021 Annex B
 */
namespace PerformanceConstants {
    // B.1.1 Frequency Accuracy
    constexpr double MAX_FREQUENCY_OFFSET_PPM = 100.0;
    
    // B.1.2 Time Measurement Granularity  
    constexpr double MAX_TIME_GRANULARITY_NS = 40.004; // 40/(1-0.0001)
    
    // B.1.3.1 Jitter Generation
    constexpr double MAX_JITTER_PEAK_TO_PEAK_NS = 2.0;
    constexpr double JITTER_MEASUREMENT_DURATION_S = 60.0;
    constexpr double JITTER_HIGH_PASS_FILTER_HZ = 10.0;
    
    // B.2 PTP Instance Requirements
    constexpr TimeInterval MAX_RESIDENCE_TIME_NS = 10000000; // 10 ms
    constexpr TimeInterval MAX_PDELAY_TURNAROUND_NS = 10000000; // 10 ms  
    constexpr double MAX_RATE_RATIO_ERROR_PPM = 0.1;
    
    // B.3 End-to-End Performance
    constexpr TimeInterval MAX_SYNC_ERROR_NS = 1000; // 1 μs
    constexpr uint32_t MAX_HOP_COUNT = 6;
    
    // B.4 Jitter and Wander Performance
    constexpr double CONSUMER_FILTER_BANDWIDTH_HZ = 10.0;
    constexpr double PROFESSIONAL_FILTER_BANDWIDTH_HZ = 1.0;
    constexpr double CONSUMER_JITTER_LIMIT_NS = 11.1;
    constexpr double PROFESSIONAL_JITTER_LIMIT_NS = 10.2;
}

} // namespace _2021
} // namespace AS
} // namespace _802_1
} // namespace IEEE

#endif // IEEE_802_1AS_2021_PERFORMANCE_MEASUREMENT_H
