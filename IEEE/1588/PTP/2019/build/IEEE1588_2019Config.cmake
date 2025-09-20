
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was IEEE1588_2019Config.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

# IEEE 1588-2019 PTP v2.1 Configuration

set(IEEE1588_2019_VERSION 1.0.0)

# Set component availability
set(IEEE1588_2019_FOUND TRUE)
set(IEEE1588_2019_INCLUDE_DIRS "/include")
set(IEEE1588_2019_LIBRARIES "")

# Import targets if available
if(EXISTS "/lib/cmake/IEEE1588_2019/IEEE1588_2019Targets.cmake")
    include("/lib/cmake/IEEE1588_2019/IEEE1588_2019Targets.cmake")
endif()

# Verify all required components
check_required_components(IEEE1588_2019)

# Real-time compatibility information
set(IEEE1588_2019_REALTIME_COMPATIBLE TRUE)
set(IEEE1588_2019_FEATURES 
    "Real-time compatible"
    "No dynamic allocation"
    "No exceptions"
    "Bounded execution time"
    "POD types"
    "Hardware agnostic"
)

message(STATUS "Found IEEE 1588-2019 v${IEEE1588_2019_VERSION} (Real-time compatible)")
