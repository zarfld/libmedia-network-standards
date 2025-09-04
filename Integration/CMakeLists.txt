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
