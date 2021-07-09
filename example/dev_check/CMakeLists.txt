# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(dev_check)

find_package(YARP COMPONENTS os sig dev REQUIRED)

add_executable(dev_check)
target_sources(dev_check PRIVATE main.cpp)
target_link_libraries(dev_check YARP::YARP_os
                                YARP::YARP_sig
                                YARP::YARP_dev
                                YARP::YARP_init)
