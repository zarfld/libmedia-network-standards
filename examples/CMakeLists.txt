# IEEE 1722.1-2021 Standards Examples
cmake_minimum_required(VERSION 3.16)

# Add example executable for complete IEEE 1722.1-2021 implementation
add_executable(ieee_1722_1_2021_complete_example
    ieee_1722_1_2021_complete_example.cpp
)

# Link with the complete standards library
target_link_libraries(ieee_1722_1_2021_complete_example
    PRIVATE
        ieee_1722_1_2021_complete
)

# Set include directories
target_include_directories(ieee_1722_1_2021_complete_example
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Platform-specific configurations
if(WIN32)
    target_link_libraries(ieee_1722_1_2021_complete_example
        PRIVATE
            ws2_32  # For network byte order functions
    )
endif()

# Set compiler options
if(MSVC)
    target_compile_options(ieee_1722_1_2021_complete_example PRIVATE
        /W4
        /permissive-
    )
else()
    target_compile_options(ieee_1722_1_2021_complete_example PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

# Add to build output
set_target_properties(ieee_1722_1_2021_complete_example PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)
