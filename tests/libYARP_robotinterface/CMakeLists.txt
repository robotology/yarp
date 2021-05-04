# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(NOT TARGET YARP::YARP_robotinterface)
  return()
endif()

add_executable(harness_robotinterface)

target_sources(harness_robotinterface PRIVATE RobotinterfaceTest.cpp)

target_link_libraries(harness_robotinterface PRIVATE YARP_harness
                                                     YARP::YARP_robotinterface)

set_property(TARGET harness_robotinterface PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_robotinterface)
