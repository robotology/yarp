# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

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
