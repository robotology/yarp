# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(simple_motor_client)
  find_package(YARP REQUIRED COMPONENTS os dev)
endif()

add_executable(simple_motor_client)
target_sources(simple_motor_client PRIVATE simple_motor_client.cpp)
target_link_libraries(simple_motor_client
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET simple_motor_client PROPERTY FOLDER "Examples/dev")
endif()
