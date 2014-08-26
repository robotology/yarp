# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

# make sure YARP_DIR environment variable is set
FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

PROJECT(dev_check)

ADD_EXECUTABLE(dev_check main.cpp)
TARGET_LINK_LIBRARIES(dev_check ${YARP_LIBRARIES})
