# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

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
