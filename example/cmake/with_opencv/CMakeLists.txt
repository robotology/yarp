# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

# create the program "foo"
add_executable(foo)

# add sources
target_sources(foo PRIVATE main.cpp)

# add the current directory in the include path
target_include_directories(foo PRIVATE ${CMAKE_SOURCE_DIR})


# execute YarpConfig.cmake
find_package(YARP COMPONENTS os REQUIRED)
# link YARP libraries
target_link_libraries(foo PRIVATE YARP::YARP_os
                                   YARP::YARP_init)

# now FindOpenCV.cmake is in CMAKE_MODULE_PATH
find_package(OpenCV REQUIRED)
# add OpenCV include directories
target_include_directories(foo PRIVATE ${OPENCV_INCLUDE_DIR})
# link OpenCV libraries
target_link_libraries(foo PRIVATE ${OPENCV_LIBRARIES}
