#ifndef INDUSTRIAL_PROTOCOLS_H
#define INDUSTRIAL_PROTOCOLS_H

/**
 * @file industrial_protocols.h
 * @brief Industrial Protocol Extensions for IEEE 1722-2016
 * 
 * Complete implementation of CAN, LIN, MOST, FlexRay, and sensor data protocols
 * for automotive and industrial applications per IEEE 1722-2016 "tutti completo"
 * 
 * @author OpenAvnu IEEE 1722-2016 Compliance Team
 * @date July 2025
 * @version 4.0.0 - Phase 4 Critical Subtype Corrections + Industrial Protocols
 */

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <array>
#include <map>
#include <utility>

namespace avtp_protocol {
namespace industrial {

// =============================
// CAN (Controller Area Network) Support
// =============================
namespace can {

/**
 * @brief CAN Frame Format for AVTP Transport
 * Supports both Classic CAN (2.0A/2.0B) and CAN FD
 */
struct CANFrame {
    uint32_t can_id;                    // CAN identifier (11 or 29 bits)
    uint8_t frame_format;               // 0=Standard (11-bit), 1=Extended (29-bit)
    uint8_t frame_type;                 // 0=Data frame, 1=Remote frame
    uint8_t can_fd_format;              // 0=Classic CAN, 1=CAN FD
    uint8_t bit_rate_switch;            // CAN FD only: 0=No BRS, 1=BRS
    uint8_t error_state_indicator;      // CAN FD only: 0=Error active, 1=Error passive
    uint8_t data_length_code;           // DLC (0-8 for Classic, 0-15 for CAN FD)
    std::array<uint8_t, 64> data;       // Payload data (max 64 bytes for CAN FD)
    uint64_t timestamp_ns;              // Nanosecond timestamp for synchronization
    
    // Quality of Service fields
    uint8_t priority_class;             // Priority class for TSN scheduling
    uint8_t dropped_frame_count;        // Number of dropped frames due to congestion
    
    // Error detection
    uint16_t crc_calculated;            // Calculated CRC for frame integrity
    uint8_t error_flags;                // Error condition flags
    
    size_t get_data_length() const;     // Returns actual data length based on DLC
    bool is_valid_frame() const;        // Validates frame consistency
    std::string to_debug_string() const; // Human-readable debug output
};

/**
 * @brief CAN Bus Configuration for AVTP Networks
 */
struct CANBusConfig {
    uint32_t bus_id;                    // Unique bus identifier
    uint32_t nominal_bitrate;           // Nominal bit rate (bits/sec)
    uint32_t data_bitrate;              // Data bit rate for CAN FD (bits/sec)
    uint8_t sample_point_percent;       // Sample point percentage (60-87%)
    uint8_t sync_jump_width;            // Synchronization Jump Width
    bool enable_can_fd;                 // Enable CAN FD support
    bool enable_bit_rate_switching;     // Enable BRS for CAN FD
    
    // TSN Integration
    uint8_t tsn_priority;               // 802.1Q priority for CAN traffic
    uint32_t max_frame_rate_hz;         // Maximum frame transmission rate
    uint32_t reserved_bandwidth_kbps;   // Reserved bandwidth for this CAN bus
};

class CANProtocolHandler {
public:
    CANProtocolHandler(const CANBusConfig& config);
    virtual ~CANProtocolHandler() = default;
    
    // Frame processing
    bool encode_can_frame(const CANFrame& frame, std::vector<uint8_t>& avtp_payload);
    bool decode_can_frame(const std::vector<uint8_t>& avtp_payload, CANFrame& frame);
    
    // Bus management
    bool configure_bus(const CANBusConfig& config);
    CANBusConfig get_bus_config() const { return bus_config_; }
    
    // Statistics and monitoring
    uint64_t get_frames_transmitted() const { return frames_transmitted_; }
    uint64_t get_frames_received() const { return frames_received_; }
    uint64_t get_error_count() const { return error_count_; }
    
private:
    CANBusConfig bus_config_;
    uint64_t frames_transmitted_;
    uint64_t frames_received_;
    uint64_t error_count_;
    
    // Internal helper methods
    uint16_t calculate_crc15(const std::vector<uint8_t>& data) const;
    bool validate_dlc(uint8_t dlc, bool is_can_fd) const;
};

} // namespace can

// =============================
// LIN (Local Interconnect Network) Support
// =============================
namespace lin {

/**
 * @brief LIN Frame Format for AVTP Transport
 * Supports LIN 2.0/2.1/2.2A specifications
 */
struct LINFrame {
    uint8_t protected_id;               // Protected identifier (6-bit ID + 2 parity bits)
    uint8_t frame_id;                   // Frame identifier (0-63)
    uint8_t data_length;                // Data length (1-8 bytes)
    std::array<uint8_t, 8> data;        // Frame data
    uint8_t checksum;                   // Classic or enhanced checksum
    uint8_t checksum_type;              // 0=Classic, 1=Enhanced
    uint64_t timestamp_ns;              // Nanosecond timestamp
    
    // LIN-specific fields
    uint8_t break_delimiter_length;     // Break + delimiter length
    uint8_t lin_version;                // LIN specification version (2.0, 2.1, 2.2A)
    uint8_t node_address;               // Slave node address
    uint8_t diagnostic_class;           // Diagnostic frame classification
    
    bool is_diagnostic_frame() const;   // Check if frame is diagnostic
    uint8_t calculate_parity_bits() const; // Calculate parity for protected ID
    std::string to_debug_string() const;
};

/**
 * @brief LIN Bus Configuration
 */
struct LINBusConfig {
    uint32_t bus_id;                    // Unique bus identifier
    uint32_t baud_rate;                 // LIN bus baud rate (1200-20000 bps)
    uint8_t lin_version;                // LIN specification version
    uint8_t master_node_id;             // Master node identifier
    std::vector<uint8_t> slave_node_ids; // List of slave node identifiers
    
    // Schedule table configuration
    uint32_t schedule_table_length_ms;  // Schedule table length in milliseconds
    std::vector<uint8_t> scheduled_frames; // Frames in schedule table
    
    // TSN Integration
    uint8_t tsn_priority;               // 802.1Q priority for LIN traffic
    uint32_t reserved_bandwidth_kbps;   // Reserved bandwidth for this LIN bus
};

class LINProtocolHandler {
public:
    LINProtocolHandler(const LINBusConfig& config);
    virtual ~LINProtocolHandler() = default;
    
    // Frame processing
    bool encode_lin_frame(const LINFrame& frame, std::vector<uint8_t>& avtp_payload);
    bool decode_lin_frame(const std::vector<uint8_t>& avtp_payload, LINFrame& frame);
    
    // Schedule table management
    bool update_schedule_table(const std::vector<uint8_t>& frame_schedule);
    std::vector<uint8_t> get_current_schedule() const;
    
    // Diagnostic support
    bool send_diagnostic_request(uint8_t nad, const std::vector<uint8_t>& request);
    bool receive_diagnostic_response(uint8_t nad, std::vector<uint8_t>& response);
    
private:
    LINBusConfig bus_config_;
    std::vector<uint8_t> current_schedule_;
    
    uint8_t calculate_checksum(const LINFrame& frame) const;
    bool validate_protected_id(uint8_t protected_id) const;
};

} // namespace lin

// =============================
// MOST (Media Oriented Systems Transport) Support
// =============================
namespace most {

/**
 * @brief MOST Frame Format for AVTP Transport
 * Supports MOST25, MOST50, MOST150 specifications
 */
struct MOSTFrame {
    uint16_t source_address;            // Source node address
    uint16_t destination_address;       // Destination node address (0x03FF = broadcast)
    uint8_t function_block_id;          // Function block identifier
    uint8_t instance_id;                // Function block instance
    uint16_t function_id;               // Function identifier
    uint8_t operation_type;             // Operation type (0-15)
    std::vector<uint8_t> data;          // Frame payload data
    uint64_t timestamp_ns;              // Nanosecond timestamp
    
    // MOST-specific fields
    uint8_t most_version;               // MOST specification version (25/50/150)
    uint8_t retry_count;                // Number of retries attempted
    uint8_t acknowledge_status;         // Acknowledge status from receiver
    uint16_t message_length;            // Total message length
    
    // Message fragmentation support
    bool is_fragmented;                 // True if message is fragmented
    uint8_t fragment_number;            // Fragment sequence number
    uint8_t total_fragments;            // Total number of fragments
    
    std::string to_debug_string() const;
};

/**
 * @brief MOST Network Configuration
 */
struct MOSTNetworkConfig {
    uint32_t network_id;                // Unique network identifier
    uint8_t most_version;               // MOST specification version
    uint32_t frame_rate;                // Frame rate (44.1 kHz / 48 kHz)
    uint16_t node_address;              // This node's MOST address
    uint16_t max_node_address;          // Maximum node address in network
    
    // Bandwidth allocation
    uint32_t control_bandwidth_bytes;   // Control bandwidth allocation
    uint32_t async_bandwidth_bytes;     // Asynchronous bandwidth allocation
    uint32_t sync_bandwidth_bytes;      // Synchronous bandwidth allocation
    
    // TSN Integration
    uint8_t tsn_priority;               // 802.1Q priority for MOST traffic
    uint32_t reserved_bandwidth_kbps;   // Reserved bandwidth for this MOST network
};

class MOSTProtocolHandler {
public:
    MOSTProtocolHandler(const MOSTNetworkConfig& config);
    virtual ~MOSTProtocolHandler() = default;
    
    // Frame processing
    bool encode_most_frame(const MOSTFrame& frame, std::vector<uint8_t>& avtp_payload);
    bool decode_most_frame(const std::vector<uint8_t>& avtp_payload, MOSTFrame& frame);
    
    // Message fragmentation
    bool fragment_message(const std::vector<uint8_t>& message, 
                         std::vector<MOSTFrame>& fragments);
    bool reassemble_message(const std::vector<MOSTFrame>& fragments, 
                           std::vector<uint8_t>& message);
    
    // Network management
    bool register_function_block(uint8_t function_block_id, uint8_t instance_id);
    bool unregister_function_block(uint8_t function_block_id, uint8_t instance_id);
    
private:
    MOSTNetworkConfig network_config_;
    std::map<uint16_t, std::vector<MOSTFrame>> pending_fragments_;
    
    bool validate_address(uint16_t address) const;
    uint16_t calculate_frame_crc(const MOSTFrame& frame) const;
};

} // namespace most

// =============================
// FlexRay Support
// =============================
namespace flexray {

/**
 * @brief FlexRay Frame Format for AVTP Transport
 * Supports FlexRay Protocol Specification v3.0.1
 */
struct FlexRayFrame {
    // Header fields
    uint8_t reserved_bit;               // Reserved bit (always 0)
    bool payload_preamble_indicator;    // Payload preamble indicator
    bool null_frame_indicator;          // Null frame indicator
    bool sync_frame_indicator;          // Sync frame indicator
    bool startup_frame_indicator;       // Startup frame indicator
    uint16_t frame_id;                  // Frame identifier (1-2047)
    uint8_t payload_length;             // Payload length in 16-bit words (0-127)
    uint8_t header_crc;                 // Header CRC (11 bits)
    uint8_t cycle_count;                // Cycle count (0-63)
    
    // Payload
    std::array<uint8_t, 254> data;      // Frame payload (max 254 bytes)
    uint32_t frame_crc;                 // Frame CRC (24 bits)
    
    // FlexRay-specific timing
    uint64_t timestamp_ns;              // Nanosecond timestamp
    uint8_t slot_id;                    // Slot identifier where frame was transmitted
    uint8_t cycle_number;               // Communication cycle number
    
    // Status information
    bool transmission_across_boundary;  // Frame transmitted across slot boundary
    bool frame_received_on_channel_a;   // Frame received on channel A
    bool frame_received_on_channel_b;   // Frame received on channel B
    uint8_t slot_status;                // Slot status information
    
    size_t get_payload_length_bytes() const; // Returns payload length in bytes
    bool is_static_segment() const;         // Check if frame belongs to static segment
    std::string to_debug_string() const;
};

/**
 * @brief FlexRay Cluster Configuration
 */
struct FlexRayClusterConfig {
    uint32_t cluster_id;                // Unique cluster identifier
    
    // Timing configuration
    uint32_t gdCycle;                   // Communication cycle length (µs)
    uint32_t gdStaticSlot;              // Static slot length (µs)
    uint32_t gdActionPointOffset;      // Action point offset (µs)
    uint32_t gdSymbolWindow;            // Symbol window length (µs)
    uint32_t gdNIT;                     // Network idle time (µs)
    
    // Frame configuration
    uint16_t gNumberOfStaticSlots;      // Number of static slots
    uint16_t gNumberOfMinislots;        // Number of minislots
    uint8_t gPayloadLengthStatic;       // Payload length for static frames
    
    // Channel configuration
    bool gdChannels;                    // Number of channels (1 or 2)
    uint32_t gdBitRate;                 // Bit rate (2.5 or 5 or 10 Mbps)
    
    // TSN Integration
    uint8_t tsn_priority;               // 802.1Q priority for FlexRay traffic
    uint32_t reserved_bandwidth_kbps;   // Reserved bandwidth for this FlexRay cluster
};

class FlexRayProtocolHandler {
public:
    FlexRayProtocolHandler(const FlexRayClusterConfig& config);
    virtual ~FlexRayProtocolHandler() = default;
    
    // Frame processing
    bool encode_flexray_frame(const FlexRayFrame& frame, std::vector<uint8_t>& avtp_payload);
    bool decode_flexray_frame(const std::vector<uint8_t>& avtp_payload, FlexRayFrame& frame);
    
    // Schedule management
    bool configure_static_frame(uint16_t slot_id, uint16_t frame_id, uint8_t payload_length);
    bool configure_dynamic_frame(uint16_t frame_id, uint8_t payload_length);
    
    // Synchronization support
    bool is_sync_node() const { return sync_node_; }
    void set_sync_node(bool is_sync) { sync_node_ = is_sync; }
    
private:
    FlexRayClusterConfig cluster_config_;
    bool sync_node_;
    std::map<uint16_t, FlexRayFrame> static_schedule_;
    
    uint8_t calculate_header_crc(const FlexRayFrame& frame) const;
    uint32_t calculate_frame_crc(const FlexRayFrame& frame) const;
    bool validate_frame_timing(const FlexRayFrame& frame) const;
};

} // namespace flexray

// =============================
// Sensor Data Protocol Support
// =============================
namespace sensor {

/**
 * @brief Generic Sensor Data Frame for AVTP Transport
 * Supports various sensor types: temperature, pressure, acceleration, etc.
 */
struct SensorDataFrame {
    uint32_t sensor_id;                 // Unique sensor identifier
    uint8_t sensor_type;                // Sensor type (temperature, pressure, etc.)
    uint8_t data_format;                // Data format (raw, calibrated, etc.)
    uint64_t timestamp_ns;              // Nanosecond timestamp
    
    // Sensor value(s)
    std::vector<double> values;         // Sensor readings (may be multi-dimensional)
    std::vector<uint8_t> raw_data;      // Raw sensor data (if applicable)
    
    // Quality indicators
    uint8_t signal_quality;             // Signal quality indicator (0-255)
    uint8_t calibration_status;         // Calibration status
    bool data_valid;                    // Data validity flag
    uint32_t sequence_number;           // Sequence number for data integrity
    
    // Sensor metadata
    std::string sensor_name;            // Human-readable sensor name
    std::string unit_of_measurement;    // Unit of measurement (e.g., "°C", "bar", "m/s²")
    double measurement_range_min;       // Minimum measurement range
    double measurement_range_max;       // Maximum measurement range
    double measurement_accuracy;        // Measurement accuracy
    
    std::string to_debug_string() const;
};

/**
 * @brief Sensor Network Configuration
 */
struct SensorNetworkConfig {
    uint32_t network_id;                // Unique sensor network identifier
    uint32_t sampling_rate_hz;          // Data sampling rate
    uint8_t data_compression;           // Data compression type (0=none, 1=lossless, 2=lossy)
    
    // Data aggregation
    bool enable_data_aggregation;       // Enable sensor data aggregation
    uint32_t aggregation_window_ms;     // Aggregation time window
    uint8_t aggregation_function;       // Aggregation function (average, max, min, etc.)
    
    // TSN Integration
    uint8_t tsn_priority;               // 802.1Q priority for sensor traffic
    uint32_t reserved_bandwidth_kbps;   // Reserved bandwidth for sensor network
    uint32_t max_latency_us;            // Maximum allowed latency for sensor data
};

class SensorProtocolHandler {
public:
    SensorProtocolHandler(const SensorNetworkConfig& config);
    virtual ~SensorProtocolHandler() = default;
    
    // Frame processing
    bool encode_sensor_frame(const SensorDataFrame& frame, std::vector<uint8_t>& avtp_payload);
    bool decode_sensor_frame(const std::vector<uint8_t>& avtp_payload, SensorDataFrame& frame);
    
    // Data compression/decompression
    bool compress_sensor_data(const std::vector<double>& values, std::vector<uint8_t>& compressed);
    bool decompress_sensor_data(const std::vector<uint8_t>& compressed, std::vector<double>& values);
    
    // Data aggregation
    bool add_sensor_reading(const SensorDataFrame& frame);
    bool get_aggregated_data(uint32_t sensor_id, SensorDataFrame& aggregated_frame);
    
    // Calibration support
    bool calibrate_sensor(uint32_t sensor_id, const std::vector<double>& calibration_coefficients);
    bool apply_calibration(SensorDataFrame& frame) const;
    
private:
    SensorNetworkConfig network_config_;
    std::map<uint32_t, std::vector<SensorDataFrame>> sensor_buffers_;
    std::map<uint32_t, std::vector<double>> calibration_data_;
    
    bool validate_sensor_data(const SensorDataFrame& frame) const;
    double apply_calibration_formula(double raw_value, const std::vector<double>& coefficients) const;
};

} // namespace sensor

// =============================
// Unified Industrial Protocol Manager
// =============================

/**
 * @brief Unified manager for all industrial protocols
 * Provides a single interface for managing CAN, LIN, MOST, FlexRay, and sensor protocols
 */
class IndustrialProtocolManager {
public:
    IndustrialProtocolManager();
    virtual ~IndustrialProtocolManager();
    
    // Protocol registration
    bool register_can_handler(uint32_t bus_id, std::shared_ptr<can::CANProtocolHandler> handler);
    bool register_lin_handler(uint32_t bus_id, std::shared_ptr<lin::LINProtocolHandler> handler);
    bool register_most_handler(uint32_t network_id, std::shared_ptr<most::MOSTProtocolHandler> handler);
    bool register_flexray_handler(uint32_t cluster_id, std::shared_ptr<flexray::FlexRayProtocolHandler> handler);
    bool register_sensor_handler(uint32_t network_id, std::shared_ptr<sensor::SensorProtocolHandler> handler);
    
    // Protocol routing
    bool route_avtp_to_protocol(uint8_t subtype, const std::vector<uint8_t>& avtp_payload);
    bool route_protocol_to_avtp(uint8_t protocol_type, uint32_t id, 
                               const std::vector<uint8_t>& protocol_data, 
                               std::vector<uint8_t>& avtp_payload);
    
    // Statistics and monitoring
    struct ProtocolStatistics {
        uint64_t frames_transmitted;
        uint64_t frames_received;
        uint64_t errors_detected;
        uint64_t bytes_transmitted;
        uint64_t bytes_received;
        double average_latency_us;
        double packet_loss_percentage;
    };
    
    ProtocolStatistics get_statistics(uint8_t protocol_type, uint32_t id) const;
    std::vector<uint8_t> get_active_protocol_types() const;
    
private:
    std::map<uint32_t, std::shared_ptr<can::CANProtocolHandler>> can_handlers_;
    std::map<uint32_t, std::shared_ptr<lin::LINProtocolHandler>> lin_handlers_;
    std::map<uint32_t, std::shared_ptr<most::MOSTProtocolHandler>> most_handlers_;
    std::map<uint32_t, std::shared_ptr<flexray::FlexRayProtocolHandler>> flexray_handlers_;
    std::map<uint32_t, std::shared_ptr<sensor::SensorProtocolHandler>> sensor_handlers_;
    
    std::map<std::pair<uint8_t, uint32_t>, ProtocolStatistics> statistics_;
    
    void update_statistics(uint8_t protocol_type, uint32_t id, bool transmitted, 
                          size_t bytes, double latency_us, bool error);
};

} // namespace industrial
} // namespace avtp_protocol

#endif // INDUSTRIAL_PROTOCOLS_H
