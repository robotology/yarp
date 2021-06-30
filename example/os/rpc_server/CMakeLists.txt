# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(rpc_server)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(rpc_server)
target_sources(rpc_server PRIVATE rpc_server.cpp)
target_link_libraries(rpc_server
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET rpc_server PROPERTY FOLDER "Examples/os")
endif()
