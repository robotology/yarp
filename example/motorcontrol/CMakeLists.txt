# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(MotorExample)

find_package(YARP os dev REQUIRED)

add_executable(motorcontrol)
target_sources(motorcontrol PRIVATE main.cpp)
target_link_libraries(motorcontrol PRIVATE ${YARP_LIBRARIES})
