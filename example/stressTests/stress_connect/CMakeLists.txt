# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6.4)

find_package(YARP REQUIRED)

include_directories(${YARP_INCLUDE_DIRS})

link_libraries(${YARP_LIBRARIES})

add_executable(stress_connect stress_connect.cpp)
