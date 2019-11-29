# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(maze_maker)

find_package(YARP REQUIRED)

# using ACE
add_definitions(-D_REENTRANT)

include_directories($(CMAKE_SOURCE_DIR))

add_executable(maze_maker main.cpp)
target_link_libraries(${YARP_LIBRARIES})
