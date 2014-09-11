# Copyright: (C) 2009 RobotCub Consortium
# Authors: Lorenzo Natale, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

PROJECT (MotorExample)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

ADD_EXECUTABLE(motorcontrol main.cpp)
TARGET_LINK_LIBRARIES(motorcontrol ${YARP_LIBRARIES})

