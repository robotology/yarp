# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(Rand)

find_package(YARP COMPONENTS sig math REQUIRED)

add_executable(rand)
target_sources(rand PRIVATE rand.cpp)
target_link_libraries(rand PRIVATE ${YARP_LIBRARIES})
