# Copyright: (C) 2009 RobotCub Consortium
# Authors: Alexandre Bernardino
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(record_sound)

find_package(YARP REQUIRED)

add_executable(record_sound record_sound.cpp)
target_link_libraries(record_sound ${YARP_LIBRARIES})
