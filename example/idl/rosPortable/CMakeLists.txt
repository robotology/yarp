# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

find_package(YARP REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(ALLOW_IDL_GENERATION TRUE)
set(generated_libs_dir ${CMAKE_CURRENT_BINARY_DIR})
yarp_idl_to_dir(SharedData.msg ${generated_libs_dir})
include_directories(${generated_libs_dir}/include)

add_executable(sender sender.cpp)
target_link_libraries(sender ${YARP_LIBRARIES})

add_executable(receiver receiver.cpp)
target_link_libraries(receiver ${YARP_LIBRARIES})

add_executable(sender2 sender2.cpp)
target_link_libraries(sender2 ${YARP_LIBRARIES})

add_executable(receiver2 receiver2.cpp)
target_link_libraries(receiver2 ${YARP_LIBRARIES})
