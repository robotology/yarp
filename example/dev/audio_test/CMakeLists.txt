# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(audio_test)
  find_package(YARP REQUIRED COMPONENTS os sig dev)
endif()

add_executable(audio_test)
target_sources(audio_test PRIVATE audio_test.cpp)
target_link_libraries(audio_test
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET audio_test PROPERTY FOLDER "Examples/dev")
endif()
