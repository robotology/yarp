# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

cmake_minimum_required(VERSION 2.6.4)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

# Start a project.
PROJECT(run_fakebot)

SET(ENABLE_fakebot TRUE CACHE BOOLEAN "we want to use fakebot")

# Search for source code.
FILE(GLOB_RECURSE folder_source *.cpp *.cc *.c)
FILE(GLOB_RECURSE folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/fakebot)

# Set up our main executable.
IF (folder_source)
  ADD_EXECUTABLE(run_fakebot ${folder_source} ${folder_header})
  TARGET_LINK_LIBRARIES(run_fakebot ${YARP_LIBRARIES})
ELSE (folder_source)
  MESSAGE(FATAL_ERROR "No source code files found. Please add something")
ENDIF (folder_source)
