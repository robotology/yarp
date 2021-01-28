# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os name math REQUIRED)

add_executable(yarpwide)
target_sources(PRIVATE main.cpp)
target_link_libraries(yarpwide PRIVATE YARP::YARP_os
                                       YARP::YARP_init
                                       YARP::YARP_name
                                       YARP::YARP_math)
