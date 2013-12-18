# Copyright: 2012 iCub Facility, Istituto Italiano di Tecnologia
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(PROJECTNAME resourceFinder)
PROJECT(${PROJECTNAME})

FIND_PACKAGE(YARP)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})

include_directories(${YARP_INCLUDE_DIRS})

add_executable(rf_basic tutorial_rf_basic.cpp)
target_link_libraries(rf_basic ${YARP_LIBRARIES})

add_executable(rf_advanced tutorial_rf_advanced.cpp)
target_link_libraries(rf_advanced ${YARP_LIBRARIES})


