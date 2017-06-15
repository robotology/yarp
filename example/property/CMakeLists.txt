# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

cmake_minimum_required(VERSION 3.0)
project(property)

find_package(YARP REQUIRED)

add_executable(property main.cpp)
target_link_libraries(property ${YARP_LIBRARIES})

add_executable(finder finder.cpp)
target_link_libraries(finder ${YARP_LIBRARIES})
