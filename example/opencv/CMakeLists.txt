# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6.4)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

SET(CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
FIND_PACKAGE(OpenCV REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${OPENCV_INCLUDE_DIR})

 # In addition to specifying the libraries to link, the directories containing the
 # libraries should be added in case the libraries are in a non-standard location
 LINK_DIRECTORIES(${OPENCV_LINK_DIRECTORIES})
 LINK_LIBRARIES(${OPENCV_LIBRARIES})

ADD_EXECUTABLE(opencv_test main.cpp)
