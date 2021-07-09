# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(portable_pair)
  find_package(YARP REQUIRED COMPONENTS os sig)
endif()

add_executable(portable_pair)
target_sources(portable_pair PRIVATE portable_pair.cpp)
target_link_libraries(portable_pair
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET portable_pair PROPERTY FOLDER "Examples/os")
endif()
