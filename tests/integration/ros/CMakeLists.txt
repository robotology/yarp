# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

find_package(YARP REQUIRED)

yarp_idl_to_dir(RosIntegrationTest.msg ${CMAKE_BINARY_DIR}/msgs SRC HDR INC)
include_directories(${INC})
add_executable(test_topic test_topic.cpp ${SRC} ${HDR})
target_link_libraries(test_topic ${YARP_LIBRARIES})

