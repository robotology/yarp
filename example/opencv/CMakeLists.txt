# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

include_directories(${CMAKE_SOURCE_DIR})

find_package(YARP REQUIRED)

find_package(OpenCV REQUIRED)
link_directories(${OPENCV_LINK_DIRECTORIES})
include_directories(${OPENCV_INCLUDE_DIR})

add_executable(opencv_test main.cpp)
target_link_libraries(${YARP_LIBRARIES}
                      ${OPENCV_LIBRARIES})
