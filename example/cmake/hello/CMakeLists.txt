# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# YARP needs CMake 3.0 or greater
cmake_minimum_required(VERSION 3.12)

# find YARP
find_package(YARP REQUIRED)

# set up our program
add_executable(hello hello.cpp)

# link with YARP libraries
target_link_libraries(hello ${YARP_LIBRARIES})
