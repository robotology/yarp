# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
SET(KEYWORD "yarpfs")

# Start a project.
PROJECT(${KEYWORD})

# Find YARP.  Point the YARP_BUILD environment variable at your build.
SET(YARP_DIR "$ENV{YARP_BUILD}" CACHE LOCATION "where is yarp?")
FIND_PACKAGE(YARP)

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)
LINK_LIBRARIES(fuse)

# Search for source code.
FILE(GLOB folder_source *.cpp *.cc *.c)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

# Set up our main executable.
IF (folder_source)
  ADD_EXECUTABLE(${KEYWORD} ${folder_source} ${folder_header})
ELSE (folder_source)
  MESSAGE(FATAL_ERROR "No source code files found. Please add something")
ENDIF (folder_source)
