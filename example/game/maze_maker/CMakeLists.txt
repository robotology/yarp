# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../../.." CACHE LOCATION "where is yarp?")
FIND_PACKAGE(YARP)

PROJECT(maze_maker)

INCLUDE_DIRECTORIES($(CMAKE_SOURCE_DIR))

ADD_EXECUTABLE(maze_maker main.cpp)

