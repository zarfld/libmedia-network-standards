#pragma once

/**
 * @file network_interface.h
 * @brief Hardware-agnostic network interface for all IEEE standards
 * @details Provides unified Ethernet packet I/O abstraction preventing vendor lock-in
 * 
 * This interface MUST be used by ALL IEEE standards implementations.
 * Direct hardware calls are FORBIDDEN in lib/Standards/ namespaces.
 * 
 * @note Part of Phase 00 Cross-Standard Architecture Foundation
 * @see 03-architecture/phase00-cross-standard-architecture-foundation.md
 */

#include <cstdint>
#include <cstddef>

namespace Common {
namespace interfaces {

/**
 * @brief Hardware-agnostic network interface for all IEEE standards
 * 
 * Provides unified abstraction for Ethernet packet I/O required by:
 * - IEEE 802.1Q (VLAN packet handling)
 * - IEEE 1588 (PTP packet transmission)
 * - IEEE 802.1AS (gPTP message handling)  
 * - IEEE 1722 (AVTP stream transmission)
 * - IEEE 1722.1 (AVDECC control messages)
 */
class NetworkInterface {
public:
    virtual ~NetworkInterface() = default;

    /**
     * @brief Send raw Ethernet packet
     * @param data Packet data including Ethernet header
     * @param length Packet length in bytes
     * @return 0 on success, negative error code on failure
     * 
     * Used by ALL IEEE standards for packet transmission.
     * Implementation handles hardware-specific packet I/O.
     */
    virtual int send_packet(const void* data, size_t length) = 0;

    /**
     * @brief Receive raw Ethernet packet  
     * @param buffer Buffer for received packet data
     * @param length Input: buffer size, Output: received packet length
     * @param timeout_ms Timeout in milliseconds (0 = non-blocking)
     * @return 0 on success, negative error code on failure/timeout
     * 
     * Used by ALL IEEE standards for packet reception.
     * Implementation handles hardware-specific packet reception.
     */
    virtual int receive_packet(void* buffer, size_t* length, uint32_t timeout_ms) = 0;

    /**
     * @brief Set network interface promiscuous mode
     * @param enabled true to enable promiscuous mode, false to disable
     * @return 0 on success, negative error code on failure
     * 
     * Required for IEEE 802.1AS gPTP message reception and
     * IEEE 1722.1 AVDECC discovery protocol.
     */
    virtual int set_promiscuous_mode(bool enabled) = 0;

    /**
     * @brief Get MAC address of network interface
     * @param mac Output buffer for 6-byte MAC address
     * @return 0 on success, negative error code on failure
     * 
     * Required by IEEE 1588 PTP for clock identity generation,
     * IEEE 802.1AS for port identity, and IEEE 1722.1 for entity ID.
     */
    virtual int get_mac_address(uint8_t mac[6]) = 0;

    /**
     * @brief Send VLAN-tagged Ethernet packet
     * @param data Packet data (without VLAN tag - added by implementation)
     * @param length Packet data length in bytes  
     * @param vlan_id VLAN ID (0-4095)
     * @param priority 802.1p priority (0-7)
     * @return 0 on success, negative error code on failure
     * 
     * Required by IEEE 802.1Q for QoS packet handling.
     * Used by IEEE 1588 PTP, IEEE 802.1AS gPTP, IEEE 1722 AVTP
     * for proper network QoS integration.
     */
    virtual int send_vlan_packet(const void* data, size_t length, 
                                uint16_t vlan_id, uint8_t priority) = 0;

    /**
     * @brief Get link status of network interface
     * @return 1 if link is up, 0 if link is down, negative on error
     * 
     * Required by IEEE 802.1AS gPTP for path delay measurement
     * and IEEE 1722 AVTP for stream establishment.
     */
    virtual int get_link_status() = 0;

    /**
     * @brief Get network interface statistics
     * @param tx_packets Output: transmitted packets count
     * @param rx_packets Output: received packets count  
     * @param tx_errors Output: transmission errors count
     * @param rx_errors Output: reception errors count
     * @return 0 on success, negative error code on failure
     * 
     * Used for network performance monitoring across all IEEE standards.
     */
    virtual int get_statistics(uint64_t* tx_packets, uint64_t* rx_packets,
                              uint64_t* tx_errors, uint64_t* rx_errors) = 0;
};

} // namespace interfaces
} // namespace Common