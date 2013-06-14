# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

find_package(YARP REQUIRED)
include_directories(${YARP_INCLUDE_DIRS})
link_libraries(${YARP_LIBRARIES})

set(CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
find_package(OpenCV REQUIRED)

include_directories(${CMAKE_SOURCE_DIR} ${OPENCV_INCLUDE_DIR})

# In addition to specifying the libraries to link, the directories containing the
# libraries should be added in case the libraries are in a non-standard location
link_directories(${OPENCV_LINK_DIRECTORIES})
link_libraries(${OPENCV_LIBRARIES})

add_executable(opencv_test main.cpp)
