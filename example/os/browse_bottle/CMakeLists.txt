# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(browse_bottle)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(browse_bottle)
target_sources(browse_bottle PRIVATE browse_bottle.cpp)
target_link_libraries(browse_bottle PRIVATE YARP::YARP_os)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET browse_bottle PROPERTY FOLDER "Examples/os")
endif()
