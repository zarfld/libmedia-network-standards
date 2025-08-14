# IEEE 802.1Q-2020 VLAN/QoS Helpers

cmake_minimum_required(VERSION 3.16)
project(IEEE802_1Q_2020 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IEEE_802_1Q_2020_SOURCES
    IEEE_802_1_Q_2020.cpp
    qos.cpp
    fdb.cpp
    filtering.cpp
)

set(IEEE_802_1Q_2020_HEADERS
    IEEE_802_1_Q_2020.h
    qos.h
    fdb.h
    filtering.h
    scheduler.h
    port_profile.h
)

add_library(ieee_802_1q_2020 STATIC
    ${IEEE_802_1Q_2020_SOURCES}
    ${IEEE_802_1Q_2020_HEADERS}
)

add_library(IEEE::802.1Q::2020 ALIAS ieee_802_1q_2020)

set_target_properties(ieee_802_1q_2020 PROPERTIES
    OUTPUT_NAME "ieee_802_1q_2020"
)

target_include_directories(ieee_802_1q_2020 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/ieee_802_1q_2020>
)

# Tests (optional)
option(IEEE_802_1Q_2020_BUILD_TESTS "Build IEEE 802.1Q-2020 tests" ON)
if(IEEE_802_1Q_2020_BUILD_TESTS AND BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Install rules
include(GNUInstallDirs)
install(TARGETS ieee_802_1q_2020 EXPORT ieee_802_1q_2020-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ieee_802_1q_2020
)

install(FILES ${IEEE_802_1Q_2020_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ieee_802_1q_2020
)

export(TARGETS ieee_802_1q_2020
    FILE "${CMAKE_CURRENT_BINARY_DIR}/ieee_802_1q_2020-targets.cmake"
)
