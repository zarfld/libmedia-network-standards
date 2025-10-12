/**
 * @file ieee_1588_2019_network_interface.h
 * @brief IEEE 1588-2019 specific network interface extensions
 * @namespace IEEE::_1588::_2019::interfaces
 * 
 * Extends Common::interfaces::NetworkInterface with IEEE 1588-2019 specific
 * hardware timestamping capabilities required for precision time protocol.
 * 
 * @see IEEE 1588-2019, Section 13 "Transport protocols"
 * @see ADR-003: IEEE 1588-2019 Architecture Implementation Strategy
 */

#ifndef IEEE_1588_2019_NETWORK_INTERFACE_H
#define IEEE_1588_2019_NETWORK_INTERFACE_H

#include "../../../Common/interfaces/network_interface.h"
#include <cstdint>

namespace IEEE {
namespace _1588 {
namespace _2019 {
namespace interfaces {

/**
 * @brief IEEE 1588-2019 packet metadata with hardware timestamps
 */
struct PTPPacketMetadata {
    uint64_t tx_timestamp_ns;   ///< Hardware TX timestamp (nanoseconds)
    uint64_t rx_timestamp_ns;   ///< Hardware RX timestamp (nanoseconds)
    uint16_t sequence_id;       ///< PTP message sequence ID
    uint8_t message_type;       ///< PTP message type (Sync, Announce, etc.)
    uint8_t reserved;           ///< Reserved for alignment
};

/**
 * @brief IEEE 1588-2019 specific network interface with hardware timestamping
 * 
 * Extends Common network interface with precision timestamping capabilities
 * required for IEEE 1588-2019 PTP synchronization accuracy.
 */
class IEEE1588NetworkInterface : public Common::interfaces::NetworkInterface {
public:
    /**
     * @brief Send PTP packet with hardware timestamp capture
     * 
     * Sends IEEE 1588-2019 PTP packet and captures precise hardware
     * transmission timestamp. Required for Sync message transmission
     * accuracy per IEEE specification.
     *
     * @param ptp_packet PTP packet data
     * @param length Packet length in bytes
     * @param metadata Output metadata with TX timestamp
     * @return 0 on success, negative error code on failure
     * 
     * @see IEEE 1588-2019, Section 11.4 "Sync message"
     */
    virtual int send_ptp_packet(const void* ptp_packet,
                               size_t length, 
                               PTPPacketMetadata* metadata) = 0;

    /**
     * @brief Receive PTP packet with hardware timestamp
     * 
     * Receives IEEE 1588-2019 PTP packet and captures precise hardware
     * reception timestamp. Required for accurate delay measurement.
     *
     * @param buffer Buffer for received PTP packet
     * @param buffer_size Size of receive buffer
     * @param received_length Actual received packet length
     * @param metadata Output metadata with RX timestamp
     * @return 0 on success, negative error code on failure
     * 
     * @see IEEE 1588-2019, Section 11.3 "Delay_Req message"
     */
    virtual int receive_ptp_packet(void* buffer,
                                  size_t buffer_size,
                                  size_t* received_length,
                                  PTPPacketMetadata* metadata) = 0;

    /**
     * @brief Get hardware timestamp capabilities
     * 
     * Returns precision timestamping capabilities of the network hardware.
     * IEEE 1588-2019 performance depends on timestamp accuracy.
     *
     * @return True if hardware timestamps supported, false for software only
     */
    virtual bool supports_hardware_timestamping() const = 0;

    /**
     * @brief Get timestamp resolution in nanoseconds
     * 
     * Returns the resolution of hardware timestamps. IEEE 1588-2019
     * requires sub-microsecond accuracy for Grand Master class clocks.
     *
     * @return Timestamp resolution in nanoseconds (e.g., 8 for 8ns resolution)
     */
    virtual uint32_t get_timestamp_resolution_ns() const = 0;

    /**
     * @brief Configure PTP multicast filtering
     * 
     * Configures hardware filtering to receive IEEE 1588-2019 multicast
     * messages (224.0.1.129 for IPv4, 01-1B-19-00-00-00 for Ethernet).
     *
     * @param enable_ipv4_multicast Enable IPv4 224.0.1.129 filtering
     * @param enable_ethernet_multicast Enable Ethernet 01-1B-19-00-00-00 filtering
     * @return 0 on success, negative error code on failure
     * 
     * @see IEEE 1588-2019, Section 13.2.2 "Ethernet multicast addresses"
     */
    virtual int configure_ptp_multicast_filtering(bool enable_ipv4_multicast,
                                                 bool enable_ethernet_multicast) = 0;

    /**
     * @brief Set PTP transport type for filtering
     * 
     * Configures packet filtering for specific IEEE 1588-2019 transport:
     * - Ethernet Layer 2 (ethertype 0x88F7)
     * - UDP/IPv4 (ports 319/320)
     *
     * @param transport_type 0=Ethernet L2, 1=UDP/IPv4
     * @return 0 on success, negative error code on failure
     * 
     * @see IEEE 1588-2019, Section 13 "Transport protocols"
     */
    virtual int set_ptp_transport_type(uint8_t transport_type) = 0;
};

} // namespace interfaces  
} // namespace _2019
} // namespace _1588
} // namespace IEEE

#endif // IEEE_1588_2019_NETWORK_INTERFACE_H