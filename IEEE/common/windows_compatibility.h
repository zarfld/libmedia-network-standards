/**
 * @file windows_compatibility.h
 * @brief Windows Compatibility Header for IEEE Standards
 * @details Prevents socket header conflicts and provides consistent Windows definitions
 * 
 * @author OpenAvnu Standards Team
 * @date 2025
 * @copyright IEEE Standards - Windows compatibility layer
 */

#ifndef IEEE_STANDARDS_WINDOWS_COMPATIBILITY_H
#define IEEE_STANDARDS_WINDOWS_COMPATIBILITY_H

#ifdef _WIN32

// Prevent multiple socket header inclusions
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

// Include Windows headers in correct order
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

// Link against required Windows libraries
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

// Define byte order functions for Windows
#if defined(_MSC_VER)
    #include <stdlib.h>
    #ifndef be64toh
        #define be64toh(x) _byteswap_uint64(x)
        #define htobe64(x) _byteswap_uint64(x)
        #define be32toh(x) _byteswap_ulong(x)
        #define htobe32(x) _byteswap_ulong(x)
        #define be16toh(x) _byteswap_ushort(x)
        #define htobe16(x) _byteswap_ushort(x)
    #endif
#endif

#endif // _WIN32

#endif // IEEE_STANDARDS_WINDOWS_COMPATIBILITY_H
