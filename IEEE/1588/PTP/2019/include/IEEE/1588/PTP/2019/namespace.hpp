/**
 * @file namespace.hpp
 * @brief IEEE 1588-2019 PTP v2.1 Precision Time Protocol namespace definitions
 * 
 * This file defines the core namespace structure for the IEEE 1588-2019 implementation,
 * providing hardware-agnostic precision time protocol functionality with enhanced
 * security, multi-domain support, and enterprise-grade calibration capabilities.
 * 
 * @copyright
 * This implementation follows IEEE 1588-2019 standard specifications.
 * All standard-defined structures, algorithms, and behaviors are implemented
 * in accordance with IEEE copyright and patent policies.
 * 
 * @note Implementation Compliance
 * - Follows IEEE 1588-2019 standard specifications exactly
 * - Hardware-agnostic implementation suitable for cross-platform deployment
 * - Compatible with existing 802.1AS gPTP implementations
 * - Provides enhanced enterprise-grade features beyond basic gPTP
 * 
 * @version 1.0.0
 * @date 2025-09-20
 */

#pragma once

#include <cstdint>
#include <chrono>
#include <array>
#include <string>
#include <vector>
#include <memory>

/**
 * @namespace IEEE
 * @brief Top-level IEEE standards namespace
 */
namespace IEEE {

/**
 * @namespace IEEE::_1588
 * @brief IEEE 1588 Precision Time Protocol family namespace
 */
namespace _1588 {

/**
 * @namespace IEEE::_1588::PTP
 * @brief Precision Time Protocol implementations
 */
namespace PTP {

/**
 * @namespace IEEE::_1588::PTP::_2019
 * @brief IEEE 1588-2019 PTP v2.1 implementation
 * 
 * This namespace contains the complete implementation of IEEE 1588-2019
 * Precision Time Protocol version 2.1, providing enhanced timing capabilities
 * beyond basic gPTP with enterprise-grade features.
 * 
 * Key Features:
 * - Enhanced precision timing synchronization
 * - Security mechanisms (authentication, authorization, integrity)
 * - Multi-domain support with domain isolation
 * - Enterprise-grade calibration procedures
 * - Hardware abstraction for cross-platform deployment
 * - Management and monitoring protocols
 * - Best Master Clock Algorithm (BMCA) enhancements
 */
namespace _2019 {

// Forward declarations for core PTP classes

/**
 * @brief Base class for all PTP clocks
 */
class PtpClock;

/**
 * @brief Ordinary Clock implementation
 */
class OrdinaryClock;

/**
 * @brief Boundary Clock implementation
 */
class BoundaryClock;

/**
 * @brief Transparent Clock implementation
 */
class TransparentClock;

/**
 * @brief PTP port management
 */
class PtpPort;

/**
 * @brief PTP message processing engine
 */
class MessageProcessor;

/**
 * @brief Best Master Clock Algorithm implementation
 */
class BestMasterClockAlgorithm;

/**
 * @brief Hardware abstraction layer for timing operations
 */
class HardwareAbstraction;

/**
 * @brief Security management for PTP messages
 */
class SecurityManager;

/**
 * @brief Management protocol implementation
 */
class ManagementProtocol;

/**
 * @brief Multi-domain coordination
 */
class DomainManager;

/**
 * @brief Calibration and precision enhancement
 */
class CalibrationEngine;

// Namespace aliases for convenience
namespace Types {
    // Type definitions will be placed here
}

namespace Messages {
    // Message format definitions will be placed here
}

namespace Algorithms {
    // Algorithm implementations will be placed here
}

namespace Security {
    // Security mechanism implementations will be placed here
}

namespace Management {
    // Management protocol implementations will be placed here
}

namespace Hardware {
    // Hardware abstraction implementations will be placed here
}

} // namespace _2019
} // namespace PTP
} // namespace _1588
} // namespace IEEE

/**
 * @brief Convenience alias for the main IEEE 1588-2019 namespace
 */
namespace IEEE1588_2019 = IEEE::_1588::PTP::_2019;