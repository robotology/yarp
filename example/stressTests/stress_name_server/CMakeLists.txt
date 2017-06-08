# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

find_package(YARP REQUIRED)

add_executable(stress_name_server_topics stress_name_server_topics.cpp)
target_link_libraries(stress_name_server_topics ${YARP_LIBRARIES})

add_executable(stress_name_server_reg stress_name_server_reg.cpp)
target_link_libraries(stress_name_server_reg ${YARP_LIBRARIES})
