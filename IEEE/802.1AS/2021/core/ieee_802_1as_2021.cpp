/**
 * @file ieee_802_1as_2021.cpp
 * @brief IEEE 802.1AS-2021 Standard Implementation
 * @details Implementation of IEEE 802.1AS-2021 gPTP for AVB/TSN networks
 * 
 * @author OpenAvnu Standards Team  
 * @date 2025
 * @copyright IEEE Standards - Implementation follows IEEE 802.1AS-2021 specification
 */

#include "ieee_802_1as_2021.h"
#include <cstring>
#include <random>
#include <chrono>
#include <algorithm>
#include <map>

// Endianness conversion utilities (Standards layer - platform neutral)
#ifdef _WIN32
    // Windows endianness conversion without winsock2.h contamination
    #include <cstdlib>
    #define htobe16(x) _byteswap_ushort(x)
    #define htobe32(x) _byteswap_ulong(x)  
    #define htobe64(x) _byteswap_uint64(x)
    #define be16toh(x) _byteswap_ushort(x)
    #define be32toh(x) _byteswap_ulong(x)
    #define be64toh(x) _byteswap_uint64(x)
#else
    #include <endian.h>
#endif

namespace IEEE {
namespace _802_1 {
namespace AS {
namespace _2021 {

// ============================================================================
// PTPHeader Implementation
// ============================================================================

PTPHeader::PTPHeader() 
    : message_type(0)
    , transport_specific(1)  // 802.1AS specific
    , version_ptp(PTP_VERSION_2_1)
    , message_length(0)
    , domain_number(GPTP_DEFAULT_DOMAIN)
    , minor_version_ptp(PTP_MINOR_VERSION_1)
    , flags(0)
    , correction_field(0)
    , message_type_specific(0)
    , sequence_id(0)
    , control_field(0)
    , log_message_interval(0) {
    
    source_port_identity.clock_identity.fill(0);
    source_port_identity.port_number = 0;
}

// ============================================================================
// Message Structure Implementations  
// ============================================================================

AnnounceMessage::AnnounceMessage() {
    header.message_type = static_cast<uint8_t>(MessageType::ANNOUNCE);
    header.message_length = sizeof(AnnounceMessage);
    current_utc_offset = 37;  // Current UTC-TAI offset (2021)
    priority1 = 248;          // gPTP default
    priority2 = 248;          // gPTP default
    steps_removed = 0;
    time_source = 0xA0;       // Internal oscillator
    grandmaster_identity.fill(0);
}

SyncMessage::SyncMessage() {
    header.message_type = static_cast<uint8_t>(MessageType::SYNC);
    header.message_length = sizeof(SyncMessage);
    header.flags = 0x0200;    // Two-step flag set for gPTP
}

FollowUpMessage::FollowUpMessage() {
    header.message_type = static_cast<uint8_t>(MessageType::FOLLOW_UP);
    header.message_length = sizeof(FollowUpMessage);
}

PDelayReqMessage::PDelayReqMessage() {
    header.message_type = static_cast<uint8_t>(MessageType::PDELAY_REQ);
    header.message_length = sizeof(PDelayReqMessage);
}

PDelayRespMessage::PDelayRespMessage() {
    header.message_type = static_cast<uint8_t>(MessageType::PDELAY_RESP);
    header.message_length = sizeof(PDelayRespMessage);
    requesting_port_identity.clock_identity.fill(0);
    requesting_port_identity.port_number = 0;
}

PDelayRespFollowUpMessage::PDelayRespFollowUpMessage() {
    header.message_type = static_cast<uint8_t>(MessageType::PDELAY_RESP_FOLLOW_UP);
    header.message_length = sizeof(PDelayRespFollowUpMessage);
    requesting_port_identity.clock_identity.fill(0);
    requesting_port_identity.port_number = 0;
}

// ============================================================================
// OrganizationExtensionTLV Implementation
// ============================================================================

OrganizationExtensionTLV::OrganizationExtensionTLV() : TLV(0x0003, 6) {
    organization_id = {{0x00, 0x80, 0xC2}};     // IEEE 802.1 OUI
    organization_sub_type = {{0x00, 0x00, 0x01}}; // gPTP sub-type
}

// ============================================================================
// TimeAwareSystem Implementation
// ============================================================================

class TimeAwareSystem::Implementation {
public:
    Implementation() : initialized_(false) {}
    
    bool initialize(uint8_t domain_number) {
        if (initialized_) {
            return true;
        }
        
        active_domains_.push_back(domain_number);
        
        // Initialize per-domain state
        domain_states_[domain_number] = DomainState{};
        domain_states_[domain_number].is_synchronized = false;
        domain_states_[domain_number].path_delay_mechanism = PathDelayMechanism::P2P;
        
        // Initialize clock quality for domain
        ClockQuality default_quality;
        domain_states_[domain_number].clock_quality = default_quality;
        
        initialized_ = true;
        return true;
    }
    
    bool shutdown() {
        active_domains_.clear();
        domain_states_.clear();
        initialized_ = false;
        return true;
    }
    
    bool add_domain(uint8_t domain_number) {
        if (::std::find(active_domains_.begin(), active_domains_.end(), domain_number) != active_domains_.end()) {
            return false; // Domain already exists
        }
        
        active_domains_.push_back(domain_number);
        domain_states_[domain_number] = DomainState{};
        return true;
    }
    
    bool remove_domain(uint8_t domain_number) {
        auto it = ::std::find(active_domains_.begin(), active_domains_.end(), domain_number);
        if (it == active_domains_.end()) {
            return false; // Domain doesn't exist
        }
        
        active_domains_.erase(it);
        domain_states_.erase(domain_number);
        return true;
    }
    
    ::std::vector<uint8_t> get_active_domains() const {
        return active_domains_;
    }
    
    bool get_time(Timestamp& current_time, uint8_t domain) const {
        auto it = domain_states_.find(domain);
        if (it == domain_states_.end()) {
            return false;
        }
        
        // Get current system time (implementation dependent)
        auto now = ::std::chrono::system_clock::now();
        auto epoch = now.time_since_epoch();
        auto seconds = ::std::chrono::duration_cast<::std::chrono::seconds>(epoch);
        auto nanoseconds = ::std::chrono::duration_cast<::std::chrono::nanoseconds>(epoch) - 
                          ::std::chrono::duration_cast<::std::chrono::nanoseconds>(seconds);
        
        current_time.seconds_field = seconds.count();
        current_time.nanoseconds_field = static_cast<uint32_t>(nanoseconds.count());
        
        return true;
    }
    
    ClockQuality get_clock_quality(uint8_t domain) const {
        auto it = domain_states_.find(domain);
        if (it != domain_states_.end()) {
            return it->second.clock_quality;
        }
        return ClockQuality{}; // Default quality
    }
    
    bool is_synchronized(uint8_t domain) const {
        auto it = domain_states_.find(domain);
        if (it != domain_states_.end()) {
            return it->second.is_synchronized;
        }
        return false;
    }
    
    TimeInterval get_offset_from_master(uint8_t domain) const {
        auto it = domain_states_.find(domain);
        if (it != domain_states_.end()) {
            return it->second.offset_from_master;
        }
        return 0;
    }
    
    bool set_path_delay_mechanism(PathDelayMechanism mechanism, uint8_t domain) {
        auto it = domain_states_.find(domain);
        if (it != domain_states_.end()) {
            it->second.path_delay_mechanism = mechanism;
            return true;
        }
        return false;
    }
    
    PathDelayMechanism get_path_delay_mechanism(uint8_t domain) const {
        auto it = domain_states_.find(domain);
        if (it != domain_states_.end()) {
            return it->second.path_delay_mechanism;
        }
        return PathDelayMechanism::P2P; // Default
    }

private:
    struct DomainState {
        ClockQuality clock_quality;
        bool is_synchronized = false;
        TimeInterval offset_from_master = 0;
        PathDelayMechanism path_delay_mechanism = PathDelayMechanism::P2P;
    };
    
    bool initialized_;
    ::std::vector<uint8_t> active_domains_;
    ::std::map<uint8_t, DomainState> domain_states_;
    ::std::vector<TransportMechanism> supported_transports_;
};

TimeAwareSystem::TimeAwareSystem() : pImpl(::std::make_unique<Implementation>()) {}

TimeAwareSystem::~TimeAwareSystem() = default;

bool TimeAwareSystem::initialize(uint8_t domain_number) {
    return pImpl->initialize(domain_number);
}

bool TimeAwareSystem::shutdown() {
    return pImpl->shutdown();
}

bool TimeAwareSystem::add_domain(uint8_t domain_number) {
    return pImpl->add_domain(domain_number);
}

bool TimeAwareSystem::remove_domain(uint8_t domain_number) {
    return pImpl->remove_domain(domain_number);
}

::std::vector<uint8_t> TimeAwareSystem::get_active_domains() const {
    return pImpl->get_active_domains();
}

bool TimeAwareSystem::get_time(Timestamp& current_time, uint8_t domain) const {
    return pImpl->get_time(current_time, domain);
}

bool TimeAwareSystem::set_time(const Timestamp& time, uint8_t domain) {
    // Implementation specific - would typically interface with system clock
    (void)time;
    (void)domain;
    return false; // Not implemented in this base class
}

ClockQuality TimeAwareSystem::get_clock_quality(uint8_t domain) const {
    return pImpl->get_clock_quality(domain);
}

bool TimeAwareSystem::is_synchronized(uint8_t domain) const {
    return pImpl->is_synchronized(domain);
}

TimeInterval TimeAwareSystem::get_offset_from_master(uint8_t domain) const {
    return pImpl->get_offset_from_master(domain);
}

bool TimeAwareSystem::set_path_delay_mechanism(PathDelayMechanism mechanism, uint8_t domain) {
    return pImpl->set_path_delay_mechanism(mechanism, domain);
}

PathDelayMechanism TimeAwareSystem::get_path_delay_mechanism(uint8_t domain) const {
    return pImpl->get_path_delay_mechanism(domain);
}

bool TimeAwareSystem::add_transport_mechanism(TransportMechanism transport) {
    // Implementation specific
    (void)transport;
    return false; // Not implemented in base class
}

::std::vector<TransportMechanism> TimeAwareSystem::get_supported_transports() const {
    return {}; // Empty in base class
}

// ============================================================================
// GPTPPort Implementation
// ============================================================================

class GPTPPort::Implementation {
public:
    Implementation(uint16_t port_number) 
        : port_number_(port_number)
        , port_state_(PortState::INITIALIZING)
        , mean_path_delay_(0)
        , path_delay_valid_(false) {
        
        port_identity_.port_number = port_number;
        port_identity_.clock_identity.fill(0);
    }
    
    PortState get_port_state() const { return port_state_; }
    bool set_port_state(PortState state) { port_state_ = state; return true; }
    PortIdentity get_port_identity() const { return port_identity_; }
    TimeInterval get_mean_path_delay() const { return mean_path_delay_; }
    bool is_path_delay_valid() const { return path_delay_valid_; }

private:
    uint16_t port_number_;
    PortState port_state_;
    PortIdentity port_identity_;
    TimeInterval mean_path_delay_;
    bool path_delay_valid_;
};

GPTPPort::GPTPPort(uint16_t port_number) : pImpl(::std::make_unique<Implementation>(port_number)) {}

GPTPPort::~GPTPPort() = default;

GPTPPort::PortState GPTPPort::get_port_state() const {
    return pImpl->get_port_state();
}

bool GPTPPort::set_port_state(PortState state) {
    return pImpl->set_port_state(state);
}

PortIdentity GPTPPort::get_port_identity() const {
    return pImpl->get_port_identity();
}

TimeInterval GPTPPort::get_mean_path_delay() const {
    return pImpl->get_mean_path_delay();
}

bool GPTPPort::is_path_delay_valid() const {
    return pImpl->is_path_delay_valid();
}

// ============================================================================
// IEEE1588Clock Implementation
// ============================================================================

class IEEE1588Clock::Implementation {
public:
    Implementation(const ClockIdentity& identity) 
        : clock_identity_(identity)
        , priority1_(248)  // gPTP default
        , priority2_(248)  // gPTP default
        , is_master_(false)
        , is_slave_(false)
        , offset_from_master_(0) {
        
        // Initialize default clock quality
        clock_quality_.clock_class = 248;
        clock_quality_.clock_accuracy = 0xFE;
        clock_quality_.offset_scaled_log_variance = 0x4E5D;
    }
    
    ClockIdentity get_clock_identity() const { return clock_identity_; }
    ClockQuality get_clock_quality() const { return clock_quality_; }
    bool set_clock_quality(const ClockQuality& quality) { clock_quality_ = quality; return true; }
    
    uint8_t get_priority1() const { return priority1_; }
    uint8_t get_priority2() const { return priority2_; }
    void set_priority1(uint8_t priority) { priority1_ = priority; }
    void set_priority2(uint8_t priority) { priority2_ = priority; }
    
    bool is_master() const { return is_master_; }
    bool is_slave() const { return is_slave_; }
    TimeInterval get_offset_from_master() const { return offset_from_master_; }
    void set_offset_from_master(TimeInterval offset) { offset_from_master_ = offset; }
    
    bool is_better_than(const AnnounceMessage& announce) const {
        // Implement BMCA (Best Master Clock Algorithm) comparison
        
        // Compare priority1
        if (priority1_ != announce.priority1) {
            return priority1_ < announce.priority1;
        }
        
        // Compare clock class
        if (clock_quality_.clock_class != announce.grandmaster_clock_quality.clock_class) {
            return clock_quality_.clock_class < announce.grandmaster_clock_quality.clock_class;
        }
        
        // Compare clock accuracy
        if (clock_quality_.clock_accuracy != announce.grandmaster_clock_quality.clock_accuracy) {
            return clock_quality_.clock_accuracy < announce.grandmaster_clock_quality.clock_accuracy;
        }
        
        // Compare offset scaled log variance
        if (clock_quality_.offset_scaled_log_variance != announce.grandmaster_clock_quality.offset_scaled_log_variance) {
            return clock_quality_.offset_scaled_log_variance < announce.grandmaster_clock_quality.offset_scaled_log_variance;
        }
        
        // Compare priority2
        if (priority2_ != announce.priority2) {
            return priority2_ < announce.priority2;
        }
        
        // Compare clock identity
        return ::std::memcmp(clock_identity_.data(), announce.grandmaster_identity.data(), 8) < 0;
    }

private:
    ClockIdentity clock_identity_;
    ClockQuality clock_quality_;
    uint8_t priority1_;
    uint8_t priority2_;
    bool is_master_;
    bool is_slave_;
    TimeInterval offset_from_master_;
};

IEEE1588Clock::IEEE1588Clock(const ClockIdentity& identity) 
    : pImpl(::std::make_unique<Implementation>(identity)) {}

IEEE1588Clock::~IEEE1588Clock() = default;

ClockIdentity IEEE1588Clock::get_clock_identity() const {
    return pImpl->get_clock_identity();
}

ClockQuality IEEE1588Clock::get_clock_quality() const {
    return pImpl->get_clock_quality();
}

bool IEEE1588Clock::set_clock_quality(const ClockQuality& quality) {
    return pImpl->set_clock_quality(quality);
}

bool IEEE1588Clock::is_better_than(const AnnounceMessage& announce) const {
    return pImpl->is_better_than(announce);
}

uint8_t IEEE1588Clock::get_priority1() const {
    return pImpl->get_priority1();
}

uint8_t IEEE1588Clock::get_priority2() const {
    return pImpl->get_priority2();
}

void IEEE1588Clock::set_priority1(uint8_t priority) {
    pImpl->set_priority1(priority);
}

void IEEE1588Clock::set_priority2(uint8_t priority) {
    pImpl->set_priority2(priority);
}

bool IEEE1588Clock::is_master() const {
    return pImpl->is_master();
}

bool IEEE1588Clock::is_slave() const {
    return pImpl->is_slave();
}

TimeInterval IEEE1588Clock::get_offset_from_master() const {
    return pImpl->get_offset_from_master();
}

void IEEE1588Clock::set_offset_from_master(TimeInterval offset) {
    pImpl->set_offset_from_master(offset);
}

// ============================================================================
// Utility Functions Implementation
// ============================================================================

namespace Utils {

::std::vector<uint8_t> serialize_message(const PTPHeader& header, const void* message_body, size_t body_size) {
    ::std::vector<uint8_t> result;
    
    // Serialize header
    result.resize(sizeof(PTPHeader) + body_size);
    ::std::memcpy(result.data(), &header, sizeof(PTPHeader));
    
    if (message_body && body_size > 0) {
        ::std::memcpy(result.data() + sizeof(PTPHeader), message_body, body_size);
    }
    
    // Convert to network byte order
    // Note: This is a simplified version - full implementation would handle all fields
    
    return result;
}

bool deserialize_message(const ::std::vector<uint8_t>& data, PTPHeader& header, void* message_body, size_t body_size) {
    if (data.size() < sizeof(PTPHeader)) {
        return false;
    }
    
    // Copy header
    ::std::memcpy(&header, data.data(), sizeof(PTPHeader));
    
    // Convert from network byte order
    // Note: This is a simplified version - full implementation would handle all fields
    
    // Copy body if provided
    if (message_body && body_size > 0 && data.size() >= sizeof(PTPHeader) + body_size) {
        ::std::memcpy(message_body, data.data() + sizeof(PTPHeader), body_size);
    }
    
    return true;
}

int64_t calculate_correction_field(TimeInterval path_delay, TimeInterval residence_time) {
    // Correction field is in units of 2^-16 nanoseconds (sub-nanosecond precision)
    return ((path_delay + residence_time) << 16);
}

bool validate_message_format(const ::std::vector<uint8_t>& message) {
    if (message.size() < sizeof(PTPHeader)) {
        return false;
    }
    
    PTPHeader header;
    ::std::memcpy(&header, message.data(), sizeof(PTPHeader));
    
    // Basic validation
    if (header.version_ptp != PTP_VERSION_2_1) {
        return false;
    }
    
    if (header.transport_specific != 1) {  // Must be 1 for 802.1AS
        return false;
    }
    
    if (header.message_length != message.size()) {
        return false;
    }
    
    return true;
}

uint64_t timestamp_to_nanoseconds(const Timestamp& ts) {
    return (static_cast<uint64_t>(ts.seconds_field) * 1000000000ULL) + ts.nanoseconds_field;
}

Timestamp nanoseconds_to_timestamp(uint64_t nanoseconds) {
    uint64_t seconds = nanoseconds / 1000000000ULL;
    uint32_t ns = static_cast<uint32_t>(nanoseconds % 1000000000ULL);
    return Timestamp(seconds, ns);
}

ClockIdentity generate_clock_identity() {
    ClockIdentity identity;
    
    // Generate based on MAC address + random/time component
    // This is a simplified version - production should use actual MAC address
    ::std::random_device rd;
    ::std::mt19937 gen(rd());
    ::std::uniform_int_distribution<int> dis(0, 255);
    
    for (size_t i = 0; i < identity.size(); ++i) {
        identity[i] = static_cast<uint8_t>(dis(gen));
    }
    
    // Ensure it follows IEEE 1588 format (should be derived from MAC)
    identity[3] = 0xFF;
    identity[4] = 0xFE;
    
    return identity;
}

bool validate_clock_identity(const ClockIdentity& identity) {
    // Check for valid IEEE 1588 clock identity format
    // Should not be all zeros or all ones
    bool all_zero = true;
    bool all_ones = true;
    
    for (uint8_t byte : identity) {
        if (byte != 0) all_zero = false;
        if (byte != 0xFF) all_ones = false;
    }
    
    return !all_zero && !all_ones;
}

} // namespace Utils

} // namespace _2021
} // namespace AS
} // namespace _802_1
} // namespace IEEE
