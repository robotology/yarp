# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(grabber_client)
  find_package(YARP REQUIRED COMPONENTS os sig dev)
endif()

add_executable(grabber_client)
target_sources(grabber_client PRIVATE grabber_client.cpp)
target_link_libraries(grabber_client
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET grabber_client PROPERTY FOLDER "Examples/dev")
endif()


add_executable(grabber_client_alternate)
target_sources(grabber_client_alternate PRIVATE grabber_client_alternate.cpp)
target_link_libraries(grabber_client_alternate
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET grabber_client_alternate PROPERTY FOLDER "Examples/dev")
endif()
