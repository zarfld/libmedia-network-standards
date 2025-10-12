#pragma once

/**
 * @file standards_coordinator.h
 * @brief Central coordinator for cross-standard IEEE protocol communication
 * @details Manages inter-standard message passing and lifecycle coordination
 * 
 * This service enables IEEE standards to communicate without direct coupling.
 * Provides unified coordination for timing, transport, and management.
 * 
 * @note Part of Phase 00 Cross-Standard Architecture Foundation  
 * @see 03-architecture/phase00-cross-standard-architecture-foundation.md
 */

#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace Common {
namespace integration {

/**
 * @brief Central coordinator for IEEE standards communication and lifecycle
 * 
 * Coordinates communication between:
 * - IEEE 1588 ↔ IEEE 802.1AS (timing synchronization)
 * - IEEE 802.1AS ↔ IEEE 1722 (synchronized timing provision)  
 * - IEEE 1722 ↔ IEEE 1722.1 (transport services)
 * - All standards ↔ Management (configuration and monitoring)
 */
class StandardsCoordinator {
public:
    /**
     * @brief Get singleton instance of standards coordinator
     * @return Reference to the singleton coordinator instance
     */
    static StandardsCoordinator& getInstance();

    /**
     * @brief Register IEEE standard implementation with coordinator
     * @param standard_name IEEE standard name (e.g., "IEEE_1588_2019")
     * @param implementation Pointer to standard implementation object
     * @return 0 on success, negative error code on failure
     * 
     * Enables cross-standard communication and lifecycle management.
     * All IEEE standards MUST register with coordinator.
     */
    int register_standard(const std::string& standard_name, void* implementation);

    /**
     * @brief Unregister IEEE standard implementation
     * @param standard_name IEEE standard name to unregister
     * @return 0 on success, negative error code on failure
     */
    int unregister_standard(const std::string& standard_name);

    /**
     * @brief Send message between IEEE standards
     * @param from_standard Source standard name  
     * @param to_standard Destination standard name
     * @param message_type Message type identifier
     * @param message Message data
     * @param length Message length in bytes
     * @return 0 on success, negative error code on failure
     * 
     * Enables decoupled communication between IEEE protocols.
     * Example: IEEE 802.1AS notifies IEEE 1722 of sync state changes.
     */
    int send_cross_standard_message(const std::string& from_standard,
                                   const std::string& to_standard, 
                                   uint32_t message_type,
                                   const void* message, size_t length);

    /**
     * @brief Register message handler for cross-standard communication
     * @param standard_name Standard that will receive messages
     * @param message_type Message type to handle
     * @param handler Function to call when message is received
     * @return 0 on success, negative error code on failure
     */
    int register_message_handler(const std::string& standard_name,
                                uint32_t message_type,
                                std::function<void(const std::string&, const void*, size_t)> handler);

    /**
     * @brief Initialize all registered IEEE standards in dependency order
     * @return 0 on success, negative error code on failure
     * 
     * Initializes standards following IEEE layering principles:
     * 1. IEEE 802.1Q (network foundation)
     * 2. IEEE 1588 (timing foundation)  
     * 3. IEEE 802.1AS (network timing)
     * 4. IEEE 1722 (media transport)
     * 5. IEEE 1722.1 (device control)
     */
    int initialize_all_standards();

    /**
     * @brief Shutdown all IEEE standards in reverse dependency order
     * @return 0 on success, negative error code on failure
     * 
     * Ensures graceful shutdown with proper dependency handling.
     */
    int shutdown_all_standards();

    /**
     * @brief Get status of all registered IEEE standards
     * @param status_report Output: JSON-formatted status report
     * @return 0 on success, negative error code on failure
     * 
     * Provides unified status monitoring across all IEEE protocols.
     */
    int get_standards_status(std::string& status_report);

    /**
     * @brief Validate cross-standard dependencies are satisfied
     * @return true if all dependencies are satisfied, false otherwise
     * 
     * Verifies IEEE layering compliance and dependency satisfaction
     * before enabling inter-standard communication.
     */
    bool validate_cross_standard_dependencies();

    /**
     * @brief Register for standards lifecycle events
     * @param event_type Event type (init, shutdown, error, etc.)
     * @param callback Function to call when event occurs
     * @return 0 on success, negative error code on failure
     */
    int register_lifecycle_callback(uint32_t event_type,
                                   std::function<void(const std::string&, uint32_t)> callback);

    /**
     * @brief Enable/disable cross-standard communication
     * @param enabled true to enable communication, false to disable
     * @return 0 on success, negative error code on failure
     * 
     * Provides emergency isolation capability to prevent
     * cascading failures across IEEE standards.
     */
    int set_cross_communication_enabled(bool enabled);

private:
    StandardsCoordinator() = default;
    ~StandardsCoordinator() = default;
    
    // Prevent copying
    StandardsCoordinator(const StandardsCoordinator&) = delete;
    StandardsCoordinator& operator=(const StandardsCoordinator&) = delete;

    struct StandardInfo {
        void* implementation;
        std::string name;
        uint32_t status;
        std::unordered_map<uint32_t, std::function<void(const std::string&, const void*, size_t)>> message_handlers;
    };

    std::mutex coordination_mutex_;
    std::unordered_map<std::string, std::unique_ptr<StandardInfo>> registered_standards_;
    std::unordered_map<uint32_t, std::function<void(const std::string&, uint32_t)>> lifecycle_callbacks_;
    bool cross_communication_enabled_{true};
    bool initialized_{false};

    /**
     * @brief Internal validation of standard registration requirements
     */
    bool validate_standard_registration(const std::string& standard_name, void* implementation);
    
    /**
     * @brief Get IEEE standard initialization order based on dependencies
     */
    std::vector<std::string> get_initialization_order();
};

// Cross-standard message types for IEEE protocol coordination
namespace MessageTypes {
    constexpr uint32_t TIMING_SYNC_STATE_CHANGE = 0x1001;      // IEEE 802.1AS → IEEE 1722
    constexpr uint32_t TRANSPORT_LINK_STATE_CHANGE = 0x1002;   // IEEE 1722 → IEEE 1722.1
    constexpr uint32_t CONFIGURATION_UPDATE = 0x1003;          // Management → All standards
    constexpr uint32_t STATUS_REQUEST = 0x1004;                // Management → All standards
    constexpr uint32_t ERROR_NOTIFICATION = 0x1005;            // Any standard → All others
    constexpr uint32_t SHUTDOWN_REQUEST = 0x1006;              // Coordinator → All standards
}

// Standards lifecycle event types
namespace LifecycleEvents {
    constexpr uint32_t STANDARD_INITIALIZING = 0x2001;
    constexpr uint32_t STANDARD_INITIALIZED = 0x2002;
    constexpr uint32_t STANDARD_ERROR = 0x2003;
    constexpr uint32_t STANDARD_SHUTTING_DOWN = 0x2004;
    constexpr uint32_t STANDARD_SHUTDOWN_COMPLETE = 0x2005;
}

} // namespace integration
} // namespace Common