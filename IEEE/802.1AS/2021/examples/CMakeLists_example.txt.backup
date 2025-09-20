cmake_minimum_required(VERSION 3.16)

# IEEE 802.1AS-2021 Pure Standard Integration Example
project(ieee_802_1as_2021_integration_example VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the pure standard library
find_library(IEEE_802_1AS_2021_PURE_LIB 
    NAMES ieee_802_1as_2021_pure
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../build_pure/lib/Standards/IEEE/802.1AS/2021/Release"
    NO_DEFAULT_PATH
)

if(NOT IEEE_802_1AS_2021_PURE_LIB)
    message(STATUS "Pure library not found, will build it first...")
    
    # Build the pure standard library first
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" "${CMAKE_BINARY_DIR}/pure_lib")
    set(IEEE_802_1AS_2021_PURE_TARGET ieee_802_1as_2021_pure_standard)
else()
    message(STATUS "Found pure library: ${IEEE_802_1AS_2021_PURE_LIB}")
    add_library(ieee_802_1as_2021_pure_standard STATIC IMPORTED)
    set_target_properties(ieee_802_1as_2021_pure_standard PROPERTIES
        IMPORTED_LOCATION "${IEEE_802_1AS_2021_PURE_LIB}"
    )
    set(IEEE_802_1AS_2021_PURE_TARGET ieee_802_1as_2021_pure_standard)
endif()

# Source files for the integration example
set(INTEGRATION_EXAMPLE_SOURCES
    pure_standard_integration_example.cpp
)

# Create the integration example executable
add_executable(ieee_802_1as_2021_integration_example ${INTEGRATION_EXAMPLE_SOURCES})

# Link with the pure standard library
target_link_libraries(ieee_802_1as_2021_integration_example 
    PRIVATE ${IEEE_802_1AS_2021_PURE_TARGET}
)

# Include directories
target_include_directories(ieee_802_1as_2021_integration_example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../../build_pure
)

# Compiler specific settings
if(MSVC)
    target_compile_options(ieee_802_1as_2021_integration_example PRIVATE
        /W4
        /wd4996  # Disable deprecated function warnings
    )
    target_compile_definitions(ieee_802_1as_2021_integration_example PRIVATE
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
else()
    target_compile_options(ieee_802_1as_2021_integration_example PRIVATE
        -Wall -Wextra -Wpedantic
        -pthread
    )
    target_link_libraries(ieee_802_1as_2021_integration_example PRIVATE pthread)
endif()

# Set output directory
set_target_properties(ieee_802_1as_2021_integration_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
)

# Installation
install(TARGETS ieee_802_1as_2021_integration_example
    RUNTIME DESTINATION bin/examples
    COMPONENT Examples
)

install(FILES pure_standard_integration_example.cpp
    DESTINATION share/examples/ieee_802_1as_2021
    COMPONENT Examples-Source
)

# Package information
set(CPACK_COMPONENT_EXAMPLES_DISPLAY_NAME "IEEE 802.1AS-2021 Integration Examples")
set(CPACK_COMPONENT_EXAMPLES_DESCRIPTION "Integration examples showing pure standards + hardware interfaces")
set(CPACK_COMPONENT_EXAMPLES-SOURCE_DISPLAY_NAME "Example Source Code")
set(CPACK_COMPONENT_EXAMPLES-SOURCE_DESCRIPTION "Source code for integration examples")

message(STATUS "IEEE 802.1AS-2021 Integration Example configured")
message(STATUS "  - Uses pure standards library")
message(STATUS "  - Demonstrates proper architecture")  
message(STATUS "  - Mock hardware implementations")
message(STATUS "  - Output: ${CMAKE_BINARY_DIR}/examples/ieee_802_1as_2021_integration_example${CMAKE_EXECUTABLE_SUFFIX}")
