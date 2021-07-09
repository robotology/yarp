# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(make_count)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(make_count)
target_sources(make_count PRIVATE make_count.cpp)
target_link_libraries(make_count
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET make_count PROPERTY FOLDER "Examples/os")
endif()
