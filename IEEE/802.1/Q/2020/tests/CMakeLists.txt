# IEEE 802.1Q-2020 Tests

add_executable(ieee_802_1q_2020_test
    ieee_802_1q_2020_test.cpp
)

target_link_libraries(ieee_802_1q_2020_test PRIVATE
    ieee_802_1q_2020
)

target_include_directories(ieee_802_1q_2020_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(ieee_802_1q_2020_test PROPERTIES
    OUTPUT_NAME "ieee_802_1q_2020_test"
)

add_test(NAME ieee_802_1q_2020_basic_test COMMAND ieee_802_1q_2020_test)
