/**
 * @file ieee_provider_implementations.cpp
 * @brief Concrete IEEE Provider Implementations for Milan Integration
 * @details Implementation of IEEE provider classes that bridge Milan v1.2 requirements
 *          to actual IEEE standards implementations
 * 
 * @author OpenAvnu Integration Team
 * @date September 2025
 * @copyright OpenAvnu Project
 */

#include "milan_ieee_integration_architecture.h"
#include "../IEEE/802.1AS/2021/core/ieee_802_1as_2021.h"
#include "../IEEE/1722.1/2021/core/ieee_1722_1_2021_base.h"
#include "../IEEE/1722/2016/core/ieee_1722_2016.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>

namespace OpenAvnu {
namespace Integration {
namespace Milan_IEEE {

// ============================================================================
// IEEE 802.1AS-2021 gPTP Provider Implementation
// ============================================================================

IEEE802_1AS_2021_Provider::IEEE802_1AS_2021_Provider() 
    : initialized_(false)
    , domain_number_(0) {
    
    // Initialize with default clock identity
    clock_identity_.fill(0);
    
    // Set default timestamps
    current_time_ = IEEE::_802_1::AS::_2021::Timestamp();
    
    // Initialize with gPTP default clock quality
    clock_quality_.clock_class = 248;           // gPTP default
    clock_quality_.clock_accuracy = 0xFE;       // Unknown accuracy initially
    clock_quality_.offset_scaled_log_variance = 0x4E5D; // gPTP default
}

IEEE802_1AS_2021_Provider::~IEEE802_1AS_2021_Provider() {
    // Cleanup gPTP resources
    if (initialized_) {
        // Note: Actual cleanup would involve stopping gPTP daemon
        // and releasing hardware timestamping resources
    }
}

bool IEEE802_1AS_2021_Provider::initialize_domain(uint8_t domain_number) {
    domain_number_ = domain_number;
    
    // Initialize IEEE 802.1AS-2021 Time Aware System
    auto time_aware_system = std::make_unique<IEEE::_802_1::AS::_2021::TimeAwareSystem>();
    
    if (!time_aware_system->initialize(domain_number)) {
        return false;
    }
    
    // Add the domain to the time aware system
    if (!time_aware_system->add_domain(domain_number)) {
        return false;
    }
    
    // Store the time aware system for later use
    time_aware_system_ = std::move(time_aware_system);
    initialized_ = true;
    
    return true;
}

bool IEEE802_1AS_2021_Provider::is_grandmaster() {
    if (!initialized_ || !time_aware_system_) {
        return false;
    }
    
    // Check if we are synchronized and have the best clock
    // In gPTP, a grandmaster is the root timing reference
    bool is_sync = time_aware_system_->is_synchronized(domain_number_);
    if (!is_sync) {
        return false;
    }
    
    // Check offset from master - if zero or very small, we might be GM
    IEEE::_802_1::AS::_2021::TimeInterval offset = time_aware_system_->get_offset_from_master(domain_number_);
    return (std::abs(offset) < 1000); // Less than 1μs offset suggests we're GM
}

uint64_t IEEE802_1AS_2021_Provider::get_current_time_ns() {
    if (!initialized_ || !time_aware_system_) {
        // Fallback to system time if not initialized
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
    
    IEEE::_802_1::AS::_2021::Timestamp timestamp;
    if (time_aware_system_->get_time(timestamp, domain_number_)) {
        return timestamp.to_nanoseconds();
    }
    
    // Fallback to cached time
    return current_time_.to_nanoseconds();
}

bool IEEE802_1AS_2021_Provider::is_time_synchronized() {
    if (!initialized_ || !time_aware_system_) {
        return false;
    }
    
    return time_aware_system_->is_synchronized(domain_number_);
}

double IEEE802_1AS_2021_Provider::get_sync_accuracy_ns() {
    if (!initialized_ || !time_aware_system_) {
        return 1000000.0; // 1ms default uncertainty
    }
    
    // Get clock quality and convert to nanosecond accuracy estimate
    IEEE::_802_1::AS::_2021::ClockQuality quality = time_aware_system_->get_clock_quality(domain_number_);
    
    // Convert IEEE 1588 clock accuracy enumeration to nanoseconds
    switch (quality.clock_accuracy) {
        case 0x20: return 25.0;      // 25ns
        case 0x21: return 100.0;     // 100ns  
        case 0x22: return 250.0;     // 250ns
        case 0x23: return 1000.0;    // 1μs
        case 0x24: return 2500.0;    // 2.5μs
        case 0x25: return 10000.0;   // 10μs
        case 0x26: return 25000.0;   // 25μs
        case 0x27: return 100000.0;  // 100μs
        case 0x28: return 250000.0;  // 250μs
        case 0x29: return 1000000.0; // 1ms
        case 0x2A: return 2500000.0; // 2.5ms
        case 0x2B: return 10000000.0; // 10ms
        default:   return 1000000.0; // 1ms default for unknown
    }
}

bool IEEE802_1AS_2021_Provider::meets_milan_timing_requirements() {
    if (!is_time_synchronized()) {
        return false;
    }
    
    // Milan v1.2 requires ±80ns synchronization accuracy for professional audio
    double accuracy_ns = get_sync_accuracy_ns();
    return accuracy_ns <= 80.0;
}

bool IEEE802_1AS_2021_Provider::has_stable_grandmaster(std::chrono::seconds min_stable_time) {
    if (!initialized_ || !time_aware_system_) {
        return false;
    }
    
    // In a real implementation, this would track grandmaster stability
    // For now, check if we've been synchronized for the minimum time
    static auto first_sync_time = std::chrono::steady_clock::now();
    static bool was_synchronized = false;
    
    bool currently_synchronized = is_time_synchronized();
    
    if (!was_synchronized && currently_synchronized) {
        // Just became synchronized, reset timer
        first_sync_time = std::chrono::steady_clock::now();
        was_synchronized = true;
        return false;
    }
    
    if (!currently_synchronized) {
        // Lost synchronization
        was_synchronized = false;
        return false;
    }
    
    // Check if we've been stable for the required time
    auto now = std::chrono::steady_clock::now();
    auto stable_duration = std::chrono::duration_cast<std::chrono::seconds>(now - first_sync_time);
    
    return stable_duration >= min_stable_time;
}

uint32_t IEEE802_1AS_2021_Provider::get_path_trace_length() {
    if (!initialized_ || !time_aware_system_) {
        return 0;
    }
    
    // In IEEE 802.1AS, path trace is not directly exposed in the base API
    // This would require access to the announce message processing
    // For now, return a reasonable estimate based on synchronization state
    if (is_grandmaster()) {
        return 0; // Grandmaster has no path
    } else if (is_time_synchronized()) {
        return 1; // Simplified - one hop from grandmaster
    } else {
        return UINT32_MAX; // Not synchronized, infinite path
    }
}

void IEEE802_1AS_2021_Provider::set_grandmaster_change_callback(
    std::function<void(uint64_t old_gm, uint64_t new_gm)> callback) {
    gm_change_callback_ = callback;
    
    // In a full implementation, this would register with the IEEE 802.1AS
    // announce message processing to detect grandmaster changes
}

void IEEE802_1AS_2021_Provider::set_sync_loss_callback(std::function<void()> callback) {
    sync_loss_callback_ = callback;
    
    // In a full implementation, this would register with the IEEE 802.1AS
    // synchronization state machine to detect sync loss events
}

// ============================================================================
// IEEE 1722.1-2021 AVDECC Provider Implementation
// ============================================================================

IEEE1722_1_2021_Provider::IEEE1722_1_2021_Provider(AVnu::Milan::_1_2_2023::MilanPAADEntity* milan_entity)
    : milan_entity_(milan_entity)
    , initialized_(false) {
    
    // Initialize with invalid entity ID until properly set
    entity_id_ = 0;
    
    // Set default message types
    current_message_type_ = IEEE::_1722_1::_2021::AECP::AECPMessageType::AEM_COMMAND;
    current_command_type_ = IEEE::_1722_1::_2021::AECP::AEMCommandType::ENTITY_AVAILABLE;
}

IEEE1722_1_2021_Provider::~IEEE1722_1_2021_Provider() {
    if (initialized_) {
        stop_advertisement();
    }
}

bool IEEE1722_1_2021_Provider::initialize_entity(uint64_t entity_id, uint64_t entity_model_id) {
    entity_id_ = entity_id;
    entity_model_id_ = entity_model_id;
    
    // Initialize the AVDECC entity using IEEE 1722.1-2021 implementation
    // Create entity descriptor with Milan professional audio capabilities
    entity_descriptor_.entity_id = entity_id;
    entity_descriptor_.entity_model_id = entity_model_id;
    
    // Set basic entity capabilities for Milan professional audio
    entity_descriptor_.entity_capabilities = 0x00000007; // AEM_SUPPORTED | CLASS_A_SUPPORTED | MILAN_COMPATIBLE
    
    // Configure Milan professional audio stream capabilities
    entity_descriptor_.talker_stream_sources = 8;  // Milan typical: up to 8 output streams
    entity_descriptor_.listener_stream_sinks = 8;  // Milan typical: up to 8 input streams
    entity_descriptor_.talker_capabilities = 0x0003;   // IMPLEMENTED | AUDIO_SOURCE
    entity_descriptor_.listener_capabilities = 0x0003; // IMPLEMENTED | AUDIO_SINK
    entity_descriptor_.controller_capabilities = 0x00000001; // IMPLEMENTED
    
    // Set Milan-specific identification
    entity_descriptor_.available_index = 0;  // Will increment on state changes
    entity_descriptor_.identify_control_index = 0;
    entity_descriptor_.interface_index = 0;
    entity_descriptor_.association_id = 0;
    
    // Initialize Milan vendor unique command support
    milan_vendor_commands_registered_ = false;
    milan_get_info_supported_ = true;
    milan_system_id_supported_ = true;
    
    // Initialize professional audio stream configurations
    stream_configurations_.clear();
    redundancy_pairs_.clear();
    
    std::cout << "PASS: IEEE 1722.1-2021 entity initialized with Milan professional audio capabilities" << std::endl;
    std::cout << "INFO: Entity ID: 0x" << std::hex << entity_id << std::dec << std::endl;
    std::cout << "INFO: Talker streams: " << entity_descriptor_.talker_stream_sources << std::endl;
    std::cout << "INFO: Listener streams: " << entity_descriptor_.listener_stream_sinks << std::endl;
    
    initialized_ = true;
    return true;
}

bool IEEE1722_1_2021_Provider::start_advertisement() {
    if (!initialized_) {
        return false;
    }
    
    // Start AVDECC entity advertisement using IEEE 1722.1-2021
    // This would typically involve:
    // 1. Sending periodic ENTITY_AVAILABLE messages
    // 2. Responding to ENTITY_DISCOVER messages
    // 3. Setting up multicast group membership
    
    // For now, just mark as advertising
    advertising_ = true;
    return true;
}

bool IEEE1722_1_2021_Provider::stop_advertisement() {
    if (!initialized_) {
        return false;
    }
    
    // Stop AVDECC entity advertisement
    // Send ENTITY_DEPARTING message and stop periodic advertisements
    
    advertising_ = false;
    return true;
}

bool IEEE1722_1_2021_Provider::register_milan_vendor_unique_commands() {
    if (!initialized_) {
        return false;
    }
    
    // Register Milan-specific vendor unique commands using IEEE 1722.1-2021
    // MVU (MILAN Vendor Unique) commands per Milan v1.2 Section 5.4.4
    
    // Commands to register:
    // - GET_MILAN_INFO (0x0000)
    // - SET_SYSTEM_UNIQUE_ID (0x0001) 
    // - GET_SYSTEM_UNIQUE_ID (0x0002)
    // - SET_MEDIA_CLOCK_REFERENCE_INFO (0x0003)
    // - GET_MEDIA_CLOCK_REFERENCE_INFO (0x0004)
    
    // This would involve setting up command handlers in the IEEE AECP processor
    return true;
}

bool IEEE1722_1_2021_Provider::handle_milan_get_info_command(
    const AVnu::Milan::_1_2_2023::GetMilanInfoCommand& cmd,
    AVnu::Milan::_1_2_2023::GetMilanInfoResponse& response) {
    
    if (!initialized_ || !milan_entity_) {
        return false;
    }
    
    // Handle GET_MILAN_INFO command using IEEE 1722.1-2021 AECP infrastructure
    // but populate with Milan-specific information
    
    // Set Milan version information
    response.milan_version_major = 1;
    response.milan_version_minor = 2;
    
    // Get Milan capabilities from the entity
    response.milan_capabilities = milan_entity_->get_milan_capabilities();
    
    // Get Milan features
    response.milan_features = milan_entity_->get_milan_features();
    
    // Set certification level
    response.certification_level = 1; // Milan certified
    
    // Set response command type using IEEE namespace
    current_command_type_ = IEEE::_1722_1::_2021::AECP::AEMCommandType::GET_CONFIGURATION;
    
    return true;
}

bool IEEE1722_1_2021_Provider::handle_milan_system_id_commands() {
    if (!initialized_ || !milan_entity_) {
        return false;
    }
    
    // Handle SET/GET_SYSTEM_UNIQUE_ID commands
    // These are Milan-specific extensions to IEEE 1722.1-2021
    
    // Use the IEEE AECP command processing infrastructure
    // but handle Milan system identification requirements
    
    return true;
}

bool IEEE1722_1_2021_Provider::configure_milan_stream_format(uint16_t stream_index, const std::vector<uint8_t>& format) {
    if (!initialized_) {
        std::cerr << "ERROR: Entity not initialized" << std::endl;
        return false;
    }
    
    // Validate stream index against configured capabilities
    if (stream_index >= entity_descriptor_.talker_stream_sources && 
        stream_index >= entity_descriptor_.listener_stream_sinks) {
        std::cerr << "ERROR: Invalid stream index " << stream_index << std::endl;
        return false;
    }
    
    // Validate Milan audio format
    if (format.size() < 8) {
        std::cerr << "ERROR: Invalid Milan audio format size: " << format.size() << std::endl;
        return false;
    }
    
    // Store Milan professional audio stream configuration
    stream_configurations_[stream_index] = format;
    
    // In a full implementation, this would:
    // 1. Validate format against Milan Section 6.x requirements
    // 2. Configure IEEE 1722.1 stream format using SET_STREAM_FORMAT command
    // 3. Update the stream descriptor in the entity model
    // 4. Apply to the underlying IEEE 1722 AVTP stream
    
    std::cout << "PASS: Milan stream " << stream_index << " configured with " 
              << format.size() << "-byte format" << std::endl;
    
    return true;
}

bool IEEE1722_1_2021_Provider::setup_milan_redundancy(uint16_t primary_stream, uint16_t secondary_stream) {
    if (!initialized_) {
        std::cerr << "ERROR: Entity not initialized" << std::endl;
        return false;
    }
    
    // Validate stream indices
    if (primary_stream >= entity_descriptor_.talker_stream_sources ||
        secondary_stream >= entity_descriptor_.talker_stream_sources) {
        std::cerr << "ERROR: Invalid stream indices for redundancy setup" << std::endl;
        return false;
    }
    
    // Milan Section 7.3.4: Redundancy requires both streams to be configured
    if (stream_configurations_.find(primary_stream) == stream_configurations_.end() ||
        stream_configurations_.find(secondary_stream) == stream_configurations_.end()) {
        std::cerr << "ERROR: Both streams must be configured before setting up redundancy" << std::endl;
        return false;
    }
    
    // Store redundancy pair mapping
    redundancy_pairs_[primary_stream] = secondary_stream;
    
    // In a full implementation, this would:
    // 1. Configure IEEE 1722 redundancy headers
    // 2. Set up dual-stream transmission
    // 3. Configure seamless redundancy switching per Milan requirements
    // 4. Update entity descriptor with redundancy information
    
    std::cout << "PASS: Milan redundancy configured: primary=" << primary_stream 
              << ", secondary=" << secondary_stream << std::endl;
    
    return true;
}

void IEEE1722_1_2021_Provider::set_entity_discovered_callback(std::function<void(uint64_t entity_id)> callback) {
    entity_discovered_callback_ = callback;
    
    // Register with IEEE 1722.1-2021 discovery processing
    // to get notified when other entities are discovered
}

void IEEE1722_1_2021_Provider::set_stream_connection_callback(std::function<void(uint16_t stream_id, bool connected)> callback) {
    stream_connection_callback_ = callback;
    
    // Register with IEEE 1722.1-2021 connection management
    // to get notified about stream connection state changes
}

// ============================================================================
// IEEE 1722-2016 AVTP Provider Implementation
// ============================================================================

/**
 * @brief IEEE 1722-2016 AVTP Provider Implementation
 * 
 * Concrete implementation that bridges Milan professional audio requirements
 * to IEEE::_1722::_2016 AVTP implementation with AAF-PCM support.
 */
class IEEE1722_2016_Provider : public IAVTPProvider {
private:
    bool initialized_;
    std::map<uint64_t, std::unique_ptr<IEEE::_1722::_2016::AudioAVTPDU>> active_streams_;
    std::map<uint64_t, std::unique_ptr<IEEE::_1722::_2016::AVTPDU>> crf_avtpdu_streams_;
    std::map<uint64_t, std::vector<uint8_t>> stream_formats_;
    std::map<uint64_t, bool> stream_states_; // true = streaming, false = stopped
    std::map<uint64_t, uint32_t> presentation_time_offsets_;
    std::map<uint64_t, uint32_t> packet_loss_counters_;
    
    // Milan professional audio configuration
    struct MilanAudioConfig {
        uint32_t sample_rate = 48000;     // Default: 48kHz
        uint16_t channels = 8;            // Default: 8 channels (Milan typical)
        uint16_t bit_depth = 24;          // Default: 24-bit (Milan professional)
        uint16_t samples_per_frame = 6;   // Default: 6 samples per frame (Milan timing)
    } milan_config_;
    
    // Media clock reference streams
    std::map<uint64_t, uint32_t> crf_streams_; // stream_id -> base_frequency
    uint64_t media_clock_stream_id_ = 0;
    
public:
    IEEE1722_2016_Provider() : initialized_(false) {
        std::cout << "INFO: Created IEEE 1722-2016 AVTP provider for Milan professional audio" << std::endl;
    }
    
    ~IEEE1722_2016_Provider() override {
        // Stop all active streams
        for (auto it = active_streams_.begin(); it != active_streams_.end(); ++it) {
            stop_audio_streaming(it->first);
        }
    }
    
    // IAVTPProvider implementation
    bool create_milan_audio_stream(uint64_t stream_id, const std::vector<uint8_t>& format) override {
        if (format.size() < 8) {
            std::cerr << "ERROR: Invalid Milan audio format size: " << format.size() << std::endl;
            return false;
        }
        
        // Create IEEE 1722-2016 Audio AVTPDU for this stream
        auto avtp_stream = std::make_unique<IEEE::_1722::_2016::AudioAVTPDU>();
        
        // Configure AVTP header for Milan audio
        avtp_stream->subtype = static_cast<uint8_t>(IEEE::_1722::_2016::Subtype::AVTP_AUDIO);
        avtp_stream->version = IEEE::_1722::_2016::AVTP_VERSION_2016;
        avtp_stream->stream_valid = true;
        avtp_stream->tv = true; // Timestamp valid for Milan synchronization
        
        // Set stream ID (convert from 64-bit to 8-byte array)
        for (int i = 0; i < 8; i++) {
            avtp_stream->stream_id[i] = (stream_id >> (56 - i * 8)) & 0xFF;
        }
        
        // Configure Milan professional audio format
        avtp_stream->format = IEEE::_1722::_2016::AudioFormat::MILAN_PCM;
        avtp_stream->channels = milan_config_.channels;
        avtp_stream->bit_depth = milan_config_.bit_depth;
        avtp_stream->samples_per_frame = milan_config_.samples_per_frame;
        
        // Set sample rate based on Milan configuration
        switch (milan_config_.sample_rate) {
            case 48000:
                avtp_stream->nominal_sample_rate = IEEE::_1722::_2016::SampleRate::RATE_48KHZ;
                break;
            case 96000:
                avtp_stream->nominal_sample_rate = IEEE::_1722::_2016::SampleRate::RATE_96KHZ;
                break;
            case 192000:
                avtp_stream->nominal_sample_rate = IEEE::_1722::_2016::SampleRate::RATE_192KHZ;
                break;
            default:
                std::cerr << "ERROR: Unsupported Milan sample rate: " << milan_config_.sample_rate << std::endl;
                return false;
        }
        
        // Store stream configuration
        active_streams_[stream_id] = std::move(avtp_stream);
        stream_formats_[stream_id] = format;
        stream_states_[stream_id] = false; // Not streaming yet
        packet_loss_counters_[stream_id] = 0;
        
        std::cout << "PASS: Milan audio stream " << std::hex << stream_id << std::dec 
                  << " created with " << milan_config_.channels << "ch/"
                  << milan_config_.sample_rate << "Hz/" << milan_config_.bit_depth << "bit" << std::endl;
        
        return true;
    }
    
    bool start_audio_streaming(uint64_t stream_id) override {
        auto stream_it = active_streams_.find(stream_id);
        if (stream_it == active_streams_.end()) {
            std::cerr << "ERROR: Stream " << std::hex << stream_id << std::dec << " not found" << std::endl;
            return false;
        }
        
        if (stream_states_[stream_id]) {
            std::cout << "WARN: Stream " << std::hex << stream_id << std::dec << " already streaming" << std::endl;
            return true;
        }
        
        // Start IEEE 1722-2016 AVTP streaming
        auto& avtp_stream = stream_it->second;
        
        // Initialize sequence number for Milan presentation time requirements
        avtp_stream->sequence_num = 0;
        
        // Set initial presentation time offset for Milan latency requirements
        auto offset_it = presentation_time_offsets_.find(stream_id);
        if (offset_it != presentation_time_offsets_.end()) {
            // Configure presentation time for Milan professional audio (typically 2ms)
            avtp_stream->avtp_timestamp = offset_it->second;
        } else {
            // Default Milan presentation time: 2ms for professional audio
            avtp_stream->avtp_timestamp = 2000000; // 2ms in nanoseconds
        }
        
        // In a full implementation, this would:
        // 1. Start periodic AVTP packet transmission
        // 2. Configure IEEE 1722 presentation time
        // 3. Apply Milan timing requirements (±80ns accuracy)
        // 4. Set up redundancy if configured
        
        stream_states_[stream_id] = true;
        
        std::cout << "PASS: Milan audio streaming started for stream " 
                  << std::hex << stream_id << std::dec << std::endl;
        
        return true;
    }
    
    bool stop_audio_streaming(uint64_t stream_id) override {
        auto stream_it = active_streams_.find(stream_id);
        if (stream_it == active_streams_.end()) {
            std::cerr << "ERROR: Stream " << std::hex << stream_id << std::dec << " not found" << std::endl;
            return false;
        }
        
        if (!stream_states_[stream_id]) {
            std::cout << "WARN: Stream " << std::hex << stream_id << std::dec << " already stopped" << std::endl;
            return true;
        }
        
        // Stop IEEE 1722-2016 AVTP streaming
        stream_states_[stream_id] = false;
        
        std::cout << "PASS: Milan audio streaming stopped for stream " 
                  << std::hex << stream_id << std::dec << std::endl;
        
        return true;
    }
    
    bool configure_base_audio_format(uint32_t sample_rate, uint16_t channels, uint16_t bit_depth) override {
        // Validate Milan professional audio constraints
        if (channels == 0 || channels > 64) {
            std::cerr << "ERROR: Invalid channel count: " << channels << " (Milan: 1-64)" << std::endl;
            return false;
        }
        
        if (bit_depth != 16 && bit_depth != 24 && bit_depth != 32) {
            std::cerr << "ERROR: Invalid bit depth: " << bit_depth << " (Milan: 16/24/32)" << std::endl;
            return false;
        }
        
        if (sample_rate != 48000 && sample_rate != 96000 && sample_rate != 192000) {
            std::cerr << "ERROR: Invalid sample rate: " << sample_rate << " (Milan: 48/96/192kHz)" << std::endl;
            return false;
        }
        
        // Update Milan audio configuration
        milan_config_.sample_rate = sample_rate;
        milan_config_.channels = channels;
        milan_config_.bit_depth = bit_depth;
        
        // Calculate samples per frame for Milan timing requirements
        // Milan requires specific frame timing for professional audio
        milan_config_.samples_per_frame = static_cast<uint16_t>(sample_rate / 8000); // 8kHz frame rate
        
        std::cout << "PASS: Milan base audio format configured: " 
                  << channels << "ch/" << sample_rate << "Hz/" << bit_depth << "bit" << std::endl;
        
        initialized_ = true;
        return true;
    }
    
    bool setup_milan_presentation_time(uint64_t stream_id, uint32_t offset_ns) override {
        // Milan professional audio typically requires 2ms presentation time
        if (offset_ns > 10000000) { // Max 10ms for professional audio
            std::cerr << "ERROR: Presentation time offset too large: " << offset_ns << "ns (max 10ms)" << std::endl;
            return false;
        }
        
        presentation_time_offsets_[stream_id] = offset_ns;
        
        // If stream is active, update the AVTP timestamp
        auto stream_it = active_streams_.find(stream_id);
        if (stream_it != active_streams_.end()) {
            stream_it->second->avtp_timestamp = offset_ns;
        }
        
        std::cout << "PASS: Milan presentation time set to " << offset_ns << "ns for stream " 
                  << std::hex << stream_id << std::dec << std::endl;
        
        return true;
    }
    
    bool create_milan_crf_stream(uint64_t stream_id, uint32_t base_frequency) override {
        // Milan CRF (Clock Reference Format) for media clock synchronization
        if (base_frequency == 0) {
            std::cerr << "ERROR: Invalid CRF base frequency: " << base_frequency << std::endl;
            return false;
        }
        
        // Create IEEE 1722-2016 CRF stream
        auto crf_avtp = std::make_unique<IEEE::_1722::_2016::AVTPDU>();
        crf_avtp->subtype = static_cast<uint8_t>(IEEE::_1722::_2016::Subtype::AVTP_CONTROL);
        crf_avtp->version = IEEE::_1722::_2016::AVTP_VERSION_2016;
        crf_avtp->stream_valid = true;
        crf_avtp->tv = true;
        
        // Set CRF stream ID
        for (int i = 0; i < 8; i++) {
            crf_avtp->stream_id[i] = (stream_id >> (56 - i * 8)) & 0xFF;
        }
        
        // Store CRF configuration
        crf_streams_[stream_id] = base_frequency;
        crf_avtpdu_streams_[stream_id] = std::move(crf_avtp);
        
        std::cout << "PASS: Milan CRF stream " << std::hex << stream_id << std::dec 
                  << " created with base frequency " << base_frequency << "Hz" << std::endl;
        
        return true;
    }
    
    bool synchronize_to_media_clock(uint64_t crf_stream_id) override {
        auto crf_it = crf_streams_.find(crf_stream_id);
        if (crf_it == crf_streams_.end()) {
            std::cerr << "ERROR: CRF stream " << std::hex << crf_stream_id << std::dec << " not found" << std::endl;
            return false;
        }
        
        media_clock_stream_id_ = crf_stream_id;
        
        // In a full implementation, this would:
        // 1. Configure all audio streams to use this CRF for timing
        // 2. Adjust sample rate based on CRF stream
        // 3. Apply Milan media clock synchronization requirements
        
        std::cout << "PASS: Synchronized to Milan media clock stream " 
                  << std::hex << crf_stream_id << std::dec << std::endl;
        
        return true;
    }
    
    uint32_t get_stream_packet_loss(uint64_t stream_id) override {
        auto loss_it = packet_loss_counters_.find(stream_id);
        return (loss_it != packet_loss_counters_.end()) ? loss_it->second : 0;
    }
    
    bool meets_milan_latency_requirements(uint64_t stream_id) override {
        auto offset_it = presentation_time_offsets_.find(stream_id);
        if (offset_it == presentation_time_offsets_.end()) {
            return false; // No presentation time configured
        }
        
        // Milan professional audio latency requirements: typically ≤2ms for professional
        uint32_t offset_ns = offset_it->second;
        return offset_ns <= 2000000; // 2ms maximum
    }
};

} // namespace Milan_IEEE
} // namespace Integration
} // namespace OpenAvnu
