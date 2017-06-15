# Copyright: (C) 2014 iCub Facility
# Authors: Lorenzo Natale
# CopyPolicy:  Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

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
