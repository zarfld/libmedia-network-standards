# Common Standards Interfaces
# Abstract interface contracts for cross-standard references

cmake_minimum_required(VERSION 3.16)

# Header-only interface library
add_library(standards_interfaces INTERFACE)

# Include directory
target_include_directories(standards_interfaces
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
)

# C++ standard requirement
target_compile_features(standards_interfaces
    INTERFACE cxx_std_14
)

# Install headers
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/Common
    FILES_MATCHING PATTERN "*.hpp"
)

# Install targets
install(
    TARGETS standards_interfaces
    EXPORT StandardsInterfacesTargets
    INCLUDES DESTINATION include
)

# Export configuration
install(
    EXPORT StandardsInterfacesTargets
    FILE StandardsInterfacesTargets.cmake
    NAMESPACE Standards::
    DESTINATION lib/cmake/StandardsInterfaces
)

# Documentation
message(STATUS "Standards Interfaces: Header-only abstract contracts")
message(STATUS "  - IIEEE1588Contract.hpp: IEEE 1588 PTP abstraction")
