# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# Make sure that YARP_DIR is set in your environment to wherever you
# compiled YARP.

cmake_minimum_required(VERSION 3.12)
project(property)

find_package(YARP COMPONENTS os REQUIRED)

add_executable(property)
target_sources(property PRIVATE main.cpp)
target_link_libraries(property PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(finder)
target_sources(finder PRIVATE finder.cpp)
target_link_libraries(finder PRIVATE YARP::YARP_os YARP::YARP_init)
