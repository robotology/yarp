# Copyright: (C) 2014 iCub Facility
# Authors: Lorenzo Natale
# CopyPolicy:  Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
set(ALLOW_IDL_GENERATION TRUE)
include(YarpIDL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${YARP_INCLUDE_DIRS})

set(generated_libs_dir ${CMAKE_CURRENT_BINARY_DIR})
yarp_idl_to_dir(SharedData.msg ${generated_libs_dir})
include_directories(${generated_libs_dir}/include ${YARP_INCLUDE_DIRS})

add_executable(sender sender.cpp)
TARGET_LINK_LIBRARIES(sender ${YARP_LIBRARIES})

add_executable(receiver receiver.cpp)
TARGET_LINK_LIBRARIES(receiver ${YARP_LIBRARIES})

add_executable(sender2 sender2.cpp)
TARGET_LINK_LIBRARIES(sender2 ${YARP_LIBRARIES})

add_executable(receiver2 receiver2.cpp)
TARGET_LINK_LIBRARIES(receiver2 ${YARP_LIBRARIES})


