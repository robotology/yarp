# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP REQUIRED)

FIND_PACKAGE(OpenCV REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${OPENCV_INCLUDE_DIR})

 # In addition to specifying the libraries to link, the directories containing the
 # libraries should be added in case the libraries are in a non-standard location
 LINK_DIRECTORIES(${OPENCV_LINK_DIRECTORIES})
 LINK_LIBRARIES(${OPENCV_LIBRARIES})

ADD_EXECUTABLE(opencv_test main.cpp)
