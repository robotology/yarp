# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS OR NOT TARGET YARP::YARP_math)
  return()
endif()

add_library(FrameTransformUtils OBJECT)

target_sources(FrameTransformUtils
  PRIVATE
    FrameTransformContainer.cpp
    FrameTransformContainer.h
)

target_include_directories(FrameTransformUtils PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(FrameTransformUtils
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
    YARP::YARP_math
)

set_property(TARGET FrameTransformUtils PROPERTY FOLDER "Devices/Shared")
