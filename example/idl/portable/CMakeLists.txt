# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.0)

#find YARP
find_package(YARP REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(sender sender.cpp SharedData.cpp)
target_link_libraries(sender ${YARP_LIBRARIES})

add_executable(receiver receiver.cpp SharedData.cpp)
target_link_libraries(receiver ${YARP_LIBRARIES})
