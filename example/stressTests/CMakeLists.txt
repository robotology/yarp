# Copyright: (C) 2009 RobotCub Consortium
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

PROJECT (stressTests)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

ADD_EXECUTABLE(stressrpc stressrpc.cpp)
ADD_EXECUTABLE(stressrpcMD stressrpcMD.cpp)
ADD_EXECUTABLE(callback callback.cpp)
ADD_EXECUTABLE(smallrpc smallrpc.cpp)
ADD_EXECUTABLE(windows1 windows1.cpp)
