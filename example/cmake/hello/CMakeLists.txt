# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# YARP needs CMake 3.0 or greater
cmake_minimum_required(VERSION 3.0)

# find YARP
find_package(YARP REQUIRED)

# set up our program
add_executable(hello hello.cpp)

# link with YARP libraries
target_link_libraries(hello ${YARP_LIBRARIES})
