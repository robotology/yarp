# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(stressTests)

find_package(YARP COMPONENTS os REQUIRED)

add_executable(stressrpc)
target_sources(stressrpc PRIVATE stressrpc.cpp)
target_link_libraries(stressrpc ${YARP_LIBRARIES})

add_executable(stressrpcMD)
target_sources(stressrpcMD PRIVATE stressrpcMD.cpp)
target_link_libraries(stressrpcMD ${YARP_LIBRARIES})

add_executable(callback)
target_sources(callback PRIVATE callback.cpp)
target_link_libraries(callback ${YARP_LIBRARIES})

add_executable(smallrpc)
target_sources(smallrpc PRIVATE smallrpc.cpp)
target_link_libraries(smallrpc ${YARP_LIBRARIES})

add_executable(windows1)
target_sources(windows1 PRIVATE windows1.cpp)
target_link_libraries(windows1 ${YARP_LIBRARIES})
