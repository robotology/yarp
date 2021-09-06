# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

#! [simple_receiver CMakeLists.txt]
if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(simple_receiver)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(simple_receiver)
target_sources(simple_receiver PRIVATE simple_receiver.cpp)
target_link_libraries(simple_receiver
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)
#! [simple_receiver CMakeLists.txt]

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET simple_receiver PROPERTY FOLDER "Examples/os")
endif()
