# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
 
cmake_minimum_required(VERSION 2.4)

find_package(YARP REQUIRED)

project(yarpc)

include_directories(${PROJECT_SOURCE_DIR})

add_library(yarpc yarp.h yarp.cpp yarpnetwork.cpp yarpport.cpp yarpcontact.cpp yarpreader.cpp yarpwriter.cpp yarpportable.cpp yarpstring.cpp yarpsemaphore.cpp yarpimpl.h)

add_executable(test1 test1.cpp)
target_link_libraries(test1 yarpc)

add_executable(test2 test2.cpp)
target_link_libraries(test2 yarpc)
