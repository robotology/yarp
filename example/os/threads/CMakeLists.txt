# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(threads)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(threads)
target_sources(threads PRIVATE threads.cpp)
target_link_libraries(threads
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET threads PROPERTY FOLDER "Examples/os")
endif()
