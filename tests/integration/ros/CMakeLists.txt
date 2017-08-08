# Copyright: (C) 2015 Istituto Italiano di Tecnologia (IIT)
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

find_package(YARP REQUIRED)

yarp_idl_to_dir(RosIntegrationTest.msg ${CMAKE_BINARY_DIR}/msgs SRC HDR INC)
include_directories(${INC})
add_executable(test_topic test_topic.cpp ${SRC} ${HDR})
target_link_libraries(test_topic ${YARP_LIBRARIES})

