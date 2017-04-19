# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

# Pick up our CMake scripts - they are all in the cmake subdirectory.
set(YARP_MODULE_DIR "${CMAKE_SOURCE_DIR}/cmake")
set(YARP_MODULE_PATH "${YARP_MODULE_DIR}")

set(YCM_REQUIRED_VERSION 0.3.1) # Used also by YarpFindDependencies
find_package(YCM ${YCM_REQUIRED_VERSION} QUIET)
if(NOT YCM_FOUND)
  list(APPEND YARP_MODULE_PATH "${YARP_MODULE_DIR}/ycm-0.3.1/find-modules"
                               "${YARP_MODULE_DIR}/ycm-0.3.1/modules"
                               "${YARP_MODULE_DIR}/ycm-0.3.1/3rdparty"
                               "${YARP_MODULE_DIR}/ycm-0.3.1/cmake-next")
  if(${CMAKE_VERSION} VERSION_LESS 3.1)
    list(APPEND YARP_MODULE_PATH "${YARP_MODULE_DIR}/ycm-0.3.1/cmake-3.1/Modules")
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

# Get the current YARP version.
# See cmake/YarpVersion.cmake.
include(YarpVersion)

# Main project
project(YARP
        VERSION ${YARP_VERSION_SHORT}
        LANGUAGES C CXX)

message(STATUS "Using:")
message(STATUS "  CMake: ${CMAKE_VERSION} (${CMAKE_COMMAND})")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION} (${CMAKE_C_COMPILER})")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} (${CMAKE_CXX_COMPILER})")

# The YARP build is fairly flexible, so we need to accumulate some
# information about what libraries are being built (and their dependencies),
# along with the include path.  We'll be passing that along to clients
# using YARP without a global install.
set_property(GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
set_property(GLOBAL PROPERTY YARP_LIBS)
set_property(GLOBAL PROPERTY YARP_DEFS)


# Check system configuration.
# See cmake/YarpSystemCheck.cmake.
include(YarpSystemCheck)

# Set up some compilation options.
# See cmake/YarpOptions.cmake.
include(YarpOptions)

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

# add rule for uninstalling YARP
include(AddUninstallTarget)
