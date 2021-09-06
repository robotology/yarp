# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(motortest)
  find_package(YARP REQUIRED COMPONENTS os dev)
endif()

add_executable(motortest)
target_sources(motortest PRIVATE motortest.cpp)
target_link_libraries(motortest
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET motortest PROPERTY FOLDER "Examples/dev")
endif()
