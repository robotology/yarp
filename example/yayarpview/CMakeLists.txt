# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

# Copyright: (C) 2010 Renaud Detry
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6.4)

PROJECT (yayarpview)

# replace with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP REQUIRED)
ADD_DEFINITIONS(${YARP_DEFINES})

FIND_PACKAGE(X11)

INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
LINK_LIBRARIES(${X11_LIBRARIES})

INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

# LINK_LIBRARIES("-ljpeg -lpng -lpthread -lm")
# ADD_DEFINITIONS("-Dcimg_use_png -Dcimg_use_jpeg")
LINK_LIBRARIES("-lpthread -lm")

FILE(GLOB HEADERS *.h)
FILE(GLOB CODE *.cpp)

ADD_EXECUTABLE(yayarpview ${HEADERS} ${CODE})
TARGET_LINK_LIBRARIES(yayarpview ${YARP_LIBRARIES})
