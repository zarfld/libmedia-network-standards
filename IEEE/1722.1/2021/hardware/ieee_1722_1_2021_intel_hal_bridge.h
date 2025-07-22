/**
 * @file ieee_1722_1_2021_intel_hal_bridge.h
 * @brief Simple Intel Hardware Bridge for IEEE 1722.1-2021
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace IEEE { namespace _1722_1 { namespace _2021 { namespace Hardware { namespace Intel {

// Forward declarations for Intel HAL types
struct device_t;

/**
 * @brief Simple Intel AVDECC hardware interface for Hive testing
 */
class IntelAVDECCHardwareInterface {
private:
    std::string devicePath_;
    bool isInitialized_;

public:
    explicit IntelAVDECCHardwareInterface(const std::string& devicePath);
    virtual ~IntelAVDECCHardwareInterface();

    bool initialize();
    void cleanup();
    
    // Hardware information
    std::string getHardwareInfo() const;
    bool isHardwareReady() const;
    
    // Simple network interface access
    void* getNetworkInterface() { return this; }  // Return non-null for testing
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
