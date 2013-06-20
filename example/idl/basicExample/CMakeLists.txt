# Copyright: (C) 2012 IITRBCS
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)
find_package(YARP REQUIRED)
include_directories(${YARP_INCLUDE_DIRS})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})
include(YarpIDL)

yarp_idl(example.thrift Example.cpp Example.h)

include_directories(${CMAKE_BINARY_DIR})
add_executable(example main.cpp ${CMAKE_BINARY_DIR}/Example.cpp ${CMAKE_BINARY_DIR}/Example.h)
target_link_libraries(example ${YARP_LIBRARIES})
