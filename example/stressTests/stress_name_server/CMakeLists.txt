# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

find_package(YARP REQUIRED)

add_executable(stress_name_server_topics stress_name_server_topics.cpp)
target_link_libraries(stress_name_server_topics ${YARP_LIBRARIES})

add_executable(stress_name_server_reg stress_name_server_reg.cpp)
target_link_libraries(stress_name_server_reg ${YARP_LIBRARIES})
