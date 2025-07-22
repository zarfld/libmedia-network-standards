/**
 * @file pure_standard_integration_example.cpp
 * @brief IEEE 802.1AS-2021 Pure Standard Library Integration Example  
 * @details Shows how to implement hardware interfaces for the pure standards library
 * 
 * This example demonstrates:
 * - Implementing HardwareTimestampInterface (mock implementation for demo)
 * - Implementing NetworkInterface (mock implementation for demo)  
 * - Using the pure IEEE 802.1AS-2021 standards library
 * - Proper separation between standards and hardware layers
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Pure Standard Integration Example
 */

#include "../core/ieee_802_1as_2021_fixed.h"
#include "../core/time_sync_engine_fixed.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <cstring>

using namespace IEEE::_802_1AS::_2021;

// ============================================================================
// Mock Hardware Timestamp Implementation (for demonstration)
// ============================================================================

/**
 * @brief Mock Hardware Timestamp Interface Implementation
 * @details Simulates hardware timestamping for demonstration purposes
 * In real implementation, this would interface with actual hardware (Intel HAL, etc.)
 */
class MockHardwareTimestamp : public HardwareTimestampInterface {
public:
    MockHardwareTimestamp(const std::string& interface_name) 
        : interface_name_(interface_name), initialized_(false) {
        // Simulate some hardware characteristics
        resolution_ns_ = 8; // 8ns resolution like Intel NICs
        frequency_drift_ = 0.0; // Start with no drift
    }
    
    virtual ~MockHardwareTimestamp() = default;
    
    bool initialize() {
        std::cout << "🔧 Initializing Mock Hardware Timestamp for " << interface_name_ << std::endl;
        
        // Simulate hardware initialization
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Start the mock hardware clock
        start_time_ = std::chrono::high_resolution_clock::now();
        
        std::cout << "✅ Mock Hardware Timestamp initialized (8ns resolution)" << std::endl;
        initialized_ = true;
        return true;
    }
    
    // Implement HardwareTimestampInterface
    virtual Timestamp capture_timestamp() override {
        if (!initialized_) {
            return Timestamp(); // Invalid timestamp
        }
        
        // Get current time and apply simulated drift
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - start_time_;
        
        // Apply frequency drift simulation
        auto adjusted_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        adjusted_ns = std::chrono::nanoseconds(static_cast<int64_t>(
            adjusted_ns.count() * (1.0 + frequency_drift_ / 1e9)));
        
        // Add simulated timestamp from epoch
        auto base_time = std::chrono::system_clock::now().time_since_epoch();
        auto total_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(base_time) + adjusted_ns;
        
        return Timestamp::from_nanoseconds(total_ns);
    }
    
    virtual uint32_t get_timestamp_resolution() const override {
        return resolution_ns_;
    }
    
    virtual bool is_hardware_timestamping_available() const override {
        return initialized_;
    }
    
    virtual bool adjust_clock(int64_t offset_ns) override {
        if (!initialized_) return false;
        
        std::cout << "🔧 Hardware clock adjustment: " << offset_ns << " ns" << std::endl;
        
        // Simulate clock step adjustment
        start_time_ -= std::chrono::nanoseconds(offset_ns);
        return true;
    }
    
    virtual bool set_frequency_adjustment(double frequency_adjustment) override {
        if (!initialized_) return false;
        
        std::cout << "🔧 Hardware frequency adjustment: " << std::fixed 
                  << std::setprecision(3) << frequency_adjustment << " ppm" << std::endl;
        
        // Store frequency drift for simulation
        frequency_drift_ = frequency_adjustment * 1e-6; // Convert ppm to ratio
        return true;
    }

private:
    std::string interface_name_;
    bool initialized_;
    uint32_t resolution_ns_;
    std::chrono::high_resolution_clock::time_point start_time_;
    double frequency_drift_; // Simulated frequency drift
};

// ============================================================================
// Mock Network Interface Implementation
// ============================================================================

/**
 * @brief Mock Network Interface Implementation  
 * @details Simulates network interface for demonstration purposes
 * In real implementation, this would use PCAP, raw sockets, etc.
 */
class MockNetworkInterface : public NetworkInterface {
public:
    MockNetworkInterface(const std::string& interface_name) 
        : interface_name_(interface_name), initialized_(false), frame_count_(0) {
        
        // Generate a mock MAC address
        mac_address_ = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
    }
    
    virtual ~MockNetworkInterface() {
        shutdown();
    }
    
    bool initialize() {
        std::cout << "🌐 Initializing Mock Network Interface for " << interface_name_ << std::endl;
        
        // Simulate network initialization  
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        std::cout << "✅ Mock Network Interface initialized (MAC: ";
        for (size_t i = 0; i < mac_address_.size(); ++i) {
            std::cout << std::hex << std::setfill('0') << std::setw(2) 
                      << static_cast<int>(mac_address_[i]);
            if (i < mac_address_.size() - 1) std::cout << ":";
        }
        std::cout << std::dec << ")" << std::endl;
        
        initialized_ = true;
        return true;
    }
    
    void shutdown() {
        initialized_ = false;
    }
    
    // Implement NetworkInterface
    virtual bool send_frame(const uint8_t* data, size_t length) override {
        if (!initialized_) return false;
        
        frame_count_++;
        
        // Simulate frame transmission
        std::cout << "📤 TX Frame #" << frame_count_ << " (" << length << " bytes)";
        
        // Identify message type for better logging
        if (length >= sizeof(PTPHeader)) {
            const PTPHeader* header = reinterpret_cast<const PTPHeader*>(data);
            MessageType msg_type = static_cast<MessageType>(header->message_type_and_version & 0x0F);
            std::cout << " - " << message_type_to_string(msg_type);
        }
        std::cout << std::endl;
        
        // Simulate transmission delay
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        
        return true;
    }
    
    virtual void set_receive_callback(std::function<void(const uint8_t*, size_t, const Timestamp&)> callback) override {
        std::lock_guard<std::mutex> lock(callback_mutex_);
        receive_callback_ = callback;
        std::cout << "📡 Network receive callback configured" << std::endl;
    }
    
    virtual array<uint8_t, 6> get_mac_address() const override {
        return mac_address_;
    }
    
    // Simulate receiving a frame (for testing)
    void simulate_received_frame(const uint8_t* data, size_t length) {
        if (!initialized_) return;
        
        std::cout << "📥 RX Frame (" << length << " bytes)";
        
        // Identify message type for better logging
        if (length >= sizeof(PTPHeader)) {
            const PTPHeader* header = reinterpret_cast<const PTPHeader*>(data);
            MessageType msg_type = static_cast<MessageType>(header->message_type_and_version & 0x0F);
            std::cout << " - " << message_type_to_string(msg_type);
        }
        std::cout << std::endl;
        
        // Simulate receive timestamp
        Timestamp receive_time = Timestamp::from_nanoseconds(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch()));
        
        std::lock_guard<std::mutex> lock(callback_mutex_);
        if (receive_callback_) {
            receive_callback_(data, length, receive_time);
        }
    }

private:
    std::string message_type_to_string(MessageType type) {
        switch (type) {
            case MessageType::SYNC: return "SYNC";
            case MessageType::FOLLOW_UP: return "FOLLOW_UP";  
            case MessageType::ANNOUNCE: return "ANNOUNCE";
            case MessageType::PDELAY_REQ: return "PDELAY_REQ";
            case MessageType::PDELAY_RESP: return "PDELAY_RESP";
            case MessageType::PDELAY_RESP_FOLLOW_UP: return "PDELAY_RESP_FU";
            default: return "UNKNOWN";
        }
    }
    
    std::string interface_name_;
    bool initialized_;
    array<uint8_t, 6> mac_address_;
    std::atomic<uint32_t> frame_count_;
    
    std::mutex callback_mutex_;
    std::function<void(const uint8_t*, size_t, const Timestamp&)> receive_callback_;
};

// ============================================================================
// Integration Example Application  
// ============================================================================

/**
 * @brief Complete integration example showing pure standards + mock hardware
 */
class IEEE802_1AS_PureIntegrationExample {
public:
    IEEE802_1AS_PureIntegrationExample() = default;
    
    bool initialize(const std::string& interface_name = "mock0") {
        std::cout << "🚀 IEEE 802.1AS-2021 Pure Standard Library Integration Example" << std::endl;
        std::cout << "📚 Using pure standards library with mock hardware implementations" << std::endl;
        std::cout << "💡 This shows the proper architecture: Standards ↔ Hardware Interfaces" << std::endl;
        
        // 1. Create mock hardware implementations
        hw_interface_ = std::make_shared<MockHardwareTimestamp>(interface_name);
        if (!hw_interface_->initialize()) {
            std::cerr << "❌ Failed to initialize hardware timestamp interface" << std::endl;
            return false;
        }
        
        net_interface_ = std::make_shared<MockNetworkInterface>(interface_name);
        if (!net_interface_->initialize()) {
            std::cerr << "❌ Failed to initialize network interface" << std::endl;
            return false;
        }
        
        // 2. Create pure standards engines using factory functions
        sync_engine_ = create_time_sync_engine(hw_interface_, net_interface_);
        if (!sync_engine_) {
            std::cerr << "❌ Failed to create synchronization engine" << std::endl;
            return false;
        }
        
        // 3. Configure the synchronization engine
        TimeSynchronizationEngine::Configuration sync_config;
        sync_config.sync_interval = std::chrono::milliseconds(125); // 8 per second
        sync_config.announce_interval = std::chrono::milliseconds(1000); // 1 per second
        sync_config.clock_class = 248; // Default slave class
        sync_config.proportional_gain = 1.0;
        sync_config.integral_gain = 0.1;
        
        // Create a clock identity (derived from MAC address)
        ClockIdentity clock_id;
        auto mac = net_interface_->get_mac_address();
        // EUI-64: MAC[0:3] + 0xFF + 0xFE + MAC[3:6]
        clock_id.identity[0] = mac[0]; clock_id.identity[1] = mac[1]; clock_id.identity[2] = mac[2];
        clock_id.identity[3] = 0xFF; clock_id.identity[4] = 0xFE;
        clock_id.identity[5] = mac[3]; clock_id.identity[6] = mac[4]; clock_id.identity[7] = mac[5];
        
        if (!sync_engine_->initialize(sync_config, clock_id)) {
            std::cerr << "❌ Failed to initialize synchronization engine" << std::endl;
            return false;
        }
        
        // 4. Set up message processing callback
        net_interface_->set_receive_callback(
            [this](const uint8_t* data, size_t length, const Timestamp& timestamp) {
                process_received_message(data, length, timestamp);
            });
        
        std::cout << "✅ IEEE 802.1AS-2021 system fully initialized" << std::endl;
        std::cout << "🔧 Hardware: Mock implementations (shows interface usage)" << std::endl;
        std::cout << "📚 Standards: Pure IEEE 802.1AS-2021 implementation" << std::endl;
        return true;
    }
    
    void run_master_example() {
        std::cout << "\n🎯 Running as IEEE 802.1AS-2021 Master" << std::endl;
        std::cout << "📡 This demonstrates the pure standards library working with hardware interfaces" << std::endl;
        
        // Force master role for demo
        sync_engine_->force_master_role(true);
        sync_engine_->start();
        
        std::cout << "📤 Sending synchronization messages..." << std::endl;
        
        // Run for 15 seconds
        auto start_time = std::chrono::steady_clock::now();
        int status_count = 0;
        
        while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(15)) {
            auto state = sync_engine_->get_synchronization_state();
            
            status_count++;
            std::cout << "[" << std::setw(2) << status_count << "] "
                     << "Master Status: " << sync_status_to_string(state.status);
            
            if (state.valid) {
                std::cout << " | Freq Adj: " << std::fixed << std::setprecision(3) 
                         << state.frequency_adjustment << " ppm";
            }
            std::cout << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        sync_engine_->stop();
        std::cout << "🛑 Master example completed" << std::endl;
        std::cout << "✨ Notice: All frame transmissions went through the NetworkInterface" << std::endl;
        std::cout << "✨ Notice: All timestamp operations went through the HardwareTimestampInterface" << std::endl;
    }
    
    void run_slave_example() {
        std::cout << "\n🎯 Running as IEEE 802.1AS-2021 Slave" << std::endl;
        std::cout << "👂 This demonstrates message processing with the pure standards library" << std::endl;
        
        sync_engine_->start();
        
        std::cout << "📥 Listening for synchronization messages..." << std::endl;
        std::cout << "💡 Simulating received messages to show message processing..." << std::endl;
        
        // Simulate some received messages
        std::thread simulation_thread([this]() {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            simulate_master_messages();
        });
        
        // Run for 20 seconds
        auto start_time = std::chrono::steady_clock::now();
        int status_count = 0;
        
        while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(20)) {
            auto state = sync_engine_->get_synchronization_state();
            
            status_count++;
            std::cout << "[" << std::setw(2) << status_count << "] "
                     << "Slave Status: " << sync_status_to_string(state.status);
            
            if (state.valid) {
                auto offset_ns = state.offset_from_master.to_nanoseconds().count();
                std::cout << " | Offset: " << std::setw(8) << offset_ns << " ns"
                         << " | Freq Adj: " << std::fixed << std::setprecision(3) 
                         << state.frequency_adjustment << " ppm";
            }
            std::cout << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        if (simulation_thread.joinable()) {
            simulation_thread.join();
        }
        
        sync_engine_->stop();
        std::cout << "🛑 Slave example completed" << std::endl;
        std::cout << "✨ Notice: All message processing used pure IEEE 802.1AS-2021 algorithms" << std::endl;
        std::cout << "✨ Notice: Hardware clock adjustments went through the interface" << std::endl;
    }
    
private:
    void simulate_master_messages() {
        // Simulate an announce message from a master
        AnnounceMessage announce;
        announce.header.sequence_id = 1;
        announce.grandmaster_priority1 = 128; // Better than our default 255
        announce.steps_removed = 1;
        
        std::cout << "🎭 Simulating received ANNOUNCE message from master..." << std::endl;
        net_interface_->simulate_received_frame(
            reinterpret_cast<const uint8_t*>(&announce), sizeof(announce));
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Simulate sync + follow-up sequence
        for (int i = 0; i < 5; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(125)); // 8 per second
            
            // Sync message
            SyncMessage sync;
            sync.header.sequence_id = i + 10;
            sync.origin_timestamp = hw_interface_->capture_timestamp();
            
            std::cout << "🎭 Simulating received SYNC message #" << (i+1) << "..." << std::endl;
            net_interface_->simulate_received_frame(
                reinterpret_cast<const uint8_t*>(&sync), sizeof(sync));
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            // Follow-up message with precise timestamp
            FollowUpMessage follow_up;
            follow_up.header.sequence_id = i + 10;
            follow_up.precise_origin_timestamp = sync.origin_timestamp;
            
            std::cout << "🎭 Simulating received FOLLOW_UP message #" << (i+1) << "..." << std::endl;
            net_interface_->simulate_received_frame(
                reinterpret_cast<const uint8_t*>(&follow_up), sizeof(follow_up));
        }
    }
    
    void process_received_message(const uint8_t* data, size_t length, const Timestamp& timestamp) {
        if (length < sizeof(PTPHeader)) return;
        
        const PTPHeader* header = reinterpret_cast<const PTPHeader*>(data);
        MessageType msg_type = static_cast<MessageType>(header->message_type_and_version & 0x0F);
        
        std::cout << "⚙️  Processing " << message_type_to_string(msg_type) 
                  << " in pure standards library..." << std::endl;
        
        switch (msg_type) {
            case MessageType::SYNC:
                if (length >= sizeof(SyncMessage)) {
                    sync_engine_->process_sync_message(
                        *reinterpret_cast<const SyncMessage*>(data), timestamp);
                }
                break;
                
            case MessageType::FOLLOW_UP:
                if (length >= sizeof(FollowUpMessage)) {
                    sync_engine_->process_follow_up_message(
                        *reinterpret_cast<const FollowUpMessage*>(data));
                }
                break;
                
            case MessageType::ANNOUNCE:
                if (length >= sizeof(AnnounceMessage)) {
                    sync_engine_->process_announce_message(
                        *reinterpret_cast<const AnnounceMessage*>(data), timestamp);
                }
                break;
                
            default:
                std::cout << "⚠️  Unknown message type: " << static_cast<int>(msg_type) << std::endl;
                break;
        }
    }
    
    std::string sync_status_to_string(SyncStatus status) {
        switch (status) {
            case SyncStatus::INITIALIZING: return "INITIALIZING";
            case SyncStatus::ACQUIRING_SYNC: return "ACQUIRING_SYNC";
            case SyncStatus::SYNCHRONIZED: return "SYNCHRONIZED";
            case SyncStatus::HOLDOVER: return "HOLDOVER";
            case SyncStatus::LOST_SYNC: return "LOST_SYNC";
            case SyncStatus::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }
    
    std::string message_type_to_string(MessageType type) {
        switch (type) {
            case MessageType::SYNC: return "SYNC";
            case MessageType::FOLLOW_UP: return "FOLLOW_UP";
            case MessageType::ANNOUNCE: return "ANNOUNCE";
            case MessageType::PDELAY_REQ: return "PDELAY_REQ";
            case MessageType::PDELAY_RESP: return "PDELAY_RESP";
            case MessageType::PDELAY_RESP_FOLLOW_UP: return "PDELAY_RESP_FU";
            default: return "UNKNOWN";
        }
    }
    
    std::shared_ptr<MockHardwareTimestamp> hw_interface_;
    std::shared_ptr<MockNetworkInterface> net_interface_;
    std::unique_ptr<TimeSynchronizationEngine> sync_engine_;
};

// ============================================================================
// Main Application
// ============================================================================

int main(int argc, char* argv[]) {
    std::cout << "🌟 IEEE 802.1AS-2021 Pure Standard Library Integration Example" << std::endl;
    std::cout << "📋 This example demonstrates the CORRECT architecture:" << std::endl;
    std::cout << "   📚 Pure Standards Library (no hardware dependencies)" << std::endl;
    std::cout << "   🔌 Hardware Interface Implementations (mock for demo)" << std::endl;
    std::cout << "   🔗 Clean separation and interface usage" << std::endl;
    
    bool run_as_master = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--master") == 0) {
            run_as_master = true;
        } else if (std::strcmp(argv[i], "--help") == 0) {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --master    Run as master (default: slave)" << std::endl;
            std::cout << "  --help      Show this help" << std::endl;
            return 0;
        }
    }
    
    try {
        IEEE802_1AS_PureIntegrationExample example;
        
        if (!example.initialize("mock0")) {
            std::cerr << "❌ Failed to initialize IEEE 802.1AS-2021 system" << std::endl;
            return 1;
        }
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        
        if (run_as_master) {
            example.run_master_example();
        } else {
            example.run_slave_example();
        }
        
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "✨ Example completed successfully!" << std::endl;
        std::cout << "\n💡 Key Architecture Points Demonstrated:" << std::endl;
        std::cout << "   ✅ Pure standards library with NO hardware dependencies" << std::endl;
        std::cout << "   ✅ Hardware interfaces implemented separately" << std::endl;
        std::cout << "   ✅ Clean separation between 'what' (standards) and 'how' (hardware)" << std::endl;
        std::cout << "   ✅ Mock implementations show interface usage patterns" << std::endl;
        std::cout << "\n🔧 For Real Hardware Integration:" << std::endl;
        std::cout << "   • Replace MockHardwareTimestamp with Intel HAL implementation" << std::endl;
        std::cout << "   • Replace MockNetworkInterface with PCAP/raw socket implementation" << std::endl;
        std::cout << "   • Standards library remains unchanged!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
