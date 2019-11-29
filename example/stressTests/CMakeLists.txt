# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
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
