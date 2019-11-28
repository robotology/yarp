# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

cmake_minimum_required(VERSION 3.12)
project(Matrix)

find_package(YARP REQUIRED COMPONENTS sig math gsl)

find_package(GSL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(basic_ops basic_ops.cpp)
target_link_libraries(basic_ops YARP::YARP_sig
                                YARP::YARP_math)

add_executable(gsl_example gsl_example.cpp)
target_link_libraries(gsl_example YARP::YARP_sig
                                  YARP::YARP_math
                                  YARP::YARP_gsl
                                  GSL::gsl
                                  GSL::gslcblas)

