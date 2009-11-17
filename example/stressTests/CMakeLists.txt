# Copyright: (C) 2009 RobotCub Consortium
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.4)

if(COMMAND cmake_policy)
 cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT (stressTests)

FIND_PACKAGE(YARP)

ADD_EXECUTABLE(stressrpc stressrpc.cpp)
ADD_EXECUTABLE(stressrpcMD stressrpcMD.cpp)
ADD_EXECUTABLE(callback callback.cpp)
ADD_EXECUTABLE(smallrpc smallrpc.cpp)
ADD_EXECUTABLE(windows1 windows1.cpp)
