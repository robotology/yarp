# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(RGBD)
  find_package(YARP REQUIRED COMPONENTS os sig dev)
endif()

add_executable(RGBD_test_1a)
target_sources(RGBD_test_1a PRIVATE RGBD_test_1a.cpp)
target_link_libraries(RGBD_test_1a
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET RGBD_test_1a PROPERTY FOLDER "Examples/dev")
endif()


add_executable(RGBD_test_1b_server)
target_sources(RGBD_test_1b_server PRIVATE RGBD_test_1b_server.cpp)
target_link_libraries(RGBD_test_1b_server
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET RGBD_test_1b_server PROPERTY FOLDER "Examples/dev")
endif()


add_executable(RGBD_test_1b_client)
target_sources(RGBD_test_1b_client PRIVATE RGBD_test_1b_client.cpp)
target_link_libraries(RGBD_test_1b_client  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET RGBD_test_1b_client PROPERTY FOLDER "Examples/dev")
endif()
