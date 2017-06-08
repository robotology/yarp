# Copyright: (C) 2009 RobotCub Consortium
# Authors: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(attachable)

find_package(YARP REQUIRED)

add_executable(attachable main.cpp)
target_link_libraries(attachable ${YARP_LIBRARIES})
