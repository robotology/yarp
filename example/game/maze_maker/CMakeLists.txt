# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(maze_maker)

find_package(YARP REQUIRED)

# using ACE
add_definitions(-D_REENTRANT)

include_directories($(CMAKE_SOURCE_DIR))

add_executable(maze_maker main.cpp)
target_link_libraries(${YARP_LIBRARIES})
