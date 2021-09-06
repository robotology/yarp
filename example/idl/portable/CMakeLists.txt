# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(portable_example)
set(CMAKE_CXX_FLAGS "-Weverything -Wno-c++98-compat -Wno-padded -Wno-shadow")

find_package(YARP COMPONENTS os idl_tools REQUIRED)

yarp_add_idl(IDL_GEN_FILES SharedData.thrift)

add_executable(sender)
target_sources(sender PRIVATE sender.cpp
                              ${IDL_GEN_FILES})
target_link_libraries(sender PRIVATE YARP::YARP_os
                                     YARP::YARP_init)

add_executable(receiver)
target_sources(receiver PRIVATE receiver.cpp
                                ${IDL_GEN_FILES})
target_link_libraries(receiver PRIVATE YARP::YARP_os
                                       YARP::YARP_init)
