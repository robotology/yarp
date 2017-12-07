# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

include_directories(${CMAKE_SOURCE_DIR})

find_package(YARP REQUIRED)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIR})

add_executable(opencv_test main.cpp)
target_link_libraries(opencv_test ${YARP_LIBRARIES}
                                  ${OpenCV_LIBRARIES})
