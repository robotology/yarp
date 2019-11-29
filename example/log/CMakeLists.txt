# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

project(log_test)

find_package(YARP REQUIRED)

add_executable(log_test log_test.cpp)
target_link_libraries(log_test YARP::YARP_os
                               YARP::YARP_init)
