# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# YARP needs CMake 3.0 or greater
cmake_minimum_required(VERSION 3.12)

# find YARP
find_package(YARP COMPONENTS os REQUIRED)

# set up our program
add_executable(hello)

# declare our source files
target_sources(hello PRIVATE hello.cpp)

# link with YARP libraries
target_link_libraries(hello PRIVATE YARP::YARP_os
                                    YARP::YARP_init)
