# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.6)

project(YARP)

# Pick up our scripts - they are all in the conf subdirectory
set(YARP_MODULE_PATH ${PROJECT_SOURCE_DIR}/conf)
set(CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

# The YARP build is fairly flexible, so we need to accumulate some
# information about what libraries are being built (and their dependencies),
# along with the include path.  We'll be passing that along to clients
# using YARP without a global install.
set_property(GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
set_property(GLOBAL PROPERTY YARP_LIBS)
set_property(GLOBAL PROPERTY YARP_DEFS)

# Set up some compilation options
include(YarpOptions)

# Get the current YARP version
include(YarpVersion)

# Check system types, find necessary libraries
include(YarpSystemCheck)

# Turn on testing
enable_testing()

# build some configuration header files
add_subdirectory(conf)

# build the main libraries and utilities
add_subdirectory(src)

# Output configuration files describing how YARP was built, and how
# its libraries may be linked against and used.
include(YarpDescribe)

# Now, let's see what we built, and record it to facilitate in-tree 
# (uninstalled) use of YARP.
get_property(YARP_INCLUDES GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
get_property(YARP_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
get_property(YARP_DEFINES GLOBAL PROPERTY YARP_DEFS)
get_property(YARP_HAS_MATH_LIB GLOBAL PROPERTY YARP_HAS_MATH_LIB)
message(STATUS "In-tree includes: ${YARP_INCLUDES}")
message(STATUS "YARP libraries: ${YARP_LIBRARIES}")

set(YARP_DEPENDENCY_FILE ${CMAKE_BINARY_DIR}/YARPDependencies.cmake)
configure_file(${CMAKE_SOURCE_DIR}/conf/template/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfig.cmake @ONLY IMMEDIATE)
export(TARGETS ${YARP_LIBRARIES} FILE ${YARP_DEPENDENCY_FILE})

# Set up a configuration file for installed use of YARP
set(YARP_DEPENDENCY_FILE ${CMAKE_INSTALL_PREFIX}/lib/YARP/YARP.cmake)
set(YARP_INCLUDES ${CMAKE_INSTALL_PREFIX}/include)
set(YARP_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/share/yarp/cmake)
configure_file(${CMAKE_SOURCE_DIR}/conf/template/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfigForInstall.cmake @ONLY IMMEDIATE)
install(FILES ${CMAKE_BINARY_DIR}/YARPConfigForInstall.cmake RENAME YARPConfig.cmake COMPONENT configuration DESTINATION lib/YARP)
install(EXPORT YARP COMPONENT configuration DESTINATION lib/YARP)

# Set up some packaging properties (for generating tarballs etc)
include(YarpPackage)

# Make documentation target, documentation goes in doc subdirectory
# of build.
include(YarpDoc)

# Modify this file when you add/remove files in directories that
# are scanned for sources.  This will make sure that cmake gets rerun
# for everybody.
# Touch: 1 2 3
