cmake_minimum_required(VERSION 3.16)

# IEEE 802.1AS-2021 Pure Standard Implementation Library
project(ieee_802_1as_2021_pure_standard VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files for the pure standard implementation
set(IEEE_802_1AS_2021_PURE_SOURCES
    ieee_802_1as_2021_fixed.h
    time_sync_engine_fixed.h
    time_sync_engine_clean.cpp
)

# Create the pure standard library
add_library(ieee_802_1as_2021_pure_standard STATIC ${IEEE_802_1AS_2021_PURE_SOURCES})

# Set target properties
set_target_properties(ieee_802_1as_2021_pure_standard PROPERTIES
    OUTPUT_NAME "ieee_802_1as_2021_pure"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/Standards/IEEE/802.1AS/2021"
)

# Include directories
target_include_directories(ieee_802_1as_2021_pure_standard PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler specific settings
if(MSVC)
    target_compile_options(ieee_802_1as_2021_pure_standard PRIVATE
        /W4
        /wd4996  # Disable deprecated function warnings
        /bigobj  # Handle large object files
    )
    target_compile_definitions(ieee_802_1as_2021_pure_standard PRIVATE
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
else()
    target_compile_options(ieee_802_1as_2021_pure_standard PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

# Define preprocessor macros
target_compile_definitions(ieee_802_1as_2021_pure_standard PRIVATE
    IEEE_802_1AS_2021_PURE_STANDARD=1
)

# Installation
install(TARGETS ieee_802_1as_2021_pure_standard
    ARCHIVE DESTINATION lib/Standards/IEEE/802.1AS/2021
    COMPONENT Standards
)

install(FILES
    ieee_802_1as_2021_fixed.h
    time_sync_engine_fixed.h
    DESTINATION include/Standards/IEEE/802.1AS/2021
    COMPONENT Standards-Headers
)

# Package information
set(CPACK_COMPONENT_STANDARDS_DISPLAY_NAME "IEEE 802.1AS-2021 Pure Standard Library")
set(CPACK_COMPONENT_STANDARDS_DESCRIPTION "Pure IEEE 802.1AS-2021 implementation without hardware dependencies")
set(CPACK_COMPONENT_STANDARDS-HEADERS_DISPLAY_NAME "IEEE 802.1AS-2021 Headers")
set(CPACK_COMPONENT_STANDARDS-HEADERS_DESCRIPTION "Header files for IEEE 802.1AS-2021 standard library")

# Create alias for easier use
add_library(IEEE::802.1AS::2021::Pure ALIAS ieee_802_1as_2021_pure_standard)

message(STATUS "IEEE 802.1AS-2021 Pure Standard Library configured")
message(STATUS "  - No hardware dependencies")
message(STATUS "  - Abstract interfaces only")
message(STATUS "  - Pure standard implementation")
message(STATUS "  - Output: ${CMAKE_BINARY_DIR}/lib/Standards/IEEE/802.1AS/2021/libieee_802_1as_2021_pure.a")
