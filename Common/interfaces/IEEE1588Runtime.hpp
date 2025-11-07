/**
 * @file IEEE1588Runtime.hpp
 * @brief Runtime helpers to choose IEEE 1588 provider via env/config
 */

#ifndef STANDARDS_INTERFACES_IEEE1588_RUNTIME_HPP
#define STANDARDS_INTERFACES_IEEE1588_RUNTIME_HPP

#include <cstdlib>
#include "IEEE1588Selector.hpp"

namespace Standards {
namespace Interfaces {

struct IEEE1588Runtime {
    // Environment variable used to select version at runtime
    // Accepts: latest | 2019 | 2008 | 2002 (case-insensitive)
    static constexpr const char* kEnvVar = "STANDARDS_IEEE1588_VERSION";

    // Read environment and apply selection; returns selected version
    static IEEE1588Selector::Version applyEnvSelection() noexcept {
        const char* v = std::getenv(kEnvVar);
        auto ver = IEEE1588Selector::parse(v);
        IEEE1588Selector::setSelectedVersion(ver);
        return ver;
    }

    // Apply from string property (e.g., config file value)
    static IEEE1588Selector::Version applyProperty(const char* v) noexcept {
        auto ver = IEEE1588Selector::parse(v);
        IEEE1588Selector::setSelectedVersion(ver);
        return ver;
    }
};

} // namespace Interfaces
} // namespace Standards

#endif // STANDARDS_INTERFACES_IEEE1588_RUNTIME_HPP
