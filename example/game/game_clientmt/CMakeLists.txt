# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

PROJECT(clientmt)
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../../.." CACHE LOCATION "where is yarp?")
#SET(YARP_DIR "$ENV{YARP_ROOT}/build/vc6" CACHE LOCATION "where is your yarp build directory?")
FIND_PACKAGE(YARP)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

IF (WIN32 AND NOT CYGWIN)
  ADD_DEFINITIONS(-DWIN32 -D_WINDOWS)
ENDIF (WIN32 AND NOT CYGWIN)

IF(WIN32 AND CYGWIN)
  ADD_DEFINITIONS(-DCYGWIN)
ENDIF(WIN32 AND CYGWIN)

IF (NOT WIN32)
   LINK_LIBRARIES(ncurses)
ENDIF (NOT WIN32)

ADD_EXECUTABLE(clientmt main.cpp)

