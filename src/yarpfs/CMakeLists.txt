# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)
project(yarpfs)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Find YARP
find_package(YARP REQUIRED)
include_directories(${YARP_INCLUDE_DIRS})
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

find_package(ACE REQUIRED)
add_definitions(${ACE_DEFINITIONS})
include_directories(${ACE_INCLUDE_DIRS})

# Find Fuse
find_package(Fuse REQUIRED)
add_definitions(${Fuse_DEFINITIONS})
include_directories(${Fuse_INCLUDE_DIRS})

set(yarpfs_SRCS yarpfs.cpp
                link.cpp)
set(yarpfs_HDRS yarpfns.h
                yarputils.h)

add_executable(yarpfs ${yarpfs_SRCS} ${yarpfs_HDRS})
target_link_libraries(yarpfs YARP::YARP_OS
                             YARP::YARP_init
                             ${ACE_LIBRARIES}
                             ${Fuse_LIBRARIES})
