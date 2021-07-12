# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(myModules)

# Enable C++14
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Must be set before find_package(YARP)
set(BUILD_SHARED_LIBS ON)

find_package(YARP REQUIRED)

yarp_begin_plugin_library(myModules)

add_subdirectory(testmoduleFoo)
add_subdirectory(testmoduleBar)

yarp_end_plugin_library(myModules)

add_subdirectory(moduleLoader)
