# Copyright: (C) 2014 iCub Facility
# Authors: Lorenzo Natale
# CopyPolicy:  Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

#find YARP
find_package(YARP REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${YARP_INCLUDE_DIRS})

add_executable(sender sender.cpp)
TARGET_LINK_LIBRARIES(sender ${YARP_LIBRARIES})

add_executable(receiver receiver.cpp)
TARGET_LINK_LIBRARIES(receiver ${YARP_LIBRARIES})


