# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(internal_monologue)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(internal_monologue)
target_sources(internal_monologue PRIVATE internal_monologue.cpp)
target_link_libraries(internal_monologue
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET internal_monologue PROPERTY FOLDER "Examples/os")
endif()
