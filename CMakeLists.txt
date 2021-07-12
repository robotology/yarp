# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

# UseSWIG generates now standard target names.
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.13)
  cmake_policy(SET CMP0078 OLD)
endif()

# UseSWIG honors SWIG_MODULE_NAME via -module flag.
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.14)
  cmake_policy(SET CMP0086 OLD)
endif()

# Main project
project(YARP
        VERSION 3.4.102
        LANGUAGES C CXX)
set(PROJECT_DESCRIPTION "YARP: A thin middleware for humanoid robots and more")


# Pick up our CMake scripts - they are all in the cmake subdirectory.
set(YARP_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_DIR})

# Find YCM
# This is required here, because YarpVersion uses GitInfo from YCM
set(YCM_REQUIRED_VERSION 0.13.0) # Used also by YarpFindDependencies
find_package(YCM ${YCM_REQUIRED_VERSION} REQUIRED)

# Get the current YARP version.
# See cmake/YarpVersion.cmake.
include(YarpVersion)

set_property(GLOBAL PROPERTY USE_FOLDERS 1)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "Autogen Targets")
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")

# Check system configuration.
# See cmake/YarpSystemCheck.cmake.
include(YarpSystemCheck)

# Set up some compilation options.
# See cmake/YarpOptions.cmake.
include(YarpOptions)

message(STATUS "Using:")
message(STATUS "  CMake: ${CMAKE_VERSION} (${CMAKE_COMMAND})")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION} (${CMAKE_C_COMPILER})")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} (${CMAKE_CXX_COMPILER})")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMake modules directory: ${CMAKE_MODULE_PATH}")

# Check YARP dependencies, find necessary libraries.
# See cmake/YarpFindDependencies.cmake.
include(YarpFindDependencies)

# Make some custom header files containing compile settings.
add_subdirectory(cmake)

# Build required 3rd party libraries
add_subdirectory(extern)


include(YarpInstallationHelpers)
include(YarpDeprecatedOption)

yarp_configure_external_installation(yarp WITH_PLUGINS)


# Set up the main libraries and utilities.
add_subdirectory(src)

# Offer to compile bindings
add_subdirectory(bindings)

# Tests
add_subdirectory(tests)

# Examples
add_subdirectory(example)

# Platform independent data
add_subdirectory(data)

# Documentation
add_subdirectory(doc)

# Output configuration files describing how YARP was built, and how
# its libraries may be linked against and used.  This uses
# information accumulated in the global property variables set up earlier.
# See cmake/YarpDescribe.cmake
include(YarpDescribe)

# Set up some packaging properties (for generating tarballs etc).
# See cmake/YarpPackage.cmake
include(YarpPackage)


# add rule for uninstalling YARP
include(AddUninstallTarget)
