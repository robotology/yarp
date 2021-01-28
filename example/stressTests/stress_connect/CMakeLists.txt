# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os REQUIRED)

add_executable(stress_connect)
target_sources(stress_connect PRIVATE stress_connect.cpp)
target_link_libraries(stress_connect PRIVATE ${YARP_LIBRARIES})
