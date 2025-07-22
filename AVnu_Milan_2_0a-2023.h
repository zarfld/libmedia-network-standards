/**
 * @file AVnu_Milan_2_0a-2023.h
 * @brief AVnu Milan Profile 2.0a-2023 Standard Header
 * 
 * Implementation of AVnu Milan Profile 2.0a-2023 for professional Audio/Video
 * Bridging (AVB) over Time-Sensitive Networks.
 * 
 * This header provides enhancements and extensions to IEEE 1722.1-2021 for:
 * - Professional audio/video streaming requirements
 * - Enhanced discovery and control mechanisms
 * - Milan-specific AVDECC extensions
 * - Interoperability requirements for professional AVB devices
 * 
 * @version 1.0.0
 * @date 2025
 * @copyright OpenAvnu Project
 * @standard AVnu Milan Profile 2.0a-2023
 */

#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

// Cross-platform networking headers
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
#endif

// Dependencies on IEEE standards
#include "1722-2016.h"      // IEEE 1722-2016 AVTP Protocol Implementation
#include "1722_1-2021.h"    // IEEE 1722.1-2021 AVDECC Entity Model

// Using declarations for convenience
using namespace IEEE::_1722::_2016;
using namespace IEEE::_1722_1::_2021;

namespace AVnu {
    namespace Milan {
        namespace _2_0a_2023 {

            /**
             * @brief Milan Profile Version Constants
             */
            constexpr uint8_t MILAN_MAJOR_VERSION = 2;
            constexpr uint8_t MILAN_MINOR_VERSION = 0;
            constexpr char MILAN_VERSION_STRING[] = "2.0a";
            constexpr uint16_t MILAN_PROFILE_ID = 0x2023;

            /**
             * @brief Milan-specific AECP Command Extensions
             * These extend the standard IEEE 1722.1-2021 AECP commands
             */
            namespace AECP {
                
                /**
                 * @brief Milan-specific AECP Command Types
                 */
                enum class MilanAECPCommand : uint16_t {
                    // Standard IEEE commands (inherited)
                    GET_DYNAMIC_INFO = 0x004b,
                    REGISTER_UNSOL_NOTIFICATION = 0x0024,
                    
                    // Milan 2.0a extensions
                    GET_MILAN_INFO = 0x1000,
                    SET_MILAN_INFO = 0x1001,
                    GET_STREAM_STATISTICS = 0x1002,
                    CLEAR_STREAM_STATISTICS = 0x1003
                };

                /**
                 * @brief Milan Device Information Structure
                 */
                struct MilanDeviceInfo {
                    uint8_t milan_version_major;
                    uint8_t milan_version_minor;
                    uint16_t certification_version;
                    uint32_t supported_features;
                    char manufacturer_name[64];
                    char model_name[64];
                } __attribute__((packed));

            } // namespace AECP

            /**
             * @brief Milan-specific ADP Extensions
             */
            namespace ADP {
                
                /**
                 * @brief Milan Capability Flags
                 */
                enum class MilanCapabilities : uint32_t {
                    MILAN_PROFILE_SUPPORT = 0x80000000,
                    REDUNDANCY_SUPPORT = 0x40000000,
                    FAST_CONNECT_SUPPORT = 0x20000000,
                    STREAM_BACKUP_SUPPORT = 0x10000000
                };

            } // namespace ADP

            /**
             * @brief Milan Stream Format Requirements
             */
            namespace StreamFormats {
                
                /**
                 * @brief Milan-compliant audio sample rates
                 */
                enum class AudioSampleRate : uint32_t {
                    SR_44_1_KHZ = 44100,
                    SR_48_KHZ = 48000,
                    SR_88_2_KHZ = 88200,
                    SR_96_KHZ = 96000,
                    SR_176_4_KHZ = 176400,
                    SR_192_KHZ = 192000
                };

                /**
                 * @brief Milan-compliant audio channel configurations
                 */
                enum class AudioChannels : uint8_t {
                    MONO = 1,
                    STEREO = 2,
                    QUAD = 4,
                    SURROUND_5_1 = 6,
                    SURROUND_7_1 = 8
                };

            } // namespace StreamFormats

            /**
             * @brief Milan Timing and Synchronization Requirements
             */
            namespace Timing {
                
                /**
                 * @brief Milan gPTP synchronization requirements
                 */
                constexpr uint32_t MAX_SYNC_UNCERTAINTY_NS = 1000;  // 1 microsecond
                constexpr uint32_t MAX_PATH_TRACE_HOPS = 8;
                constexpr uint32_t SYNC_TIMEOUT_MS = 2000;

                /**
                 * @brief Milan stream presentation time requirements
                 */
                constexpr uint32_t MIN_PRESENTATION_TIME_OFFSET_NS = 125000;  // 125 microseconds
                constexpr uint32_t MAX_PRESENTATION_TIME_OFFSET_NS = 2000000; // 2 milliseconds

            } // namespace Timing

        } // namespace _2_0a_2023
    } // namespace Milan
} // namespace AVnu
