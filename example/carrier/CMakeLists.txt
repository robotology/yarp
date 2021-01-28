# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(Carrier)

find_package(YARP COMPONENTS os REQUIRED)

add_executable(carrier_stub)
target_sources(carrier_stub PRIVATE carrier_stub.cpp)
target_link_libraries(carrier_stub PRIVATE YARP::YARP_os
                                           YARP::YARP_init)

add_executable(carrier_human)
target_sources(carrier_human PRIVATE carrier_human.cpp)
target_link_libraries(carrier_human PRIVATE YARP::YARP_os
                                            YARP::YARP_init)
