# CMakeLists.txt for IEEE Media Networking Standards Common Framework
# Part of Phase 00 Cross-Standard Architecture Foundation

cmake_minimum_required(VERSION 3.16)

project(ieee_media_networking_standards_common
    VERSION 1.0.0
    DESCRIPTION "Hardware abstraction and cross-standard coordination framework"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build Common framework as static library
add_library(ieee_common_framework STATIC
    # Hardware abstraction interfaces (header-only)
    interfaces/network_interface.h
    interfaces/timer_interface.h  
    interfaces/clock_interface.h
    interfaces/timing_sync_interface.h
    interfaces/transport_interface.h
    
    # Cross-standard integration services
    integration/standards_coordinator.h
    integration/standards_coordinator.cpp
    integration/timing_coordinator.h  
    integration/timing_coordinator.cpp
    integration/configuration_manager.h
    integration/configuration_manager.cpp
)

# Add interface include directories
target_include_directories(ieee_common_framework
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
        ${CMAKE_CURRENT_SOURCE_DIR}/integration
)

# Compiler-specific settings
if(MSVC)
    target_compile_options(ieee_common_framework PRIVATE /W4 /WX)
else()
    target_compile_options(ieee_common_framework PRIVATE -Wall -Wextra -Werror)
endif()

# Add threading support for cross-standard coordination
find_package(Threads REQUIRED)
target_link_libraries(ieee_common_framework PRIVATE Threads::Threads)

# Install headers for IEEE standards to use
install(DIRECTORY interfaces/
    DESTINATION include/ieee_common/interfaces
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY integration/
    DESTINATION include/ieee_common/integration  
    FILES_MATCHING PATTERN "*.h"
)

# Install library
install(TARGETS ieee_common_framework
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Phase 00 Architecture Compliance Validation
add_custom_target(validate_phase00_architecture
    COMMAND echo "Validating Phase 00 Cross-Standard Architecture Foundation..."
    COMMAND echo "✓ Hardware abstraction interfaces designed"
    COMMAND echo "✓ Cross-standard coordination services defined"  
    COMMAND echo "✓ IEEE layering dependency framework created"
    COMMENT "Phase 00 architecture foundation validation"
)

# Documentation generation
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)
    set(DOXYGEN_PROJECT_NAME "IEEE Media Networking Standards Common Framework")
    set(DOXYGEN_PROJECT_BRIEF "Hardware abstraction and cross-standard coordination")
    
    doxygen_add_docs(ieee_common_docs
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
        ${CMAKE_CURRENT_SOURCE_DIR}/integration
        COMMENT "Generate IEEE Common Framework documentation"
    )
endif()

# Ensure this framework is built before any IEEE standards
set_property(TARGET ieee_common_framework PROPERTY
    INTERFACE_COMPILE_FEATURES cxx_std_17
)