/**
 * @file ieee_1722_1_2021_windows.h
 * @brief Windows compatibility layer for IEEE 1722.1-2021
 * @details Proper Windows header inclusion to avoid C/C++ linkage conflicts
 */

#pragma once

// Only include Windows headers when absolutely necessary
#ifdef _WIN32

// Ensure we include C++ headers first to avoid linkage conflicts
#ifndef __cplusplus
#error "This header requires C++ compilation"
#endif

// Windows headers in proper order
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

// Include Windows headers with C++ linkage - NO extern "C" needed for STL compatibility
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Windows-specific utility functions can be C++ functions
// No extern "C" wrapper needed - this prevents STL template linkage conflicts

#else
// Non-Windows platforms
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif // _WIN32
