# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

cmake_minimum_required(VERSION 2.8.7)

PROJECT(PortPower)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

FILE(GLOB_RECURSE examples *.cpp)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

FOREACH(example ${examples})
	# get name without extension
	GET_FILENAME_COMPONENT(example_we "${example}" NAME_WE)
	ADD_EXECUTABLE(${example_we} ${example})
ENDFOREACH(example ${examples})
