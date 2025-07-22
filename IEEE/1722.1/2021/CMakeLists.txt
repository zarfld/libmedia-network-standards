# IEEE 1722.1-2021 Modern AVDECC Implementation
cmake_minimum_required(VERSION 3.16)

# Core AVDECC library - Using working minimal implementation
add_library(ieee_1722_1_2021_core STATIC
    core/ieee_1722_1_2021_minimal.cpp
)

target_include_directories(ieee_1722_1_2021_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
)

# AECP Protocol library - ✅ ACTIVATED & WORKING - Real IEEE 1722.1-2021 implementation
add_library(ieee_1722_1_2021_aecp STATIC
    protocols/aecp/ieee_1722_1_2021_aecp_phase2c_real.cpp
)

target_include_directories(ieee_1722_1_2021_aecp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/protocols/aecp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
)

target_link_libraries(ieee_1722_1_2021_aecp PUBLIC ieee_1722_1_2021_core)

# ACMP Protocol library - ✅ ACTIVATED & WORKING - C interface functional
add_library(ieee_1722_1_2021_acmp STATIC
    protocols/acmp/ieee_1722_1_2021_acmp_implementation.cpp
)

target_include_directories(ieee_1722_1_2021_acmp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/protocols/acmp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
)

target_link_libraries(ieee_1722_1_2021_acmp PUBLIC ieee_1722_1_2021_core)

# State Machines library - TEMPORARILY DISABLED due to compilation errors
# TODO: Fix namespace conflicts and missing includes
# add_library(ieee_1722_1_2021_state_machines STATIC
#     state_machines/ieee_1722_1_2021_state_machines.cpp
#     state_machines/ieee_1722_1_2021_state_machines_refactored.cpp
#     state_machines/ieee_1722_1_2021_state_machines_internal.cpp
# )

# target_include_directories(ieee_1722_1_2021_state_machines PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/core
#     ${CMAKE_CURRENT_SOURCE_DIR}/state_machines
#     ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
# )

# target_link_libraries(ieee_1722_1_2021_state_machines PUBLIC 
#     ieee_1722_1_2021_core
#     ieee_1722_1_2021_aecp
#     ieee_1722_1_2021_acmp
# )

# High-level Library - TEMPORARILY DISABLED due to compilation errors
# TODO: Fix missing dependencies on disabled libraries
# add_library(ieee_1722_1_2021_library STATIC
#     library/ieee_1722_1_2021_library.cpp
#     library/ieee_1722_1_2021_complete_entity.cpp
# )

# target_include_directories(ieee_1722_1_2021_library PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/core
#     ${CMAKE_CURRENT_SOURCE_DIR}/library
#     ${CMAKE_CURRENT_SOURCE_DIR}/protocols/aecp
#     ${CMAKE_CURRENT_SOURCE_DIR}/protocols/acmp
#     ${CMAKE_CURRENT_SOURCE_DIR}/state_machines
#     ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
# )

# target_link_libraries(ieee_1722_1_2021_library PUBLIC 
#     ieee_1722_1_2021_core
#     ieee_1722_1_2021_aecp
#     ieee_1722_1_2021_acmp
#     ieee_1722_1_2021_state_machines
# )

# Hardware integration library - TEMPORARILY DISABLED due to compilation errors
# TODO: Fix missing core dependencies  
# add_library(ieee_1722_1_2021_hardware STATIC
#     hardware/ieee_1722_1_2021_intel_hal_bridge.cpp
# )

# target_include_directories(ieee_1722_1_2021_hardware PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}/core
#     ${CMAKE_CURRENT_SOURCE_DIR}/hardware
#     ${CMAKE_CURRENT_SOURCE_DIR}/../../../Common
# )

# target_link_libraries(ieee_1722_1_2021_hardware PUBLIC ieee_1722_1_2021_core)

# Complete IEEE 1722.1-2021 library - NOW INCLUDES WORKING AECP + ACMP
add_library(ieee_1722_1_2021_complete INTERFACE)

target_link_libraries(ieee_1722_1_2021_complete INTERFACE
    ieee_1722_1_2021_core
    ieee_1722_1_2021_aecp       # ✅ WORKING - Real AECP Protocol implementation
    ieee_1722_1_2021_acmp       # ✅ WORKING - ACMP Protocol with C interface
    # TODO: Re-enable other libraries once compilation errors are fixed:
    # ieee_1722_1_2021_state_machines
    # ieee_1722_1_2021_library
    # ieee_1722_1_2021_hardware
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(ieee_1722_1_2021_core PRIVATE ws2_32)
    
    target_compile_definitions(ieee_1722_1_2021_core PRIVATE
        _WIN32_WINNT=0x0601
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        IEEE_1722_1_2021_COMPLETE_AEM_SUPPORT=1
        IEEE_1722_1_2021_ALL_75_COMMANDS=1
    )
endif()

# Examples (if enabled)
if(BUILD_EXAMPLES)
    add_executable(ieee_1722_1_2021_controller_example
        examples/ieee_1722_1_2021_controller_example.cpp
    )
    
    target_link_libraries(ieee_1722_1_2021_controller_example PRIVATE ieee_1722_1_2021_complete)
    
    add_executable(example_avdecc_controller
        examples/example_avdecc_controller.cpp
    )
    
    target_link_libraries(example_avdecc_controller PRIVATE ieee_1722_1_2021_complete)
endif()

# Tests (if enabled)
if(BUILD_TESTS)
    add_executable(ieee_1722_1_2021_test
        tests/ieee_1722_1_2021_test.cpp
    )
    
    target_link_libraries(ieee_1722_1_2021_test PRIVATE ieee_1722_1_2021_complete)
    
    add_executable(test_1722_1_2021
        tests/test_1722_1_2021.cpp
    )
    
    target_link_libraries(test_1722_1_2021 PRIVATE ieee_1722_1_2021_complete)
    
    add_executable(test_aecp_implementation
        tests/test_aecp_implementation.cpp
    )
    
    target_link_libraries(test_aecp_implementation PRIVATE ieee_1722_1_2021_aecp)
    
    add_executable(test_core_descriptors
        tests/test_core_descriptors.cpp
    )
    
    target_link_libraries(test_core_descriptors PRIVATE ieee_1722_1_2021_core)
endif()

# IEEE 1722.1-2021 AECP Integration Test
add_executable(ieee_1722_1_2021_aecp_integration_test
    test_phase2c_aecp_integration.cpp
)

target_link_libraries(ieee_1722_1_2021_aecp_integration_test
    ieee_1722_1_2021_aecp
)

target_include_directories(ieee_1722_1_2021_aecp_integration_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Test target for integration validation
add_custom_target(run_aecp_integration_test
    COMMAND ieee_1722_1_2021_aecp_integration_test
    DEPENDS ieee_1722_1_2021_aecp_integration_test
    COMMENT "Running IEEE 1722.1-2021 AECP Integration Test"
)

# IEEE 1722.1-2021 ACMP Integration Test
add_executable(ieee_1722_1_2021_acmp_integration_test
    test_acmp_integration.cpp
)

target_link_libraries(ieee_1722_1_2021_acmp_integration_test
    ieee_1722_1_2021_acmp
    ieee_1722_1_2021_core
)

target_include_directories(ieee_1722_1_2021_acmp_integration_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Test target for ACMP integration validation
add_custom_target(run_acmp_integration_test
    COMMAND ieee_1722_1_2021_acmp_integration_test
    DEPENDS ieee_1722_1_2021_acmp_integration_test
    COMMENT "Running IEEE 1722.1-2021 ACMP Integration Test"
)

message(STATUS "IEEE 1722.1-2021 Modern AVDECC libraries configured")
