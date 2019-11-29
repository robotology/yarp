# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(dev_check)

# make sure YARP_DIR environment variable is set
find_package(YARP REQUIRED)

add_executable(dev_check main.cpp)
target_link_libraries(dev_check ${YARP_LIBRARIES})
