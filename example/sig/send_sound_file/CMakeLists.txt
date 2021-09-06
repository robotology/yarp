# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(send_sound_file)
  find_package(YARP REQUIRED COMPONENTS os sig)
endif()

add_executable(send_sound_file)
target_sources(send_sound_file PRIVATE send_sound_file.cpp)
target_link_libraries(send_sound_file
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET send_sound_file PROPERTY FOLDER "Examples/sig")
endif()
