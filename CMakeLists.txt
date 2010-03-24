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

# Get the current yarp version.  Really this should be placed into
# a generated header file.
include(YarpVersion)

# Configure where the output will go
include(YarpOutput)

# get appropriate libraries and settings
include(YarpReqLib)

# define useful macros for sub-libraries and programs
#INCLUDE(YarpLibrary)
#INCLUDE(YarpProgram)

# turn on testing
enable_testing()

set_property(GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
set_property(GLOBAL PROPERTY YARP_TREE_LIBRARIES)

# declare active subdirectories
add_subdirectory(conf)
ADD_SUBDIRECTORY(src)

get_property(YARP_INCLUDES GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
message(STATUS "In-tree includes: ${YARP_INCLUDES}")

get_property(YARP_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
message(STATUS "YARP libraries: ${YARP_LIBRARIES}")

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/conf/template/YARPConfig.cmake.in
               ${CMAKE_BINARY_DIR}/YARPConfig.cmake @ONLY IMMEDIATE)

# set up some packaging properties
include(YarpPackage)

# make doc target - we could check whether doxygen is present, but
# let's not make a big deal of it for now.
set(YARP_ADMIN "$ENV{YARP_ADMIN}")
if (YARP_ADMIN)
  configure_file(${CMAKE_SOURCE_DIR}/conf/doxygen/Doxyfile.part.template
                 ${CMAKE_BINARY_DIR}/dox/Doxyfile.part IMMEDIATE)
  add_custom_target(dox COMMAND doxygen ${CMAKE_BINARY_DIR}/dox/Doxyfile.part)
endif ()

INSTALL(EXPORT YARP DESTINATION lib/YARP)

# Modify this file when you add/remove files in directories that
# are scanned for sources.  This will make sure that cmake gets rerun
# for everybody.
# Touch: 1 2 3

