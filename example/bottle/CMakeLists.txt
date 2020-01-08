# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(bottle)

find_package(YARP COMPONENTS os REQUIRED)

add_executable(bottle)
target_sources(bottle PRIVATE main.cpp)
target_link_libraries(bottle PRIVATE YARP::YARP_os
                                     YARP::YARP_init)
