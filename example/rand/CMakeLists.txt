# Copyright: (C) 2009 RobotCub Consortium
# Authors: Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT (Rand)

FIND_PACKAGE(YARP)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

ADD_EXECUTABLE(rand rand.cpp)
TARGET_LINK_LIBRARIES(rand ${YARP_LIBRARIES})

