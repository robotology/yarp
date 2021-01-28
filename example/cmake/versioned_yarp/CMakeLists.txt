# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# Test finding specific installed version of YARP

cmake_minimum_required(VERSION 3.12)

#find_package(YARP 2 REQUIRED)  # should fail
#find_package(YARP 3.3.3 REQUIRED)  # should succeed for YARP 3.3.3
#find_package(YARP 4 REQUIRED)  # should fail for YARP 3
#find_package(YARP 3.2.1 REQUIRED)  # should succeed for YARP 3.2.[1-]
#find_package(YARP 3.2.5 REQUIRED)  # should succeed for YARP 3.2.[5-]
find_package(YARP 3.3 COMPONENTS os REQUIRED)

add_executable(test_yarp)
target_sources(test_yarp PRIVATE main.cpp)
target_link_libraries(test_yarp PRIVATE YARP::YARP_os
                                        YARP::YARP_init)
