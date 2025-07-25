# AVnu Milan 1.2-2023 Implementation
cmake_minimum_required(VERSION 3.16)

add_library(avnu_milan_1_2_2023 STATIC
    AVnu_Milan_1_2-2023_complete.cpp
)

target_include_directories(avnu_milan_1_2_2023 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../IEEE/1722.1/2021/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../IEEE/1722.1/2021/library
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
)

# Link to IEEE 1722.1-2021 if available
if(TARGET ieee_1722_1_2021_complete)
    target_link_libraries(avnu_milan_1_2_2023 PUBLIC ieee_1722_1_2021_complete)
endif()

# Platform-specific settings
if(WIN32)
    target_link_libraries(avnu_milan_1_2_2023 PRIVATE ws2_32)
    
    target_compile_definitions(avnu_milan_1_2_2023 PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        AVNU_MILAN_1_2_2023_SUPPORT=1
    )
endif()

# Test executable for Milan implementation
add_executable(milan_1_2_2023_test
    milan_1_2_2023_test.cpp
)

target_link_libraries(milan_1_2_2023_test 
    PRIVATE avnu_milan_1_2_2023
)

target_include_directories(milan_1_2_2023_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

message(STATUS "AVnu Milan 1.2-2023 library configured")
