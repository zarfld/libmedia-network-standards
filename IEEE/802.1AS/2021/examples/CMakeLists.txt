# IEEE 802.1AS-2021 Examples
cmake_minimum_required(VERSION 3.16)

project(IEEE802_1AS_2021_Examples
    VERSION 1.0.0
    DESCRIPTION "IEEE 802.1AS-2021 Examples and Demonstrations"
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include parent directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Find required libraries
find_library(IEEE_802_1AS_2021_CORE_LIB ieee_802_1as_2021_core 
    PATHS ${CMAKE_CURRENT_BINARY_DIR}/../)

# Complete Integration Example
add_executable(ieee_802_1as_2021_integration_example
    ieee_802_1as_2021_integration_example.cpp
)

target_include_directories(ieee_802_1as_2021_integration_example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
)

target_link_libraries(ieee_802_1as_2021_integration_example
    PRIVATE ieee_802_1as_2021_core
)

# Platform-specific linking
if(WIN32)
    target_link_libraries(ieee_802_1as_2021_integration_example 
        PRIVATE ws2_32
    )
elseif(UNIX AND NOT APPLE)
    target_link_libraries(ieee_802_1as_2021_integration_example 
        PRIVATE pthread
    )
endif()

# Set executable properties
set_target_properties(ieee_802_1as_2021_integration_example PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME "ieee_802_1as_2021_integration_example"
)

# Compiler-specific settings  
target_compile_options(ieee_802_1as_2021_integration_example PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -pedantic>
)

# Install targets
install(TARGETS ieee_802_1as_2021_integration_example
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT examples
)
