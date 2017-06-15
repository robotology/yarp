# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you
# compiled YARP.

cmake_minimum_required(VERSION 3.0)

find_package(YARP REQUIRED)

if(NOT YARP_HAS_MATH_LIB)
  message(FATAL_ERROR "Please compile YARP with math library turned on")
endif()

add_executable(yarpwide main.cpp)
target_link_libraries(yarpwide ${YARP_LIBRARIES})
