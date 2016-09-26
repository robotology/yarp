# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

# CMake Policy CMP0020 (Automatically link Qt executables to qtmain
# target on Windows). Can be removed with
# CMAKE_MINIMUM_REQUIRED_VERSION 2.8.11 or later.
if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS 2.8.11)
  message(AUTHOR_WARNING "CMake Policy CMP0020 is now NEW by default. You can remove this.")
endif()
if(NOT ${CMAKE_VERSION} VERSION_LESS 2.8.11)
  cmake_policy(SET CMP0020 NEW)
endif()

project(YARP)

# Pick up our CMake scripts - they are all in the cmake subdirectory.
set(YARP_MODULE_DIR ${PROJECT_SOURCE_DIR}/cmake)
set(YARP_MODULE_PATH ${YARP_MODULE_DIR})
if(${CMAKE_VERSION} VERSION_LESS 3.5)
  list(APPEND YARP_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/cmake-3.5/Modules)
endif()
if(${CMAKE_VERSION} VERSION_LESS 3.1)
  list(APPEND YARP_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/cmake-3.1/Modules)
endif()
set(CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

# The YARP build is fairly flexible, so we need to accumulate some
# information about what libraries are being built (and their dependencies),
# along with the include path.  We'll be passing that along to clients
# using YARP without a global install.
set_property(GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
set_property(GLOBAL PROPERTY YARP_LIBS)
set_property(GLOBAL PROPERTY YARP_DEFS)

# Get the current YARP version.
# See cmake/YarpVersion.cmake.
include(YarpVersion)

# Check system configuration.
# See cmake/YarpSystemCheck.cmake.
include(YarpSystemCheck)

# Set up some compilation options.
# See cmake/YarpOptions.cmake.
include(YarpOptions)

# Include some workarounds for older cmake versions
# See cmake/YarpCMakeWorkarounds.cmake
include(YarpCMakeWorkarounds)

# Check YARP dependencies, find necessary libraries.
# See cmake/YarpFindDependencies.cmake.
include(YarpFindDependencies)

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

# add rule for uninstalling YARP
# see cmake/YARPUninstall.cmake
include(YARPUninstall)

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

# Make sure yarp.i and related source files get installed, to allow
# bindings for other languages to be compiled from the build material.
include(YarpBindingsSource)
