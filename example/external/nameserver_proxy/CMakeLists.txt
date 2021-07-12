# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# Make sure that YARP_DIR is set in your environment to wherever you
# compiled YARP.

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os name REQUIRED)

add_executable(nameserver_proxy)
target_sources(nameserver_proxy PRIVATE main.cpp)
target_link_libraries(nameserver_proxy YARP::YARP_os
                                       YARP::YARP_name
                                       YARP::YARP_init)
