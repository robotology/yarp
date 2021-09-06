# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)
  project(fake_grabber)
  find_package(YARP REQUIRED COMPONENTS os sig dev)
endif()

add_executable(fake_grabber)
target_sources(fake_grabber
  PRIVATE
    fake_grabber.cpp
    FakeFrameGrabber.h
)
target_link_libraries(fake_grabber
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET fake_grabber PROPERTY FOLDER "Examples/dev")
endif()


add_executable(fake_grabber_net)
target_sources(fake_grabber_net
  PRIVATE
    fake_grabber_net.cpp
    FakeFrameGrabber.h
)
target_link_libraries(fake_grabber_net
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET fake_grabber_net PROPERTY FOLDER "Examples/dev")
endif()


add_executable(fake_grabber_net2)
target_sources(fake_grabber_net2
  PRIVATE
    fake_grabber_net2.cpp
    FakeFrameGrabber2.h
)
target_link_libraries(fake_grabber_net2
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_sig
    YARP::YARP_dev
)

if(DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  set_property(TARGET fake_grabber_net2 PROPERTY FOLDER "Examples/dev")
endif()
