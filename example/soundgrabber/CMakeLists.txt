# Copyright: (C) 2009 RobotCub Consortium
# Authors: Alexandre Bernardino
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6.4)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

PROJECT(record_sound) 

ADD_EXECUTABLE(record_sound record_sound.cpp)
TARGET_LINK_LIBRARIES(record_sound ${YARP_LIBRARIES})

