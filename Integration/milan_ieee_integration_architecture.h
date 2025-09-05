/**
 * @file milan_ieee_integration_architecture.h
 * @brief Milan-IEEE Integration Architecture Design
 * @details Comprehensive bridge architecture to integrate Milan v1.2 requirements 
 *          with IEEE::_802_1::* namespace implementations
 * 
 * ARCHITECTURE OVERVIEW:
 * ======================
 * This design solves the critical gap where Milan v1.2 operates standalone
 * despite being designed to integrate with IEEE standards implementations.
 * 
 * INTEGRATION LAYERS:
 * ------------------
 * 1. Milan Application Layer (AVnu::Milan::_1_2_2023)
 * 2. Standards Bridge Layer (NEW - this architecture)  
 * 3. IEEE Standards Layer (IEEE::_802_1::AS::_2021, IEEE::_1722_1::_2021, etc.)
 * 4. Hardware Abstraction Layer (Intel HAL)
 * 
 * @author OpenAvnu Integration Team
 * @date September 2025
 * @copyright OpenAvnu Project
 */

#pragma once

#include "../AVnu/Milan/1.2-2023/AVnu_Milan_1_2-2023_complete.h"
#include "../IEEE/802.1AS/2021/core/ieee_802_1as_2021.h"
#include "../IEEE/1722.1/2021/core/ieee_1722_1_2021_base.h"
#include "../IEEE/1722/2016/core/ieee_1722_2016.h"
#include <memory>
#include <functional>
#include <chrono>
#include <vector>
#include <map>
#include <string>

namespace OpenAvnu {
namespace Integration {
namespace Milan_IEEE {

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

class MilanIEEEBridge;
class StandardsCoordinator;
class ProtocolMapper;

// ============================================================================
// INTERFACE ABSTRACTIONS
// ============================================================================

/**
 * @brief Abstract interface for gPTP integration
 * 
 * This interface abstracts IEEE 802.1AS-2021 gPTP operations for Milan v1.2
 * requirements including professional audio timing and synchronization.
 */
class IGPTPProvider {
public:
    virtual ~IGPTPProvider() = default;
    
    // Core gPTP operations
    virtual bool initialize_domain(uint8_t domain_number) = 0;
    virtual bool is_grandmaster() = 0;
    virtual uint64_t get_current_time_ns() = 0;
    virtual bool is_time_synchronized() = 0;
    virtual double get_sync_accuracy_ns() = 0;
    
    // Milan-specific timing requirements
    virtual bool meets_milan_timing_requirements() = 0;
    virtual bool has_stable_grandmaster(std::chrono::seconds min_stable_time) = 0;
    virtual uint32_t get_path_trace_length() = 0;
    
    // Event callbacks for Milan integration
    virtual void set_grandmaster_change_callback(std::function<void(uint64_t old_gm, uint64_t new_gm)> callback) = 0;
    virtual void set_sync_loss_callback(std::function<void()> callback) = 0;
};

/**
 * @brief Abstract interface for AVDECC integration
 * 
 * This interface abstracts IEEE 1722.1-2021 AVDECC operations for Milan v1.2
 * professional audio device requirements.
 */
class IAVDECCProvider {
public:
    virtual ~IAVDECCProvider() = default;
    
    // Entity management
    virtual bool initialize_entity(uint64_t entity_id, uint64_t entity_model_id) = 0;
    virtual bool start_advertisement() = 0;
    virtual bool stop_advertisement() = 0;
    
    // Milan-specific AVDECC features
    virtual bool register_milan_vendor_unique_commands() = 0;
    virtual bool handle_milan_get_info_command(const AVnu::Milan::_1_2_2023::GetMilanInfoCommand& cmd,
                                               AVnu::Milan::_1_2_2023::GetMilanInfoResponse& response) = 0;
    virtual bool handle_milan_system_id_commands() = 0;
    
    // Stream management
    virtual bool configure_milan_stream_format(uint16_t stream_index, const std::vector<uint8_t>& format) = 0;
    virtual bool setup_milan_redundancy(uint16_t primary_stream, uint16_t secondary_stream) = 0;
    
    // Discovery and connection
    virtual void set_entity_discovered_callback(std::function<void(uint64_t entity_id)> callback) = 0;
    virtual void set_stream_connection_callback(std::function<void(uint16_t stream_id, bool connected)> callback) = 0;
};

/**
 * @brief Abstract interface for AVTP integration
 * 
 * This interface abstracts IEEE 1722-2016 AVTP operations for Milan v1.2
 * audio streaming requirements.
 */
class IAVTPProvider {
public:
    virtual ~IAVTPProvider() = default;
    
    // Stream setup
    virtual bool create_milan_audio_stream(uint64_t stream_id, const std::vector<uint8_t>& format) = 0;
    virtual bool start_audio_streaming(uint64_t stream_id) = 0;
    virtual bool stop_audio_streaming(uint64_t stream_id) = 0;
    
    // Milan audio format support
    virtual bool configure_base_audio_format(uint32_t sample_rate, uint16_t channels, uint16_t bit_depth) = 0;
    virtual bool setup_milan_presentation_time(uint64_t stream_id, uint32_t offset_ns) = 0;
    
    // Media clock streaming
    virtual bool create_milan_crf_stream(uint64_t stream_id, uint32_t base_frequency) = 0;
    virtual bool synchronize_to_media_clock(uint64_t crf_stream_id) = 0;
    
    // Quality monitoring
    virtual uint32_t get_stream_packet_loss(uint64_t stream_id) = 0;
    virtual bool meets_milan_latency_requirements(uint64_t stream_id) = 0;
};

// ============================================================================
// DEPENDENCY INJECTION CONTAINER
// ============================================================================

/**
 * @brief Standards Provider Container
 * 
 * Dependency injection container that provides IEEE standards implementations
 * to Milan layer through abstract interfaces.
 */
class StandardsContainer {
private:
    std::shared_ptr<IGPTPProvider> gptp_provider_;
    std::shared_ptr<IAVDECCProvider> avdecc_provider_;
    std::shared_ptr<IAVTPProvider> avtp_provider_;
    
public:
    // Provider registration
    void register_gptp_provider(std::shared_ptr<IGPTPProvider> provider) {
        gptp_provider_ = provider;
    }
    
    void register_avdecc_provider(std::shared_ptr<IAVDECCProvider> provider) {
        avdecc_provider_ = provider;
    }
    
    void register_avtp_provider(std::shared_ptr<IAVTPProvider> provider) {
        avtp_provider_ = provider;
    }
    
    // Provider access
    std::shared_ptr<IGPTPProvider> get_gptp_provider() const { return gptp_provider_; }
    std::shared_ptr<IAVDECCProvider> get_avdecc_provider() const { return avdecc_provider_; }
    std::shared_ptr<IAVTPProvider> get_avtp_provider() const { return avtp_provider_; }
    
    // Validation
    bool all_providers_registered() const {
        return gptp_provider_ && avdecc_provider_ && avtp_provider_;
    }
};

// ============================================================================
// CONCRETE IEEE PROVIDER IMPLEMENTATIONS
// ============================================================================

/**
 * @brief IEEE 802.1AS-2021 gPTP Provider Implementation
 * 
 * Concrete implementation that bridges Milan requirements to our
 * IEEE::_802_1::AS::_2021 namespace implementation.
 */
class IEEE802_1AS_2021_Provider : public IGPTPProvider {
private:
    IEEE::_802_1::AS::_2021::ClockIdentity clock_identity_;
    IEEE::_802_1::AS::_2021::Timestamp current_time_;
    IEEE::_802_1::AS::_2021::ClockQuality clock_quality_;
    std::function<void(uint64_t, uint64_t)> gm_change_callback_;
    std::function<void()> sync_loss_callback_;
    bool initialized_;
    uint8_t domain_number_;
    std::unique_ptr<IEEE::_802_1::AS::_2021::TimeAwareSystem> time_aware_system_;
    
public:
    IEEE802_1AS_2021_Provider();
    ~IEEE802_1AS_2021_Provider() override;
    
    // IGPTPProvider implementation
    bool initialize_domain(uint8_t domain_number) override;
    bool is_grandmaster() override;
    uint64_t get_current_time_ns() override;
    bool is_time_synchronized() override;
    double get_sync_accuracy_ns() override;
    
    // Milan-specific implementations
    bool meets_milan_timing_requirements() override;
    bool has_stable_grandmaster(std::chrono::seconds min_stable_time) override;
    uint32_t get_path_trace_length() override;
    
    // Callback registration
    void set_grandmaster_change_callback(std::function<void(uint64_t, uint64_t)> callback) override;
    void set_sync_loss_callback(std::function<void()> callback) override;
};

/**
 * @brief IEEE 1722.1-2021 AVDECC Provider Implementation
 * 
 * Concrete implementation that bridges Milan requirements to our
 * IEEE::_1722_1::_2021 namespace implementation.
 */
class IEEE1722_1_2021_Provider : public IAVDECCProvider {
private:
    IEEE::_1722_1::_2021::EntityID entity_id_;
    IEEE::_1722_1::_2021::EntityID entity_model_id_;
    IEEE::_1722_1::_2021::AECP::AECPMessageType current_message_type_;
    IEEE::_1722_1::_2021::AECP::AEMCommandType current_command_type_;
    AVnu::Milan::_1_2_2023::MilanPAADEntity* milan_entity_;
    bool initialized_;
    bool advertising_;
    std::function<void(uint64_t)> entity_discovered_callback_;
    std::function<void(uint16_t, bool)> stream_connection_callback_;
    
    // Milan professional audio entity configuration
    struct {
        uint64_t entity_id;
        uint64_t entity_model_id;
        uint32_t entity_capabilities;
        uint16_t talker_stream_sources;
        uint16_t talker_capabilities;
        uint16_t listener_stream_sinks;
        uint16_t listener_capabilities;
        uint32_t controller_capabilities;
        uint32_t available_index;
        uint16_t identify_control_index;
        uint16_t interface_index;
        uint64_t association_id;
    } entity_descriptor_;
    
    // Milan vendor command support
    bool milan_vendor_commands_registered_;
    bool milan_get_info_supported_;
    bool milan_system_id_supported_;
    
    // Professional audio stream management
    std::map<uint16_t, std::vector<uint8_t>> stream_configurations_;
    std::map<uint16_t, uint16_t> redundancy_pairs_;  // primary -> secondary stream mapping
    
public:
    IEEE1722_1_2021_Provider(AVnu::Milan::_1_2_2023::MilanPAADEntity* milan_entity);
    ~IEEE1722_1_2021_Provider() override;
    
    // IAVDECCProvider implementation
    bool initialize_entity(uint64_t entity_id, uint64_t entity_model_id) override;
    bool start_advertisement() override;
    bool stop_advertisement() override;
    
    // Milan-specific implementations
    bool register_milan_vendor_unique_commands() override;
    bool handle_milan_get_info_command(const AVnu::Milan::_1_2_2023::GetMilanInfoCommand& cmd,
                                       AVnu::Milan::_1_2_2023::GetMilanInfoResponse& response) override;
    bool handle_milan_system_id_commands() override;
    
    // Stream management
    bool configure_milan_stream_format(uint16_t stream_index, const std::vector<uint8_t>& format) override;
    bool setup_milan_redundancy(uint16_t primary_stream, uint16_t secondary_stream) override;
    
    // Callbacks
    void set_entity_discovered_callback(std::function<void(uint64_t)> callback) override;
    void set_stream_connection_callback(std::function<void(uint16_t, bool)> callback) override;
};

// ============================================================================
// CONFIGURATION STRUCTURES
// ============================================================================

/**
 * @brief Milan-IEEE Configuration Structure
 */
struct MilanIEEEConfig {
    uint8_t gptp_domain = 0;
    uint32_t milan_capabilities = 0;
    uint16_t milan_features = 0;
    bool redundancy_enabled = false;
    
    // IEEE-specific configurations
    IEEE::_802_1::AS::_2021::ClockQuality clock_quality;
    IEEE::_1722_1::_2021::EntityID entity_id;
    IEEE::_1722_1::_2021::EntityID entity_model_id;
};

// ============================================================================
// MILAN-IEEE INTEGRATION BRIDGE
// ============================================================================

/**
 * @brief Main Milan-IEEE Integration Bridge
 * 
 * This is the central integration component that coordinates Milan v1.2
 * operations with IEEE standards implementations through dependency injection.
 */
class MilanIEEEBridge {
private:
    StandardsContainer standards_;
    std::unique_ptr<AVnu::Milan::_1_2_2023::MilanPAADEntity> milan_entity_;
    bool initialized_;
    
    // Configuration mapping
    MilanIEEEConfig config_;
    
public:
    /**
     * @brief Constructor
     * @param entity_id Milan entity identifier
     * @param entity_model_id Milan entity model identifier
     */
    MilanIEEEBridge(uint64_t entity_id, uint64_t entity_model_id);
    
    /**
     * @brief Initialize the integration bridge
     * @return true if successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Register IEEE standards providers
     * @param container Pre-configured standards container
     * @return true if successful, false otherwise
     */
    bool register_standards_providers(const StandardsContainer& container);
    
    /**
     * @brief Start Milan entity with IEEE standards integration
     * @return true if successful, false otherwise
     */
    bool start_milan_entity();
    
    /**
     * @brief Stop Milan entity and cleanup IEEE integration
     * @return true if successful, false otherwise
     */
    bool stop_milan_entity();
    
    /**
     * @brief Configure Milan requirements through IEEE implementations
     * @param milan_config Milan-specific configuration
     * @return true if successful, false otherwise
     */
    bool configure_milan_requirements(const MilanIEEEConfig& milan_config);
    
    /**
     * @brief Get the underlying Milan entity
     * @return Pointer to Milan entity (may be null)
     */
    AVnu::Milan::_1_2_2023::MilanPAADEntity* get_milan_entity() const;
    
    /**
     * @brief Check if bridge is properly initialized
     * @return true if initialized and ready, false otherwise
     */
    bool is_initialized() const { return initialized_; }
    
    // Status and monitoring
    bool verify_milan_compliance() const;
    bool check_ieee_standards_health() const;
    std::string get_integration_status() const;
};

// ============================================================================
// CONFIGURATION MAPPING UTILITIES
// ============================================================================

/**
 * @brief Protocol Requirements Mapper
 * 
 * Maps Milan v1.2 professional audio requirements to specific IEEE
 * standards parameters and configurations.
 */
class ProtocolMapper {
public:
    /**
     * @brief Map Milan timing requirements to gPTP configuration
     * @param milan_features Milan feature flags
     * @return IEEE 802.1AS-2021 configuration
     */
    static IEEE::_802_1::AS::_2021::ClockQuality map_milan_timing_to_gptp(uint16_t milan_features);
    
    /**
     * @brief Map Milan audio formats to AVTP stream configuration
     * @param base_format Milan base audio format
     * @return IEEE 1722-2016 stream format
     */
    static std::vector<uint8_t> map_milan_audio_to_avtp(const AVnu::Milan::_1_2_2023::MilanBaseAudioFormat& base_format);
    
    /**
     * @brief Map Milan device capabilities to AVDECC entity model
     * @param milan_capabilities Milan capability flags
     * @return IEEE 1722.1-2021 entity configuration
     */
    static IEEE::_1722_1::_2021::EntityID map_milan_capabilities_to_avdecc(uint32_t milan_capabilities);
    
    /**
     * @brief Validate Milan-IEEE parameter compatibility
     * @param milan_config Milan configuration
     * @param ieee_config IEEE standards configuration
     * @return true if compatible, false otherwise
     */
    static bool validate_protocol_compatibility(const MilanIEEEConfig& milan_config,
                                                const IEEE::_802_1::AS::_2021::ClockQuality& ieee_config);
};

// ============================================================================
// FACTORY AND BUILDER PATTERN
// ============================================================================

/**
 * @brief Milan-IEEE Integration Factory
 * 
 * Factory class for creating properly configured Milan-IEEE integration
 * instances with all necessary IEEE providers.
 */
class MilanIEEEFactory {
public:
    /**
     * @brief Create complete Milan-IEEE integration instance
     * @param entity_id Milan entity identifier
     * @param entity_model_id Milan entity model identifier
     * @param config Integration configuration
     * @return Configured integration bridge
     */
    static std::unique_ptr<MilanIEEEBridge> create_integrated_milan_entity(
        uint64_t entity_id,
        uint64_t entity_model_id,
        const MilanIEEEConfig& config
    );
    
    /**
     * @brief Create standards container with IEEE implementations
     * @return Configured standards container
     */
    static StandardsContainer create_ieee_standards_container();
    
    /**
     * @brief Validate factory prerequisites
     * @return true if all IEEE implementations available, false otherwise
     */
    static bool validate_prerequisites();
};

} // namespace Milan_IEEE
} // namespace Integration
} // namespace OpenAvnu
