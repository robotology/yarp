# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(maze_maker)

find_package(YARP COMPONENTS os REQUIRED)

# using ACE
add_definitions(-D_REENTRANT)

add_executable(maze_maker)
target_sources(maze_maker PRIVATE main.cpp)
target_include_directories(maze_maker PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(maze_maker PRIVATE ${YARP_LIBRARIES})
