# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
cmake_minimum_required(VERSION 2.8.7)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})
SET(CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
INCLUDE(YarpDevice)

BEGIN_DEVICE_LIBRARY(baz)
ADD_SUBDIRECTORY(bazBot)
ADD_SUBDIRECTORY(bazDevice)
END_DEVICE_LIBRARY(baz)
