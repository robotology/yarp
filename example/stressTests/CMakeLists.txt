# Copyright: (C) 2009 RobotCub Consortium
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(stressTests)

find_package(YARP REQUIRED)

add_executable(stressrpc stressrpc.cpp)
target_link_libraries(stressrpc ${YARP_LIBRARIES})

add_executable(stressrpcMD stressrpcMD.cpp)
target_link_libraries(stressrpcMD ${YARP_LIBRARIES})

add_executable(callback callback.cpp)
target_link_libraries(callback ${YARP_LIBRARIES})

add_executable(smallrpc smallrpc.cpp)
target_link_libraries(smallrpc ${YARP_LIBRARIES})

add_executable(windows1 windows1.cpp)
target_link_libraries(windows1 ${YARP_LIBRARIES})
