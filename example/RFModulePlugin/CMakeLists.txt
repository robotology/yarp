################################################################################
#                                                                              #
# Copyright (C) 2017 Fondazione Istituto Italiano di Tecnologia (IIT)          #
# All Rights Reserved.                                                         #
#                                                                              #
################################################################################

cmake_minimum_required(VERSION 3.5)
project(myModules)

# Enable C++11
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

# Must be set before find_package(YARP)
set(BUILD_SHARED_LIBS ON)

find_package(YARP REQUIRED)

yarp_begin_plugin_library(myModules)

add_subdirectory(testmoduleFoo)
add_subdirectory(testmoduleBar)

yarp_end_plugin_library(myModules)

add_subdirectory(moduleLoader)
