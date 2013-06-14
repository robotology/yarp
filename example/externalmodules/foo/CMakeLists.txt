# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

find_package(YARP REQUIRED)
set(CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include_directories(${YARP_INCLUDE_DIRS})
link_libraries(${YARP_LIBRARIES})
include(YarpDevice)

begin_device_library(foo)
add_subdirectory(fooBot)
add_subdirectory(fooDevice)
end_device_library(foo)
