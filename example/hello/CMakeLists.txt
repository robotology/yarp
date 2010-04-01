# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(example)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

ADD_EXECUTABLE(example main.cpp)
TARGET_LINK_LIBRARIES(example ${YARP_LIBRARIES})
