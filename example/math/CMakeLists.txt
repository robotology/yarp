# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(math_examples)

find_package(YARP COMPONENTS os sig math REQUIRED)

add_executable(benchmark)
target_sources(benchmark PRIVATE benchmark.cpp)
target_link_libraries(benchmark PRIVATE YARP::YARP_os
                                        YARP::YARP_init
                                        YARP::YARP_sig
                                        YARP::YARP_math)
