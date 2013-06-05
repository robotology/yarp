# Copyright: (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Test finding specific installed version of YARP

cmake_minimum_required(VERSION 2.8.7)

#find_package(YARP 1 REQUIRED)  # should fail
#find_package(YARP 2.3.3 REQUIRED)  # should succeed for YARP 2.3.3
#find_package(YARP 3 REQUIRED)  # should fail for YARP 2
#find_package(YARP 2.3.1 REQUIRED)  # should succeed for YARP 2.3.[1-]
#find_package(YARP 2.3.5 REQUIRED)  # should succeed for YARP 2.3.[5-]
find_package(YARP 2.3 REQUIRED)

include_directories(${YARP_LIBRARIES})
add_executable(test_yarp main.cpp)
target_link_libraries(test_yarp ${YARP_LIBRARIES})
