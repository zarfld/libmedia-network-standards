# Minimal CMakeLists.txt for IEEE 1722.1-2021 Complete Implementation
cmake_minimum_required(VERSION 3.16)

project(IEEE_1722_1_2021_Complete
    VERSION 1.0.0
    DESCRIPTION "IEEE 1722.1-2021 Complete Standards Implementation"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Create the IEEE 1722.1-2021 Complete Standards Implementation Library
add_library(ieee_1722_1_2021_complete STATIC
    ieee_1722_1_2021_clean.cpp
    ieee_1722_1_2021_clean.h
    ieee_1722_1_2021_complete_entity.cpp
    ieee_1722_1_2021_complete_entity.h
    ieee_1722_1_2021_aecp_windows_compatible.cpp
    ieee_1722_1_2021_acmp_implementation.cpp
)

# Add IEEE 1722.1-2021 AECP Protocol Library (Windows Compatible)
add_library(ieee_1722_1_2021_aecp STATIC
    ieee_1722_1_2021_base.h
    ieee_1722_1_2021_aecp_windows_compatible.cpp
)

# Add IEEE 1722.1-2021 ACMP Protocol Library
add_library(ieee_1722_1_2021_acmp STATIC
    ieee_1722_1_2021_base.h
    ieee_1722_1_2021_acmp_implementation.cpp
)

# Create the IEEE 1722-2016 AVTP Implementation Library
add_library(ieee_1722_2016_avtp STATIC
    ieee_1722_2016_streaming.cpp
    ieee_1722_2016_streaming.h
)

# Create the IEEE 1722.1-2013 Complete Implementation Library (EIGENSTÄNDIG)
add_library(ieee_1722_1_2013_complete STATIC
    ieee_1722_1_2013_complete.cpp
    ieee_1722_1_2013_complete.h
)

# Set include directories
target_include_directories(ieee_1722_1_2021_complete PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set include directories for IEEE 1722-2016
target_include_directories(ieee_1722_2016_avtp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set include directories for IEEE 1722.1-2013 (EIGENSTÄNDIG)
target_include_directories(ieee_1722_1_2013_complete PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add compiler definitions
target_compile_definitions(ieee_1722_1_2021_complete PUBLIC
    IEEE_1722_1_2021_COMPLETE_AEM_SUPPORT=1
    IEEE_1722_1_2021_ALL_75_COMMANDS=1
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(ieee_1722_1_2021_complete PRIVATE ws2_32)
    target_compile_definitions(ieee_1722_1_2021_complete PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    # Platform-specific libraries for IEEE 1722-2016
    target_link_libraries(ieee_1722_2016_avtp PRIVATE ws2_32)
    target_compile_definitions(ieee_1722_2016_avtp PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    
    # Platform-specific libraries for IEEE 1722.1-2013 (EIGENSTÄNDIG)
    target_link_libraries(ieee_1722_1_2013_complete PRIVATE ws2_32)
    target_compile_definitions(ieee_1722_1_2013_complete PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
endif()

# Create simple test executable
add_executable(test_ieee_complete
    examples/ieee_1722_1_2021_complete_example.cpp
)

target_link_libraries(test_ieee_complete PRIVATE ieee_1722_1_2021_complete)

# Create IEEE 1722-2016 test executable
add_executable(test_ieee_1722_2016
    ieee_1722_2016_test.cpp
)

target_link_libraries(test_ieee_1722_2016 PRIVATE ieee_1722_2016_avtp)

# Create IEEE 1722.1-2013 test executable (EIGENSTÄNDIG)
add_executable(test_ieee_1722_1_2013_complete
    ieee_1722_1_2013_complete_test.cpp
)

target_link_libraries(test_ieee_1722_1_2013_complete PRIVATE ieee_1722_1_2013_complete)

# Create IEEE 1722.1-2013 simple test executable (EIGENSTÄNDIG)
add_executable(test_ieee_1722_1_2013_simple
    ieee_1722_1_2013_simple_test.cpp
)

target_link_libraries(test_ieee_1722_1_2013_simple PRIVATE ieee_1722_1_2013_complete)

# Information output
message(STATUS "IEEE 1722.1-2021 Complete Implementation:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Features:")
message(STATUS "    - Complete IEEE 1722.1-2021 AEM support (75 commands)")
message(STATUS "    - Complete IEEE 1722-2016 AVTP transport protocol")
message(STATUS "    - Complete IEEE 1722.1-2013 EIGENSTÄNDIGE implementation")
message(STATUS "    - Entity management and descriptor system")
message(STATUS "    - Cross-platform support")
message(STATUS "  ")
message(STATUS "IEEE Standards Implementiert:")
message(STATUS "  ✅ IEEE 1722.1-2021 (AVDECC): VOLLSTÄNDIG")
message(STATUS "  ✅ IEEE 1722-2016 (AVTP): VOLLSTÄNDIG") 
message(STATUS "  ✅ IEEE 1722.1-2013 (AVDECC 2013): EIGENSTÄNDIG VOLLSTÄNDIG")

# Build examples
add_subdirectory(examples)
