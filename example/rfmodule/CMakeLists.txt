# Copyright: 2012 iCub Facility, Istituto Italiano di Tecnologia
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 3.0)
project(module)

find_package(YARP)

add_executable(module main.cpp)
target_link_libraries(module ${YARP_LIBRARIES})
