# Copyright: (C) 2010 RobotCub Consortium
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

SET(PROJECTNAME collatzServer)
PROJECT(${PROJECTNAME})

cmake_minimum_required(VERSION 2.6.4)

# Add Yarp to the "packages" used by this project.
FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

# Find .cpp and .h automatically. This is a bit lazy,
# and in principle it would be better to list these files
# manually.
FILE(GLOB folder_source src/*.cpp)
FILE(GLOB folder_header include/*.hpp include/*.h ../common/*.hpp ../common/*.h)

# Declare groups of source and header, makes tings pretty in VisualStudio.
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

# Create everything needed to build our executable.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/../common)
ADD_EXECUTABLE(${PROJECTNAME} ${folder_source} ${folder_header})

# Make VisualStudio a bit quieter with warnings.
IF(MSVC)
  SET_TARGET_PROPERTIES(${PROJECTNAME} PROPERTIES COMPILE_FLAGS /wd4996)
ENDIF(MSVC)

