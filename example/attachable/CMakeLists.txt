# Copyright: (C) 2009 RobotCub Consortium
# Authors: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

PROJECT(attachable)

ADD_EXECUTABLE(attachable main.cpp)
TARGET_LINK_LIBRARIES(attachable ${YARP_LIBRARIES})
