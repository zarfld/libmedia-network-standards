# IEEE 1588-2019 PTP v2.1 Examples
# Example applications demonstrating time-sensitive design principles

# Time-sensitive types example
add_executable(time_sensitive_types_example
    time_sensitive_types_example.cpp
)

target_link_libraries(time_sensitive_types_example
    IEEE1588_2019_interface
)

# Set C++14 for maximum compatibility
set_property(TARGET time_sensitive_types_example PROPERTY CXX_STANDARD 14)
set_property(TARGET time_sensitive_types_example PROPERTY CXX_STANDARD_REQUIRED ON)

# Compiler specific settings for deterministic behavior
if(MSVC)
    target_compile_options(time_sensitive_types_example PRIVATE 
        /W4 
        /wd4100  # Allow unreferenced parameters in examples
    )
else()
    target_compile_options(time_sensitive_types_example PRIVATE 
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter  # Allow unreferenced parameters in examples
    )
endif()

# Installation
install(TARGETS time_sensitive_types_example
    RUNTIME DESTINATION bin
)

install(FILES time_sensitive_types_example.cpp
    DESTINATION share/ieee_1588_2019/examples
)