# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(minimal_client)

find_package(YARP REQUIRED)

if(WIN32 AND NOT CYGWIN)
  add_definitions(-DWIN32 -D_WINDOWS)
endif(WIN32 AND NOT CYGWIN)

if(WIN32 AND CYGWIN)
  add_definitions(-DCYGWIN)
endif(WIN32 AND CYGWIN)

add_executable(minimal_client main.cpp)
target_link_libraries(minimal_client ${YARP_LIBRARIES})
