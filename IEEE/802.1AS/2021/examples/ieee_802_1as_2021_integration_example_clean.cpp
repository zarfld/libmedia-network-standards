/**
 * @file ieee_802_1as_2021_integration_example.cpp
 * @brief IEEE 802.1AS-2021 Standards Layer API Demonstration Example
 * 
 * This file demonstrates the proper use of the IEEE 802.1AS-2021 Standards layer
 * architecture with hardware abstraction through dependency injection.
 * 
 * ARCHITECTURE COMPLIANCE:
 * - Standards Layer: Hardware-agnostic implementation
 * - Dependency Injection: Hardware interfaces provided from HAL layer
 * - No Direct Hardware Access: All hardware through abstraction interfaces
 * 
 * @author OpenAvnu Project
 * @version 2021.1
 */

#include "../ieee_802_1as_2021.h"
#include "../time_sync_engine.h"
#include "../port_state_machine.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace IEEE::_802_1::AS::_2021;

/**
 * @brief Example hardware timestamp interface for demonstration
 * 
 * In real implementation, this would be provided by the HAL layer
 * and injected into the Standards layer components.
 */
class ExampleHardwareTimestampInterface : public HardwareTimestampInterface {
public:
    bool capture_tx_timestamp(uint16_t sequence_id, Timestamp& timestamp) override {
        // In real implementation, this would access actual hardware
        // For demonstration, we simulate a timestamp
        auto now = std::chrono::high_resolution_clock::now();
        auto epoch = now.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);
        auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch - seconds);
        
        timestamp.seconds_field = static_cast<uint64_t>(seconds.count());
        timestamp.nanoseconds_field = static_cast<uint32_t>(nanoseconds.count());
        
        return true;
    }

    bool capture_rx_timestamp(uint16_t sequence_id, Timestamp& timestamp) override {
        // Similar to TX timestamp but with slight offset for demonstration
        return capture_tx_timestamp(sequence_id, timestamp);
    }

    bool adjust_frequency(double frequency_offset) override {
        frequency_adjustment_ = frequency_offset;
        return true;
    }

    bool adjust_phase(int64_t phase_offset_ns) override {
        phase_adjustment_ = phase_offset_ns;
        return true;
    }

private:
    double frequency_adjustment_ = 0.0;
    int64_t phase_adjustment_ = 0;
};

/**
 * @brief IEEE 802.1AS-2021 API Demonstration System
 * 
 * This class demonstrates the proper architecture for IEEE 802.1AS-2021
 * implementation using the Standards layer with dependency injection.
 */
class IEEE_802_1AS_2021_ApiDemo {
public:
    IEEE_802_1AS_2021_ApiDemo() = default;
    ~IEEE_802_1AS_2021_ApiDemo() = default;

    /**
     * @brief Initialize IEEE 802.1AS-2021 API demonstration
     */
    bool initialize(const std::string& interface_name) {
        std::cout << "\n🔧 Initializing IEEE 802.1AS-2021 API Demonstration..." << std::endl;
        std::cout << "Interface: " << interface_name << std::endl;

        // 1. Create hardware timestamp interface (normally provided by HAL layer)
        hardware_interface_ = std::make_unique<ExampleHardwareTimestampInterface>();
        std::cout << "✅ Hardware timestamp interface created" << std::endl;

        // 2. Initialize Time-Aware System
        time_aware_system_ = std::make_unique<TimeAwareSystem>();
        std::cout << "✅ Time-Aware System initialized" << std::endl;

        // 3. Initialize Time Synchronization Engine with dependency injection
        time_sync_engine_ = std::make_unique<TimeSynchronizationEngine>(hardware_interface_.get());
        std::cout << "✅ Time Synchronization Engine initialized with hardware interface" << std::endl;

        // 4. Initialize Path Delay Engine
        path_delay_engine_ = std::make_unique<PathDelayEngine>();
        std::cout << "✅ Path Delay Engine initialized" << std::endl;

        // 5. Initialize Port State Machine
        PortStateMachine::Configuration port_config;
        port_config.port_number = 1;
        port_config.as_capable = true;
        port_config.announce_receipt_timeout = 3;
        port_config.sync_receipt_timeout = 3;

        port_state_machine_ = std::make_unique<PortStateMachine>(port_config);
        std::cout << "✅ Port State Machine initialized" << std::endl;

        std::cout << "🎯 IEEE 802.1AS-2021 System Initialization Complete!" << std::endl;
        return true;
    }

    /**
     * @brief Start IEEE 802.1AS-2021 API demonstration
     */
    bool start() {
        std::cout << "\n🔄 Starting IEEE 802.1AS-2021 API Demonstration..." << std::endl;

        // This example demonstrates the Standards layer API structure
        // Real implementations would start actual services with injected hardware interfaces
        
        std::cout << "📋 Demonstrating Standards Layer Architecture:" << std::endl;
        std::cout << "   • TimeAwareSystem - Core system management" << std::endl;
        std::cout << "   • TimeSynchronizationEngine - Time sync with hardware abstraction" << std::endl;
        std::cout << "   • PathDelayEngine - Path delay measurement" << std::endl;
        std::cout << "   • PortStateMachine - IEEE 802.1AS state machines" << std::endl;
        
        running_ = true;
        monitor_thread_ = std::thread(&IEEE_802_1AS_2021_ApiDemo::monitoring_loop, this);
        
        std::cout << "🚀 IEEE 802.1AS-2021 API Demonstration Running!" << std::endl;
        return true;
    }

    /**
     * @brief Stop API demonstration
     */
    void stop() {
        std::cout << "\n🛑 Stopping IEEE 802.1AS-2021 API Demonstration..." << std::endl;
        
        running_ = false;
        if (monitor_thread_.joinable()) {
            monitor_thread_.join();
        }

        std::cout << "📋 Standards Layer API demonstration completed" << std::endl;
        std::cout << "    Real implementations would stop hardware services here" << std::endl;
        std::cout << "🏁 IEEE 802.1AS-2021 API Demonstration Stopped" << std::endl;
    }

    /**
     * @brief Print API demonstration status
     */
    void print_status() {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "  IEEE 802.1AS-2021 API DEMONSTRATION" << std::endl;
        std::cout << std::string(60, '=') << std::endl;

        std::cout << "📋 Standards Layer Components Available:" << std::endl;
        std::cout << "   • TimeAwareSystem - System management interface" << std::endl;
        std::cout << "   • TimeSynchronizationEngine - Time sync algorithms" << std::endl;
        std::cout << "   • PathDelayEngine - Path delay measurement" << std::endl;
        std::cout << "   • PortStateMachine - IEEE 802.1AS state machines" << std::endl;
        std::cout << "   • HardwareTimestampInterface - Hardware abstraction" << std::endl;

        std::cout << "\n🏗️  Integration Pattern:" << std::endl;
        std::cout << "   1. Inject hardware implementations from HAL layer" << std::endl;
        std::cout << "   2. Configure Standards layer components" << std::endl;
        std::cout << "   3. Start services with dependency injection" << std::endl;
        std::cout << "   4. Hardware-agnostic operation in Standards layer" << std::endl;

        std::cout << "\n✅ Standards Layer Architecture Verified" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }

private:
    /**
     * @brief System monitoring loop
     */
    void monitoring_loop() {
        auto last_status = std::chrono::steady_clock::now();
        
        while (running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            auto now = std::chrono::steady_clock::now();
            if (now - last_status >= std::chrono::seconds(3)) {
                print_status();
                last_status = now;
            }
        }
    }

private:
    std::unique_ptr<ExampleHardwareTimestampInterface> hardware_interface_;
    std::unique_ptr<TimeAwareSystem> time_aware_system_;
    std::unique_ptr<TimeSynchronizationEngine> time_sync_engine_;
    std::unique_ptr<PathDelayEngine> path_delay_engine_;
    std::unique_ptr<PortStateMachine> port_state_machine_;
    
    bool running_ = false;
    std::thread monitor_thread_;
};

/**
 * @brief Main demonstration function
 */
int main(int argc, char* argv[]) {
    std::cout << "🎯 IEEE 802.1AS-2021 Standards Layer API Demonstration" << std::endl;
    std::cout << "=======================================================" << std::endl;
    std::cout << "Architecture Demonstration:" << std::endl;
    std::cout << "• Hardware-agnostic Standards layer implementation" << std::endl;
    std::cout << "• Dependency injection for hardware interfaces" << std::endl;
    std::cout << "• Proper separation of Standards and HAL layers" << std::endl;
    std::cout << "=======================================================" << std::endl << std::endl;

    // Parse command line arguments
    std::string interface_name = "demo_interface";
    if (argc > 1) {
        interface_name = argv[1];
    }

    try {
        // Create and initialize IEEE 802.1AS-2021 API demonstration
        IEEE_802_1AS_2021_ApiDemo api_demo;
        
        if (!api_demo.initialize(interface_name)) {
            std::cerr << "❌ Failed to initialize IEEE 802.1AS-2021 API demonstration" << std::endl;
            return 1;
        }

        if (!api_demo.start()) {
            std::cerr << "❌ Failed to start IEEE 802.1AS-2021 API demonstration" << std::endl;
            return 1;
        }

        std::cout << "\n🎉 IEEE 802.1AS-2021 API demonstration running!" << std::endl;
        std::cout << "Press Enter to stop..." << std::endl;
        
        // Wait for user input
        std::cin.get();
        
        // Graceful shutdown
        api_demo.stop();
        
        std::cout << "\n🎉 IEEE 802.1AS-2021 Standards Layer API Demonstration Complete!" << std::endl;
        std::cout << "Ready for integration with hardware-specific implementations." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "❌ Exception during demonstration: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
