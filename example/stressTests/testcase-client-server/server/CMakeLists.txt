# Copyright: (C) 2010 RobotCub Consortium
# Author: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(collatzServer)
cmake_minimum_required(VERSION 3.0)

# Add Yarp to the "packages" used by this project.
find_package(YARP REQUIRED)

# Find .cpp and .h automatically. This is a bit lazy,
# and in principle it would be better to list these files
# manually.
file(GLOB folder_source src/*.cpp)
file(GLOB folder_header include/*.hpp include/*.h ../common/*.hpp ../common/*.h)

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
