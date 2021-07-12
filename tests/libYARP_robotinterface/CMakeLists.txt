# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT TARGET YARP::YARP_robotinterface)
  return()
endif()

add_executable(harness_robotinterface)

target_sources(harness_robotinterface PRIVATE RobotinterfaceTest.cpp)

target_link_libraries(harness_robotinterface PRIVATE YARP_harness
                                                     YARP::YARP_robotinterface)

set_property(TARGET harness_robotinterface PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_robotinterface)
