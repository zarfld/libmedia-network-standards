# Integration utilities for cross-standard functionality
cmake_minimum_required(VERSION 3.16)

add_library(ieee_standards_integration STATIC
    simple_integration.cpp
    test_standards_build.cpp
    # Additional integration files as needed
)

target_include_directories(ieee_standards_integration PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../IEEE/802.1/Q/2020
    ${CMAKE_CURRENT_SOURCE_DIR}/../Common
)

# Link to required libraries
if(TARGET ieee_802_1q_2020)
    target_link_libraries(ieee_standards_integration PUBLIC ieee_802_1q_2020)
endif()

if(TARGET ieee_standards_common)
    target_link_libraries(ieee_standards_integration PUBLIC ieee_standards_common)
endif()

# Platform-specific settings
if(WIN32)
    target_link_libraries(ieee_standards_integration PRIVATE ws2_32)
    
    target_compile_definitions(ieee_standards_integration PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
endif()

message(STATUS "IEEE Standards integration utilities configured")

# Minimal test for IEEE 1588 runtime selection (header-only, mocks)
if(BUILD_TESTS)
    add_executable(test_ieee1588_selector
        test_ieee1588_selector.cpp
    )
    target_include_directories(test_ieee1588_selector PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    )
    if(TARGET standards_interfaces)
        target_link_libraries(test_ieee1588_selector PRIVATE standards_interfaces)
    endif()
    if(TARGET ieee_standards_common)
        target_link_libraries(test_ieee1588_selector PRIVATE ieee_standards_common)
    endif()
endif()
