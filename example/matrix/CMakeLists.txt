# Copyright: (C) 2009 RobotCub Consortium
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

cmake_minimum_required(VERSION 2.8.7)

PROJECT (Matrix)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
  
ADD_EXECUTABLE(basic_ops basic_ops.cpp)
ADD_EXECUTABLE(gsl_example gsl_example.cpp)

