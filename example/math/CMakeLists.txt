# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.5)
project(math_examples)

# Find YARP.  Point the YARP_DIR environment variable at your build.
find_package(YARP REQUIRED)

if(NOT YARP_HAS_MATH_LIB)
  message(FATAL_ERROR "YARP found, but was not compiled with math library enabled")
endif(NOT YARP_HAS_MATH_LIB)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark ${YARP_LIBRARIES})
