#pragma once

/**
 * @file transport_interface.h
 * @brief Cross-standard transport coordination interface  
 * @details Provides IEEE 1722 → IEEE 1722.1 transport coordination abstraction
 * 
 * This interface enables IEEE 1722.1 AVDECC to transport control messages
 * over IEEE 1722 AVTP streams without tight coupling between protocols.
 * 
 * @note Part of Phase 00 Cross-Standard Architecture Foundation
 * @see 03-architecture/phase00-cross-standard-architecture-foundation.md
 */

#include <cstdint>
#include <cstddef>
#include <functional>

namespace Common {
namespace interfaces {

/**
 * @brief Cross-standard transport coordination interface
 * 
 * Enables transport coordination between IEEE standards:
 * - IEEE 1722.1 AVDECC uses IEEE 1722 AVTP for message transport
 * - IEEE 1722 AVTP provides transport services to control protocols
 */
class TransportInterface {
public:
    virtual ~TransportInterface() = default;

    /**
     * @brief Send AVTP packet for AVDECC control message
     * @param avtp_data AVTP packet data including subtype and payload
     * @param length Packet length in bytes
     * @return 0 on success, negative error code on failure
     * 
     * Used by IEEE 1722.1 AVDECC protocols (AECP, ACMP, ADP)
     * to transport control messages over IEEE 1722 AVTP.
     */
    virtual int send_avtp_packet(const void* avtp_data, size_t length) = 0;

    /**
     * @brief Receive AVTP packet for AVDECC processing
     * @param buffer Buffer for received AVTP packet
     * @param length Input: buffer size, Output: received packet length  
     * @param timeout_ms Timeout in milliseconds (0 = non-blocking)
     * @return 0 on success, negative error code on failure/timeout
     * 
     * Used by IEEE 1722.1 AVDECC to receive control messages
     * transported over IEEE 1722 AVTP.
     */
    virtual int receive_avtp_packet(void* buffer, size_t* length, uint32_t timeout_ms) = 0;

    /**
     * @brief Register AVTP subtype handler for AVDECC protocols
     * @param subtype AVTP subtype (e.g., 0x7A for AECP, 0x7B for ACMP)
     * @param handler Function to call when packet of this subtype is received
     * @return 0 on success, negative error code on failure
     * 
     * Enables IEEE 1722 AVTP to demultiplex control messages
     * to appropriate IEEE 1722.1 AVDECC protocol handlers.
     */
    virtual int register_subtype_handler(uint8_t subtype, 
                                        std::function<void(const void*, size_t)> handler) = 0;

    /**
     * @brief Unregister AVTP subtype handler
     * @param subtype AVTP subtype to unregister
     * @return 0 on success, negative error code on failure
     */
    virtual int unregister_subtype_handler(uint8_t subtype) = 0;

    /**
     * @brief Get transport layer status and statistics  
     * @param packets_sent Output: number of packets transmitted
     * @param packets_received Output: number of packets received
     * @param transport_errors Output: transport layer errors
     * @return 0 on success, negative error code on failure
     * 
     * Used for transport layer monitoring and diagnostics
     * across IEEE 1722 and IEEE 1722.1 protocols.
     */
    virtual int get_transport_status(uint64_t* packets_sent,
                                    uint64_t* packets_received, 
                                    uint64_t* transport_errors) = 0;

    /**
     * @brief Set transport quality of service parameters
     * @param priority 802.1p priority (0-7) for AVTP packets
     * @param vlan_id VLAN ID for packet tagging
     * @return 0 on success, negative error code on failure
     * 
     * Enables IEEE 1722.1 AVDECC to configure QoS for control
     * message transport over IEEE 1722 AVTP.
     */
    virtual int set_transport_qos(uint8_t priority, uint16_t vlan_id) = 0;

    /**
     * @brief Enable/disable transport layer features
     * @param feature Feature identifier (implementation specific)
     * @param enabled true to enable feature, false to disable
     * @return 0 on success, negative error code on failure
     * 
     * Allows configuration of transport layer features like
     * packet validation, flow control, or error correction.
     */
    virtual int set_transport_feature(uint32_t feature, bool enabled) = 0;

    /**
     * @brief Get maximum transport unit size
     * @return Maximum packet size in bytes that can be transported
     * 
     * Used by IEEE 1722.1 AVDECC to determine if control messages
     * need fragmentation for transport over IEEE 1722 AVTP.
     */
    virtual size_t get_max_transport_unit() = 0;

    /**
     * @brief Validate transport layer connectivity
     * @param destination_address Target MAC address for connectivity test
     * @return true if transport is available to destination, false otherwise
     * 
     * Used by IEEE 1722.1 AVDECC to validate network connectivity
     * before attempting control message exchange.
     */
    virtual bool validate_transport_connectivity(const uint8_t destination_address[6]) = 0;

    /**
     * @brief Register for transport status change notifications
     * @param callback Function to call on transport status changes
     * @return 0 on success, negative error code on failure
     * 
     * Enables IEEE 1722.1 AVDECC to react to transport layer
     * events like link up/down or QoS changes.
     */
    virtual int register_transport_callback(std::function<void(uint32_t event)> callback) = 0;
};

} // namespace interfaces
} // namespace Common