# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(collatzServer)

# Add YARP to the "packages" used by this project.
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
