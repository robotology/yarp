# Copyright: (C) 2009 RobotCub Consortium
# Authors: Alexandre Bernardino
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

PROJECT(record_sound) 

ADD_EXECUTABLE(record_sound record_sound.cpp)
TARGET_LINK_LIBRARIES(record_sound ${YARP_LIBRARIES})

