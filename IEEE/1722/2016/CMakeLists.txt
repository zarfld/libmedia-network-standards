# IEEE 1722-2016 AVTP Implementation - Production Ready
cmake_minimum_required(VERSION 3.16)

# Production IEEE 1722-2016 Core AVTP library
add_library(ieee_1722_2016_production_core STATIC
    core/avtpdu.cpp
    core/avtpdu.h
)

target_include_directories(ieee_1722_2016_production_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
)

# Set C++ standard and compiler options
target_compile_features(ieee_1722_2016_production_core PUBLIC cxx_std_17)
set_target_properties(ieee_1722_2016_production_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(ieee_1722_2016_production_core PRIVATE ws2_32)
    
    target_compile_definitions(ieee_1722_2016_production_core PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
    )
    
    # MSVC specific options for production quality
    if(MSVC)
        target_compile_options(ieee_1722_2016_production_core PRIVATE
            /W4          # High warning level
            /WX          # Treat warnings as errors
            /permissive- # Disable non-conforming code
            /Zc:__cplusplus # Enable correct __cplusplus value
        )
    endif()
else()
    # GCC/Clang options for production quality
    target_compile_options(ieee_1722_2016_production_core PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Werror      # Treat warnings as errors
        -Wconversion # Warn about type conversions
        -Wsign-conversion
    )
endif()

# Legacy libraries (keep for compatibility - marked as deprecated)
add_library(ieee_1722_2016_core STATIC
    core/ieee_1722_2016.cpp
    core/ieee_1722_2016_impl.cpp
)

target_include_directories(ieee_1722_2016_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
)

# Note: Legacy streaming libraries have build conflicts - disabled for now
# add_library(ieee_1722_2016_streaming STATIC
#     streaming/ieee_1722_2016_streaming.cpp
#     streaming/ieee_1722_2016_streaming_clean.cpp
#     streaming/ieee_1722_2016_streaming_simple.cpp
#     streaming/ieee_1722_2016_streaming_fixed.cpp
# )

# Platform-specific settings for legacy
if(WIN32)
    target_link_libraries(ieee_1722_2016_core PRIVATE ws2_32)
    
    target_compile_definitions(ieee_1722_2016_core PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
endif()

# Tests (if enabled) - Use production implementation
if(BUILD_TESTS)
    # Production test suite
    add_executable(ieee_1722_2016_production_test
        tests/ieee_1722_2016_production_tests.cpp
    )
    
    target_link_libraries(ieee_1722_2016_production_test PRIVATE 
        ieee_1722_2016_production_core
    )
    
    target_compile_features(ieee_1722_2016_production_test PRIVATE cxx_std_17)
    
    # Legacy test (may have issues)
    add_executable(ieee_1722_2016_legacy_test
        tests/ieee_1722_2016_test.cpp
    )
    
    target_link_libraries(ieee_1722_2016_legacy_test PRIVATE 
        ieee_1722_2016_core
    )
endif()

message(STATUS "IEEE 1722-2016 AVTP libraries configured:")
message(STATUS "  - ieee_1722_2016_production_core: Production-ready implementation")
message(STATUS "  - ieee_1722_2016_core: Legacy implementation (compatibility)")
if(BUILD_TESTS)
    message(STATUS "  - ieee_1722_2016_production_test: Comprehensive test suite")
    message(STATUS "  - ieee_1722_2016_legacy_test: Legacy tests")
endif()
