# Copyright: (C) 2012 iCub Facility
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

set(BUILD_SHARED_LIBS ON)

find_package(YARP REQUIRED)

include_directories(${YARP_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})

add_library(hoopy hoopy.cpp)
target_link_libraries(hoopy ${YARP_LIBRARIES})

add_executable(test_hoopy test_hoopy.cpp)
target_link_libraries(test_hoopy ${YARP_LIBRARIES})
