# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)
find_package(YARP REQUIRED)
include_directories(${YARP_INCLUDE_DIRS})
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpPlugin)

yarp_begin_plugin_library(userlib)
add_subdirectory(fake_grabber)
yarp_end_plugin_library(userlib)
add_executable(test_userlib test_userlib.cpp)

target_link_libraries(test_userlib ${YARP_LIBRARIES})
target_link_libraries(test_userlib userlib)
