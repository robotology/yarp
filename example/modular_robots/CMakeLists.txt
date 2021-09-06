# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os name math REQUIRED)

add_executable(yarpwide)
target_sources(PRIVATE main.cpp)
target_link_libraries(yarpwide PRIVATE YARP::YARP_os
                                       YARP::YARP_init
                                       YARP::YARP_name
                                       YARP::YARP_math)
