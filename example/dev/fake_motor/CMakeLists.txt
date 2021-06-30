# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(fake_motor)
  find_package(YARP REQUIRED COMPONENTS os dev)
endif()

add_executable(fake_motor)
target_sources(fake_motor PRIVATE fake_motor.cpp)
target_link_libraries(fake_motor
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET fake_motor PROPERTY FOLDER "Examples/dev")
endif()
