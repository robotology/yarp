# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# Make sure that YARP_DIR is set in your environment to wherever you
# compiled YARP.

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os sig REQUIRED)

add_executable(framerate)
target_sources(framerate PRIVATE main.cpp)
target_link_libraries(framerate PRIVATE ${YARP_LIBRARIES})
