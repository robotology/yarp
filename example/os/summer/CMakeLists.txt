# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(summer)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(summer)
target_sources(summer PRIVATE summer.cpp)
target_link_libraries(summer
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET summer PROPERTY FOLDER "Examples/os")
endif()
