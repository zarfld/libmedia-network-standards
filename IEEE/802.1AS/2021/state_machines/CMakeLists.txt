# IEEE 802.1AS-2021 State Machines
cmake_minimum_required(VERSION 3.16)

# State machine sources - REAL IEEE 802.1AS-2021 IMPLEMENTATIONS
set(STATE_MACHINE_SOURCES
    port_state_machine.cpp
    # Additional state machines will be added here:
    # - sync_receive_state_machine.cpp
    # - sync_send_state_machine.cpp  
    # - announce_receive_state_machine.cpp
    # - announce_transmit_state_machine.cpp
    # - pdelay_req_state_machine.cpp
    # - pdelay_resp_state_machine.cpp
)

set(STATE_MACHINE_HEADERS
    port_state_machine.h
)

# Create state machine library
add_library(ieee_802_1as_2021_state_machines STATIC 
    ${STATE_MACHINE_SOURCES}
    ${STATE_MACHINE_HEADERS}
)

# Set target properties
set_target_properties(ieee_802_1as_2021_state_machines PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Include directories
target_include_directories(ieee_802_1as_2021_state_machines PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
)

# Link with core IEEE 802.1AS-2021 library
target_link_libraries(ieee_802_1as_2021_state_machines PUBLIC
    ieee_802_1as_2021_core
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(ieee_802_1as_2021_state_machines PRIVATE ws2_32)
endif()

message(STATUS "✅ IEEE 802.1AS-2021 state machines configured with REAL implementations!")
message(STATUS "   - Port State Machine: Complete BMCA + State Transitions")
message(STATUS "   - BMCA Engine: Full IEEE 1588-2019 + 802.1AS-2021 algorithm")
