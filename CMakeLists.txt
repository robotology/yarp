# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.6)

project(YARP)

# Pick up our scripts - they are all in the conf subdirectory
set(YARP_MODULE_PATH ${PROJECT_SOURCE_DIR}/conf)
set(CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

# Set up some large-scale compile options - do you want devices? guis?
include(YarpOptions)

# Get the current YARP version.
include(YarpVersion)

# Check system types, find necessary libraries.
include(YarpSystemCheck)

# Turn on testing
enable_testing()

# The YARP build is fairly flexible, so we need to accumulate some
# information about what libraries are being built (and their dependencies),
# along with the include path.
set_property(GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
set_property(GLOBAL PROPERTY YARP_LIBS)
set_property(GLOBAL PROPERTY YARP_DEFS)

# build some configuration header files
add_subdirectory(conf)

# build the main libraries and utilities
add_subdirectory(src)

# Now, let's see what we built, and record it to facilitate in-tree 
# (uninstalled) use of YARP.
get_property(YARP_INCLUDES GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
get_property(YARP_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
get_property(YARP_DEFINES GLOBAL PROPERTY YARP_DEFS)
message(STATUS "In-tree includes: ${YARP_INCLUDES}")
message(STATUS "YARP libraries: ${YARP_LIBRARIES}")

configure_file(${CMAKE_SOURCE_DIR}/conf/template/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfig.cmake @ONLY IMMEDIATE)
export(TARGETS ${YARP_LIBRARIES} 
	       FILE ${CMAKE_BINARY_DIR}/YARPDependencies.cmake)

# Set up a configuration file for installed use of YARP
install(EXPORT YARP DESTINATION lib/YARP)

# Set up some packaging properties (for generating tarballs etc)
include(YarpPackage)

# Make documentation target, documentation goes in doc subdirectory
# of build.
find_program(DOXYGEN_EXE NAMES doxygen)
mark_as_advanced(DOXYGEN_EXE)
if (DOXYGEN_EXE)
  configure_file(${CMAKE_SOURCE_DIR}/conf/doxygen/Doxyfile.part.template
                 ${CMAKE_BINARY_DIR}/dox/Doxyfile.part IMMEDIATE)
  add_custom_target(dox COMMAND ${DOXYGEN_EXE} ${CMAKE_BINARY_DIR}/dox/Doxyfile.part)
endif ()

# Modify this file when you add/remove files in directories that
# are scanned for sources.  This will make sure that cmake gets rerun
# for everybody.
# Touch: 1 2 3
