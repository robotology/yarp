# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(bottle)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(bottle)
target_sources(bottle PRIVATE bottle.cpp)
target_link_libraries(bottle PRIVATE YARP::YARP_os)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET bottle PROPERTY FOLDER "Examples/os")
endif()
