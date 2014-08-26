# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

PROJECT (Carrier)

# replace "..." with the path to your YARP binaries if needed
#SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
ADD_DEFINITIONS(${YARP_DEFINES})

ADD_EXECUTABLE(carrier_stub carrier_stub.cpp)
TARGET_LINK_LIBRARIES(carrier_stub ${YARP_LIBRARIES})

ADD_EXECUTABLE(carrier_human carrier_human.cpp)
TARGET_LINK_LIBRARIES(carrier_human ${YARP_LIBRARIES})
