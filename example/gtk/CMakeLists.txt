# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6.4)

PROJECT(gtkdemo)

FIND_PACKAGE(YARP REQUIRED)
LINK_LIBRARIES(${YARP_LIBRARIES})
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

SET(CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
FIND_PACKAGE(GtkPlus REQUIRED)
ADD_DEFINITIONS(${GTKPLUS_C_FLAGS})
LINK_LIBRARIES(${GTKPLUS_LINK_FLAGS})
INCLUDE_DIRECTORIES(${GTKPLUS_INCLUDE_DIR})

FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h include/*.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

ADD_EXECUTABLE(gtkdemo ${folder_source} ${folder_header})

