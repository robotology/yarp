# Copyright: (C) 2010 RobotCub Consortium
# Author: Lorenzo Natale, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

# execute YarpConfig.cmake
find_package(YARP)

# pick up YARP find scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})

# now FindOpenCV.cmake is in CMAKE_MODULE_PATH
find_package(OpenCV)

# searches for all .cpp and .h files in the current directory and add them
# to the current project
file(GLOB folder_source *.cpp)
file(GLOB folder_header *.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

# add the current directory in the include path
include_directories(${CMAKE_SOURCE_DIR})
# add YARP include directories
include_directories(${YARP_INCLUDE_DIRS})
# add OpenCV include directories
include_directories(${OPENCV_INCLUDE_DIR})

# create the program "foo"
add_executable(foo ${folder_source} ${folder_header})

# link opencv and YARP libraries
target_link_libraries(foo ${OPENCV_LIBRARIES} ${YARP_LIBRARIES})
