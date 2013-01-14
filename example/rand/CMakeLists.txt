# Copyright: (C) 2009 RobotCub Consortium
# Authors: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6.4)

PROJECT (Rand)

FIND_PACKAGE(YARP)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

ADD_EXECUTABLE(rand rand.cpp)
TARGET_LINK_LIBRARIES(rand ${YARP_LIBRARIES})

