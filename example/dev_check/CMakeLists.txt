# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(dev_check)

# make sure YARP_DIR environment variable is set
find_package(YARP REQUIRED)

add_executable(dev_check main.cpp)
target_link_libraries(dev_check ${YARP_LIBRARIES})
