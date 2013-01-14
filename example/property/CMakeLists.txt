# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

cmake_minimum_required(VERSION 2.6.4)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

PROJECT(property)

ADD_EXECUTABLE(property main.cpp)
TARGET_LINK_LIBRARIES(property ${YARP_LIBRARIES})

ADD_EXECUTABLE(finder finder.cpp)
TARGET_LINK_LIBRARIES(finder ${YARP_LIBRARIES})

