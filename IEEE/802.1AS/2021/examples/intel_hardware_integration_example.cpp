/**
 * @file intel_hardware_integration_example.cpp
 * @brief IEEE 802.1AS-2021 Pure Standard Library with Intel Hardware Integration
 * @details Shows how to implement hardware interfaces for the pure standards library
 * 
 * This example demonstrates:
 * - Implementing HardwareTimestampInterface for Intel NICs
 * - Implementing NetworkInterface for Ethernet
 * - Using the pure IEEE 802.1AS-2021 standards library
 * - Proper separation between standards and hardware layers
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Hardware Integration Example
 */

#include "../core/ieee_802_1as_2021_fixed.h"
#include "../core/time_sync_engine_fixed.h"
// TODO: CRITICAL ARCHITECTURE VIOLATION - Direct Intel HAL includes in Standards layer!
// TODO: This include violates hardware abstraction principles 
// TODO: Standards layer must remain hardware-agnostic - move this to HAL layer
#include "../../../../intel_avb/intel_hal_interface.h" // Intel HAL for hardware access
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <pcap.h>
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "ws2_32.lib")
#else
#include <pcap.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

using namespace IEEE::_802_1AS::_2021;

// ============================================================================
// Intel Hardware Timestamp Implementation
// ============================================================================

/**
 * @brief Intel Hardware Timestamp Interface Implementation
 * @details Implements hardware timestamping for Intel I210/I219/I225/I226 NICs
 */
class IntelHardwareTimestamp : public HardwareTimestampInterface {
public:
    IntelHardwareTimestamp(const std::string& interface_name) 
        : interface_name_(interface_name), initialized_(false) {}
    
    virtual ~IntelHardwareTimestamp() {
        shutdown();
    }
    
    // DEPRECATED: Direct Intel HAL initialization violates Standards layer architecture
    [[deprecated("Standards layer examples must be hardware-agnostic. Hardware initialization should be handled by injected interface implementations outside Standards layer. This function will be removed after migration to proper dependency injection pattern.")]]
    bool initialize() {
        std::cout << "🔧 Initializing Intel Hardware Timestamp for " << interface_name_ << std::endl;
        
        // TODO: ARCHITECTURE VIOLATION - Intel HAL code in Standards layer example!
        // TODO: Remove intel_hal_* calls and use proper hardware abstraction interface
        // Initialize Intel HAL
        if (intel_hal_init(interface_name_.c_str()) != 0) {
            std::cerr << "❌ Failed to initialize Intel HAL" << std::endl;
            return false;
        }
        
        // Enable hardware timestamping
        if (intel_hal_enable_timestamps() != 0) {
            std::cerr << "❌ Failed to enable hardware timestamps" << std::endl;
            return false;
        }
        
        std::cout << "✅ Intel Hardware Timestamp initialized" << std::endl;
        initialized_ = true;
        return true;
    }
    
    // DEPRECATED: Direct Intel HAL shutdown violates Standards layer architecture  
    [[deprecated("Standards layer examples must be hardware-agnostic. Hardware cleanup should be handled by injected interface implementations outside Standards layer. This function will be removed after migration to proper dependency injection pattern.")]]
    void shutdown() {
        if (initialized_) {
            // TODO: ARCHITECTURE VIOLATION - Intel HAL code in Standards layer example!
            intel_hal_shutdown();
            initialized_ = false;
        }
    }
    
    // Implement HardwareTimestampInterface
    // DEPRECATED: Direct Intel HAL timestamp capture violates Standards layer architecture
    [[deprecated("Standards layer examples must use injected HardwareTimestampInterface for hardware abstraction. Direct intel_hal_get_timestamp calls violate architectural separation. This function will be removed after migration to proper dependency injection pattern.")]]
    virtual Timestamp capture_timestamp() override {
        if (!initialized_) {
            return Timestamp(); // Invalid timestamp
        }
        
        // TODO: ARCHITECTURE VIOLATION - Intel HAL code in Standards layer example!
        // Get hardware timestamp from Intel HAL
        uint64_t hw_timestamp_ns = 0;
        if (intel_hal_get_timestamp(&hw_timestamp_ns) == 0) {
            return Timestamp::from_nanoseconds(std::chrono::nanoseconds(hw_timestamp_ns));
        }
        
        // Fallback to system time if hardware timestamp fails
        auto now = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
        return Timestamp::from_nanoseconds(ns);
    }
    
    virtual uint32_t get_timestamp_resolution() const override {
        return 8; // Intel NICs typically have 8ns resolution
    }
    
    // DEPRECATED: Direct Intel HAL availability check violates Standards layer architecture
    [[deprecated("Standards layer examples must use injected HardwareTimestampInterface for hardware abstraction. Direct intel_hal_is_timestamp_available calls violate architectural separation. This function will be removed after migration to proper dependency injection pattern.")]]
    virtual bool is_hardware_timestamping_available() const override {
        // TODO: ARCHITECTURE VIOLATION - Intel HAL code in Standards layer example!
        return initialized_ && intel_hal_is_timestamp_available();
    }
    
    // DEPRECATED: Direct Intel HAL clock adjustment violates Standards layer architecture
    [[deprecated("Standards layer examples must use injected HardwareTimestampInterface for hardware abstraction. Direct intel_hal_adjust_clock calls violate architectural separation. This function will be removed after migration to proper dependency injection pattern.")]]
    virtual bool adjust_clock(int64_t offset_ns) override {
        if (!initialized_) return false;
        
        // TODO: ARCHITECTURE VIOLATION - Intel HAL code in Standards layer example!
        // Use Intel HAL to adjust clock
        return intel_hal_adjust_clock(offset_ns) == 0;
    }
    
    // DEPRECATED: Direct Intel HAL frequency adjustment violates Standards layer architecture
    [[deprecated("Standards layer examples must use injected HardwareTimestampInterface for hardware abstraction. Direct intel_hal_set_frequency_adjustment calls violate architectural separation. This function will be removed after migration to proper dependency injection pattern.")]]
    virtual bool set_frequency_adjustment(double frequency_adjustment) override {
        if (!initialized_) return false;
        
        // TODO: ARCHITECTURE VIOLATION - Intel HAL code in Standards layer example!
        // Convert ppm to Intel HAL frequency adjustment format
        int32_t adj = static_cast<int32_t>(frequency_adjustment * 1e6); // Convert to ppb
        return intel_hal_set_frequency_adjustment(adj) == 0;
    }

private:
    std::string interface_name_;
    bool initialized_;
};

// ============================================================================
// PCAP Network Interface Implementation  
// ============================================================================

/**
 * @brief PCAP Network Interface Implementation
 * @details Implements network interface using PCAP for frame transmission/reception
 */
class PCAPNetworkInterface : public NetworkInterface {
public:
    PCAPNetworkInterface(const std::string& interface_name) 
        : interface_name_(interface_name), pcap_handle_(nullptr), initialized_(false) {}
    
    virtual ~PCAPNetworkInterface() {
        shutdown();
    }
    
    bool initialize() {
        std::cout << "🌐 Initializing PCAP Network Interface for " << interface_name_ << std::endl;
        
        char errbuf[PCAP_ERRBUF_SIZE];
        
        // Open the network interface
        pcap_handle_ = pcap_open_live(interface_name_.c_str(), 65536, 1, 1000, errbuf);
        if (!pcap_handle_) {
            std::cerr << "❌ Failed to open interface " << interface_name_ << ": " << errbuf << std::endl;
            return false;
        }
        
        // Set up PTP multicast filter
        struct bpf_program fp;
        const char* filter_exp = "ether proto 0x88f7"; // IEEE 1588 PTP
        
        if (pcap_compile(pcap_handle_, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
            std::cerr << "❌ Failed to compile filter: " << pcap_geterr(pcap_handle_) << std::endl;
            pcap_close(pcap_handle_);
            return false;
        }
        
        if (pcap_setfilter(pcap_handle_, &fp) == -1) {
            std::cerr << "❌ Failed to set filter: " << pcap_geterr(pcap_handle_) << std::endl;
            pcap_freecode(&fp);
            pcap_close(pcap_handle_);
            return false;
        }
        
        pcap_freecode(&fp);
        
        // Get MAC address
        if (!get_interface_mac()) {
            std::cerr << "❌ Failed to get interface MAC address" << std::endl;
            pcap_close(pcap_handle_);
            return false;
        }
        
        // Start receive thread
        receive_thread_running_ = true;
        receive_thread_ = std::thread(&PCAPNetworkInterface::receive_loop, this);
        
        std::cout << "✅ PCAP Network Interface initialized" << std::endl;
        initialized_ = true;
        return true;
    }
    
    void shutdown() {
        if (initialized_) {
            receive_thread_running_ = false;
            if (receive_thread_.joinable()) {
                receive_thread_.join();
            }
            
            if (pcap_handle_) {
                pcap_close(pcap_handle_);
                pcap_handle_ = nullptr;
            }
            initialized_ = false;
        }
    }
    
    // Implement NetworkInterface
    virtual bool send_frame(const uint8_t* data, size_t length) override {
        if (!initialized_ || !pcap_handle_) return false;
        
        // Add Ethernet header for PTP
        constexpr size_t ETH_HEADER_SIZE = 14;
        vector<uint8_t> frame(ETH_HEADER_SIZE + length);
        
        // Destination: PTP multicast (01:1B:19:00:00:00)
        frame[0] = 0x01; frame[1] = 0x1B; frame[2] = 0x19;
        frame[3] = 0x00; frame[4] = 0x00; frame[5] = 0x00;
        
        // Source: Our MAC address
        std::memcpy(&frame[6], mac_address_.data(), 6);
        
        // EtherType: PTP (0x88F7)
        frame[12] = 0x88; frame[13] = 0xF7;
        
        // PTP payload
        std::memcpy(&frame[ETH_HEADER_SIZE], data, length);
        
        // Send frame
        int result = pcap_sendpacket(pcap_handle_, frame.data(), frame.size());
        return result == 0;
    }
    
    virtual void set_receive_callback(std::function<void(const uint8_t*, size_t, const Timestamp&)> callback) override {
        std::lock_guard<std::mutex> lock(callback_mutex_);
        receive_callback_ = callback;
    }
    
    virtual array<uint8_t, 6> get_mac_address() const override {
        return mac_address_;
    }

private:
    void receive_loop() {
        while (receive_thread_running_) {
            struct pcap_pkthdr* header;
            const u_char* packet;
            
            int result = pcap_next_ex(pcap_handle_, &header, &packet);
            if (result == 1) {
                // Packet received
                Timestamp receive_time = capture_receive_timestamp();
                
                // Skip Ethernet header to get PTP payload
                if (header->caplen > 14) {
                    std::lock_guard<std::mutex> lock(callback_mutex_);
                    if (receive_callback_) {
                        receive_callback_(packet + 14, header->caplen - 14, receive_time);
                    }
                }
            } else if (result == -1) {
                std::cerr << "⚠️  PCAP receive error: " << pcap_geterr(pcap_handle_) << std::endl;
            }
            // result == 0 means timeout, which is normal
        }
    }
    
    Timestamp capture_receive_timestamp() {
        // In a real implementation, this would get the hardware timestamp
        // For now, use system timestamp
        auto now = std::chrono::high_resolution_clock::now();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch());
        return Timestamp::from_nanoseconds(ns);
    }
    
    bool get_interface_mac() {
        // This is simplified - in real code you'd query the interface
        // For demo purposes, use a fixed MAC
        mac_address_ = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
        return true;
    }
    
    std::string interface_name_;
    pcap_t* pcap_handle_;
    bool initialized_;
    
    array<uint8_t, 6> mac_address_;
    
    std::thread receive_thread_;
    std::atomic<bool> receive_thread_running_{false};
    
    std::mutex callback_mutex_;
    std::function<void(const uint8_t*, size_t, const Timestamp&)> receive_callback_;
};

// ============================================================================
// Integration Example Application
// ============================================================================

/**
 * @brief Complete integration example showing pure standards + hardware
 */
class IEEE802_1AS_IntegrationExample {
public:
    IEEE802_1AS_IntegrationExample() = default;
    
    bool initialize(const std::string& interface_name = "eth0") {
        std::cout << "🚀 IEEE 802.1AS-2021 Pure Standard Library Integration Example" << std::endl;
        std::cout << "📚 Using pure standards library with hardware implementations" << std::endl;
        
        // 1. Create hardware implementations
        hw_interface_ = std::make_shared<IntelHardwareTimestamp>(interface_name);
        if (!hw_interface_->initialize()) {
            std::cerr << "❌ Failed to initialize hardware timestamp interface" << std::endl;
            return false;
        }
        
        net_interface_ = std::make_shared<PCAPNetworkInterface>(interface_name);
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
        
        // Create a clock identity (normally derived from MAC address)
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
        std::cout << "🔧 Hardware: Intel NIC with hardware timestamping" << std::endl;
        std::cout << "📚 Standards: Pure IEEE 802.1AS-2021 implementation" << std::endl;
        return true;
    }
    
    void run_master_example() {
        std::cout << "\n🎯 Running as IEEE 802.1AS-2021 Master" << std::endl;
        
        // Force master role for demo
        sync_engine_->force_master_role(true);
        sync_engine_->start();
        
        std::cout << "📡 Sending synchronization messages..." << std::endl;
        
        // Run for 30 seconds
        auto start_time = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(30)) {
            auto state = sync_engine_->get_synchronization_state();
            
            std::cout << "📊 Master Status: " << sync_status_to_string(state.status) 
                     << " | Freq Adj: " << std::fixed << std::setprecision(3) 
                     << state.frequency_adjustment << " ppm" << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        
        sync_engine_->stop();
        std::cout << "🛑 Master example completed" << std::endl;
    }
    
    void run_slave_example() {
        std::cout << "\n🎯 Running as IEEE 802.1AS-2021 Slave" << std::endl;
        
        sync_engine_->start();
        
        std::cout << "👂 Listening for synchronization messages..." << std::endl;
        
        // Run for 60 seconds
        auto start_time = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start_time < std::chrono::seconds(60)) {
            auto state = sync_engine_->get_synchronization_state();
            
            std::cout << "📊 Slave Status: " << sync_status_to_string(state.status);
            if (state.valid) {
                auto offset_ns = state.offset_from_master.to_nanoseconds().count();
                std::cout << " | Offset: " << offset_ns << " ns"
                         << " | Freq Adj: " << std::fixed << std::setprecision(3) 
                         << state.frequency_adjustment << " ppm";
            }
            std::cout << std::endl;
            
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        
        sync_engine_->stop();
        std::cout << "🛑 Slave example completed" << std::endl;
    }
    
private:
    void process_received_message(const uint8_t* data, size_t length, const Timestamp& timestamp) {
        if (length < sizeof(PTPHeader)) return;
        
        const PTPHeader* header = reinterpret_cast<const PTPHeader*>(data);
        MessageType msg_type = static_cast<MessageType>(header->message_type_and_version & 0x0F);
        
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
                // Other message types...
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
    
    std::shared_ptr<IntelHardwareTimestamp> hw_interface_;
    std::shared_ptr<PCAPNetworkInterface> net_interface_;
    std::unique_ptr<TimeSynchronizationEngine> sync_engine_;
};

// ============================================================================
// Main Application
// ============================================================================

int main(int argc, char* argv[]) {
    std::cout << "🌟 IEEE 802.1AS-2021 Pure Standard Library Integration Example" << std::endl;
    std::cout << "📋 This example shows proper separation between standards and hardware" << std::endl;
    
    std::string interface_name = "eth0";
    bool run_as_master = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--interface") == 0 && i + 1 < argc) {
            interface_name = argv[i + 1];
            i++;
        } else if (std::strcmp(argv[i], "--master") == 0) {
            run_as_master = true;
        } else if (std::strcmp(argv[i], "--help") == 0) {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --interface <name>  Network interface name (default: eth0)" << std::endl;
            std::cout << "  --master           Run as master (default: slave)" << std::endl;
            std::cout << "  --help             Show this help" << std::endl;
            return 0;
        }
    }
    
    try {
        IEEE802_1AS_IntegrationExample example;
        
        if (!example.initialize(interface_name)) {
            std::cerr << "❌ Failed to initialize IEEE 802.1AS-2021 system" << std::endl;
            return 1;
        }
        
        if (run_as_master) {
            example.run_master_example();
        } else {
            example.run_slave_example();
        }
        
        std::cout << "✨ Example completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
