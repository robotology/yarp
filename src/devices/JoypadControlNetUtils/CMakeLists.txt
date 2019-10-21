# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

# See https://gitlab.kitware.com/cmake/cmake/issues/14778
# See https://gitlab.kitware.com/cmake/cmake/merge_requests/1524
if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS 3.12)
  message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. object libraries can be used with target_link_libraries now.")
endif()
add_library(joypadcontrolnetutils OBJECT JoypadControlNetUtils.cpp
                                         JoypadControlNetUtils.h)
target_include_directories(joypadcontrolnetutils PRIVATE $<TARGET_PROPERTY:YARP::YARP_os,INTERFACE_INCLUDE_DIRECTORIES>)
target_compile_definitions(joypadcontrolnetutils PRIVATE $<TARGET_PROPERTY:YARP::YARP_os,INTERFACE_COMPILE_DEFINITIONS>)
target_include_directories(joypadcontrolnetutils PRIVATE $<TARGET_PROPERTY:YARP::YARP_sig,INTERFACE_INCLUDE_DIRECTORIES>)
target_compile_definitions(joypadcontrolnetutils PRIVATE $<TARGET_PROPERTY:YARP::YARP_sig,INTERFACE_COMPILE_DEFINITIONS>)
target_include_directories(joypadcontrolnetutils PUBLIC ${CMAKE_CURRENT_LIST_DIR})

set_property(TARGET joypadcontrolnetutils PROPERTY FOLDER "Libraries/Msgs")
