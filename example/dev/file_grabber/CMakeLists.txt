# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(file_grabber)
  find_package(YARP REQUIRED COMPONENTS os sig dev)
endif()

add_executable(file_grabber)
target_sources(file_grabber PRIVATE file_grabber.cpp)
target_link_libraries(file_grabber
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET file_grabber PROPERTY FOLDER "Examples/dev")
endif()


add_executable(file_grabber_net)
target_sources(file_grabber_net PRIVATE file_grabber_net.cpp)
target_link_libraries(file_grabber_net
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET file_grabber_net PROPERTY FOLDER "Examples/dev")
endif()
