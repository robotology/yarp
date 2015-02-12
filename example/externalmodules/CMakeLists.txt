# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include_directories(${YARP_INCLUDE_DIRS})
include(YarpPlugin)

yarp_begin_plugin_library(foobaz)
add_subdirectory(baz)
add_subdirectory(foo)
yarp_end_plugin_library(foobaz)
yarp_add_plugin_library_executable(foobazdev foobaz)
