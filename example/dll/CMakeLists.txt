# Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6.4)

FIND_PACKAGE(YARP REQUIRED)
LINK_LIBRARIES(${YARP_LIBRARIES})
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

ADD_LIBRARY(hello SHARED Hello.cpp Hello.h)

ADD_EXECUTABLE(program program.cpp)
