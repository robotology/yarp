# Copyright: (C) 2011 Istituto Italiano di Tecnologia (IIT)
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

find_package(YARP REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

add_library(hello SHARED Hello.cpp Hello.h)
target_link_libraries(hello ${YARP_LIBRARIES})

add_executable(program program.cpp)
target_link_libraries(program ${YARP_LIBRARIES})
