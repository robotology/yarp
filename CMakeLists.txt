# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.5)

if(NOT ${CMAKE_VERSION} VERSION_LESS 3.4)
  cmake_policy(SET CMP0064 NEW)
endif()

if(NOT ${CMAKE_VERSION} VERSION_LESS 3.8)
  cmake_policy(SET CMP0067 NEW)
endif()

# RPATH cmake policy set to NEW
if(NOT ${CMAKE_VERSION} VERSION_LESS 3.9)
  cmake_policy(SET CMP0068 NEW)
endif()

# Run automoc on generated files
if(NOT ${CMAKE_VERSION} VERSION_LESS 3.10)
  cmake_policy(SET CMP0071 NEW)
endif()

# Pick up our CMake scripts - they are all in the cmake subdirectory.
set(YARP_MODULE_DIR "${CMAKE_SOURCE_DIR}/cmake")
set(YARP_MODULE_PATH "${YARP_MODULE_DIR}")

set(YCM_REQUIRED_VERSION 0.8.0) # Used also by YarpFindDependencies
find_package(YCM ${YCM_REQUIRED_VERSION} QUIET)
if(NOT YCM_FOUND)
  list(APPEND YARP_MODULE_PATH "${YARP_MODULE_DIR}/ycm-${YCM_REQUIRED_VERSION}/find-modules"
                               "${YARP_MODULE_DIR}/ycm-${YCM_REQUIRED_VERSION}/modules"
                               "${YARP_MODULE_DIR}/ycm-${YCM_REQUIRED_VERSION}/3rdparty")
  if(${CMAKE_VERSION} VERSION_LESS 3.8)
    list(APPEND YARP_MODULE_PATH "${YARP_MODULE_DIR}/ycm-${YCM_REQUIRED_VERSION}/cmake-3.8/Modules")
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

# Main project
project(YARP
        VERSION 3.0.0
        LANGUAGES C CXX)
set(PROJECT_DESCRIPTION "YARP: A thin middleware for humanoid robots and more")


# Get the current YARP version.
# See cmake/YarpVersion.cmake.
include(YarpVersion)

message(STATUS "Using:")
message(STATUS "  CMake: ${CMAKE_VERSION} (${CMAKE_COMMAND})")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION} (${CMAKE_C_COMPILER})")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} (${CMAKE_CXX_COMPILER})")

set_property(GLOBAL PROPERTY USE_FOLDERS 1)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "Autogen Targets")
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")

# Check system configuration.
# See cmake/YarpSystemCheck.cmake.
include(YarpSystemCheck)

# Set up some compilation options.
# See cmake/YarpOptions.cmake.
include(YarpOptions)

# Check YARP dependencies, find necessary libraries.
# See cmake/YarpFindDependencies.cmake.
include(YarpFindDependencies)

include(YarpSourceGroup)

# Make some custom header files containing compile settings.
add_subdirectory(cmake)

# Build required 3rd party libraries
add_subdirectory(extern)

# Set up the main libraries and utilities.
add_subdirectory(src)

# Offer to compile bindings
add_subdirectory(bindings)

# Tests
add_subdirectory(tests)

# Platform independent data
add_subdirectory(data)

# Output configuration files describing how YARP was built, and how
# its libraries may be linked against and used.  This uses
# information accumulated in the global property variables set up earlier.
# See cmake/YarpDescribe.cmake
include(YarpDescribe)

# Set up some packaging properties (for generating tarballs etc).
# See cmake/YarpPackage.cmake
include(YarpPackage)

# Make documentation target, documentation goes in doc subdirectory
# of build.  See cmake/YarpDoc.cmake
include(YarpDoc)

# add rule for uninstalling YARP
include(AddUninstallTarget)
