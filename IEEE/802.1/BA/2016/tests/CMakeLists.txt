# Tests for IEEE 802.1BA-2016
cmake_minimum_required(VERSION 3.16)

enable_testing()

add_executable(test_ieee_802_1ba_2016
    test_ieee_802_1ba_2016.cpp
)

target_link_libraries(test_ieee_802_1ba_2016 PRIVATE ieee_802_1ba_2016)

add_test(NAME ieee_802_1ba_2016_basic COMMAND test_ieee_802_1ba_2016)
