# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.4)

# Start a project.
PROJECT(math_examples)

# Find YARP.  Point the YARP_DIR environment variable at your build.
FIND_PACKAGE(YARP)

IF (NOT YARP_HAS_MATH_LIB)
	MESSAGE(FATAL_ERROR "YARP found, but was not compiled with math library enabled")
ENDIF (NOT YARP_HAS_MATH_LIB)


ADD_EXECUTABLE(benchmark benchmark.cpp)

