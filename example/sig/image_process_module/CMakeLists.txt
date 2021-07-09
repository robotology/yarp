# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(image_process_module)
  find_package(YARP REQUIRED COMPONENTS os sig)
endif()

add_executable(image_process_module)
target_sources(image_process_module PRIVATE image_process_module.cpp)
target_link_libraries(image_process_module
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET image_process_module PROPERTY FOLDER "Examples/sig")
endif()
