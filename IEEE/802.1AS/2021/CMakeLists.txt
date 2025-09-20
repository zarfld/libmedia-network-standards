# IEEE 802.1AS-2021 Standard Implementation
cmake_minimum_required(VERSION 3.16)

project(IEEE802_1AS_2021
    VERSION 1.0.0
    DESCRIPTION "IEEE 802.1AS-2021 gPTP Implementation"
    LANGUAGES CXX)

# Set library version variables for config file
set(ieee_802_1as_2021_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(ieee_802_1as_2021_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(ieee_802_1as_2021_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(ieee_802_1as_2021_VERSION ${PROJECT_VERSION})

# Set C++ standard (IEEE 802.1AS-2021 requires modern C++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include required CMake modules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Build configuration
option(IEEE_802_1AS_2021_BUILD_TESTS "Build IEEE 802.1AS-2021 tests" ON)
option(IEEE_802_1AS_2021_BUILD_EXAMPLES "Build IEEE 802.1AS-2021 examples" ON)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DNOMINMAX)  # Prevent Windows min/max macros
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)

# Source files
set(IEEE_802_1AS_2021_SOURCES
    core/ieee_802_1as_2021.cpp
    core/time_sync_engine.cpp
    core/performance_measurement.cpp
    core/conformity_test_framework.cpp
    core/message_validation.cpp
)

set(IEEE_802_1AS_2021_HEADERS
    core/ieee_802_1as_2021.h
    core/time_sync_engine.h
    core/performance_measurement.h
    core/conformity_test_framework.h
    core/message_validation.h
)

# Create the main library
add_library(ieee_802_1as_2021_core STATIC
    ${IEEE_802_1AS_2021_SOURCES}
    ${IEEE_802_1AS_2021_HEADERS}
)

# Set target properties
set_target_properties(ieee_802_1as_2021_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# Link libraries
if(WIN32)
    target_link_libraries(ieee_802_1as_2021_core PRIVATE ws2_32)
endif()

# Export include directories
target_include_directories(ieee_802_1as_2021_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
    $<INSTALL_INTERFACE:include/ieee_802_1as_2021>
)

# Create export target for installation
install(TARGETS ieee_802_1as_2021_core EXPORT ieee_802_1as_2021-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ieee_802_1as_2021
)

# Add tests
if(IEEE_802_1AS_2021_BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Add examples - REMOVED: Duplicate add_subdirectory call
# The examples are added later with a unique binary directory (line ~137)

# Add state machines
add_subdirectory(state_machines)

# Installation (headers only, targets installed above)
install(FILES ${IEEE_802_1AS_2021_HEADERS}
    DESTINATION include/ieee_802_1as_2021
)

# Package configuration
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ieee_802_1as_2021-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

export(TARGETS ieee_802_1as_2021_core
    FILE "${CMAKE_CURRENT_BINARY_DIR}/ieee_802_1as_2021-targets.cmake"
)

# Create config file
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ieee_802_1as_2021-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ieee_802_1as_2021-config.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ieee_802_1as_2021-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ieee_802_1as_2021-config-version.cmake"
    DESTINATION lib/cmake/ieee_802_1as_2021
)

install(EXPORT ieee_802_1as_2021-targets
    FILE ieee_802_1as_2021-targets.cmake
    DESTINATION lib/cmake/ieee_802_1as_2021
)

# Add examples subdirectory with unique binary directory
if(IEEE_802_1AS_2021_BUILD_EXAMPLES)
    add_subdirectory(examples ieee_802_1as_2021_examples_build)
    message(STATUS "IEEE 802.1AS-2021 Integration Example: ENABLED")
else()
    message(STATUS "IEEE 802.1AS-2021 Integration Example: DISABLED")
endif()

# Summary
message(STATUS "")
message(STATUS "IEEE 802.1AS-2021 Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Tests: ${IEEE_802_1AS_2021_BUILD_TESTS}")
message(STATUS "  Build Examples: ${IEEE_802_1AS_2021_BUILD_EXAMPLES}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
