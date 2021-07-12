# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(hello)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(hello)
target_sources(hello PRIVATE hello.cpp)
target_link_libraries(hello
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET hello PROPERTY FOLDER "Examples/os")
endif()
