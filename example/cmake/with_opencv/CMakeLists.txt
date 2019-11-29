# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

# execute YarpConfig.cmake
find_package(YARP)

# now FindOpenCV.cmake is in CMAKE_MODULE_PATH
find_package(OpenCV)

# searches for all .cpp and .h files in the current directory
file(GLOB folder_source *.cpp)
file(GLOB folder_header *.h)

# add the current directory in the include path
include_directories(${CMAKE_SOURCE_DIR})

# add OpenCV include directories
include_directories(${OPENCV_INCLUDE_DIR})

# create the program "foo"
add_executable(foo ${folder_source} ${folder_header})

# link opencv and YARP libraries
target_link_libraries(foo ${OPENCV_LIBRARIES} ${YARP_LIBRARIES})
