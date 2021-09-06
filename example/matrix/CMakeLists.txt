# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# Make sure that YARP_DIR is set in your environment to wherever you
# compiled YARP.

cmake_minimum_required(VERSION 3.12)
project(Matrix)

find_package(YARP REQUIRED COMPONENTS sig math gsl)
find_package(GSL REQUIRED)

add_executable(basic_ops)
target_sources(basic_ops PRIVATE basic_ops.cpp)
target_include_directories(basic_ops PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(basic_ops YARP::YARP_sig
                                YARP::YARP_math)

add_executable(gsl_example)
target_sources(gsl_example PRIVATE gsl_example.cpp)
target_include_directories(gsl_example PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(gsl_example PRIVATE YARP::YARP_sig
                                          YARP::YARP_math
                                          YARP::YARP_gsl
                                          GSL::gsl
                                          GSL::gslcblas)
