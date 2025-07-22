# Common utilities and protocols
cmake_minimum_required(VERSION 3.16)

add_library(ieee_standards_common STATIC
    industrial_protocols.h
    security_cryptography.h
    # Add .cpp files as they are created
)

target_include_directories(ieee_standards_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(ieee_standards_common PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
endif()

message(STATUS "IEEE Standards common utilities configured")
