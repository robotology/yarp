# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os REQUIRED)

add_library(hoopy SHARED)
target_sources(hoopy PRIVATE hoopy.cpp)
target_include_directories(hoopy PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(hoopy PRIVATE YARP::YARP_os)

add_executable(test_hoopy)
target_sources(test_hoopy PRIVATE test_hoopy.cpp)
target_include_directories(test_hoopy PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(test_hoopy PRIVATE YARP::YARP_os YARP::YARP_init)
