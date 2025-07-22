/**
 * @file example_avdecc_controller.cpp
 * @brief Example IEEE 1722.1-2021 AVDECC Controller Application
 * @details Demonstrates how to use the refactored AVDECC library for controller applications
 * 
 * @author OpenAvnu AVDECC Implementation
 * @date 2025
 * @version 1.0.0
 * 
 * @copyright
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "1722_1_state_machines_refactored.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <atomic>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <unistd.h>
#endif

using namespace IEEE::_1722_1::_2021::StateMachines;

// ============================================================================
// EXAMPLE NETWORK INTERFACE IMPLEMENTATION
// ============================================================================

/**
 * @brief Example network interface implementation using raw sockets
 * @details This demonstrates how applications can implement INetworkInterface
 */
class ExampleNetworkInterface : public INetworkInterface {
public:
    ExampleNetworkInterface(const std::string& interfaceName = "")
        : _interfaceName(interfaceName), _socket(-1), _ready(false) {
        initialize();
    }
    
    ~ExampleNetworkInterface() {
        cleanup();
    }
    
    bool sendRawMessage(const uint8_t* data, size_t length, const MacAddress& destination) override {
        if (!_ready || !data || length == 0) {
            return false;
        }
        
        std::cout << "📤 Sending " << length << " bytes to " << destination.toString() << std::endl;
        
        // In a real implementation, this would send the raw Ethernet frame
        // For this example, we'll just simulate successful sending
        return true;
    }
    
    MacAddress getLocalMacAddress() const override {
        // In a real implementation, this would query the actual network interface
        // For this example, return a placeholder MAC address
        return MacAddress(0x02, 0x00, 0x00, 0x00, 0x00, 0x01);
    }
    
    bool isReady() const override {
        return _ready;
    }
    
    size_t getMTU() const override {
        return 1500; // Standard Ethernet MTU
    }

private:
    std::string _interfaceName;
    int _socket;
    bool _ready;
    
    void initialize() {
        std::cout << "🔌 Initializing network interface: " << (_interfaceName.empty() ? "default" : _interfaceName) << std::endl;
        
        // In a real implementation, this would:
        // 1. Open a raw socket
        // 2. Bind to the specified network interface
        // 3. Set up packet filtering for AVDECC frames
        // 4. Configure non-blocking mode
        
        // For this example, just simulate successful initialization
        _ready = true;
        std::cout << "✅ Network interface ready" << std::endl;
    }
    
    void cleanup() {
        if (_socket >= 0) {
#ifdef _WIN32
            closesocket(_socket);
#else
            close(_socket);
#endif
            _socket = -1;
        }
        _ready = false;
    }
};

// ============================================================================
// EXAMPLE CONTROLLER APPLICATION DELEGATE
// ============================================================================

/**
 * @brief Example controller application delegate
 * @details Demonstrates how applications handle AVDECC events
 */
class ExampleControllerDelegate : public AVDECCLibrary::Delegate {
public:
    ExampleControllerDelegate() : _entityCount(0) {}
    
    void onEntityDiscovered(const DiscoveredEntity& entity) override {
        _entityCount++;
        std::cout << "🎯 Entity discovered #" << _entityCount << ":" << std::endl;
        std::cout << "   Entity ID: 0x" << std::hex << std::setfill('0') << std::setw(16) << entity.entityID << std::dec << std::endl;
        std::cout << "   Model ID:  0x" << std::hex << std::setfill('0') << std::setw(16) << entity.entityModelID << std::dec << std::endl;
        std::cout << "   MAC:       " << entity.macAddress.toString() << std::endl;
        std::cout << "   Talkers:   " << entity.talkerStreamSources << std::endl;
        std::cout << "   Listeners: " << entity.listenerStreamSinks << std::endl;
        std::cout << std::endl;
        
        // Store entity for later operations
        _discoveredEntities[entity.entityID] = entity;
        
        // Example: After discovering 2 entities, try to connect them
        if (_entityCount == 2) {
            demonstrateStreamConnection();
        }
    }
    
    void onEntityDeparted(EntityID entityID) override {
        std::cout << "👋 Entity departed: 0x" << std::hex << std::setfill('0') << std::setw(16) << entityID << std::dec << std::endl;
        _discoveredEntities.erase(entityID);
    }
    
    void onStreamConnected(EntityID talkerEntityID, uint16_t talkerUniqueID,
                          EntityID listenerEntityID, uint16_t listenerUniqueID) override {
        std::cout << "🔗 Stream connected:" << std::endl;
        std::cout << "   Talker:   0x" << std::hex << std::setfill('0') << std::setw(16) << talkerEntityID << std::dec << "[" << talkerUniqueID << "]" << std::endl;
        std::cout << "   Listener: 0x" << std::hex << std::setfill('0') << std::setw(16) << listenerEntityID << std::dec << "[" << listenerUniqueID << "]" << std::endl;
    }
    
    void onStreamDisconnected(EntityID talkerEntityID, uint16_t talkerUniqueID,
                             EntityID listenerEntityID, uint16_t listenerUniqueID) override {
        std::cout << "🔌 Stream disconnected:" << std::endl;
        std::cout << "   Talker:   0x" << std::hex << std::setfill('0') << std::setw(16) << talkerEntityID << std::dec << "[" << talkerUniqueID << "]" << std::endl;
        std::cout << "   Listener: 0x" << std::hex << std::setfill('0') << std::setw(16) << listenerEntityID << std::dec << "[" << listenerUniqueID << "]" << std::endl;
    }
    
    void onDescriptorChanged(EntityID entityID, DescriptorType descriptorType, uint16_t descriptorIndex) override {
        std::cout << "📝 Descriptor changed:" << std::endl;
        std::cout << "   Entity: 0x" << std::hex << std::setfill('0') << std::setw(16) << entityID << std::dec << std::endl;
        std::cout << "   Type:   " << descriptorType << std::endl;
        std::cout << "   Index:  " << descriptorIndex << std::endl;
    }
    
    void onConfigurationChanged(EntityID entityID, uint16_t configurationIndex) override {
        std::cout << "⚙️  Configuration changed:" << std::endl;
        std::cout << "   Entity: 0x" << std::hex << std::setfill('0') << std::setw(16) << entityID << std::dec << std::endl;
        std::cout << "   Config: " << configurationIndex << std::endl;
    }
    
    void setLibrary(AVDECCLibrary* library) {
        _library = library;
    }

private:
    std::unordered_map<EntityID, DiscoveredEntity> _discoveredEntities;
    std::atomic<int> _entityCount;
    AVDECCLibrary* _library = nullptr;
    
    void demonstrateStreamConnection() {
        if (_discoveredEntities.size() < 2 || !_library) {
            return;
        }
        
        // Find a talker and listener
        EntityID talkerID = 0, listenerID = 0;
        uint16_t talkerUniqueID = 0, listenerUniqueID = 0;
        
        for (const auto& pair : _discoveredEntities) {
            if (pair.second.talkerStreamSources > 0 && talkerID == 0) {
                talkerID = pair.first;
                talkerUniqueID = 0; // Use first stream
            }
            if (pair.second.listenerStreamSinks > 0 && listenerID == 0) {
                listenerID = pair.first;
                listenerUniqueID = 0; // Use first stream
            }
        }
        
        if (talkerID != 0 && listenerID != 0) {
            std::cout << "🔄 Attempting to connect streams..." << std::endl;
            bool success = _library->connectStream(talkerID, talkerUniqueID, listenerID, listenerUniqueID);
            std::cout << (success ? "✅" : "❌") << " Connection attempt " << (success ? "initiated" : "failed") << std::endl;
        }
    }
};

// ============================================================================
// MAIN APPLICATION
// ============================================================================

int main() {
    std::cout << "🚀 IEEE 1722.1-2021 AVDECC Controller Example" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << std::endl;
    
    try {
        // 1. Create network interface
        std::cout << "📡 Creating network interface..." << std::endl;
        auto networkInterface = std::make_unique<ExampleNetworkInterface>("eth0");
        
        // 2. Create controller delegate
        std::cout << "🎮 Creating controller delegate..." << std::endl;
        auto controllerDelegate = std::make_unique<ExampleControllerDelegate>();
        
        // 3. Create AVDECC library using factory
        std::cout << "📚 Creating AVDECC library..." << std::endl;
        auto library = LibraryFactory::createController(std::move(networkInterface), std::move(controllerDelegate));
        
        // Need to get raw pointer for delegate callback
        ExampleControllerDelegate* delegatePtr = static_cast<ExampleControllerDelegate*>(controllerDelegate.get());
        delegatePtr->setLibrary(library.get());
        
        // 4. Configure library
        std::cout << "⚙️  Configuring library..." << std::endl;
        library->setDiscoveryInterval(std::chrono::milliseconds(2000)); // Discover every 2 seconds
        library->setEntityTimeout(std::chrono::seconds(10));           // Timeout after 10 seconds
        library->setCommandTimeout(std::chrono::milliseconds(1000));   // Command timeout 1 second
        
        // 5. Start the library
        std::cout << "🟢 Starting AVDECC library..." << std::endl;
        if (!library->start()) {
            std::cerr << "❌ Failed to start AVDECC library" << std::endl;
            return 1;
        }
        
        std::cout << "✅ AVDECC library started successfully" << std::endl;
        std::cout << std::endl;
        
        // 6. Begin discovery
        std::cout << "🔍 Starting entity discovery..." << std::endl;
        library->discoverAllEntities();
        
        // 7. Run for a while to demonstrate functionality
        std::cout << "⏱️  Running for 30 seconds..." << std::endl;
        std::cout << "   (In a real application, this would run indefinitely)" << std::endl;
        std::cout << std::endl;
        
        for (int i = 0; i < 30; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            // Every 5 seconds, show discovered entities
            if ((i + 1) % 5 == 0) {
                auto entities = library->getDiscoveredEntities();
                std::cout << "📊 Discovered entities: " << entities.size() << std::endl;
                
                // In a real application, you might also:
                // - Read descriptors from entities
                // - Monitor stream connections
                // - Handle user commands
                // - Update UI displays
            }
        }
        
        // 8. Demonstrate additional operations
        std::cout << std::endl;
        std::cout << "🔧 Demonstrating additional operations..." << std::endl;
        
        auto entities = library->getDiscoveredEntities();
        if (!entities.empty()) {
            EntityID entityID = entities[0].entityID;
            
            // Example: Read entity descriptor
            std::cout << "📖 Reading entity descriptor..." << std::endl;
            uint8_t descriptorData[512];
            size_t descriptorSize = sizeof(descriptorData);
            bool success = library->readDescriptor(entityID, 0, 0, descriptorData, descriptorSize);
            std::cout << (success ? "✅" : "❌") << " Read descriptor " << (success ? "succeeded" : "failed") << std::endl;
            
            // Example: Set configuration
            std::cout << "⚙️  Setting configuration..." << std::endl;
            success = library->setConfiguration(entityID, 0);
            std::cout << (success ? "✅" : "❌") << " Set configuration " << (success ? "succeeded" : "failed") << std::endl;
        }
        
        // 9. Clean shutdown
        std::cout << std::endl;
        std::cout << "🛑 Shutting down..." << std::endl;
        library->stop();
        std::cout << "✅ Shutdown complete" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "💥 Exception: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "🎉 Example completed successfully!" << std::endl;
    std::cout << std::endl;
    std::cout << "📋 Summary of what this example demonstrated:" << std::endl;
    std::cout << "   ✓ Network interface abstraction" << std::endl;
    std::cout << "   ✓ Controller delegate implementation" << std::endl;
    std::cout << "   ✓ Library factory usage" << std::endl;
    std::cout << "   ✓ Entity discovery" << std::endl;
    std::cout << "   ✓ Stream connection attempts" << std::endl;
    std::cout << "   ✓ Descriptor reading" << std::endl;
    std::cout << "   ✓ Configuration management" << std::endl;
    std::cout << "   ✓ Clean shutdown" << std::endl;
    std::cout << std::endl;
    std::cout << "🔗 Next steps for real applications:" << std::endl;
    std::cout << "   • Implement actual raw socket networking" << std::endl;
    std::cout << "   • Add proper entity model handling" << std::endl;
    std::cout << "   • Integrate with audio/video streaming" << std::endl;
    std::cout << "   • Add user interface for control" << std::endl;
    std::cout << "   • Implement device-specific features" << std::endl;
    
    return 0;
}

// ============================================================================
// BUILD INSTRUCTIONS
// ============================================================================

/*
To build this example:

Windows (Visual Studio):
1. Open VS Developer Command Prompt
2. cl /EHsc /I..\ example_avdecc_controller.cpp ..\1722_1_state_machines_refactored.cpp ws2_32.lib

Linux/WSL:
1. g++ -std=c++11 -I../ -o example_controller example_avdecc_controller.cpp ../1722_1_state_machines_refactored.cpp -lpthread

CMake (cross-platform):
Add to CMakeLists.txt:
```cmake
add_executable(example_controller 
    example_avdecc_controller.cpp 
    1722_1_state_machines_refactored.cpp)
target_include_directories(example_controller PRIVATE .)
if(WIN32)
    target_link_libraries(example_controller ws2_32)
else()
    target_link_libraries(example_controller pthread)
endif()
```

To run:
./example_controller

Expected output:
- Library initialization messages
- Network interface setup
- Discovery attempts
- Simulated entity responses
- Stream connection attempts
- Clean shutdown

Note: This example uses simulated networking. For real AVDECC operation, 
implement actual raw socket communication with Ethernet frames.
*/
