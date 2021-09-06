# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os idl_tools REQUIRED)

yarp_idl_to_dir(
  RosIntegrationTest.msg
  ${CMAKE_BINARY_DIR}/msgs
  SRC
  HDR
  INC
)

add_executable(test_topic)
target_sources(test_topic PRIVATE test_topic.cpp ${SRC} ${HDR})
target_include_directories(test_topic PRIVATE ${INC})
target_link_libraries(test_topic PRIVATE ${YARP_LIBRARIES})
