# Copyright: (C) 2009 RobotCub Consortium
# Authors: Lorenzo Natale, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(MotorExample)

find_package(YARP REQUIRED)

add_executable(motorcontrol main.cpp)
target_link_libraries(motorcontrol ${YARP_LIBRARIES})
