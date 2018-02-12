# Copyright: (C) 2012 Istituto Italiano di Tecnologia (IIT)
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 3.0)
project(module)

find_package(YARP)

add_executable(module main.cpp)
target_link_libraries(module ${YARP_LIBRARIES})
