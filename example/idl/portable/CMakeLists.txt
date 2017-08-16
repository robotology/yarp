# Copyright: (C) 2014 Istituto Italiano di Tecnologia (IIT)
# Authors: Lorenzo Natale
# CopyPolicy:  Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

#find YARP
find_package(YARP REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(sender sender.cpp SharedData.cpp)
target_link_libraries(sender ${YARP_LIBRARIES})

add_executable(receiver receiver.cpp SharedData.cpp)
target_link_libraries(receiver ${YARP_LIBRARIES})
