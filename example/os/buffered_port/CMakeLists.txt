# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

#! [buffered_port CMakeLists.txt]
if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(buffered_port)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(buffered_port)
target_sources(buffered_port PRIVATE buffered_port.cpp)
target_link_libraries(buffered_port
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)
#! [buffered_port CMakeLists.txt]

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET buffered_port PROPERTY FOLDER "Examples/os")
endif()
