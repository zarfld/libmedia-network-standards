# IEEE 1722.1-2013 Legacy AVDECC Implementation
cmake_minimum_required(VERSION 3.16)

# Core legacy AVDECC library
add_library(ieee_1722_1_2013_legacy STATIC
    core/ieee_1722_1_2013_legacy.cpp
    core/ieee_1722_1_2013.cpp
    core/ieee_1722_1_2013_complete.cpp
)

target_include_directories(ieee_1722_1_2013_legacy PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(ieee_1722_1_2013_legacy PRIVATE ws2_32)
    
    target_compile_definitions(ieee_1722_1_2013_legacy PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        IEEE_1722_1_2013_LEGACY_SUPPORT=1
    )
endif()

# Tests (if enabled)
if(BUILD_TESTS)
    add_executable(ieee_1722_1_2013_test
        tests/ieee_1722_1_2013_test.cpp
    )
    
    target_link_libraries(ieee_1722_1_2013_test PRIVATE ieee_1722_1_2013_legacy)
    
    add_executable(ieee_1722_1_2013_complete_test
        tests/ieee_1722_1_2013_complete_test.cpp
    )
    
    target_link_libraries(ieee_1722_1_2013_complete_test PRIVATE ieee_1722_1_2013_legacy)
    
    add_executable(ieee_1722_1_2013_simple_test
        tests/ieee_1722_1_2013_simple_test.cpp
    )
    
    target_link_libraries(ieee_1722_1_2013_simple_test PRIVATE ieee_1722_1_2013_legacy)
    
    add_executable(test_1722_1_2013
        tests/test_1722_1_2013.cpp
    )
    
    target_link_libraries(test_1722_1_2013 PRIVATE ieee_1722_1_2013_legacy)
endif()

message(STATUS "IEEE 1722.1-2013 Legacy AVDECC library configured")
