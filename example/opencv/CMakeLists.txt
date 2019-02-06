# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.5)

include_directories(${CMAKE_SOURCE_DIR})

find_package(YARP REQUIRED)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIR})

add_executable(opencv_test main.cpp)
target_link_libraries(opencv_test ${YARP_LIBRARIES}
                                  ${OpenCV_LIBRARIES})
