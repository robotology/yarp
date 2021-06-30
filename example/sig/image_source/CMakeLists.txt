# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(image_source)
  find_package(YARP REQUIRED COMPONENTS os sig)
endif()

add_executable(image_source)
target_sources(image_source PRIVATE image_source.cpp)
target_link_libraries(image_source
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET image_source PROPERTY FOLDER "Examples/sig")
endif()
