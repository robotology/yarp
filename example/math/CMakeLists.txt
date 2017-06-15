# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(math_examples)

# Find YARP.  Point the YARP_DIR environment variable at your build.
find_package(YARP REQUIRED)

if(NOT YARP_HAS_MATH_LIB)
  message(FATAL_ERROR "YARP found, but was not compiled with math library enabled")
endif(NOT YARP_HAS_MATH_LIB)

add_executable(benchmark benchmark.cpp)
target_link_libraries(benchmark ${YARP_LIBRARIES})
