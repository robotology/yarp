# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(collatzServer)
cmake_minimum_required(VERSION 3.12)

# Add Yarp to the "packages" used by this project.
find_package(YARP REQUIRED)

# Find .cpp and .h automatically. This is a bit lazy,
# and in principle it would be better to list these files
# manually.
file(GLOB folder_source src/*.cpp)
file(GLOB folder_header include/*.h ../common/*.h)

# Declare groups of source and header, makes tings pretty in VisualStudio.
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

# Create everything needed to build our executable.
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/../common)
add_executable(collatzServer ${folder_source} ${folder_header})
target_link_libraries(collatzServer ${YARP_LIBRARIES})

# Make VisualStudio a bit quieter with warnings.
if(MSVC)
  set_target_properties(collatzServer PROPERTIES COMPILE_FLAGS /wd4996)
endif()
