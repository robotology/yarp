# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

find_package(YARP REQUIRED)

yarp_begin_plugin_library(foo)
add_subdirectory(fooBot)
add_subdirectory(fooDevice)
yarp_end_plugin_library(foo)
