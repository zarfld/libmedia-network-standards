/**
 * @file performance_measurement.cpp
 * @brief IEEE 802.1AS-2021 Annex B Performance Measurement Implementation
 * @details Implementation of comprehensive performance validation
 */

#include "performance_measurement.h"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>

namespace IEEE {
namespace _802_1 {
namespace AS {
namespace _2021 {

/**
 * @brief Implementation class for performance measurement
 */
class PerformanceMeasurement::Implementation {
public:
    // Measurement state
    double measurement_duration_seconds = 120.0;
    bool continuous_monitoring_enabled = false;
    
    // Historical data for trending
    std::deque<FrequencyAccuracyMeasurement> frequency_history;
    std::deque<TimeMeasurementGranularity> granularity_history;
    std::deque<NoiseGenerationMeasurement> noise_history;
    
    // Statistical tracking
    uint64_t total_measurements = 0;
    uint64_t compliant_measurements = 0;
    
    Implementation() = default;
    ~Implementation() = default;
    
    // TDEV calculation implementation
    double calculate_single_tdev(const std::vector<double>& phase_errors, double tau, double sample_rate) {
        if (phase_errors.size() < 3) return 0.0;
        
        size_t n = static_cast<size_t>(tau * sample_rate);
        if (n >= phase_errors.size() - 1) return 0.0;
        
        double sum_squares = 0.0;
        size_t count = 0;
        
        for (size_t i = 0; i + 2*n < phase_errors.size(); ++i) {
            double diff = phase_errors[i + 2*n] - 2*phase_errors[i + n] + phase_errors[i];
            sum_squares += diff * diff;
            count++;
        }
        
        if (count == 0) return 0.0;
        return std::sqrt(sum_squares / (6.0 * count));
    }
    
    // ADEV calculation implementation
    double calculate_single_adev(const std::vector<double>& phase_errors, double tau, double sample_rate) {
        if (phase_errors.size() < 2) return 0.0;
        
        size_t n = static_cast<size_t>(tau * sample_rate);
        if (n >= phase_errors.size()) return 0.0;
        
        double sum_squares = 0.0;
        size_t count = 0;
        
        for (size_t i = 0; i + n < phase_errors.size(); ++i) {
            double diff = phase_errors[i + n] - phase_errors[i];
            sum_squares += diff * diff;
            count++;
        }
        
        if (count == 0) return 0.0;
        return std::sqrt(sum_squares / (2.0 * tau * tau * count));
    }
    
    // PTPDEV calculation (specific to PTP applications)
    double calculate_single_ptpdev(const std::vector<double>& phase_errors, double tau, double sample_rate) {
        // PTPDEV is similar to TDEV but with PTP-specific weighting
        return calculate_single_tdev(phase_errors, tau, sample_rate);
    }
    
    // Statistical analysis utilities
    double calculate_rms(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        
        double sum_squares = 0.0;
        for (double value : values) {
            sum_squares += value * value;
        }
        return std::sqrt(sum_squares / values.size());
    }
    
    double calculate_peak_to_peak(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        
        auto [min_it, max_it] = std::minmax_element(values.begin(), values.end());
        return *max_it - *min_it;
    }
    
    double calculate_standard_deviation(const std::vector<double>& values) {
        if (values.size() < 2) return 0.0;
        
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        double sum_squares = 0.0;
        
        for (double value : values) {
            double diff = value - mean;
            sum_squares += diff * diff;
        }
        
        return std::sqrt(sum_squares / (values.size() - 1));
    }
};

PerformanceMeasurement::PerformanceMeasurement() 
    : pImpl(std::make_unique<Implementation>()) {
}

PerformanceMeasurement::~PerformanceMeasurement() = default;

PerformanceMeasurement::FrequencyAccuracyMeasurement 
PerformanceMeasurement::measure_frequency_accuracy(
    const std::vector<Timestamp>& reference_times,
    const std::vector<Timestamp>& local_times) {
    
    FrequencyAccuracyMeasurement result;
    result.measurement_time = std::chrono::steady_clock::now();
    
    if (reference_times.size() != local_times.size() || reference_times.size() < 2) {
        return result; // Invalid input
    }
    
    // Calculate frequency offset using linear regression
    std::vector<double> ref_intervals, local_intervals;
    
    for (size_t i = 1; i < reference_times.size(); ++i) {
        double ref_interval = static_cast<double>((reference_times[i] - reference_times[i-1]).to_nanoseconds());
        double local_interval = static_cast<double>((local_times[i] - local_times[i-1]).to_nanoseconds());
        
        if (ref_interval > 0 && local_interval > 0) {
            ref_intervals.push_back(ref_interval);
            local_intervals.push_back(local_interval);
        }
    }
    
    if (ref_intervals.empty()) {
        return result;
    }
    
    // Calculate frequency ratio and convert to ppm offset
    double sum_ref = std::accumulate(ref_intervals.begin(), ref_intervals.end(), 0.0);
    double sum_local = std::accumulate(local_intervals.begin(), local_intervals.end(), 0.0);
    
    if (sum_ref > 0) {
        double frequency_ratio = sum_local / sum_ref;
        result.frequency_offset_ppm = (frequency_ratio - 1.0) * 1e6;
        result.meets_requirement = std::abs(result.frequency_offset_ppm) <= PerformanceConstants::MAX_FREQUENCY_OFFSET_PPM;
    }
    
    return result;
}

PerformanceMeasurement::TimeMeasurementGranularity 
PerformanceMeasurement::measure_time_granularity(
    std::function<Timestamp()> time_source, 
    uint32_t sample_count) {
    
    TimeMeasurementGranularity result;
    result.measurement_samples = sample_count;
    
    if (!time_source || sample_count < 10) {
        return result;
    }
    
    std::vector<Timestamp> timestamps;
    timestamps.reserve(sample_count);
    
    // Collect rapid timestamp samples
    for (uint32_t i = 0; i < sample_count; ++i) {
        timestamps.push_back(time_source());
    }
    
    // Calculate minimum non-zero time difference
    std::vector<TimeInterval> differences;
    for (size_t i = 1; i < timestamps.size(); ++i) {
        Timestamp diff_timestamp = timestamps[i] - timestamps[i-1];
        TimeInterval diff = static_cast<TimeInterval>(diff_timestamp.to_nanoseconds());
        if (diff > 0) {
            differences.push_back(diff);
        }
    }
    
    if (!differences.empty()) {
        auto min_diff = *std::min_element(differences.begin(), differences.end());
        result.granularity_ns = static_cast<double>(min_diff);
        result.meets_requirement = result.granularity_ns <= PerformanceConstants::MAX_TIME_GRANULARITY_NS;
    }
    
    return result;
}

PerformanceMeasurement::NoiseGenerationMeasurement 
PerformanceMeasurement::measure_noise_generation(
    const std::vector<Timestamp>& time_samples,
    double measurement_duration_seconds) {
    
    NoiseGenerationMeasurement result;
    
    if (time_samples.size() < 100) {
        return result; // Insufficient data
    }
    
    // Convert timestamps to phase error samples
    std::vector<double> phase_errors;
    phase_errors.reserve(time_samples.size());
    
    // Calculate phase errors relative to ideal timing
    double ideal_interval = measurement_duration_seconds / (time_samples.size() - 1);
    for (size_t i = 1; i < time_samples.size(); ++i) {
        double actual_interval = static_cast<double>((time_samples[i] - time_samples[i-1]).to_nanoseconds()) / 1e9; // Convert to seconds
        double phase_error = actual_interval - ideal_interval;
        phase_errors.push_back(phase_error * 1e9); // Convert back to nanoseconds
    }
    
    // B.1.3.1 Jitter Generation Measurement
    result.jitter_peak_to_peak_ns = pImpl->calculate_peak_to_peak(phase_errors);
    result.jitter_meets_requirement = result.jitter_peak_to_peak_ns <= PerformanceConstants::MAX_JITTER_PEAK_TO_PEAK_NS;
    
    // B.1.3.2 Wander Generation (TDEV/ADEV/PTPDEV)
    double sample_rate = time_samples.size() / measurement_duration_seconds;
    std::vector<double> tau_values = {0.1, 1.0, 10.0, 100.0, 1000.0}; // Observation intervals in seconds
    
    result.observation_intervals = tau_values;
    result.tdev_values = calculate_tdev(phase_errors, tau_values);
    result.adev_values = calculate_adev(phase_errors, tau_values);  
    result.ptpdev_values = calculate_ptpdev(phase_errors, tau_values);
    
    // Check TDEV mask compliance (simplified check)
    result.wander_meets_requirement = true;
    for (size_t i = 0; i < result.tdev_values.size(); ++i) {
        // IEEE 802.1AS-2021 TDEV mask: varies by τ
        double tdev_limit = 1e-9 * std::pow(tau_values[i], -0.5); // Simplified mask
        if (result.tdev_values[i] > tdev_limit) {
            result.wander_meets_requirement = false;
            break;
        }
    }
    
    return result;
}

PerformanceMeasurement::PTPInstancePerformance 
PerformanceMeasurement::measure_ptp_instance_performance(
    TimeInterval residence_time,
    TimeInterval pdelay_turnaround,
    double rate_ratio_error) {
    
    PTPInstancePerformance result;
    
    // B.2.2 Residence Time Measurement
    result.residence_time_ns = residence_time;
    result.residence_time_meets_requirement = residence_time <= PerformanceConstants::MAX_RESIDENCE_TIME_NS;
    
    // B.2.3 Pdelay Turnaround Time Measurement  
    result.pdelay_turnaround_ns = pdelay_turnaround;
    result.pdelay_turnaround_meets_requirement = pdelay_turnaround <= PerformanceConstants::MAX_PDELAY_TURNAROUND_NS;
    
    // B.2.4 Rate Ratio Measurement Error
    result.rate_ratio_error_ppm = rate_ratio_error;
    result.rate_ratio_meets_requirement = std::abs(rate_ratio_error) <= PerformanceConstants::MAX_RATE_RATIO_ERROR_PPM;
    
    return result;
}

PerformanceMeasurement::EndToEndPerformance 
PerformanceMeasurement::measure_end_to_end_performance(
    const std::vector<Timestamp>& master_times,
    const std::vector<Timestamp>& slave_times,
    uint32_t hop_count) {
    
    EndToEndPerformance result;
    result.hop_count = hop_count;
    result.measurement_time = std::chrono::steady_clock::now();
    
    if (master_times.size() != slave_times.size() || master_times.empty()) {
        return result;
    }
    
    // Calculate synchronization error as RMS of time differences
    std::vector<double> sync_errors;
    sync_errors.reserve(master_times.size());
    
    for (size_t i = 0; i < master_times.size(); ++i) {
        TimeInterval error = std::abs(static_cast<int64_t>(slave_times[i].to_nanoseconds()) - 
                                     static_cast<int64_t>(master_times[i].to_nanoseconds()));
        sync_errors.push_back(static_cast<double>(error));
    }
    
    // Use peak-to-peak synchronization error
    result.synchronization_error_ns = static_cast<TimeInterval>(pImpl->calculate_peak_to_peak(sync_errors));
    
    // B.3 Requirement: ≤1μs for ≤6 hops
    result.meets_requirement = (hop_count <= PerformanceConstants::MAX_HOP_COUNT) && 
                              (result.synchronization_error_ns <= PerformanceConstants::MAX_SYNC_ERROR_NS);
    
    return result;
}

PerformanceMeasurement::JitterWanderPerformance 
PerformanceMeasurement::measure_jitter_wander_performance(
    const std::vector<Timestamp>& time_samples,
    double filter_bandwidth_hz) {
    
    JitterWanderPerformance result;
    result.endpoint_filter_bandwidth_hz = filter_bandwidth_hz;
    
    if (time_samples.size() < 100) {
        return result;
    }
    
    // Convert to phase error sequence
    std::vector<double> phase_errors;
    for (size_t i = 1; i < time_samples.size(); ++i) {
        double phase_error = static_cast<double>((time_samples[i] - time_samples[i-1]).to_nanoseconds());
        phase_errors.push_back(phase_error);
    }
    
    // Apply simplified filter (high-pass at filter_bandwidth_hz)
    // In practice, this would use proper DSP filtering
    std::vector<double> filtered_errors = phase_errors; // Simplified
    
    // Measure jitter (peak-to-peak over 60 seconds)
    result.measured_jitter_pp_ns = pImpl->calculate_peak_to_peak(filtered_errors);
    
    // Set requirements based on filter bandwidth (B.4)
    if (filter_bandwidth_hz >= PerformanceConstants::CONSUMER_FILTER_BANDWIDTH_HZ) {
        result.required_jitter_pp_ns = PerformanceConstants::CONSUMER_JITTER_LIMIT_NS;
    } else {
        result.required_jitter_pp_ns = PerformanceConstants::PROFESSIONAL_JITTER_LIMIT_NS;
    }
    
    result.jitter_meets_requirement = result.measured_jitter_pp_ns <= result.required_jitter_pp_ns;
    
    // MTIE calculation (simplified)
    result.measured_mtie_ns = pImpl->calculate_peak_to_peak(filtered_errors);
    result.required_mtie_ns = result.required_jitter_pp_ns; // Simplified
    result.mtie_meets_requirement = result.measured_mtie_ns <= result.required_mtie_ns;
    
    return result;
}

std::vector<double> PerformanceMeasurement::calculate_tdev(
    const std::vector<double>& phase_errors, 
    const std::vector<double>& observation_intervals) {
    
    std::vector<double> tdev_values;
    tdev_values.reserve(observation_intervals.size());
    
    double sample_rate = 1.0; // Assume 1 Hz sample rate for simplicity
    
    for (double tau : observation_intervals) {
        double tdev = pImpl->calculate_single_tdev(phase_errors, tau, sample_rate);
        tdev_values.push_back(tdev);
    }
    
    return tdev_values;
}

std::vector<double> PerformanceMeasurement::calculate_adev(
    const std::vector<double>& phase_errors, 
    const std::vector<double>& observation_intervals) {
    
    std::vector<double> adev_values;
    adev_values.reserve(observation_intervals.size());
    
    double sample_rate = 1.0; // Assume 1 Hz sample rate for simplicity
    
    for (double tau : observation_intervals) {
        double adev = pImpl->calculate_single_adev(phase_errors, tau, sample_rate);
        adev_values.push_back(adev);
    }
    
    return adev_values;
}

std::vector<double> PerformanceMeasurement::calculate_ptpdev(
    const std::vector<double>& phase_errors, 
    const std::vector<double>& observation_intervals) {
    
    std::vector<double> ptpdev_values;
    ptpdev_values.reserve(observation_intervals.size());
    
    double sample_rate = 1.0; // Assume 1 Hz sample rate for simplicity
    
    for (double tau : observation_intervals) {
        double ptpdev = pImpl->calculate_single_ptpdev(phase_errors, tau, sample_rate);
        ptpdev_values.push_back(ptpdev);
    }
    
    return ptpdev_values;
}

PerformanceMeasurement::PerformanceReport PerformanceMeasurement::generate_performance_report() {
    PerformanceReport report;
    report.report_time = std::chrono::steady_clock::now();
    
    // This would typically aggregate recent measurements
    // For now, create a summary report structure
    
    // Check overall compliance
    report.overall_compliance = 
        report.frequency_accuracy.meets_requirement &&
        report.time_granularity.meets_requirement &&
        report.noise_generation.jitter_meets_requirement &&
        report.noise_generation.wander_meets_requirement &&
        report.ptp_instance.residence_time_meets_requirement &&
        report.ptp_instance.pdelay_turnaround_meets_requirement &&
        report.ptp_instance.rate_ratio_meets_requirement &&
        report.end_to_end.meets_requirement &&
        report.jitter_wander.jitter_meets_requirement &&
        report.jitter_wander.mtie_meets_requirement;
    
    // Generate summary
    std::stringstream ss;
    ss << "IEEE 802.1AS-2021 Annex B Performance Report\n";
    ss << "==========================================\n";
    ss << "Overall Compliance: " << (report.overall_compliance ? "PASS" : "FAIL") << "\n";
    ss << "Frequency Accuracy: " << (report.frequency_accuracy.meets_requirement ? "PASS" : "FAIL") << "\n";
    ss << "Time Granularity: " << (report.time_granularity.meets_requirement ? "PASS" : "FAIL") << "\n";
    ss << "Noise Generation: " << (report.noise_generation.jitter_meets_requirement && 
                                   report.noise_generation.wander_meets_requirement ? "PASS" : "FAIL") << "\n";
    ss << "PTP Instance: " << (report.ptp_instance.residence_time_meets_requirement &&
                              report.ptp_instance.pdelay_turnaround_meets_requirement &&
                              report.ptp_instance.rate_ratio_meets_requirement ? "PASS" : "FAIL") << "\n";
    ss << "End-to-End: " << (report.end_to_end.meets_requirement ? "PASS" : "FAIL") << "\n";
    ss << "Jitter/Wander: " << (report.jitter_wander.jitter_meets_requirement &&
                               report.jitter_wander.mtie_meets_requirement ? "PASS" : "FAIL") << "\n";
    
    report.compliance_summary = ss.str();
    
    return report;
}

bool PerformanceMeasurement::validate_ieee_802_1as_compliance() {
    PerformanceReport report = generate_performance_report();
    return report.overall_compliance;
}

void PerformanceMeasurement::reset_measurements() {
    pImpl->frequency_history.clear();
    pImpl->granularity_history.clear();
    pImpl->noise_history.clear();
    pImpl->total_measurements = 0;
    pImpl->compliant_measurements = 0;
}

void PerformanceMeasurement::set_measurement_duration(double seconds) {
    pImpl->measurement_duration_seconds = seconds;
}

void PerformanceMeasurement::enable_continuous_monitoring(bool enable) {
    pImpl->continuous_monitoring_enabled = enable;
}

} // namespace _2021
} // namespace AS
} // namespace _802_1
} // namespace IEEE
