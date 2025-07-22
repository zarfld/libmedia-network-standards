# IEEE 802.1AS-2021 Tests
cmake_minimum_required(VERSION 3.16)

# Test configuration
add_executable(ieee_802_1as_2021_test
    ieee_802_1as_2021_test.cpp
)

# Link to the main library
target_link_libraries(ieee_802_1as_2021_test PRIVATE
    ieee_802_1as_2021_core
)

# Include directories
target_include_directories(ieee_802_1as_2021_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
)

# Set properties
set_target_properties(ieee_802_1as_2021_test PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Add as test
enable_testing()
add_test(NAME ieee_802_1as_2021_basic_test COMMAND ieee_802_1as_2021_test)

# Platform-specific settings
if(WIN32)
    target_link_libraries(ieee_802_1as_2021_test PRIVATE ws2_32)
endif()

message(STATUS "IEEE 802.1AS-2021 tests configured successfully")
