# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(collatzServer)

# Add Yarp to the "packages" used by this project.
find_package(YARP COMPONENTS os REQUIRED)

# Find .cpp and .h automatically. This is a bit lazy,
# and in principle it would be better to list these files
# manually.
file(GLOB folder_source src/*.cpp)
file(GLOB folder_header include/*.h ../common/*.h)

# Create everything needed to build our executable.
add_executable(collatzServer)
target_sources(collatzServer PRIVATE ${folder_source} ${folder_header})
target_include_directories(collatzServer PRIVATE ${PROJECT_SOURCE_DIR}/include
                                                 ${PROJECT_SOURCE_DIR}/../common)
target_link_libraries(collatzServer PRIVATE ${YARP_LIBRARIES})
