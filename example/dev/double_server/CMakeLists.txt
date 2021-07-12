# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(double_server)
  find_package(YARP REQUIRED COMPONENTS os dev)
endif()

add_executable(double_server)
target_sources(double_server PRIVATE double_server.cpp)
target_link_libraries(double_server
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET double_server PROPERTY FOLDER "Examples/dev")
endif()
