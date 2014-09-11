# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

PROJECT(maze_maker)

# using ACE
ADD_DEFINITIONS(-D_REENTRANT)

INCLUDE_DIRECTORIES($(CMAKE_SOURCE_DIR))

ADD_EXECUTABLE(maze_maker main.cpp)

