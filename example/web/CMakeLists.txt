# Copyright: (C) 2012 IITRBCS
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

cmake_minimum_required(VERSION 3.0)

find_package(YARP REQUIRED)

add_executable(web_test web_test.cpp)
target_link_libraries(web_test ${YARP_LIBRARIES})
