# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# This is a small project to check if ace is working
PROJECT(timers)

# There is a script for finding the ACE library in the prev directory
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../)

# We need ACE
FIND_PACKAGE(Ace REQUIRED)

# Find source code files
FILE(GLOB HEADERS *.h)
SET(CODE main.cpp)

# Lay the files out in folders in environments where that makes sense
# This is completely optional
SOURCE_GROUP("Header Files" FILES ${HEADERS})
SOURCE_GROUP("Source Files" FILES ${CODE})

# Create our executable
ADD_EXECUTABLE(checkTime ${HEADERS} ${CODE})
ADD_DEFINITIONS(-D_REENTRANT)