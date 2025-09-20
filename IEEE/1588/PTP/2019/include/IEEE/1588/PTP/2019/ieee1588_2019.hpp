/**
 * @file ieee1588_2019.hpp
 * @brief IEEE 1588-2019 PTP v2.1 main include header
 * 
 * This is the main header file for the IEEE 1588-2019 implementation.
 * Include this file to access all IEEE 1588-2019 PTP functionality.
 * 
 * @copyright
 * This implementation follows IEEE 1588-2019 standard specifications.
 * 
 * @version 1.0.0
 * @date 2025-09-20
 */

#pragma once

// Core type definitions
#include "types.hpp"

// Forward declaration of implementation files that will be created
// #include "messages.hpp"        // PTP message formats
// #include "clock.hpp"           // Clock implementations  
// #include "algorithms.hpp"      // BMCA and timing algorithms
// #include "security.hpp"        // Security mechanisms
// #include "management.hpp"      // Management protocol
// #include "hardware.hpp"        // Hardware abstraction

/**
 * @brief Convenience alias for the main IEEE 1588-2019 namespace
 */
namespace IEEE1588_2019 = IEEE::_1588::PTP::_2019;