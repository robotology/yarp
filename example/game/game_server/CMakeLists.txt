# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6.4)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

PROJECT(game_server)

# using ACE
ADD_DEFINITIONS(-D_REENTRANT)

IF (WIN32 AND NOT CYGWIN)
  ADD_DEFINITIONS(-DWIN32 -D_WINDOWS)
ENDIF (WIN32 AND NOT CYGWIN)

IF(WIN32 AND CYGWIN)
  ADD_DEFINITIONS(-DCYGWIN)
ENDIF(WIN32 AND CYGWIN)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

ADD_EXECUTABLE(game_server Game.cpp  Login.cpp main.cpp  Player.cpp  Thing.cpp  Things.cpp)
