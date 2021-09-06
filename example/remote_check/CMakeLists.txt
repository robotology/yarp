# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# Make sure that YARP_DIR is set in your environment to wherever you
# compiled YARP.

cmake_minimum_required(VERSION 3.12)
project(remote_check)

find_package(YARP COMPONENTS os sig dev REQUIRED)

add_executable(remote_check)
target_sources(remote_check PRIVATE main.cpp)
target_link_libraries(remote_check PRIVATE ${YARP_LIBRARIES})
