/**
 * @file ieee_1722_1_2021_acmp_implementation.cpp
 * @brief IEEE 1722.1-2021 ACMP (AVDECC Connection Management Protocol) Implementation
 * 
 * PRIORITÄT: IEEE 1722.1-2021 - ACMP nach AECP Implementation
 * Connection Management Protocol für Stream-Verbindungen
 */

#include "ieee_1722_1_2021_base.h"
#include <memory>
#include <mutex>
#include <atomic>
#include <cstring>

namespace IEEE {
namespace _1722_1 {
namespace _2021 {
namespace ACMP {

// ============================================================================
// ACMP Protocol Definitions - IEEE 1722.1-2021 Section 8
// ============================================================================

/**
 * @brief ACMP Message Types (IEEE 1722.1-2021 Table 8.1)
 */
enum class ACMPMessageType : uint8_t {
    CONNECT_TX_COMMAND          = 0x00,
    CONNECT_TX_RESPONSE         = 0x01,
    DISCONNECT_TX_COMMAND       = 0x02,
    DISCONNECT_TX_RESPONSE      = 0x03,
    GET_TX_STATE_COMMAND        = 0x04,
    GET_TX_STATE_RESPONSE       = 0x05,
    CONNECT_RX_COMMAND          = 0x06,
    CONNECT_RX_RESPONSE         = 0x07,
    DISCONNECT_RX_COMMAND       = 0x08,
    DISCONNECT_RX_RESPONSE      = 0x09,
    GET_RX_STATE_COMMAND        = 0x0A,
    GET_RX_STATE_RESPONSE       = 0x0B,
    GET_TX_CONNECTION_COMMAND   = 0x0C,
    GET_TX_CONNECTION_RESPONSE  = 0x0D
};

/**
 * @brief ACMP Status Codes (IEEE 1722.1-2021 Table 8.2)
 */
enum class ACMPStatusCode : uint8_t {
    SUCCESS                     = 0x00,
    LISTENER_UNKNOWN_ID         = 0x01,
    TALKER_UNKNOWN_ID           = 0x02,
    TALKER_DEST_MAC_FAIL        = 0x03,
    TALKER_NO_STREAM_INDEX      = 0x04,
    TALKER_NO_BANDWIDTH         = 0x05,
    TALKER_EXCLUSIVE            = 0x06,
    LISTENER_TALKER_TIMEOUT     = 0x07,
    LISTENER_EXCLUSIVE          = 0x08,
    STATE_UNAVAILABLE           = 0x09,
    NOT_CONNECTED               = 0x0A,
    NO_SUCH_CONNECTION          = 0x0B,
    COULD_NOT_SEND_MESSAGE      = 0x0C,
    TALKER_MISBEHAVING          = 0x0D,
    LISTENER_MISBEHAVING        = 0x0E,
    RESERVED                    = 0x0F,
    CONTROLLER_NOT_AUTHORIZED   = 0x10,
    INCOMPATIBLE_REQUEST        = 0x11,
    NOT_SUPPORTED               = 0x1F
};

/**
 * @brief ACMP PDU Format (IEEE 1722.1-2021 Figure 8.1)
 */
struct ACMPPDUFormat {
    // AVTPDU Common header (12 bytes)
    uint8_t subtype;                    // 0xFC for ACMP
    uint8_t sv_version_msg_type;        // Stream valid, version, message type
    uint8_t status_control_data_length_h; // Status, upper 3 bits of control_data_length
    uint8_t control_data_length_l;      // Lower 8 bits of control_data_length
    uint64_t stream_id;                 // Stream ID (big endian)
    
    // ACMP specific fields
    uint64_t controller_entity_id;      // Controller entity ID (big endian)
    uint64_t talker_entity_id;          // Talker entity ID (big endian)
    uint64_t listener_entity_id;        // Listener entity ID (big endian)
    uint16_t talker_unique_id;          // Talker unique ID (big endian)
    uint16_t listener_unique_id;        // Listener unique ID (big endian)
    uint8_t dest_mac[6];                // Destination MAC address
    uint16_t connection_count;          // Connection count (big endian)
    uint16_t sequence_id;               // Sequence ID (big endian)
    uint16_t flags;                     // Connection flags (big endian)
    uint16_t stream_vlan_id;            // Stream VLAN ID (big endian)
    uint16_t reserved;                  // Reserved, set to 0
    
    // Helper functions for byte order conversion
    uint64_t getStreamId() const { return be64toh(stream_id); }
    uint64_t getControllerEntityId() const { return be64toh(controller_entity_id); }
    uint64_t getTalkerEntityId() const { return be64toh(talker_entity_id); }
    uint64_t getListenerEntityId() const { return be64toh(listener_entity_id); }
    uint16_t getTalkerUniqueId() const { return ntohs(talker_unique_id); }
    uint16_t getListenerUniqueId() const { return ntohs(listener_unique_id); }
    uint16_t getConnectionCount() const { return ntohs(connection_count); }
    uint16_t getSequenceId() const { return ntohs(sequence_id); }
    uint16_t getFlags() const { return ntohs(flags); }
    uint16_t getStreamVlanId() const { return ntohs(stream_vlan_id); }
    
    void setStreamId(uint64_t id) { stream_id = htobe64(id); }
    void setControllerEntityId(uint64_t id) { controller_entity_id = htobe64(id); }
    void setTalkerEntityId(uint64_t id) { talker_entity_id = htobe64(id); }
    void setListenerEntityId(uint64_t id) { listener_entity_id = htobe64(id); }
    void setTalkerUniqueId(uint16_t id) { talker_unique_id = htons(id); }
    void setListenerUniqueId(uint16_t id) { listener_unique_id = htons(id); }
    void setConnectionCount(uint16_t count) { connection_count = htons(count); }
    void setSequenceId(uint16_t id) { sequence_id = htons(id); }
    void setFlags(uint16_t f) { flags = htons(f); }
    void setStreamVlanId(uint16_t vlan) { stream_vlan_id = htons(vlan); }
    
    // Get status from the message
    ACMPStatusCode getStatus() const {
        return static_cast<ACMPStatusCode>((status_control_data_length_h >> 3) & 0x1F);
    }
    
    // Set status in the message
    void setStatus(ACMPStatusCode status) {
        uint8_t status_bits = static_cast<uint8_t>(status) & 0x1F;
        status_control_data_length_h = (status_control_data_length_h & 0x07) | (status_bits << 3);
    }
    
    // Get message type
    ACMPMessageType getMessageType() const {
        return static_cast<ACMPMessageType>(sv_version_msg_type & 0x0F);
    }
    
    // Set message type
    void setMessageType(ACMPMessageType msgType) {
        sv_version_msg_type = (sv_version_msg_type & 0xF0) | (static_cast<uint8_t>(msgType) & 0x0F);
    }
} __attribute__((packed));

// ============================================================================
// ACMP Connection State Management
// ============================================================================

/**
 * @brief Stream Connection Information
 */
struct StreamConnection {
    uint64_t streamId;
    uint64_t controllerEntityId;
    uint64_t talkerEntityId;
    uint64_t listenerEntityId;
    uint16_t talkerUniqueId;
    uint16_t listenerUniqueId;
    uint8_t destMac[6];
    uint16_t connectionCount;
    uint16_t flags;
    uint16_t streamVlanId;
    bool active;
    uint64_t establishedTime;  // milliseconds since epoch
    
    StreamConnection() : streamId(0), controllerEntityId(0), talkerEntityId(0), 
                        listenerEntityId(0), talkerUniqueId(0), listenerUniqueId(0),
                        connectionCount(0), flags(0), streamVlanId(0), active(false),
                        establishedTime(0) {
        memset(destMac, 0, sizeof(destMac));
    }
};

/**
 * @brief ACMP Protocol Handler - Windows Compatible
 */
class WindowsACMPProtocolHandler {
private:
    static const size_t MAX_CONNECTIONS = 256;
    StreamConnection connections[MAX_CONNECTIONS];
    size_t connectionCount;
    
    mutable std::mutex connectionMutex;
    std::atomic<uint16_t> nextSequenceId;
    
    // Helper: Find connection index
    int findConnectionIndex(uint64_t streamId) {
        for (size_t i = 0; i < connectionCount; ++i) {
            if (connections[i].streamId == streamId && connections[i].active) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    
    // Helper: Find free connection slot
    int findFreeConnectionSlot() {
        for (size_t i = 0; i < MAX_CONNECTIONS; ++i) {
            if (!connections[i].active) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    
    // Get current time in milliseconds - Windows compatible
    uint64_t getCurrentTimeMs() {
        #ifdef _WIN32
            return static_cast<uint64_t>(GetTickCount64());
        #else
            auto now = std::chrono::steady_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        #endif
    }

public:
    WindowsACMPProtocolHandler() : connectionCount(0), nextSequenceId(1) {}
    
    // ========================================================================
    // IEEE 1722.1-2021 ACMP Core Implementation
    // ========================================================================
    
    /**
     * @brief Process incoming ACMP command
     */
    bool processCommand(const ACMPPDUFormat& command, ACMPPDUFormat& response) {
        std::lock_guard<std::mutex> lock(connectionMutex);
        
        // Initialize response
        response = command;  // Copy common fields
        response.setSequenceId(command.getSequenceId());
        
        // Process based on message type
        ACMPMessageType msgType = command.getMessageType();
        
        switch (msgType) {
            case ACMPMessageType::CONNECT_TX_COMMAND:
                response.setMessageType(ACMPMessageType::CONNECT_TX_RESPONSE);
                return processConnectTx(command, response);
                
            case ACMPMessageType::DISCONNECT_TX_COMMAND:
                response.setMessageType(ACMPMessageType::DISCONNECT_TX_RESPONSE);
                return processDisconnectTx(command, response);
                
            case ACMPMessageType::GET_TX_STATE_COMMAND:
                response.setMessageType(ACMPMessageType::GET_TX_STATE_RESPONSE);
                return processGetTxState(command, response);
                
            case ACMPMessageType::CONNECT_RX_COMMAND:
                response.setMessageType(ACMPMessageType::CONNECT_RX_RESPONSE);
                return processConnectRx(command, response);
                
            case ACMPMessageType::DISCONNECT_RX_COMMAND:
                response.setMessageType(ACMPMessageType::DISCONNECT_RX_RESPONSE);
                return processDisconnectRx(command, response);
                
            case ACMPMessageType::GET_RX_STATE_COMMAND:
                response.setMessageType(ACMPMessageType::GET_RX_STATE_RESPONSE);
                return processGetRxState(command, response);
                
            default:
                response.setStatus(ACMPStatusCode::NOT_SUPPORTED);
                return true;
        }
    }
    
    /**
     * @brief Get connection information
     */
    bool getConnectionInfo(uint64_t streamId, StreamConnection& connectionInfo) {
        std::lock_guard<std::mutex> lock(connectionMutex);
        
        int idx = findConnectionIndex(streamId);
        if (idx >= 0) {
            connectionInfo = connections[idx];
            return true;
        }
        return false;
    }
    
    /**
     * @brief Get all active connections
     */
    size_t getActiveConnections(StreamConnection* connectionList, size_t maxConnections) {
        std::lock_guard<std::mutex> lock(connectionMutex);
        
        size_t activeCount = 0;
        for (size_t i = 0; i < connectionCount && activeCount < maxConnections; ++i) {
            if (connections[i].active) {
                connectionList[activeCount++] = connections[i];
            }
        }
        return activeCount;
    }
    
private:
    // ========================================================================
    // Command Processors - IEEE 1722.1-2021 Specific
    // ========================================================================
    
    bool processConnectTx(const ACMPPDUFormat& command, ACMPPDUFormat& response) {
        uint64_t streamId = command.getStreamId();
        
        // Check if connection already exists
        int existingIdx = findConnectionIndex(streamId);
        if (existingIdx >= 0) {
            // Update existing connection
            auto& conn = connections[existingIdx];
            conn.controllerEntityId = command.getControllerEntityId();
            conn.flags = command.getFlags();
            response.setStatus(ACMPStatusCode::SUCCESS);
            return true;
        }
        
        // Create new connection
        int newIdx = findFreeConnectionSlot();
        if (newIdx < 0) {
            response.setStatus(ACMPStatusCode::TALKER_NO_BANDWIDTH);
            return true;
        }
        
        auto& conn = connections[newIdx];
        conn.streamId = streamId;
        conn.controllerEntityId = command.getControllerEntityId();
        conn.talkerEntityId = command.getTalkerEntityId();
        conn.listenerEntityId = command.getListenerEntityId();
        conn.talkerUniqueId = command.getTalkerUniqueId();
        conn.listenerUniqueId = command.getListenerUniqueId();
        memcpy(conn.destMac, command.dest_mac, 6);
        conn.connectionCount = command.getConnectionCount();
        conn.flags = command.getFlags();
        conn.streamVlanId = command.getStreamVlanId();
        conn.active = true;
        conn.establishedTime = getCurrentTimeMs();
        
        if (newIdx >= static_cast<int>(connectionCount)) {
            connectionCount = newIdx + 1;
        }
        
        response.setStatus(ACMPStatusCode::SUCCESS);
        return true;
    }
    
    bool processDisconnectTx(const ACMPPDUFormat& command, ACMPPDUFormat& response) {
        uint64_t streamId = command.getStreamId();
        
        int idx = findConnectionIndex(streamId);
        if (idx >= 0) {
            connections[idx].active = false;
            response.setStatus(ACMPStatusCode::SUCCESS);
        } else {
            response.setStatus(ACMPStatusCode::NOT_CONNECTED);
        }
        
        return true;
    }
    
    bool processGetTxState(const ACMPPDUFormat& command, ACMPPDUFormat& response) {
        uint64_t streamId = command.getStreamId();
        
        int idx = findConnectionIndex(streamId);
        if (idx >= 0) {
            const auto& conn = connections[idx];
            response.setStreamId(conn.streamId);
            response.setTalkerEntityId(conn.talkerEntityId);
            response.setListenerEntityId(conn.listenerEntityId);
            response.setTalkerUniqueId(conn.talkerUniqueId);
            response.setListenerUniqueId(conn.listenerUniqueId);
            memcpy(response.dest_mac, conn.destMac, 6);
            response.setConnectionCount(conn.connectionCount);
            response.setFlags(conn.flags);
            response.setStreamVlanId(conn.streamVlanId);
            response.setStatus(ACMPStatusCode::SUCCESS);
        } else {
            response.setStatus(ACMPStatusCode::NO_SUCH_CONNECTION);
        }
        
        return true;
    }
    
    bool processConnectRx(const ACMPPDUFormat& command, ACMPPDUFormat& response) {
        // Similar to ConnectTx but from listener perspective
        return processConnectTx(command, response);
    }
    
    bool processDisconnectRx(const ACMPPDUFormat& command, ACMPPDUFormat& response) {
        // Similar to DisconnectTx but from listener perspective
        return processDisconnectTx(command, response);
    }
    
    bool processGetRxState(const ACMPPDUFormat& command, ACMPPDUFormat& response) {
        // Similar to GetTxState but from listener perspective
        return processGetTxState(command, response);
    }
};

// ============================================================================
// Global Instance Management
// ============================================================================

static std::unique_ptr<WindowsACMPProtocolHandler> globalACMPHandler;
static std::mutex globalACMPMutex;

/**
 * @brief Get global ACMP protocol handler instance
 */
WindowsACMPProtocolHandler* getWindowsACMPHandler() {
    std::lock_guard<std::mutex> lock(globalACMPMutex);
    if (!globalACMPHandler) {
        globalACMPHandler = std::make_unique<WindowsACMPProtocolHandler>();
    }
    return globalACMPHandler.get();
}

} // namespace ACMP
} // namespace _2021
} // namespace _1722_1  
} // namespace IEEE

// ============================================================================
// C Interface for Integration
// ============================================================================

extern "C" {
    
/**
 * @brief C interface for ACMP command processing
 */
int acmp_windows_process_command(const void* command_data, size_t command_size,
                                void* response_data, size_t* response_size) {
    if (!command_data || !response_data || !response_size) {
        return -1;
    }
    
    if (command_size < sizeof(IEEE::_1722_1::_2021::ACMP::ACMPPDUFormat)) {
        return -2;
    }
    
    auto* handler = IEEE::_1722_1::_2021::ACMP::getWindowsACMPHandler();
    if (!handler) {
        return -3;
    }
    
    const auto* command = static_cast<const IEEE::_1722_1::_2021::ACMP::ACMPPDUFormat*>(command_data);
    IEEE::_1722_1::_2021::ACMP::ACMPPDUFormat response;
    
    if (handler->processCommand(*command, response)) {
        size_t responseSize = sizeof(response);
        if (*response_size < responseSize) {
            *response_size = responseSize;
            return -4;
        }
        
        memcpy(response_data, &response, responseSize);
        *response_size = responseSize;
        return 0;
    }
    
    return -5;
}

} // extern "C"
