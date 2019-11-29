# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# Test finding specific installed version of YARP

cmake_minimum_required(VERSION 3.12)

#find_package(YARP 1 REQUIRED)  # should fail
#find_package(YARP 2.3.3 REQUIRED)  # should succeed for YARP 2.3.3
#find_package(YARP 3 REQUIRED)  # should fail for YARP 2
#find_package(YARP 2.3.1 REQUIRED)  # should succeed for YARP 2.3.[1-]
#find_package(YARP 2.3.5 REQUIRED)  # should succeed for YARP 2.3.[5-]
find_package(YARP 2.3 REQUIRED)

include_directories(${YARP_LIBRARIES})
add_executable(test_yarp main.cpp)
target_link_libraries(test_yarp ${YARP_LIBRARIES})
