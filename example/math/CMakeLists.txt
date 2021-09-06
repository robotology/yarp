# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(math_examples)

find_package(YARP COMPONENTS os sig math REQUIRED)

add_executable(benchmark)
target_sources(benchmark PRIVATE benchmark.cpp)
target_link_libraries(benchmark PRIVATE YARP::YARP_os
                                        YARP::YARP_init
                                        YARP::YARP_sig
                                        YARP::YARP_math)
