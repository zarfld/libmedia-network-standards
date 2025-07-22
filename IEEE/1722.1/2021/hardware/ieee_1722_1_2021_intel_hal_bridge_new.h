/**
 * @file ieee_1722_1_2021_intel_hal_bridge.h
 * @brief Hardware Integration Bridge for IEEE 1722.1-2021 with Intel AVB HAL
 * @details Layer 3: Intel hardware integration for AVDECC protocol implementation
 * 
 * This module provides the hardware abstraction layer bridge between the IEEE 1722.1-2021
 * state machines and Intel Ethernet controllers with AVB/TSN capabilities.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include "ieee_1722_1_2021_library.h"

namespace IEEE { namespace _1722_1 { namespace _2021 { namespace Hardware { namespace Intel {

// Forward declarations for Intel HAL types
struct device_t;
struct intel_packet;
struct intel_dma_alloc;

/**
 * @brief Intel network interface implementation for AVDECC
 */
class IntelNetworkInterface : public ::IEEE::_1722_1::_2021::StateMachines::INetworkInterface {
private:
    device_t* device_;
    std::string devicePath_;
    bool isInitialized_;

public:
    explicit IntelNetworkInterface(const std::string& devicePath);
    virtual ~IntelNetworkInterface();

    // INetworkInterface implementation
    bool initialize() override;
    void cleanup() override;
    bool sendPacket(const uint8_t* data, size_t length) override;
    bool receivePacket(uint8_t* buffer, size_t& length) override;
    ::IEEE::_1722_1::_2021::Core::MacAddress getLocalMacAddress() override;
    bool isLinkUp() override;

    // Intel-specific methods
    device_t* getDevice() { return device_; }
    const std::string& getDevicePath() const { return devicePath_; }
};

/**
 * @brief Intel hardware timer manager for AVDECC timing requirements
 */
class IntelHardwareTimerManager : public ::IEEE::_1722_1::_2021::StateMachines::ITimerManager {
private:
    device_t* device_;
    bool isInitialized_;

public:
    explicit IntelHardwareTimerManager(device_t* device);
    virtual ~IntelHardwareTimerManager();

    // ITimerManager implementation
    bool initialize() override;
    void cleanup() override;
    uint64_t getCurrentTime() override;
    void scheduleTimer(uint32_t timerId, uint32_t timeoutMs) override;
    void cancelTimer(uint32_t timerId) override;

    // Intel-specific methods
    uint64_t getHardwareTimestamp();
    bool synchronizeWithgPTP();
};

/**
 * @brief Complete Intel AVDECC hardware interface
 */
class IntelAVDECCHardwareInterface {
private:
    std::unique_ptr<IntelNetworkInterface> networkInterface_;
    std::unique_ptr<IntelHardwareTimerManager> timerManager_;
    device_t* device_;
    std::string devicePath_;
    bool isInitialized_;

public:
    explicit IntelAVDECCHardwareInterface(const std::string& devicePath);
    virtual ~IntelAVDECCHardwareInterface();

    bool initialize();
    void cleanup();
    
    // Component access
    ::IEEE::_1722_1::_2021::StateMachines::INetworkInterface* getNetworkInterface();
    ::IEEE::_1722_1::_2021::StateMachines::ITimerManager* getTimerManager();
    
    // Hardware information
    std::string getHardwareInfo() const;
    bool isHardwareReady() const;
    
    // Intel-specific features
    bool enableTSNFeatures();
    bool configureQoS();
    device_t* getDevice() { return device_; }
};

/**
 * @brief Utility functions for Intel hardware validation
 */
bool detectIntelAVBDevices(std::vector<std::string>& devicePaths);
bool validateIntelDeviceForAVDECC(const std::string& devicePath);
std::string getIntelDeviceInfo(const std::string& devicePath);

} // namespace Intel
} // namespace Hardware
} // namespace _2021
} // namespace _1722_1
} // namespace IEEE
