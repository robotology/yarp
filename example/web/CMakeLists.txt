# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# Make sure that YARP_DIR is set in your environment to wherever you
# compiled YARP.

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os REQUIRED)

add_executable(web_test)
target_sources(web_test PRIVATE web_test.cpp)
target_link_libraries(web_test PRIVATE YARP::YARP_os YARP::YARP_init)
