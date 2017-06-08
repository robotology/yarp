# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(game_server)

find_package(YARP REQUIRED)

# using ACE
add_definitions(-D_REENTRANT)

if(WIN32 AND NOT CYGWIN)
  add_definitions(-DWIN32 -D_WINDOWS)
endif(WIN32 AND NOT CYGWIN)

if(WIN32 AND CYGWIN)
  add_definitions(-DCYGWIN)
endif(WIN32 AND CYGWIN)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(game_server Game.cpp
                           Login.cpp
                           main.cpp
                           Player.cpp
                           Thing.cpp
                           Things.cpp)
target_link_libraries(game_server ${YARP_LIBRARIES})
