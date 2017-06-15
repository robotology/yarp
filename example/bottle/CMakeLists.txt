# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(bottle)

find_package(YARP REQUIRED)

add_executable(bottle main.cpp)
target_link_libraries(bottle ${YARP_LIBRARIES})
