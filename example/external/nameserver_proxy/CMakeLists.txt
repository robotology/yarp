# Copyright: (C) 2012 IITRBCS
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(YARP REQUIRED)

INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
ADD_EXECUTABLE(nameserver_proxy main.cpp)
TARGET_LINK_LIBRARIES(nameserver_proxy ${YARP_LIBRARIES})

