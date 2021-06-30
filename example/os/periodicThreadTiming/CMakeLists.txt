# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(periodicThreadTiming)
  find_package(YARP REQUIRED COMPONENTS os sig)
endif()

add_executable(periodicThreadTiming)
target_sources(periodicThreadTiming PRIVATE periodicThreadTiming.cpp)
target_link_libraries(periodicThreadTiming
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET periodicThreadTiming PROPERTY FOLDER "Examples/os")
endif()
