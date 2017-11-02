# Copyright: (C) 2009 RobotCub Consortium
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

cmake_minimum_required(VERSION 3.0)
project(Matrix)

find_package(YARP REQUIRED)

find_package(GSL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(basic_ops basic_ops.cpp)
target_link_libraries(basic_ops ${YARP_LIBRARIES})

add_executable(gsl_example gsl_example.cpp)
target_link_libraries(gsl_example ${YARP_LIBRARIES} YARP::YARP_gsl GSL::gsl GSL::gslcblas)

