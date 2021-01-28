# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

add_library(joypadcontrolnetutils OBJECT)

target_sources(joypadcontrolnetutils PRIVATE JoypadControlNetUtils.cpp
                                             JoypadControlNetUtils.h)

target_include_directories(joypadcontrolnetutils PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(joypadcontrolnetutils PRIVATE YARP::YARP_os
                                                    YARP::YARP_sig)

set_property(TARGET joypadcontrolnetutils PROPERTY FOLDER "Libraries/Msgs")
