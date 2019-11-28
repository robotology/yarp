# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(clientmt)

find_package(YARP REQUIRED)

# using ACE
add_definitions(-D_REENTRANT)

include_directories(${CMAKE_SOURCE_DIR})

if(WIN32 AND NOT CYGWIN)
  add_definitions(-DWIN32 -D_WINDOWS)
endif(WIN32 AND NOT CYGWIN)

if(WIN32 AND CYGWIN)
  add_definitions(-DCYGWIN)
endif(WIN32 AND CYGWIN)

add_executable(clientmt main.cpp)

target_link_libraries(clientmt ${YARP_LIBRARIES})
if(NOT WIN32)
  target_link_libraries(clientmt ncurses)
endif(NOT WIN32)

