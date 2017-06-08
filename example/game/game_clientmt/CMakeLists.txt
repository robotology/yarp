# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
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

