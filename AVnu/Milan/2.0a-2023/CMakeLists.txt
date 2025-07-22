# AVnu Milan 2.0a-2023 Implementation
cmake_minimum_required(VERSION 3.16)

add_library(avnu_milan_2_0a_2023 INTERFACE
    # Note: This is currently a header-only library
    # Implementation files will be added as they are created
)

target_include_directories(avnu_milan_2_0a_2023 INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../IEEE/1722.1/2021/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../IEEE/1722.1/2021/library
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
)

# Link to IEEE 1722.1-2021 if available
if(TARGET ieee_1722_1_2021_complete)
    target_link_libraries(avnu_milan_2_0a_2023 INTERFACE ieee_1722_1_2021_complete)
endif()

# Platform-specific settings
if(WIN32)
    target_compile_definitions(avnu_milan_2_0a_2023 INTERFACE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        AVNU_MILAN_2_0A_2023_SUPPORT=1
    )
endif()

message(STATUS "AVnu Milan 2.0a-2023 library configured")
