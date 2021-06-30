# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(log_test)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(log_test)
target_sources(log_test PRIVATE log_test.cpp)
target_link_libraries(log_test
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET log_test PROPERTY FOLDER "Examples/os")
endif()
