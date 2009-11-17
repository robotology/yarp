# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../../.." CACHE LOCATION "where is yarp?")
FIND_PACKAGE(YARP)

PROJECT(game_server)

IF (WIN32 AND NOT CYGWIN)
  ADD_DEFINITIONS(-DWIN32 -D_WINDOWS)
ENDIF (WIN32 AND NOT CYGWIN)

IF(WIN32 AND CYGWIN)
  ADD_DEFINITIONS(-DCYGWIN)
ENDIF(WIN32 AND CYGWIN)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

ADD_EXECUTABLE(game_server Game.cpp  Login.cpp main.cpp  Player.cpp  Thing.cpp  Things.cpp)
