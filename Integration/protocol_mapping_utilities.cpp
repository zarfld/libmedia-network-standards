/**
 * @file protocol_mapping_utilities.cpp
 * @brief Protocol Mapping Utilities for Milan-IEEE Integration
 * @details Implementation of utilities for mapping between Milan v1.2 professional 
 *          audio requirements and IEEE standards parameters
 * 
 * @author OpenAvnu Integration Team
 * @date September 2025
 * @copyright OpenAvnu Project
 */

#include "milan_ieee_integration_architecture.h"
#include "../AVnu/Milan/1.2-2023/AVnu_Milan_1_2-2023_complete.h"
#include "../IEEE/802.1AS/2021/core/ieee_802_1as_2021.h"
#include "../IEEE/1722.1/2021/core/ieee_1722_1_2021_base.h"
#include <array>
#include <cstring>

namespace OpenAvnu {
namespace Integration {
namespace Milan_IEEE {

// ============================================================================
// Protocol Requirements Mapper Implementation
// ============================================================================

IEEE::_802_1::AS::_2021::ClockQuality ProtocolMapper::map_milan_timing_to_gptp(uint16_t milan_features) {
    IEEE::_802_1::AS::_2021::ClockQuality clock_quality;
    
    // Check Milan features for timing requirements
    bool has_professional_audio = (milan_features & 
        static_cast<uint16_t>(AVnu::Milan::_1_2_2023::MilanProtocolFeatures::PROFESSIONAL_AUDIO)) != 0;
    
    bool has_media_clock_management = (milan_features & 
        static_cast<uint16_t>(AVnu::Milan::_1_2_2023::MilanProtocolFeatures::MEDIA_CLOCK_MANAGEMENT)) != 0;
    
    if (has_professional_audio) {
        // Professional audio requires very high precision timing
        clock_quality.clock_class = 6;          // Primary reference clock class
        clock_quality.clock_accuracy = 0x20;    // 25ns accuracy (meets Milan ±80ns requirement)
        clock_quality.offset_scaled_log_variance = 0x4321; // Low variance for stability
    } else if (has_media_clock_management) {
        // Media clocking requires good timing but not as stringent as professional audio
        clock_quality.clock_class = 7;          // Primary reference (holdover)
        clock_quality.clock_accuracy = 0x21;    // 100ns accuracy
        clock_quality.offset_scaled_log_variance = 0x4521; // Moderate variance
    } else {
        // Standard Milan baseline timing requirements
        clock_quality.clock_class = 248;        // Default gPTP clock class
        clock_quality.clock_accuracy = 0x22;    // 250ns accuracy
        clock_quality.offset_scaled_log_variance = 0x4E5D; // Standard gPTP variance
    }
    
    return clock_quality;
}

std::vector<uint8_t> ProtocolMapper::map_milan_audio_to_avtp(
    const AVnu::Milan::_1_2_2023::MilanBaseAudioFormat& base_format) {
    
    std::vector<uint8_t> avtp_format;
    avtp_format.reserve(64); // Reserve space for AVTP stream format descriptor
    
    // Milan Base Audio Format Mapping to IEEE 1722-2016 AVTP
    // Per Milan v1.2 Section 6.5 - Base Audio Stream Formats
    
    // AVTP Stream Format Header (per IEEE 1722-2016)
    avtp_format.push_back(0x02); // Stream format subtype (AAF - AVTP Audio Format)
    avtp_format.push_back(0x00); // Reserved
    avtp_format.push_back(0x00); // Stream format version
    avtp_format.push_back(0x00); // Reserved
    
    // Decode Milan base format enum to individual parameters
    uint64_t format_value = static_cast<uint64_t>(base_format);
    
    // Extract sample rate from Milan format encoding
    uint32_t sample_rate_code = (format_value >> 24) & 0xFF;
    switch (sample_rate_code) {
        case 0x08: // 48 kHz
            avtp_format.push_back(0x02);
            break;
        case 0x10: // 96 kHz
            avtp_format.push_back(0x04);
            break;
        default:
            avtp_format.push_back(0x02); // Default to 48 kHz
            break;
    }
    
    // Extract channel count from Milan format encoding
    uint8_t channel_count = format_value & 0xFF;
    avtp_format.push_back(channel_count);
    
    // Extract bit depth from Milan format encoding
    uint32_t bit_depth_code = (format_value >> 16) & 0xFF;
    switch (bit_depth_code) {
        case 0x02: // 24-bit
            avtp_format.push_back(0x18);
            break;
        case 0x04: // 32-bit
            avtp_format.push_back(0x20);
            break;
        default:
            avtp_format.push_back(0x18); // Default to 24-bit
            break;
    }
    
    // Sample format (Milan uses PCM for AAF formats)
    if ((format_value & 0x00A0000000000000ULL) == 0x00A0000000000000ULL) {
        avtp_format.push_back(0x01); // PCM format
    } else {
        avtp_format.push_back(0x02); // IEC 61883-6 format
    }
    
    // Additional AVTP format fields for Milan compatibility
    avtp_format.push_back(0x00); // Reserved
    avtp_format.push_back(0x00); // Reserved
    avtp_format.push_back(0x00); // Reserved
    avtp_format.push_back(0x00); // Reserved
    
    // Milan-specific extensions (in AVTP format reserved area)
    // Milan always includes redundancy capability
    avtp_format.push_back(0x01); // Redundancy capable
    
    // Professional audio class marker
    avtp_format.push_back(0x4D); // 'M' for Milan
    avtp_format.push_back(0x4C); // 'L' 
    avtp_format.push_back(0x41); // 'A'
    avtp_format.push_back(0x4E); // 'N'
    
    return avtp_format;
}

IEEE::_1722_1::_2021::EntityID ProtocolMapper::map_milan_capabilities_to_avdecc(uint32_t milan_capabilities) {
    // Convert Milan capabilities to AVDECC entity configuration
    // This mapping creates an Entity ID that encodes Milan capability information
    
    IEEE::_1722_1::_2021::EntityID entity_config = 0;
    
    // Base Milan AVDECC entity identifier
    entity_config |= 0x001B920000000000ULL; // Milan OUI prefix (0x001B92)
    
    // Encode Milan capabilities in the entity ID lower bits
    if (milan_capabilities & static_cast<uint32_t>(
        AVnu::Milan::_1_2_2023::MilanCapabilityFlags::BASELINE_INTEROPERABILITY)) {
        entity_config |= 0x0001;
    }
    
    if (milan_capabilities & static_cast<uint32_t>(
        AVnu::Milan::_1_2_2023::MilanCapabilityFlags::DISCOVERY_CONNECTION_CONTROL)) {
        entity_config |= 0x0002;
    }
    
    if (milan_capabilities & static_cast<uint32_t>(
        AVnu::Milan::_1_2_2023::MilanCapabilityFlags::BASE_STREAM_FORMATS)) {
        entity_config |= 0x0004;
    }
    
    if (milan_capabilities & static_cast<uint32_t>(
        AVnu::Milan::_1_2_2023::MilanCapabilityFlags::MEDIA_CLOCKING)) {
        entity_config |= 0x0008;
    }
    
    if (milan_capabilities & static_cast<uint32_t>(
        AVnu::Milan::_1_2_2023::MilanCapabilityFlags::SEAMLESS_REDUNDANCY)) {
        entity_config |= 0x0010;
    }
    
    if (milan_capabilities & static_cast<uint32_t>(
        AVnu::Milan::_1_2_2023::MilanCapabilityFlags::GPTA_AS_MEDIA_CLOCK)) {
        entity_config |= 0x0020;
    }
    
    if (milan_capabilities & static_cast<uint32_t>(
        AVnu::Milan::_1_2_2023::MilanCapabilityFlags::MEDIA_CLOCK_REFERENCE)) {
        entity_config |= 0x0040;
    }
    
    if (milan_capabilities & static_cast<uint32_t>(
        AVnu::Milan::_1_2_2023::MilanCapabilityFlags::DYNAMIC_MAPPING)) {
        entity_config |= 0x0080;
    }
    
    if (milan_capabilities & static_cast<uint32_t>(
        AVnu::Milan::_1_2_2023::MilanCapabilityFlags::SYSTEM_UNIQUE_ID)) {
        entity_config |= 0x0100;
    }
    
    return entity_config;
}

bool ProtocolMapper::validate_protocol_compatibility(
    const MilanIEEEConfig& milan_config,
    const IEEE::_802_1::AS::_2021::ClockQuality& ieee_config) {
    
    // Validate that the IEEE configuration can support Milan requirements
    
    // Check gPTP domain compatibility
    if (milan_config.gptp_domain > 255) {
        return false; // Invalid domain number
    }
    
    // Check if IEEE clock quality meets Milan timing requirements
    bool milan_has_professional_audio = (milan_config.milan_features & 
        static_cast<uint16_t>(AVnu::Milan::_1_2_2023::MilanProtocolFeatures::PROFESSIONAL_AUDIO)) != 0;
    
    if (milan_has_professional_audio) {
        // Professional audio requires very high timing accuracy
        if (ieee_config.clock_accuracy > 0x22) { // Worse than 250ns
            return false; // Not sufficient for professional audio
        }
        
        if (ieee_config.clock_class > 127) { // Not a high-quality clock
            return false; // Professional audio needs high-quality clock source
        }
    }
    
    // Check Milan redundancy requirements
    if (milan_config.redundancy_enabled) {
        // Redundancy requires specific clock configuration for seamless switching
        if (ieee_config.offset_scaled_log_variance > 0x5000) {
            return false; // Too much variance for seamless redundancy
        }
    }
    
    // Check media clock management requirements
    bool milan_has_media_clocking = (milan_config.milan_capabilities & 
        static_cast<uint32_t>(AVnu::Milan::_1_2_2023::MilanCapabilityFlags::MEDIA_CLOCKING)) != 0;
    
    if (milan_has_media_clocking) {
        // Media clocking requires stable timing reference
        if (ieee_config.clock_class == 255) { // Slave-only clock
            return false; // Cannot provide media clock reference
        }
    }
    
    // All validation checks passed
    return true;
}

// ============================================================================
// Milan-IEEE Integration Factory Implementation
// ============================================================================

// NOTE: create_integrated_milan_entity() implementation moved to 
// milan_ieee_bridge_implementation.cpp to avoid duplicate symbols

StandardsContainer MilanIEEEFactory::create_ieee_standards_container() {
    StandardsContainer container;
    
    // Create IEEE 802.1AS-2021 gPTP provider
    auto gptp_provider = std::make_shared<IEEE802_1AS_2021_Provider>();
    container.register_gptp_provider(gptp_provider);
    
    // Create IEEE 1722.1-2021 AVDECC provider (requires Milan entity)
    // Note: Milan entity will be created by the bridge, so we'll register this later
    
    // Create IEEE 1722-2016 AVTP provider
    // TODO: Implement IEEE1722_2016_Provider in next task
    
    return container;
}

bool MilanIEEEFactory::validate_prerequisites() {
    // Check that all required IEEE implementations are available
    
    // Verify IEEE 802.1AS-2021 implementation
    try {
        IEEE::_802_1::AS::_2021::TimeAwareSystem test_gptp;
        // If we can create it, the implementation is available
    } catch (...) {
        return false;
    }
    
    // Verify IEEE 1722.1-2021 implementation 
    // Check that required types are available
    IEEE::_1722_1::_2021::EntityID test_entity_id = 0;
    (void)test_entity_id; // Suppress unused variable warning
    
    // Verify Milan implementation
    try {
        AVnu::Milan::_1_2_2023::MilanPAADEntity test_milan(0x001B92FFFE000001, 0x001B92FFFE000010);
        // If we can create it, the implementation is available
    } catch (...) {
        return false;
    }
    
    // All prerequisites validated
    return true;
}

} // namespace Milan_IEEE
} // namespace Integration
} // namespace OpenAvnu
