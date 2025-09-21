# IEEE 1588-2019 PTP v2.1 Implementation
# Hardware-agnostic Precision Time Protocol
# Copyright (C) 2025 OpenAvnu Project

cmake_minimum_required(VERSION 3.16)

project(IEEE1588_2019_PTP
    VERSION 1.0.0
    DESCRIPTION "IEEE 1588-2019 Precision Time Protocol v2.1 Implementation"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Include directories
set(IEEE1588_2019_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files (will be populated as implementation progresses)
set(IEEE1588_2019_SOURCES
    src/clocks.cpp
    # src/types.cpp
    # src/messages.cpp
    # src/algorithms.cpp
    # src/security.cpp
    # src/management.cpp
    # src/hardware.cpp
)

# Header files
set(IEEE1588_2019_HEADERS
    include/IEEE/1588/PTP/2019/ieee1588_2019.hpp
    include/IEEE/1588/PTP/2019/namespace.hpp
    include/IEEE/1588/PTP/2019/types.hpp
    include/IEEE/1588/PTP/2019/messages.hpp
    include/clocks.hpp
    # include/IEEE/1588/PTP/2019/clock.hpp
    # include/IEEE/1588/PTP/2019/algorithms.hpp
    # include/IEEE/1588/PTP/2019/security.hpp
    # include/IEEE/1588/PTP/2019/management.hpp
    # include/IEEE/1588/PTP/2019/hardware.hpp
)

# Create interface library for header-only components
add_library(IEEE1588_2019_interface INTERFACE)
target_include_directories(IEEE1588_2019_interface
    INTERFACE
        $<BUILD_INTERFACE:${IEEE1588_2019_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# Future: Create main library when source files are implemented
# add_library(IEEE1588_2019 ${IEEE1588_2019_SOURCES} ${IEEE1588_2019_HEADERS})
# target_include_directories(IEEE1588_2019
#     PUBLIC
#         $<BUILD_INTERFACE:${IEEE1588_2019_INCLUDE_DIR}>
#         $<INSTALL_INTERFACE:include>
# )
# target_link_libraries(IEEE1588_2019 PUBLIC IEEE1588_2019_interface)

# Testing support
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation
install(
    DIRECTORY ${IEEE1588_2019_INCLUDE_DIR}/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

install(
    TARGETS IEEE1588_2019_interface
    EXPORT IEEE1588_2019Targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Examples (optional build)
option(IEEE1588_2019_BUILD_EXAMPLES "Build IEEE 1588-2019 examples" ON)

if(IEEE1588_2019_BUILD_EXAMPLES)
    # Basic types example
    add_executable(ieee1588_2019_basic_example
        examples/basic_types_example.cpp
    )
    
    # Time-sensitive design example
    add_executable(ieee1588_2019_time_sensitive_example
        examples/time_sensitive_types_example.cpp
    )
    
    # Simple verification test
    add_executable(ieee1588_2019_simple_test
        examples/simple_clock_test.cpp
    )
    
    target_link_libraries(ieee1588_2019_basic_example
        PRIVATE IEEE1588_2019_interface
    )
    
    target_link_libraries(ieee1588_2019_time_sensitive_example
        PRIVATE IEEE1588_2019_interface
    )
    
    target_link_libraries(ieee1588_2019_simple_test
        PRIVATE IEEE1588_2019_interface
    )
    
    # Set output directory
    set_target_properties(ieee1588_2019_basic_example ieee1588_2019_time_sensitive_example ieee1588_2019_simple_test
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
    )
endif()

# Export configuration
install(
    EXPORT IEEE1588_2019Targets
    FILE IEEE1588_2019Targets.cmake
    NAMESPACE IEEE1588_2019::
    DESTINATION lib/cmake/IEEE1588_2019
)

# Create config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    IEEE1588_2019ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/IEEE1588_2019Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/IEEE1588_2019Config.cmake
    INSTALL_DESTINATION lib/cmake/IEEE1588_2019
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/IEEE1588_2019Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/IEEE1588_2019ConfigVersion.cmake
    DESTINATION lib/cmake/IEEE1588_2019
)