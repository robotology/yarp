# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(Carrier)

find_package(YARP REQUIRED)

add_executable(carrier_stub carrier_stub.cpp)
target_link_libraries(carrier_stub ${YARP_LIBRARIES})

add_executable(carrier_human carrier_human.cpp)
target_link_libraries(carrier_human ${YARP_LIBRARIES})
