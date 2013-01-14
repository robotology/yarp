# Copyright: (C) 2012 IITRBCS
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

cmake_minimum_required(VERSION 2.6.4)

find_package(YARP REQUIRED)

include_directories(${YARP_INCLUDE_DIRS})
add_executable(nameserver_proxy main.cpp)
target_link_libraries(nameserver_proxy ${YARP_LIBRARIES})

