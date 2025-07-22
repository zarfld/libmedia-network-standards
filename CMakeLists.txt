# IEEE 1722 Standards Library - Hierarchical CMake Configuration
cmake_minimum_required(VERSION 3.16)

project(IEEE1722Standards 
    VERSION 1.0.0 
    DESCRIPTION "IEEE 1722 Standards Implementation Library"
    LANGUAGES CXX)

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

# Build options
option(BUILD_IEEE_1722_2016 "Build IEEE 1722-2016 AVTP implementation" ON)
option(BUILD_IEEE_1722_1_2013 "Build IEEE 1722.1-2013 legacy AVDECC implementation" ON)
option(BUILD_IEEE_1722_1_2021 "Build IEEE 1722.1-2021 modern AVDECC implementation" ON)
option(BUILD_IEEE_802_1AS_2021 "Build IEEE 802.1AS-2021 gPTP implementation" ON)
option(BUILD_AVNU_MILAN "Build AVnu Milan extensions" ON)
option(BUILD_EXAMPLES "Build example applications" ON)
option(BUILD_TESTS "Build test applications" ON)
# Include directories for hierarchical structure
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/IEEE/1722/2016/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/IEEE/1722.1/2013/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/IEEE/1722.1/2021/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/IEEE/1722.1/2021/library)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/IEEE/802.1AS/2021/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/AVnu/Milan/1.2-2023)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common)

# Add subdirectories based on build options
if(BUILD_IEEE_1722_2016)
    add_subdirectory(IEEE/1722/2016)
endif()

if(BUILD_IEEE_1722_1_2013)
    add_subdirectory(IEEE/1722.1/2013)
endif()

if(BUILD_IEEE_1722_1_2021)
    add_subdirectory(IEEE/1722.1/2021)
endif()

if(BUILD_IEEE_802_1AS_2021)
    add_subdirectory(IEEE/802.1AS/2021)
endif()

if(BUILD_AVNU_MILAN)
    add_subdirectory(AVnu/Milan)
endif()

add_subdirectory(Common)
add_subdirectory(Integration)

# Summary
message(STATUS "IEEE 1722 Standards Library Configuration:")
message(STATUS "  IEEE 1722-2016 (AVTP): ${BUILD_IEEE_1722_2016}")
message(STATUS "  IEEE 1722.1-2013 (Legacy AVDECC): ${BUILD_IEEE_1722_1_2013}")
message(STATUS "  IEEE 1722.1-2021 (Modern AVDECC): ${BUILD_IEEE_1722_1_2021}")
message(STATUS "  AVnu Milan Extensions: ${BUILD_AVNU_MILAN}")
message(STATUS "  Examples: ${BUILD_EXAMPLES}")
message(STATUS "  Tests: ${BUILD_TESTS}")

message(STATUS "")
message(STATUS "IEEE Standards Implementation Status:")
message(STATUS "  ✅ IEEE 1722.1-2021 (Modern AVDECC): COMPLETE")
message(STATUS "  ✅ IEEE 1722-2016 (AVTP): COMPLETE") 
message(STATUS "  ✅ IEEE 1722.1-2013 (Legacy AVDECC): COMPLETE")
message(STATUS "  ✅ AVnu Milan Extensions: COMPLETE")
