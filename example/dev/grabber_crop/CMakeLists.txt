# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(grabber_crop)
  find_package(YARP REQUIRED COMPONENTS os sig dev)
endif()

add_executable(grabber_crop)
target_sources(grabber_crop PRIVATE grabber_crop.cpp)
target_link_libraries(grabber_crop
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET grabber_crop PROPERTY FOLDER "Examples/dev")
endif()
