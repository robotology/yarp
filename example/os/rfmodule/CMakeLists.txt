# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(rfmodule)
  find_package(YARP REQUIRED COMPONENTS os)
endif()

add_executable(rfmodule)
target_sources(rfmodule PRIVATE rfmodule.cpp)
target_link_libraries(rfmodule
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET rfmodule PROPERTY FOLDER "Examples/os")
endif()
