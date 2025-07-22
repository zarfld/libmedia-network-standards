# IEEE 1722-2016 AVTP Implementation
cmake_minimum_required(VERSION 3.16)

# Core AVTP library
add_library(ieee_1722_2016_core STATIC
    core/ieee_1722_2016.cpp
    core/ieee_1722_2016_impl.cpp
)

target_include_directories(ieee_1722_2016_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
)

# Streaming AVTP library
add_library(ieee_1722_2016_streaming STATIC
    streaming/ieee_1722_2016_streaming.cpp
    streaming/ieee_1722_2016_streaming_clean.cpp
    streaming/ieee_1722_2016_streaming_simple.cpp
    streaming/ieee_1722_2016_streaming_fixed.cpp
)

target_include_directories(ieee_1722_2016_streaming PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/streaming
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
)

target_link_libraries(ieee_1722_2016_streaming PUBLIC ieee_1722_2016_core)

# Platform-specific settings
if(WIN32)
    target_link_libraries(ieee_1722_2016_core PRIVATE ws2_32)
    target_link_libraries(ieee_1722_2016_streaming PRIVATE ws2_32)
    
    target_compile_definitions(ieee_1722_2016_core PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    target_compile_definitions(ieee_1722_2016_streaming PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
endif()

# Tests (if enabled)
if(BUILD_TESTS)
    add_executable(ieee_1722_2016_test
        tests/ieee_1722_2016_test.cpp
    )
    
    target_link_libraries(ieee_1722_2016_test PRIVATE 
        ieee_1722_2016_core 
        ieee_1722_2016_streaming
    )
    
    # Phase 4 tests
    add_executable(ieee_1722_2016_phase4_test
        tests/ieee_1722_2016_phase4_tests.h
    )
    
    target_link_libraries(ieee_1722_2016_phase4_test PRIVATE 
        ieee_1722_2016_core 
        ieee_1722_2016_streaming
    )
endif()

message(STATUS "IEEE 1722-2016 AVTP libraries configured:")
