# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

#! [simple_sender CMakeLists.txt]
if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(simple_sender)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(simple_sender)
target_sources(simple_sender PRIVATE simple_sender.cpp)
target_link_libraries(simple_sender
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)
#! [simple_sender CMakeLists.txt]

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET simple_sender PROPERTY FOLDER "Examples/os")
endif()
