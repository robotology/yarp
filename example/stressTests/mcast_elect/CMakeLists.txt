# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

# Start a project.
project(mcast_elect)

# Find YARP.  Point the YARP_DIR environment variable at your build.
find_package(YARP COMPONENTS os REQUIRED)

# Set up our main executable.
add_executable(mcast_elect)
target_sources(mcast_elect PRIVATE mcast_elect.cpp)
target_link_libraries(mcast_elect PRIVATE ${YARP_LIBRARIES})
