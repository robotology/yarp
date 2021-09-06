# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os dev REQUIRED)

yarp_begin_plugin_library(userlib)
add_subdirectory(fake_grabber)
yarp_end_plugin_library(userlib)

add_executable(test_userlib)
target_sources(test_userlib PRIVATE test_userlib.cpp)

target_link_libraries(test_userlib PRIVATE ${YARP_LIBRARIES})
target_link_libraries(test_userlib PRIVATE userlib)
