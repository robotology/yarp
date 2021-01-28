# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_library(hello SHARED)
target_sources(hello PRIVATE Hello.cpp
                             Hello.h)
target_link_libraries(hello PRIVATE YARP::YARP_os)

add_executable(program)
target_sources(program PRIVATE program.cpp)
target_link_libraries(program PRIVATE YARP::YARP_os
                                       YARP::YARP_init)
