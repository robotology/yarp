# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

set(BUILD_SHARED_LIBS ON)

find_package(YARP REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_library(hoopy hoopy.cpp)
target_link_libraries(hoopy ${YARP_LIBRARIES})

add_executable(test_hoopy test_hoopy.cpp)
target_link_libraries(test_hoopy ${YARP_LIBRARIES})
